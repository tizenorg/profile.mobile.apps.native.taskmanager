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

 #include "conf.h"
 #include "item.h"
 #include "list.h"
 #include "log.h"
 #include "main.h"
 #include "scroller.h"
 #include "util.h"

#define PRIVATE_DATA_KEY_DRAG "pri_drag"
#define PRIVATE_DATA_KEY_MOUSE_DOWN "pri_m_dw"
#define PRIVATE_DATA_KEY_POP_ALL_TIMER "pri_pop_tm"
#define PRIVATE_DATA_KEY_REVERSE_LIST "pri_re_li"
#define PRIVATE_DATA_KEY_TERMINATE "pri_tm"



extern task_mgr_error_e scroller_push_item(Evas_Object *scroller, Evas_Object *item)
{
	_D("");
	retv_if(!scroller, TASK_MGR_ERROR_INVALID_PARAMETER);
	retv_if(!item, TASK_MGR_ERROR_INVALID_PARAMETER);

	Evas_Object *box_layout = NULL;
	Evas_Object *box = NULL;

	box_layout = elm_object_content_get(scroller);
	retv_if(!box_layout, TASK_MGR_ERROR_FAIL);

	box = elm_object_part_content_get(box_layout, BOX_GROUP_NAME);
	retv_if(!box, TASK_MGR_ERROR_FAIL);

	elm_box_pack_end(box, item);

	return TASK_MGR_ERROR_NONE;
}



extern void scroller_pop_item(Evas_Object *scroller, Evas_Object *item, int terminate)
{
	ret_if(!scroller);
	ret_if(!item);

	Evas_Object *box_layout = NULL;
	Evas_Object *box = NULL;
	Evas_Object *tmp_item = NULL;

	Eina_List *list = NULL;
	const Eina_List *l = NULL;
	const Eina_List *ln = NULL;

	box_layout = elm_object_content_get(scroller);
	ret_if(!box_layout);

	box = elm_object_part_content_get(box_layout, BOX_GROUP_NAME);
	ret_if(!box);

	list = elm_box_children_get(box);
	ret_if(!list);

	EINA_LIST_FOREACH_SAFE(list, l, ln, tmp_item) {
		if (item != tmp_item) continue;
		if (terminate) item_terminate(item);

		elm_box_unpack(box, item);
		item_destroy(item);
		break;
	}
	eina_list_free(list);
}



extern task_mgr_error_e scroller_push_all_item(Evas_Object *scroller, Eina_List *list)
{
	const Eina_List *l = NULL;
	const Eina_List *ln = NULL;
	list_type_default_s *info = NULL;
	Evas_Object *item = NULL;

	if (!list) {
		_D("No app in the history list");
		return TASK_MGR_ERROR_NONE;
	}

	EINA_LIST_FOREACH_SAFE(list, l, ln, info) {
		if (!info) continue;
		item = item_create(scroller, info);
		continue_if(!item);
		scroller_push_item(scroller, item);
	}

	return TASK_MGR_ERROR_NONE;
}



static Eina_Bool _pop_all_item_cb(void *data)
{
	Evas_Object *scroller = data;
	Evas_Object *item = NULL;
	Eina_List *list = NULL;
	static int i = 0;
	int count = 0;
	int terminate = (int) evas_object_data_get(scroller, PRIVATE_DATA_KEY_TERMINATE);

	list = evas_object_data_get(scroller, PRIVATE_DATA_KEY_REVERSE_LIST);
	goto_if(!list, END);

	count = eina_list_count(list);
	_D("remove list count(include clear button) : %d", count);
	if (i >= count-1) goto END;

	item = eina_list_nth(list, i);
	i++;
	goto_if(!item, END);

	scroller_pop_item(scroller, item, terminate);
	return ECORE_CALLBACK_RENEW;

END:
	i = 0;
	eina_list_free(list);
	evas_object_data_del(scroller, PRIVATE_DATA_KEY_REVERSE_LIST);
	evas_object_data_del(scroller, PRIVATE_DATA_KEY_TERMINATE);
	evas_object_data_del(scroller, PRIVATE_DATA_KEY_POP_ALL_TIMER);
	item_clear_set_disable(scroller);

	return ECORE_CALLBACK_CANCEL;
}



