// Example of semaphore from the perspective of shared resources

#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;

#define MAX_RES_CNT 3 // Defines the maximum number of resources as 3.

bool g_bExit = false;						// Flag to indicate thread termination.
BOOL g_abUsedFlag[MAX_RES_CNT];				// Array to represent used flags.
INT g_anSharedRes[MAX_RES_CNT] = {5, 3, 7}; // An array to represent resources.
HANDLE g_hShareLock;						// Mutex handle for searching available resources.

DWORD WINAPI SemaphoreProc(LPVOID pParam)
{
	HANDLE hSemaphore = (HANDLE)pParam;		 // A handle for semaphore.
	DWORD dwThreadId = GetCurrentThreadId(); // Current thread ID.

	while (!g_bExit)
	{
		DWORD dwWaitCode = WaitForSingleObject(hSemaphore, INFINITE);
		// Waits for the semaphore.

		if (dwWaitCode == WAIT_FAILED)
		{
			cout << " ~~~ WaitForSingleObject failed : " << GetLastError() << endl;
			break;
		}

		// If dwWaitCode is not an error, it means that the semaphore is in the signal state.
		// Then, the thread can acquire the shared resource and perform its operation.

		int nSharedIdx = 0;
		WaitForSingleObject(g_hShareLock, INFINITE);
		for (; nSharedIdx < MAX_RES_CNT; nSharedIdx++)
		{
			if (!g_abUsedFlag[nSharedIdx])
				break;
		}
		g_abUsedFlag[nSharedIdx] = TRUE;
		ReleaseMutex(g_hShareLock);

		/* Since the semaphore being in the signal state only means that there is an available resource,
		there is no separate means of identifying that resource.
		Therefore, a loop is necessary to find the available resource.
		Since multiple threads can perform this loop at the same time, synchronization is required
		when searching for an available resource, so synchronization is performed using a mutex. */

		cout << " ==> Thread " << dwThreadId << " waits " << g_anSharedRes[nSharedIdx] << " seconds..." << endl;
		Sleep((DWORD)g_anSharedRes[nSharedIdx] * 1000);
		cout << " ==> Thread " << dwThreadId << " releases semaphore..." << endl;
		g_abUsedFlag[nSharedIdx] = FALSE;

		ReleaseSemaphore(hSemaphore, 1, NULL);
		// After using all resources, ReleaseSemaphore is called to increase the resource count by 1.
		// Then, other waiting threads can use the released resource.
	}

	cout << " *** Thread " << dwThreadId << " exits..." << endl;

	return 0;
}

void _tmain()
{
	cout << "======= Start Semaphore Test ========" << endl;
	// Create a mutex to search for unused resources.
	g_hShareLock = CreateMutex(NULL, FALSE, NULL);

	// Create a semaphore with maximum count and initial count of MAX_RES_CNT.
	HANDLE hSemaphore = CreateSemaphore(NULL, MAX_RES_CNT, MAX_RES_CNT, NULL);
	// The maximum number of available resources is 3 and the initial number of available resources is also 3.
	// Therefore, the semaphore is created in the signaled state.

	DWORD dwThrId;
	HANDLE hThreads[MAX_RES_CNT + 2];

	// Create 5 threads.
	for (int i = 0; i < MAX_RES_CNT + 2; i++)
	{
		// Pass the semaphore handle as a parameter to the thread entry function.
		hThreads[i] = CreateThread(NULL, 0, SemaphoreProc, hSemaphore, 0, &dwThrId);
		// The first 3 threads will occupy resources, but the fourth and fifth threads will be in a waiting state.
	}

	getchar();

	// Set a global flag to true to exit threads when a key is pressed.
	g_bExit = true;

	// Wait for all 5 threads to terminate.
	WaitForMultipleObjects(MAX_RES_CNT + 2, hThreads, TRUE, INFINITE);
	// This is the safest way to ensure that all threads exit properly.

	// Close thread handles, semaphore handle, and mutex handle.
	for (int i = 0; i < MAX_RES_CNT + 2; i++)
	{
		CloseHandle(hThreads[i]);
	}
	CloseHandle(hSemaphore);
	CloseHandle(g_hShareLock);

	cout << "======= End Semaphore Test ==========" << endl;
}
