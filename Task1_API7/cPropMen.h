////////////////////////////////////////////////////////////////////////////////
//
// PropertyManagerObject - Класс для обслуживания панели свойств 
// PropertyManagerEvent  - Класс событий менеджера свойств 
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _CPROPMEN_H
#define _CPROPMEN_H

#ifndef _ABASEEVENT_H
#include "abaseEvent.h"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//----------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
//
// Класс для обслуживания панели свойств 
//
// ctrlID используется как идентивикатор контрола и как идентификатор для загрузки 
// ресурсов контрола
//
////////////////////////////////////////////////////////////////////////////////
class PropertyManagerObject {
protected:                        
  int                             flagMode;         // Признак редактирования по дабл-клику
  ksAPI7::IProcessParamPtr        procParam;        // Параметры процесса          
  ksAPI7::IPropertyTabsPtr        propTabs;         // Закладки
  ksAPI7::IPropertyControlsPtr    curentCollection; // Контролы текущей закладки панели свойств 
  ksAPI7::IPropertyGridPtr        paramGrid;        // Грид для вывода дополнительных параметров элемента 
  long                            rowIndex;         // Индекс строки
public:

           PropertyManagerObject();
  virtual ~PropertyManagerObject();
// Менеджер свойств ##################################################################
    // События     
    // prButtonClick - Нажатие кнопки.
  virtual bool OnButtonClick( long buttonID ) { return false; }
    
    //prChangeControlValue - Изменение значения контрола
  virtual bool OnChangeControlValue( long ctrlID, const VARIANT& newVal ) { return true; }

   // Инициализация параметров процесса 
   bool InitProcessParam( long toolBarID, SpecPropertyToolBarEnum toolBarType, long firstTabID = 0 );

   // ksPlacementChanged    -Изменено положения объекта
   virtual BOOL  PlacementChanged(double x, double y, double angle, BOOL dynamic) { return FALSE; };

   virtual void SelectDocObject(double x, double y) { };

    // Добавление закладки
    bool CreateTab( long tabID, BOOL visible = TRUE, BOOL active = FALSE ); 

  // Добавление комбобокса целочисленных значений 
  ksAPI7::IPropertyListPtr               CreateList       ();
  // Добавление сепаратора 
  ksAPI7::IPropertySeparatorPtr          CreateSeparator  ( SeparatorTypeEnum type );
  // Добавление CheckBox-а 
  ksAPI7::IPropertyCheckBoxPtr           CreateCheckBox   ( bool checked );
  // Добавление группы кнопок 
  ksAPI7::IPropertyMultiButtonPtr        CreateMultiButton( ButtonTypeEnum type );

  // Получить указатель на Control по его идентификатору (для изменения параметров контрола)
  ksAPI7::IPropertyControlPtr  GetPropertyControl( int ctrlID );

  // Добавление кнопки в группу
          void AddButton(ksAPI7::IPropertyMultiButtonPtr& buttons, long btnID, bool cheched = false,  bool enable = true );
  // Добавление кнопки в группу
          void AddButton2(ksAPI7::IPropertyMultiButtonPtr& buttons, long btnID, long bmpID, bool cheched = false,  bool enable = true );

  // Изменить состояние контрола
          void SetControlEnable( long ctrlID, bool enabled = true ); 

  // Накачка контрола общими параметрами
  void InitPropertyControl(ksAPI7::IPropertyControl* control, long ctrlID, UINT hint = 0, UINT tips = 0,
                            BOOL enable = TRUE, PropertyControlNameVisibility nameVisibility = ksNameAlwaysVisible, 
                            BOOL visible = TRUE );

  // Виртуальная функция наполнения контролов элемента
  virtual void  ShowControls()  {}  
  // процесс создания эл-та
  virtual void  Draw1() {  };

  // Количество параметров выводимых в гриде 
  virtual int   ParamCount() { return 0; }
  // Заполнение грида параметров 
  virtual void  ShowParam()  {}
  // Функции для заполнения грида параметров        
  void  AddStringToGrig( long paramID, LPCTSTR  value   ); // Добавление строкового значения
  void  AddDoubleToGrig( long paramID, double   value   ); // Добавление числового  значения
  
  // Завершить процесс редактирования
          virtual BOOL  EndProcess();
          virtual void  UnadviseEvents();
// Менеджер свойств ##################################################################
          virtual reference  GetPreviewGroup() { return 0; }

};


////////////////////////////////////////////////////////////////////////////////
//
// PropertyManagerEvent - обрабокчик событий от менеджера панели свойств
//
////////////////////////////////////////////////////////////////////////////////
class PropertyManagerEvent : public ABaseEvent
{

protected:
  PropertyManagerObject&  obj;           // Указатель на подписанный объект
  // Attributes
public:
  PropertyManagerEvent( LPDISPATCH manager, PropertyManagerObject& _obj );           
  virtual ~PropertyManagerEvent();

// Operations
public:

  // prButtonClick        - Нажатие кнопки.  "Создался OCX контрол."
  afx_msg BOOL ButtonClick( long buttonID );

  // prControlCommand     - Нажатие кнопки контрола
  afx_msg BOOL ControlCommand(LPDISPATCH  ctrl, long buttonID);

  // prChangeControlValue - "Удаляется OCX контрол."
  afx_msg BOOL ChangeControlValue( LPDISPATCH ctrl );

  // prButtonUpdate        - Установка состояния кнопки спецпанели.
  afx_msg BOOL ButtonUpdate ( long buttonID, long* check, VARIANT_BOOL* _enable);

// prProcessActivate    - Начало процесса  
  afx_msg BOOL ProcessActivate();

// prProcessDeactivate  - Завершение процесса
  afx_msg BOOL ProcessDeactivate();

  DECLARE_EVENTSINK_MAP()
};

// Освободить интерфейсы
void App7NULL();
/////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
// _2_ Process2DEvent - обработчик событий от процесса 2D
//
////////////////////////////////////////////////////////////////////////////////
class Process2DEvent : public ABaseEvent
{

protected:
  PropertyManagerObject& m_obj;           // Указатель на подписанный объект
  bool                m_ownObj;
  // Attributes
public:
  Process2DEvent(const ksAPI7::IProcess2DPtr& process, PropertyManagerObject& _obj, bool ownObj );
  virtual ~Process2DEvent();

  // Operations
public:

  // ksPlacementChanged   - Изменено положения объекта
  afx_msg BOOL PlacementChanged(double x, double y, double angle, BOOL dynamic);
  // ksStop               - Остановка процесса
  afx_msg BOOL Stop();
  // ksProcessActivate    - Начало процесса  
  afx_msg BOOL ProcessActivate();
  // ksProcessDeactivate  - Завершение процесса
  afx_msg BOOL ProcessDeactivate();
  // ksProcessDeactivate  - Завершение процесса
  afx_msg BOOL Run();
  // ksEndProcess         - Окончание процесса.
  afx_msg BOOL EndProcess();
  DECLARE_EVENTSINK_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


LPTSTR LoadStr(int id);

#endif 
