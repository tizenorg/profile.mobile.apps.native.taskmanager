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




#include <stdio.h>
#include <unistd.h>
#include <appcore-efl.h>
#include <vconf.h>
#include <utilX.h>
#include <aul.h>
#include <Ecore_X.h>
#include <Eina.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#include "taskmanager.h"
#include "_genlist.h"
#include "_util_log.h"
#include "_util_efl.h"
#include "_logic.h"
#include "_cpu.h"
#include "_eina.h"
#include "_info.h"
#include "_progressbar.h"

pthread_t pt;
pthread_cond_t	pc;
pthread_mutex_t	pm;
pthread_mutex_t mutex_for_graph_update = PTHREAD_MUTEX_INITIALIZER;
static Elm_Object_Item *g_egi;

/* group list:gl, data list:dl, button list:bl, no list: nl */
static Elm_Genlist_Item_Class itc_gl;
static Elm_Genlist_Item_Class itc_hl;
static Elm_Genlist_Item_Class itc_dl;
static Elm_Genlist_Item_Class itc_bl;
static Elm_Genlist_Item_Class itc_nl;
static Elm_Genlist_Item_Class itc_separator4;
static Elm_Genlist_Item_Class itc_separator2;

static char *button_text[TS_MAX] = {
	"IDS_TASKMGR_BUTTON_CLOSE_ALL_APPLICATIONS",
	"IDS_TASKMGR_BUTTON_CLEAR_HISTORY_ABB"
};

static void end_all_inuse_cb(void *data, Evas_Object *obj, void *event_info);
static void delete_all_history_cb(void *data, Evas_Object *obj,
				  void *event_info);
void (*func_del[TS_MAX]) (void *data, Evas_Object *obj, void *event_info) = {
&end_all_inuse_cb, &delete_all_history_cb};

static void end_inuse_cb(void *data, Evas_Object *obj, void *event_info);
static void delete_history_cb(void *data, Evas_Object *obj, void *event_info);
void (*func_end[TS_MAX]) (void *data, Evas_Object *obj, void *event_info) = {
&end_inuse_cb, &delete_history_cb};

static char *group_name[TS_MAX] = {
	"IDS_TASKMGR_HEADER_RUNNING",
	"IDS_TASKMGR_MBODY_RECENTLY_USED"
};

static char *nolist_text[TS_MAX] = {
	"IDS_TASKMGR_MBODY_NO_APPS_OPEN",
	"IDS_TASKMGR_MBODY_NO_RECENTLY_USED_APPS"
};

void taskmanager_free_info(struct _task_info *info);
Eina_Bool _update_list(void *data);

static void clear_genlist(void *data)
{
	ret_if(data == NULL);

	struct appdata *ad = data;

	if (ad->gl) {
		elm_genlist_clear(ad->gl);
	}
}

void clear_task_manager_list(void *data)
{
	ret_if(data == NULL);

	struct appdata *ad = data;
	_free_einalist_all(ad);
	clear_genlist(ad);
}

static void app_genlist_item_update(void *data)
{
	ret_if(data == NULL);

	struct appdata *ad = (struct appdata *)data;

	Elm_Object_Item *it_r;
	Eina_List *realized_item_list, *l_r;
	unsigned int cnt = 0;

	realized_item_list = elm_genlist_realized_items_get(ad->gl);
	cnt = eina_list_count(realized_item_list);
	if (cnt > 0) {
		EINA_LIST_FOREACH(realized_item_list, l_r, it_r) {
			if (it_r != NULL) {
				elm_genlist_item_update(it_r);
			}
		}
	}
}

Eina_Bool alert_app_info(void *data)
{
	retv_if(data == NULL, -1);

	struct appdata *ad = (struct appdata *)data;

	app_genlist_item_update(ad);
	return ECORE_CALLBACK_CANCEL;
}

static void end_all_inuse_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(data == NULL);

	struct appdata *ad = data;
	char buf[_BUF_MAX] = { 0, };

	ad->mode = MODE_END_ALL_INUSE;
	snprintf(buf, sizeof(buf), T_("IDS_TASKMGR_POP_CLOSE_ALL_APPS_Q_THIS_MAY_CAUSE_DATA_TO_BE_LOST"));
	if (ad->popup_ask) {
		evas_object_del(ad->popup_ask);
		ad->popup_ask = NULL;
	}
	ad->popup_ask = _add_popup_ask(ad->win, buf, ad);
}

