//
//  sensor.h
//  Simoc
//
//  Created by Rafael Amorim on 9/30/14.
//  Copyright (c) 2014 Rafael Amorim. All rights reserved.
//

#ifndef __Simoc__sensor__
#define __Simoc__sensor__

#include <stdio.h>

#define RIGHT_LEG_SENSOR 1
#define LEFT_LEG_SENSOR 3
#define RIGHT_THIGH_SENSOR = 2;
#define LEFT_THIGH_SENSOR = 4;
#define BACK_SENSOR = 5;

struct sensor_bytes{
    unsigned char x_msb;
    unsigned char x_lsb;
    unsigned char y_msb;
    unsigned char y_lsb;
};

struct sensor{
    int inclination_x;
    int inclination_y;
    float degrees_x;
    float degrees_y;
    sensor_bytes bytes;
};

void calculate_inclination(sensor *sensor);

#endif /* defined(__Simoc__sensor__) */
