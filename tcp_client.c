#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

int main()
{
    struct sockaddr_in serveraddr;
    int sd;
    char buf[13];

    sd = socket(PF_INET, 
                SOCK_STREAM,
                IPPROTO_TCP);

    memset(&serveraddr, 0, sizeof(struct sockaddr_in));

    serveraddr.sin_family = AF_INET;

inet_pton(AF_INET, "xx.xx.xx.xx", &serveraddr.sin_addr);

serveraddr.sin_port = htons(2009);

connect(sd, (struct sockaddr*) &serveraddr, sizeof(struct sockaddr_in));

write(sd, "Hello World!", 12);

read(sd, buf, 12);

buf[12] = '\0';
printf("%s\n", buf);

close(sd);

}