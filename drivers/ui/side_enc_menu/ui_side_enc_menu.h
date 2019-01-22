#ifndef __UI_SIDE_ENC_MENU_H
#define __UI_SIDE_ENC_MENU_H

#define SLIDER_COUNT	3

struct SLIDER_STATE {

	uchar	values[SLIDER_COUNT];
	uchar	ranges[SLIDER_COUNT];
	uchar	active_id;

} SLIDER_STATE;

void ui_side_enc_menu_create(void);
void ui_side_enc_menu_destroy(void);

#endif
