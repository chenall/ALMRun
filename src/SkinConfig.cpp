#include "SkinConfig.h"

SkinConfig *skin;
SkinConfig::SkinConfig()
{
	memset(config,0,sizeof(config));
	config[EDIT_CTRL_WIDTH] = 260;
	config[EDIT_CTRL_HEIGHT] = 17;
	config[FONT_SIZE] = 11;
	config[LISTFMT_NAME_MAX] = -20;
	config[LISTFMT_DESC_MAX] = -20;
	config[LISTFMT_KEY_MAX] = 5;
	config[LISTFMT_CMD_MAX] = 0;
	config[LIST_ITEM_MARGIN] = 5;
	config[LISTFMT_ID_MAX] = 5;
	config[LISTFMT_NUM_MAX] = 2;
	config[TITLE_ALIGN] = -1;
	config[LIST_BORDER_STYLE] = 0;
	config[LIST_BORDER_WIDTH] = 1;
	//default custom config;
	custom[DEFAULT_TITLE] =wxT("ALMRun");
	custom[LIST_FMT] = "$$. $N";
	custom[SKIN_FONT]="ËÎÌו";
	skin_name = "merry";

	read_config();

	if (config[LIST_FONT_SIZE] <= 0)
		config[LIST_FONT_SIZE] = config[FONT_SIZE];

	config[LIST_ITEM_HEIGHT] = config[LIST_FONT_SIZE]+config[LIST_ITEM_MARGIN];

	if (!color[LIST_TEXT_COLOR].IsOk())
		color[LIST_TEXT_COLOR] = color[TEXT_COLOR];
	if (!color[LIST_FOCUSTEXT_COLOR].IsOk())
		color[LIST_FOCUSTEXT_COLOR].Set("#FFFFFF");

	if (!color[WINDOW_COLOR].IsOk())
		color[WINDOW_COLOR].Set("#e1ebf2");

	if (!color[LIST_BACKCOLOR].IsOk())
		color[LIST_BACKCOLOR] = wxTransparentColour;

	if (!wxImage::FindHandler(wxBITMAP_TYPE_JPEG))
		wxImage::AddHandler(new wxJPEGHandler);

	if (!wxImage::FindHandler(wxBITMAP_TYPE_BMP))
		wxImage::AddHandler(new wxBMPHandler);

	if (!wxImage::FindHandler(wxBITMAP_TYPE_PNG))
		wxImage::AddHandler(new wxPNGHandler);

	if (config[MAIN_HEIGHT] == 0)
		config[MAIN_HEIGHT] = config[LIST_BOX_TOP];

	if (custom[SKINPICTURE].empty())
	{
		wxString tmpname = wxT("skin/")+skin_name;
		if (wxFileExists(tmpname+".png"))
			custom[SKINPICTURE] = skin_name+".png";
		else if (wxFileExists(tmpname + ".jpg"))
			custom[SKINPICTURE] = skin_name+".jpg";
		else if (wxFileExists(tmpname + ".bmp"))
			custom[SKINPICTURE] = skin_name+".bmp";
	}

	if (custom[LISTPICTURE].empty())
	{
		wxString tmpname = wxT("skin/")+skin_name;
		if (wxFileExists(tmpname+"_listbg.png"))
			custom[LISTPICTURE] = skin_name+"_listbg.png";
		else if (wxFileExists(tmpname + "_listbg.jpg"))
			custom[LISTPICTURE] = skin_name+"_listbg.jpg";
		else if (wxFileExists(tmpname + "_listbg.bmp"))
			custom[LISTPICTURE] = skin_name+"_listbg.bmp";
	}

	if (!custom[SKINPICTURE].empty())
	{
		custom[SKINPICTURE].insert(0,"skin/");
		wxImage img(custom[SKINPICTURE]);
		if (config[MAIN_WIDTH] == 0)
			config[MAIN_WIDTH] = img.GetWidth()+1;
		if (config[MAIN_HEIGHT] == 0)
			config[MAIN_HEIGHT] = img.GetHeight()+1;
		if (config[EDIT_CTRL_TOP] == 0)
			config[EDIT_CTRL_TOP] = (img.GetHeight() - config[FONT_SIZE])/2 - 2;
		if (config[EDIT_CTRL_LEFT] == 0)
				config[EDIT_CTRL_LEFT] = 3;
	}

	if (!custom[LISTPICTURE].empty())
		custom[LISTPICTURE].insert(0,wxT("skin/"));

	if (config[MAIN_WIDTH] == 0)
		config[MAIN_WIDTH] = config[EDIT_CTRL_WIDTH] + config[EDIT_CTRL_LEFT];

	if (config[MAIN_HEIGHT] == 0)
		config[MAIN_HEIGHT] = config[LIST_BOX_TOP]?config[LIST_BOX_TOP]:config[EDIT_CTRL_HEIGHT];

	if (config[LIST_BOX_WIDTH] <=0 )
		config[LIST_BOX_WIDTH] = config[MAIN_WIDTH] - config[LIST_BOX_LEFT];

	config[LIST_BORDER_STYLE] += 100;
};

