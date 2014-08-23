#ifndef _MERRY_WX_H_
#define _MERRY_WX_H_

#include <wx/wxprec.h>
#include "MerryError.h"
#ifdef __BORLANDC__
	#pragma hdrstop
#endif
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif
/*
#ifdef __WXMSW__
	#if (_DEBUG || _RELWITHDEBINFO)
		#define VLD_FORCE_ENABLE
		#include "vld.h"
	#endif
#endif
	*/
#if _DEBUG_LOG
#include <wx/log.h>
#define __DEBUG_BEGIN(str)	wxLogMessage("%s START %s",__FUNCTION__,str);
#define __DEBUG_END(str)	wxLogMessage("%s END %s",__FUNCTION__,str);
#else
	#define __DEBUG_BEGIN(str)
	#define __DEBUG_END(str)
#endif
#endif