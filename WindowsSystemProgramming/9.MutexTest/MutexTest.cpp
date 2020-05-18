#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;


DWORD WINAPI ThreadProc(LPVOID pParam)
{
	HANDLE hMutex = (HANDLE)pParam;

	DWORD dwWaitCode = WaitForSingleObject(hMutex, INFINITE);
	// 소유권을 획득할 때까지 대기한다.
	if (dwWaitCode == WAIT_FAILED)
	{
		cout << "WaitForSingleObject failed : " << GetLastError() << endl;
		return 0;
	}
	if (dwWaitCode == WAIT_ABANDONED)
		cout << "Abandoned Mutex acquired!!!" << GetLastError() << endl;

	SYSTEMTIME st;
	GetLocalTime(&st);

	cout << "..." << "SubThread " << GetCurrentThreadId() << " => ";
	cout << st.wYear << '/' << st.wMonth << '/' << st.wDay << ' ';
	cout << st.wHour << ':' << st.wMinute << ':' << st.wSecond << '+';
	cout << st.wMilliseconds << endl;
	//ReleaseMutex(hMutex);
	// 획득한 소유권을 해제한다.
	/* 만약 뮤텍스의 소유권을 획득한 후 ReleaseMutex를 호출하지 않고 해당 스레드를 종료하도록 처리한 경우,
	아니면 스레드가 뮤텍스를 소유한 후 작업 처리 중이며 ReleaseMutex를 호출하기 전인 상황에서 다른 스레드에서
	TerminateThread를 호출해 그 스레드를 강제로 종료시켜 버릴 경우에 운영체제는 스레드가 종료될 때
	뮤텍스에 대한 소유가 있는지를 검사하게 되고 뮤텍스의 종료로 인해 포기되었음을 알리기 위해
	리턴값을 WAIT_ABANDONED로 취해 대기 함수에서 리턴하도록 한다.
	이 결과, 비록 소유권을 가진 채 스레드가 종료되더라도 그 버려진 뮤텍스를 다른 스레드가 이어서 사용할 수 있게 해준다.
	*/

	return 0;
}

#define MAX_THR_CNT	10
void _tmain(void)
{
	cout << "======= Start Mutex Test ========" << endl;
	HANDLE hMutex = CreateMutex(NULL, FALSE, NULL);
	// Mutex 생성.
	// 두 번째 매개변수로 FALSE를 넘겨주어 메인 스레드가 뮤텍스를 소유하지 않음을 명시한다.
	// 생성한 뮤텍스의 핸들은 스레드 엔트리 함수의 매개변수로 넘겨준다.

	//HANDLE hMutex = CreateMutex(NULL, FALSE, NULL);
	/*
	CreateMutex 의 bInitialOwner 매개변수는 앞서 설명한 대로 이 함수를 호출한 스레드가 생성되는 뮤텍스를 소유할 것인지의 여부를 지정한다.
	만약 위 소스에서 이 매개변수를 TRUE로 넘겨주면 CreateMutex를 호출한 프로그램의 메인 스레드가
	새롭게 생성된 이 뮤텍스를 생성과 동시에 소유하게 된다.
	따라서 엔트리 함수 내에서 WaitForXXX를 호출하고 대기 중인 10개의 스레드들은 결코 이 뮤텍스의 소유권을 획득하지 못하고
	무한히 대기하게 된다.
	*/

	HANDLE arhThreads[MAX_THR_CNT];
	for (int i = 0; i < MAX_THR_CNT; i++)
	{
		DWORD dwTheaID = 0;
		arhThreads[i] = CreateThread(NULL, 0, ThreadProc, hMutex, 0, &dwTheaID);
	}

	//if (ReleaseMutex(hMutex) == FALSE)
	//	printf("~~~ ReleaseMutex error, code=%d\n", GetLastError());

	WaitForMultipleObjects(MAX_THR_CNT, arhThreads, TRUE, INFINITE);
	// 생성한 모든 스레드가 종료될 때까지 대기한다.

	for (int i = 0; i < MAX_THR_CNT; i++)
		CloseHandle(arhThreads[i]);
	CloseHandle(hMutex);
	cout << "======= End Mutex Test ==========" << endl;
}

