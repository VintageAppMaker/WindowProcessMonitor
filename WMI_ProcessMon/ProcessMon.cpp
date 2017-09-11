#include "StdAfx.h"
#include "ProcessMon.h"
#include "eventsink.h"


CProcessMon::CProcessMon(void)
{
    
}

CProcessMon::~CProcessMon(void)
{
}

int CProcessMon::StartWatching(char* szName)
{
	ZeroMemory(m_szProcessName, 1024);
	strcpy(m_szProcessName, szName);

    hres =  CoInitializeEx(0, COINIT_MULTITHREADED); 
    if (FAILED(hres))
    {
        return 1;                  // Program has failed.
    }


    hres =  CoInitializeSecurity(
        NULL, 
        -1,                          // COM negotiates service
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
        );

                      
    if (FAILED(hres))
    {
        CoUninitialize();
        return 1;                      // Program has failed.
    }

    pLoc = NULL;

    hres = CoCreateInstance(
        CLSID_WbemLocator,             
        0, 
        CLSCTX_INPROC_SERVER, 
        IID_IWbemLocator, (LPVOID *) &pLoc);
 
    if (FAILED(hres))
    {
        CoUninitialize();
        return 1;                 // Program has failed.
    }

    pSvc = NULL;
	
    hres = pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"), 
        NULL,
        NULL, 
        0, 
        NULL, 
        0, 
        0, 
        &pSvc
    );
	    
    if (FAILED(hres))
    {
        pLoc->Release();     
        CoUninitialize();
        return 1;                // Program has failed.
    }
    
    hres = CoSetProxyBlanket(
        pSvc,                        // Indicates the proxy to set
        RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx 
        RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx 
        NULL,                        // Server principal name 
        RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
        RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
        NULL,                        // client identity
        EOAC_NONE                    // proxy capabilities 
    );

    if (FAILED(hres))
    {
        pSvc->Release();
        pLoc->Release();     
        CoUninitialize();
        return 1;               // Program has failed.
    }

    // Use an unsecured apartment for security
    pUnsecApp = NULL;

    hres = CoCreateInstance(CLSID_UnsecuredApartment, NULL, 
        CLSCTX_LOCAL_SERVER, IID_IUnsecuredApartment, 
        (void**)&pUnsecApp);
 
	// 2012.05.25 추가 callback을 위해 interface를 상속한 지금의 클래스를 넘긴다.
	pSink = new EventSink(this, EventSink::CRE );
    pSink->AddRef();

    pStubUnk = NULL; 
    pUnsecApp->CreateObjectStub(pSink, &pStubUnk);

    pStubSink = NULL;
    pStubUnk->QueryInterface(IID_IWbemObjectSink,
        (void **) &pStubSink);

	char* szFormat = 
        "SELECT * " 
        "FROM __InstanceCreationEvent WITHIN 1 "
        "WHERE TargetInstance ISA 'Win32_Process' and TargetInstance.Name = '%s'";

	char szQuery[1024];
	ZeroMemory(szQuery, 1024);

	sprintf(szQuery, szFormat, szName);

    hres = pSvc->ExecNotificationQueryAsync(
        _bstr_t("WQL"), 
        _bstr_t(szQuery), 
        WBEM_FLAG_SEND_STATUS, 
        NULL, 
        pStubSink);

    if (FAILED(hres))
    {
        pSvc->Release();
        pLoc->Release();
        pUnsecApp->Release();
        pStubUnk->Release();
        pSink->Release();
        pStubSink->Release();
        CoUninitialize();    
        return 1;
    }

	pSink2 = new EventSink(this, EventSink::DEL);
    pSink2->AddRef();
	pUnsecApp->CreateObjectStub(pSink2, &pStubUnk);

	pStubSink = NULL;
    pStubUnk->QueryInterface(IID_IWbemObjectSink,
        (void **) &pStubSink);

	char* szFormat2 = 
        "SELECT * " 
        "FROM __InstanceDeletionEvent WITHIN 1 "
        "WHERE TargetInstance ISA 'Win32_Process' and TargetInstance.Name = '%s'";

	ZeroMemory(szQuery, 1024);
	sprintf(szQuery, szFormat2, szName);

	hres = pSvc->ExecNotificationQueryAsync(
        _bstr_t("WQL"), 
        _bstr_t(szQuery), 
        WBEM_FLAG_SEND_STATUS, 
        NULL, 
        pStubSink);

	
    if (FAILED(hres))
    {
        pSvc->Release();
        pLoc->Release();
        pUnsecApp->Release();
        pStubUnk->Release();
        pSink->Release();
        pStubSink->Release();
        CoUninitialize();    
        return 1;
    }
   
    return 0;   // Program successfully completed.
}

int CProcessMon::StopWatching()
{
	try{
		hres = pSvc->CancelAsyncCall(pStubSink);
    
        pSvc->Release();
        pLoc->Release();
        pUnsecApp->Release();
        pStubUnk->Release();
        pSink->Release();
		pSink2->Release();
        pStubSink->Release();
        CoUninitialize();

	} catch(...){
        return -1;
	}

	return 0;
}