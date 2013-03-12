/*
 * org.tizen.taskmgr
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */




#include <appcore-efl.h>

#include "taskmanager.h"
#include "_util_log.h"
#include "_util_efl.h"
#include "_logic.h"

Evas_Object *_add_window(const char *name)
{
	Evas_Object *eo;
	int w, h;

	eo = elm_win_add(NULL, name, ELM_WIN_BASIC);
	if (eo) {
		elm_win_title_set(eo, name);
		elm_win_borderless_set(eo, EINA_TRUE);
		ecore_x_window_size_get(ecore_x_window_root_first_get(),
					&w, &h);
		evas_object_resize(eo, w, h);
	}

	return eo;
}

Evas_Object *_add_bg(Evas_Object *parent, char *style)
{
	Evas_Object *bg;

	bg = elm_bg_add(parent);
	retvm_if(bg == NULL, NULL, "Failed to add bg\n");
	if (style)	elm_object_style_set(bg, style);
	evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND,
					 EVAS_HINT_EXPAND);
	evas_object_show(bg);
	return bg;
}

Evas_Object *_add_genlist(Evas_Object *parent)
{
	Evas_Object *eo;

	eo = elm_genlist_add(parent);
	if (eo == NULL) {
		printf("[Error] Cannot add genlist\n");
		return NULL;
	}

	evas_object_size_hint_weight_set(eo, EVAS_HINT_EXPAND,
					 EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(eo, EVAS_HINT_FILL, EVAS_HINT_FILL);

	return eo;
}

Evas_Object *_add_icon(Evas_Object *parent, const char *png)
{
	Evas_Object *eo;
	char buf[128] = { 0, };

	eo = elm_icon_add(parent);
	if (eo == NULL) {
		printf("[Error] Cannot add button\n");
		return NULL;
	}

	snprintf(buf, sizeof(buf), "%s/%s", IMAGEDIR, png);
	elm_image_file_set(eo, buf, NULL);
	elm_image_resizable_set(eo, 1, 1);
	evas_object_size_hint_aspect_set(eo, EVAS_ASPECT_CONTROL_VERTICAL, 1,
					 1);

	return eo;
}

Evas_Object *_add_layout(Evas_Object *parent, const char *file,
			      const char *group)
{
	Evas_Object *eo = NULL;
	int r;

	eo = elm_layout_add(parent);
	if (eo == NULL) {
		printf("[Error] Cannot add layout\n");
		return NULL;
	}

	r = elm_layout_file_set(eo, file, group);
	if (!r) {
		printf("[Error] Cannot set file layout\n");
		evas_object_del(eo);
		return NULL;
	}

	evas_object_size_hint_weight_set(eo, EVAS_HINT_EXPAND,
					 EVAS_HINT_EXPAND);

	return eo;
}

Evas_Object *_add_ctxpopup(Evas_Object *parent)
{
	Evas_Object *eo = NULL;

	eo = elm_ctxpopup_add(parent);
	if (eo == NULL) {
		printf("[Error] Cannot add ctxpopup\n");
		return NULL;
	}

	elm_ctxpopup_horizontal_set(eo, EINA_TRUE);
	elm_ctxpopup_direction_priority_set(eo,
					    ELM_CTXPOPUP_DIRECTION_DOWN,
					    ELM_CTXPOPUP_DIRECTION_UP,
					    ELM_CTXPOPUP_DIRECTION_LEFT,
					    ELM_CTXPOPUP_DIRECTION_RIGHT);

	return eo;
}

Evas_Object *_add_label(Evas_Object *parent, const char *msg)
{
	Evas_Object *eo = NULL;

	eo = elm_label_add(parent);
	if (eo == NULL) {
		printf("[Error] Cannot add label\n");
		return NULL;
	}

	elm_label_line_wrap_set(eo, ELM_WRAP_WORD);
	elm_object_text_set(eo, msg);

	return eo;
}

static Eina_Bool _disappear_popup(void *data)
{
	Evas_Object *eo = (Evas_Object *)data;
	if (eo == NULL) {
		printf("[Error] Invalid argument: popup is NULL\n");
		return ECORE_CALLBACK_CANCEL;
	}
	evas_object_del(eo);
	return ECORE_CALLBACK_CANCEL;
}

Evas_Object *_add_popup_ask(Evas_Object *parent, char *text, void *data)
{
	Evas_Object *pu, *bt1, *bt2;
	retvm_if(parent == NULL, NULL, "Invalid argument: parent is NULL\n");

	pu = elm_popup_add(parent);
	retvm_if(pu == NULL, NULL, "Falied to add popup\n");
	evas_object_size_hint_weight_set(pu, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_text_set(pu, "title,text", S_("IDS_COM_POP_WARNING"));
	elm_object_text_set(pu, text);
	evas_object_show(pu);

	bt1 = elm_button_add(pu);
	elm_object_text_set(bt1, S_("IDS_COM_SK_OK"));
	elm_object_part_content_set(pu, "button1", bt1);
	evas_object_smart_callback_add(bt1, "clicked", _ok_response_cb, data);

	bt2 = elm_button_add(pu);
	elm_object_text_set(bt2, S_("IDS_COM_POP_CANCEL"));
	elm_object_part_content_set(pu, "button2", bt2);
	evas_object_smart_callback_add(bt2, "clicked", _cancel_response_cb, data);


	return pu;
}

void util_show_popup_with_message(Evas_Object *parent, double in,
				  const char *msg)
{
	Evas_Object *eo = NULL;

	eo = elm_popup_add(parent);
	if (eo == NULL) {
		printf("[Error] Cannot add popup\n");
		return;
	}

	evas_object_size_hint_weight_set(eo, EVAS_HINT_EXPAND,
					 EVAS_HINT_EXPAND);
	elm_object_text_set(eo, msg);

	ecore_timer_add(in, _disappear_popup, eo);
}

Evas_Object *_add_naviframe(Evas_Object *parent)
{
	Evas_Object *nv;

	retv_if(parent == NULL, NULL);

	nv = elm_naviframe_add(parent);
	retvm_if(nv == NULL, NULL, "Failed to add naviframe\n");
	elm_object_part_content_set(parent, "elm.swallow.content", nv);

	evas_object_show(nv);

	return nv;
}

Evas_Object *_add_layout_main(Evas_Object *parent,
		Eina_Bool content, Eina_Bool transparent)
{
	Evas_Object *ly;

	retv_if(parent == NULL, NULL);

	ly = elm_layout_add(parent);
	retvm_if(ly == NULL, NULL, "Failed elm_layout_add.\n");

	elm_layout_theme_set(ly, "layout", "application", "default");
	evas_object_size_hint_weight_set(ly,
		EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(parent, ly);
	if (content)
		elm_object_signal_emit(ly, "elm,state,show,content", "elm");
	if (transparent)
		elm_object_signal_emit(ly, "elm,bg,show,transparent", "elm");
	evas_object_show(ly);
	return ly;
}

Evas_Object *_add_progressbar(Evas_Object *parent, const char *style,
		Evas_Coord w, Evas_Coord h)
{
	Evas_Object *pb;
	double scale;

	retvm_if(parent == NULL, NULL, "Invalid argument: parent is NULL\n");

	scale = elm_config_scale_get();

	pb = elm_progressbar_add(parent);
	retvm_if(pb == NULL, NULL, "Failed to add progressbar\n");

	elm_object_style_set(pb, style);
	evas_object_resize(pb, w, (int)(60 * scale));
	evas_object_move(pb, 0, h / 2);
	elm_progressbar_pulse(pb, EINA_TRUE);
	evas_object_show(pb);

	return pb;
}