static void
delete_all_history_cb(void *data, Evas_Object *obj, void *event_info)
{
_D("func\n");
	ret_if(data == NULL);

	struct appdata *ad = data;
	char buf[_BUF_MAX] = { 0, };

	ad->mode = MODE_DEL_ALL_HISTORY;
	snprintf(buf, sizeof(buf), T_("IDS_TASKMGR_POP_CLEAR_ALL_APP_HISTORY_Q"));
	if (ad->popup_ask) {
		evas_object_del(ad->popup_ask);
		ad->popup_ask = NULL;
	}
	ad->popup_ask = _add_popup_ask(ad->win, buf, ad);
}

static void end_inuse_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(data == NULL);

	struct _task_info *info_ev = (struct _task_info *)data;
	struct appdata *ad = info_ev->ad;
	char buf[_BUF_MAX] = { 0, };

	ad->mode = MODE_END_INUSE;
	snprintf(buf, _BUF_MAX, T_("IDS_TASKMGR_POP_CLOSE_PS_APP_Q_THIS_MAY_CAUSE_DATA_TO_BE_LOST"), info_ev->app_name);
	if (ad->popup_ask) {
		evas_object_del(ad->popup_ask);
		ad->popup_ask = NULL;
	}
	ad->popup_ask = _add_popup_ask(ad->win, buf, ad);
	g_egi = (void *)info_ev->it;
}

static void delete_history_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(data == NULL);

	struct _task_info *info_ev = (struct _task_info *)data;
	struct appdata *ad = info_ev->ad;
	char buf[_BUF_MAX] = { 0, };

	ad->mode = MODE_DEL_HISTORY;

	snprintf(buf, _BUF_MAX, T_("IDS_TASKMGR_POP_CLEAR_PS_HISTORY_Q"), info_ev->app_name);
	if (ad->popup_ask) {
		evas_object_del(ad->popup_ask);
		ad->popup_ask = NULL;
	}
	ad->popup_ask = _add_popup_ask(ad->win, buf, ad);
	g_egi = (void *)info_ev->it;
}

static void nl_sel(void *data, Evas_Object *obj, void *event_info)
{
_D("func\n");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, EINA_FALSE);
	return;
}

static char *nl_text_get(void *data, Evas_Object *obj, const char *part)
{
	char buf[_BUF_MAX] = { 0, };

	if (!strcmp(part, "elm.text")) {
		snprintf(buf, sizeof(buf), "%s", T_(nolist_text[(int)data]));

		return strdup(buf);
	}
	return NULL;
}

static void _gl_sel_app(void *data, Evas_Object *obj, void *event_info)
{
_D("func\n");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	struct appdata *ad = (struct appdata *)data;
	struct _task_info *info;
	/* parameter to block double click */
	static int selected = 0;
	Elm_Object_Item *it = (Elm_Object_Item *) event_info;
	int ret = -1;

	elm_genlist_item_selected_set(it, EINA_FALSE);

	retm_if(ad == NULL, "Invalid argument: appdata is NULL\n");

	_fini_pthread();
	if (ad->update_timer) {
		ecore_timer_del(ad->update_timer);
		ad->update_timer = NULL;
	}

	if (selected == 1)
		return;
	selected = 1;

	if (item == NULL) {
		_E("[Error] Cannot find genlist item\n");
		selected = 0;
		return;
	}

	info = (struct _task_info *)elm_object_item_data_get(item);
	if (info == NULL) {
		_E("[Error] Cannot get item data: info\n");
		selected = 0;
		return;
	}

	if (info->pid) {
		/* when application is alive */
		ret = aul_resume_pid(info->pid);
		selected = 0;
	} else {
		/* when application is dead */
		if (info->pkg_name == NULL) {
			util_show_popup_with_message(info->ad->win,
					3.0,
					T_("IDS_TASKMGR_POP_UNABLE_TO_OPEN_APPLICATION"));
			selected = 0;

		} else {
			_unset_notification_level(info->ad->win);

			if (!strcmp(info->pkg_name, "org.tizen.phone")) {
				/* exception : Because dialer doesn't need bundle
				 * since being unifyed dialer, voice call and video call
				 */
				ret = aul_launch_app(info->pkg_name, NULL);
				selected = 0;
			} else {
				ret = aul_launch_app(info->pkg_name, info->b);
				selected = 0;
			}
		}
	}
	if(ret == 0) {
		_D("exit after 0.3 sec\n");
		ad->exit_timer = ecore_timer_add(0.3, _exit_cb, ad);
	}
}


