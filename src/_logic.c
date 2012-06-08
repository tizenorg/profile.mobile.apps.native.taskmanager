 /*
  * Copyright 2012  Samsung Electronics Co., Ltd
  *
  * Licensed under the Flora License, Version 1.0 (the License);
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *     http://www.tizenopensource.org/license
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an AS IS BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */


#include <appcore-common.h>
#include <aul.h>
#include <Ecore_X.h>
#include <vconf.h>

#include "taskmanager.h"
#include "_util_log.h"
#include "_util_efl.h"
#include "_genlist.h"
#include "_eina.h"
#include "_progressbar.h"
#include "_info.h"


int _dead_cb(int pid, void *data)
{
_D("func\n");
	/* redraw list */
	struct appdata *ad = (struct appdata *)data;
	retvm_if(ad == NULL, -1, "Invalid argument: appdata is NULL\n");

	_fini_pthread();
	if (ad->update_timer) {
		ecore_timer_del(ad->update_timer);
		ad->update_timer = NULL;
	}
	_subt_einalist_item(ad, pid);

	_D("mode(%d) count(%d)\n", ad->mode, ad->endcnt);
	switch (ad->mode) {
	default:
	case MODE_END_INUSE:
	case MODE_DEL_HISTORY:
	case MODE_DEL_ALL_HISTORY:
	case MODE_KILL_INUSE:
		_D("aa\n");
		_del_popup_timer(ad);
		_del_progressbar(ad);
		refresh_app_info(ad);
		_restart_pthread(ad);
		break;

	case MODE_END_ALL_INUSE:
	case MODE_KILL_ALL_INUSE:
		_D("bb\n");
		if (ad->endcnt <= 1) {
			_D("count set 0\n");
			_del_popup_timer(ad);
			_del_progressbar(ad);
			refresh_app_info(ad);
			_restart_pthread(ad);

		} else {
			ad->endcnt--;
		}
		break;
	}
}

static void _back_cb(void *data, Evas_Object *obj, void *event_info)
{
	struct appdata *ad = (struct appdata *)data;
	/*
	_unset_notification_level(ad->win);
	*/
	elm_exit();

}

static void _drag_start_cb(void *data, Evas_Object *obj, void *event_info)
{
	_D("func\n");
	struct appdata *ad = (struct appdata *)data;
	if (ad->update_timer) {
		ecore_timer_del(ad->update_timer);
		ad->update_timer = NULL;
	}
}

static void _drag_stop_cb(void *data, Evas_Object *obj, void *event_info)
{
	_D("func\n");
	struct appdata *ad = (struct appdata *)data;
	if (ad->update_timer) {
		ecore_timer_del(ad->update_timer);
		ad->update_timer = NULL;
	}
	ad->update_timer = ecore_timer_add(2.0, _update_list, ad);
}


static void _anim_start_cb(void *data, Evas_Object *obj, void *event_info)
{
	_D("func\n");
	struct appdata *ad = (struct appdata *)data;
	if (ad->update_timer) {
		ecore_timer_del(ad->update_timer);
		ad->update_timer = NULL;
	}
}

static void _anim_stop_cb(void *data, Evas_Object *obj, void *event_info)
{
	_D("func\n");
	struct appdata *ad = (struct appdata *)data;
	if (ad->update_timer) {
		ecore_timer_del(ad->update_timer);
		ad->update_timer = NULL;
	}
	ad->update_timer = ecore_timer_add(2.0, _update_list, ad);
}

static void _moved_cb(void *data, Evas_Object *obj, void *event_info)
{
	_D("func\n");
}

int _app_create(struct appdata *ad)
{
	Evas_Object *ly, *bg, *nv, *bt, *gl;

	retvm_if(ad == NULL, -1, "Invalid argument: appdata is NULL\n");
	ad->ending = EINA_FALSE;

	ly = _add_layout_main(ad->win, EINA_TRUE, EINA_FALSE);
	retvm_if(ly == NULL, -1, "Failed to add layout main\n");

	bg = _add_bg(ad->win, "group_list");
	retvm_if(bg == NULL, -1, "Failed to add bg\n");
	elm_object_part_content_set(ly, "elm.swallow.bg", bg);

	nv = _add_naviframe(ly);
	retvm_if(nv == NULL, -1, "Failed to add naviframe\n");
	evas_object_show(nv);
	ad->nv = nv;

	ly = _add_layout(ad->nv, EDJ_NAME, GRP_TM);
	retvm_if(ly == NULL, -1, "Failed to add layout\n");
	ad->ly = ly;

	/* Load default content (running task) */
	gl = _add_genlist(ly);
	retvm_if(gl == NULL, -1, "Failed to add genlist\n");
	elm_genlist_block_count_set(gl, 20);
	evas_object_data_set(gl, "appdata", ad);
	elm_object_part_content_set(ly, "list", gl);
	evas_object_smart_callback_add(gl, "scroll,drag,start", _drag_start_cb, ad);
	evas_object_smart_callback_add(gl, "scroll,drag,stop", _drag_stop_cb, ad);
	evas_object_smart_callback_add(gl, "scroll,anim,start", _anim_start_cb, ad);
	evas_object_smart_callback_add(gl, "scroll,anim,stop", _anim_stop_cb, ad);
//	evas_object_smart_callback_add(gl, "edge,bottom", _moved_cb, ad);
	evas_object_show(gl);
	ad->gl = gl;

	bt = elm_button_add(nv);
	retvm_if(bt == NULL, -1, "Failed to add button\n");
	elm_object_style_set(bt, "naviframe/end_btn/default");
	evas_object_smart_callback_add(bt, "clicked", _back_cb, ad);

	elm_naviframe_item_push(nv,
			_("IDS_TASKMGR_HEADER_TASK_SWITCHER"),
			bt, NULL, ly, NULL);

	return 0;
}

