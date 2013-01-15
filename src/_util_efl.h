/*
 * org.tizen.taskmgr
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
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




#ifndef __TASKMANAGER_UTIL_EFL_H__
#define __TASKMANAGER_UTIL_EFL_H__

#include <Elementary.h>

Evas_Object *_add_window(const char *name);
Evas_Object *_add_bg(Evas_Object *parent, char *style);
Evas_Object *_add_genlist(Evas_Object *parent);
Evas_Object *_add_icon(Evas_Object *parent, const char *png);
Evas_Object *_add_layout(Evas_Object *parent, const char *file, const char *group);
Evas_Object *_add_ctxpopup(Evas_Object *parent);
Evas_Object *_add_label(Evas_Object *parent, const char *msg);
Evas_Object *_add_naviframe(Evas_Object *parent);
Evas_Object *_add_layout_main(Evas_Object *parent,
		Eina_Bool content, Eina_Bool transparent);
Evas_Object *_add_progressbar(Evas_Object *parent, const char *style,
		Evas_Coord w, Evas_Coord h);
Evas_Object *_add_popup_ask(Evas_Object *parent, char *text, void *data);
void util_show_popup_with_message(Evas_Object *parent, double in, const char *msg);

#endif
/* __TASKMANAGER_UTIL_EFL_H__ */