static Evas_Object *_gl_content_get_app(void *data, Evas_Object *obj,
				     const char *part)
{
	struct _task_info *info = (struct _task_info *)data;
	char buf[_BUF_MAX] = { 0, };

	Evas_Object *icon = NULL;
	Evas_Object *btn = NULL;

	Evas_Object *rt, *icon_ly = NULL;

	retvm_if(data == NULL, NULL, "Invalid argument: task info is NULL\n");

	if (!strcmp(part, "elm.icon.1")) {
		snprintf(buf, sizeof(buf), "%s", info->icn_path);
		retvm_if(buf == NULL, NULL, "%s icon is NULL\n", info->app_name);
		if (!ecore_file_exists(buf) || strlen(buf) < 4)
			snprintf((char *)buf, (size_t) sizeof(buf),
				 (const char *)IMAGEDIR "/icon_taskmgr.png");

		if (!strncmp(&buf[strlen(buf) - 3], "edj", 3)) {
			icon_ly = _add_layout(obj, buf, "icon");

		} else {
			icon_ly = elm_icon_add(obj);
			elm_icon_file_set(icon_ly, buf, NULL);
		}

		icon = _add_layout(obj, EDJ_NAME, "icon");
		retvm_if(icon == NULL, NULL, "Cannot add layout: icon\n");

		rt = evas_object_rectangle_add(evas_object_evas_get(obj));
		retvm_if(rt == NULL, NULL, "Failed to add rectangle\n");

		evas_object_color_set(rt, 0, 0, 0, 0);
		evas_object_size_hint_min_set(rt,
					(int)72 * elm_scale_get(),
					(int)72 * elm_scale_get());
		elm_object_part_content_set(icon, "icon_ly", rt);

		elm_object_part_content_set(icon, "icon", icon_ly);

		return icon;

	} else if (!strcmp(part, "elm.icon.2")) {
		btn = elm_button_add(obj);
		elm_object_text_set(btn, S_("IDS_COM_BODY_END"));
		elm_object_style_set(btn, "default");

		evas_object_smart_callback_add(btn, "clicked",
					       func_end[info->category], info);
		elm_object_focus_set(btn, EINA_FALSE);
		evas_object_propagate_events_set(btn, EINA_FALSE);

		return btn;
	}

	return NULL;
}

static char *_gl_text_get_app(void *data, Evas_Object *obj, const char *part)
{
	struct _task_info *info = (struct _task_info *)data;
	char buf[_BUF_MAX] = { 0, };
	Evas_Object *eo;

	retvm_if(data == NULL, NULL, "Invalid argument: task info is NULL\n");
	retvm_if(part == NULL, NULL, "Invalid argument: part is NULL\n");

	if (!strcmp(part, "elm.text.1")) {
		snprintf(buf, _BUF_MAX, "%s", info->app_name);
		return strdup(buf);

	} else if (!strcmp(part, "elm.text.2")) {
		if (info->category == TS_INUSE) {
			snprintf(buf, _BUF_MAX, "CPU: %.1f%%", info->cpu);
			return strdup(buf);

		} else {
			if (info->it) {
				elm_object_signal_emit(info->it, "prog.hide.mem","taskmanager");
			}
			return NULL;
		}
	}
	return NULL;
}

static void _bl_sel(void *data, Evas_Object *obj, void *event_info)
{
_D("func\n");
	int mode = (int)data;
	struct appdata *ad;
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;

	elm_genlist_item_selected_set(item, EINA_FALSE);
}

static char *_bl_text_get(void *data, Evas_Object *obj, const char *part)
{
	if (!strcmp(part, "elm.text")) {
		return strdup(T_(button_text[(int)data]));

	}
	return NULL;
}

