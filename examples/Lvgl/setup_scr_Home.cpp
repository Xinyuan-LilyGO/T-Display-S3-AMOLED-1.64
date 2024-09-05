#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"


void setup_scr_Home(lv_ui *ui)
{
	//Write codes Home
	ui->Home = lv_obj_create(NULL);
	lv_obj_set_size(ui->Home, 280, 456);
	lv_obj_set_scrollbar_mode(ui->Home, LV_SCROLLBAR_MODE_OFF);

	//Write style for Home, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->Home, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->Home, lv_color_hex(0x0b0025), LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes Home_arc_1
	ui->Home_arc_1 = lv_arc_create(ui->Home);
	lv_arc_set_mode(ui->Home_arc_1, LV_ARC_MODE_NORMAL);
	lv_arc_set_range(ui->Home_arc_1, 0, 100);
	lv_arc_set_bg_angles(ui->Home_arc_1, 0, 360);
	lv_arc_set_angles(ui->Home_arc_1, 0, 0);
	lv_arc_set_rotation(ui->Home_arc_1, 90);
	lv_obj_set_style_arc_rounded(ui->Home_arc_1, 0,  LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_rounded(ui->Home_arc_1, 0, LV_STATE_DEFAULT);
	lv_obj_set_pos(ui->Home_arc_1, 50, 126);
	lv_obj_set_size(ui->Home_arc_1, 180, 205);
	lv_obj_set_scrollbar_mode(ui->Home_arc_1, LV_SCROLLBAR_MODE_OFF);

	//Write style for Home_arc_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->Home_arc_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->Home_arc_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_width(ui->Home_arc_1, 12, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_color(ui->Home_arc_1, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->Home_arc_1, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->Home_arc_1, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->Home_arc_1, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->Home_arc_1, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->Home_arc_1, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->Home_arc_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for Home_arc_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_arc_width(ui->Home_arc_1, 12, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_arc_color(ui->Home_arc_1, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write style for Home_arc_1, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->Home_arc_1, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->Home_arc_1, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_all(ui->Home_arc_1, 5, LV_PART_KNOB|LV_STATE_DEFAULT);

	//Update current screen layout.
	lv_obj_update_layout(ui->Home);

	
}
