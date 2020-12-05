/*	스레드 간 데이터 전달을 위한 이벤트 사용 예시
메인 함수에서 콘솔로부터 입력받아 공유 버퍼를 통해 서브 스레드에게 그 내용을 전달해준다.
콘솔 입력이 "time"이면 현재 시간을 구해서 SYSTEMTIME 구조체에 담아 공유 버퍼에 쓰고,
"point"면 POINT 구조체에 x, y 좌표의 랜덤값을 설정하고 그 POINT 구조체를 공유 버퍼에 쓴다.
그 이외의 경우면 문자열로 간주해서 그 문자열을 공유 버퍼에 쓰고 서브 스레드에게 통지한다.
*/

#include "stdafx.h"
#include "Windows.h"
#include "iostream"
using namespace std;

#define CMD_NONE 0
#define CMD_STR 1	 // 문자열
#define CMD_POINT 2	 // POINT 구조체
#define CMD_TIME 3	 // SYSTMETIME 구조체
#define CMD_EXIT 100 // 종료 명령

struct WAIT_ENV
{
	HANDLE _hevSend;   // 쓰기 완료 통지용 이벤트
	HANDLE _hevResp;   // 읽기 완료 통지용 이벤트
	LONG _readCnt;	   // 사용계수 관리를 위한 멤버 필드
	BYTE _arBuff[256]; // 공유 버퍼
};

#define CONSUMER_CNT 4 // 버퍼를 읽는 작업 스레드의 수

DWORD WINAPI WorkerProc(LPVOID pParam)
{
	WAIT_ENV* pwe = (WAIT_ENV*)pParam;
	DWORD dwThrId = GetCurrentThreadId();

	while (true)
	{
		DWORD dwWaitCode = WaitForSingleObject(pwe->_hevSend, INFINITE);
		if (dwWaitCode == WAIT_FAILED)
		{
			cout << " ~~~ WaitForSingleObject failed : " << GetLastError() << endl;
			break;
		}

		PBYTE pIter = pwe->_arBuff;
		LONG lCmd = *((PLONG)pIter);
		pIter += sizeof(LONG);
		if (lCmd == CMD_EXIT)
			break;

		LONG lSize = *((PLONG)pIter);
		pIter += sizeof(LONG);
		PBYTE pData = new BYTE[lSize + 1]; // lSize + 1 만큼의 버퍼를 할당한 이유는 문자열일 경우 마지막 NULL 문자를 지정하기 위함
		memcpy(pData, pIter, lSize);	   // 공유 버퍼에서 데이터 크기를 얻고 그 크기만큼 임시 버퍼를 할당한 후 데이터 부분을 복사한다.

		ResetEvent(pwe->_hevSend);
		if (InterlockedIncrement(&pwe->_readCnt) == CONSUMER_CNT)
		{
			pwe->_readCnt = 0;
			SetEvent(pwe->_hevResp);
		}

		switch (lCmd)
		{
		case CMD_STR:
		{
			pData[lSize] = 0;
			printf("  <== R-TH %d read STR : %s\n", dwThrId, pData);
		}
		break;

		// POINT 구조체 형에 대한 명령 처리
		case CMD_POINT:
		{
			PPOINT ppt = (PPOINT)pData;
			printf("  <== R-TH %d read POINT : (%d, %d)\n", dwThrId, ppt->x, ppt->y);
		}
		break;

		// SYSTEMTIME 구조체 형에 대한 명령 처리
		case CMD_TIME:
		{
			PSYSTEMTIME pst = (PSYSTEMTIME)pData;
			printf("  <== R-TH %d read TIME : %04d-%02d-%02d %02d:%02d:%02d+%03d\n",
				dwThrId, pst->wYear, pst->wMonth, pst->wDay, pst->wHour,
				pst->wMinute, pst->wSecond, pst->wMilliseconds);
		}
		break;
		}

		// 공유 버퍼로부터 데이터를 복사한 임시 버퍼를 삭제한다.
		delete[] pData;
	}
	cout << " *** WorkerProc Thread exits..." << endl;

	return 0;
}

