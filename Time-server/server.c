#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#define PORT 3737 

int main(){

    //Variables 
int server_socket; 
struct sockaddr_in server_addr; 
struct sockaddr_in client_addr; 
socklen_t client_len = sizeof(client_addr);
uint32_t rfc_time; 
char dummy;

    //Create socket
server_socket = socket(AF_INET, SOCK_DGRAM, 0);

if(server_socket < 0) {
    perror("Error creating socket");
    exit(1);
}

    //Prepare server adress
memset(&server_addr, 0, sizeof(server_addr));
server_addr.sin_family = AF_INET;
server_addr.sin_addr.s_addr = INADDR_ANY;
server_addr.sin_port = htons(PORT);

    //Bind socket to port to reach server
if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    perror("Bind failed");
    close(server_socket);
    exit(1);
}

printf("Server is running and waiting for client\n");

while(1) {

    recvfrom(server_socket, &dummy, sizeof(dummy), 0, (struct sockaddr*)&client_addr, &client_len);

    time_t now = time(NULL);
    rfc_time = (uint32_t)(now + 2208988800);
    rfc_time = htonl(rfc_time);

    sendto(server_socket, &rfc_time, sizeof(rfc_time), 0, (struct sockaddr*)&client_addr, client_len);

}
close(server_socket);
return 0;

}
