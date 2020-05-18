#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;

//#define USE_VOLATILE

#define OFF	 0
#define ON	 1

#ifdef USE_VOLATILE
void AcquireLock(volatile LONG& lSign)
{
	while (lSign == OFF);
	// lSign이 OFF일 동안 계속 루프를 돌다가 ON이 되는 순간 빠져나와 코드를 실행한다.
	lSign = OFF;
	// 사용하고 있음을 알리기 위해 OFF 상태로 만든다.
}
#else
void AcquireLock(LONG& lSign)
{
	while (lSign == OFF);
	lSign = OFF;
}
#endif

void ReleaseLock(LONG& lSign)
{
	lSign = ON;
	// lSign을 ON으로 만들어 다른 스레드가 공유 코드를 실행할 수 있도록 한다.
}


LONG g_lCSSign = ON;
// ON/OFF 표식으로 사용될 전역 변수이다. 최초 ON 상태로 설정하여 공유자원을 누구나 사용할 수 있음을 표시해줘야 한다.
int  g_nValue = 0;
// 원자성을 확보해줘야 할, 스레드 사이에서 서로 공유되는 전역 변수이다.

DWORD WINAPI ThreadProc(PVOID pParam)
{
	INT dwCurId = (INT)pParam;

	AcquireLock(g_lCSSign);	// <- 공유자원 사용 여부 체크 및 대기

	cout << " ==> Thread " << dwCurId << " Enter : " << g_nValue << endl;
	g_nValue++;
	cout << "  <= Thread " << dwCurId << " Leave : " << g_nValue << endl;

	ReleaseLock(g_lCSSign);	// <- 공유자원 사용 완료 통지

	return 0;
}

#define MAX_THR_CNT	6
void _tmain()
{
	HANDLE arhThrs[MAX_THR_CNT];
	for (int i = 0; i < MAX_THR_CNT; i++)
	{
		DWORD dwTheaID = 0;
		arhThrs[i] = CreateThread(NULL, 0, ThreadProc, (PVOID)i, 0, &dwTheaID);
	}

	getchar();

	for (int i = 0; i < MAX_THR_CNT; i++)
		CloseHandle(arhThrs[i]);
}

// 이와 같은 방법으로는 Acquire/ReleaseLock 사이의 코드에 대한 원자성을 확보할 수 없다. = 동기화를 이루지 못한다.