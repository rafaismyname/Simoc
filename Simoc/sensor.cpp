//
//  sensor.cpp
//  Simoc
//
//  Created by Rafael Amorim on 9/30/14.
//  Copyright (c) 2014 Rafael Amorim. All rights reserved.
//

#include "sensor.h"
#include <cmath>

void calculate_inclination(sensor *sensor){
    sensor->inclination_x = ((sensor->bytes.x_msb << 8) + sensor->bytes.x_lsb);
    sensor->inclination_y = ((sensor->bytes.y_msb << 8) + sensor->bytes.y_lsb);
    
    sensor->degrees_x = asin((sensor->inclination_x - 1024)/819);
    sensor->degrees_y = asin((sensor->inclination_y - 1024)/819);
}