#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <unistd.h>
#include <ctype.h>

#define PORT 6666
#define ADDR "172.21.72.124"
#define MAX 1024


char *ltrim(char *s)
{
    while(isspace(*s)) s++;
    return s;
}

char *rtrim(char *s)
{
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

char *trim(char *s)
{
    return rtrim(ltrim(s)); 
}


void func(int sockfd) 
{ 
    char *buff = malloc(MAX); 
    int n; 
    bzero(buff, sizeof(buff)); 
    printf("Enter file name : ");

    fgets(buff, MAX, stdin); //get input

    printf("buff: %s\n", buff);

    char *filepath = malloc(sizeof(char) * 36);
    sprintf(filepath, "%s", buff);

    trim(filepath);
    printf("\nfilepath: %s\n", filepath);

    // write(sockfd, buff, sizeof(buff)); //write to socket
    write(sockfd, filepath, sizeof(buff));

    if ((strncmp(buff, "exit", 4)) == 0) { 
        printf("Client Exit...\n"); 
        return;
    }  

    


    FILE * fp = fopen(filepath, "r");
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        printf("Retrieved line of length %zu:\n", read);
        printf("%s\n", line);

        if (write(sockfd, line, MAX) < 0)
        {
            perror("write to server sock\n");
            // continue;
        }
    }

    fclose(fp);
    if (line)
        free(line);
    exit(EXIT_SUCCESS);



    // FILE * fp = fopen(filepath, "r");

    // char * line = NULL;
    // size_t len = 1024;
    // ssize_t read;

    // if (fp == NULL)
    // {
    //     printf("\nerreur ouverture fichier\n");
    //     exit(EXIT_FAILURE);
    // }

    // char *newline;
    // while ((read = getline(&buff, &len, fp)) != -1) 
    // {
    //     printf("%s", line);
    //     newline = malloc(sizeof(line) + 4);
    //     sprintf(newline, "%s\r\n", line);
        
    //     if (write(sockfd, newline, sizeof(line)) < 0)
    //     {
    //         perror("write to server sock\n");
    //         // continue;
    //     }
    //     sleep(1);
    // } 

    // printf("\n\n");

    // free(newline);

    // fclose(fp);
    // if (line)
    //     free(line);

    printf("file sent\n");
    
} 

int client()
{
    struct sockaddr_in servaddr, cli; 

    // create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
    {
        printf("socket successfully created.\n");
    }

    //connect
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr(ADDR); 
    servaddr.sin_port = htons(PORT); 
  
    // connect the client socket to server socket 
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        printf("connected to the server..\n"); 
  
    // function for chat 
    func(sockfd); 
  
    // close the socket 
    close(sockfd); 

    return 0;
}


int main(void)
{
    client();
    return 0;
}