#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 3737 
#define SERVER_IP "127.0.0.1"

int main() {

    int sock_id;  //integer to identify UDP-socket 
    struct sockaddr_in server_addr;  //struct for serveradress

    socklen_t addr_len = sizeof(server_addr);
    uint32_t rfc_time; 
    char dummy = 0;  //trying 1 byte dummy-data

    //Creating socket
    sock_id = socket(AF_INET, SOCK_DGRAM, 0);

    if(sock_id < 0) {
       perror("Error creating socket");
        exit(1);           
    }
    //Prepare serveradress
    memset(&server_addr, 0, sizeof(server_addr)); //empty stuct 
    server_addr.sin_family = AF_INET; 
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    //Send empty UDP-package 
    sendto(sock_id, &dummy, sizeof(dummy),0,
        (struct sockaddr*)&server_addr, addr_len); 
    
        printf("Client: sent request, waiting for reply\n");

        //Recieve time
    recvfrom(sock_id, &rfc_time, sizeof(rfc_time), 0, NULL, NULL); 
    
    rfc_time = ntohl(rfc_time);

    printf("Server time %u seconds since 1900-01-01 00:00 \n", rfc_time);

        close(sock_id);
        return 0;
    
}