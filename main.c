/*Project    : MP3 Tag Reader & Editor
Author       : C.REDDY VASANTHA NARASIMHA REDDY
Batch ID     : 26015_093
Language     : C
File         : main.c

DESCRIPTION
-----------
    MP3 Tag Reader & Editor is a C-based utility that reads, displays, and edits
the metadata embedded inside an MP3 audio file. This metadata, known as ID3 tags,
stores details such as Title, Artist, Album, Year, Genre, Track Number, Comment,
and Cover Art.

    An MP3 file can contain two types of ID3 tags:

    1. ID3v1 - A fixed 128-byte metadata block located at the end of the file.
    2. ID3v2 - A variable-size, frame-based metadata block located at the
               beginning of the file.

    ID3v2 supports different versions, including:

    2.2 - Uses 3-byte frame identifiers and 3-byte frame sizes.
    2.3 - Uses 4-byte frame identifiers and big-endian 4-byte frame sizes.
    2.4 - Uses 4-byte frame identifiers and synchsafe 4-byte frame sizes.

FEATURES
--------

VIEW MODE
---------
    - Reads and displays ID3v1 metadata such as Title, Artist, Album, Year,
      Comment, and Genre.
    - Reads and displays supported ID3v2 text frames.
    - Displays available picture information stored in APIC frames.
    - Detects and displays both ID3v1 and ID3v2 tags when present.

EDIT MODE
---------
    - Allows the user to select and edit one metadata field at a time.
    - Updates the selected metadata value in the supported ID3 tags.
    - Handles metadata frames and their size information during modification.
    - Preserves the remaining metadata and audio content during editing.

IMPLEMENTATION
--------------
    - Developed using the C programming language.
    - Uses binary file-handling functions to read and modify MP3 files.
    - Processes ID3 headers, frame identifiers, and frame sizes.
    - Uses functions and modular programming to organize the implementation.

OBJECTIVE
---------
    To develop a utility that enables users to view and modify MP3 metadata
efficiently while gaining practical experience in C programming, binary file
handling, and metadata processing.

CONCLUSION
----------
    The MP3 Tag Reader & Editor demonstrates the practical use of C programming
concepts to access and modify metadata in MP3 audio files. It provides a
structured approach to reading, displaying, and editing ID3 tags.*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "view.h"
#include "edit.h"

// Debug helper: print raw hex
void hex_dump(const char *data, int size) {
    for (int i = 0; i < size; i++) {
        printf("%02X ", (unsigned char)data[i]);
    }
    printf("\n");
}

// Unified text frame printer (handles ISO-8859-1 and UTF-16)
void print_text_frame(const char *label, char *data, int frame_size) {
    if (frame_size < 1) {
        printf("%s\n", label);
        return;
    }

    unsigned char encoding = data[0];
    if (encoding == 0x00) {
        char *start = data + 1;
        if (*start == '\0' && frame_size > 2) {
            start = data + 2; // skip extra null if present
        }
        printf("%s%s\n", label, start);
    } else if (encoding == 0x01) {
        printf("%s", label);
        for (int i = 1; i < frame_size - 1; i += 2) {
            if (data[i] == 0 && data[i+1] == 0) break;
            putchar(data[i]);
        }
        putchar('\n');
    } else {
        printf("%s%s\n", label, data + 1);
    }
}

// Helper to view all tags (used in both choice 1 and after editing)
void view_all_tags(const char *filename) {
    FILE *fp = open_file(filename);
    if (!fp) return;

    char header[10];
    fread(header, 1, 10, fp);

    if (memcmp(header, "ID3", 3) != 0) {
        printf("No ID3v2 tag found.\n");
        fclose(fp);
        return;
    }

    unsigned char tag_size_bytes[4];
    memcpy(tag_size_bytes, &header[6], 4);
    int tag_size = convert_tag_size(tag_size_bytes);

    printf("\nID3v2.%d.%d tag found, size = %d bytes\n\n",
           header[3], header[4], tag_size);

    printf("--- ID3v2.3 Metadata ---\n");

    int bytes_read = 0;
    while (bytes_read < tag_size) {
        char frame_id[5];
        unsigned char frame_size_bytes[4];
        unsigned char frame_flags[2];

        if (fread(frame_id, 1, 4, fp) != 4) break;
        frame_id[4] = '\0';
        if (frame_id[0] == 0) break;

        fread(frame_size_bytes, 1, 4, fp);
        int frame_size = convert_frame_size(frame_size_bytes);

        fread(frame_flags, 1, 2, fp);
        if (frame_size <= 0 || frame_size > 10000) break;

        char data[1024];
        fread(data, 1, frame_size, fp);
        data[frame_size] = '\0';

        printf("%s : ", frame_id);
        print_text_frame("", data, frame_size);

        bytes_read += 10 + frame_size;
    }

    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    int running = 1;
    while (running) {
        char choice;
        printf("\n--- MP3 Tag Reader/Editor ---\n");
        printf("1. View tags (all frames)\n");
        printf("2. Edit a tag\n");
        printf("3. Debug raw frames\n");
        printf("4. Exit\n");
        printf("Enter choice (1/2/3/4): ");
        scanf(" %c", &choice);

        if (choice == '1') {
            view_all_tags(argv[1]);
        }
        else if (choice == '2') {
            char tag[5];
            char new_value[256];

            printf("Enter tag to edit (e.g., TIT2, TPE1, TALB, TYER, TCON, COMM): ");
            scanf("%4s", tag);

            printf("Enter new value: ");
            getchar(); // clear newline
            fgets(new_value, sizeof(new_value), stdin);
            new_value[strcspn(new_value, "\n")] = '\0';

            EditInfo info;
            info.filename = argv[1];
            info.tag = tag;
            info.new_data = new_value;

            if (edit_tag(&info)) {
                printf("Tag %s updated to \"%s\" successfully!\n", info.tag, info.new_data);
                // 👇 Immediately show all tags after editing
                view_all_tags(argv[1]);
            } else {
                printf("Failed to update tag %s.\n", info.tag);
            }
        }
        else if (choice == '3') {
            FILE *fp = open_file(argv[1]);
            if (!fp) continue;

            char header[10];
            fread(header, 1, 10, fp);

            if (memcmp(header, "ID3", 3) != 0) {
                printf("No ID3v2 tag found.\n");
                fclose(fp);
                continue;
            }

            unsigned char tag_size_bytes[4];
            memcpy(tag_size_bytes, &header[6], 4);
            int tag_size = convert_tag_size(tag_size_bytes);

            int bytes_read = 0;
            while (bytes_read < tag_size) {
                char frame_id[5];
                unsigned char frame_size_bytes[4];
                unsigned char frame_flags[2];

                if (fread(frame_id, 1, 4, fp) != 4) break;
                frame_id[4] = '\0';
                if (frame_id[0] == 0) break;

                fread(frame_size_bytes, 1, 4, fp);
                int frame_size = convert_frame_size(frame_size_bytes);

                fread(frame_flags, 1, 2, fp);
                if (frame_size <= 0 || frame_size > 10000) break;

                char data[1024];
                fread(data, 1, frame_size, fp);
                data[frame_size] = '\0';

                printf("\nFrame ID: %s, Size: %d\n", frame_id, frame_size);
                printf("Raw data (hex): ");
                hex_dump(data, frame_size);

                bytes_read += 10 + frame_size;
            }

            fclose(fp);
        }
        else if (choice == '4') {
            running = 0;
            printf("Exiting program.\n");
        }
        else {
            printf("Invalid choice.\n");
        }
    }

    return 0;
}
