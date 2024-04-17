/////////////////////////////////////////////////////////////////////////////
//
// EventsList - Список подписчиков
// ABaseEvent - Базовый класс для обработчиков событий в автоматизации
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
// Базовый класс для автоматизационных обработчиков событий
//
/////////////////////////////////////////////////////////////////////////////
class ABaseEvent : public CCmdTarget
{
protected:
  static CObList&   m_EventList;  // Список обработчиков событий
  DWORD             m_dwCookie;   // Идентификатор соединения
  IID                m_iidEvents;  // IID интерфейса событий
  LPCONNECTIONPOINT  m_pConnPt;    // Соединение 
  LPUNKNOWN         m_pContainer; // Источник событий
          
public:
           // Конструктор
           ABaseEvent( LPUNKNOWN pObject,
                             IID iidEvents );
           // Деструктор
  virtual ~ABaseEvent();
  
  int          Advise  ();        // Подписаться на получение событий
  void         Unadvise();        // Отписаться от получения событий
  void         RemoveThis();      // Удалить себя из списка   
  
  void         Disconnect();      // Отсоединиться
  virtual void Clear();  

  static  void DestroyList();                          // Удалить список      
  static  void TerminateEvents( void );                // Отписать все события
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
