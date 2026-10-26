// Compatibility shim: many legacy .rc files include <afxres.h> which is only
// available with the MFC resource headers.  The Windows SDK <winres.h> provides
// everything those resource scripts actually use (IDS_*, IDC_STATIC, ...).
#pragma once
#include <winres.h>
#ifndef IDC_STATIC
#define IDC_STATIC (-1)
#endif
