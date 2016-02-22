/*
 *  Task Manager
 *
 * Copyright (c) 2000 - 2015 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <Elementary.h>
#include <app_manager.h>
#include <stdbool.h>
#include <rua.h>

#include "conf.h"
#include "item.h"
#include "list.h"
#include "log.h"
#include "main.h"
#include "scroller.h"
#include "util.h"

#define PRIVATE_DATA_KEY_CLEAR "p_clr"
#define PRIVATE_DATA_KEY_NEXT_ITEM "p_n_it"
#define PRIVATE_DATA_KEY_ITEM_INNER_FOR_MOVING "p_in_mv"
#define PRIVATE_DATA_KEY_ITEM_ANIM_FOR_MOVING "p_an_mv"
#define PRIVATE_DATA_KEY_ITEM_X "p_it_x"
#define PRIVATE_DATA_KEY_ITEM_Y "p_it_y"
#define PRIVATE_DATA_KEY_ITEM_W "p_it_w"
#define PRIVATE_DATA_KEY_ITEM_H "p_it_h"
#define PRIVATE_DATA_KEY_DOWN_X "p_dw_x"
#define PRIVATE_DATA_KEY_DOWN_Y "p_dw_y"
#define PRIVATE_DATA_KEY_DOWN_TIME "p_dw_t"
#define PRIVATE_DATA_KEY_SCROLLED "p_sc"
#define PRIVATE_DATA_KEY_PRESSED "p_pre"
#define PRIVATE_DATA_KEY_ITEM_MOVED "p_it_mv"

#define PRIVATE_MOVE_THRESHOLD 30
#define PRIVATE_FLICK_TIME 100
#define PRIVATE_ITEM_ALPHA 0.8
#define PRIVATE_ITEM_ALPHA_MAX 100



extern list_type_default_s *item_get_info(Evas_Object *item)
{
	retv_if(!item, NULL);
	list_type_default_s *info = NULL;

	info = evas_object_data_get(item, DATA_KEY_ITEM_INFO);

	return info;
}



extern void item_clear_set_disable(Evas_Object *scroller)
{
	Evas_Object *clear_item = NULL;
	ret_if(!scroller);

	clear_item = evas_object_data_get(scroller, PRIVATE_DATA_KEY_CLEAR);
	ret_if(!clear_item);
	elm_layout_signal_emit(main_get_info()->layout, "no,apps,txt,show", "no,apps,txt");
	elm_layout_signal_emit(clear_item, "no,apps", "clear");

}



static void _clear_all_clicked_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	Evas_Object *scroller = data;
	ret_if(!scroller);
	_D("clear all item is clicked");
	scroller_pop_all_item(scroller, 1);
}



extern Evas_Object *item_clear_all_create(Evas_Object *scroller)
{
	retv_if(!scroller, NULL);

	Evas_Object *clear_item = NULL;

	clear_item = elm_layout_add(scroller);
	retv_if(!clear_item, NULL);

	if (!elm_layout_file_set(clear_item, ITEM_LAYOUT, "clear_item")) {
		_E("Failed to set the layout");
		evas_object_del(clear_item);
		return NULL;
	}

	elm_object_part_text_set(clear_item, "name", _("IDS_TASKMGR_BUTTON_CLEAR_ALL"));

	elm_object_signal_callback_add(clear_item, "mouse,clicked,1", "event", _clear_all_clicked_cb, scroller);
	evas_object_data_set(scroller, PRIVATE_DATA_KEY_CLEAR, clear_item);

	evas_object_size_hint_weight_set(clear_item, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(clear_item);

	return clear_item;

}



extern void item_clear_all_destroy(Evas_Object *scroller)
{
	ret_if(!scroller);

	Evas_Object *clear_item = NULL;

	clear_item = evas_object_data_del(scroller, PRIVATE_DATA_KEY_CLEAR);
	if (clear_item) evas_object_del(clear_item);
}



static Evas_Object *_content_set_item_inner(Evas_Object *item_outer)
{
	Evas_Object *box = NULL;
	Evas_Object *item_inner = NULL;
	Evas_Object *next_item = NULL;

	box = main_get_info()->box;
	retv_if(!box, NULL);

	item_inner = evas_object_data_get(item_outer, PRIVATE_DATA_KEY_ITEM_INNER_FOR_MOVING);
	retv_if(!item_inner, NULL);

	next_item = evas_object_data_del(item_outer, PRIVATE_DATA_KEY_NEXT_ITEM);

	elm_box_unpack(box, item_outer);
	elm_object_part_content_set(item_outer, "inner", item_inner);

	if (!next_item) {
		elm_box_pack_end(box, item_outer);
	} else {
		elm_box_pack_before(box, item_outer, next_item);
	}
	return item_outer;
}



static Evas_Object *_content_unset_item_inner(Evas_Object *item_outer)
{
	Evas_Object *box = NULL;
	Evas_Object *item_inner = NULL;
	Evas_Object *tmp_item = NULL;
	Eina_List *list = NULL;
	const Eina_List *l = NULL;
	const Eina_List *ln = NULL;


	box = main_get_info()->box;
	retv_if(!box, NULL);

	item_inner = elm_object_part_content_unset(item_outer, "inner");
	retv_if(!item_inner, NULL);
	evas_object_data_set(item_outer, PRIVATE_DATA_KEY_ITEM_INNER_FOR_MOVING, item_inner);

	list = elm_box_children_get(box);
	retv_if(!list, NULL);

	EINA_LIST_FOREACH_SAFE(list, l, ln, tmp_item) {
		if (item_outer != tmp_item) continue;
		Evas_Object *next_item = eina_list_data_get(ln);
		evas_object_data_set(item_outer, PRIVATE_DATA_KEY_NEXT_ITEM, next_item);
		break;
	}
	eina_list_free(list);

	return item_outer;
}



#define ANIM_RATE 5
#define ANIM_RATE_SPARE ANIM_RATE - 1
static Eina_Bool _anim_move_item(void *data)
{
	Evas_Object *scroller = NULL;
	Evas_Object *item_inner = NULL;
	Evas_Object *item_outer = NULL;

	int cur_x, cur_y;
	int end_x, end_y;
	int end_pos, cur_pos;
	int vec_pos;

	item_outer = data;
	goto_if(!data, ERROR);

	scroller = main_get_info()->scroller;
	goto_if(!scroller, ERROR);

	item_inner = evas_object_data_get(item_outer, PRIVATE_DATA_KEY_ITEM_INNER_FOR_MOVING);
	goto_if(!item_inner, ERROR);

	evas_object_geometry_get(item_inner, &cur_x, &cur_y, NULL, NULL);
	evas_object_geometry_get(item_outer, &end_x, &end_y, NULL, NULL);

	end_pos = end_x;
	vec_pos = (end_x - cur_x)/ANIM_RATE;
	cur_x += vec_pos;
	cur_pos = cur_x;

	if ((end_pos - cur_pos) < 0) { // Move : Right -> Left
		evas_object_move(item_inner, cur_x, cur_y);
		int alp = 255 - ((cur_pos - end_pos) * PRIVATE_ITEM_ALPHA) - 1;
		if (alp < PRIVATE_ITEM_ALPHA_MAX) alp = PRIVATE_ITEM_ALPHA_MAX;
		evas_object_color_set(item_inner, alp, alp, alp, alp);

		if (cur_pos - ANIM_RATE_SPARE <= end_pos) {
			evas_object_move(item_inner, end_x, end_y);
			goto_if (! _content_set_item_inner(item_outer), ERROR);
			/* unfreeze the scroller after setting the content */
			scroller_unfreeze(scroller);
			evas_object_data_del(item_outer, PRIVATE_DATA_KEY_ITEM_INNER_FOR_MOVING);
			evas_object_data_del(item_outer, PRIVATE_DATA_KEY_ITEM_ANIM_FOR_MOVING);
			return ECORE_CALLBACK_CANCEL;
		}
	}
	else { // Move : Left -> Right
		evas_object_move(item_inner, cur_x, cur_y);
		int alp = 255 - ((end_pos - cur_pos) * PRIVATE_ITEM_ALPHA) - 1;
		if (alp < PRIVATE_ITEM_ALPHA_MAX) alp = PRIVATE_ITEM_ALPHA_MAX;
		evas_object_color_set(item_inner, alp, alp, alp, alp);

		if (cur_pos + ANIM_RATE_SPARE >= end_pos) {
			evas_object_move(item_inner, end_x, end_y);
			goto_if (NULL == _content_set_item_inner(item_outer), ERROR);
			/* unfreeze the scroller after setting the content */
			scroller_unfreeze(scroller);
			evas_object_data_del(item_outer, PRIVATE_DATA_KEY_ITEM_INNER_FOR_MOVING);
			evas_object_data_del(item_outer, PRIVATE_DATA_KEY_ITEM_ANIM_FOR_MOVING);
			return ECORE_CALLBACK_CANCEL;
		}
	}
	return ECORE_CALLBACK_RENEW;

