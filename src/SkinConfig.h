#ifndef _SKIN_CONFIG_H_
#define _SKIN_CONFIG_H_

#include "MerryWx.h"
#include <wx/fileconf.h>

const char		ALMRUN_DEFAULT_TITLE[] = "ALMRun 快速启动工具";
const char		ALMRUN_DEFAULT_CONFIG_FILE[] = "LuaEx/ALMRun.lua";
const int		MERRY_DEFAULT_LIST_BOX_ITEM_MAX_NUM = 10;
#define SKIN_TYPE_ALPHA 1
#define SKIN_TYPE_MEDIUM 2
#define SKIN_TYPE_MINI 3
#define SKIN_TYPE_SMALL 4
typedef enum _skin_config
{
	SHOW_WINDOW,
	FONT_SIZE,
	FONT_BOLD,
	EDIT_CTRL_LEFT,
	EDIT_CTRL_TOP,
	EDIT_CTRL_WIDTH,
	EDIT_CTRL_HEIGHT,

	LIST_BOX_TOP,
	LIST_BOX_WIDTH,
	LIST_BOX_LEFT,
	LIST_FONT_SIZE,
	LIST_BORDER_STYLE,
	LIST_BORDER_WIDTH,
	LIST_ITEM_MARGIN,

	LISTFMT_NAME_MAX,
	LISTFMT_DESC_MAX,
	LISTFMT_KEY_MAX,
	LISTFMT_CMD_MAX,
	LISTFMT_ID_MAX,
	LISTFMT_NUM_MAX,

	TITLE_CONFIG,

	TITLE_LEFT = TITLE_CONFIG,
	TITLE_TOP,
	TITLE_WIDTH,
	TITLE_HEIGHT,
	TITLE_ENABLED,
	TITLE_ALIGN,

	LIST_ITEM_HEIGHT,
	LIST_ITEM_MAX,
	MAIN_WIDTH,
	MAIN_HEIGHT,
	SKIN_TYPE,
	SKIN_CONFIG_MAX,
} skin_config_t;

typedef enum _skin_color_config
{
	TEXT_COLOR,
	TEXT_BACKCOLOR,
	WINDOW_COLOR,
	LIST_FOCUSBG_COLOR,
	LIST_FOCUSTEXT_COLOR,
	LIST_BACKCOLOR,
	LIST_TEXT_COLOR,
	LIST_TITLE_COLOR,
	TITLE_TEXT_COLOR,
	BUTTONARROWCOLOR,
	BUTTONOUTLINECOLOR,
	BUTTONFILLCOLOR,
	COLOR_CONFIG_MAX,
} skin_color_t;

typedef enum _skin_custom_config
{
	SKINPICTURE,
	LISTPICTURE,
	DEFAULT_TITLE,
	LIST_FMT,
	LIST_FONT,
	SKIN_FONT,
	CUSTOM_CONFIG_MAX,
} skin_custom_t;

class SkinConfig
{
public:
	~SkinConfig();
	SkinConfig();
	int get(skin_config_t item) const;
	wxColor get(skin_color_t item) const;
	wxString get(skin_custom_t item) const;
private:
	void read_config();
	wxString skin_name;
	int config[SKIN_CONFIG_MAX];
	wxColor color[COLOR_CONFIG_MAX];
	wxString custom[CUSTOM_CONFIG_MAX];
};

#endif