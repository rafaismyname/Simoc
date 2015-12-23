/** /
//  client.cpp
//  Simoc Client
//
//  Created by Rafael Amorim on 10/21/14.
//  Copyright (c) 2014 Rafael Amorim. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "stimulator.h"

#define PORT 5049
#define SERVER "10.1.0.171"

int client_fd;

void close_socket();

void error(const char *msg);

int main(int argc, char *argv[])
{
    unsigned char checksum, checksum_received, checksum_calculated;
    char recvBuff[36];
    unsigned char *sendBuff;
    int buffSize = sizeof(recvBuff);
    int aux = 2;
    int received_size = 0;
    struct sockaddr_in serv_addr;
    stimulator stimulators[8];
    
    bzero(recvBuff, buffSize);
    if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) error("\n Error : Could not create socket \n");
    
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    if(inet_pton(AF_INET, SERVER, &serv_addr.sin_addr) <= 0) error("\n inet_pton error occured\n");
    
    if(connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) error("\n Error : Connect Failed \n");
    
    do{
        bzero(recvBuff, buffSize);
        checksum = 0;
        received_size = read(client_fd, recvBuff, buffSize);
        if (received_size <= 0){
            error("Cliente parou de responder");
        }else{
            printf("\n");
            for(int i = 0; i < buffSize; i++) {
                printf("%u ", recvBuff[i] & 0xff);
                if(i < buffSize - 2){
                    checksum += recvBuff[i];
                }
            }
            checksum_received = recvBuff[(buffSize - 2)] & 0xff;
            printf("\nChecksum RxC: %u | %u ", checksum_received, checksum);
            if(checksum != checksum_received) printf("\nChecksum INVÁLIDO!");
            printf("\n");
            
            if(recvBuff[1] == 67){
                aux = 1;
                checksum_calculated = 0;
                for (int y = 0; y < 8; y++) {
                    stimulators[y].pulse_length = recvBuff[++aux];
                    printf("%u ", stimulators[y].pulse_length);
                    
                    stimulators[y].time_between_puses = recvBuff[++aux];
                    printf("%u ", stimulators[y].time_between_puses);
                    
                    stimulators[y].amplitude = recvBuff[++aux];
                    printf("%u ", stimulators[y].amplitude);
                    
                    stimulators[y].modulation_frequency = recvBuff[++aux];
                    printf("%u ", stimulators[y].modulation_frequency);
                }
                checksum_calculated += recvBuff[0] + recvBuff[1] + calculate_stimulators_checksum(stimulators, 8);
                printf("\nChecksum RxCxS: %u | %u | %u ", checksum_received, checksum, checksum_calculated);
                
                printf("\n");
                
                sendBuff = generate_stimulation_command(stimulators, 8);
                for(int i = 0; i < 36; i++) {
                    printf("%u ", sendBuff[i]);
                }
                
                printf("\n");
            }
        }
    }while(received_size > 0);
    
    return 0;
}

void close_socket(){
    if(client_fd) close(client_fd);
}

void error(const char *msg)
{
    close_socket();
    perror(msg);
    exit(1);
}
 
/**/