/*
 * Copyright (c) 2019 Coopermaq
 * All rights reserved
 */

#include <stdbool.h>
#include "lv_hal_indev.h"


#ifndef MY_TOUCH_H_
#define MY_TOUCH_H_

#define TOUCH_INVERT        0

#define TOUCH_HOR_RES     800
#define TOUCH_VER_RES     480
#define TOUCH_X_MIN       200
#define TOUCH_Y_MIN       300
#define TOUCH_X_MAX       3880
#define TOUCH_Y_MAX       3630
#define TOUCH_AVG         5

bool read_touch(lv_indev_drv_t *dvr, lv_indev_data_t * data);
int16_t ad_touch_handler(void);

#endif /* MY_TOUCH_H_ */
