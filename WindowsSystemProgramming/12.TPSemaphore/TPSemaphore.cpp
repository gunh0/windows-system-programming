#include "stdafx.h"
#include "Windows.h"
#include "string"
#include "list"
#include "iostream"

using namespace std;

HANDLE g_hExit = NULL;	// 스레드 종료를 위한 뮤텍스

typedef std::list<std::string> TP_QUE;
struct THREAD_POOL
{
	HANDLE	m_hMutx;	// enqueue/dequeue 동기화를 위한 뮤텍스
	HANDLE	m_hSema;	// 스레드 풀 구현을 위한 세마포어
	TP_QUE	m_queue;	// 큐 기능을 담당할 STL 리스트
};
typedef THREAD_POOL* PTHREAD_POOL;

// 스레드는 기본적으로 종료 시그널을 위한 뮤텍스와 큐 관리를 위한 세마포어 두 개에 대해 무한 루프 내에서 대기 상태로 존재하게 된다.
DWORD WINAPI PoolItemProc(LPVOID pParam)
{
	PTHREAD_POOL pTQ = (PTHREAD_POOL)pParam;

	DWORD dwThId = GetCurrentThreadId();
	HANDLE arhObjs[2] = { g_hExit, pTQ->m_hSema };
	while (true)
	{
		DWORD dwWaitCode = WaitForMultipleObjects(2, arhObjs, FALSE, INFINITE);
		/* 처음에 모든 스레드는 세마포어가 넌시그널 상태기 때문에 대기 상태로 들어간다.
		그러다 요청 수신 스레드(여기서는 메인 스레드)에서 요청이 들어왔을 때 ReleaseSemaphore를 통해서 자원계수를 1 증가시켜 주면
		대기 상태에서 풀려 다음 코드를 실행하게 된다.
		물론 WaitForMultipleObjects의 부가적 효과 때문에 대기 상태에서 풀릴 때는 이미 세마포어의 자원계수는 1 감소되어 0이 되어 있을 것이다.
		*/
		if (dwWaitCode == WAIT_FAILED)
		{
			cout << " ~~~ WaitForSingleObject failed : " << GetLastError() << endl;
			break;
		}
		
		if (dwWaitCode == WAIT_OBJECT_0)
			break;	// 뮤텍스가 시그널 상태가 되면 스레드 종료를 의미하기 때문에 루프를 탈출한다.

		std::string item;
		WaitForSingleObject(pTQ->m_hMutx, INFINITE);
		TP_QUE::iterator it = pTQ->m_queue.begin();
		item = *it;
		pTQ->m_queue.pop_front();
		ReleaseMutex(pTQ->m_hMutx);
		// 큐에서 작업 항목을 dequeue한다. 뮤텍스를 사용해 dequeue할 동안 큐에 대한 동시 접근을 막는다.

		printf(" => BEGIN | Thread %d works : %s\n", dwThId, item.c_str());
		Sleep(((rand() % 3) + 1) * 1000);
		printf(" <= END   | Thread %d works : %s\n", dwThId, item.c_str());
		// 큐로부터 작업 항목을 획득하면 이에 대한 처리를 하고, 다시 루프로 돌아가 대기 상태로 들어간다.
	}

	ReleaseMutex(g_hExit);	// 뮤텍스를 종료 시그널로 사용하기 때문에 종료하기 전 종료 통지용 뮤텍스의 소유권을 놓아준다.

	printf("......PoolItemProc Thread %d Exit!!!\n", dwThId);

	return 0;
}

