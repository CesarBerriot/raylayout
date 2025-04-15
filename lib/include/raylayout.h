#include <raylib.h>
#include <raygui.h>

enum gui_layout_axis
{	RGL_AXIS_VERTICAL,
	RGL_AXIS_HORIZONTAL,
	RGL_AXIS_MAX
};

enum gui_layout_padding
{	RGL_PADDING_NONE = 0,
	RGL_PADDING_SMALL = 5,
	RGL_PADDING_MEDIUM = 20,
	RGL_PADDING_LARGE = 50
};

void GuiLayoutBeginFrame(float gui_scale, enum gui_layout_padding outer_padding);
void GuiLayout(enum gui_layout_axis, enum gui_layout_padding outer_padding, enum gui_layout_padding inner_padding, int length, ...);
void GuiLayoutSpacer(void);
Rectangle GuiLayoutNext(void);
void GuiLayoutEndFrame(void);
