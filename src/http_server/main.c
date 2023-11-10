#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "http_server.h"

int main(int argc, char const* argv[]) {
    printf("====== Start HTTP server ======\n");
    int                server_sock = -1;
    uint16_t           port; // 监听端口号
    int                client_sock = -1;
    struct sockaddr_in client_name;
    socklen_t          client_name_len = sizeof(client_name);
    // pthread_t          new_thread;

    if (argc < 2) {
        printf("program {port}\n");
        return -1;
    }

    sscanf(argv[1], "%hu", &port);

    server_sock = startup(&port); // 服务器端监听套接字设置
    printf("http running on port %d\n", port);

    // while (1) {
    //     client_sock = accept(server_sock, (struct sockaddr *)&client_name, &client_name_len);
    //     if (client_sock == -1) {
    //         if (errno == EWOULDBLOCK || errno == EAGAIN) {
    //             // 没有等待中的连接请求，继续循环
    //             usleep(1000 * 10);
    //             continue;
    //         } else {
    //             perror("accept");
    //             exit(EXIT_FAILURE);
    //         }
    //     }

    //     pthread_t request_id;
    //     if (pthread_create(&request_id, NULL, accept_request, (void*)&client_sock) != 0)
    //         error_die("accept_request");
    //     usleep(10);

    //     // accept_request(client_sock);
    //     // close(client_sock);
    //     // printf("HTTP client disconnected.\n");
    // }

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr*)&client_name, &client_name_len);
        if (client_sock == -1) {
            error_die("accept");
        } else {
            // printf("HTTP client connected.\n");
        }
        try_accept_request(client_sock);
    }

    close(server_sock);

    return 0;
}