void _tmain()
{
	cout << "======= Start TPSemaphore Test ========" << endl;

	g_hExit = CreateMutex(NULL, TRUE, NULL);	// 스레드 종료를 위해서 뮤텍스를 생성한다.
	// 보통 수동 리셋 이벤트로 처리하지만, 뮤텍스를 이용해 종료 통지를 한다.

	THREAD_POOL tp;
	tp.m_hSema = CreateSemaphore(NULL, 0, INT_MAX, NULL);	// 세마포어를 생성한다.
	/* 사용 가능한 초기 지원계수를 0으로 넘기면 이 세마포어는 넌시그널 상태로 생성되므로
	이 세마포어를 사용하고자하는 스레드들은 모두 대기 상태로 들어가게 된다.
	여기서는 사용 가능한 최대 자원 수를 INT_MAX로 지정했지만,
	큐가 무한정 커지는 것을 막고자 한다면 일정 한계치의 값을 넘겨주면 된다.
	*/

	tp.m_hMutx = CreateMutex(NULL, FALSE, NULL);	// enqueue/dequeue 동기화를 위한 뮤텍스를 생성한다.

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	// 풀에 들어갈 스레드 수를 결정하기 위해 시스템 CPU의 개수를 획득한다.
	// 스레드 풀에서 대기할 스레드 수를 CPU의 개수만큼 생성할 것이다.

	PHANDLE parPoolThs = new HANDLE[si.dwNumberOfProcessors];
	for (int i = 0; i < (int)si.dwNumberOfProcessors; i++)
	{
		DWORD dwThreadId;
		parPoolThs[i] = CreateThread(NULL, 0, PoolItemProc, &tp, 0, &dwThreadId);
		// CPU 개수만큼 스레드를 생성한다.
		// 생성된 스레드들은 세마포어가 넌시그널 상태이므로 모두 대기 상태로 존재하게 된다.
	}
	cout << "*** MAIN -> ThreadPool Started, count=" << si.dwNumberOfProcessors << endl;

	char szIn[1024];
	while (true)
	{
		cin >> szIn;
		if (_stricmp(szIn, "quit") == 0)
			break;
		// 유저가 메인 콘솔에서 데이터를 입력하면 그 데이터를 대기 스레드가 처리하도록 맡긴다.
		// 이 과정은 클라이이언트가 서비스 요청을 해 왔음을 시뮬레이션한 것이다.
		// 이 부분을 만약 소켓으로 사용한다면 클라이언트에 대해 리슨을 하고 있는 것으로 대체할 수 있을 것이다.

		WaitForSingleObject(tp.m_hMutx, INFINITE);
		tp.m_queue.push_back(szIn);
		ReleaseMutex(tp.m_hMutx);
		// 문자열을 큐에 enqueue한다.
		// 뮤텍스 객체를 이용해 큐에 대한 독점권을 획등한 후 작업 항목을 큐에 추가한다.

		ReleaseSemaphore(tp.m_hSema, 1, NULL);
		/* 대기 중인 스레드에게 작업 항목이 추가되었음을 알리는 방식이다.
		이 시점에서 CPU 수만큼의 스레드들은 전부 m_hSema를 대상으로 대기하고 있다.
		현재 자원계수가 0이므로 요청에 대해 ReleaseSemaphore를 호출해 1만큼 자원 수를 증가시켜줌으로써
		대기 스레드 중 임의의 하나를 활성화시켜 요청을 처리하도록 한다.
		ReleaseSemaphore를 호출하는 스레드는 메인 스레드이다.
		또한 스레드를 CPU 수만큼 생성시켜 주었기 때문에 작업이 계속 추가되더라도 항상 CPU 수만큼의 스레드만이 활성화된다.
		*/
	}

	ReleaseMutex(g_hExit);	// 종료를 위해 종료 통지 뮤텍스를 시그널한다.
	WaitForMultipleObjects(si.dwNumberOfProcessors, parPoolThs, TRUE, INFINITE);
	for (int i = 0; i < (int)si.dwNumberOfProcessors; i++)
		CloseHandle(parPoolThs[i]);
	delete[] parPoolThs;

	CloseHandle(tp.m_hSema);
	CloseHandle(tp.m_hMutx);
	CloseHandle(g_hExit);

	cout << "======= End TPSemaphore Test ==========" << endl;
}
