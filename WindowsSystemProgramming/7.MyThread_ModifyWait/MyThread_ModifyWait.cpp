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
	bool	m_bExit;
	DWORD	m_dwDelay;

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
		m_hThread = CreateThread(NULL, 0, MyThreadProc, this, 0, &m_dwThreadId);
		if (m_hThread == NULL)
			return HRESULT_FROM_WIN32(GetLastError());
		return S_OK;
	}

	void Stop()
	{
		m_bExit = true;
		WaitForSingleObject(m_hThread, INFINITE);
		/*
		스레드 핸들에 대해 WaitForSingleObject 함수를 호출할 때 타임아웃 값을 0으로 지정하면?
		WaitForSingleObject로부터 바로 리턴되는데, 리턴값이 WAIT_OBJECT_0이면 시그널 상태임을 의미하므로
		m_hThread에 해당하는 스레드는 종료되었음을 알 수 있다. (단순히 해당 커널 객체의 신호 상태를 체크한다는 의미)
		반대로 리턴값이 WAIT_TIMEOUT, 즉 타임아웃이 발생한 경우라면 해당 스레드는 넌시그널 상태임을 의미하므로
		여전히 실행 중이라는 것을 알 수 있다.
		*/
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
	while (!m_bExit)
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		printf("Thread %d, delay %d => %5d/%02d/%02d-%02d:%02d:%02d+%03d\n",
			m_dwThreadId, m_dwDelay, st.wYear, st.wMonth, st.wDay,
			st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		Sleep(m_dwDelay);
	}
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
