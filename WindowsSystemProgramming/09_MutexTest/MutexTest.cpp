#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;

DWORD WINAPI ThreadProc(LPVOID pParam)
{
	HANDLE hMutex = (HANDLE)pParam;

	DWORD dwWaitCode = WaitForSingleObject(hMutex, INFINITE);
	if (dwWaitCode == WAIT_FAILED)
	{
		cout << "WaitForSingleObject failed : " << GetLastError() << endl;
		return 0;
	}
	if (dwWaitCode == WAIT_ABANDONED)
		cout << "Abandoned Mutex acquired!!!" << GetLastError() << endl;

	SYSTEMTIME st;
	GetLocalTime(&st);

	cout << "..."
		 << "SubThread " << GetCurrentThreadId() << " => ";
	cout << st.wYear << '/' << st.wMonth << '/' << st.wDay << ' ';
	cout << st.wHour << ':' << st.wMinute << ':' << st.wSecond << '+';
	cout << st.wMilliseconds << endl;
	// ReleaseMutex(hMutex);
	//  Releases the ownership of the mutex.
	/* If a thread acquires the ownership of a mutex but does not call ReleaseMutex before exiting,
	or if another thread calls TerminateThread to forcibly terminate the thread while it is
	holding the mutex, the operating system checks if the mutex was owned when the thread exited
	and sets the return value to WAIT_ABANDONED to indicate that the mutex was abandoned.
	As a result, even if the thread that owned the mutex exits with the ownership, the abandoned mutex
	can be used by another thread that picks it up.*/

	return 0;
}

#define MAX_THR_CNT 10
void _tmain(void)
{
	cout << "======= Start Mutex Test ========" << endl;
	HANDLE hMutex = CreateMutex(NULL, FALSE, NULL);
	// Create a mutex.
	// Pass FALSE as the second parameter to indicate that the main thread does not own the mutex.
	// The handle of the created mutex is passed as a parameter to the thread entry function.

	// HANDLE hMutex = CreateMutex(NULL, FALSE, NULL);
	/*
	The bInitialOwner parameter of CreateMutex specifies whether the thread that calls this function will own the created mutex.
	If you pass TRUE to this parameter in the source above, the main thread of the program that called CreateMutex will own this newly created mutex at the same time as it is created.
	Therefore, if you call WaitForXXX in the entry function and there are 10 threads waiting, they will never acquire ownership of this mutex and wait infinitely.
	*/

	HANDLE arhThreads[MAX_THR_CNT];
	for (int i = 0; i < MAX_THR_CNT; i++)
	{
		DWORD dwTheaID = 0;
		arhThreads[i] = CreateThread(NULL, 0, ThreadProc, hMutex, 0, &dwTheaID);
	}

	// if (ReleaseMutex(hMutex) == FALSE)
	//	printf("~~~ ReleaseMutex error, code=%d\n", GetLastError());

	WaitForMultipleObjects(MAX_THR_CNT, arhThreads, TRUE, INFINITE);

	for (int i = 0; i < MAX_THR_CNT; i++)
		CloseHandle(arhThreads[i]);
	CloseHandle(hMutex);
	cout << "======= End Mutex Test ==========" << endl;
}
