/*
int main(int argc, char* argv[])
// The main function is a special function that is the starting point for all C and C++ programs.

int wmain(int argc, wchar_t* argv[])
// "When writing code that follows the Unicode programming model, you can use the wide-character version of main, wmain" (MSDN)

int _tmain(int argc, _TCHAR *argv[])
// _tmain is resolved to main if _UNICODE is not defined.
// If _UNICODE is defined, _tmain is resolved to wmain.
// When Unicode is defined, it is compiled as wmain, and when it is not defined, it is compiled as main.
// However, to use _tmain, you must declare the header file TCHAR.h.

// What happens when declaring void instead of int?
// main and wmain functions can be declared to return void (no return value) instead of int.
// If you declare main or wmain to return void, you cannot use the return statement to return an exit code to the parent process or operating system.
// If main or wmain is declared to return void, you must use the exit function to return an exit code.
*/

#include "stdafx.h"
#include "Windows.h"
#include "iostream"

using namespace std;

DWORD WINAPI ThreadProc(PVOID pParam)
{
	DWORD dwDelay = (DWORD)pParam;
	// Obtain the value of 5000, which was passed as a parameter, from the pParam parameter.

	cout << ">>>>> Thread " << GetCurrentThreadId() << " enter." << endl;
	Sleep(dwDelay);
	cout << "<<<<< Thread " << GetCurrentThreadId() << " leave." << endl;
	// Set the value received as a parameter and perform Sleep processing for 5 seconds before ending the thread.

	return dwDelay;
	// Return the exit code of the thread as 5000. As with the main function, this value can be obtained using the GetExitCodeThread function.
	// In fact, in most cases, it will return 0.
}

void _tmain()
{
	cout << "Main thread creating sub thread..." << endl;
	DWORD dwThreadID = 0;
	HANDLE hThread = CreateThread(
		NULL,		 // Pass NULL as the security identifier.
		0,			 // Specify the default stack size (1MB) by passing 0.
		ThreadProc,	 // Pass a pointer to the user-defined thread entry function
		(PVOID)5000, // and specify the parameter to be passed. In this case, the parameter is set to 5000.
		0,			 // Set it to be executed immediately upon creation.
		&dwThreadID	 // The ID of the thread is stored in the dwThreadID variable and returned.
	);
	if (hThread == NULL) // If the result of the CreateThread function call is failure, it is NULL, and GetLastError is used to output the error code.
	{
		cout << "~~~ CreateThread failed, error code : "
			<< GetLastError() << endl;
		return;
	}

	WaitForSingleObject(hThread, INFINITE);

	DWORD dwExitCode = 0;
	GetExitCodeThread(hThread, &dwExitCode);
	// Obtain the thread exit code, i.e., the return value of the entry function.

	cout << "Sub thread " << dwThreadID
		<< " terminated with ExitCode " << dwExitCode << endl;
	CloseHandle(hThread);
	// Since a thread is also a kernel object, it must be closed by calling CloseHandle to enable it to be deleted by the kernel.
}
