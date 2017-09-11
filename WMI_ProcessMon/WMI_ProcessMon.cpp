// WMI_ProcessMon.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//
#include "stdafx.h"
#include "eventsink.h"
#include "ProcessMon.h"

// CProcessMon을 상속받아 구현해서 사용하는 클래스 
class CStartMon : public CProcessMon
{
	void Log(char* sMsg)
    {
        SYSTEMTIME LocalTime; 
        GetLocalTime(&LocalTime); 
 
        char szDate[1024];
        ZeroMemory(szDate, 1024);
 
        sprintf(szDate, "[%04d-%02d-%02d %02d:%02d:%02d]",  
                LocalTime.wYear, LocalTime.wMonth,
                LocalTime.wDay, LocalTime.wHour,
                LocalTime.wMinute, LocalTime.wSecond); 
 
        printf("%s %s\n", szDate, sMsg);
        
    }

	void OnCreate()
	{
		char szMSG[1024];
        ZeroMemory(szMSG, 1024);
		sprintf(szMSG, "[%s] 실행 되었습니다!!", this->m_szProcessName);

        Log(szMSG);
	};

	void OnDelete()
	{
        char szMSG[1024];
        ZeroMemory(szMSG, 1024);
		sprintf(szMSG, "[%s] 종료 되었습니다!!", this->m_szProcessName);

		Log(szMSG);

	};
};


int main(int iArgCnt, char ** argv)
{
	
	CStartMon c;
	
	printf("cmd.exe를 모니터링 합니다. 아무키나 누르면 다음으로 넘어갑니다.\n");
	if (c.StartWatching("cmd.exe")) 
		printf("WMI 생성에 실패했습니다.");
	
	getchar();

	c.StopWatching();
	printf("python.exe를 모니터링 합니다. 아무키나 누르면 종료합니다.\n");
	if (c.StartWatching("python.exe")) 
		printf("WMI 생성에 실패했습니다.");
	
	getchar();
}
