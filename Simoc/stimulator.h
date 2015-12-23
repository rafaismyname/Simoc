//
//  stimulator.h
//  Simoc
//
//  Created by Rafael Amorim on 10/21/14.
//  Copyright (c) 2014 Rafael Amorim. All rights reserved.
//

#ifndef __Simoc__stimulator__
#define __Simoc__stimulator__

#include <stdio.h>

struct stimulator{
    unsigned char pulse_length = 0;
    unsigned char time_between_puses = 0;
    unsigned char amplitude = 0;
    unsigned char modulation_frequency = 0;
};

unsigned char calculate_stimulator_checksum(stimulator stimulator);
unsigned char calculate_stimulators_checksum(stimulator stimulators[], int quantity);
unsigned char *generate_stimulation_command(stimulator stimulators[], int quantity);

#endif /* defined(__Simoc__stimulator__) */