extern void scroller_pop_all_item(Evas_Object *scroller, int terminate)
{
	Evas_Object *box_layout = NULL;
	Evas_Object *box = NULL;
	Eina_List *list = NULL;
	Eina_List *reverse_list = NULL;
	Ecore_Timer *timer = NULL;

	ret_if(!scroller);

	timer = evas_object_data_del(scroller, PRIVATE_DATA_KEY_POP_ALL_TIMER);
	if (timer) {
		_D("There is already a timer for popping all items.");
		ecore_timer_del(timer);
	}

	/* An user tap the end all button, all items have to be terminated even if paused. */
	if (!evas_object_data_get(scroller, PRIVATE_DATA_KEY_TERMINATE)) {
		evas_object_data_set(scroller, PRIVATE_DATA_KEY_TERMINATE, (void *) terminate);
	}

	box_layout = elm_object_content_get(scroller);
	ret_if(!box_layout);

	box = elm_object_part_content_get(box_layout, BOX_GROUP_NAME);
	ret_if(!box);

	list = elm_box_children_get(box);
	if (!list) return;

	/* This reverse list should be freed in the timer */
	reverse_list = eina_list_reverse_clone(list);
	eina_list_free(list);
	ret_if(!reverse_list);
	evas_object_data_set(scroller, PRIVATE_DATA_KEY_REVERSE_LIST, reverse_list);

	timer = ecore_timer_add(0.01f, _pop_all_item_cb, scroller);
	if (!timer) {
		_E("Cannot add a timer");
		evas_object_data_del(scroller, PRIVATE_DATA_KEY_REVERSE_LIST);
		evas_object_data_del(scroller, PRIVATE_DATA_KEY_TERMINATE);
		eina_list_free(reverse_list);
		return;
	}

	evas_object_data_set(scroller, PRIVATE_DATA_KEY_POP_ALL_TIMER, timer);
}



extern int scroller_count(Evas_Object *scroller)
{
	Evas_Object *box_layout = NULL;
	Evas_Object *box = NULL;
	Eina_List *list = NULL;

	retv_if(!scroller, 0);

	box_layout = elm_object_content_get(scroller);
	retv_if(!box_layout, 0);

	box = elm_object_part_content_get(box_layout, BOX_GROUP_NAME);
	retv_if(!box, 0);

	list = elm_box_children_get(box);
	if (!list) {
		_D("list is null");
		return 0;
	}

	return eina_list_count(list);
}



extern Eina_Bool scroller_is_scrolling(Evas_Object *scroller)
{
	return evas_object_data_get(scroller, DATA_KEY_IS_SCROLLING)? EINA_TRUE:EINA_FALSE;
}



extern void scroller_freeze(Evas_Object *scroller)
{
	Evas_Object *box_layout = NULL;
	Evas_Object *box = NULL;

	ret_if(!scroller);

	box_layout = elm_object_content_get(scroller);
	ret_if(!box_layout);

	box = elm_object_part_content_get(box_layout, BOX_GROUP_NAME);
	ret_if(!box);

	elm_object_scroll_freeze_push(box);
}



extern void scroller_unfreeze(Evas_Object *scroller)
{
	Evas_Object *box_layout = NULL;
	Evas_Object *box = NULL;

	ret_if(!scroller);

	box_layout = elm_object_content_get(scroller);
	ret_if(!box_layout);

	box = elm_object_part_content_get(box_layout, BOX_GROUP_NAME);
	ret_if(!box);

	while (elm_object_scroll_freeze_get(box)) {
		elm_object_scroll_freeze_pop(box);
	}
}



static void _mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	_D("Mouse is down");
	evas_object_data_set(obj, PRIVATE_DATA_KEY_MOUSE_DOWN, (void *) 1);
}



static void _mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	_D("Mouse is up");
	evas_object_data_del(obj, PRIVATE_DATA_KEY_MOUSE_DOWN);
	if (evas_object_data_get(obj, PRIVATE_DATA_KEY_DRAG)) return;
	evas_object_data_del(obj, DATA_KEY_IS_SCROLLING);
}



static void _anim_start_cb(void *data, Evas_Object *scroller, void *event_info)
{
	_D("start the scroller animation");
	evas_object_data_set(scroller, DATA_KEY_IS_SCROLLING, (void *) 1);
}



static void _anim_stop_cb(void *data, Evas_Object *scroller, void *event_info)
{
	_D("stop the scroller animation");
	if (evas_object_data_get(scroller, PRIVATE_DATA_KEY_MOUSE_DOWN)) return;
	evas_object_data_del(scroller, DATA_KEY_IS_SCROLLING);
}



static void _drag_start_cb(void *data, Evas_Object *scroller, void *event_info)
{
	_D("start to drag the scroller animation");
	evas_object_data_set(scroller, PRIVATE_DATA_KEY_DRAG, (void *) 1);
}