void SkinConfig::read_config()
{
	const char* skin_cfg_str[]={"SHOWWINDOW",
		"fontsize","fontbold",
		"editleft","edittop","editwidth","editheight",
		"listtop","listwidth","listleft","listfontsize","LIST_BORDER_STYLE","LIST_BORDER_WIDTH","listmargin","LISTFMT_NAME_MAX","LISTFMT_DESC_MAX","LISTFMT_KEY_MAX","LISTFMT_CMD_MAX",	"LISTFMT_ID_MAX","LISTFMT_NUM_MAX",
		"titleleft","titletop","titlewidth","titleheight"};	
	const char* skin_color_str[]={"TEXTCOLOR","TEXTBACKCOLOR","WINDOWCOLOR","LISTFOCUSBGCOLOR","listfocustextcolor","LISTBACKCOLOR","LISTTEXTCOLOR","LISTTITLECOLOR","titletextcolor","BUTTONARROWCOLOR","BUTTONOUTLINECOLOR","BUTTONFILLCOLOR"};
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
	skin_name = m_config->Read("/Config/skin",skin_name);
	wxDELETE(m_config);

	wxString path = Home + wxT("skin/") + skin_name;

	if (!path.EndsWith(".skn"))
		path += wxT(".skn");

	if (wxFileExists(path) == false)
		return;
	wxFileConfig *conf =  new wxFileConfig(wxT("ALMRun"),wxEmptyString,path,wxEmptyString, wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_GLOBAL_FILE | wxCONFIG_USE_RELATIVE_PATH |wxCONFIG_USE_NO_ESCAPE_CHARACTERS);
	conf->SetPath("skinvalues");
	custom[SKINPICTURE] = conf->Read("skinpicture");
	custom[LISTPICTURE] = conf->Read("listpicture");
	custom[LIST_FMT] = conf->Read("listfmt",custom[LIST_FMT]);
	custom[SKIN_FONT] = conf->Read("fontname",custom[SKIN_FONT]);
	custom[LIST_FONT] = conf->Read("listfontname",custom[SKIN_FONT]);
	config[TITLE_ENABLED] = conf->ReadLong("titleenabled",0);

	wxString skin_type = conf->Read("skintype");

	if (skin_type.IsEmpty())
		config[SKIN_TYPE] = 0;
	else if (skin_type.IsSameAs("alpha",FALSE))
		config[SKIN_TYPE] = SKIN_TYPE_ALPHA;
	else if (skin_type.IsSameAs("medium",false))
		config[SKIN_TYPE] = SKIN_TYPE_MEDIUM;
	else if (skin_type.IsSameAs("mini",false))
		config[SKIN_TYPE] = SKIN_TYPE_MINI;
	else if (skin_type.IsSameAs("small",false))
		config[SKIN_TYPE] = SKIN_TYPE_SMALL;

	i = (config[TITLE_ENABLED]?TITLE_ENABLED:TITLE_CONFIG);
	for(--i;i>=0;--i)
	{
		config[i] = conf->ReadLong(skin_cfg_str[i],config[i]);
	}

	for(i=0;i<COLOR_CONFIG_MAX;++i)
	{
		wxString col = conf->Read(skin_color_str[i]);
		if (col.IsEmpty())
			continue;
		if (col.IsNumber())
			color[i] = conf->ReadLong(skin_color_str[i],0);
		else
			color[i] = col;
	}

	//Title align config
	wxString title_align = conf->Read("titlealign");
	if (!title_align.IsEmpty())
	{
		if (title_align.IsSameAs("left",false))
			config[TITLE_ALIGN] = wxALIGN_LEFT;
		else if (title_align.IsSameAs("right",false))
			config[TITLE_ALIGN] = wxALIGN_RIGHT;
		else if (title_align.IsSameAs("top",false))
			config[TITLE_ALIGN] = wxALIGN_TOP;
		else if (title_align.IsSameAs("bottom",false))
			config[TITLE_ALIGN] = wxALIGN_BOTTOM;
		else if (title_align.IsSameAs("center",false))
			config[TITLE_ALIGN] = wxALIGN_CENTER;
		else if (title_align.IsSameAs("CENTER_VERTICAL",false))
			config[TITLE_ALIGN] = wxALIGN_CENTER_VERTICAL;
		else if (title_align.IsSameAs("CENTER_HORIZONTAL",false))
			config[TITLE_ALIGN] = wxALIGN_CENTER_HORIZONTAL;
	}

	wxDELETE(conf);
}

int SkinConfig::get(skin_config_t item) const
{
	if (item<=SKIN_CONFIG_MAX)
		return config[item];
	return 0;
}

wxColor SkinConfig::get(skin_color_t item) const
{
	if (item<=COLOR_CONFIG_MAX)
	{
		return color[item].IsOk()?color[item]:wxColor(0x111111);
	}
	return 1;
}

wxString SkinConfig::get(skin_custom_t item) const
{
	return custom[item];
}

SkinConfig::~SkinConfig()
{
}
