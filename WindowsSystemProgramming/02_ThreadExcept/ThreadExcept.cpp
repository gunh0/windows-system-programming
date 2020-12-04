#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;

LONG ExpFilter(LPEXCEPTION_POINTERS pEx)
{
	PEXCEPTION_RECORD pER = pEx->ExceptionRecord;
	CHAR szOut[512];

	int nLen = sprintf(szOut, "Code = %x, Address = %p",
		pER->ExceptionCode, pER->ExceptionAddress);

	if (pER->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
	{
		sprintf(szOut + nLen, "\nAttempt to %s data at address %p",
			pER->ExceptionInformation[0] ? "write" : "read",
			pER->ExceptionInformation[1]);
	}
	cout << szOut << endl;

	return EXCEPTION_EXECUTE_HANDLER;
}


DWORD WINAPI ThreadProc(PVOID pParam)
{
	PCSTR pszTest = (PCSTR)pParam;
	PINT  pnVal = NULL;	// 의도적으로 예외를 발생시키기 위해 NULL로 초기화된 포인터 변수를 선언한다.

	PSTR  pBuff = new char[strlen(pszTest) + 1];
	strcpy(pBuff, pszTest);
	// 메모리를 할당하고 매개변수를 통해 받은 문자열을 복사한다.
	// 스레드 종료 시에 해제되어야 할 리소스다.
	// 할당된 메모리를 예외 발생 시에도 해제 가능함을 보여주기 위해 추가한 코드이다.

	__try
	{
		cout << ">>>>> Thread " << GetCurrentThreadId() << endl;
		cout << "  copied data : " << pBuff << endl;

		*pnVal = strlen(pszTest);	// 이 지점에서 의도적으로 예외를 발생시킨다.
		// NULL로 초기화된 포인터 변수에 값을 대입한다.

		delete[] pBuff;
	}
	__except (ExpFilter(GetExceptionInformation()))	// 예외를 잡고 상세 내용을 분석하기 위해
	{												// GetExceptionInformation 매크로를 ExpFilter 매개변수로 넘겨준다.
		DWORD dwExCode = GetExceptionCode();
		printf("===== Exception occurred in thread %d, code=%08X\n",
			GetCurrentThreadId(), dwExCode);
		delete[] pBuff;
		// 할당했던 메모리를 해제한다. 예외 처리가 없을 경우 메모리를 해제할 기회를 갖지 못한다.
		return dwExCode;
		// 예외 코드를 스레드 종료 코드로 리턴한다.
	}
	return 0;
}

void _tmain()	// 예외가 발생했더라도 메인 함수까지 정상적으로 종료된다.
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
	// 예외가 발생했으나 스레드에서 예외를 처리하고 스레드가 정상적으로 종료되었으며,
	// 종료 코드를 통해 메인 스레드에서 예외의 원인을 식별할 수 있는 예외 코드를 획들할 수 있다.
	// 그리고 메인 함수도 정상적인 종료가 가능하다.
}
