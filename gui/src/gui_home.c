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
#include "gpio.h"
#include "gui_home.h"

//static void clear_cb(lv_obj_t * obj, lv_event_t event);
static void do_test(lv_obj_t * obj, lv_event_t event);
static void home_refresh(lv_task_t * task);

lv_task_t * home_refresh_task;

/*static void clean_page(void) {
    if (home_refresh_task) {
        lv_task_del(home_refresh_task);
        home_refresh_task = NULL;
    }
    lv_obj_clean(lv_scr_act());
}*/

void home_screen() {
	lv_obj_t *label;

    lv_obj_t *btn_test = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(btn_test, do_test);
    lv_obj_set_size(btn_test, 200, 80);
    lv_obj_align_origo(btn_test, NULL, LV_ALIGN_CENTER, 0, 0);

    label = lv_label_create(btn_test, NULL);
    lv_label_set_text(label, "TEMPLATE");

    home_refresh_task = lv_task_create(home_refresh, 100, LV_TASK_PRIO_LOW, NULL);
}

static void do_test(lv_obj_t * obj, lv_event_t event) {
    if(event == LV_EVENT_CLICKED) {
        buzzer(75);
        char buf[] = "TESTE\n";
        int len = 0;
        len = strlen(buf);
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);
        HAL_UART_Transmit(&huart2, (uint8_t*)buf, len, 5000);
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);
    }
}

/*static void goto_readings(lv_obj_t * obj, lv_event_t event) {
    if(event == LV_EVENT_CLICKED) {
        clean_page();
        readings_screen();
    }
}*/

static void home_refresh(lv_task_t * task) {
    __asm("nop");
}