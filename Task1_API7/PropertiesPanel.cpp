#include "pch.h"
#include "PropertiesPanel.h"
#include "resource.h"
#include "Task1_API7.h"

//-------------------------------------------------------------------------------
// EXTENSION ����������
// ---
extern AFX_EXTENSION_MODULE StepDLL;

extern ksAPI7::IApplicationPtr newKompasAPI; // ��������� ���������� ������
extern ksAPI7::IViewsPtr GetViewsPtr();


// ������ ����������� �����
std::vector<reference> Points;

// ������ ��������
PArray<ksAPI7::IDrawingObjectPtr> ObjArray;
// ������ �����
ksAPI7::IPropertyListPtr PointsListCopy;

//-------------------------------------------------------------------------------------
// �����������
//---
PropertiesPanel::PropertiesPanel()
	: PropertyManagerObject()
	, x(0)
	, y(0)
{
}

// ����� �������� �������
bool           ManualMode = true;

//�������������� VARIANT � PArray
template <class T>
void FillDispArray(const _variant_t& var, PArray<T>& dispArr)
{
	if (var.vt == VT_DISPATCH)
	{
		dispArr.Add(new T(var.pdispVal));
	}
	else
	{
		if (var.vt == (VT_ARRAY | VT_DISPATCH))
		{
			LPDISPATCH HUGEP* pvar = NULL;
			::SafeArrayAccessData(var.parray, (void HUGEP * FAR*) & pvar);
			if (pvar)
			{
				// ��������� ����������� � ���������
				for (long i = 0, count = var.parray->rgsabound[0].cElements; i < count; ++i)
					dispArr.Add(new T(pvar[i]));

				::SafeArrayUnaccessData(var.parray);
			}
		}
	}
}

