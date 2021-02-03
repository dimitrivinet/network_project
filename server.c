#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define PORT 6666
#define ADDR "172.21.72.124"
#define LISTEN_BACKLOG 50
#define MAX_MSGSIZE 1024
#define MAX_CLIENTS 380


struct accepter_func_args {
    int sockfd;
    uint32_t *clients;
};


struct reader_func_args {
    int serverfd;
    int sockfd;
    char *client_ip;
};

struct broadcaster_func_args {
    int serverfd;
    uint32_t *clients;
};


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


void reader_func(struct reader_func_args *reader_args)
{   
    char* buff = malloc(sizeof(char) * 1024);

    FILE * fp;

    // char filename[] = "file1";
    read(reader_args -> sockfd, buff, 1024);


    char *filepath = malloc(sizeof(char) * 1024);
    sprintf(filepath, "files/%s", buff);

    trim(filepath);

    if (printf("%s\n", filepath) <= 0)
    {
        perror("filepath");
    }

    fp = fopen(filepath, "w+");

    // fp = fopen("files/test", "w+");
    if (fp == NULL)
    {
        printf("\nerreur ouverture fichier\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        // read the message from client and write it on file
        bzero(buff, MAX_MSGSIZE);
        int size_read = read(reader_args -> sockfd, buff, MAX_MSGSIZE);
        // if (printf("line received") <= 0)
        // {
        //     perror("received");
        // }   

        // printf("%s", buff);
        if (size_read <= 0)
        {
            perror("read");
            printf("file received");
            fclose(fp);    
            return;
        }

        fprintf(fp, "%s", buff);
    }   


    return;
}


void accept_func(struct accepter_func_args *accepter_args)
{
    int client_sock;

    struct sockaddr_in client;
    int len = sizeof(client);
    
    while (1)
    {
        // accept connection
        client_sock = accept(accepter_args -> sockfd, (struct sockaddr *)&client, &len);
        if (client_sock < 0)
        {
            printf("server acccept failed...\n");
            perror("accept");
            return;
        }
        else
        {
            char* cli_addr = inet_ntoa(client.sin_addr);
            printf("\nserver acccepted client %s\n", cli_addr);

            struct reader_func_args *reader_args = malloc(sizeof(struct reader_func_args));
            reader_args -> serverfd = accepter_args -> sockfd;
            reader_args -> sockfd = client_sock;
            reader_args -> client_ip = cli_addr;

            pthread_t new_client;
            if (pthread_create(&new_client, NULL, (void *)reader_func, reader_args) != 0)
            {
                printf("new client connection failed...\n");
                perror("new client");
            }
            else
            {
                printf("new client connected.\n\n");
                pthread_detach(new_client);
            }
        }
    }

    return;
}


int server()
{
    struct sockaddr_in servaddr;
    int *clients = malloc(sizeof(int) * MAX_CLIENTS);

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ADDR);
    servaddr.sin_port = htons(PORT);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        perror("socket creation");
        return 1;
    }
    else
    {
        printf("socket created.\n");
    }

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        printf("bind failed...\n");
        perror("bind");
        return 1;
    }
    else
    {
        printf("bind successful.\n");
    }

    if (listen(sockfd, LISTEN_BACKLOG) == -1)
    {
        printf("listen failed...\n");
        perror("listen");
        return 1;
    }
    else
    {
        printf("server listening.\n");
    }


    struct accepter_func_args *accepter_args = malloc(sizeof(struct accepter_func_args));
    accepter_args -> sockfd = sockfd;

    pthread_t accepter_thread;
    if (pthread_create(&accepter_thread, NULL, (void *)accept_func, accepter_args) != 0)
    {
        printf("accepter thread creation failed...\n");
        perror("accepter");
    }
    else
    {
        printf("accepter thread created.\n");
    }


    // struct broadcaster_func_args *broadcaster_args = malloc(sizeof(struct broadcaster_func_args));
    // broadcaster_args -> serverfd = sockfd;
    // broadcaster_args -> clients = clients;

    // pthread_t broadcaster_thread;
    // if (pthread_create(&broadcaster_thread, NULL, (void *)broadcast_func, broadcaster_args) != 0)
    // {
    //     printf("broadcaster thread creation failed...\n");
    //     perror("broadcaster");
    // }
    // else
    // {
    //     printf("broadcaster thread created.\n");
    // }
    

    pthread_join(accepter_thread, 0);
    // pthread_join(broadcaster_thread, 0);

    printf("exiting...\n");
    return 0;
}

int main(void)
{
    server();
    return 0;
}