#ifndef __UI_AUDIO_POPUP_H
#define __UI_AUDIO_POPUP_H

#define SLIDER_COUNT	3

struct SLIDER_STATE {

	uchar	values[SLIDER_COUNT];
	uchar	ranges[SLIDER_COUNT];
	uchar	active_id;

} SLIDER_STATE;

void ui_audio_popup_create(void);
void ui_audio_popup_destroy(void);

#endif