static Evas_Object *_bl_content_get(void *data, Evas_Object *obj,
				const char *part)
{
	Evas_Object *btn = NULL;
	struct appdata *ad = evas_object_data_get(obj, "appdata");

	if (!strcmp(part, "elm.icon")) {

		btn = elm_button_add(obj);
		elm_object_style_set(btn, "default");

		elm_object_text_set(btn, T_(button_text[(int)data]));
		evas_object_smart_callback_add(btn, "clicked",
					       func_del[(int)data], ad);
		elm_object_focus_set(btn, EINA_FALSE);

		evas_object_size_hint_min_set(btn, 0, 50);
		evas_object_size_hint_max_set(btn, 0, 50);
		evas_object_propagate_events_set(btn, EINA_FALSE);

		return btn;

	}
	return NULL;
}

static char *_gl_text_get_title(void *data, Evas_Object *obj, const char *part)
{
	char buf[_BUF_MAX];

	if (!strcmp(part, "elm.text")) {
		snprintf(buf, sizeof(buf), "%s (%d)",
				T_(group_name[(int)data]), _get_grp_cnt((int)data));
		return strdup(buf);
	}
	return NULL;
}

static char *_gl_text_get_his(void *data, Evas_Object *obj, const char *part)
{
	struct _task_info *info = (struct _task_info *)data;
	char buf[_BUF_MAX] = { 0, };
	Evas_Object *eo;

	if (!strcmp(part, "elm.text")) {
		snprintf(buf, _BUF_MAX, "%s", info->app_name);
		return strdup(buf);
	}
	return NULL;
}

static Evas_Object *_gl_content_get_his(void *data, Evas_Object *obj,
				     const char *part)
{
	struct _task_info *info = (struct _task_info *)data;
	char buf[_BUF_MAX] = { 0, };

	Evas_Object *icon = NULL;
	Evas_Object *btn = NULL;

	Evas_Object *rt, *icon_ly = NULL;
	retvm_if(data == NULL, NULL, "Invalid argument: task info is NULL\n");

	if (!strcmp(part, "elm.icon.1")) {
		snprintf(buf, sizeof(buf), "%s", info->icn_path);
		retvm_if(buf == NULL, NULL, "%s icon is NULL\n", info->app_name);
		if (!ecore_file_exists(buf) || strlen(buf) < 4)
			snprintf((char *)buf, (size_t) sizeof(buf),
				 (const char *)IMAGEDIR "/icon_taskmgr.png");

		if (!strncmp(&buf[strlen(buf) - 3], "edj", 3)) {
			icon_ly = _add_layout(obj, buf, "icon");

		} else {
			icon_ly = elm_icon_add(obj);
			elm_icon_file_set(icon_ly, buf, NULL);
		}

		icon = _add_layout(obj, EDJ_NAME, "icon");
		retvm_if (icon == NULL, NULL, "Cannot add layout: icon\n");

		rt = evas_object_rectangle_add(evas_object_evas_get(obj));
		retvm_if (rt == NULL, NULL, "Failed to add rectangle\n");

		evas_object_color_set(rt, 0, 0, 0, 0);
		evas_object_size_hint_min_set(rt,
					(int)72 * elm_scale_get(),
					(int)72 * elm_scale_get());
		elm_object_part_content_set(icon, "icon_ly", rt);

		elm_object_part_content_set(icon, "icon", icon_ly);

		return icon;

	} else if (!strcmp(part, "elm.icon.2")) {
		btn = elm_button_add(obj);
		elm_object_text_set(btn, S_("IDS_COM_OPT_DELETE"));
		elm_object_style_set(btn, "default");

		evas_object_smart_callback_add(btn, "clicked",
					       func_end[info->category], info);
		elm_object_focus_set(btn, EINA_FALSE);
		evas_object_propagate_events_set(btn, EINA_FALSE);

		return btn;
	}

	return NULL;

}

void _set_itc(void)
{
	itc_gl.item_style = "grouptitle";
	itc_gl.func.text_get = _gl_text_get_title;

	itc_dl.item_style = "2text.2icon.7";
	itc_dl.func.text_get = _gl_text_get_app;
	itc_dl.func.content_get = _gl_content_get_app;

	itc_hl.item_style = "1text.2icon.4";
	itc_hl.func.text_get = _gl_text_get_his;
	itc_hl.func.content_get = _gl_content_get_his;

	itc_separator4.item_style = "dialogue/seperator.4";
	itc_separator2.item_style = "dialogue/seperator.2";

	itc_bl.item_style = "1icon";
	itc_bl.func.content_get = _bl_content_get;

	itc_nl.item_style = "1text";
	itc_nl.func.text_get = nl_text_get;

}

