#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;


DWORD WINAPI ThreadProc(PVOID pParam)
{
	DWORD dwDelay = (DWORD)pParam;

	cout << ">>>>> Thread " << GetCurrentThreadId() << " enter." << endl;
	Sleep(5000);
	cout << "<<<<< Thread " << GetCurrentThreadId() << " leave." << endl;

	return dwDelay;
}

void _tmain()
{
	cout << "Main thread creating sub thread..." << endl;

	DWORD dwThreadID = 0;
	HANDLE hThread = CreateThread(NULL, 0, ThreadProc, (PVOID)5000, 0, &dwThreadID);
	// 스레드 hThread를 생성한다.

	//Sleep(10000);
	WaitForSingleObject(hThread, INFINITE);
	// 생성한 hThread가 종료되기를 기다린다.

	DWORD dwExitCode;
	GetExitCodeThread(hThread, &dwExitCode);
	cout << "\nSub thread " << dwThreadID << " terminated. " << endl;
	cout << "Exit code=" << dwExitCode << endl;
	CloseHandle(hThread);
	// 스레드가 종료되면 종료 코드를 출력해주고 hThread의 핸들을 닫는다.
}