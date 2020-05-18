#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;


#define MAX_RES_CNT		3

bool	g_bExit = false;
BOOL	g_abUsedFlag[MAX_RES_CNT];
INT		g_anSharedRes[MAX_RES_CNT] = { 5, 3, 7 };
//HANDLE	g_hShareLock;
HANDLE	g_hExit = NULL;

DWORD WINAPI SemaphoreProc(LPVOID pParam)
{
	HANDLE hSemaphore = (HANDLE)pParam;
	DWORD  dwThreadId = GetCurrentThreadId();

	HANDLE arhWaits[2] = { g_hExit, hSemaphore };
	/* WaitForMultipleObjects를 사용하기 위해 커널 객체 핸들의 배열을 선언하고
	종료 이벤트와 세마포어를 원소로 지정한다.
	어떤 객체가 시그널 상태가 되었는지는 이 배열의 인덱스를 통해 판단하기 때문에
	순서가 중요하다. */

	while (!g_bExit)
	{
		//DWORD dwWaitCode = WaitForSingleObject(hSemaphore, INFINITE);
		DWORD dwWaitCode = WaitForMultipleObjects(2, arhWaits, FALSE, INFINITE);
		// WaitForMultipleObjects를 호출해 이벤트와 세마포어 두 개의 커널 객체에 대해 시그널 상태를 기다린다.

		if (dwWaitCode == WAIT_FAILED)
		{
			cout << " ~~~ WaitForSingleObject failed : " << GetLastError() << endl;
			break;
		}

		int nSharedIdx = 0;
		//WaitForSingleObject(g_hShareLock, INFINITE);
		for (; nSharedIdx < MAX_RES_CNT; nSharedIdx++)
		{
			if (!g_abUsedFlag[nSharedIdx])
				break;
		}
		g_abUsedFlag[nSharedIdx] = TRUE;
		//ReleaseMutex(g_hShareLock);


		if (dwWaitCode == WAIT_OBJECT_0)
			break;
		/* 대기 결괏값이 WAIT_OBJECT_0이면 종료 이벤트가 시그널 상태기 때문에
		메인 스레드가 종료를 원한다는 것을 판단할 수 있다.
		이 경우 break를 통해 정상적으로 무한 루프를 탈출하면 깔끔한 스레드 종료 처리가 완료된다.
		WAIT_OBJECT_0 + 1은 세마포어가 시그널 상태가 되었음을 의미하므로 스레드 자신의 고유한 작업을 수행하면 된다. */

		cout << " ==> Thread " << dwThreadId <<
			" waits " << g_anSharedRes[nSharedIdx] << " seconds..." << endl;
		Sleep((DWORD)g_anSharedRes[nSharedIdx] * 1000);
		cout << " ==> Thread " << dwThreadId << " releases semaphore..." << endl;
		g_abUsedFlag[nSharedIdx] = FALSE;

		ReleaseSemaphore(hSemaphore, 1, NULL);
	}

	cout << " *** Thread " << dwThreadId << " exits..." << endl;

	return 0;
}


void _tmain()
{
	cout << "======= Start Semaphore Test ========" << endl;
	//g_hShareLock = CreateMutex(NULL, FALSE, NULL);
	g_hExit = CreateEvent(NULL, TRUE, FALSE, NULL);
	// 종료를 통지하는 이벤트를 생성한다.
	// 두 번째 매개변수를 TRUE로 넘겨주어 수동 리셋 이벤트를 만들어야 한다.
	// 또한 세 번째 매개변수인 초기 상태 지정을 FALSE로 넘겨주어 넌시그널 상태로 생성해야 한다.
	// 그렇지 않으면 스레드들은 바로 종료로 판단하고 종료되어 버릴 것이다.

	HANDLE hSemaphore = CreateSemaphore(NULL, MAX_RES_CNT, MAX_RES_CNT, NULL);

	DWORD	dwThrId;
	HANDLE	hThreads[MAX_RES_CNT + 2];
	for (int i = 0; i < MAX_RES_CNT + 2; i++)
		hThreads[i] = CreateThread(NULL, 0, SemaphoreProc, hSemaphore, 0, &dwThrId);

	getchar();
	//g_bExit = true;
	SetEvent(g_hExit);	// 종료를 위해 종료 이벤트를 시그널 상태로 설정한다.
	/* 종료해야 할 시점에서 SetEvent를 호출하면 g_hExit 이벤트는 넌시그널 상태에서
	시그널 상태로 바뀌고, 이 이벤트에 대해 대기하는 스레드들은 모두 대기에서 깨어나
	종료 처리를 수행할 수 있게 될 것이다. */
	WaitForMultipleObjects(MAX_RES_CNT + 2, hThreads, TRUE, INFINITE);

	for (int i = 0; i < MAX_RES_CNT + 2; i++)
		CloseHandle(hThreads[i]);
	CloseHandle(hSemaphore);

	//CloseHandle(g_hShareLock);
	CloseHandle(g_hExit);	// 종료 이벤트의 핸들을 닫는다.
	cout << "======= End Semaphore Test ==========" << endl;
}
