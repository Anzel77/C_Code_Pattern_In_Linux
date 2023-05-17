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

#include "common/common.h"

#define PORT 8998
#define MAX_PENDING 5
#define MAX_BUF_SIZE 1024

#include <stdint.h>

static const uint8_t crc8Table[256] = {
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};

uint8_t CalculateCRC8(const uint8_t* data, size_t length) {
    uint8_t crc = 0;

    for (size_t i = 0; i < length; i++) {
        crc = crc8Table[crc ^ data[i]];
    }

    return crc;
}


uint8_t calculate_checksum(uint8_t *data, int len) {
    uint32_t sum = 0;
    uint16_t word = 0;
    int i;

    // 计算所有16位的和
    for (i = 0; i < len; i += 2) {
        word = ((data[i] << 8) & 0xFF00) + (data[i + 1] & 0xFF);
        sum += word;
        printf("sum = %x\n", sum);
    }

    // 如果数据长度是奇数，则处理最后一个字节
    if (len % 2) {
        sum += ((uint32_t)(data[len - 1]) << 8) & 0xffff;
    }
    printf("sum = %x\n", sum);

    // 将高16位和低16位相加，直到高16位为0
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    printf("sum = %x\n", sum);

    // 对和取反，得到校验位
    uint8_t checksum = (uint8_t)(~sum & 0xFF);

    return checksum;
}
void serialize_control_packet(uint8_t *buffer, ControlPacket *packet) {
    buffer[0] = (packet->header.type >> 8) & 0x00FF;
    buffer[1] = packet->header.type & 0xFF;

    buffer[2] = (packet->header.length >> 8) & 0x00FF;
    buffer[3] = packet->header.length & 0xFF;

    buffer[4] = (packet->command >> 8) & 0x00FF;
    buffer[5] = packet->command & 0xFF;

    // uint8_t checksum = calculate_checksum(buffer, sizeof(ControlPacket) - 2);
    uint8_t checksum = CalculateCRC8(buffer, sizeof(ControlPacket) - 2);
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
    socklen_t address_length;
    struct sockaddr_in server_address;

    int cover_state;
    const int cover_state_on = COVER_ON;
    const int cover_state_off = COVER_OFF;

    // uint8_t recv_buffer[MAX_BUF_SIZE];
    // memset(recv_buffer, 0, MAX_BUF_SIZE);
    uint8_t send_buffer_c[MAX_BUF_SIZE];
    memset(send_buffer_c, 0, MAX_BUF_SIZE);

    int resend_state = 0;

    do {
        server_socket = create_server_socket(&server_address);
    } while (server_socket < 0);
    printf("Waiting for connections...\n");

    address_length = sizeof(server_address);

    while (1) {
        // Accept connection from client
        if ((client_socket = accept(server_socket, (struct sockaddr *)&server_address, &address_length)) < 0) {
            printf("Failed to accept connection\n");
            continue;
        }
        printf("Client connected: %s:%d\n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));

        while (1) {
            uint8_t recv_buffer[MAX_BUF_SIZE];
            memset(recv_buffer, 0, MAX_BUF_SIZE);
            uint8_t send_buffer[MAX_BUF_SIZE];
            memset(send_buffer, 0, MAX_BUF_SIZE);
            ControlPacket recv_packet;
            memset(&recv_packet, 0, sizeof(recv_packet));
            ControlPacket send_packet;
            memset(&send_packet, 0, sizeof(send_packet));

            if (recv(client_socket, recv_buffer, MAX_BUF_SIZE, 0) <= 0) {
                printf("Client disconnected\n");
                break;
            }
            printf("recv_buffer: \n");
            for (int i = 0; i < sizeof(ControlPacket); i++) {
                printf("%02x\n", recv_buffer[i]);
                if (i % 16 == 15) printf("\n");
            }

            // 将数组中的内容保存近本地结构体
            deserialize_control_packet(recv_buffer, &recv_packet);

            printf("length = %d\n", recv_packet.header.length);

            // uint8_t checksum = calculate_checksum(recv_buffer, sizeof(ControlPacket) - 2);
            uint8_t checksum = CalculateCRC8(recv_buffer, sizeof(ControlPacket) - 2);
            if (checksum != recv_packet.checksum) {
                printf("the received packet is wrong! Please resend.\n");
                sprintf(send_buffer, "RESEND");

                if (send(client_socket, send_buffer, strlen(send_buffer), 0) < 0) {
                    printf("Client disconnected\n");
                    break;
                }

                break;
            } else {
                if (recv_packet.header.type == PACK_TYPE_CTRL) {
                    if (recv_packet.command == CONTROL_ADD_COVER) {
                        // set_cover_switch(&cover_state_on);
                        sprintf(send_buffer, "COVER ON");
                    } else if (recv_packet.command == CONTROL_RMV_COVER) {
                        // set_cover_switch(&cover_state_off);
                        sprintf(send_buffer, "COVER OFF");
                    } else if (recv_packet.command == CONTROL_GET_COVER_STATE) {
                        // get_cover_state(&cover_state);
                        send_packet.header.type = PACK_TYPE_MESG;
                        send_packet.header.length = sizeof(ControlPacket) - sizeof(PacketHeader);
                        if (cover_state == COVER_ON) {
                            send_packet.command = COVER_ON;
                        } else if (cover_state == COVER_OFF) {
                            send_packet.command = COVER_OFF;
                        }
                        // send_packet.checksum = calculate_checksum((uint8_t*)&send_packet, sizeof(send_packet) - 2);
                        send_packet.checksum = CalculateCRC8((uint8_t*)&send_packet, sizeof(send_packet) - 2);
                        serialize_control_packet(send_buffer, &send_packet);
                    }
                }
            }
            
            if (send(client_socket, send_buffer, strlen(send_buffer), 0) < 0) {
                printf("Client disconnected\n");
                break;
            }
            // sleep(MAX_PENDING);
        }

        close(client_socket);
    }

    close(server_socket);
    printf("close server socket\n");

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
            sleep(5);
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

    uint8_t cover_state = COVER_OFF;
    int resend_state = 0;

    ControlPacket control_packet;
    memset(&control_packet, 0, sizeof(control_packet));
    ControlPacket recv_packet;
    memset(&recv_packet, 0, sizeof(recv_packet));

    while (1) {
        // initialize socket
        client_socket = create_client_socket(server_ip_addr);

        memset(send_buffer, 0, MAX_BUF_SIZE);
        memset(recv_buffer, 0, MAX_BUF_SIZE);

        int timestamp = 0;

        while (1) {
            timestamp++;
            if (timestamp % 10 == 0) {
                cover_state = (cover_state == COVER_OFF) ? COVER_ON : COVER_OFF;
            }
            // if (cover_state == COVER_NULL) {
            //     control_packet.header.type = PACK_TYPE_CTRL;
            //     control_packet.header.length = 8;
            //     control_packet.command = CONTROL_GET_COVER_STATE;
            // } else if (cover_state == COVER_OFF) {
            //     control_packet.header.type = PACK_TYPE_CTRL;
            //     control_packet.header.length = 8;
            //     control_packet.command = CONTROL_ADD_COVER;
            // } else if (cover_state == COVER_ON) {
            //     control_packet.header.type = PACK_TYPE_CTRL;
            //     control_packet.header.length = 8;
            //     control_packet.command = CONTROL_RMV_COVER;
            // }

            // printf("size of PacketHeader = %d\n", sizeof(PacketHeader));
            // printf("size of ControlPacket = %d\n", sizeof(ControlPacket));

            // serialize_control_packet(send_buffer, &control_packet);

            uint32_t offset = 0;
            uint16_t len = 10;
            PutByteStream(send_buffer, PACK_TYPE_HERT, 2, &offset);
            PutByteStream(send_buffer, len, 2, &offset);
            PutByteStream(send_buffer, timestamp, 4, &offset);
            PutByteStream(send_buffer, cover_state, 1, &offset);
            // uint8_t checksum = calculate_checksum(send_buffer, len - 1);
            uint8_t checksum = CalculateCRC8(send_buffer, len - 1);
            PutByteStream(send_buffer, checksum, 1, &offset);

            // sended data
            printf("send_buffer:\n");
            for (int i = 0; i < len; i++) {
                printf("%02x ", send_buffer[i]);
                if (i % 16 == 15) printf("\n");
            }
            printf("\n---\n");

            printf("send_checksum = %d\n", checksum);

            if (send(client_socket, send_buffer, len, 0) <= 0) {
                printf("Error: Failed to send message.\n");
                break;
            }

            if (recv(client_socket, recv_buffer, sizeof(recv_buffer), 0) < 0) {
                printf("Error: Failed to receive massage.\n");
                break;
            }

            printf("recv_buffer: %s\n", recv_buffer);

            // if (strncmp(recv_buffer, "COVER ON", 8) == 0) {
            //     cover_state = COVER_ON;
            // } else if (strncmp(recv_buffer, "COVER OFF", 9) == 0) {
            //     cover_state = COVER_OFF;
            //     counter++;
            //     printf("counter = %d\n", counter);
            // } else if (strncmp(recv_buffer, "RESEND", 6) == 0) {
            //     printf("recv_mess: %s\n", recv_buffer);
            //     retry++;
            //     continue;
            // }

            // memset(&recv_packet, 0, sizeof(recv_packet));
            memset(recv_buffer, 0, MAX_BUF_SIZE);

            if (resend_state == 0) {
                // memset(&control_packet, 0, sizeof(control_packet));
                memset(send_buffer, 0, MAX_BUF_SIZE);
            }

            sleep(MAX_PENDING);
        }

        close(client_socket);
        sleep(10);

    }
    pthread_exit(NULL);
}

void *check_server_state(void *p) {

}

void *check_client_state(void *p) {

}