ERROR:
	if (item_outer) {
		scroller_unfreeze(scroller);
		evas_object_data_del(item_outer, PRIVATE_DATA_KEY_ITEM_INNER_FOR_MOVING);
		evas_object_data_del(item_outer, PRIVATE_DATA_KEY_ITEM_ANIM_FOR_MOVING);
	}
	return ECORE_CALLBACK_CANCEL;
}



#define SLIPPED_LENGTH 40
static Eina_Bool _anim_slipped_item(void *data)
{
	Evas_Object *scroller = NULL;
	Evas_Object *item_outer = data;

	int item_outer_w, item_outer_h;

	goto_if(!item_outer, ERROR);

	scroller = main_get_info()->scroller;
	goto_if(!scroller, ERROR);

	item_outer_w = (int)evas_object_data_get(item_outer, PRIVATE_DATA_KEY_ITEM_W);
	item_outer_h = (int)evas_object_data_get(item_outer, PRIVATE_DATA_KEY_ITEM_H);

	item_outer_h -= SLIPPED_LENGTH;
	evas_object_size_hint_min_set(item_outer, ELM_SCALE_SIZE(item_outer_w), ELM_SCALE_SIZE(item_outer_h));
	evas_object_data_set(item_outer, PRIVATE_DATA_KEY_ITEM_H, (void *)item_outer_h);

	if (item_outer_h <= 0) {
		scroller_pop_item(scroller, item_outer, 1);

		if (scroller_count(scroller) < 2) {
			item_clear_set_disable(scroller);
		}
		goto ERROR;
	}

	return ECORE_CALLBACK_RENEW;

ERROR:
	if (item_outer) {
		evas_object_data_del(item_outer, PRIVATE_DATA_KEY_ITEM_W);
		evas_object_data_del(item_outer, PRIVATE_DATA_KEY_ITEM_H);
		evas_object_data_del(item_outer, PRIVATE_DATA_KEY_ITEM_ANIM_FOR_MOVING);
	}
	return ECORE_CALLBACK_CANCEL;
}



