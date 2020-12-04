#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;

#define TEST_MODE 3

#if TEST_MODE == 1
volatile int g_x, g_y;
#else
int g_x, g_y;
#endif
int g_a, g_b;

DWORD WINAPI TP25(PVOID)
{
	g_x = 25;
#if TEST_MODE == 2
	_ReadWriteBarrier();
#elif TEST_MODE == 3
	MemoryBarrier();
#endif
	g_a = g_y;
	return 0;
}

DWORD WINAPI TP99(PVOID)
{
	g_y = 99;
#if TEST_MODE == 2
	_ReadWriteBarrier();
#elif TEST_MODE == 3
	MemoryBarrier();
#endif
	g_b = g_x;
	return 0;
}

void _tmain()
{
	DWORD dwTheaID = 0;
	CloseHandle(CreateThread(NULL, 0, TP25, NULL, 0, &dwTheaID));
	CloseHandle(CreateThread(NULL, 0, TP99, NULL, 0, &dwTheaID));

	getchar();

	cout << "A= " << g_a << ", B=" << g_b << endl;
}

//Debug mode (F10) > Windows > Disassembley (Ctrl + Alt + D)
/*
  최적화 결과 코드는 작성한 순서대로 명령어 배치가 이루어지지 않는다.
  C++로 작성한 TP25 엔트리 함수에서는 g_x에 25 값을 대입하는 코드가 먼저 나와 있지만,
  역어셈플된 결과는 g_a = g_y; 코드 뒤로 간다.
  결국 CPU는 g_a에 g_y 값을 대입하는 코드를 먼저 실행한 후 g_x에 25를 대입하는 코드를 실행하게 된다.
*/