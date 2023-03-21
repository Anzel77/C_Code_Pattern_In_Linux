#ifndef NET_CTRL_H
#define NET_CTRL_H

#include <stdint.h>

#define COVER_OFF   0x00
#define COVER_ON    0x01

#define MESSAGE_USER_OFFLINE  0x00B0
#define MESSAGE_USER_ONLINE   0x00B1

#define CONTROL_MODIF_URL    0x01A0
#define CONTROL_ADD_COVER    0x01A1
#define CONTROL_RMV_COVER    0x01A2

typedef struct PacketHeader {
    uint16_t type;      // 0x00: string info; 0x01: control info
    uint16_t length;
} PacketHeader;

typedef struct ControlPacket {
    PacketHeader header;
    uint16_t command;    // 0xA0: modify the push_url; 0xA1: add cover; 0xA2: remove cover
    uint16_t checksum;
} ControlPacket;

void *server_thread(void *arg);

void *client_thread(void *arg);

#endif