#define TERMINATE_LENGTH 60
static Eina_Bool _anim_terminate_item(void *data)
{
	Evas_Object *item_outer = data;
	Evas_Object *item_inner = NULL;

	int item_x, item_y, item_w, item_h, is_boundary = 0;
	int outer_x, outer_y;

	goto_if(!item_outer, ERROR);

	item_inner = evas_object_data_get(item_outer, PRIVATE_DATA_KEY_ITEM_INNER_FOR_MOVING);
	if(!item_inner) {
		item_inner = elm_object_part_content_unset(item_outer, "inner");
		retv_if(!item_inner, ECORE_CALLBACK_CANCEL);
		evas_object_data_set(item_outer, PRIVATE_DATA_KEY_ITEM_INNER_FOR_MOVING, item_inner);
	}

	evas_object_geometry_get(item_inner, &item_x, &item_y, &item_w, &item_h);
	evas_object_geometry_get(item_outer, &outer_x, &outer_y, NULL, NULL);

	if (item_x >= outer_x) {
		item_x += TERMINATE_LENGTH;
	} else {
		item_x -= TERMINATE_LENGTH;
	}

	if (item_x >= main_get_info()->root_w || (item_x + item_w) <= 0) {
		is_boundary = 1;
	}

	evas_object_move(item_inner, item_x, item_y);

	if (is_boundary) {
		Evas_Object *scroller = NULL;
		Ecore_Animator *anim = NULL;

		scroller = main_get_info()->scroller;
		goto_if(!scroller, ERROR);

		/* unfreeze the scroller after setting the content */
		scroller_unfreeze(scroller);
		evas_object_data_del(item_outer, PRIVATE_DATA_KEY_ITEM_INNER_FOR_MOVING);
		evas_object_data_del(item_outer, PRIVATE_DATA_KEY_ITEM_ANIM_FOR_MOVING);
		evas_object_color_set(item_inner, 255, 255, 255, 0);
		elm_object_part_content_set(item_outer, "inner", item_inner);

		anim = evas_object_data_get(item_outer, PRIVATE_DATA_KEY_ITEM_ANIM_FOR_MOVING);
		if (anim) {
			ecore_animator_del(anim);
			anim = NULL;
			evas_object_data_del(item_outer, PRIVATE_DATA_KEY_ITEM_ANIM_FOR_MOVING);
		}
		anim = ecore_animator_add(_anim_slipped_item, item_outer);
		goto_if(!anim, ERROR);
		evas_object_data_set(item_outer, PRIVATE_DATA_KEY_ITEM_ANIM_FOR_MOVING, anim);
		return ECORE_CALLBACK_CANCEL;
	}
	return ECORE_CALLBACK_RENEW;

ERROR:
	if (item_outer) {
		evas_object_data_del(item_outer, PRIVATE_DATA_KEY_ITEM_INNER_FOR_MOVING);
		evas_object_data_del(item_outer, PRIVATE_DATA_KEY_ITEM_ANIM_FOR_MOVING);
	}
	return ECORE_CALLBACK_CANCEL;
}



