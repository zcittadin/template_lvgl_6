/*
 * Copyright (c) 2021 Coopermaq
 * All rights reserved
 * Author: Zander Cittadin
 */

#include <stdlib.h>
#include "lvgl.h"
#include "log.h"
#include "buzzer.h"
#include "usart.h"
#include "util.h"
#include "gpio.h"
#include "gui_keyboard.h"

//static void clear_cb(lv_obj_t * obj, lv_event_t event);
static void keyboard_event(lv_obj_t * obj, lv_event_t event);
static void keyboard_refresh(lv_task_t * task);

static const char * btnm_map[] = { "1", "2", "3", "\n", "4", "5", "6", "\n",
                    "7", "8", "9", "\n", LV_SYMBOL_TRASH"", "0", LV_SYMBOL_LEFT"", "" };

static char str_number[2];

lv_obj_t *keyboard;
lv_obj_t *text_area;
lv_obj_t *image;

lv_task_t *keyboard_refresh_task;

/*static void clean_page(void) {
    if (keyboard_refresh_task) {
        lv_task_del(keyboard_refresh_task);
        keyboard_refresh_task = NULL;
    }
    lv_obj_clean(lv_scr_act());
}*/

void keyboard_screen() {

    lv_obj_t *container_box;
    container_box = lv_cont_create(lv_scr_act(), NULL);
    lv_cont_set_fit(container_box, LV_FIT_NONE);
    lv_obj_set_size(container_box, 380, 410);
    lv_obj_align(container_box, NULL, LV_ALIGN_IN_TOP_LEFT, 30, 60);
    lv_cont_set_layout(container_box, LV_LAYOUT_COL_M);

    text_area = lv_ta_create(container_box, NULL);

    lv_obj_set_size(text_area, 160, 70);
    lv_obj_align(text_area, NULL, LV_ALIGN_IN_TOP_RIGHT, -5, 20);
    lv_ta_set_max_length(text_area, 2);
    lv_ta_set_one_line(text_area, true);
    lv_ta_set_text(text_area, str_number);
    lv_ta_set_cursor_click_pos(text_area, true);
    lv_ta_set_cursor_pos(text_area, 0);
    lv_ta_set_cursor_blink_time(text_area, 300);

    lv_obj_t *btn_confirm = lv_btn_create(container_box, NULL);
    //lv_obj_set_event_cb(btn_confirm, do_test);
    lv_obj_set_size(btn_confirm, 200, 60);
    lv_obj_align_origo(btn_confirm, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *label;
    label = lv_label_create(btn_confirm, NULL);
    lv_label_set_text(label, "SET");

    lv_obj_t * btn_matrix = lv_btnm_create(lv_scr_act(), NULL);
    lv_btnm_set_map(btn_matrix, btnm_map);
    lv_btnm_set_btn_width(btn_matrix, 20, 4);
    lv_obj_align(btn_matrix, NULL, LV_ALIGN_IN_TOP_RIGHT, -90, 60);
    lv_obj_set_width(btn_matrix, 345);
    lv_obj_set_height(btn_matrix, 410);
    lv_obj_set_event_cb(btn_matrix, keyboard_event);

    keyboard_refresh_task = lv_task_create(keyboard_refresh, 100, LV_TASK_PRIO_LOW, NULL);
}

/*static void goto_readings(lv_obj_t * obj, lv_event_t event) {
    if(event == LV_EVENT_CLICKED) {
        clean_page();
        readings_screen();
    }
}*/

static void keyboard_event(lv_obj_t * obj, lv_event_t event) {
    if (event == LV_EVENT_VALUE_CHANGED) {
        buzzer(50);
        uint16_t val = lv_btnm_get_active_btn(obj);
        char c[2];
        int key = val + 1;
        switch (val) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            trim_digit(str_number);
            itoa(key, c, 10);
            strcat(str_number, c);
            lv_ta_set_text(text_area, str_number);
            break;
            case 9:
            memset(str_number, ' ', 2);
            lv_ta_set_text(text_area, str_number);
            lv_ta_set_cursor_pos(text_area, 0);
            break;
            case 10:
            key = 0;
            trim_digit(str_number);
            itoa(key, c, 10);
            strcat(str_number, c);
            lv_ta_set_text(text_area, str_number);
            break;
            case 11:
            lv_ta_del_char(text_area);
            strcpy(str_number, lv_ta_get_text(text_area));
            break;
            default:
            __asm("nop");
            break;
        }
        if(strlen(str_number) > 1) {
            int num;
            num = atoi(str_number);
            if(num == 21) {
                uart6_printf("%d: B\n", num);
            } else if(num == 39) {
                uart6_printf("%d: L\n", num);
            } else {
                uart6_printf("%d: INVALIDO\n", num);
            }
        }
    }
}

static void keyboard_refresh(lv_task_t * task) {
    __asm("nop");
}