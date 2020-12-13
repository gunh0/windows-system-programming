// WaitableTimerTest.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;

void _tmain()
{
	WORD wHour, wMin;
	DWORD dwPeriod;

	cout << "Enter Hour Minute Period : ";
	cin >> wHour >> wMin >> dwPeriod;

	HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL);

	SYSTEMTIME st;
	GetLocalTime(&st);
	st.wHour = wHour;
	st.wMinute = wMin;
	st.wSecond = st.wMilliseconds = 0;

	FILETIME ftLocal, ftUTC;
	SystemTimeToFileTime(&st, &ftLocal);
	LocalFileTimeToFileTime(&ftLocal, &ftUTC);
	LARGE_INTEGER lUTC;
	lUTC.LowPart = ftUTC.dwLowDateTime;
	lUTC.HighPart = ftUTC.dwHighDateTime;

	SetWaitableTimer(hTimer, &lUTC, dwPeriod * 1000, NULL, NULL, TRUE);

	char szTimeS[64];
	sprintf_s(szTimeS, "%04d/%02d/%02d-%02d:%02d:%02d",
			  st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	cout << "Alarm Reserved : " << szTimeS << endl
		 << endl;

	int nSnoozeCnt = 0;
	while (nSnoozeCnt < 5)
	{
		WaitForSingleObject(hTimer, INFINITE);
		GetLocalTime(&st);

		sprintf_s(szTimeS, "%04d/%02d/%02d-%02d:%02d:%02d",
				  st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		if (nSnoozeCnt == 0)
			cout << "Alarm Started : ";
		else
			cout << nSnoozeCnt << " Snooze Time : ";
		cout << szTimeS << endl;
		nSnoozeCnt++;
	}
	CloseHandle(hTimer);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started:
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
