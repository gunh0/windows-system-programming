#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;


DWORD WINAPI ThreadProc(LPVOID pParam)
{
	PHANDLE	parWaits = (PHANDLE)pParam;
	DWORD	dwThrId = GetCurrentThreadId();

	while (true)
	{
		DWORD dwWaitCode = WaitForMultipleObjects(2, parWaits, FALSE, INFINITE);
		// 뮤텍스 2개에 대해 동시 대기한다.
		// 첫 번째 뮤텍스는 종료 통지가 목적이고, 두 번째 뮤텍스는 아래 시간 출력 코드의 동기화를 위해 사용된다.
		if (dwWaitCode == WAIT_FAILED)
		{
			cout << " ~~~ WaitForSingleObject failed : " << GetLastError() << endl;
			break;
		}

		if (dwWaitCode == WAIT_OBJECT_0)
		{
			printf(" ===> SubThread %d exits....\n", dwThrId);
			ReleaseMutex(parWaits[0]);
			break;
			// 첫 번째 뮤텍스가 시그널 상태가 되었고, 이는 종료 통지를 의미하므로 루프를 탈출한다.
			// 탈출하기 전에 ReleaseMutex를 호출해 탈출하게 될 스레드 역시 소유권을 놓아줘야 한다.
			// 이렇게 소유권을 놓아주면 마치 노미노처럼 스레드 사이에서
			// 뮤텍스에 대한 소유권 해제와 획득이 차례대로 반복 전파되어 최종 스레드에게까지 전달된다.
		}
		//if (dwWaitCode == WAIT_ABANDONED)
		//	cout << " **** SubThread " << dwThrId << " : Abandoned Mutex"  << endl;
		//if (dwWaitCode == WAIT_OBJECT_0 || dwWaitCode == WAIT_ABANDONED)
		//	break;

		SYSTEMTIME st;
		GetLocalTime(&st);
		cout << "..." << "SubThread " << dwThrId << " => ";
		cout << st.wYear << '/' << st.wMonth << '/' << st.wDay << ' ';
		cout << st.wHour << ':' << st.wMinute << ':' << st.wSecond << '+';
		cout << st.wMilliseconds << endl;

		if (!ReleaseMutex(parWaits[1]))
		{
			cout << " ~~~ Thread " << dwThrId <<
				" : ReleaseMutex failed, Error Code=" << GetLastError() << endl;
		}
		Sleep(1000);
	}
	return 0;
}

#define MAX_THR_CNT	3
void _tmain(void)
{
	cout << "======= Start MutexNoti Test ========" << endl;
	HANDLE arWaits[2];
	arWaits[0] = CreateMutex(NULL, TRUE, NULL);
	// 스레드 종료 통지용 뮤텍스를 생성하기 위해 메인 스레드가 우선 스레드를 소유하도록
	// bInitialOwner 매개변수를 TRUE로 지정한다.
	arWaits[1] = CreateMutex(NULL, FALSE, NULL);
	// 임계구역 설정을 위한 또 다른 뮤텍스를 생성한다.

	HANDLE arhThreads[MAX_THR_CNT];
	for (int i = 0; i < MAX_THR_CNT; i++)
	{
		DWORD dwTheaID = 0;
		arhThreads[i] = CreateThread(NULL, 0, ThreadProc, arWaits, 0, &dwTheaID);
	}

	getchar();
	ReleaseMutex(arWaits[0]);
	// 키를 입력받으면 실행 중인 스레드들을 종료하기 위해서 소유한 뮤텍스를 해제한다.

	WaitForMultipleObjects(MAX_THR_CNT, arhThreads, TRUE, INFINITE);
	for (int i = 0; i < MAX_THR_CNT; i++)
		CloseHandle(arhThreads[i]);
	for (int i = 0; i < 2; i++)
		CloseHandle(arWaits[i]);
	cout << "======= End MutexNoti Test ==========" << endl;
}