//�������� ������ ����� �� VARIANT*
void CopyVariantToVector(const VARIANT& v, std::vector<double>& arr)
{
	if (v.vt == (VT_ARRAY | VT_R8) && v.parray)
	{
		//������ ������ ���� ����������
		int count = v.parray->rgsabound[0].cElements - v.parray->rgsabound[0].lLbound;

		if (v.parray->cDims == 1 && count)
		{
			HRESULT hr;
			double HUGEP* pvar;
			hr = ::SafeArrayAccessData(v.parray, reinterpret_cast<void**>(&pvar));

			if (!FAILED(hr) && pvar)
			{
				for (int i = 0; i < count; i++)
					arr.push_back(pvar[i]);

				hr = ::SafeArrayUnaccessData(v.parray);
			}
		}
	}
	else
	{
		if (v.vt == VT_R8)
		{
			arr.push_back(static_cast<double>(static_cast<const _variant_t&>(v)));
		}
		else if (v.vt == (VT_ARRAY | VT_VARIANT)) // ��� Python-�
		{
			//������ ������ ���� ����������
			int count = v.parray->rgsabound[0].cElements - v.parray->rgsabound[0].lLbound;

			if (v.parray->cDims == 1 && count)
			{
				HRESULT hr;
				_variant_t HUGEP* pvar;
				hr = ::SafeArrayAccessData(v.parray, reinterpret_cast<void HUGEP * FAR*>(&pvar));

				if (!FAILED(hr) && pvar)
				{
					for (int i = 0; i < count; i++)
						arr.push_back(pvar[i]);

					hr = ::SafeArrayUnaccessData(v.parray);
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------
// ��������� �������� �������� (������ �����)
//---

bool PropertiesPanel::OnChangeControlValue(long ctrlID, const VARIANT& newVal) {

	if (ctrlID == ID_POINTSLIST) {

		PointsListCopy->PutValue(newVal.lVal);

		for (byte i = 0; i < Points.size(); i++) {
			if (i + 1 == newVal.lVal) {
				LightObj(Points[i], 1);
			}
			else {
				LightObj(Points[i], 0);
			}
				
		}
	}
	return true;

}

//-------------------------------------------------------------------------------------
// ������ �������� Selection
//---
void PropertiesPanel::SelectionProcess()
{
	ksAPI7::IKompasDocument2D1Ptr doc2D(newKompasAPI->ActiveDocument);

	if (doc2D)
	{
		ksAPI7::IProcess2DPtr process2D;
		process2D = doc2D->LibProcess[ksProcess2DCursor];
		process = process2D;

		m_process2D = process2D;

		if (process)
		{
			process->ProcessParam = procParam;
			process2D->X = x;
			process2D->Y = y;
			process->ProcessParam = procParam;

			new Process2DEvent(process, *this, true);
			process->Run(false, false);
		}
	}
}

//---------------------------------------------------------------------------
// ���������� ��������
//---
BOOL PropertiesPanel::EndProcess()
{
	BOOL res = PropertyManagerObject::EndProcess();
	return res;
}

//-------------------------------------------------------------------------------
// ��������� ������ ����������
// ---
void PropertiesPanel::ShowControls()
{

	//	�����
	ksAPI7::IPropertyMultiButtonPtr buttons(CreateMultiButton(ksRadioButton));
	if (buttons)
	{
		InitPropertyControl(buttons, ID_CALLPANEL, TRUE, ksNameAlwaysVisible);
		// ������ ���� ����� (�� ���� ���������)
		AddButton2(buttons, ID_AUTO, IDF_AUTO, false, true);
		// ������ ������ �����
		AddButton2(buttons, ID_MANUAL, IDF_MANUAL, true, true);
	}

	ksAPI7::IPropertyTextButtonPtr ExecuteButton = curentCollection->Add(ksControlTextButton);
	if (ExecuteButton) {
		InitPropertyControl(ExecuteButton, ID_EXECUTE, TRUE, ksNameAlwaysVisible);
		ExecuteButton->PutName(_bstr_t("�����"));
	}

	// ���������
	ksAPI7::IPropertySeparatorPtr sep(CreateSeparator(ksSeparatorDownName));
	if (sep)
	{
		sep->Name = ::LoadStr(ID_SEPARATOR);
	}
		

	// ��������� ���������� �����
	ksAPI7::IPropertyListPtr PointsList(CreateList());
	if (PointsList)
	{
		InitPropertyControl(PointsList, ID_POINTSLIST);
		PointsListCopy = PointsList;
		PointsList->Visible = false;
		for (byte i = 0; i < Points.size(); i++) {
			PointsList->Add(_variant_t(Points[i]));
		}
		PointsList->Value = PointsList->GetMinValue();
	}

}


//---------------------------------------------------------------------------
// ���������� �� ������� ��������
//---
void PropertiesPanel::UnadviseEvents()
{
	PropertyManagerObject::UnadviseEvents();
}

//-------------------------------------------------------------------------------------
// ������� �� ������
//---

bool PropertiesPanel::OnButtonClick(long buttonID) {
	
	switch (buttonID)
	{
	case ID_MANUAL:
	{
		Manual();
		break;

	}
	case ID_AUTO:
	{
		Auto();
		break;
	}
	case ID_EXECUTE:
	{
		Execute();
		break;
	}
	}
	return true;
}

//--------------------------------------------------------------------------
// ������� ��������� �������
//---
int PropertiesPanel::_GetMacroParam(void* value, unsigned int size)
{
	return ::GetMacroParam(0, value, size);
}

//--------------------------------------------------------------------------
// �� ������� �������� �������� �������������
//---
int PropertiesPanel::GetPropertyID(int index)
{
	int res = 0;
	return res;
}

void PropertiesPanel::SelectDocObject(double x, double y) {

	// ����� �������� �������
	if (ManualMode) {

		// ��������� �� �������� ��������
		ksAPI7::IKompasDocument2D1Ptr doc2D(newKompasAPI->ActiveDocument);
		// ��������� �� ��������
		ksAPI7::IKompasDocumentPtr doc(newKompasAPI->ActiveDocument);
		ksAPI7::IKompasDocument1Ptr doc1(doc);

		// ��������� ���������� ������
		ksAPI7::IFindObjectParametersPtr Params;
		Params = doc1->GetInterface(ksObjectFindObjectParameters);
		Params->PutDrawingObjectType(ksAllObj);

		// ����� ������� �� �����
		ksAPI7::IDrawingObjectPtr pFoundObject = doc2D->FindObject(x, y, 2, Params);
		if (pFoundObject != NULL && pFoundObject->GetDrawingObjectType() != ksDrArc) {

			// ����� ������� � �������
			unsigned int index = 0;
			bool found = false;
			
			while (!found && index < ObjArray.Count()) {
				if (pFoundObject == ObjArray[index])
					found = true;
				if (!found)
					index++;
			}
			
			// ���� ������ ��� �������� - �������, ����� ���������
			if (found) {
				ObjArray.RemoveInd(index);
				LightObj(pFoundObject->GetReference(), 0);
			}
			else
			{
				ObjArray.Add(new ksAPI7::IDrawingObjectPtr(pFoundObject));
				LightObj(pFoundObject->Reference, 1);
			}

		} //if (pFoundObj != NULL

	} // if (ManualMode)

}

void PropertiesPanel::Execute() {

	// ������� ������ �����
	PointsListCopy->ClearList();

	// ������� ������ �����
	for (byte i = 0; i < Points.size(); i++)
		LightObj(Points[i], 0);
	Points.clear();

	// �������� ���� �� ���������� ������� ��� ������
	if (ObjArray.Count() < 2) {
		Message("������������ �������� ��� ������ �����������.");
		PointsListCopy->PutVisible(false);
	}
	else {

		for (byte i = 0; i < ObjArray.Count() - 1; i++)
			LightObj(reference(ObjArray[i]), 0);

		// ������� ��������� ���������� ��������
		for (unsigned int i = 0; i < ObjArray.Count(); i++) {
			ksAPI7::IDrawingObjectPtr pCurve = *ObjArray[i];
			LightObj(pCurve->GetReference(), 0);
		}

		// ����������� ����� �����������
		for (byte i = 0; i < ObjArray.Count(); i++) {

			// ������������� i-� ������ 
			ksAPI7::IDrawingObject1Ptr pCurveOutside(*ObjArray[i]);

			// ������� ���������� i-� ������ � ������������
			for (byte k = i; k < ObjArray.Count(); k++) {

				// ������������� k-� ������
				ksAPI7::IDrawingObject1Ptr pCurveInside(*ObjArray[k]);

				// ������� ��� ����� �����������
				VARIANT VarPointsArray;
				std::vector<double> PointsArray;

				// ���� �������� ��� ������
				if (pCurveInside->GetCurve2D() != NULL && pCurveOutside->GetCurve2D() != NULL) {

					// ����������� ����� ����������� i-� � k-� ������
					VarPointsArray = pCurveOutside->GetCurve2D()->Intersect(pCurveInside->GetCurve2D());
					CopyVariantToVector(VarPointsArray, PointsArray);

					for (byte j = 0; j < PointsArray.size() / 2; j++) {

						// ������ ����� �����������
						reference tmp = Point(PointsArray[2 * j], PointsArray[2 * j + 1], 1);
						Points.push_back(tmp);

						// ��������� ����� � ������ �����
						PointsListCopy->Add(_variant_t(Points.size()));

					} // for (byte j = 0...
				}
			} // for (byte k...

		} // for (byte i = 0...

		// ���� ����� ����������� ���
		if (Points.size() == 0) {
			Message("�� ���������� ����� �����������.");
		}
		else {
			// ������������� ����� �������� ������ �����
			PointsListCopy->SetCurrentByIndex(0);
			PointsListCopy->Visible = true;
			// ������������ ������ �����
			LightObj(Points[0], 1);
		}
		ObjArray.Flush();
	}

}

void PropertiesPanel::Auto() {

	ManualMode = false;

	Points.clear();
	PointsListCopy->ClearList();
	PointsListCopy->Visible = false;

	// ������� ��������� ���������� ��������
	for (unsigned int i = 0; i < ObjArray.Count(); i++) {
		ksAPI7::IDrawingObjectPtr pCurve = *ObjArray[i];
		LightObj(pCurve->GetReference(), 0);
	}

	ObjArray.Flush();

	//�������� ��������� �� ��������� IViews
	ksAPI7::IViewsPtr pViews = GetViewsPtr();
	if (!(bool)pViews)
		exit;

	//�������� ��������� �� ��������� ����
	//�������� ActiveView ���������� ��������� �� �������� ���
	ksAPI7::IViewPtr pView(pViews->ActiveView);

	//�������� ��������� �� ��������� IDrawingContainer
	ksAPI7::IDrawingContainerPtr pDrwContainer(pView);
	//��������� ������ �� ����� ������������ ���������
	PArray<ksAPI7::IDrawingObjectPtr> tmpObjArray;
	FillDispArray(pDrwContainer->Objects[NULL], tmpObjArray);

	//������� ���� �� ������� (������� ����� ������):
	for (unsigned int i = 0; i < tmpObjArray.Count(); i++) {

		ksAPI7::IDrawingObjectPtr Obj(*tmpObjArray[i]);
		if (Obj && Obj->GetDrawingObjectType() != ksDrArc) {
			ObjArray.Add(new ksAPI7::IDrawingObjectPtr(Obj));
			LightObj(Obj->GetReference(), 1);
		}

	}

}

void PropertiesPanel::Manual() {

	Points.clear();
	PointsListCopy->ClearList();
	PointsListCopy->Visible = false;

	// ������� ��������� ���������� ��������
	for (unsigned int i = 0; i < ObjArray.Count(); i++) {
		ksAPI7::IDrawingObjectPtr pCurve = *ObjArray[i];
		LightObj(pCurve->GetReference(), 0);
	}

	ManualMode = true;

}