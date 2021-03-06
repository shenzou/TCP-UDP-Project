#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
//SQLite
#include <iostream>
#include <sqlite3.h>
#include <fstream>
#include <chrono>
#include <ctime>

#define PORT 8080

int Initialize();

static int serverAnswer()
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = (char *)"Hello from client, starting text chat";
    long lSize;
    char buffer[1000] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    send(sock, hello, strlen(hello), 0);
    printf("\nHello message sent\n");
    valread = read(sock, buffer, 10000);
    printf("%s\n", buffer);

    int chatRunning = 1;

    char bufferChat[250];

    while (chatRunning)
    {
        for (int i = 0; i < 250; i++)
        {
            bufferChat[i] = 0;
        }
        std::cin.ignore();
        printf("\nEnter message to send: (Type #Help for more commands)\n");
        char *message = (char *)malloc(250);

        std::cin.get(message, 250);
        std::cout.flush();
        int lgt = 0;
        for (int i = 250; i > 0; i--)
        {
            if (message[i] != ' ' && message[i] != (char)0)
            {
                lgt = i + 2;
                break;
            }
        }
        char endMessage[lgt] = {0};
        for (int i = 0; i < lgt; i++)
        {
            endMessage[i] = message[i];
        }

        if (strcmp(endMessage, "") == 0)
        {
            std::ofstream logFile("log.txt", std::ios::app);
            char *myIP = inet_ntoa(address.sin_addr);
            logFile << ", client IP: ";
            logFile << myIP;
            char *serverIP = inet_ntoa(serv_addr.sin_addr);
            logFile << ", server IP: ";
            logFile << serverIP;
            logFile << ", data type: chat";
            logFile.close();
            printf("No entry, back to the main menu.\n");
            return 0;
        }
        if (!strcmp(endMessage, "#Exit")/* || endMessage == NULL*/)
        {
            chatRunning = 0;
        }
        else if (!strcmp(endMessage, "#Help"))
        {
            printf("\n#Exit : Exit the chat, go back to the main menu.\n#Help : List all the commands.\n");
        }
        else
        {
            send(sock, message, strlen(message), 0);
            for (int i = 0; i < strlen(bufferChat); i++)
            {
                bufferChat[i] = 0;
            }
            valread = read(sock, bufferChat, 250);
            printf("'%s' received from server\n", bufferChat);
        }      

        free(message);
    }

    std::ofstream logFile("log.txt", std::ios::app);
    char *myIP = inet_ntoa(address.sin_addr);
    logFile << ", client IP: ";
    logFile << myIP;
    char *serverIP = inet_ntoa(serv_addr.sin_addr);
    logFile << ", server IP: ";
    logFile << serverIP;
    logFile << ", data type: chat";
    logFile.close();
    return 0;
}

static int sendPhoto()
{
    struct sockaddr_in address;
    int sock = 0, valread, valread2;
    struct sockaddr_in serv_addr;
    char *hello = (char *)"Hello from client, get photo";
    long lSize;
    //faut changer 31706 (octets) par la taille variable du fichier donc envoyer les données en deux temps
    char buffer[20];
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    send(sock, hello, strlen(hello), 0);
    printf("Ask for photo sent\n");

    valread = read(sock, buffer, 20);

    printf("Received size of file: %s\n", buffer);

    int value = atoi(buffer);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    char *buffer2 = (char *)malloc(value);
    valread2 = read(sock, buffer2, value);

    //le fichier et créer et l'on y écrit les données
    FILE *pFile2;
    pFile2 = fopen("photo2.jpg", "wb");
    // 31706 à changer en fonction de la taille de file
    //lSize = ftell (pFile2);
    //rewind (pFile2);
    //fwrite (buffer , lSize,sizeof(char), pFile2);
    fwrite(buffer2, value, sizeof(char), pFile2);

    fclose(pFile2);
    //free (buffer);
    printf("Photo file written.\n");

    std::ofstream logFile("log.txt", std::ios::app);
    char *myIP = inet_ntoa(address.sin_addr);
    logFile << ", client IP: ";
    logFile << myIP;
    char *serverIP = inet_ntoa(serv_addr.sin_addr);
    logFile << ", server IP: ";
    logFile << serverIP;
    logFile << ", data type: File sending";
    logFile.close();
    return 0;
}

static int callback(void *count, int argc, char **argv, char **azColName) //Method used to get the returned values of sqlite request
{
    int *c = (int *)count;
    *c = atoi(argv[0]);

    /*
    int i;
	for(i=0; i<argc; i++)
	{
		cout<<azColName[i]<<" = " << (argv[i] ? argv[i] : "NULL")<<"\n";
	}
	cout<<"\n";
    */
    return 0;
}

static int DatabaseConnect()
{
    std::ofstream logFile("log.txt", std::ios::app); //Création ou ouverture du fichier de log. ::app pour dire qu'on n'écrase pas le contenu du fichier existant.

    sqlite3 *db;
    char *zErrMsg = 0;
    char *pSQL;
    int rc;
    int count = 0;
    rc = sqlite3_open("logins.db", &db); //Ouverture du fichier contenant la base de données.
    if (rc)
    {
        printf("Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return (1);
    }
    else
    {
        printf("Open database successfully\n\n");
    }

    //Demande à l'utilisateur son id et mot de passe
    printf("Enter your username:");
    char username[15];
    std::cin.getline(username, 15);

    printf("Enter your password:");
    char password[15];
    std::cin.getline(password, 15);

    //Requête SQL
    char buff[100];
    char *select = (char *)"select count(*) from myTable where username='";
    char *pass = (char *)"' and password='";
    char *end = (char *)"'";

    //Concaténation de la requête
    strcpy(buff, select);
    strcat(buff, username);
    strcat(buff, pass);
    strcat(buff, password);
    strcat(buff, end);
    pSQL = buff;

    rc = sqlite3_exec(db, pSQL, callback, &count, &zErrMsg); //Envoi de la requête
    if (rc != SQLITE_OK)
    {
        printf("SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_free(zErrMsg);
    }

    //Ecriture dans le log de la connexion en cas de succès (count est true puisqu'on a une seule ligne en retour SQL (count))
    if (count)
    {
        logFile << "\nUsername: ";
        logFile << username;
        logFile << ", login Time: ";
        auto end = std::chrono::system_clock::now();
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);
        logFile << std::ctime(&end_time);
        logFile << ", connection type: TCP";
    }
    else
    {
        logFile << "\nLogin verification failed. Aborting.";
    }

    sqlite3_close(db);
    logFile.close();
    return count;
}

int Initialize()
{
    if (DatabaseConnect())
    {
        int i = 0;
        int choice;
        while (i == 0)
        {
            printf("\nEnter your choice: 1 for getting photo, 2 for chat\n");
            /*
            char toConvert[4];
            std::cin.get(toConvert, 4);
            sscanf(toConvert, "%d", &choice);
            */
            try
            {
                scanf("%d", &choice);
            }
            catch (...)
            {
                printf("Wrong entry.");
                return 0;
            }
            if (choice == 1)
            {
                i = sendPhoto();
            }
            else if (choice == 2)
            {
                i = serverAnswer();
            }
            else
            {
                printf("This choice doesn't exist.");
                return 0;
            }
        }
    }
    return 0;
}

int main(int argc, char const *argv[])
{
    Initialize();

    return 0;
}
