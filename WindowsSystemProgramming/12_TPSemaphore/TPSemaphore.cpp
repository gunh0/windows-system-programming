// Header files including
#include "stdafx.h"
#include "Windows.h"
#include "string"
#include "list"
#include "iostream"

using namespace std;

// Global handle for exit event
HANDLE g_hExit = NULL;

// Define thread pool data structure with mutex, semaphore, and a queue
typedef std::list<std::string> TP_QUE;
struct THREAD_POOL
{
	HANDLE m_hMutx; // Mutex for thread safety in queue manipulation
	HANDLE m_hSema; // Semaphore to limit concurrent access to queue
	TP_QUE m_queue; // Queue to store items waiting to be processed
};
typedef THREAD_POOL *PTHREAD_POOL;

// Thread function to process queue items
DWORD WINAPI PoolItemProc(LPVOID pParam)
{
	// Get pointer to thread pool data structure
	PTHREAD_POOL pTQ = (PTHREAD_POOL)pParam;

	// Get current thread ID
	DWORD dwThId = GetCurrentThreadId();

	// Create an array of handles for wait functions
	HANDLE arhObjs[2] = {g_hExit, pTQ->m_hSema};

	// Process items in the queue
	while (true)
	{
		// Wait for exit event or semaphore signal
		DWORD dwWaitCode = WaitForMultipleObjects(2, arhObjs, FALSE, INFINITE);
		if (dwWaitCode == WAIT_FAILED)
		{
			// Error occurred while waiting
			cout << " ~~~ WaitForSingleObject failed : " << GetLastError() << endl;
			break;
		}

		// If exit event is signaled, exit the thread
		if (dwWaitCode == WAIT_OBJECT_0)
			break;

		// If semaphore is signaled, process an item from the queue
		std::string item;
		WaitForSingleObject(pTQ->m_hMutx, INFINITE);
		TP_QUE::iterator it = pTQ->m_queue.begin();
		item = *it;
		pTQ->m_queue.pop_front();
		ReleaseMutex(pTQ->m_hMutx);

		// Process the item
		printf(" => BEGIN | Thread %d works : %s\n", dwThId, item.c_str());
		Sleep(((rand() % 3) + 1) * 1000);
		printf(" <= END   | Thread %d works : %s\n", dwThId, item.c_str());
	}

	// Signal the exit event
	ReleaseMutex(g_hExit);

	// Print exit message
	printf("......PoolItemProc Thread %d Exit!!!\n", dwThId);

	return 0;
}

void _tmain()
{
	// Create a mutex object that is initially owned and assign its handle to g_hExit
	g_hExit = CreateMutex(NULL, TRUE, NULL);

	// Define a THREAD_POOL structure and create a semaphore object that can be used to limit the number of threads
	THREAD_POOL tp;
	tp.m_hSema = CreateSemaphore(NULL, 0, INT_MAX, NULL);

	// Create a mutex object and assign its handle to tp.m_hMutx
	tp.m_hMutx = CreateMutex(NULL, FALSE, NULL);

	// Retrieve system information and create thread handles for each processor
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	PHANDLE parPoolThs = new HANDLE[si.dwNumberOfProcessors];
	for (int i = 0; i < (int)si.dwNumberOfProcessors; i++)
	{
		DWORD dwThreadId;
		// Create a thread to execute PoolItemProc with the THREAD_POOL structure as the parameter
		parPoolThs[i] = CreateThread(NULL, 0, PoolItemProc, &tp, 0, &dwThreadId);
	}

	// Print the number of threads that have been started
	cout << "*** MAIN -> ThreadPool Started, count=" << si.dwNumberOfProcessors << endl;

	char szIn[1024];
	while (true)
	{
		cin >> szIn;
		if (_stricmp(szIn, "quit") == 0)
			break;

		// Add the entered string to the end of the queue and release the mutex object
		WaitForSingleObject(tp.m_hMutx, INFINITE);
		tp.m_queue.push_back(szIn);
		ReleaseMutex(tp.m_hMutx);

		// Signal that the semaphore object has been released once
		ReleaseSemaphore(tp.m_hSema, 1, NULL);
	}

	// Release the mutex object and wait for all threads to exit
	ReleaseMutex(g_hExit);
	WaitForMultipleObjects(si.dwNumberOfProcessors, parPoolThs, TRUE, INFINITE);

	// Close all thread handles and delete the handle array
	for (int i = 0; i < (int)si.dwNumberOfProcessors; i++)
		CloseHandle(parPoolThs[i]);
	delete[] parPoolThs;

	// Close all created objects' handles
	CloseHandle(tp.m_hSema);
	CloseHandle(tp.m_hMutx);
	CloseHandle(g_hExit);

	cout << "======= End TPSemaphore Test ==========" << endl;
}
