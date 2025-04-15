#include <stdlib.h>
#include <string.h>
#include <raylayout.h>
#include <raygui/styles/style_dark.h>
#include <raygui/styles/style_bluish.h>
#include <raygui/styles/style_candy.h>
#include <raygui/styles/style_cherry.h>
#include <raygui/styles/style_cyber.h>
#include <raygui/styles/style_jungle.h>
#include <raygui/styles/style_lavanda.h>
#include <raygui/styles/style_terminal.h>
#include <raygui/styles/style_sunny.h>
#include <raygui/styles/style_ashes.h>
#include <raygui/styles/style_enefete.h>

enum { default_gui_scale = 2 };

static int gui_scale = default_gui_scale;

static void initialize_window(void);
static void run_window(void);
static void render_gui_scale_editor(void);
static void render_next_style_button(void);
static void render_button_grid(void);

int main(void)
{	initialize_window();
	run_window();
	return EXIT_SUCCESS;
}

static void initialize_window(void)
{	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(700, 350, "RayLayout Demo");
	SetWindowMinSize(400, 200);
	SetTargetFPS(20);
	GuiLoadStyleCyber();
}

static void run_window(void)
{	while(!WindowShouldClose())
	{	BeginDrawing();
			ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
			GuiLayoutBeginFrame(gui_scale, RGL_PADDING_MEDIUM);
				GuiLayout(RGL_AXIS_VERTICAL, RGL_PADDING_NONE, RGL_PADDING_MEDIUM, 3, 1, 2, 6);
					render_gui_scale_editor();
					render_next_style_button();
					render_button_grid();
			GuiLayoutEndFrame();
		EndDrawing();
	}
	CloseWindow();
}

static void render_gui_scale_editor(void)
{	GuiLayout(RGL_AXIS_HORIZONTAL, RGL_PADDING_NONE, RGL_PADDING_MEDIUM, 2, 1, 4);
		GuiLabel(GuiLayoutNext(), "GUI Scale");

		static int value = default_gui_scale;
		static int edit_mode;

		Rectangle value_box_bounds = GuiLayoutNext();

		if
		(	edit_mode &&
			(	!CheckCollisionPointRec(GetMousePosition(), value_box_bounds) ||
				IsKeyPressed(KEY_ENTER) ||
				IsKeyPressed(KEY_KP_ENTER)
			)
		)
			edit_mode = false;

		if(GuiValueBox(value_box_bounds, NULL, &value, 1, 5, edit_mode))
			edit_mode = true;

		if(!edit_mode)
			gui_scale = value;
}

static void render_next_style_button(void)
{	void(*styles[])(void) =
	{	GuiLoadStyleCyber,
		GuiLoadStyleJungle,
		GuiLoadStyleLavanda,
		GuiLoadStyleTerminal,
		GuiLoadStyleSunny,
		GuiLoadStyleAshes,
		GuiLoadStyleEnefete,
		GuiLoadStyleDefault,
		GuiLoadStyleDark,
		GuiLoadStyleBluish,
		GuiLoadStyleCandy,
		GuiLoadStyleCherry
	};

	static int style;

	if(GuiButton(GuiLayoutNext(), "Next Style"))
	{	++style;
		if(style >= sizeof(styles) / sizeof(styles[0]))
			style = 0;
		styles[style]();
	}
}

static void render_button_grid(void)
{	GuiLayout(RGL_AXIS_VERTICAL, RGL_PADDING_NONE, RGL_PADDING_MEDIUM, 2, 3, 2);
		GuiLayout(RGL_AXIS_HORIZONTAL, RGL_PADDING_NONE, RGL_PADDING_MEDIUM, 2, 1, 1);
			GuiButton(GuiLayoutNext(), "Button1");
			GuiButton(GuiLayoutNext(), "Button2");
		GuiLayout(RGL_AXIS_HORIZONTAL, RGL_PADDING_NONE, RGL_PADDING_NONE, 4, 3, 2, 7, 1);
			GuiButton(GuiLayoutNext(), "Button3");
			GuiLayoutSpacer();
			GuiButton(GuiLayoutNext(), "Button4");
			GuiLayoutSpacer();
}
