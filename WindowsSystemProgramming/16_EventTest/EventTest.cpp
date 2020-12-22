#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;

// Declare a function that will be executed by threads.
// It waits for an auto-reset event to become signaled and prints the current time.
DWORD WINAPI ThreadProc(LPVOID pParam)
{
	HANDLE hEvent = (HANDLE)pParam;

	DWORD dwWaitCode = WaitForSingleObject(hEvent, INFINITE);
	// Wait for the auto-reset event instead of a mutex.

	if (dwWaitCode == WAIT_FAILED)
	{
		cout << "WaitForSingleObject failed : " << GetLastError() << endl;
		return 0;
	}

	SYSTEMTIME st;
	GetLocalTime(&st);

	cout << "..."
		 << "SubThread " << GetCurrentThreadId() << " => ";
	cout << st.wYear << '/' << st.wMonth << '/' << st.wDay << ' ';
	cout << st.wHour << ':' << st.wMinute << ':' << st.wSecond << '+';
	cout << st.wMilliseconds << endl;

	// Set the event to signal it to other waiting threads.
	// This replaces the ReleaseMutex call.
	SetEvent(hEvent);
	/* ReleaseMutex releases the ownership of a mutex, making it signaled, and allows
	another thread to take ownership of the mutex.
	Similarly, SetEvent is called here to make the event signaled and allow other threads to proceed.
	Since this is an auto-reset event, only one of the waiting threads will execute the above code.
	For events, SetEvent performs the same role as ReleaseMutex by making the unsignaled object signaled,
	as if releasing the mutex ownership.
	If this were a manual-reset event, all waiting threads would become active and enter the critical section simultaneously,
	causing synchronization to fail.
	*/

	return 0;
}

void _tmain(void)
{
	cout << "======= Start Event Test ========" << endl;
	HANDLE hEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	// Create an auto-reset event instead of a mutex and pass it as a parameter to the threads.

	HANDLE arhThreads[10];
	for (int i = 0; i < 10; i++)
	{
		DWORD dwTheaID = 0;
		arhThreads[i] = CreateThread(NULL, 0, ThreadProc, hEvent, 0, &dwTheaID);
	}
	// Create 10 threads and pass the auto-reset event as a parameter.

	WaitForMultipleObjects(10, arhThreads, TRUE, INFINITE);
	// Wait for all threads to finish.

	CloseHandle(hEvent);
	cout << "======= End Event Test ==========" << endl;
}
