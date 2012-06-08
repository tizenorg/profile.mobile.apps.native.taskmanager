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


#include "taskmanager.h"
#include "_util_log.h"
#include "_util_efl.h"

void _del_progressbar(void *data)
{
	struct appdata *ad = (struct appdata *)data;
	retm_if(ad == NULL, "Invalid argument: appdata is NULL\n");

	if (ad->popup_progressbar) {
		evas_object_hide(ad->popup_progressbar);
		evas_object_del(ad->popup_progressbar);
		ad->popup_progressbar = NULL;
	}
}

void _show_progressbar(void *data)
{
	struct appdata *ad = (struct appdata *)data;
	retm_if(ad == NULL, "Invalid argument: appdata is NULL\n");

	if (ad->popup_progressbar)
		_del_progressbar(ad);

	ad->popup_progressbar = _add_progressbar(ad->win, "list_process",
			ad->root_w, ad->root_h);

	evas_object_show(ad->popup_progressbar);
}
