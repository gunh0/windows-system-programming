#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;

// Exclusive/Read/Write Lock structure
struct ERWLOCK
{
	HANDLE _hevRO; // Event object for Read-Only
	LONG _nRoRefs; // Number of read-only locks held

	HANDLE _hevWO; // Event object for Write-Only
	HANDLE _hmuWO; // Mutex object for Write-Only

	// Constructor
	ERWLOCK()
	{
		_hevRO = _hevWO = _hmuWO = NULL;
		_nRoRefs = 0;
	}

	// Destructor
	~ERWLOCK()
	{
		if (_hevRO != NULL)
			CloseHandle(_hevRO);
		if (_hevWO != NULL)
			CloseHandle(_hevWO);
		if (_hmuWO != NULL)
			CloseHandle(_hmuWO);
	}
};

// Typedef ERWLOCK pointer to PERWLOCK
typedef ERWLOCK *PERWLOCK;

// Write Delay
#define DELAY_WRITE 5000
// Read Delay
#define DELAY_READ 200

LONG g_nValue;	// Shared variable
LONG g_bIsExit; // Exit flag

// ReaderProc function: Function executed by reader threads
DWORD WINAPI ReaderProc(PVOID pParam)
{
	// Cast the ERWLOCK pointer from the parameter
	PERWLOCK prwl = (PERWLOCK)pParam;
	// Get current thread ID
	DWORD dwThrId = GetCurrentThreadId();

	// Continue execution while g_bIsExit variable is 0
	while (g_bIsExit == 0)
	{
		// Wait for the _hevWO event object that the writer thread is waiting on
		WaitForSingleObject(prwl->_hevWO, INFINITE);
		// Reset the _hevRO event object for reading
		ResetEvent(prwl->_hevRO);
		// Increment the number of read-only locks held
		InterlockedIncrement(&prwl->_nRoRefs);

		// Output the current value
		cout << "    => TH " << dwThrId << " Reads : " << g_nValue << endl;

		// If all readers are done, signal the _hevRO event object
		if (InterlockedDecrement(&prwl->_nRoRefs) == 0)
			SetEvent(prwl->_hevRO);

		// Wait for a random time
		Sleep(rand() % DELAY_READ);
	}

	return 0;
}

// WriterProc function: Function executed by writer threads
DWORD WINAPI WriterProc(PVOID pParam)
{
	// Cast the ERWLOCK pointer from the parameter
	PERWLOCK prwl = (PERWLOCK)pParam;
	// Get current thread ID
	DWORD dwThrId = GetCurrentThreadId();

	while (g_bIsExit == 0)
	{
		// Wait for a random time
		Sleep(rand() % DELAY_WRITE);

		// Reset the _hevWO event object for writing
		ResetEvent(prwl->_hevWO);
		// Wait for the _hevRO event object for reading and _hmuWO mutex object for writing
		WaitForSingleObject(prwl->_hevRO, INFINITE);
		WaitForSingleObject(prwl->_hmuWO, INFINITE);

		// Increase the value
		g_nValue++;
		// Output the new value
		cout << " <= TH " << dwThrId << " Writes value : " << g_nValue << endl;

		// Release the _h
		ReleaseMutex(prwl->_hmuWO);
		SetEvent(prwl->_hevWO);
	}

	return 0;
}

#define MAX_READ_CNT 5
#define MAX_WRITE_CNT 2

void _tmain()
{
	ERWLOCK rwl;
	rwl._hevRO = CreateEvent(NULL, TRUE, TRUE, NULL);
	rwl._hevWO = CreateEvent(NULL, TRUE, TRUE, NULL);
	rwl._hmuWO = CreateMutex(NULL, FALSE, NULL);

	HANDLE woThrs[MAX_WRITE_CNT];
	for (int i = 0; i < MAX_WRITE_CNT; i++)
	{
		DWORD dwThrId = 0;
		// Create threads for writers with WriterProc as the entry point
		// and pass the rwl object as the parameter.
		woThrs[i] = CreateThread(NULL, 0, WriterProc, &rwl, 0, &dwThrId);
	}

	HANDLE roThrs[MAX_READ_CNT];
	for (int i = 0; i < MAX_READ_CNT; i++)
	{
		DWORD dwThrId = 0;
		// Create threads for readers with ReaderProc as the entry point
		// and pass the rwl object as the parameter.
		roThrs[i] = CreateThread(NULL, 0, ReaderProc, &rwl, 0, &dwThrId);
	}

	getchar();
	g_bIsExit = TRUE;

	// Wait for all reader threads to finish
	WaitForMultipleObjects(MAX_READ_CNT, roThrs, TRUE, INFINITE);
	// Wait for all writer threads to finish
	WaitForMultipleObjects(MAX_WRITE_CNT, woThrs, TRUE, INFINITE);
}
