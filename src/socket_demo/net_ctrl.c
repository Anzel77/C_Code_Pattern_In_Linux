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

#define PORT 7988
#define MAX_PENDING 10
#define MAX_BUF_SIZE 1024
#define WAIT_INTERVAL 5

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

void serialize_control_packet(uint8_t *buffer, const ControlPacket *packet) {
    uint16_t network_packet_type = htons(packet->header.type);
    memcpy(buffer, &network_packet_type, sizeof(network_packet_type));
    buffer += sizeof(packet->header.type);

    uint16_t network_packet_length = htons(packet->header.length);
    memcpy(buffer, &network_packet_length, sizeof(network_packet_length));
    buffer += sizeof(packet->header.length);

    uint16_t network_command = htons(packet->command);
    memcpy(buffer, &network_command, sizeof(packet->command));
    buffer += sizeof(packet->command);

    uint16_t network_checksum = htons(packet->checksum);
    memcpy(buffer, &network_checksum, sizeof(network_checksum));
}

void deserialize_control_packet(const uint8_t *buffer, ControlPacket *packet) {
    uint16_t network_packet_type;
    memcpy(&network_packet_type, buffer, sizeof(packet->header.type));
    packet->header.type = ntohs(network_packet_type);
    buffer += sizeof(packet->header.type);

    uint16_t network_packet_length;
    memcpy(&network_packet_length, buffer, sizeof(packet->header.length));
    packet->header.length = ntohs(network_packet_length);
    buffer += sizeof(packet->header.length);

    uint16_t network_command;
    memcpy(&network_command, buffer, sizeof(network_command));
    packet->command = ntohs(network_command);
    buffer += sizeof(packet->command);

    uint16_t network_checksum;
    memcpy(&network_checksum, buffer, sizeof(network_checksum));
    packet->checksum = ntohs(network_checksum);
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

        sleep(WAIT_INTERVAL);
    }

    printf("Client reconnected: %s:%d\n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));

    pthread_exit(NULL);

}

int create_client_socket(const char *server_ip_addr) {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, server_ip_addr, &server_address.sin_addr);
    // serv_addr.sin_addr.s_addr = inet_addr(server_ip_addr);
    server_address.sin_port = htons(PORT);


    int connection_status = connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    if (connection_status == -1) {
        printf("connect failed!\n");
        return -1;
    }

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

    int cover_state = COVER_OFF;

    memset(send_buffer, 0, MAX_BUF_SIZE);

    ControlPacket add_cover_pack;
    ControlPacket rmv_cover_pack;
    
    add_cover_pack.header.type = 0x01;
    add_cover_pack.header.length = sizeof(ControlPacket) - sizeof(PacketHeader);
    add_cover_pack.command = CONTROL_ADD_COVER;
    add_cover_pack.checksum = calculate_checksum((uint8_t*)&add_cover_pack, sizeof(add_cover_pack) - 2);

    rmv_cover_pack.header.type = 0x01;
    rmv_cover_pack.header.length = sizeof(ControlPacket) - sizeof(PacketHeader);
    rmv_cover_pack.command = CONTROL_RMV_COVER;
    rmv_cover_pack.checksum = calculate_checksum((uint8_t*)&rmv_cover_pack, sizeof(rmv_cover_pack) - 2);
    

    while (retry < MAX_RETRIES) {
        client_socket = create_client_socket(server_ip_addr);

        memset(send_buffer, 0, MAX_BUF_SIZE);
        if (cover_state == COVER_OFF) {
            serialize_control_packet(send_buffer, &add_cover_pack);
            printf("checksum = %d\n", add_cover_pack.checksum);
        } else if (cover_state == COVER_ON) {
            serialize_control_packet(send_buffer, &rmv_cover_pack);
            printf("checksum = %d\n", rmv_cover_pack.checksum);
        }

        // printf("size of PacketHeader = %d\n", sizeof(PacketHeader));
        // printf("size of ControlPacket = %d\n", sizeof(ControlPacket));
        // for (int i = 0; i < sizeof(ControlPacket); i++) {
        //     printf("%#x\n", send_buffer[i]);
        // }

        ret = send(client_socket, send_buffer, sizeof(ControlPacket), 0);
        if (ret == -1) {
            printf("Error: Failed to send message.\n");
            close(client_socket);
            retry++;
            continue;
        }

        memset(recv_buffer, 0, MAX_BUF_SIZE);
        ret = recv(client_socket, recv_buffer, sizeof(recv_buffer), 0);
        if (ret == -1) {
            printf("Error: Failed to receive massage.\n");
            close(client_socket);
            retry++;
            continue;
        }

        printf("[CLIENT] Received: %s\n", recv_buffer);
        
        if (strncmp(recv_buffer, "COVER ON", 8) == 0) {
            cover_state = COVER_ON;
        } else if (strncmp(recv_buffer, "COVER OFF", 9) == 0) {
            cover_state = COVER_OFF;
            counter++;
            printf("counter = %d\n", counter);
        } else if (strncmp(recv_buffer, "RESEND", 6) == 0) {
            printf("recv_mess: %s\n", recv_buffer);
            retry++;
            close(client_socket);
            continue;
        }

        close(client_socket);
        sleep(MAX_PENDING);
    }

    pthread_exit(NULL);
}

void *check_server_state(void *p) {

}

void *check_client_state(void *p) {

}
