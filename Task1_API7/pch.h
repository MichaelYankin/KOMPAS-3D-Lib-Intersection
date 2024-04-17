// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcmn.h>         // MFC support for Windows 95 Common Controls

#include <AFXDISP.H>
#include <COMUTIL.H>
#include <comdef.h>
#pragma warning( disable : 4192 )
#pragma warning( disable : 4278 )

#include "PARRAY.H"
#include <string>
#include <vector>

#ifndef __LHEAD3D_H
#include <LHead3d.h>
#endif

#import "ksConstants.tlb"   no_namespace named_guids
#import "ksConstants3D.tlb" no_namespace named_guids
#import "kAPI2D5COM.tlb"  no_namespace named_guids
#import "kAPI3D5COM.tlb"  no_namespace named_guids

#include <libtool.h>

#import "kAPI7.tlb" rename( "KompasAPI7", "ksAPI7" ) named_guids
#pragma warning( once : 4192 ) 
#pragma warning( once : 4278 )

#endif //PCH_H