int check_genlist(struct appdata *ad)
{
	Elm_Object_Item *egi;
	struct _task_info *info;
	int i = 1;

	egi = elm_genlist_first_item_get(ad->gl);
	while(egi) {
		info = (struct _task_info *)elm_object_item_data_get(egi);
		if(info) {
			_D("%s info[0x%x]\n", (int)info < 3 ? "-" : info->app_name, info);
		} else {
			_D("group\n");
		}
		egi = elm_genlist_item_next_get(egi);
	}

	return 0;
}

int _set_genlist_from_eina(struct appdata *ad)
{
_D("func\n");
	Eina_List *l;
	Elm_Object_Item *git, *item;
	struct _task_info *info;
	int i;

	retvm_if(ad == NULL, -1, "Invalid argument: appdata is NULL\n");
	retvm_if(ad->gl == NULL, -1, "Invalid argument:genlist is NULL\n");

	for (i = 0; i < TS_MAX; i++) {
		git = elm_genlist_item_append(ad->gl, &itc_gl,
						 (void *)i, NULL,
						 ELM_GENLIST_ITEM_NONE,
					      NULL, NULL);
		retvm_if(git == NULL, -1, "Failed append item\n");
		elm_genlist_item_select_mode_set(git, ELM_OBJECT_SELECT_MODE_NONE);

		if (eina_list_count(ad->applist[i]) > 0) {

			item = elm_genlist_item_append(ad->gl, &itc_separator4,
						NULL, git,
						ELM_GENLIST_ITEM_NONE,
						NULL, NULL);
			elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

			item = elm_genlist_item_append(ad->gl, &itc_separator2,
						NULL, git,
						ELM_GENLIST_ITEM_NONE,
						NULL, NULL);
			elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

			ad->applist[i] = eina_list_nth_list(ad->applist[i], 0);
			EINA_LIST_FOREACH(ad->applist[i], l, info) {
				if (info != NULL) {
					info->it = elm_genlist_item_append(ad->gl,
								    (i == TS_INUSE) ? &itc_dl : &itc_hl,
								    (void *)info, git,
								    ELM_GENLIST_ITEM_NONE,
								    _gl_sel_app, ad);
				}
			}
		} else {
			item = elm_genlist_item_append(ad->gl, &itc_nl,
						(void *)i, git,
						ELM_GENLIST_ITEM_NONE,
						nl_sel, NULL);
			elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
		}
	}
	return 0;
}

Eina_Bool _update_list(void *data)
{
_D("func\n");
	struct appdata *ad = data;
	Eina_List *l, *l_next;
	struct _task_info *info;

	pthread_mutex_lock(&mutex_for_graph_update);

	if(ad->applist[TS_INUSE] == NULL) {
		_D("inuse is NULL\n");
		pthread_mutex_unlock(&mutex_for_graph_update);
		ad->update_timer = NULL;
		return ECORE_CALLBACK_CANCEL;
	}
	ad->applist[TS_INUSE] = eina_list_nth_list(ad->applist[TS_INUSE], 0);
	_D("%d\n", eina_list_count(ad->applist[TS_INUSE]));

	EINA_LIST_FOREACH_SAFE(ad->applist[TS_INUSE], l, l_next, info) {
		elm_genlist_item_fields_update(info->it, "elm.text*", ELM_GENLIST_ITEM_FIELD_TEXT);
	}
	pthread_mutex_unlock(&mutex_for_graph_update);
	return ECORE_CALLBACK_RENEW;
}

void *_update_pthread_cb(void *data)
{
_D("func\n");
	struct appdata *ad = data;
	long tick;
	int ncpu;
	Eina_List *l, *l_next;
	struct _task_info *info;

	while(1) {

		if (ad->ending == EINA_FALSE) {
			if(ad->applist[TS_INUSE] == NULL) {
				_D("in use list is NULL\n");
				return ECORE_CALLBACK_CANCEL;
			}

			_get_sysconf(&ncpu, &tick);

			ad->applist[TS_INUSE] = eina_list_nth_list(ad->applist[TS_INUSE], 0);
			if (eina_list_count(ad->applist[TS_INUSE]) < 1) {
				_D("list count is 0\n");
				return NULL;
			}

			EINA_LIST_FOREACH_SAFE(ad->applist[TS_INUSE], l, l_next, info) {
				if (info) {
					info->cpu = _get_cpu_ratio(info, ncpu, tick);
					_D("%d / %lf\n", info->pid, info->cpu);
				}

			}
			sleep(2);
		}
	}

	return NULL;

}

