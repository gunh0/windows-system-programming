// 공유자원의 관점에서 세마포어를 바라본 예시

#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;


#define MAX_RES_CNT		3	// 자원의 최대 개수를 3으로 정한다.

bool	g_bExit = false;	// 스레드 종료를 위한 플래그를 정의한다.
BOOL	g_abUsedFlag [MAX_RES_CNT];
INT		g_anSharedRes[MAX_RES_CNT] = { 5 , 3, 7 };
// 자원을 나타내는 배열로 배열의 원소 개수는 3개다.
HANDLE	g_hShareLock;	// 가용 자원 검색을 위한 뮤텍스 핸들이다.

DWORD WINAPI SemaphoreProc(LPVOID pParam)
{
	HANDLE hSemaphore = (HANDLE)pParam;
	DWORD  dwThreadId = GetCurrentThreadId();

	while (!g_bExit)
	{
		DWORD dwWaitCode = WaitForSingleObject(hSemaphore, INFINITE);
		// 세마포어에 대해 대기 상태로 들어간다.

		if (dwWaitCode == WAIT_FAILED)
		{
			cout << " ~~~ WaitForSingleObject failed : " << GetLastError() << endl;
			break;
		}
		// dwWaitCode가 에러가 아닌 경우는 세마포어가 시그널 상태가 되었음을 의미한다.
		// 따라서 공유자원을 획득할 수 있는 상태가 되고, 그것에 대한 작업을 수행한다.

		int nSharedIdx = 0;
		WaitForSingleObject(g_hShareLock, INFINITE);
		for (; nSharedIdx < MAX_RES_CNT; nSharedIdx++)
		{
			if (!g_abUsedFlag[nSharedIdx])
				break;
		}
		g_abUsedFlag[nSharedIdx] = TRUE;
		ReleaseMutex(g_hShareLock);
		/* 세마포어가 시그널 상태가 되었다는 말은 가용 자원이 있다는 것만을 의미하기 때문에
		그 자원에 대한 식별 수단은 따로 제공되지 않는다.
		따라서 사용 가능한 자원을 찾기 위해 루프를 돌면서 그 자원을 찾는 과정이 필요하다.
		동시에 여러 스레드가 이 루프를 수행할 수 있기 때문에
		가용 자원 검색 시의 동기화 역시 필요하므로 뮤텍스를 통해 동기화를 수행한다.
		*/

		cout << " ==> Thread " << dwThreadId <<
			" waits " << g_anSharedRes[nSharedIdx] << " seconds..." << endl;
		Sleep((DWORD)g_anSharedRes[nSharedIdx] * 1000);
		cout << " ==> Thread " << dwThreadId << " releases semaphore..." << endl;
		g_abUsedFlag[nSharedIdx] = FALSE;

		ReleaseSemaphore(hSemaphore, 1, NULL);
		// 자원을 모두 사용했다면 ReleaseSemaphore를 호출해 자원계수를 1 증가시킨다.
		// 그러면 대기 중인 다른 스레드가 해제된 자원을 사용할 수 있게 된다.
	}

	cout << " *** Thread " << dwThreadId << " exits..." << endl;

	return 0;
}


void _tmain()
{
	cout << "======= Start Semaphore Test ========" << endl;
	g_hShareLock = CreateMutex(NULL, FALSE, NULL);	// 사용 중이지 않는 자원 검색을 위한 뮤텍스를 생성한다.

	HANDLE hSemaphore = CreateSemaphore(NULL, MAX_RES_CNT, MAX_RES_CNT, NULL);	// 세마포어를 생성한다.
	// 여기서는 사용 가능한 최대 자원개수가 3이며, 동시에 최초 사용 가능한 자원계수 역시 3임을 의미하도록
	// 두 번째와 세 번째 매개변수의 값을 MAX_RES_CNT로 넘긴다. 따라서 이 세마포어는 시그널 상태로 생성된다.

	DWORD	dwThrId;
	HANDLE	hThreads[MAX_RES_CNT + 2];
	for (int i = 0; i<MAX_RES_CNT + 2; i++)
		hThreads[i] = CreateThread(NULL, 0, SemaphoreProc, hSemaphore, 0, &dwThrId);	// 스레드를 5개 생성한다.
	// 그렇게 되면 처음 3개의 스레드는 자원을 차지하게 되지만 네 번째 스레드부터는 대기 상태로 들어갈 것이다.
	// 스레드 생성 시 세마포어의 핸들을 스레드 엔트리 함수의 매개변수로 넘겨준다.

	getchar();
	g_bExit = true;
	// 임의의 키가 입력될 때까지 메인 스레드를 대기시킨다.
	// 키가 입력되면 스레드 종료 처리를 위해 전역 플래그를 true로 설정한다.

	WaitForMultipleObjects(MAX_RES_CNT + 2, hThreads, TRUE, INFINITE);
	// 5개의 스레드가 모두 종료될 때까지 대기 상태에서 기다린다.
	// 이는 스레드가 정상적으로 종료되도록 하는 가장 안전한 방법이다.

	for (int i = 0; i < MAX_RES_CNT + 2; i++)
	{
		CloseHandle(hThreads[i]);
		CloseHandle(hSemaphore);
		CloseHandle(g_hShareLock);
	}
	// 스레드들과 세마포어, 그리고 뮤텍스의 핸들을 닫는다.

	cout << "======= End Semaphore Test ==========" << endl;
}
