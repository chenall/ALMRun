#include "SkinConfig.h"

SkinConfig *skin;
SkinConfig::SkinConfig()
{
	memset(config,0,sizeof(config));
	config[EDIT_CTRL_WIDTH] = 260;
	config[EDIT_CTRL_HEIGHT] = 17;
	config[FONT_SIZE] = 11;

	//default custom config;
	custom[DEFAULT_TITLE] =wxT("ALMRun");

	read_config();

	if (config[LIST_FONT_SIZE] <= 0)
		config[LIST_FONT_SIZE] = config[FONT_SIZE];

	config[LIST_ITEM_HEIGHT] = config[LIST_FONT_SIZE]*2;

	if (!color[LIST_TEXT_COLOR].IsOk())
		color[LIST_TEXT_COLOR] = color[TEXT_COLOR];
	if (!color[LIST_FOCUSTEXT_COLOR].IsOk())
		color[LIST_FOCUSTEXT_COLOR].Set("#FFFFFF");

	if (!wxImage::FindHandler(wxBITMAP_TYPE_JPEG))
		wxImage::AddHandler(new wxJPEGHandler);

	if (!wxImage::FindHandler(wxBITMAP_TYPE_BMP))
		wxImage::AddHandler(new wxBMPHandler);

	if (!wxImage::FindHandler(wxBITMAP_TYPE_PNG))
		wxImage::AddHandler(new wxPNGHandler);

	if (!custom[SKINPICTURE].empty())
	{
		custom[SKINPICTURE].insert(0,"skin/");
		wxImage img(custom[SKINPICTURE]);
		config[MAIN_WIDTH] = img.GetWidth()+1;
		config[MAIN_HEIGHT] = img.GetHeight()+2;
		if (config[EDIT_CTRL_TOP] == 0)
			config[EDIT_CTRL_TOP] = (img.GetHeight() - config[FONT_SIZE])/2 - 2;
		if (config[EDIT_CTRL_LEFT] == 0)
				config[EDIT_CTRL_LEFT] = 3;
	}

	if (!custom[LISTPICTURE].empty())
		custom[LISTPICTURE].insert(0,wxT("skin/"));

	if (config[MAIN_WIDTH] == 0)
		config[MAIN_WIDTH] = config[EDIT_CTRL_WIDTH];
	if (config[MAIN_HEIGHT] == 0)
		config[MAIN_HEIGHT] = config[EDIT_CTRL_HEIGHT] + 2;

	if (config[LIST_BOX_WIDTH] <=0 )
		config[LIST_BOX_WIDTH] = config[MAIN_WIDTH];
};

void SkinConfig::read_config()
{
	const char* skin_cfg_str[]={"SHOWWINDOW",
		"fontsize","fontbold",
		"editleft","edittop","editwidth","editheight",
		"listtop","listwidth","listleft","listfontsize",
		"titleleft","titletop","titlewidth","titleheight","titlealigh"};	
	const char* skin_color_str[]={"TEXTCOLOR","TEXTBACKCOLOR","WINDOWCOLOR","LISTFOCUSBGCOLOR","listfocustextcolor","LISTTEXTCOLOR","LISTTITLECOLOR","TITLE_EXTCOLOR","BUTTONARROWCOLOR","BUTTONOUTLINECOLOR","BUTTONFILLCOLOR"};
	wxString Home;
	wxString config_file;
	int i;

	if (wxGetEnv(wxT("ALMRUN_HOME"),&Home))
		config_file = Home + wxT("config/ALMRun.ini");
	else
		config_file = ::wxGetCwd() + wxT("./config/ALMRun.ini");
	wxFileConfig *m_config =  new wxFileConfig(wxT("ALMRun"),wxEmptyString,config_file,wxEmptyString,7|wxCONFIG_USE_NO_ESCAPE_CHARACTERS);
	if (!m_config)
		return;
	wxString path = m_config->Read("/GUI/skin");
	wxDELETE(m_config);
	if (path.empty())
		return;
	path = Home + wxT("skin/") + path + wxT(".skn");
	if (wxFileExists(path) == false)
		return;
	wxFileConfig *conf =  new wxFileConfig(wxT("ALMRun"),wxEmptyString,path,wxEmptyString, wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_GLOBAL_FILE | wxCONFIG_USE_RELATIVE_PATH |wxCONFIG_USE_NO_ESCAPE_CHARACTERS);
	conf->SetPath("skinvalues");
	custom[SKINPICTURE] = conf->Read("skinpicture");
	custom[LISTPICTURE] = conf->Read("listpicture");
	custom[SKIN_TYPE] = conf->Read("skintype");

	config[TITLE_ENABLED] = conf->ReadLong("titleenable",0);
	
	i = (config[TITLE_ENABLED] == 1?TITLE_ENABLED:TITLE_CONFIG);
	for(--i;i>=0;--i)
	{
		config[i] = conf->ReadLong(skin_cfg_str[i],config[i]);
	}

	for(i=0;i<COLOR_CONFIG_MAX;++i)
	{
		color[i] = wxColor(conf->Read(skin_color_str[i]));
	}
	wxDELETE(conf);
}

int SkinConfig::get(skin_config_t item) const
{
	return config[item];
}

wxColor SkinConfig::get(skin_color_t item) const
{
	return color[item];
}

wxString SkinConfig::get(skin_custom_t item) const
{
	return custom[item];
}

SkinConfig::~SkinConfig()
{
}
