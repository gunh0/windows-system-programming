#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;


class MyClass
{
	static LONG ExpFilter(LPEXCEPTION_POINTERS pEx);
	static DWORD WINAPI MyThreadProc(PVOID pParam);
	void InnerThreadProc();
	// 클래스 내에서 스레드 엔트리 함수를 정적 함수로 선언한다.
	// 그리고 클래스 멤버 변수의 편리한 사용을 위해 스레드 엔트리 함수 내부에서 호출해
	// 엔트리 함수의 역할을 대신할 InnerThreadProc 함수를 선언한다.

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
		m_hThread = CreateThread(NULL, 0, MyThreadProc, this, 0, &m_dwThreadId);	// 스레드를 생성한다.
		//생성 시 매개변수를 본 클래스의 인스턴스 포인터, 즉 this로 넘겨준다.
		if (m_hThread == NULL)
			return HRESULT_FROM_WIN32(GetLastError());
		return S_OK;
	}

	void Stop()
	{
		m_bExit = true;	// 스레드에게 종료는 통지하는 역할을 한다.
		while (m_dwThreadId > 0)
			Sleep(100);	// 스레드가 종료될 때까지 대기 - 스레드가 종료될 때까지 대기하는 처리는 중요!
			// 폴링 방식 스레드 종료 대기 처리
	}
};


// 예외 정보 출력 함수 정의:
LONG MyClass::ExpFilter(LPEXCEPTION_POINTERS pEx)
{
	PEXCEPTION_RECORD pER = pEx->ExceptionRecord;
	printf("~~~~ Exception : Code = %x, Address = %p",
		pER->ExceptionCode, pER->ExceptionAddress);
	// 예외 관련 정보를 간단하게 출력

	return EXCEPTION_EXECUTE_HANDLER;
}


// 스레드 엔트리 함수 정의:
DWORD WINAPI MyClass::MyThreadProc(PVOID pParam)
{
	MyClass* pThis = (MyClass*)pParam;
	__try
	{
		pThis->InnerThreadProc();	// 스레드 엔트리 함수의 정의
		// 매개변수로 전달받은 this 포인터를 타입 변환을 통하여 스레드 수행을 대리하는 멤버 함수 InnerThreadProc을 호출
	}
	__except (ExpFilter(GetExceptionInformation()))
	{
		pThis->m_dwThreadId = 0;
		// SEH를 이용해 예외 발생 시의 처리를 추가
		// 스레드 종료 처리 시 m_dwThreadId를 이용하기 때문에,
		// 예외 시에도 정상 종료를 위해 예외 처리에서 m_dwThreadId를 0으로 설정해준다.
	}
	return 0;
}

void MyClass::InnerThreadProc()
{
	while (!m_bExit)	// 스레드의 종료 통지를 받기 위해 m_bExit를 사용한다.
	// m_bExit가 true면 루프를 탈출한다.
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		printf("Thread %d, delay %d => %5d/%02d/%02d-%02d:%02d:%02d+%03d\n",
			m_dwThreadId, m_dwDelay, st.wYear, st.wMonth, st.wDay, 
			st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		Sleep(m_dwDelay);
		// 스레드의 작업 처리 부분을 Sleep을 통해 에뮬레이션한다.
	}
	m_dwThreadId = 0;
	// 스레드가 종료되엇음을 알리기 위해 m_dwThreadId를 0으로 설정한다.
	// 불완전한 처리지만 역시 중요한 부분이다.
}


// 메인 함수 정의:
void _tmain()
{
	cout << "Main thread creating sub thread..." << endl;

	MyClass mc;
	HRESULT hr = mc.Start();
	// MyClass를 선언하고 Start 멤버 함수를 호출해 내부 스레드를 개시한다.

	if (FAILED(hr))
	{
		cout << "MyClass start failed, error code is " << hr;
		return;
	}

	getchar();

	mc.Stop();
	// 임의의 키로 입력이 들어오면 Stop을 호출해 MyClass의 스레드 실행을 종료한다.
	// Stop 내에 스레드 종료 대기 처리가 들어 있다.

	cout << "Main thread creating sub thread..." << endl;
}
