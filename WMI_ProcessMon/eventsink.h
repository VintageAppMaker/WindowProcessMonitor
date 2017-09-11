// EventSink.h
#ifndef EVENTSINK_H
#define EVENTSINK_H

#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>
#include "EventInterface.h"

# pragma comment(lib, "wbemuuid.lib")


class EventSink : public IWbemObjectSink
{
    LONG m_lRef;
    bool bDone;
	int  m_state; // 0: create, 1: delete
	
public:

	enum state{
        CRE = 0, 
        DEL     
	};
	
    EventSink::EventSink() { m_lRef = 0; }
    EventSink(NotificationInterface* i, int state);
   ~EventSink() { bDone = true; }

    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();        
    virtual HRESULT 
        STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv);

    virtual HRESULT STDMETHODCALLTYPE Indicate( 
            LONG lObjectCount,
            IWbemClassObject __RPC_FAR *__RPC_FAR *apObjArray
            );
        
    virtual HRESULT STDMETHODCALLTYPE SetStatus( 
            /* [in] */ LONG lFlags,
            /* [in] */ HRESULT hResult,
            /* [in] */ BSTR strParam,
            /* [in] */ IWbemClassObject __RPC_FAR *pObjParam
            );

	NotificationInterface* m_pInterface;

};

#endif    // end of EventSink.h