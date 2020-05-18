#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;


DWORD WINAPI ThreadProc(PVOID pParam)
{
	DWORD dwDelay = (DWORD)pParam * 1000;

	cout << ">>>>> Thread " << GetCurrentThreadId() << " enter." << endl;
	Sleep(dwDelay);
	cout << "<<<<< Thread " << GetCurrentThreadId() << " leave." << endl;

	return 0;
}

#define MAX_THR_CNT	 5
void _tmain()
{
	cout << "Main thread creating sub thread..." << endl;

	HANDLE arhThreads[MAX_THR_CNT];
	for (int i = 0; i < MAX_THR_CNT; i++)
	{
		DWORD dwThreadID = 0;
		arhThreads[i] = CreateThread(NULL, 0, ThreadProc, (PVOID)(i + 3), 0, &dwThreadID);
	}

	//WaitForMultipleObjects(MAX_THR_CNT, arhThreads, TRUE, INFINITE);
	WaitForMultipleObjects(MAX_THR_CNT, arhThreads, FALSE, INFINITE);
	/* WaitForMultipleObjects 의 세 번째 매개변수인 bWaitAll 매개변수를 TRUE로 넘겨준 것을 볼 수 있는데,
	만약 이 값이 FALSE라면 돌고 있는 5개의 스레드 중 하나만 종료되더라도
	이 코드는 WaitForMultipleObjects 로부터 탈출해서 메인 스레드를 종료하게 된다.
	// 나머지 4개의 스레드의 비정상적 종료
	*/

	cout << "All sub threads terminated......" << endl;
	for (int i = 0; i < MAX_THR_CNT; i++)
		CloseHandle(arhThreads[i]);
}

