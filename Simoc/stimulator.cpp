//
//  stimulator.cpp
//  Simoc
//
//  Created by Rafael Amorim on 10/21/14.
//  Copyright (c) 2014 Rafael Amorim. All rights reserved.
//

#include "stimulator.h"

unsigned char calculate_stimulator_checksum(stimulator stimulator){
    unsigned char checksum = 0;
    
    checksum += stimulator.pulse_length;
    checksum += stimulator.time_between_puses;
    checksum += stimulator.amplitude;
    checksum += stimulator.modulation_frequency;
    
    return checksum;
}

unsigned char calculate_stimulators_checksum(stimulator stimulators[], int quantity){
    unsigned char checksum = 0;
    
    for (int i = 0; i < quantity; i++) {
        checksum += calculate_stimulator_checksum(stimulators[i]);
    }
    
    return checksum;
}

unsigned char *generate_stimulation_command(stimulator stimulators[], int quantity){
    int size_of_array = 4 + (quantity * 4);
    int index_aux = 1;
    unsigned char commands[size_of_array];
    
    commands[0] = 36;
    commands[1] = 67;
    
    for (int y = 0; y < quantity; y++) {
        commands[++index_aux] = stimulators[y].pulse_length;
        commands[++index_aux] = stimulators[y].time_between_puses;
        commands[++index_aux] = stimulators[y].amplitude;
        commands[++index_aux] = stimulators[y].modulation_frequency;
    }
    
    commands[size_of_array - 2] = commands[0] + commands[1] + calculate_stimulators_checksum(stimulators, quantity);
    commands[size_of_array - 1] = 59;
    
    return commands;
}

unsigned char *generate_stop_stimulation(){
    unsigned char commands[3] = {36, 90, 59};
    
    return commands;
}