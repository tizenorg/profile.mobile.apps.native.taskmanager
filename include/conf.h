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

#ifndef __TASK_MGR_CONF_H__
#define __TASK_MGR_CONF_H__

//size
#define BASE_WIDTH 540
#define BASE_HEIGHT 960

#define CLEAR_ICON_WIDTH 60
#define CLEAR_ICON_HEIGHT 60
#define CLEAR_LINE_WIDTH 2
#define CLEAR_LINE_HEIGHT 120
#define CLEAR_ITEM_WIDTH CLEAR_ICON_WIDTH
#define CLEAR_ITEM_HEIGHT CLEAR_LINE_HEIGHT

#define ITEM_ICON_WIDTH 88
#define ITEM_ICON_HEIGHT 88
#define ITEM_LINE_WIDTH 2
#define ITEM_LINE_HEIGHT 148
#define ITEM_WIDTH ITEM_ICON_WIDTH
#define ITEM_HEIGHT ITEM_LINE_HEIGHT

//color
#define BG_COLOR (255*0.5)

//edc
#define LAYOUT EDJEDIR"/layout.edj"
#define BOX_LAYOUT EDJEDIR"/box.edj"
#define ITEM_LAYOUT EDJEDIR"/item.edj"

#define BOX_GROUP_NAME "box"

#endif //__TASK_MGR_CONF_H__
