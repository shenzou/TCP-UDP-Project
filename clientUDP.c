
// udp client driver program 
#include <stdio.h> 
#include <strings.h> 
#include <sys/types.h> 
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include<netinet/in.h> 
#include<unistd.h> 
#include<stdlib.h> 
#include <iostream>
#include <sqlite3.h>
#include <fstream>
#include <chrono>
#include <ctime>
#include <string.h>

#define PORT 5000 
#define MAXLINE 1000 
  
static int whenLoginSuccess()
{
    char buffer[100]; 
    char *message = "Hello Server"; 
    int sockfd, n; 
    struct sockaddr_in servaddr; 
      
    // clear servaddr 
    bzero(&servaddr, sizeof(servaddr)); 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_family = AF_INET; 
      
    // create datagram socket 
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
      
    // connect to server 
    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) 
    { 
        printf("\n Error : Connect Failed \n"); 
        exit(0); 
    } 
  
    // request to send datagram 
    // no need to specify server address in sendto 
    // connect stores the peers IP and port 
    sendto(sockfd, message, MAXLINE, 0, (struct sockaddr*)NULL, sizeof(servaddr)); 
      
    // waiting for response 
    recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL); 
    puts(buffer); 
  
    std::ofstream logFile("log.txt", std::ios::app);
    char *myIP = inet_ntoa(servaddr.sin_addr);
    logFile << ", server IP: ";
    logFile << myIP;
    logFile.close();

    // close the descriptor 
    close(sockfd);


}

static int callback(void *count, int argc, char **argv, char **azColName)
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
    std::ofstream logFile("log.txt", std::ios::app);

    sqlite3 *db;
    char *zErrMsg = 0;
    char *pSQL;
    int rc;
    int count = 0;
    rc = sqlite3_open("logins.db", &db);
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

    //char *username = "alexandre";
    printf("Enter your username:");
    char username[15];
    std::cin.getline(username, 15);
    //char *password = "test";
    printf("Enter your password:");
    char password[15];
    std::cin.getline(password, 15);
    char buff[100];
    char *select = "select count(*) from myTable where username='";
    char *pass = "' and password='";
    char *end = "'";

    strcpy(buff, select);
    strcat(buff, username);
    strcat(buff, pass);
    strcat(buff, password);
    strcat(buff, end);
    pSQL = buff;

    rc = sqlite3_exec(db, pSQL, callback, &count, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        printf("SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_free(zErrMsg);
    }
    if (count)
    {
        logFile << "\nUsername: ";
        logFile << username;
        logFile << ", login Time: ";
        auto end = std::chrono::system_clock::now();
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);
        logFile << std::ctime(&end_time);
        logFile << ", connection type: UDP";
    }
    else{
        logFile << "\nLogin verification failed. Aborting.";
    }
    sqlite3_close(db);
    logFile.close();
    return count;
}

// Driver code 
int main() 
{    
     if (DatabaseConnect())
    {
        whenLoginSuccess();
    }

    return 0;
} 

