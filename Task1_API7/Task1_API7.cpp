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
// ����������� ��������� ������������ � ������� ������������� DLL
// ---
static AFX_EXTENSION_MODULE DocCollectionDLL = { NULL, NULL };
HINSTANCE g_hInstance = NULL;

ksAPI7::IApplicationPtr newKompasAPI(NULL);		   // ���������� Kompas API7
PropertiesPanel* Panel;

void OnProcessDetach();                            // ���������� ����������

//-------------------------------------------------------------------------------
// ����������� ����� �����
// ������������� � ���������� DLL
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
// �������� ������ � ������ API
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

			HINSTANCE hAppAuto = LoadLibrary(filename); // ������������� kAPI7.dll

			if (hAppAuto)
			{
				// ��������� �� ������� ������������ ��������� KompasApplication  
				typedef LPDISPATCH(WINAPI* FCreateKompasApplication)();

				FCreateKompasApplication pCreateKompasApplication =
					(FCreateKompasApplication)GetProcAddress(hAppAuto, "CreateKompasApplication");

				if (pCreateKompasApplication)
					newKompasAPI = IDispatchPtr(pCreateKompasApplication(), false /*AddRef*/); // �������� ��������� Application
				FreeLibrary(hAppAuto);
			}
		}
	}
}

//-------------------------------------------------------------------------------
// ������ ������������� ��������
// ---
unsigned int WINAPI LIBRARYID()
{
	return IDR_LIBID;
}

//-------------------------------------------------------------------------------
// ���������� ��� ����������
// ---
char* WINAPI LIBRARYNAME()
{
	return "����������� ������";
}

//-------------------------------------------------------------------------------
// �������� ������� ����������
// ---
void WINAPI LIBRARYENTRY(unsigned int comm)
{
	
	GetNewKompasAPI();
	
	if (ksGetCurrentDocument(1)) // �������� �������� 2D ��������
	{
		Panel = new PropertiesPanel();
		Panel->InitProcessParam(ID_PANEL, pnEnterEscHelp, ID_FIRSTTAB);
		Panel->SelectionProcess();
	}
}

//-------------------------------------------------------------------------------
// ���������� ����������
// ---
void OnProcessDetach()
{
	newKompasAPI = NULL;
}

//-------------------------------------------------------------------------------
// �������� ��������� �� ��������� ��������� ����� (COM)
// ---
ksAPI7::IViewsPtr GetViewsPtr()
{
	ksAPI7::IViewsPtr res;

	if (newKompasAPI)
	{
		//�������� ��������� �� ��������� ��������� ������
		ksAPI7::IKompasDocument2DPtr pKompasDocument2D;
		pKompasDocument2D = newKompasAPI->ActiveDocument;

		if ((bool)pKompasDocument2D)
		{
			//�������� ��������� �� ��������� ��������� ����� � ����
			ksAPI7::IViewsAndLayersManagerPtr pViewsAndLayersManager;
			pViewsAndLayersManager = pKompasDocument2D->GetViewsAndLayersManager();

			if (pViewsAndLayersManager)
				res = pViewsAndLayersManager->GetViews();
		}
	}
	return res;
}

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
