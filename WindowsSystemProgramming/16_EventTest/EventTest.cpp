#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;


DWORD WINAPI ThreadProc(LPVOID pParam)
{
	HANDLE hEvent = (HANDLE)pParam;

	DWORD dwWaitCode = WaitForSingleObject(hEvent, INFINITE);
	// 뮤텍스 대신 자동 리셋 이벤트에 대해 대기한다.

	if (dwWaitCode == WAIT_FAILED)
	{
		cout << "WaitForSingleObject failed : " << GetLastError() << endl;
		return 0;
	}

	SYSTEMTIME st;
	GetLocalTime(&st);

	cout << "..." << "SubThread " << GetCurrentThreadId() << " => ";
	cout << st.wYear << '/' << st.wMonth << '/' << st.wDay << ' ';
	cout << st.wHour << ':' << st.wMinute << ':' << st.wSecond << '+';
	cout << st.wMilliseconds << endl;
	SetEvent(hEvent);	// ReleaseMutext 호출 부분을 SetEvent로 대체한다.
	/* ReleaseMutex는 뮤텍스에 대한 소유권을 해제함으로써 뮤텍스를 시그널 상태로 만드는 동시에
	다른 스레드로 하여금 뮤텍스를 소유할 수 있도록 한다.
	마찬가지로 여기에서는 SetEvent를 호출해 이벤트를 시그널 상태로 만들어 다른 스레드가 움직일 수 있도록 해준다.
	물론 자동 리셋 이벤트이므로 대기하는 스레드 중 임의의 하나만이 위의 코드를 수행할 것이다.
	이벤트의 경우는 ReleaseMutext가 하는 역할을 SetEvent를 통해 넌시그널 상태의 객체를 시그널 상태로
	만들어 마치 뮤텍스의 소유권을 해제한 것과 같은 역할을 수행한다.
	만약 수동 리셋 이벤트였다면 대기하던 모든 스레드가 활성화되어 임계구역에 동시에 진입하기 때문에 동기화는 실패하게 될 것이다.	
	*/

	return 0;
}

void _tmain(void)
{
	cout << "======= Start Event Test ========" << endl;
	HANDLE hEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	// 뮤텍스 대신 자동 리셋 이벤트를 생성해 스레드의 매개변수로 넘긴다.

	HANDLE arhThreads[10];
	for (int i = 0; i < 10; i++)
	{
		DWORD dwTheaID = 0;
		arhThreads[i] = CreateThread(NULL, 0, ThreadProc, hEvent, 0, &dwTheaID);
	}
	WaitForMultipleObjects(10, arhThreads, TRUE, INFINITE);
	CloseHandle(hEvent);
	cout << "======= End Event Test ==========" << endl;
}

