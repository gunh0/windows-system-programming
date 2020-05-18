/*
종료 통지를 통해서 스레드를 안전하게 종료시키는 처리를 추가
*/

#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;


class MyClass
{
	static LONG ExpFilter(LPEXCEPTION_POINTERS pEx);
	static DWORD WINAPI MyThreadProc(PVOID pParam);
	void InnerThreadProc();

	HANDLE	m_hThread;
	DWORD	m_dwThreadId;
	HANDLE	m_hExit;	// m_bExit로 선언된 Bool 타입의 변수를 이벤트를 위한 핸들 타입의 변수로 변경
	DWORD	m_dwDelay;

public:
	MyClass() : m_hThread(NULL), m_hExit(NULL), m_dwDelay(1000)
	{
	}
	~MyClass()
	{
		if (m_hThread != NULL)
			CloseHandle(m_hThread);
		if (m_hExit != NULL)
			CloseHandle(m_hExit);	// 소멸자에서 m_hExit 이벤트 커널 객체를 닫는 코드를 추가
	}

public:
	HRESULT Start()
	{
		m_hExit = CreateEvent(NULL, FALSE, FALSE, NULL);
		// 수동 리셋 이벤트를 생성
		if (m_hExit == NULL)
			return HRESULT_FROM_WIN32(GetLastError());
		m_hThread = CreateThread(NULL, 0, MyThreadProc, this, 0, &m_dwThreadId);
		if (m_hThread == NULL)
			return HRESULT_FROM_WIN32(GetLastError());
		return S_OK;
	}

	void Stop()
	{
		//SetEvent(m_hExit);
		//WaitForSingleObject(m_hThread, INFINITE);
		// 종료 통지를 위해 m_hExit 이벤트를 시그널 상태로 만들고,
		// 곧바로 m_hThread 핸들을 사용해서 스레드가 종료되기를 기다리는 두 단계의 처리를 하나의 함수로 대체
		SignalObjectAndWait(m_hExit, m_hThread, INFINITE, FALSE);
	}
};


LONG MyClass::ExpFilter(LPEXCEPTION_POINTERS pEx)
{
	PEXCEPTION_RECORD pER = pEx->ExceptionRecord;
	printf("~~~~ Exception : Code = %x, Address = %p",
		pER->ExceptionCode, pER->ExceptionAddress);

	return EXCEPTION_EXECUTE_HANDLER;
}

DWORD WINAPI MyClass::MyThreadProc(PVOID pParam)
{
	MyClass* pThis = (MyClass*)pParam;
	__try
	{
		pThis->InnerThreadProc();
	}
	__except (ExpFilter(GetExceptionInformation()))
	{
	}
	return 0;
}

void MyClass::InnerThreadProc()
{
	do
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		printf("Thread %d, delay %d => %5d/%02d/%02d-%02d:%02d:%02d+%03d\n",
			m_dwThreadId, m_dwDelay, st.wYear, st.wMonth, st.wDay,
			st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	}
	while (WaitForSingleObject(m_hExit, m_dwDelay) == WAIT_TIMEOUT);
	/* WaitForSingleObject의 호출 결과가 WAIT_OBJECT_0이면 m_hExit가
	시그널 상태임을 의미하므로 루프를 탈출한다.
	WAIT_TIMEOUT이면 Sleep 역할을 하는 동시에 계속 루프를 돌게 된다.	
	*/
}


void _tmain()
{
	cout << "Main thread creating sub thread..." << endl;

	MyClass mc;
	HRESULT hr = mc.Start();
	if (FAILED(hr))
	{
		cout << "MyClass start failed, error code is " << hr;
		return;
	}

	getchar();

	mc.Stop();

	cout << "Main thread creating sub thread..." << endl;
}
