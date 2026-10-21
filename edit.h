#ifndef EDIT_H
#define EDIT_H

#include <stdio.h>

/* Structure for edit operation */
typedef struct
{
    char *tag;
    char *new_data;
    char *filename;

} EditInfo;


/* Function declarations */
int edit_tag(EditInfo *editInfo);

#endif