void _tmain()
{
	cout << "======= Start EventNotify2 Test ========" << endl;
	cout << "  [1] \"time\"" << endl;
	cout << "  [2] \"point\"" << endl;
	cout << "  [3] Other User Input String" << endl;
	cout << "  [4] \"quit\"" << endl
		<< endl;

	WAIT_ENV we;
	we._hevSend = CreateEvent(NULL, TRUE, FALSE, NULL);
	// 자동 리셋 이벤트는 하나의 스레드만 활동화
	// 4개의 스레드가 모두 반응하도록 하기 위해 수동 리셋 이벤트 생성
	we._hevResp = CreateEvent(NULL, FALSE, FALSE, NULL);
	we._readCnt = 0;

	DWORD dwThrId;
	HANDLE harThrCsmrs[CONSUMER_CNT];

	// 공유 버퍼를 읽는 스레드 4개 생성
	for (int i = 0; i < CONSUMER_CNT; i++)
	{
		harThrCsmrs[i] = CreateThread(NULL, 0, WorkerProc, &we, 0, &dwThrId);
	}

	char szIn[512];
	while (true)
	{
		cin >> szIn;
		// 콘솔로부터 입력받은 문자열이 "quit"일 경우 프로그램 종료를 위해 루프를 탈출한다.
		if (_stricmp(szIn, "quit") == 0)
			break;

		LONG lCmd = CMD_NONE, lSize = 0;
		PBYTE pIter = we._arBuff + sizeof(LONG) * 2;
		// 공유 버퍼 구성을 위해서 데이터를 먼저 채우기 위한 포인터를 지정한다.

		if (_stricmp(szIn, "time") == 0)
		{
			SYSTEMTIME st;
			GetLocalTime(&st);
			memcpy(pIter, &st, sizeof(st));
			lCmd = CMD_TIME, lSize = sizeof(st);
			// "time"인 경우 현재 시간을 구해서 공유 버퍼에 복사하고, 명령 종류와 SYSTEMTIME 구조체의 크기를 설정한다.
		}
		else if (_stricmp(szIn, "point") == 0)
		{
			POINT pt;
			pt.x = rand() % 1000;
			pt.y = rand() % 1000;
			*((PPOINT)pIter) = pt;
			lCmd = CMD_POINT, lSize = sizeof(pt);
			// "point"인 경우 현재 난수를 통해 x,y 좌표를 구해서 POINT 구조체에 대입한 후 공유 버퍼에 쓴다.
			// 또한 명령 종류와 POINT 구조체의 크기를 설정한다.
		}
		else
		{
			lSize = strlen(szIn);
			memcpy(pIter, szIn, lSize);
			lCmd = CMD_STR;
			// 문자열인 경우 문자열의 길이를 얻어 그 길이만큼 공유 버퍼에 복사한 후 CMD_STR 명령을 설정한다.
		}
		((PLONG)we._arBuff)[0] = lCmd;
		((PLONG)we._arBuff)[1] = lSize;
		// 공유 버퍼의 맨 앞 부분에 명령 종류와 데이터 길이를 설정한다.
		// 이것으로 공유 버퍼 구성이 마무리되었으므로 이제 쓰기 완료 통지를 해야 한다.

		SignalObjectAndWait(we._hevSend, we._hevResp, INFINITE, FALSE);
		// 쓰기 완료 통지와 더불어 읽기 완료 통지를 위해 대기한다.
	}

	*((PLONG)we._arBuff) = CMD_EXIT;
	SetEvent(we._hevSend);
	WaitForMultipleObjects(CONSUMER_CNT, harThrCsmrs, TRUE, INFINITE);

	CloseHandle(we._hevResp);
	CloseHandle(we._hevSend);
	cout << "======= End EventNotify Test ==========" << endl;
}
