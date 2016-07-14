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

/**
 * @defgroup Defines Defines
 */

/**
 * @addtogroup Defines
 * @{
 */

//size
#define BASE_WIDTH 540 /**<Width of the base layout */
#define BASE_HEIGHT 960 /**<Height of the base layout */

#define CLEAR_ICON_WIDTH 62 /**<Width of the clear icon item*/
#define CLEAR_ICON_HEIGHT 62 /**<Height of the clear icon item*/
#define CLEAR_LINE_WIDTH 2 /**<Width of the clear icon line*/
#define CLEAR_LINE_HEIGHT 118 /**<Height of the clear icon line*/
#define CLEAR_ITEM_WIDTH CLEAR_ICON_WIDTH /**<Width of the clear all item icon*/
#define CLEAR_ITEM_HEIGHT CLEAR_LINE_HEIGHT /**<Height of the clear all item icon*/

#define ITEM_ICON_WIDTH 118 /**<Width of the item icon*/
#define ITEM_ICON_HEIGHT 118 /**<Height of the item icon*/
#define ITEM_LINE_WIDTH 2 /**<Width of the item line*/
#define ITEM_LINE_HEIGHT 180 /**<Height of the item line*/
#define ITEM_WIDTH ITEM_ICON_WIDTH /**<Width of the entire list item*/
#define ITEM_HEIGHT ITEM_LINE_HEIGHT /**<Height of the entire list item*/

#define PADDING_TOP 210 /**<Padding from the top of the view to clear item*/

#define SWIPE_THRESHOLD 0.1 /**<Item swipe threshold. If item position exceeds this value, related
			      application is killed*/
#define SWIPE_LEFT_THRESHOLD SWIPE_THRESHOLD /**<Item left swipe threshold*/
#define SWIPE_RIGHT_THRESHOLD (1.0 - SWIPE_THRESHOLD) /**<Items right swipe threshold*/

//color
#define BG_COLOR (255*0.5) /**<Background color transparency*/

//edc
#define EDJE_DIR "edje" /**<Name of the edje files directory*/
#define LAYOUT_EDJ EDJE_DIR"/layout.edj" /**<Relative path to the main layout edj file*/
#define BOX_LAYOUT_EDJ EDJE_DIR"/box.edj" /**<Relative path to the box layout edj file*/
#define ITEM_LAYOUT_EDJ EDJE_DIR"/item.edj" /**<Relative path to the item edj file*/

#define IMAGES_DIR "images" /**<Name of the image files directory*/
#define DEFAULT_ICON IMAGES_DIR"/default.png" /**<Relative path to the item default icon image*/

#define BOX_GROUP_NAME "box" /**<Name of the box edje group*/

/**
 * @}
 */

#endif //__TASK_MGR_CONF_H__
