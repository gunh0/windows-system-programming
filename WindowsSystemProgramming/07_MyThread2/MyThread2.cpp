#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;

class MyClass
{
	// Declare thread entry function as static function within the class.
	// Declare the InnerThreadProc function within the thread entry function for convenience of using class member variables.
	static LONG ExpFilter(LPEXCEPTION_POINTERS pEx);
	static DWORD WINAPI MyThreadProc(PVOID pParam);
	void InnerThreadProc();

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
		// Create a thread, passing the instance pointer of the current class (this) as a parameter.
		m_hThread = CreateThread(NULL, 0, MyThreadProc, this, 0, &m_dwThreadId);
		if (m_hThread == NULL)
			return HRESULT_FROM_WIN32(GetLastError());
		return S_OK;
	}

	void Stop()
	{
		m_bExit = true; // Notify the thread to exit.

		// Wait until the thread has finished executing.
		// The previous polling method of waiting for the thread to exit is replaced by calling a waiting function for the thread.
		WaitForSingleObject(m_hThread, INFINITE);
	}
};

// Definition of function to print exception information:
LONG MyClass::ExpFilter(LPEXCEPTION_POINTERS pEx)
{
	PEXCEPTION_RECORD pER = pEx->ExceptionRecord;
	printf("~~~~ Exception : Code = %x, Address = %p",
		   pER->ExceptionCode, pER->ExceptionAddress);
	// Print basic information about the exception.

	return EXCEPTION_EXECUTE_HANDLER;
}

// Definition of thread entry function:
DWORD WINAPI MyClass::MyThreadProc(PVOID pParam)
{
	MyClass *pThis = (MyClass *)pParam;
	__try
	{
		pThis->InnerThreadProc(); // Call the member function InnerThreadProc to perform the thread's work.
								  // Convert the 'this' pointer passed as a parameter to the appropriate type.
	}
	__except (ExpFilter(GetExceptionInformation()))
	{
		pThis->m_dwThreadId = 0;
		// Add exception handling for when an exception is thrown in the thread.
		// Set m_dwThreadId to 0 in the exception handling code, to ensure normal termination.
	}
	return 0;
}

void MyClass::InnerThreadProc()
{
	while (!m_bExit) // Use m_bExit to receive a signal to terminate the thread.
	// If m_bExit is true, exit the loop.
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		printf("Thread %d, delay %d => %5d/%02d/%02d-%02d:%02d:%02d+%03d\n",
			   m_dwThreadId, m_dwDelay, st.wYear, st.wMonth, st.wDay,
			   st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		Sleep(m_dwDelay);
		// Emulate the thread's work using Sleep.
	}
	m_dwThreadId = 0;
	// Set m_dwThreadId to 0 to indicate that the thread has terminated.
	// This is an important step, although it is not a complete one.
}

// Main function definition:
void _tmain()
{
	cout << "Main thread creating sub thread..." << endl;

	MyClass mc;
	HRESULT hr = mc.Start();
	// Declare MyClass and call its Start member function to start an internal thread.

	if (FAILED(hr))
	{
		cout << "MyClass start failed, error code is " << hr;
		return;
	}

	getchar();

	mc.Stop();
	// Call Stop to terminate the execution of MyClass's thread when a key is pressed.
	// The thread exit waiting processing is included in Stop.

	cout << "Main thread creating sub thread..." << endl;
}
