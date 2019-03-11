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

static int serverAnswer()
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client, starting text chat";
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
    printf("Hello message sent\n");
    valread = read(sock, buffer, 10000);

    std::ofstream logFile("log.txt", std::ios::app);
    char *myIP = inet_ntoa(address.sin_addr);
    logFile << ", client IP: ";
    logFile << myIP;
    char *serverIP = inet_ntoa(serv_addr.sin_addr);
    logFile << ", server IP: ";
    logFile << serverIP;
    logFile.close();
    return 0;
}

static int sendPhoto()
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client, get photo";
    long lSize;
    //faut changer 31706 (octets) par la taille variable du fichier donc envoyer les données en deux temps
    char buffer[1448978] = {0};
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
    printf("Hello message sent\n");
    valread = read(sock, buffer, 1448978);

    //le fichier et créer et l'on y écrit les données
    FILE *pFile2;
    pFile2 = fopen("photo2.jpg", "w");
    // 31706 à changer en fonction de la taille de file
    //lSize = ftell (pFile2);
    //rewind (pFile2);
    //fwrite (buffer , lSize,sizeof(char), pFile2);
    fwrite(buffer, 1448978, sizeof(char), pFile2);

    fclose(pFile2);
    //free (buffer);

    std::ofstream logFile("log.txt", std::ios::app);
    char *myIP = inet_ntoa(address.sin_addr);
    logFile << ", client IP: ";
    logFile << myIP;
    char *serverIP = inet_ntoa(serv_addr.sin_addr);
    logFile << ", server IP: ";
    logFile << serverIP;
    logFile.close();
    return 0;
}

static int callback(void *count, int argc, char **argv, char **azColName) //Method used to get the returned values of sqlite request
{
    int *c = count;
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
    char *select = "select count(*) from myTable where username='";
    char *pass = "' and password='";
    char *end = "'";

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

int main(int argc, char const *argv[])
{

    if (DatabaseConnect())
    {
        printf("Enter your choice: 1 for getting photo, 2 for chat");
        int choice;
        char toConvert[4];
        std::cin.get(toConvert, 4);
        sscanf(toConvert, "%d", &choice);
        if(choice == 1)
        {
            sendPhoto();
        }
        if(choice == 2)
        {
            serverAnswer();
        }
    }

    return 0;
}
