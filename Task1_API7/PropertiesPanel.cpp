#include "pch.h"
#include "PropertiesPanel.h"
#include "resource.h"
#include "Task1_API7.h"

//-------------------------------------------------------------------------------
// EXTENSION Приложение
// ---
extern AFX_EXTENSION_MODULE StepDLL;

extern ksAPI7::IApplicationPtr newKompasAPI; // Интерфейс Приложения Компас
extern ksAPI7::IViewsPtr GetViewsPtr();


// Массив именованных точек
std::vector<reference> Points;

// Массив объектов
PArray<ksAPI7::IDrawingObjectPtr> ObjArray;
// Список точек
ksAPI7::IPropertyListPtr PointsListCopy;

//-------------------------------------------------------------------------------------
// Конструктор
//---
PropertiesPanel::PropertiesPanel()
	: PropertyManagerObject()
	, x(0)
	, y(0)
{
}

// Выбор объектов вручную
bool           ManualMode = true;

//Преобразование VARIANT к PArray
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
				// Добавляем комментарии в коллекцию
				for (long i = 0, count = var.parray->rgsabound[0].cElements; i < count; ++i)
					dispArr.Add(new T(pvar[i]));

				::SafeArrayUnaccessData(var.parray);
			}
		}
	}
}

//Получаем массив точек из VARIANT*
void CopyVariantToVector(const VARIANT& v, std::vector<double>& arr)
{
	if (v.vt == (VT_ARRAY | VT_R8) && v.parray)
	{
		//массив должен быть одномерный
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
		else if (v.vt == (VT_ARRAY | VT_VARIANT)) // Для Python-а
		{
			//массив должен быть одномерный
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
// Изменение значения контрола (списка точек)
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
// Запуск процесса Selection
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
// Завершение процесса
//---
BOOL PropertiesPanel::EndProcess()
{
	BOOL res = PropertyManagerObject::EndProcess();
	return res;
}

//-------------------------------------------------------------------------------
// Наполнить панель контролами
// ---
void PropertiesPanel::ShowControls()
{

	//	Режим
	ksAPI7::IPropertyMultiButtonPtr buttons(CreateMultiButton(ksRadioButton));
	if (buttons)
	{
		InitPropertyControl(buttons, ID_CALLPANEL, TRUE, ksNameAlwaysVisible);
		// Кнопка Авто поиск (во всем документе)
		AddButton2(buttons, ID_AUTO, IDF_AUTO, false, true);
		// Кнопка Ручной поиск
		AddButton2(buttons, ID_MANUAL, IDF_MANUAL, true, true);
	}

	ksAPI7::IPropertyTextButtonPtr ExecuteButton = curentCollection->Add(ksControlTextButton);
	if (ExecuteButton) {
		InitPropertyControl(ExecuteButton, ID_EXECUTE, TRUE, ksNameAlwaysVisible);
		ExecuteButton->PutName(_bstr_t("Найти"));
	}

	// Сепаратор
	ksAPI7::IPropertySeparatorPtr sep(CreateSeparator(ksSeparatorDownName));
	if (sep)
	{
		sep->Name = ::LoadStr(ID_SEPARATOR);
	}
		

	// Комбобокс полученных точек
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
// Отписаться от событий процесса
//---
void PropertiesPanel::UnadviseEvents()
{
	PropertyManagerObject::UnadviseEvents();
}

//-------------------------------------------------------------------------------------
// Нажатие на кнопку
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
// Считать параметры элемета
//---
int PropertiesPanel::_GetMacroParam(void* value, unsigned int size)
{
	return ::GetMacroParam(0, value, size);
}

//--------------------------------------------------------------------------
// По индексу свойства получить идентификатор
//---
int PropertiesPanel::GetPropertyID(int index)
{
	int res = 0;
	return res;
}

void PropertiesPanel::SelectDocObject(double x, double y) {

	// Выбор объектов вручную
	if (ManualMode) {

		// Указатель на активный документ
		ksAPI7::IKompasDocument2D1Ptr doc2D(newKompasAPI->ActiveDocument);
		// Указатель на документ
		ksAPI7::IKompasDocumentPtr doc(newKompasAPI->ActiveDocument);
		ksAPI7::IKompasDocument1Ptr doc1(doc);

		// Настройка параметров поиска
		ksAPI7::IFindObjectParametersPtr Params;
		Params = doc1->GetInterface(ksObjectFindObjectParameters);
		Params->PutDrawingObjectType(ksAllObj);

		// Поиск объекта по клику
		ksAPI7::IDrawingObjectPtr pFoundObject = doc2D->FindObject(x, y, 2, Params);
		if (pFoundObject != NULL && pFoundObject->GetDrawingObjectType() != ksDrArc) {

			// Поиск объекта в массиве
			unsigned int index = 0;
			bool found = false;
			
			while (!found && index < ObjArray.Count()) {
				if (pFoundObject == ObjArray[index])
					found = true;
				if (!found)
					index++;
			}
			
			// Если объект уже добавлен - удаляем, иначе добавляем
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

	// Очищаем список точек
	PointsListCopy->ClearList();

	// Очищаем массив точек
	for (byte i = 0; i < Points.size(); i++)
		LightObj(Points[i], 0);
	Points.clear();

	// Проверка есть ли выделенные объекты для поиска
	if (ObjArray.Count() < 2) {
		Message("Недостаточно объектов для поиска пересечений.");
		PointsListCopy->PutVisible(false);
	}
	else {

		for (byte i = 0; i < ObjArray.Count() - 1; i++)
			LightObj(reference(ObjArray[i]), 0);

		// Убираем подсветку выделенных объектов
		for (unsigned int i = 0; i < ObjArray.Count(); i++) {
			ksAPI7::IDrawingObjectPtr pCurve = *ObjArray[i];
			LightObj(pCurve->GetReference(), 0);
		}

		// Расставляем точки пересечений
		for (byte i = 0; i < ObjArray.Count(); i++) {

			// Рассматриваем i-ю кривую 
			ksAPI7::IDrawingObject1Ptr pCurveOutside(*ObjArray[i]);

			// Попарно сравниваем i-ю кривую с последующими
			for (byte k = i; k < ObjArray.Count(); k++) {

				// Рассматриваем k-ю кривую
				ksAPI7::IDrawingObject1Ptr pCurveInside(*ObjArray[k]);

				// Массивы для точек пересечения
				VARIANT VarPointsArray;
				std::vector<double> PointsArray;

				// Если получены обе кривые
				if (pCurveInside->GetCurve2D() != NULL && pCurveOutside->GetCurve2D() != NULL) {

					// Расставляем точки пересечений i-й и k-й кривой
					VarPointsArray = pCurveOutside->GetCurve2D()->Intersect(pCurveInside->GetCurve2D());
					CopyVariantToVector(VarPointsArray, PointsArray);

					for (byte j = 0; j < PointsArray.size() / 2; j++) {

						// Ставим точку пересечения
						reference tmp = Point(PointsArray[2 * j], PointsArray[2 * j + 1], 1);
						Points.push_back(tmp);

						// Добавляем точку в список точек
						PointsListCopy->Add(_variant_t(Points.size()));

					} // for (byte j = 0...
				}
			} // for (byte k...

		} // for (byte i = 0...

		// Если точек пересечения нет
		if (Points.size() == 0) {
			Message("Не обнаружено точек пересечения.");
		}
		else {
			// Устанавливаем новое значение списка точек
			PointsListCopy->SetCurrentByIndex(0);
			PointsListCopy->Visible = true;
			// Подсвечиваем первую точку
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

	// Убираем подсветку выделенных объектов
	for (unsigned int i = 0; i < ObjArray.Count(); i++) {
		ksAPI7::IDrawingObjectPtr pCurve = *ObjArray[i];
		LightObj(pCurve->GetReference(), 0);
	}

	ObjArray.Flush();

	//Получить указатель на интерфейс IViews
	ksAPI7::IViewsPtr pViews = GetViewsPtr();
	if (!(bool)pViews)
		exit;

	//Получить указатель на интерфейс вида
	//Свойство ActiveView возвращает указатель на активный вид
	ksAPI7::IViewPtr pView(pViews->ActiveView);

	//Получить указатель на интерфейс IDrawingContainer
	ksAPI7::IDrawingContainerPtr pDrwContainer(pView);
	//Временный массив со всеми графическими объектами
	PArray<ksAPI7::IDrawingObjectPtr> tmpObjArray;
	FillDispArray(pDrwContainer->Objects[NULL], tmpObjArray);

	//Удаляем дуги из массива (создаем новый массив):
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

	// Убираем подсветку выделенных объектов
	for (unsigned int i = 0; i < ObjArray.Count(); i++) {
		ksAPI7::IDrawingObjectPtr pCurve = *ObjArray[i];
		LightObj(pCurve->GetReference(), 0);
	}

	ManualMode = true;

}