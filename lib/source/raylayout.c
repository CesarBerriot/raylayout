#include "raylayout.h"
#include <malloc.h>
#include <hard_assert.h>
#include <rlgl.h>

struct layout_stack_item
{	struct layout_stack_item * ascendant, * descendant;
	Rectangle bounds;
	enum gui_layout_padding outer_padding, inner_padding;
	enum gui_layout_axis axis;
	int length;
	int i;
	int sizes[];
};

struct layout_transform { float offset, size; };

static Vector2 window_size;
static float gui_scale;
static struct layout_stack_item * cursor;

static struct layout_transform compute_transform(int index);
static float compute_normalized_padding(enum gui_layout_padding);
static int sum_layout_sizes(void);

void GuiLayoutBeginFrame(float gui_scale_argument, enum gui_layout_padding outer_padding)
{	gui_scale = gui_scale_argument;

	window_size = (Vector2){ GetScreenWidth(), GetScreenHeight() };

	rlPushMatrix();
	rlScalef(gui_scale, gui_scale, 0);

	SetMouseScale(1 / gui_scale, 1 / gui_scale);

	cursor = malloc(sizeof(struct layout_stack_item) + sizeof(int));
	*cursor = (struct layout_stack_item)
	{	.bounds = { 0, 0, window_size.x / gui_scale, window_size.y / gui_scale },
		.outer_padding = outer_padding,
		.length = 1
	};
	cursor->sizes[0] = 1;

	GuiLayout(RGL_AXIS_HORIZONTAL, outer_padding, RGL_PADDING_NONE, 1, 1);
}

void GuiLayout(enum gui_layout_axis axis, enum gui_layout_padding outer_padding, enum gui_layout_padding inner_padding, int length, ...)
{	struct layout_stack_item * new = malloc(sizeof(struct layout_stack_item) + sizeof(int) * length);
	*new = (struct layout_stack_item)
	{	.bounds = GuiLayoutNext(),
		.outer_padding = outer_padding,
		.inner_padding = inner_padding,
		.axis = axis,
		.length = length
	};

	new->ascendant = cursor;

	va_list sizes;
	va_start(sizes, length);
	for(int i = 0; i < length; ++i)
		new->sizes[i] = va_arg(sizes, int);
	va_end(sizes);

	cursor->descendant = new;
	cursor = new;
}

void GuiLayoutSpacer(void)
{	GuiLayoutNext();
}

Rectangle GuiLayoutNext(void)
{	if(cursor->i >= cursor->length)
	{	struct layout_stack_item * ascendant = cursor->ascendant;
		ascendant->descendant = NULL;
		free(cursor);
		cursor = ascendant;
	}

	/*int sizes_total_until_current = 0;
	for(int i = 0; i < cursor->i; ++i)
		sizes_total_until_current += cursor->sizes[i];

	int sizes_total = 0;
	for(int i = 0; i < cursor->length; ++i)
		sizes_total += cursor->sizes[i];

	float passed_size = cursor->sizes[cursor->i];

	float position = (float)sizes_total_until_current / sizes_total;
	float size = passed_size / sizes_total;


	float layout_position = !cursor->axis ? cursor->bounds.y : cursor->bounds.x;
	float layout_size = !cursor->axis ? cursor->bounds.height : cursor->bounds.width;

	float final_size = size * layout_size;
	float final_position = layout_position + position * layout_size;

	int padding = 5;
	int padding_total = padding * (cursor->length + 1);
	int reduced_layout_size = layout_size - padding_total;
	final_size = final_size * reduced_layout_size / layout_size;
	final_position = final_position * reduced_layout_size / layout_size + padding * (cursor->i + 1);

	++cursor->i;

	return (Rectangle)
		{	.x = !cursor->axis ? cursor->bounds.x : final_position,
			.y = !cursor->axis ? final_position : cursor->bounds.y,
			.width = !cursor->axis ? cursor->bounds.width : final_size,
			.height = !cursor->axis ? final_size : cursor->bounds.height
		};
	*/

	struct layout_transform normalized_transform = compute_transform(cursor->i);

	++cursor->i;

	struct layout_transform layout_transform = (struct layout_transform[RGL_AXIS_MAX])
	{	[RGL_AXIS_VERTICAL] = { .offset = cursor->bounds.y, .size = cursor->bounds.height },
		[RGL_AXIS_HORIZONTAL] = { .offset = cursor->bounds.x, .size = cursor->bounds.width },
	}[cursor->axis];

	struct layout_transform final_transform =
	{	.offset = layout_transform.offset + normalized_transform.offset * layout_transform.size,
		.size = normalized_transform.size * layout_transform.size
	};

	return (Rectangle[RGL_AXIS_MAX])
	{	[RGL_AXIS_VERTICAL] = { cursor->bounds.x, final_transform.offset, cursor->bounds.width, final_transform.size },
		[RGL_AXIS_HORIZONTAL] = { final_transform.offset, cursor->bounds.y, final_transform.size, cursor->bounds.height }
	}[cursor->axis];
}

void GuiLayoutEndFrame(void)
{	rlPopMatrix();

	struct layout_stack_item * item;
	for(item = cursor; item->descendant; item = item->descendant);

	while(item->ascendant)
	{	struct layout_stack_item * ascendant = item->ascendant;
		free(item);
		item = ascendant;
	}

	free(item);
	cursor = NULL;
}

static struct layout_transform compute_transform(int index)
{	struct layout_transform transform = { 0 };

	float inner_padding = compute_normalized_padding(cursor->inner_padding);
	float outer_padding = compute_normalized_padding(cursor->outer_padding);

	float padding = index ? inner_padding : outer_padding;

	float total_outer_padding = outer_padding * 2;
	float total_inner_padding = inner_padding * (cursor->length > 1 ? cursor->length - 1 : 0);
	float total_padding = total_outer_padding + total_inner_padding;

	int full_layout_size = sum_layout_sizes();
	float layout_size = full_layout_size - total_padding;

	if(index)
	{	struct layout_transform previous_transform = compute_transform(index - 1);
		transform.offset = (previous_transform.offset + previous_transform.size) * full_layout_size;
	}

	transform.offset += padding;
	transform.size = layout_size * cursor->sizes[index] / full_layout_size;

	return (struct layout_transform){ .offset = transform.offset / full_layout_size, .size = transform.size / full_layout_size };
}

static float compute_normalized_padding(enum gui_layout_padding padding)
{	return
		sum_layout_sizes() * padding /
		(float[RGL_AXIS_MAX])
		{	[RGL_AXIS_VERTICAL] = cursor->bounds.height,
			[RGL_AXIS_HORIZONTAL] = cursor->bounds.width
		}[cursor->axis] / gui_scale;
}

static int sum_layout_sizes(void)
{	int result = 0;
	for(int i = 0; i < cursor->length; ++i)
		result += cursor->sizes[i];
	return result;
}
