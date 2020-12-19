#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "Windows.h"
#include "list"
#include "iostream"
using namespace std;


#define WQI_CMD_EXIT	0	// 종료 처리를 위한 명령
// 스레드 풀에서는 종료 처리를 위해 별도로 전역 뮤텍스를 사용했지만,
// 여기서는 명령 식별값으로 대체한다. 따라서 종료를 위한 별도의 객체가 필요 없다.
#define WQI_CMD_STR		1	// 문자열 처리를 위한 명령
#define WQI_CMD_INT		2	// 정수값 처리를 위한 명령

struct WQUE_ITEM
{
	INT		_cmd;
	PVOID	_data;
};
typedef std::list<WQUE_ITEM> ITEM_QUE;
// 큐의 항목을 정의한다. _cmd는 _data의 타입을 식별하고 _data는 처리할 실제 데이터를 담게 된다.

struct WAIT_QUE
{
	HANDLE	 _hMutx;
	HANDLE	 _hSema;
	ITEM_QUE _queue;
};
typedef WAIT_QUE* PWAIT_QUE;
// 대기기능 큐에 필요한 항목을 담고 있다. 스레드 풀의 경우와 구성요소는 동일하다.

// enqueue 처리 부분은 다음의 함수로 구현한다. 스레드 풀 예제에서의 엔큐 처리와 동일하다.
void WQ_Enqueue(PWAIT_QUE pWQ, INT cmd, PVOID data)
{
	WQUE_ITEM wqi;
	wqi._cmd = cmd, wqi._data = data;

	WaitForSingleObject(pWQ->_hMutx, INFINITE);
	pWQ->_queue.push_back(wqi);
	ReleaseMutex(pWQ->_hMutx);

	ReleaseSemaphore(pWQ->_hSema, 1, NULL);
}

// dequeue 처리 부분은 다음의 함수로 구현한다. 스레드 풀 예제에서의 dequeue 처리와 동일하다.
INT WQ_Dequeue(PWAIT_QUE pWQ, PVOID& data)
{
	WaitForSingleObject(pWQ->_hSema, INFINITE);

	WQUE_ITEM wqi;
	WaitForSingleObject(pWQ->_hMutx, INFINITE);
	ITEM_QUE::iterator it = pWQ->_queue.begin();
	wqi = *it;
	pWQ->_queue.pop_front();
	ReleaseMutex(pWQ->_hMutx);

	data = wqi._data;
	return wqi._cmd;
}


DWORD WINAPI QueueWorkerProc(LPVOID pParam)
{
	PWAIT_QUE pWQ = (PWAIT_QUE)pParam;

	while (true)
	{
		/*
		DWORD dwWaitCode = WaitForSingleObject(pWQ->_hSema, INFINITE);
		// 세마포어에 대해 대기한다.
		// WQI_CMD_EXIT로 종료 처리가 가능하기 때문에 세마포어 하나에 대해서만 대기해도 상관없다.
		// 또한, 필요하다면 다른 여러 동기화 객체와 더불어
		// WaitForMultipleObjects를 호출해 스레드 내의 여러 통지에 대한 처리 역시 가능하다.
		*/

		PVOID data = NULL;
		INT cmd = WQ_Dequeue(pWQ, data);	// 큐로부터 항목을 추출한다.

		if (cmd == WQI_CMD_EXIT)
			break;
		// 종료 명령이면 루프를 탈출하고 스레드를 종료한다. 별도의 종료 통지용 객체 없이 일반 처리 항목과 같이 사용할 수 있다.

		if (cmd == WQI_CMD_INT)
			printf(" => CMD_INT queued : %d\n", (int)data);
		else
		{
			printf(" => CMD_STR queued: %s\n", (char*)data);
			delete[]((char*)data);
		}
		// 명령에 맞게 데이터를 출력한 후 작업을 완료한 다음 다시 루프를 돌아 대기 함수를 호출한다.
	}
	printf("......QueueWorkerProc Thread Exit!!!\n");

	return 0;
}

void _tmain()
{
	cout << "======= Start WQSemaphore Test ========" << endl;

	WAIT_QUE wq;
	wq._hSema = CreateSemaphore(NULL, 0, LONG_MAX, NULL);
	wq._hMutx = CreateMutex(NULL, FALSE, NULL);
	// 임계구역 설정용 뮤텍스와 큐 관리를 위한 세마포어를 생성한다.
	// 세마포어 생성 시와 마찬가지로 초기값을 0으로 저장한다.

	DWORD dwThreadId;
	HANDLE hThread = CreateThread(NULL, 0, QueueWorkerProc, &wq, 0, &dwThreadId);
	// 스레드를 하나만 생성한다. enqueue된 큐의 항목을 처리하는 스레드이다.

	char szIn[1024];
	while (true)
	{
		cin >> szIn;

		INT   cmd = 0;
		PVOID data = NULL;
		if (_stricmp(szIn, "quit") == 0)
			cmd  = WQI_CMD_EXIT;	// 문자열을 입력받아 quit이면 종료를 알리는 WQI_CMD_EXIT를 설정해준다.
		else
		{
			int nVal = atoi(szIn);
			if (nVal == 0)	// 정수값으로의 변환 결과가 0이 아니면 정수로 간주하고 WQI_CMD_INT와 정수값을 지정한다.
			{
				cmd = WQI_CMD_STR;
				data = new char[strlen(szIn) + 1];
				strcpy((char*)data, szIn);
				// 정수값으로의 변환 결과가 0이면 문자열로 간주하고, WQI_CMD_STR과 입력받은 문자열을 복사한다.
			}
			else
			{
				cmd = WQI_CMD_INT;
				data = (PVOID)nVal;
			}
		}

		WQ_Enqueue(&wq, cmd, data);	// 큐 항목을 큐에 enqueue한다. 이와 동시에 스레드는 활성화 된다.
		if (cmd == WQI_CMD_EXIT)
			break;
	}

	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	CloseHandle(wq._hSema);
	CloseHandle(wq._hMutx);

	cout << "======= End WQSemaphore Test ==========" << endl;
}