static void _down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	Evas_Event_Mouse_Down *ei = event_info;
	Evas_Object *item_outer = data;
	Evas_Object *item_inner = NULL;

	int x = ei->output.x;
	int y = ei->output.y;
	int down_time = ei->timestamp;
	int rect_x, rect_y, rect_w, rect_h;

	ret_if(!item_outer);

	_D("Down (%d, %d)", x, y);

	item_inner = elm_object_part_content_get(item_outer, "inner");
	if (!item_inner) {
		item_inner = evas_object_data_del(item_outer, PRIVATE_DATA_KEY_ITEM_INNER_FOR_MOVING);
		ret_if(!item_inner);
		elm_object_part_content_set(item_outer, "inner", item_inner);
	}

	evas_object_data_set(item_outer, PRIVATE_DATA_KEY_PRESSED, (void *) 1);
	evas_object_data_set(item_outer, PRIVATE_DATA_KEY_DOWN_X, (void *) x);
	evas_object_data_set(item_outer, PRIVATE_DATA_KEY_DOWN_Y, (void *) y);
	evas_object_data_set(item_outer, PRIVATE_DATA_KEY_DOWN_TIME, (void *)down_time);

	evas_object_geometry_get(item_outer, &rect_x, &rect_y, &rect_w, &rect_h);
	evas_object_data_set(item_outer, PRIVATE_DATA_KEY_ITEM_X, (void *) rect_x);
	evas_object_data_set(item_outer, PRIVATE_DATA_KEY_ITEM_Y, (void *) rect_y);
	evas_object_data_set(item_outer, PRIVATE_DATA_KEY_ITEM_W, (void *) rect_w);
	evas_object_data_set(item_outer, PRIVATE_DATA_KEY_ITEM_H, (void *) rect_h);
}



