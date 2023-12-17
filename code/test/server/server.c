#include <stdio.h>     // perror, printf
#include <stdlib.h>    // exit, atoi
#include <unistd.h>    // read, write, close
#include <arpa/inet.h> // sockaddr_in, AF_INET, SOCK_STREAM, INADDR_ANY, socket etc...
#include <string.h>    // memset
#include <ctype.h>     // Viet hoa ki tu

int main(int argc, char const *argv[])
{

    int serverFd, clientFd;
    struct sockaddr_in server, client;
    int len;
    int port = 8080;
    char buffer[1024];
    if (argc == 2)
    {
        port = atoi(argv[1]);
    }
    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0)
    {
        perror("Cannot create socket");
        exit(1);
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    len = sizeof(server);
    if (bind(serverFd, (struct sockaddr *)&server, len) < 0)
    {
        perror("Cannot bind sokcet");
        exit(2);
    }
    if (listen(serverFd, 10) < 0)
    {
        perror("Listen error");
        exit(3);
    }
    while (1)
    {
        len = sizeof(client);
        printf("waiting for clients\n");
        if ((clientFd = accept(serverFd, (struct sockaddr *)&client, &len)) < 0)
        {
            perror("accept error");
            exit(4);
        }
        char *client_ip = inet_ntoa(client.sin_addr);
        printf("Accepted new connection from a client %s:%d\n", client_ip, ntohs(client.sin_port));
        memset(buffer, 0, sizeof(buffer));
        int size = read(clientFd, buffer, sizeof(buffer));
        if (size < 0)
        {
            perror("read error");
            exit(5);
        }
        else
        {
            printf("received %s from client\n", buffer);
            for (int i = 0; i < size; i++)
            {
                buffer[i] = toupper(buffer[i]);
            }
        }
        if (write(clientFd, buffer, size) < 0)
        {
            perror("write error");
            exit(6);
        }
        close(clientFd);
    }
    close(serverFd);
    return 0;
}