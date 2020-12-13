// This is a C++ code snippet with an English comment header.

// This line is a preprocessor directive that disables warnings about using functions considered unsafe by Microsoft.
#define _CRT_SECURE_NO_WARNINGS

// These lines include the necessary header files for this code to function.
#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;

// This is the function that handles exceptions. It takes an LPEXCEPTION_POINTERS argument, which is a pointer to an exception record and a context record.
LONG ExpFilter(LPEXCEPTION_POINTERS pEx)
{
	// This line creates a pointer to an exception record.
	PEXCEPTION_RECORD pER = pEx->ExceptionRecord;
	// This creates a character array that will hold the exception message.
	CHAR szOut[512];

	// This line generates a string that will be printed to the console containing information about the exception.
	int nLen = sprintf(szOut, "Code = %x, Address = %p",
					   pER->ExceptionCode, pER->ExceptionAddress);

	// This if statement checks if the exception is an access violation.
	if (pER->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
	{
		// This line appends to the string the type of access that caused the violation (read or write) and the address where the violation occurred.
		sprintf(szOut + nLen, "\nAttempt to %s data at address %p",
				pER->ExceptionInformation[0] ? "write" : "read",
				pER->ExceptionInformation[1]);
	}
	// This line prints the exception message to the console.
	cout << szOut << endl;

	// This statement tells the exception handler to execute the handler code, which in this case is simply printing the message to the console.
	return EXCEPTION_EXECUTE_HANDLER;
}

// This function is the entry point for a thread. It takes a PVOID argument, which is a pointer to the parameter passed to the thread when it was created.
DWORD WINAPI ThreadProc(PVOID pParam)
{
	// This line creates a pointer to a constant string.
	PCSTR pszTest = (PCSTR)pParam;
	// This line creates a null pointer to an integer. It is intentionally left uninitialized to cause an exception later.
	PINT pnVal = NULL;

	// This line creates a character array that is the same length as the input string and copies the string into it.
	PSTR pBuff = new char[strlen(pszTest) + 1];
	strcpy(pBuff, pszTest);
	// The memory allocated by this line should be released when the thread exits.
	// The following code is included to show that the memory can be released even if an exception occurs.

	// This line begins an exception handling block.
	__try
	{
		// This line prints the thread ID and the copied data to the console.
		cout << ">>>>> Thread " << GetCurrentThreadId() << endl;
		cout << "  copied data : " << pBuff << endl;

		// This line intentionally causes an exception by attempting to assign a value to a null pointer.
		*pnVal = strlen(pszTest);

		// This line deletes the allocated memory.
		delete[] pBuff;
	}
	// This line catches any exceptions thrown within the try block and passes them to the exception filter function.
	__except (ExpFilter(GetExceptionInformation()))
	{
		// This line retrieves the exception code.
		DWORD dwExCode = GetExceptionCode();
		// This line prints the exception information to the console.
		printf("===== Exception occurred in thread %d, code=%08X\n",
			   GetCurrentThreadId(), dwExCode);
		// This line deletes the allocated memory.
		delete[] pBuff;
		// This line returns the exception code as the thread exit code.
		return dwExCode;
	}
	// This line returns 0 as the thread exit code if no exceptions occur.
	return 0;
}

// Even if an exception occurs, the main function will exit normally.
void _tmain()
{
	cout << "Main thread creating sub thread..." << endl;

	PCSTR pszTest = "Thread Test...";
	DWORD dwThrID = 0;
	HANDLE hThread = CreateThread(NULL, 0, ThreadProc, (PVOID)pszTest, 0, &dwThrID);
	if (hThread == NULL)
	{
		cout << "~~~ CreateThread failed, error code : "
			 << GetLastError() << endl;
		return;
	}

	getchar();
	WaitForSingleObject(hThread, INFINITE);

	DWORD dwExitCode = 0;
	GetExitCodeThread(hThread, &dwExitCode);
	printf("....Sub thread %d terminated with ExitCode 0x%08X\n", dwThrID, dwExitCode);
	CloseHandle(hThread);
	// Even if an exception occurs in the thread, the exception is handled in the thread,
	// the thread terminates normally, and the exception code that can identify the cause of the exception
	// can be obtained through the exit code, allowing the main thread to exit normally as well.
}