void _set_genlist(struct appdata *ad)
{
_D("func\n");
	retm_if(ad == NULL, "Invalid argument: appdata is NULL\n");
	int ret = AUL_R_ERROR;
	int retry_cnt = 0;
	int sleep_value = 1000;

	_init_grp_cnt();

	while (ret != AUL_R_OK && retry_cnt < 5) {
		usleep(sleep_value);
		ret = aul_app_get_running_app_info(runapp_info_get, ad);

		if (ret != AUL_R_OK) {
			_D("Fail to get running app information from ail");
		}

		retry_cnt++;
		sleep_value *= 2;
	}

	taskmanager_get_history_app_info(ad);
	_set_genlist_from_eina(ad);

}

void _init_pthread(void)
{
	pthread_mutex_init(&pm, NULL);
	pthread_cond_init(&pc, NULL);
}

void _fini_pthread(void)
{
_D("func\n");
	if (pt) {
		pthread_cancel(pt);
	}
}

void refresh_app_info(struct appdata *ad)
{
_D("func\n");
	retm_if(ad == NULL, "Invalid argument: appdata is NULL\n");

	_free_einalist_all(ad);
	clear_genlist(ad);

	_set_genlist(ad);

	alert_app_info(ad);

}

void _del_popup_timer(struct appdata *ad)
{
	if (ad->popup_timer) {
		ecore_timer_del(ad->popup_timer);
		ad->popup_timer = NULL;
	}
}

void taskmanager_free_info(struct _task_info *info)
{
	if (info) {
		if (info->app_name) {
			free(info->app_name);
			info->app_name = NULL;
		}
		if (info->pkg_name) {
			free(info->pkg_name);
			info->pkg_name = NULL;
		}
		if (info->icn_path) {
			free(info->icn_path);
			info->icn_path = NULL;
		}

		free(info);
	}
}

void _restart_pthread(struct appdata *ad)
{
_D("func\n");
	pthread_cancel(pt);
	if (eina_list_count(ad->applist[TS_INUSE]) > 0) {

		pthread_create(&pt, NULL, _update_pthread_cb, ad);
		pthread_detach(pt);
		if (ad->update_timer) {
			ecore_timer_del(ad->update_timer);
			ad->update_timer = NULL;
		}
		ad->update_timer = ecore_timer_add(2.0, _update_list, ad);
	}
}
int response_end_inuse(struct appdata *ad)
{
_D("func\n");
	Eina_List *l, *l_next;
	struct _task_info *info;
	Elm_Object_Item *egi;
	Eina_Bool dead = EINA_FALSE;

	retvm_if(ad == NULL, -1, "Invalid argument: appdata is NULL\n");

	ad->ending = EINA_TRUE;

	EINA_LIST_FOREACH_SAFE(ad->applist[TS_INUSE], l, l_next, info) {
		_D("applist pid : %d", info->pid);
		if (info->it == g_egi) {
			_D("matched applist pid : %d", info->pid);
			if (info->pid > 0) {
				if (aul_terminate_pid(info->pid) < 0) {
					kill(info->pid, SIGKILL);
					dead = EINA_TRUE;
				}
			}
			break;
		}
	}
	ad->ending = EINA_FALSE;

	if(!dead){
		_D("matched applist is nothing\n");
		_del_progressbar(ad);
	}

	return 0;
}

Eina_Bool _refresh_idler_cb(void *data)
{
_D("func\n");
	struct appdata *ad = (struct appdata *)data;
	retvm_if(data == NULL, ECORE_CALLBACK_CANCEL, "Invalid argument: appdata is NULL\n:");

	_del_popup_timer(ad);
	_del_progressbar(ad);
	refresh_app_info(ad);
	return ECORE_CALLBACK_CANCEL;
}

