/*
int main(int argc, char* argv[])
	main 이라는 특수함수는 모든 c및 c++ 프로그램이 실행이 시작되는 지점


int wmain(int argc, wchar_t* argv[])
	"유니코드 프로그래밍모델을 따르는 코드를 작성하는경우 main 의 와이드 문자버전인 wmain을 사용할 수 있습니다" (MSDN)


int _tmain(int argc, _TCHAR *argv[])
	_tmain은 _UNICODE가 정의되지 않은 한 main으로 확인된다.
	_UNICODE가 정의된 경우에는 _tmain이 wmain으로 확인됩니다.
	유니코드가 정의된경우 컴파일시 wmain으로, 정의되지 않은경우 main으로 컴파일된다.
	단, _tmain 사용을위해서 TCHAR.h 헤더파일을 선언해주어야 한다.


int 형대신 void형으로 선언하는 경우?
	main 및 wmain 함수가 void(반환 값 없음)를 반환하는 것으로 선언될 수 있다.
	void를 반환하는 것으로 main 또는 wmain을 선언하는 경우 return 문을 사용하여 부모 프로세스나 운영 체제에 종료 코드를 반환할 수 없다.
	main 또는 wmain이 void를 반환하는 것으로 선언된 경우, 종료 코드를 반환하려면 exit 함수를 사용해야 한다.
*/

#include "stdafx.h"
#include "Windows.h"
#include "iostream"

using namespace std;

DWORD WINAPI ThreadProc(PVOID pParam)
{
	DWORD dwDelay = (DWORD)pParam;
	// CreateThread의 매개변수로 넘겨줬던 5000이란 값을 pParam 매개변수로부터 획득한다.

	cout << ">>>>> Thread " << GetCurrentThreadId() << " enter." << endl;
	Sleep(dwDelay);
	cout << "<<<<< Thread " << GetCurrentThreadId() << " leave." << endl;
	// 매개변수로 전달받은 값을 지정해 5초 동안 Sleep 처리를 한 후 스레드를 종료한다.

	return dwDelay;
	// 스레드 종료 코드를 5000으로 돌려준다. 메인 함수에서 확인했던 것처럼, GetExitCodeThread 함수를 통해 이 값을 획득할 수 있다.
	// 사실 대부분의 경우 0을 리턴할 것이다.
}

void _tmain()
{
	cout << "Main thread creating sub thread..." << endl;
	DWORD dwThreadID = 0;
	HANDLE hThread = CreateThread
	(
		NULL,			// 보안 식별자를 NULL로 넘겼다.
		0,				// 0으로 넘겨서 디폴트 스택 크기(1MB)를 지정했다.
		ThreadProc,		// 유저가 정의한 스레드 엔트리 함수에 대한 포인터와
		(PVOID)5000,	// 넘겨줄 매개변수를 지정한다. 매개병수는 5000 값을 주었다.
		0,				// 생성과 동시에 실행되도록 설정한다.
		&dwThreadID		// dwThreadID 변수에 스레드의 ID가 저장되어 반환된다.
	);
	if (hThread == NULL)	// CreateThread 함수 호출 결과가 실패인 경우에는 NULL이며, GetLastError를 통해 에러 코드를 출력한다.
	{
		cout << "~~~ CreateThread failed, error code : "
			<< GetLastError() << endl;
		return;
	}

	WaitForSingleObject(hThread, INFINITE);

	DWORD dwExitCode = 0;
	GetExitCodeThread(hThread, &dwExitCode);
	// 스레드 종료 코드, 즉 엔트리 함수의 리턴값을 획득한다.

	cout << "Sub thread " << dwThreadID
		<< " terminated with ExitCode " << dwExitCode << endl;
	CloseHandle(hThread);
	// 스레드 역시 커널 객체이므로, CloseHandle을 호출해 닫아줘야 커널에 의해 삭제가 가능하다.
}
