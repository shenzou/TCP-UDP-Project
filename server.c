#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>

#define PORT 8080

void cleanExit() { exit(0); }

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1000] = {0};
    char *hello = "Hello from server";
    // Creating socket file descriptor
    //ici tcp utilisé  sinon use  SOCK_DGRA instead SOCK_STREAM
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        for (int i = 0; i < 1000; i++)
        {
            buffer[i] = (char)0;
        }

        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                 (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        //recv()
        valread = read(new_socket, buffer, 1000);
        printf("%s\n", buffer);

        int lgt = 0;
        for (int i = 1000; i > 0; i--)
        {
            if (buffer[i] != ' ' && buffer[i] != (char)0)
            {
                lgt = i + 2;
                break;
            }
        }
        char valRead2[lgt];
        for (int i = 0; i < lgt; i++)
        {
            valRead2[i] = buffer[i];
        }

        //printf("Value of length is%d\n", lgt);

        if (strcmp(valRead2, "Hello from client, starting text chat") == 0)
        {
            char bufferChat[250];
            printf("\nHello from client, starting text chat\n");
            send(new_socket, hello, strlen(hello), 0);
            printf("Hello from server sent.\n");
            int chatRunning = 1;
            while (chatRunning)
            {
                for(int i=0; i<250; i++)
                {
                    bufferChat[i] = 0;
                }
                int serverRead;
                serverRead = read(new_socket, bufferChat, 250);
                printf("%s\n", bufferChat);
                int lgt = 0;
                for (int i = 250; i > 0; i--)
                {
                    if (bufferChat[i] != ' ' && bufferChat[i] != (char)0)
                    {
                        lgt = i + 2;
                        break;
                    }
                }
                char endMessage[lgt];
                for (int i = 0; i < lgt; i++)
                {
                    endMessage[i] = bufferChat[i];
                }
                if (strcmp(endMessage, "#Exit") == 0)
                {
                    chatRunning = 0;
                    char end[250] = "End of chat";
                    send(new_socket, end, strlen(end), 0);
                    break;
                }
                send(new_socket, bufferChat, strlen(bufferChat), 0);
            }
        }
        if (strcmp(valRead2, "Hello from client, get photo") == 0)
        {
            char buffer3[20] = {0};

            FILE *pFile;
            long lSize;
            char *buffer;

            pFile = fopen("photo.jpg", "rb");
            if (pFile == NULL)
            {
                fputs("File error", stderr);
                exit(1);
            }

            // obtain file size:
            fseek(pFile, 0, SEEK_END);
            lSize = ftell(pFile);
            rewind(pFile);
            char *buffer2 = (char *)malloc(sizeof(char) * lSize);
            if (buffer3 == NULL)
            {
                fputs("Memory error", stderr);
                exit(2);
            }
            size_t result;
            // data is now in buffer
            result = fread(buffer2, 1, lSize, pFile);
            if (result != lSize)
            {
                fputs("Reading error", stderr);
                exit(3);
            }
            printf("\nPhoto file loaded in memory.\n");
            fclose(pFile);
            int c = (int)lSize;

            snprintf(buffer3, 20, "%d", c);
            printf("Decimal value = %s   and length = %ld\n", buffer3, strlen(buffer3));

            int value1 = atoi(buffer3);
            printf("Value1 = %d\n", value1);

            printf("Sending buffer containing size.\n");

            send(new_socket, buffer3, 15, 0);

            if (listen(server_fd, 3) < 0)
            {
                perror("listen");
                exit(EXIT_FAILURE);
            }
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                     (socklen_t *)&addrlen)) < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("\nSending buffer containing photo.\n");
            send(new_socket, buffer2, lSize * sizeof(char), 0);

            printf("\nPhoto sent.\n");
        }
        //send(new_socket , hello , strlen(hello) , 0 );

        signal(SIGTERM, cleanExit);
        signal(SIGINT, cleanExit);
    }
    return 0;
}