int response_end_all_inuse(struct appdata *ad)
{
	Eina_List *l;
	struct _task_info *info;

	retvm_if(ad == NULL, -1, "Invalid argument: appdata is NULL\n");

	ad->ending = EINA_TRUE;

	if(ad->update_timer) {
		ecore_timer_del(ad->update_timer);
		ad->update_timer = NULL;
	}

	ad->endcnt = eina_list_count(ad->applist[TS_INUSE]);
	_D("set end count (%d)\n", ad->endcnt);

	EINA_LIST_FOREACH(ad->applist[TS_INUSE], l, info) {
		if (info != NULL) {
			_D("applist pid : %d", info->pid);
			if (info->pid > 0) {
				if (aul_terminate_pid(info->pid) < 0) {
					kill(info->pid, SIGKILL);
				}
				_D("terminated\n");
			}
		}
	}
	ad->ending = EINA_FALSE;
	return 0;
}

int response_del_history(struct appdata *ad)
{
	Eina_List *l, *l_next;
	struct _task_info *info;
	Elm_Object_Item *egi;

	retvm_if(ad == NULL, -1, "Invalid argument: appdata is NULL\n");

	_show_progressbar(ad);
	EINA_LIST_FOREACH_SAFE(ad->applist[TS_HISTORY], l, l_next, info) {
		_D("history applist pid : %d", info->pid);
		if (info->it == g_egi) {

			if (rua_init() == -1) {
				break;
			}
			rua_delete_history_with_pkgname(info->pkg_name);
			rua_fini();

			ad->applist[TS_HISTORY] =
			    eina_list_remove_list(ad->applist[TS_HISTORY], l);

			if (info->b) {
				bundle_free(info->b);
			}

			elm_object_item_del(info->it);
			taskmanager_free_info(info);
			break;
		}
	}
	alert_app_info(ad);
	refresh_app_info(ad);
	_del_popup_timer(ad);
	_del_progressbar(ad);

	ad->mode = MODE_NONE;
	return 0;
}

int response_del_all_history(struct appdata *ad)
{
	Eina_List *l;
	struct _task_info *info;

	retvm_if(ad == NULL, -1, "Invalid argument: appdata is NULL\n");

	if(ad->update_timer) {
		_D("update timer is deleted\n");
		ecore_timer_del(ad->update_timer);
		ad->update_timer = NULL;
	}


	_show_progressbar(ad);

	if (rua_init() == -1) {
		return -1;
	}

	EINA_LIST_FOREACH(ad->applist[TS_HISTORY], l, info) {
		if (info != NULL) {
			rua_delete_history_with_pkgname(info->pkg_name);
		}
	}

	if (eina_list_count(ad->applist[TS_INUSE]) == 0) {
		rua_clear_history();
	}

	rua_fini();
	refresh_app_info(ad);
	_del_popup_timer(ad);
	_del_progressbar(ad);

	ad->mode = MODE_NONE;
	return 0;
}

int response_kill_inuse(struct appdata *ad)
{
	Eina_List *l, *l_next;
	struct _task_info *info;
	Elm_Object_Item *egi;

	retvm_if(ad == NULL, -1, "Invalid argument: appdata is NULL\n");

	_show_progressbar(ad);

	EINA_LIST_FOREACH_SAFE(ad->applist[TS_INUSE], l, l_next, info) {
		_D("kill applist pid : %d", info->pid);
		if (info->it == g_egi) {
			if (info->pid > 0) {
				kill(info->pid, SIGKILL);
			}

			ad->applist[TS_INUSE] =
			    eina_list_remove_list(ad->applist[TS_INUSE], l);
			taskmanager_free_info(info);
			break;
		}
	}
	refresh_app_info(ad);
	_del_progressbar(ad);

	return 0;
}

int response_kill_all_inuse(struct appdata *ad)
{
	Eina_List *l;
	struct _task_info *info;

	retvm_if(ad == NULL, -1, "Invalid argument: appdata is NULL\n");

	_show_progressbar(ad);

	EINA_LIST_FOREACH(ad->applist[TS_INUSE], l, info) {
		_D("kill all applist pid : %d", info->pid);
		if (info != NULL) {
			if (info->pid > 0) {
				kill(info->pid, SIGKILL);
			}
		}
	}
	refresh_app_info(ad);
	_del_progressbar(ad);

	return 0;
}

