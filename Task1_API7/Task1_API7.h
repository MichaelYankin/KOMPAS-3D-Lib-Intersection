// Task1_API7.h : main header file for the Task1_API7 DLL
//

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

// AAA K10 #include <LibToolA/SelfText.h>
//------------------------------------------------------------------------------
// Вспомогательные функции
// ---
// Полное имя файла
CString FullFileName(CString patch, CString fileName);
// Полное имя файла
bool    GetFullName(char* inName, char* outName, int outNameLen);
// Проверить существование файла
bool    ExistFile(char* file);