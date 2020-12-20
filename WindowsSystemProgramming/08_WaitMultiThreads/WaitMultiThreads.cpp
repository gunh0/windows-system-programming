#include "stdafx.h"
#include "Windows.h"
#include "iostream"

using namespace std;

// Thread procedure function definition:
DWORD WINAPI ThreadProc(PVOID pParam)
{
	// Sleep for a certain amount of time determined by the parameter:
	DWORD dwDelay = (DWORD)pParam * 1000;

	cout << ">>>>> Thread " << GetCurrentThreadId() << " enter." << endl;
	Sleep(dwDelay);
	cout << "<<<<< Thread " << GetCurrentThreadId() << " leave." << endl;

	return 0;
}

#define MAX_THR_CNT 5
void _tmain()
{
	cout << "Main thread creating sub thread..." << endl;

	HANDLE arhThreads[MAX_THR_CNT];
	// Create multiple threads:
	for (int i = 0; i < MAX_THR_CNT; i++)
	{
		DWORD dwThreadID = 0;
		// The ThreadProc function is passed as the thread procedure function:
		arhThreads[i] = CreateThread(NULL, 0, ThreadProc, (PVOID)(i + 3), 0, &dwThreadID);
	}

	// Wait for all threads to complete before terminating the main thread:
	WaitForMultipleObjects(MAX_THR_CNT, arhThreads, TRUE, INFINITE);
	// WaitForMultipleObjects(MAX_THR_CNT, arhThreads, FALSE, INFINITE);

	// Alternatively, WaitForMultipleObjects can be called with the last parameter as FALSE
	// to wait for any one of the threads to complete before returning.

	cout << "All sub threads terminated......" << endl;
	// Close all thread handles:
	for (int i = 0; i < MAX_THR_CNT; i++)
		CloseHandle(arhThreads[i]);
}