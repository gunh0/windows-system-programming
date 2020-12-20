#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;

#include "PowrProf.h"
#pragma comment(lib, "PowrProf.lib")

void _tmain()
{
	// create a waitable timer object and initialize it
	SYSTEMTIME st;
	HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL);

	LARGE_INTEGER lUTC;
	lUTC.QuadPart = -300000000LL;

	REASON_CONTEXT rc;
	rc.Version = POWER_REQUEST_CONTEXT_VERSION;
	rc.Flags = POWER_REQUEST_CONTEXT_SIMPLE_STRING;
	wchar_t szReason[] = L"Continuous measurement";
	rc.Reason.SimpleReasonString = szReason;

	// set the timer object to signal after 30 seconds
	// and associate it with the specified reason context
	SetWaitableTimerEx(hTimer, &lUTC, 30000, NULL, NULL, &rc, 1000);
	GetLocalTime(&st);
	printf("Timer Set Call : %04d/%02d/%02d-%02d:%02d:%02d+%03d\n",
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	// put the system into standby mode after setting the timer
	SetSuspendState(TRUE, FALSE, FALSE);

	// wait for the timer object to signal and beep after it does so
	int nSnoozeCnt = 0;
	while (nSnoozeCnt < 5)
	{
		WaitForSingleObject(hTimer, INFINITE);

		GetLocalTime(&st);
		printf("Timer Signaled : %04d/%02d/%02d-%02d:%02d:%02d+%03d\n",
			st.wYear, st.wMonth, st.wDay,
			st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

		if (nSnoozeCnt == 0)
			MessageBeep(MB_ICONWARNING);

		nSnoozeCnt++;
	}

	getchar();
	CloseHandle(hTimer);
}
