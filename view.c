#include <stdio.h>
#include <string.h>
#include "view.h"

FILE *open_file(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("ERROR: unable to open file\n");
        return NULL;
    }
    printf("File opened successfully\n");
    return fp;
}

void read_Tag(FILE *fp, char *tag) {
    fread(tag, 1, 4, fp);
    tag[4] = '\0';
}

void read_size(FILE *fp, unsigned char *size_byte) {
    fread(size_byte, 1, 4, fp);
}

// For the tag header (total tag size, syncsafe integer)
int convert_tag_size(unsigned char *size_bytes) {
    return (size_bytes[0] & 0x7F) << 21 |
           (size_bytes[1] & 0x7F) << 14 |
           (size_bytes[2] & 0x7F) << 7  |
           (size_bytes[3] & 0x7F);
}

// For each frame size (ID3v2.3 uses normal big-endian)
int convert_frame_size(unsigned char *size_bytes) {
    return (size_bytes[0] << 24) |
           (size_bytes[1] << 16) |
           (size_bytes[2] << 8)  |
           (size_bytes[3]);
}

void read_data(FILE *fp, char *data, int size) {
    fread(data, 1, size, fp);
    data[size - 1] = '\0';
}

void display_tag(char *tag, char *data) {
    printf("%s : %s\n", tag, data);
}