#define LAUNCHING_LENGTH 60
#define SCROLLER_THRESHOLD 15
static void _move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	Evas_Event_Mouse_Move *ei = event_info;
	Evas_Object *item_outer = data;

	Evas_Object *layout = NULL;
	Evas_Object *item_inner = NULL;
	Evas_Object *scroller = NULL;
	Ecore_Animator *anim = NULL;

	int down_x, down_y;
	int outer_x, outer_y;
	int cur_x, cur_y, vec_x, vec_y;
	int alpha = 0;

	if (!item_outer) return;
	if (!evas_object_data_get(item_outer, PRIVATE_DATA_KEY_PRESSED)) return;

	scroller = main_get_info()->scroller;
	ret_if(!scroller);
	if (scroller_is_scrolling(scroller)) {
		evas_object_data_set(item_outer, PRIVATE_DATA_KEY_SCROLLED, (void *)1);
		evas_object_data_del(item_outer, PRIVATE_DATA_KEY_PRESSED);

		return;
	}

	layout = main_get_info()->layout;
	ret_if(!layout);

	anim = evas_object_data_get(item_outer, PRIVATE_DATA_KEY_ITEM_ANIM_FOR_MOVING);
	if (anim) return;

	down_x = (int) evas_object_data_get(item_outer, PRIVATE_DATA_KEY_DOWN_X);
	down_y = (int) evas_object_data_get(item_outer, PRIVATE_DATA_KEY_DOWN_Y);

	outer_x = (int)evas_object_data_get(item_outer, PRIVATE_DATA_KEY_ITEM_X);
	outer_y = (int)evas_object_data_get(item_outer, PRIVATE_DATA_KEY_ITEM_Y);

	cur_x = ei->cur.output.x;
	cur_y = ei->cur.output.y;

	vec_x = cur_x - down_x;
	vec_y = cur_y - down_y;

	outer_x += vec_x;

	if (!evas_object_data_get(item_outer, PRIVATE_DATA_KEY_ITEM_MOVED)) {
		if (abs(vec_y) > abs(vec_x) + SCROLLER_THRESHOLD) {
			evas_object_data_set(item_outer, PRIVATE_DATA_KEY_SCROLLED, (void *)1);
			return;
		}
		if (abs(vec_x) <= PRIVATE_MOVE_THRESHOLD) return;
		if (abs(vec_y) >= SCROLLER_THRESHOLD) return;
	}

	evas_object_data_set(item_outer, PRIVATE_DATA_KEY_ITEM_MOVED, (void *) 1);
	item_inner = evas_object_data_get(item_outer, PRIVATE_DATA_KEY_ITEM_INNER_FOR_MOVING);
	if (!item_inner) {
		/* freeze the scroller before unsetting the content */
		scroller_freeze(scroller);
		ret_if(!_content_unset_item_inner(item_outer));
	}

	alpha = 255 -(abs(vec_x) * PRIVATE_ITEM_ALPHA);
	if (alpha < PRIVATE_ITEM_ALPHA_MAX) {
		alpha = PRIVATE_ITEM_ALPHA_MAX;
	}

	evas_object_color_set(item_inner, alpha, alpha, alpha, alpha);
	evas_object_move(item_inner, outer_x, outer_y);
}



static void _item_terminate_anim(Evas_Object *item_outer)
{
	Ecore_Animator *anim = NULL;

	anim = evas_object_data_get(item_outer, PRIVATE_DATA_KEY_ITEM_ANIM_FOR_MOVING);
	if (anim) {
		ecore_animator_del(anim);
		anim = NULL;
		evas_object_data_del(item_outer, PRIVATE_DATA_KEY_ITEM_ANIM_FOR_MOVING);
	}

	anim = ecore_animator_add(_anim_terminate_item, item_outer);
	ret_if(!anim);
	evas_object_data_set(item_outer, PRIVATE_DATA_KEY_ITEM_ANIM_FOR_MOVING, anim);
}



static void _up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	Evas_Event_Mouse_Up *ei = event_info;
	Evas_Object *item_outer = data;

	Evas_Object *scroller = NULL;

	int x = ei->output.x;
	int y = ei->output.y;
	int up_time = ei->timestamp;
	int down_time = 0;
	int init_pos, item_pos, item_size, tm_threshold;

	_D("Up (%d, %d)", x, y);

	ret_if (!item_outer);

	scroller = main_get_info()->scroller;
	ret_if(!scroller);

	evas_object_data_del(item_outer, PRIVATE_DATA_KEY_PRESSED);
	evas_object_data_del(item_outer, PRIVATE_DATA_KEY_DOWN_Y);
	evas_object_data_del(item_outer, PRIVATE_DATA_KEY_DOWN_X);
	evas_object_data_del(item_outer, PRIVATE_DATA_KEY_ITEM_MOVED);
	evas_object_data_del(item_outer, PRIVATE_DATA_KEY_SCROLLED);

	down_time = (int) evas_object_data_del(item_outer, PRIVATE_DATA_KEY_DOWN_TIME);

	/* This means the inner item has not been moved */
	Evas_Object *item_inner = evas_object_data_get(item_outer, PRIVATE_DATA_KEY_ITEM_INNER_FOR_MOVING);
	if (!item_inner) return;

	init_pos = (int) evas_object_data_del(item_outer, PRIVATE_DATA_KEY_ITEM_X);
	evas_object_data_del(item_outer, PRIVATE_DATA_KEY_ITEM_Y);
	evas_object_geometry_get(item_inner, &item_pos, NULL, &item_size, NULL);
	tm_threshold = main_get_info()->root_w - 100;

	if (item_pos > tm_threshold || item_pos < 10 || (up_time - down_time < PRIVATE_FLICK_TIME && abs(item_pos - init_pos) > 0)) {
		_item_terminate_anim(item_outer);

	} else if (item_pos != init_pos) {
		Ecore_Animator *anim = NULL;
		anim = evas_object_data_get(item_outer, PRIVATE_DATA_KEY_ITEM_ANIM_FOR_MOVING);
		if (anim) {
			ecore_animator_del(anim);
			anim = NULL;
			evas_object_data_del(item_outer, PRIVATE_DATA_KEY_ITEM_ANIM_FOR_MOVING);
		}

		anim = ecore_animator_add(_anim_move_item, item_outer);
		ret_if(NULL == anim);
		evas_object_data_set(item_outer, PRIVATE_DATA_KEY_ITEM_ANIM_FOR_MOVING, anim);
	}
}



