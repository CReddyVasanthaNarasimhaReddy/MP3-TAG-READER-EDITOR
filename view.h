#ifndef VIEW_H
#define VIEW_H

#include<stdio.h>
#include<string.h>



typedef struct
{
    char title[100];
    char artist[100];
    char album[100];
    char year[5];
    char content[100];
    char composer[100];
}ID3v2Tag;
FILE *open_file(const char *filename);
void read_Tag(FILE *fp, char *tag);
void read_size(FILE *fp, unsigned char *size_byte);
int convert_tag_size(unsigned char *size_bytes);
int convert_frame_size(unsigned char *size_bytes);
void read_data(FILE *fp, char *data, int size);
void display_tag(char *tag, char *data);

#endif