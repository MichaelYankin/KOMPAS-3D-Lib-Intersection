// Task1_API7.h : main header file for the Task1_API7 DLL
//

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

// AAA K10 #include <LibToolA/SelfText.h>
//------------------------------------------------------------------------------
// ��������������� �������
// ---
// ������ ��� �����
CString FullFileName(CString patch, CString fileName);
// ������ ��� �����
bool    GetFullName(char* inName, char* outName, int outNameLen);
// ��������� ������������� �����
bool    ExistFile(char* file);