#include "proxy_parse.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <winsock2.h>     // Use Winsock for socket programming
#include <ws2tcpip.h>    // For sockaddr_in and other structures
#include <windows.h>      // For Windows-specific functions
#include <fcntl.h>
#include <sys/types.h>    // Optional for types
#include <errno.h>        // Use for error handling if needed

#pragma comment(lib, "ws2_32.lib") // Link with Winsock library
#define MAX_CLIENT 10
typedef struct cache_element cache_element;

struct cache_element
{
    char *data;
    int len;
    char *url;
    time_t lru_time_track;
    struct cache_element *next;
};

cache_element *find(char *url);
int add_cache_element(char *data, int size, char *url);
void remove_cache_element();

int port_number = 8080;
int proxy_socketId;
pthread_t tid[MAX_CLIENT];


sem_t semaphore;
pthread_mutex_t lock;

cache_element* head;
int cache_size;

int main(int argc, char* argv[]){
    int client_socketId, client_len;
    struct sockaddr_in server_addr, client_addr; // store the address information, including the host's name, port number, and binary address
    sem_init(&semaphore,0, MAX_CLIENT);

    pthread_mutex_init(&lock, NULL);

    if(argv == 2){
        port_number = atoi(argv[1]);
        // ./proxy 8090

    }else{
        printf("too few agrument");
        exit(1);
    }


    printf("Starting Proxy server at port : %d\n",port_number);
    proxy_socketId = socker(AF_INET, SOCK_STREAM, 0); // TCP
    if(proxy_socketId<0){
        perror("Failed To cretae a socket \n");
        exit(1);
    }
    int reuse = 1;
    if(setsockopt(proxy_socketId, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0){
        perror("setSockOpt failed");
    }

    bzero((char*)&server_addr,sizeof(server_addr)); // by default struct did not clean null value that why we have to clean manually
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_number);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(proxy_socketId, (struct sockaddr*)&server_addr, sizeof(server_addr) < 0)){
        perror("Port is not available ");
        exit(1);
    }
    printf("Binding on port %d\n", port_number);
    int listen_status = listen(proxy_socketId, MAX_CLIENT);
    if(listen_status){
        perror("Error in Listening");
        exit(1);
    }
    int i = 0;
    int Connected_socketId[MAX_CLIENT];
    while(1){
        bzero((char *)&client_addr, sizeof(client_addr));
        client_len = sizeof(client_addr);
        client_socketId = accept(proxy_socketId, (struct sockaddr *)&client_addr, (socklen_t*)&client_len );
        if(client_socketId < 0){
            printf("Not able to Connect");
            exit(1);
        }else {
            Connected_socketId[i] = client_socketId;
        }

        struct sockaddr_in * client_pt = (struct sockaddr_in *)&client_addr;
        struct in_addr ip_addr = client_pt->sin_addr;
        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ip_addr, str, INET_ADDRSTRLEN);
        printf("Client is connected with port number is %d and ip address is %s\n", ntohs(client_addr.sin_port), str);


        pthread_create();
    }
}