// Simple controller that listens for UDP packets, and sends response data	
#ifndef UDP_MESSAGE_CONTROLLER_H
#define UDP_MESSAGE_CONTROLLER_H

#include<stdio.h>	
#include<string.h> 
#include<stdlib.h> 
#include<arpa/inet.h>
#include<sys/socket.h>
#include <unistd.h>
#include "KillSignal.h"


// Code based on guide from 
// https://www.binarytides.com/programming-udp-sockets-c-linux/#include<stdio.h>	
#define UDP_PACKET_SIZE 32	//Max length of buffer
#define PORT 12345	//The port on which to listen for incoming data
#define TOKEN_SIZE 64

// close the port and print an error message on socket/port issue
void  UDPdie(char *data);
// listen for UDP messages on PORT
char* UDPlistenForMessage(void);
// return a packet to the last address that a packet was received from
void  UDPreturnPacket(char* data);
// free resources
void UDPListenerCleanup();
// initialize the socket to start listening on PORT
void UDPListenerInit();
// forwards a command to the relevant module after validating data
int UDPhandleCommand(char* message);
#endif