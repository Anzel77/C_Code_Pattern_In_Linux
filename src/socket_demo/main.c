#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include "net_ctrl.h"


int main(int argc, char *argv[]) {
    // pthread_t server_socket_id;
    // pthread_create(&server_socket_id, NULL, server_thread, NULL);
    // pthread_join(server_socket_id, NULL);

    const char server_ip_addr[] = "10.0.0.254";
    // const char server_ip_addr[] = "10.0.0.86";
    pthread_t client_socekt_id;
    pthread_create(&client_socekt_id, NULL, client_thread, (void *)server_ip_addr);
    pthread_join(client_socekt_id, NULL);

    return 0;
}
