#pragma once
#include "cPropMen.h"
#include "pch.h"
#include "PARRAY.H"

class PropertiesPanel :
    public PropertyManagerObject
{
    ksAPI7::IProcessPtr        process;
    ksAPI7::IProcess2DPtr      m_process2D;

protected:

    double         x;
    double         y;

public:

    PropertiesPanel();

    // ��������� ������� ��������������
    virtual BOOL EndProcess();
    virtual void UnadviseEvents();

    virtual bool OnButtonClick(long buttonID);

    virtual bool IsButtonEnable(long buttonID) { return true; } // ������ �� ������

    //prChangeControlValue - ��������� �������� ��������
    virtual bool OnChangeControlValue(long ctrlID, const VARIANT& newVal);

    // ����������� ������� ���������� ��������� ��������
    virtual void ShowControls();

    // �������� ������� ##################################################################
    // ���� �������     ##################################################################
    virtual int      GetPropertyID(int  index);

    void SelectionProcess();
    void SelectDocObject(double x, double y);
    void Execute();
    void Auto();
    void Manual();

    virtual int        _GetMacroParam(void* value, unsigned int size);

};