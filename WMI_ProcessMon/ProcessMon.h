#pragma once
/*
    제목:   WMI를 이용한 Process 감시
    작성자: 박성완(adsloader@naver.com)
    작성일: 2012.05.25
    목적 :  MSDN의 WMI 예제 활용
	참고 :  MSDN의 예제를 class로 편하게 고쳐사용함
*/

#include "eventsink.h"
#include "EventInterface.h"

class CProcessMon : public NotificationInterface
{
protected:
	char m_szProcessName[1024];

public:
	CProcessMon(void);
	virtual ~CProcessMon(void);

	int StartWatching(char* szName);
	int StopWatching();

	virtual void OnCreate() = 0;    
	virtual void OnDelete() = 0;    

private:
	HRESULT hres;
	IWbemLocator *pLoc;
	IWbemServices *pSvc;
	IUnsecuredApartment* pUnsecApp;
	
	EventSink* pSink;
	EventSink* pSink2;

	IUnknown* pStubUnk;
	IWbemObjectSink* pStubSink;
};
