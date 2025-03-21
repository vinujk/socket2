#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <sys/time.h>
#include "socket.h"
#include <time.h>
#include <signal.h>

int path_received = 0;
int sock = 0;

//struct in_addr sender_ip, receiver_ip;

char sender_ip[16];
char receiver_ip[16];


struct session* path_head;
struct session* resv_head;

#define IP_ADDRLEN 16

/*void get_ip(char buffer[], char *sender_ip, char *receiver_ip) {

	struct session_object *temp = (struct session_object*)(buffer+START_RECV_SESSION_OBJ);
	//inet_pton(AF_INET, "192.168.13.2", &sender_ip);
	inet_ntop(AF_INET, &temp->src_ip, sender_ip, IP_ADDRLEN);
	inet_ntop(AF_INET, &temp->dst_ip, receiver_ip, IP_ADDRLEN);

	printf(" ip is %s %s\n", sender_ip,receiver_ip);
}*/ 	
		
int main() {

    struct in_addr sender_ip;
    struct in_addr receiver_ip;
    u_int8_t reached = 0;

    struct sockaddr_in addr;
    sock = socket(AF_INET, SOCK_RAW, RSVP_PROTOCOL);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }
  
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
	perror("binding failed");
	close(sock);
	exit(EXIT_FAILURE);
    }

    char buffer[1024];

    struct sockaddr_in sender_addr;
    socklen_t addr_len = sizeof(sender_addr);

    resv_event_handler();
    path_event_handler();
    
    while(1) {
	printf("waiting to receive mesgae\n");
   	memset(buffer, 0, sizeof(buffer));
	int bytes_received = recvfrom(sock, buffer, sizeof(buffer), 0,
       				(struct sockaddr*)&sender_addr, &addr_len);
       	if (bytes_received < 0) {
	        perror("Receive failed");
       		continue;
	}

       	struct rsvp_header *rsvp = (struct rsvp_header*)(buffer+20);

	printf("---- %d\n",rsvp->msg_type);
	switch(rsvp->msg_type) {

		case PATH_MSG_TYPE: 
		
			//Receive PATH Message

			// get ip from the received path packet
			get_ip(buffer, &sender_ip, &receiver_ip);
		        reached = dst_reached(receiver_ip);
	
			printf("insert_path_session\n");
	                if(path_head == NULL) {
       		       		 path_head = insert_session(path_head, inet_ntoa(sender_ip), inet_ntoa(receiver_ip), reached);
               		} else {
                       	         insert_session(path_head, inet_ntoa(sender_ip), inet_ntoa(receiver_ip),reached);
                        }
			
			receive_path_message(sock,buffer,sender_addr);	

			break;

		case RESV_MSG_TYPE:

			// Receive RSVP-TE RESV Message
 
			//get ip from the received resv msg
 			get_ip(buffer, &sender_ip, &receiver_ip);
			reached = dst_reached(sender_ip);

                        printf("insert_resv_session\n");
                        if(resv_head == NULL) {
                                resv_head = insert_session(resv_head, inet_ntoa(sender_ip), inet_ntoa(receiver_ip), reached);
                        } else {
                                insert_session(resv_head, inet_ntoa(sender_ip), inet_ntoa(receiver_ip), reached);
                        }
		
			receive_resv_message(sock,buffer,sender_addr);

			break;

	}
    }
    close(sock);
    return 0;
}

