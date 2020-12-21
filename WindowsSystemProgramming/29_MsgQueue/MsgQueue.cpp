#include "stdafx.h"
#include "Windows.h"
#include "list"
#include "iostream"
using namespace std;

// Define constants for command messages
#define TM_CMD_EXIT WM_USER + 1
#define TM_CMD_DATA WM_USER + 2

// Define function for the worker thread
DWORD WINAPI QueueWorkerProc(LPVOID pParam)
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == TM_CMD_EXIT) // Exit the loop if the message is to quit
			break;

		char *pszData = (char *)msg.lParam;
		printf(" => queued data : %s\n", pszData);
		delete[] pszData; // Free up the memory used for the message data
	}
	printf("......QueueWorkerProc Thread Exit!!!\n");

	return 0;
}

void _tmain()
{
	cout << "======= Start MsgQueue Test ========" << endl;

	// Create a worker thread to process messages
	DWORD dwThreadId;
	HANDLE hThread = CreateThread(NULL, 0, QueueWorkerProc, NULL, 0, &dwThreadId);

	// Get input from the user and send it to the worker thread
	char szIn[1024];
	while (true)
	{
		cin >> szIn;

		if (_stricmp(szIn, "quit") == 0) // Send exit command if the user enters "quit"
		{
			PostThreadMessage(dwThreadId, TM_CMD_EXIT, 0, 0);
			break;
		}

		// Allocate memory for the message data and send it to the worker thread
		char *pszData = new char[strlen(szIn) + 1];
		strcpy(pszData, szIn);
		PostThreadMessage(dwThreadId, TM_CMD_DATA, 0, (LPARAM)pszData);
	}

	// Wait for the worker thread to finish and close its handle
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);

	cout << "======= End MsgQueue Test ==========" << endl;
}
