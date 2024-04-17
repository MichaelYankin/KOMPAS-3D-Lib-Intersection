// Task1_API7.cpp : Defines the initialization routines for the DLL.
//

#include "pch.h"
#include "Task1_API7.h"
#include <afxdllx.h>

#include "PropertiesPanel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class LpSelfText;

//-------------------------------------------------------------------------------
// Специальная структура используемая в течении инициализации DLL
// ---
static AFX_EXTENSION_MODULE DocCollectionDLL = { NULL, NULL };
HINSTANCE g_hInstance = NULL;

ksAPI7::IApplicationPtr newKompasAPI(NULL);		   // Приложение Kompas API7
PropertiesPanel* Panel;

void OnProcessDetach();                            // Отключение библиотеки

//-------------------------------------------------------------------------------
// Стандартная точка входа
// Инициализация и завершение DLL
// ---
extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	g_hInstance = hInstance;

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("DOCCOLLECTION.AWX Initializing!\n");

		AfxInitExtensionModule(DocCollectionDLL, hInstance);

		new CDynLinkLibrary(DocCollectionDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		
		TRACE0("DOCCOLLECTION.AWX Terminating!\n");
		OnProcessDetach();
		AfxTermExtensionModule(DocCollectionDLL);

	}
	return 1;   // ok
}


//-------------------------------------------------------------------------------
// Получить доступ к новому API
// ---
void GetNewKompasAPI()
{
	if (!(ksAPI7::IApplication*)newKompasAPI)
	{
		CString filename;

		if (::GetModuleFileName(NULL, filename.GetBuffer(255), 255))
		{
			filename.ReleaseBuffer(255);
			CString libname;

			libname = "kAPI7.dll";    // kAPI7.dll
			filename.Replace(filename.Right(filename.GetLength() - (filename.ReverseFind(_T('\\')) + 1)), libname);

			HINSTANCE hAppAuto = LoadLibrary(filename); // идентификатор kAPI7.dll

			if (hAppAuto)
			{
				// Указатель на функцию возвращающую интерфейс KompasApplication  
				typedef LPDISPATCH(WINAPI* FCreateKompasApplication)();

				FCreateKompasApplication pCreateKompasApplication =
					(FCreateKompasApplication)GetProcAddress(hAppAuto, "CreateKompasApplication");

				if (pCreateKompasApplication)
					newKompasAPI = IDispatchPtr(pCreateKompasApplication(), false /*AddRef*/); // Получаем интерфейс Application
				FreeLibrary(hAppAuto);
			}
		}
	}
}

//-------------------------------------------------------------------------------
// Задать идентификатор ресурсов
// ---
unsigned int WINAPI LIBRARYID()
{
	return IDR_LIBID;
}

//-------------------------------------------------------------------------------
// Определить имя библиотеки
// ---
char* WINAPI LIBRARYNAME()
{
	return "Пересечение кривых";
}

//-------------------------------------------------------------------------------
// Головная функция библиотеки
// ---
void WINAPI LIBRARYENTRY(unsigned int comm)
{
	
	GetNewKompasAPI();
	
	if (ksGetCurrentDocument(1)) // Получить активный 2D документ
	{
		Panel = new PropertiesPanel();
		Panel->InitProcessParam(ID_PANEL, pnEnterEscHelp, ID_FIRSTTAB);
		Panel->SelectionProcess();
	}
}

//-------------------------------------------------------------------------------
// Завершение приложения
// ---
void OnProcessDetach()
{
	newKompasAPI = NULL;
}

//-------------------------------------------------------------------------------
// Получить указатель на интерфейс коллекции видов (COM)
// ---
ksAPI7::IViewsPtr GetViewsPtr()
{
	ksAPI7::IViewsPtr res;

	if (newKompasAPI)
	{
		//Получить указатель на интерфейс документа Компас
		ksAPI7::IKompasDocument2DPtr pKompasDocument2D;
		pKompasDocument2D = newKompasAPI->ActiveDocument;

		if ((bool)pKompasDocument2D)
		{
			//Получить указатель на интерфейс менеджера видов и слоёв
			ksAPI7::IViewsAndLayersManagerPtr pViewsAndLayersManager;
			pViewsAndLayersManager = pKompasDocument2D->GetViewsAndLayersManager();

			if (pViewsAndLayersManager)
				res = pViewsAndLayersManager->GetViews();
		}
	}
	return res;
}

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
