/////////////////////////////////////////////////////////////////////////////
//
// EventsList - ������ �����������
// ABaseEvent - ������� ����� ��� ������������ ������� � �������������
// 
////////////////////////////////////////////////////////////////////////////
#ifndef _ABASEEVENT_H
#define _ABASEEVENT_H

#ifndef __LDEFIN2D_H
#include <ldefin2d.h>
#endif       
   
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
//
// ������� ����� ��� ����������������� ������������ �������
//
/////////////////////////////////////////////////////////////////////////////
class ABaseEvent : public CCmdTarget
{
protected:
  static CObList&   m_EventList;  // ������ ������������ �������
  DWORD             m_dwCookie;   // ������������� ����������
  IID                m_iidEvents;  // IID ���������� �������
  LPCONNECTIONPOINT  m_pConnPt;    // ���������� 
  LPUNKNOWN         m_pContainer; // �������� �������
          
public:
           // �����������
           ABaseEvent( LPUNKNOWN pObject,
                             IID iidEvents );
           // ����������
  virtual ~ABaseEvent();
  
  int          Advise  ();        // ����������� �� ��������� �������
  void         Unadvise();        // ���������� �� ��������� �������
  void         RemoveThis();      // ������� ���� �� ������   
  
  void         Disconnect();      // �������������
  virtual void Clear();  

  static  void DestroyList();                          // ������� ������      
  static  void TerminateEvents( void );                // �������� ��� �������
  static  void TerminateEvents( LPUNKNOWN container );

protected:
  BEGIN_INTERFACE_PART(EventHandler, IDispatch)
    INIT_INTERFACE_PART(BaseEvent, EventHandler)
    STDMETHOD(GetTypeInfoCount)(unsigned int*);
    STDMETHOD(GetTypeInfo)(unsigned int, LCID, ITypeInfo**);
    STDMETHOD(GetIDsOfNames)( REFIID, LPOLESTR*, unsigned int, LCID, DISPID*);
    STDMETHOD(Invoke)(DISPID, REFIID, LCID, unsigned short, DISPPARAMS*,
               VARIANT*, EXCEPINFO*, unsigned int*);
  END_INTERFACE_PART(EventHandler)

  DECLARE_INTERFACE_MAP()
  DECLARE_EVENTSINK_MAP()
};

////////////////////////////////////////////////////////////////////////////////
#endif 
