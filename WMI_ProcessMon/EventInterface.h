#ifndef __PSW_INTERFACE_EVT__
#define __PSW_INTERFACE_EVT__
/*
    力格:   Interface 急攫
    累己磊: 冠己肯(adsloader@naver.com)
    累己老: 2012.05.25
    格利 :  Event Interface
*/

class NotificationInterface
{
public:
   
    // callback 包访
    virtual void OnCreate() = 0;    
	virtual void OnDelete() = 0;     

};
 

#endif