#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/sendfile.h> 
#include <unistd.h>
#include <stdlib.h>

#define BUFF_SIZE 4096
#define PORT 8080


char* get_mime(char *path) {
    if (strstr(path, ".html")) return "text/html";
    if (strstr(path, ".jpg"))  return "image/jpeg";
    if (strstr(path, ".png"))  return "image/png";
    if (strstr(path, ".css"))  return "text/css";
    return "text/plain";
}

void handle_client(int client_fd) {
    char buffer[BUFF_SIZE];
    read(client_fd, buffer, BUFF_SIZE);

    char method[16], path[256];
    sscanf(buffer, "%s %s", method, path);

    // Default to index.html
    if (strcmp(path, "/") == 0)
        strcpy(path, "/index.html");

    char filepath[512] = "sample_website";
    strcat(filepath, path);

    FILE *file = fopen(filepath, "rb");

    if (!file) {
        char *msg =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Length: 13\r\n"
            "\r\n"
            "404 Not Found";
        write(client_fd, msg, strlen(msg));
        return;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);

    char *data = malloc(size);
    fread(data, 1, size, file);
    fclose(file);

    char header[512];
    sprintf(header,
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: %d\r\n"
        "Content-Type: %s\r\n"
        "\r\n",
        size, get_mime(path));

    write(client_fd, header, strlen(header));
    write(client_fd, data, size);

    free(data);
}


// int main()
// {
//     struct sockaddr_in serveraddr;
//     struct sockaddr_in clientaddr;
//     int clientaddrlen;
//     int request_sd, sd;
//     int serverfd, new_socket;

//     request_sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

//     memset(&serveraddr, 0, sizeof(struct sockaddr_in    ));

  
//     int s = socket(AF_INET, SOCK_STREAM, 0);
//     struct sockaddr_in addr = {
//         AF_INET,
//         0x901F,
//         0
//     }
//     bind(s, &addr, sizeof(addr));
//     listen (s, 10);
//     int client_fd =accept(s, 0, 0);

//     char buffer[BUFF_SIZE] = {0};
//     recv(client_fd, buffer , 256, 0);

//     // GET /file.html .....
//     char* f = buffer + 5; 
//     *strchr(f, ' ') = 0;
//     int opened_fd = open(f, O_RDONLY);
    
//     // change 256
//     sendfile(client_fd, opened_fd, 0, 256);
//     close(opened_fd);
//     close(client_fd); 
//     close(s);
// }

// int main(){
//     int server_fd, new_socket;
//     struct sockaddr_in address;
//     int opt = 1;
//     int addrlen = sizeof(address);
//     char buffer[BUFF_SIZE] = {0};

//     // create a socket file descriptor
//     if(server_fd = socket(AF_INET, SOCK_STREAM, 0) < 0)
//     {
//         perror("Socket failed!");
//         exit(EXIT_FAILURE);
//     }
//     // Set socket options
//     if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
//     {
//         perror("Set socket options");
//         exit(EXIT_FAILURE);
//     }

//     // setup server address
//     addr.sin_family = AF_INET;
//     addr.sin_addr.s_addr = INADDR_ANY;
//     addr.sin_port = htons(PORT);

//     if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
//     {
//         perror("Bind failed!");
//         exit(EXIT_FAILURE); 
//     }

//     // Listening for incoming connections
//     if(listen(server_fd, 3) < 0)
//     {
//         perror("Listening failed!");
//         exit(EXIT_FAILURE);
//     }

//     printf("Server is listening on port %d\n", PORT);

//     if((new_socket = accept(server_fd, (struct sockaddr)&address, (socklen_t*)&addrlen)) < 0)
//     {
//         perror("Accept failed!");
//         exit(EXIT_FAILURE);
//     }
//     printf("Connection accepted!\n");

//     // read data from client and print it
//     ssize_t valread;
//     while ((valread = read(new_socket, buffer, BUFF_SIZE)) > 0)
//     {
//         printf("Client: %s", buffer);
//         memset(buffer, 0, sizeof(buffer));
//     }

//     close(new_socket);
//     close(server_fd);
//     printf("Server closed.\n");
//     return 0;
// }




int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[BUFF_SIZE] = {0};

    // 1. Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2. Set socket options to reuse address/port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // 3. Configure server address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 4. Bind socket to port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // 5. Start listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // 6. Accept incoming connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    printf("Connection accepted!\n");

    // 7. Read data from client and respond
    ssize_t valread;
    while ((valread = read(new_socket, buffer, BUFF_SIZE - 1)) > 0) {
        buffer[valread] = '\0';  // Null-terminate received data
        printf("Client: %s", buffer);

        // Send HTTP response back to client
        const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello from server!";
        write(new_socket, response, strlen(response));

        memset(buffer, 0, sizeof(buffer));
    }

    // 8. Clean up
    close(new_socket);
    close(server_fd);
    printf("Server closed.\n");
    return 0;
}

