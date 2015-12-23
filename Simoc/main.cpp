/**/
//  main.cpp
//  Simoc
//
//  Created by Rafael Amorim on 9/30/14.
//  Copyright (c) 2014 Rafael Amorim. All rights reserved.
//

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "sensor.h"
#include "stimulator.h"

#define PORT 5049

sensor sensors[5];
stimulator stimulators[8];
int server_fd, client_fd;
int battery_level = 0;

void close_socket();

void error(const char *msg);

void read_socket_buffer(unsigned char socket_buffer[], int socket_buffer_size);

void read_battery_from_buffer(unsigned char battery_buffer[]);

void read_sensors_from_buffer(unsigned char sensor_buffer[], int buffer_size);

int main(int argc, char *argv[])
{
    ssize_t buffer_read, buffer_write = 0;
    socklen_t client_len;
    struct sockaddr_in server_addr, client_addr;
    unsigned char *send_buffer;
    unsigned char socket_buffer[24];
    int socket_buffer_size = sizeof(socket_buffer);
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0); //new socket ipv4 tcp
    if (server_fd < 0) error("Erro ao abrir o socket");
    else printf("Socket aberto\n");
    
    bzero((char *) &server_addr, sizeof(server_addr)); //clear the server addr
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) error("Erro ao atrelar a porta");
    else printf("Porta atrelada\n");

    listen(server_fd, 1); //listen to tcp
    client_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_len); //waits and accept an tcp connection
    if (client_fd < 0) error("Erro ao receber conexão");
    else printf("Conexão efetuada\n");
    
    
    /*do{
        stimulators[0].pulse_length = 1;
        stimulators[0].time_between_puses = 3;
        stimulators[0].amplitude = 10;
        stimulators[0].modulation_frequency = 20;
        send_buffer = generate_stimulation_command(stimulators, 8);
        for(int i = 0; i < 36; i++) {
            printf("%u ", send_buffer[i] & 0xff);
        }
        printf("\n");
        buffer_write = write(client_fd, send_buffer, 36);
        if (buffer_write <= 0) error("Cliente não recebeu");
    }while(buffer_write > 0);*/
    
    do{
        bzero(socket_buffer, socket_buffer_size); //clears the buffer
        buffer_read = read(client_fd, socket_buffer, socket_buffer_size); //reads the socket buffer and returns the number of bytes
        if (buffer_read <= 0) error("Cliente parou de responder");
        else read_socket_buffer(socket_buffer, socket_buffer_size);
    }while(buffer_read > 0);
    
    close_socket();
    return 0;
}

void close_socket(){
    if(client_fd) close(client_fd);
    if(server_fd) close(server_fd);
}

void error(const char *msg)
{
    close_socket();
    perror(msg);
    exit(1);
}

void read_socket_buffer(unsigned char socket_buffer[], int socket_buffer_size){
    unsigned char sensor_buffer[socket_buffer_size], battery_buffer[4];
    
    int function_byte = socket_buffer[1];
    if(function_byte == 65){
        memcpy(battery_buffer, socket_buffer, sizeof(battery_buffer));
        read_battery_from_buffer(battery_buffer);
    }else if(function_byte == 66){
        memcpy(sensor_buffer, socket_buffer, socket_buffer_size);
        read_sensors_from_buffer(sensor_buffer, socket_buffer_size);
    }
}

void read_battery_from_buffer(unsigned char battery_buffer[]){
    battery_level = battery_buffer[2];
    printf("\nBattery: %i", battery_level);
    printf("\n");
}

void read_sensors_from_buffer(unsigned char sensor_buffer[], int buffer_size){
    unsigned char checksum = 0;
    unsigned char sensor_checksum = sensor_buffer[buffer_size - 2] & 0xff;
    int index_aux = 1;
    int sensor_quantity =  (buffer_size - 4) / 4;
    
    printf("\n");
    for(int i = 0; i < buffer_size; i++) {
        printf("%u ", sensor_buffer[i] & 0xff);
        if(i < (buffer_size - 2)){
            checksum += sensor_buffer[i];
        }
    }
    
    if(sensor_checksum != checksum)
        printf("\nCHECKSUM INVÁLIDO: %i - %i", sensor_checksum, checksum);
    
    for (int y = 0; y < sensor_quantity; y++) {
        sensors[y].bytes.x_msb = sensor_buffer[++index_aux] & 0xff;
        sensors[y].bytes.x_lsb = sensor_buffer[++index_aux] & 0xff;
        sensors[y].bytes.y_msb = sensor_buffer[++index_aux] & 0xff;
        sensors[y].bytes.y_lsb = sensor_buffer[++index_aux] & 0xff;
        if(sensors[y].bytes.x_msb > 0 && sensors[y].bytes.x_lsb > 0){
            calculate_inclination(&sensors[y]);
            printf("\nX%i: %u", (y + 1), sensors[y].inclination_x);
            printf("\nY%i: %u", (y + 1), sensors[y].inclination_y);
            printf("\nXo%i: %f", (y + 1), sensors[y].degrees_x);
            printf("\nYo%i: %f", (y + 1), sensors[y].degrees_y);
            printf("\n");
        }
    }
    
    printf("\n");
}

/**/