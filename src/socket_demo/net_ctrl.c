#include "net_ctrl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define PORT 8998
#define MAX_PENDING 5
#define MAX_BUF_SIZE 512

uint16_t calculate_checksum(uint8_t *data, int len) {
    uint32_t sum = 0;
    uint16_t word = 0;
    int i;

    // 计算所有16位的和
    for (i = 0; i < len; i += 2) {
        word = ((data[i] << 8) & 0xFF00) + (data[i + 1] & 0xFF);
        sum += word;
    }

    // 如果数据长度是奇数，则处理最后一个字节
    if (len % 2) {
        sum += (data[len - 1] << 8);
    }

    // 将高16位和低16位相加，直到高16位为0
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    // 对和取反，得到校验位
    uint16_t checksum = (uint16_t)(~sum & 0xFFFF);

    return checksum;
}

void serialize_control_packet(uint8_t *buffer, ControlPacket *packet) {
    buffer[0] = (packet->header.type >> 8) & 0x00FF;
    buffer[1] = packet->header.type & 0xFF;

    buffer[2] = (packet->header.length >> 8) & 0x00FF;
    buffer[3] = packet->header.length & 0xFF;

    buffer[4] = (packet->command >> 8) & 0x00FF;
    buffer[5] = packet->command & 0xFF;

    uint16_t checksum = calculate_checksum(buffer, sizeof(ControlPacket) - 2);
    packet->checksum = checksum;

    buffer[6] = (checksum >> 8) & 0x00FF;
    buffer[7] = checksum & 0xFF;
}

void deserialize_control_packet(const uint8_t *buffer, ControlPacket *packet) {
    packet->header.type = ((buffer[0] << 8) & 0xff00) | (buffer[1] & 0x00ff);

    packet->header.length = ((buffer[2] << 8) & 0xff00) | (buffer[3] & 0x00ff);

    packet->command = ((buffer[4] << 8) & 0xff00) | (buffer[5] & 0x00ff);

    packet->checksum = ((buffer[6] << 8) & 0xff00) | (buffer[7] & 0x00ff);
}

int create_server_socket(struct sockaddr_in *address) {
    int server_socket;

    struct sockaddr_in server_address;
    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Failed to create socket\n");
        return -1;
    }

    // Set server address
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(PORT);

    // Bind socket to address
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        printf("Failed to bind socket\n");
        return -1;
    }

    // Listen for connections
    if (listen(server_socket, MAX_PENDING) < 0) {
        printf("Failed to listen for connections\n");
        return -1;
    }

    memcpy(address, (void *)&server_address, sizeof(struct sockaddr_in));

    return server_socket;
}

void *server_thread(void *arg) {
    int server_socket, client_socket;
    int address_length;
    char buffer[MAX_BUF_SIZE] = {0};
    struct sockaddr_in server_address;

    server_socket = create_server_socket(&server_address);

    while (1) {
        printf("Waiting for connections...\n");
        address_length = sizeof(server_address);

        // Accept connection from client
        if ((client_socket = accept(server_socket, (struct sockaddr *)&server_address, (socklen_t *)&address_length)) < 0) {
            printf("Failed to accept connection\n");
            close(server_socket);
            exit(1);
        }

        printf("Client connected: %s:%d\n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));

        if (recv(client_socket, buffer, MAX_BUF_SIZE, 0) < 0) {
            printf("Client disconnected\n");
            close(server_socket);
            exit(1);
        }

        printf("Received response: %s\n", buffer);
        memset(buffer, 0, MAX_BUF_SIZE);

        // other operation

        if (send(client_socket, "Hey Bro! What's up?", 20, 0) < 0) {
            printf("Client disconnected\n");
            close(server_socket);
            exit(1);
        }

        sleep(MAX_PENDING);
    }

    printf("Client reconnected: %s:%d\n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));

    pthread_exit(NULL);

}

int create_client_socket(const char *server_ip_addr) {
    int client_socket;

    do {
        client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket >= 0) {
            break;
        } else {
            printf("recreate socket\n");
        }
    } while (1);
    

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    // inet_pton(AF_INET, server_ip_addr, &server_address.sin_addr);
    server_address.sin_addr.s_addr = inet_addr(server_ip_addr);
    server_address.sin_port = htons(PORT);

    do {
        int connection_status = connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address));
        if (connection_status < 0) {
            printf("connect failed!\n");
        } else {
            break;
        }
    } while (1);

    return client_socket;
}

