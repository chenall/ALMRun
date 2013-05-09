#pragma once
#ifndef _ALMRUN_CONFIG_H_
#define _ALMRUN_CONFIG_H_
#include "MerryWx.h"

class ALMRunConfig
{
public:
	int CompareMode;
	wxString Explorer;
	wxString Root;
	void get(const wxString& name);
	bool set(const wxString& name,const wxString& value);
	bool set(const wxString& name,const int value);
	ALMRunConfig(void);
	virtual ~ALMRunConfig(void);
};
extern ALMRunConfig* g_config;
#endif