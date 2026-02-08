#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFF_SIZE 4096
#define PORT 8080

char* get_mime(char *path)
{
    if(strstr(path, ".html")) return "text/html";
    if(strstr(path, ".jpg")) return "image/jpeg";
    if(strstr(path, ".png")) return "image/png";
    if(strstr(path, ".css")) return "text/css";
    return "text/plain";
}

void parse_request(char *buffer, char *method, char *path)
{
    sscanf(buffer, "%15s %255s", method, path);
}

int main()
{
    // socket()
    int sd, new_socket;
    struct sockaddr_in addr, client_addr;
    int opt = 1;
    socklen_t addrlen = sizeof(addr);
    char buffer[BUFF_SIZE] = {0};


    if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error while creating a socket!\n");
        exit(EXIT_FAILURE);
    }

    // Set socket options to reuse address/port
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    // bind()
    if(bind(sd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
    {
        perror("Error in binding!\n");
        exit(EXIT_FAILURE);
    }

    // listen()
    if(listen(sd, 3) < 0)
    {
        perror("Error in listening!\n");
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d\n", PORT);

    // accept()
    while(1)
    {
        if((new_socket = accept(sd, (struct sockaddr *) &client_addr, &addrlen)) < 0)
        {
            perror("Error in accepting!\n");
            exit(EXIT_FAILURE);
        }
        printf("Connection accepted!\n");
        
        
        // read()
        ssize_t value = read(new_socket, buffer, BUFF_SIZE - 1);
        if(value > 0)
        {
            buffer[value] = '\0';
            printf("Client: %s", buffer);
            
            char method[16];
            char path[256];

            parse_request(buffer, method, path);
            printf("Method: %s\n", method);
            printf("Path: %s\n", path);

            if(strcmp(path, "/") == 0)
                strcpy(path, "/index.html");
            
            char filepath[512] = "sample_website";
            strcat(filepath, path);

            FILE *file = fopen(filepath, "rb");

            if(!file)
            {
                char *msg = "HTTP/1.1 404 Not Found\r\n"
                "Content-Length: 13\r\n"
                "\r\n"
                "404 Not Found";

                write(new_socket, msg, strlen(msg));
            } else {
                    fseek(file, 0, SEEK_END);
                    int filesize = ftell(file);
                    rewind(file);

                    char *data = malloc(filesize);
                    fread(data, 1, filesize, file);
                    fclose(file);

                    char header[512];
                    sprintf(header, 
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Length: %d\r\n"
                    "Content-Type: %s\r\n"
                    "\r\n",
                    filesize, get_mime(path));

                    write(new_socket, header, strlen(header));
                    write(new_socket, data, filesize);
                    free(data);
                }
        }
        memset(buffer, 0, sizeof(buffer));
            
        // close()
        close(new_socket);
    }

    close(sd);
    printf("Server closed!\n");
    return 0;
}