void *client_thread(void *arg) {
    int ret;
    int client_socket;
    const char *server_ip_addr = (const char *)arg;

    int retry = 0;
    const int MAX_RETRIES = 5;
    int counter = 0;

    uint8_t send_buffer[MAX_BUF_SIZE] = {0};
    uint8_t recv_buffer[MAX_BUF_SIZE] = {0};

    // int cover_state = COVER_NULL;
    int cover_state = COVER_OFF;
    int resend_state = 0;

    ControlPacket control_packet;
    memset(&control_packet, 0, sizeof(control_packet));
    ControlPacket recv_packet;
    memset(&recv_packet, 0, sizeof(recv_packet));
    
    // initialize socket
    client_socket = create_client_socket(server_ip_addr);

    memset(send_buffer, 0, MAX_BUF_SIZE);
    memset(recv_buffer, 0, MAX_BUF_SIZE);

    while (retry < MAX_RETRIES) {
        if (cover_state == COVER_NULL) {
            control_packet.header.type = PACK_TYPE_CTRL;
            control_packet.header.length = sizeof(ControlPacket) - sizeof(PacketHeader);
            control_packet.command = CONTROL_GET_COVER_STATE;
        } else if (cover_state == COVER_OFF) {
            control_packet.header.type = PACK_TYPE_CTRL;
            control_packet.header.length = sizeof(ControlPacket) - sizeof(PacketHeader);
            control_packet.command = CONTROL_ADD_COVER;
        } else if (cover_state == COVER_ON) {
            control_packet.header.type = PACK_TYPE_CTRL;
            control_packet.header.length = sizeof(ControlPacket) - sizeof(PacketHeader);
            control_packet.command = CONTROL_RMV_COVER;
        }

        // printf("size of PacketHeader = %d\n", sizeof(PacketHeader));
        // printf("size of ControlPacket = %d\n", sizeof(ControlPacket));

        serialize_control_packet(send_buffer, &control_packet);

        // sended data
        printf("send_buffer:\n");
        for (int i = 0; i < sizeof(ControlPacket); i++) {
            printf("%#x\n", send_buffer[i]);
        }
        printf("send_checksum = %d\n", control_packet.checksum);

        if (send(client_socket, send_buffer, sizeof(ControlPacket), 0) < 0) {
            printf("Error: Failed to send message.\n");
            retry++;
            continue;
        }

        if (recv(client_socket, recv_buffer, sizeof(recv_buffer), 0) < 0) {
            printf("Error: Failed to receive massage.\n");
            retry++;
            continue;
        }

        printf("recv_buffer: %s\n", recv_buffer);

        if (strncmp(recv_buffer, "COVER ON", 8) == 0) {
            cover_state = COVER_ON;
        } else if (strncmp(recv_buffer, "COVER OFF", 9) == 0) {
            cover_state = COVER_OFF;
            counter++;
            printf("counter = %d\n", counter);
        } else if (strncmp(recv_buffer, "RESEND", 6) == 0) {
            printf("recv_mess: %s\n", recv_buffer);
            retry++;
            continue;
        }

        

        memset(&recv_packet, 0, sizeof(recv_packet));
        memset(recv_buffer, 0, MAX_BUF_SIZE);

        if (resend_state != 0) {
            memset(&control_packet, 0, sizeof(control_packet));
            memset(send_buffer, 0, MAX_BUF_SIZE);
        }

        sleep(MAX_PENDING);
    }

    close(client_socket);

    pthread_exit(NULL);
}

void *check_server_state(void *p) {

}

void *check_client_state(void *p) {

}
