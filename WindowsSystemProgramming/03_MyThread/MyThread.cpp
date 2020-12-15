#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;

class MyClass
{
	static LONG ExpFilter(LPEXCEPTION_POINTERS pEx);
	static DWORD WINAPI MyThreadProc(PVOID pParam);
	void InnerThreadProc();
	// Declare the thread entry function as a static function within the class.
	// Declare InnerThreadProc function inside the thread entry function to replace the role of the entry function for convenient use of class member variables.

	HANDLE m_hThread;
	DWORD m_dwThreadId;
	bool m_bExit;
	DWORD m_dwDelay;

public:
	MyClass() : m_hThread(NULL), m_bExit(false), m_dwDelay(1000)
	{
	}
	~MyClass()
	{
		if (m_hThread != NULL)
			CloseHandle(m_hThread);
	}

public:
	HRESULT Start()
	{
		m_hThread = CreateThread(NULL, 0, MyThreadProc, this, 0, &m_dwThreadId); // Create the thread.
		// Pass the instance pointer of this class as a parameter when creating.
		if (m_hThread == NULL)
			return HRESULT_FROM_WIN32(GetLastError());
		return S_OK;
	}

	void Stop()
	{
		m_bExit = true; // Notify the thread to exit.
		while (m_dwThreadId > 0)
			Sleep(100); // Wait for the thread to exit - waiting for the thread to exit is important!
						// Polling thread exit wait processing
	}
};

// Define the exception information output function:
LONG MyClass::ExpFilter(LPEXCEPTION_POINTERS pEx)
{
	PEXCEPTION_RECORD pER = pEx->ExceptionRecord;
	printf("~~~~ Exception : Code = %x, Address = %p",
		   pER->ExceptionCode, pER->ExceptionAddress);
	// Print simple exception-related information
	return EXCEPTION_EXECUTE_HANDLER;
}

// Define the thread entry function:
DWORD WINAPI MyClass::MyThreadProc(PVOID pParam)
{
	MyClass *pThis = (MyClass *)pParam;
	__try
	{
		pThis->InnerThreadProc(); // Define the thread entry function.
								  // Call the member function InnerThreadProc, which delegates the thread execution, by type casting the this pointer received as a parameter.
	}
	__except (ExpFilter(GetExceptionInformation()))
	{
		pThis->m_dwThreadId = 0;
		// Add processing for when an exception occurs using SEH.
		// Since m_dwThreadId is used for thread exit processing,
		// m_dwThreadId is set to 0 in the exception processing for normal termination even in the event of an exception.
	}
	return 0;
}

void MyClass::InnerThreadProc()
{
	while (!m_bExit) // Use m_bExit to receive the thread's exit notification.
	// If m_bExit is true, exit the loop.
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		printf("Thread %d, delay %d => %5d/%02d/%02d-%02d:%02d:%02d+%03d\n",
			   m_dwThreadId, m_dwDelay, st.wYear, st.wMonth, st.wDay,
			   st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		Sleep(m_dwDelay);
		// Emulate the thread's work processing using Sleep.
	}
	m_dwThreadId = 0;
	// Set m_dwThreadId to 0 to indicate that the thread has ended.
	// This is an incomplete process, but it is also an important part.
}

// Define the main function:
void _tmain()
{
	cout << "Main thread creating sub thread..." << endl;

	MyClass mc;
	HRESULT hr = mc.Start();
	// Declare MyClass and call the Start member function to start an internal thread.

	if (FAILED(hr))
	{
		cout << "MyClass start failed, error code is " << hr;
		return;
	}

	getchar();

	mc.Stop();
	// If a random key is pressed, call Stop to end the thread execution of MyClass.
	// Thread termination wait processing is included in Stop.

	cout << "Sub thread terminated. Main thread exiting..." << endl;
}