static void _clicked_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	_D("Clicked");

	ret_if(!data);
	Evas_Object *item = data;
	Evas_Object *item_inner = NULL;

	const char *appid = NULL;

	list_type_default_s *info = NULL;

	item_inner = elm_object_part_content_get(item, "inner");
	if (!item_inner) return;

	info = item_get_info(item);
	ret_if(!info);

	appid = info->appid;
	ret_if(!appid);

	elm_object_signal_emit(item_inner, "clicked", "event");

	util_launch_app(appid);
	//layout_hide_with_timer();
}



extern Evas_Object *item_create(Evas_Object *scroller, list_type_default_s *info)
{
	retv_if(NULL == scroller, NULL);
	retv_if(NULL == info, NULL);

	Evas_Object *icon = NULL;
	Evas_Object *item = NULL;
	Evas_Object *item_inner = NULL;

	item = elm_layout_add(scroller);
	retv_if(NULL == item, NULL);
	elm_layout_file_set(item, ITEM_LAYOUT, "item");
	evas_object_size_hint_weight_set(item, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(item);

	item_inner = elm_layout_add(scroller);
	goto_if(NULL == item_inner, ERROR);
	elm_layout_file_set(item_inner, ITEM_LAYOUT, "item_inner");
	evas_object_size_hint_weight_set(item_inner, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(item_inner);
	elm_object_part_content_set(item, "inner", item_inner);
	evas_object_event_callback_add(item_inner, EVAS_CALLBACK_MOUSE_DOWN, _down_cb, item);
	evas_object_event_callback_add(item_inner, EVAS_CALLBACK_MOUSE_MOVE, _move_cb, item);
	evas_object_event_callback_add(item_inner, EVAS_CALLBACK_MOUSE_UP, _up_cb, item);

	icon = evas_object_image_add(main_get_info()->e);
	goto_if(NULL == icon, ERROR);
	evas_object_image_file_set(icon, info->icon, NULL);
	evas_object_image_filled_set(icon, EINA_TRUE);
	evas_object_show(icon);
	elm_object_part_content_set(item_inner, "icon", icon);
	elm_object_part_text_set(item_inner, "name", info->name);

	elm_object_signal_callback_add(item_inner, "mouse,clicked,1", "event", _clicked_cb, item);

	evas_object_data_set(item, DATA_KEY_ITEM_INFO, info);

	return item;

ERROR:
	item_destroy(item);
	return NULL;
}



extern void item_destroy(Evas_Object *item)
{
	ret_if(!item);

	Evas_Object *icon = NULL;
	Evas_Object *item_inner = NULL;

	item_inner = elm_object_part_content_unset(item, "inner");
	if (!item_inner) {
		_E("cannot get the object");
		evas_object_del(item);
		return;
	}

	icon = elm_object_part_content_unset(item_inner, "icon");
	if (icon) {
		evas_object_del(icon);
	} else {
		_E("cannot get the object");
	}

	evas_object_data_del(item, DATA_KEY_ITEM_INFO);

	evas_object_del(item_inner);
	evas_object_del(item);
}



extern void item_terminate(Evas_Object *item)
{
	char *appid = NULL;
	int ret;
	bool running = false;

	ret_if(!item);
	ret_if(!item_get_info(item));
	appid = item_get_info(item)->appid;
	ret_if(!appid);

	ret = app_manager_is_running(appid, &running);
	ret_if(APP_MANAGER_ERROR_NONE != ret);

	_D("Terminate: %s(%d)", appid, running);
	if (running) {
		util_kill_app(appid);
	}

	if (0 != rua_delete_history_with_pkgname(appid)) {
		_E("Cannot delete history for package(%s)", appid);
	}
}



//End of a file
