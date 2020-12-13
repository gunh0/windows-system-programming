#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;


DWORD WINAPI ThreadProc(LPVOID pParam)
{	
	/* 스레드 엔트리 함수에서는 4개의 커널 객체에 대해 동시 대기를 하며,
	각 객체가 시그널 상태가 될때마다 대기 상태에서 탈출해서 해당 작업을 처리한다.
	*/

	PHANDLE parSyncs = (PHANDLE)pParam;

	while (true)
	{
		DWORD dwWaitCode = WaitForMultipleObjects(4, parSyncs, FALSE, INFINITE);
		// WaitForMultipleObjects를 이용해 4개의 동기화 객체에 대해 동시에 대기에 들어간 후
		// 각각의 통지에 대해 본 스레드에서 모든 처리를 수행한다.

		if (dwWaitCode == WAIT_FAILED)
		{
			cout << "WaitForSingleObject failed : " << GetLastError() << endl;
			return 0;
		}
		if (dwWaitCode == WAIT_OBJECT_0) // 종료 이벤트 시그널
			break;

		dwWaitCode -= WAIT_OBJECT_0;
		switch (dwWaitCode)
		{
			case 1 : // 이벤트 시그널
				cout << " +++++ Event Singnaled!!!" << endl;
				ResetEvent(parSyncs[dwWaitCode]);
			break;
			case 2:	// 뮤텍스 시그널
				cout << " ***** Mutex Singnaled!!!" << endl;
				ReleaseMutex(parSyncs[dwWaitCode]);
			break;
			case 3:	// 세마포어 시그널
				cout << " ##### Semaphore Singnaled!!!" << endl;
			break;
		}
	}
	return 0;
}

void _tmain(void)
{
	cout << "======= Start MultiSyncWaits Test ========" << endl;

	HANDLE arhSyncs[4];
	arhSyncs[0] = CreateEvent(NULL, TRUE, FALSE, NULL);	
	// 종료를 위한 이벤트를 수동 리셋 이벤트로 생성하고 배열의 인덱스 0에 할당한다.

	//arhSyncs[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
	arhSyncs[1] = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	arhSyncs[2] = CreateMutex(NULL, TRUE, NULL);
	arhSyncs[3] = CreateSemaphore(NULL, 0, 1, NULL);
	// 각각 특정 사건을 대변하는 이벤트, 뮤텍스, 세마포어 객체를 생성해 각각 배열에 순서대로 할당한다.
	// 이벤트의 경우 자동 리셋 이벤트로 생성시켰다.

	DWORD dwTheaID = 0;
	HANDLE hThread = CreateThread(NULL, 0, ThreadProc, arhSyncs, 0, &dwTheaID);
	// 동기화 객체 핸들 배열을 스레드의 엔트리 함수 매개변수로 전달한다.

	char szIn[32];
	while (true)
	{
		cin >> szIn;
		if (_stricmp(szIn, "quit") == 0)
			break;
		// 종료 처리로, 루프를 빠져나가 종료 이벤트를 시그널링한다.

		if (_stricmp(szIn, "event") == 0)
			SetEvent(arhSyncs[1]);
		// 이벤트를 통해 시간을 통지한다.

		else if (_stricmp(szIn, "mutex") == 0)
		{
			ReleaseMutex(arhSyncs[2]);
			WaitForSingleObject(arhSyncs[2], INFINITE);
			// 뮤텍스를 통해 사건을 통지한다.
			// 뮤텍스의 경우 데이터 보호에 최적화되어 있으나 위의 방식으로 통지해도 사용 가능하다.
		}

		else if (_stricmp(szIn, "semaphore") == 0)
			ReleaseSemaphore(arhSyncs[3], 1, NULL);
		// 세마포어를 통해 사건을 통지한다.
	}
	SetEvent(arhSyncs[0]);	// 스레드 종료를 통지한다.
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	for (int i = 0; i < 4; i++)
		CloseHandle(arhSyncs[i]);

	cout << "======= End MultiSyncWaits Test ==========" << endl;
}

