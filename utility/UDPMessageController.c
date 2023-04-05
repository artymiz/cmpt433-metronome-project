// Simple controller that listens for UDP packets, and sends response data
// Code based on guide from 
// https://www.binarytides.com/programming-udp-sockets-c-linux/#include<stdio.h>
#include "UDPMessageController.h"


#define MAX_COMMAND_LEN VOL_DOWN_LEN

static struct sockaddr_in si_me, si_other;
static int s;
static unsigned slen = sizeof(si_other), recv_len;
static char buff[UDP_PACKET_SIZE];

void UDPdie(char *data) {
	perror(data);
	UDPListenerCleanup();
	//exit(1);
}

//free resources
void UDPListenerCleanup() {
	close(s);
}

//initialize udp structure
void UDPListenerInit(){
    printf("opening socket\n");
	//create a UDP socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		UDPdie("socket");
	}
	
	// zero out the structure
	memset((char *) &si_me, 0, sizeof(si_me));
	//si_me.sin_zero
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    printf("binding port to socket\n");
	//bind socket to port
	if( bind(s, (struct sockaddr*)&si_me, sizeof(si_me) ) == -1) {
		UDPdie("bind");
	}
}

//listens for a message on a socket
char* UDPlistenForMessage(void) {
	//keep listening for data
    printf("Waiting for data...\n");
    fflush(stdout);
    
    //try to receive some data, this is a blocking call
    if ((recv_len = recvfrom(s, buff, UDP_PACKET_SIZE, 0, (struct sockaddr *) &si_other, &slen)) == -1) {
        UDPdie("recvfrom()");
    }
    //print details of the client/peer and the data received
    printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
	return buff;
}

//send a packet back to the device a packet was last received from
void UDPreturnPacket(char* data) {
    //now reply the client with the same data
    if (sendto(s, data, UDP_PACKET_SIZE, 0, (struct sockaddr*) &si_other, slen) == -1) {
        UDPdie("sendto()");
    }
}