static void _drag_stop_cb(void *data, Evas_Object *scroller, void *event_info)
{
	_D("stop to drag the scroller animation");
	evas_object_data_del(scroller, PRIVATE_DATA_KEY_DRAG);
}



static void _scroll_cb(void *data, Evas_Object *scroller, void *event_info)
{
	evas_object_data_set(scroller, DATA_KEY_IS_SCROLLING, (void *) 1);
}



extern Evas_Object *scroller_create(Evas_Object *layout)
{
	retv_if(!layout, NULL);

	Evas_Object *box = NULL;
	Evas_Object *box_layout = NULL;
	Evas_Object *scroller = NULL;

	scroller = elm_scroller_add(layout);
	retv_if(!scroller, NULL);

	elm_scroller_bounce_set(scroller, EINA_FALSE, EINA_FALSE);
	elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
	//elm_object_style_set(scroller, "list_effect");
	elm_object_focus_allow_set(scroller, EINA_FALSE);
	elm_object_part_content_set(layout, "scroller", scroller);
	evas_object_show(scroller);

	evas_object_event_callback_add(scroller, EVAS_CALLBACK_MOUSE_DOWN, _mouse_down_cb, NULL);
	evas_object_event_callback_add(scroller, EVAS_CALLBACK_MOUSE_UP, _mouse_up_cb, NULL);
	evas_object_smart_callback_add(scroller, "scroll,anim,start", _anim_start_cb, NULL);
	evas_object_smart_callback_add(scroller, "scroll,anim,stop", _anim_stop_cb, NULL);
	evas_object_smart_callback_add(scroller, "scroll,drag,start", _drag_start_cb, NULL);
	evas_object_smart_callback_add(scroller, "scroll,drag,stop", _drag_stop_cb, NULL);
	evas_object_smart_callback_add(scroller, "scroll", _scroll_cb, NULL);
	/* Because change an align in box, use the layout betweein box and scroller. */

	box_layout = elm_layout_add(scroller);
	if (!box_layout) {
		_E("Fail to create box layout");
		evas_object_del(scroller);
		return NULL;
	}
	elm_layout_file_set(box_layout, BOX_LAYOUT, BOX_GROUP_NAME);
	evas_object_size_hint_align_set(box_layout, 0.5, 1.0);
	evas_object_size_hint_weight_set(box_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(box_layout);
	elm_object_content_set(scroller, box_layout);

	box = elm_box_add(scroller);
	if (!box) {
		_E("Fail to create box");
		evas_object_del(box_layout);
		evas_object_del(scroller);
		return NULL;
	}
	elm_box_horizontal_set(box, EINA_FALSE);
	evas_object_size_hint_align_set(box, 0.5, 1.0);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(box_layout, BOX_GROUP_NAME, box);
	evas_object_show(box);
	main_get_info()->box = box;

	return scroller;
}



void scroller_destroy(Evas_Object *scroller)
{
	ret_if(!scroller);

	Evas_Object *item = NULL;
	Evas_Object *box_layout = NULL;
	Evas_Object *box = NULL;
	Eina_List *list = NULL;

	ret_if(!scroller);
	scroller_pop_all_item(scroller, 0);

	box_layout = elm_object_content_unset(scroller);
	ret_if(!box_layout);

	box = elm_object_part_content_unset(box_layout, BOX_GROUP_NAME);
	ret_if(!box);

	list = elm_box_children_get(box);
	if (!list) {
		_D("list is empty");
		return;
	}

	EINA_LIST_FREE(list, item) {
		if (!item) break;
		item_destroy(item);
	}
	free(list);

	evas_object_del(box);
	evas_object_del(box_layout);

	evas_object_event_callback_del(scroller, EVAS_CALLBACK_MOUSE_DOWN, _mouse_down_cb);
	evas_object_event_callback_del(scroller, EVAS_CALLBACK_MOUSE_UP, _mouse_up_cb);
	evas_object_smart_callback_del(scroller, "scroll,anim,start", _anim_start_cb);
	evas_object_smart_callback_del(scroller, "scroll,anim,stop", _anim_stop_cb);
	evas_object_smart_callback_del(scroller, "scroll,drag,start", _drag_start_cb);
	evas_object_smart_callback_del(scroller, "scroll,drag,stop", _drag_stop_cb);
	evas_object_smart_callback_del(scroller, "scroll", _scroll_cb);

	evas_object_del(scroller);
}



/* End of the file */
