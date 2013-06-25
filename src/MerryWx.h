#ifndef _MERRY_WX_H_
#define _MERRY_WX_H_

#include <wx/wxprec.h>
#ifdef __BORLANDC__
	#pragma hdrstop
#endif
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif
#ifdef __WXMSW__
	#if (_DEBUG || _RELWITHDEBINFO)
		#define VLD_FORCE_ENABLE
		#include "vld.h"
	#endif
#endif
#if _DEBUG_CLEAR
#define __DEBUG_BEGIN(str)	wxMessageBox(__FUNCTION__##"__START\n"##str);
#define __DEBUG_END(str)	wxMessageBox(__FUNCTION__##"__END\n"##str);
#else
	#define __DEBUG_BEGIN(str)
	#define __DEBUG_END(str)
#endif
#endif