static void _get_win_geometry(struct appdata *ad)
{
	Ecore_X_Window focus_win;
	Ecore_X_Window root_win;

	focus_win = ecore_x_window_focus_get();
	root_win = ecore_x_window_root_get(focus_win);
	ecore_x_window_size_get(root_win, &ad->root_w, &ad->root_h);
}

static void _vconf_noti_cb(keynode_t *node, void *data)
{
	elm_exit();
}

static void _set_vconf_noti(void *data)
{
	vconf_notify_key_changed(VCONFKEY_IDLE_LOCK_STATE,
			_vconf_noti_cb, NULL);
}

/* this func is to exit taskmanager after launching application */
static Eina_Bool __climsg_cb(void *data, int type, void *event)
{
	static Atom a_deact;
	pid_t pid_a, pid_d;

	Ecore_X_Event_Client_Message *ev = event;

	if(ev == NULL) {
		_E("Invalid argument: event is NULL\n");
		ecore_timer_add(1.5, _exit_cb, NULL);
		return ECORE_CALLBACK_CANCEL;
	}

	pid_a = ev->data.l[1];
	pid_d = ev->data.l[3];
	a_deact = ecore_x_atom_get("_X_ILLUME_DEACTIVATE_WINDOW");

	/* when pid_a == pid_d, this is useless data */
	if (pid_a == pid_d) {
		return ECORE_CALLBACK_RENEW;
	}

	if (ev->message_type == a_deact) {
		_D("exit after 1.0 sec\n");
		ecore_timer_add(1.5, _exit_cb, NULL);
		return ECORE_CALLBACK_CANCEL;
	} else {
		_D("messagre is act\n");

	}

	return ECORE_CALLBACK_CANCEL;
}

static Eina_Bool _ask_kill_process(void *data)
{
	struct appdata *ad = data;

	_del_progressbar(ad);

	ad->mode *= 2;
	/* why? check enum in taskmgr.h */

	switch (ad->mode) {
		case MODE_KILL_INUSE:
			_D("kill all inuse\n");
			response_kill_inuse(ad);
			break;

		case MODE_KILL_ALL_INUSE:
			_D("kill all inuse\n");
			response_kill_all_inuse(ad);
			break;
	}
/*
	if (ad->popup_ask) {
		evas_object_del(ad->popup_ask);
		ad->popup_ask = NULL;
	}
	ad->popup_ask = _add_popup_ask(ad->win,
			"It might be an invalid process. Do you want to kill this proceess anyway?",
			ad);
*/
	return ECORE_CALLBACK_CANCEL;
}

void _ok_response_cb(void *data, Evas_Object *obj, void *event_info)
{
	struct appdata *ad = (struct appdata *)data;

	retm_if(data == NULL, "Invalid argument: appdata is NULL\n");
	if (ad->popup_ask) {
		evas_object_del(ad->popup_ask);
		ad->popup_ask = NULL;
	}

	switch (ad->mode) {
		case MODE_END_INUSE:
			_D("end inuse\n");
			_del_popup_timer(ad);
			ad->popup_timer = ecore_timer_add(7.0, _ask_kill_process, ad);
			_show_progressbar(ad);
			response_end_inuse(ad);
			_restart_pthread(ad);
			break;

		case MODE_END_ALL_INUSE:
			_D("end all inuse\n");
			_del_popup_timer(ad);
			ad->popup_timer = ecore_timer_add(7.0, _ask_kill_process, ad);
			_show_progressbar(ad);
			response_end_all_inuse(ad);
			break;

		case MODE_DEL_HISTORY:
			_D("del inuse\n");
			_del_popup_timer(ad);
			ad->popup_timer = ecore_timer_add(5.0, _ask_kill_process, ad);
			response_del_history(ad);
			break;

		case MODE_DEL_ALL_HISTORY:
			_D("del all inuse\n");
			_del_popup_timer(ad);
			ad->popup_timer = ecore_timer_add(5.0, _ask_kill_process, ad);
			response_del_all_history(ad);
			break;

		case MODE_KILL_INUSE:
			_D("kill all inuse\n");
			response_kill_inuse(ad);
			break;

		case MODE_KILL_ALL_INUSE:
			_D("kill all inuse\n");
			response_kill_all_inuse(ad);
			break;

		default:
			printf("[Wanning] taskmanager: check mode [%d]\n",
					ad->mode);
			break;
	}
}

void _cancel_response_cb(void *data, Evas_Object *obj, void *event_info)
{
	struct appdata *ad = (struct appdata *)data;

	retm_if(data == NULL, "Invalid argument: appdata is NULL\n");
	if (ad->popup_ask) {
		evas_object_del(ad->popup_ask);
		ad->popup_ask = NULL;
	}
}

Eina_Bool _create_idler_cb(void *data)
{
	Evas_Object *pu, *bt1, *bt2;
	struct appdata *ad = (struct appdata *)data;
	retvm_if(ad == NULL, ECORE_CALLBACK_CANCEL, "Invalid argument\n");

	evas_object_show(ad->win);

	_key_grab(ad);

	_init_pthread();
	_get_win_geometry(ad);
	_set_vconf_noti(ad);
	ecore_event_handler_add(ECORE_X_EVENT_CLIENT_MESSAGE, __climsg_cb, NULL);

	return ECORE_CALLBACK_CANCEL;
}



