#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "edit.h"

static unsigned int read_be32(unsigned char *b)
{
    return ((unsigned int)b[0] << 24) |
           ((unsigned int)b[1] << 16) |
           ((unsigned int)b[2] << 8) |
           (unsigned int)b[3];
}

static void write_be32(unsigned int n, unsigned char *b)
{
    b[0] = (n >> 24) & 0xFF;
    b[1] = (n >> 16) & 0xFF;
    b[2] = (n >> 8) & 0xFF;
    b[3] = n & 0xFF;
}

static unsigned int read_syncsafe(unsigned char *b)
{
    return ((unsigned int)(b[0] & 0x7F) << 21) |
           ((unsigned int)(b[1] & 0x7F) << 14) |
           ((unsigned int)(b[2] & 0x7F) << 7) |
           (unsigned int)(b[3] & 0x7F);
}

static void write_syncsafe(unsigned int n, unsigned char *b)
{
    b[0] = (n >> 21) & 0x7F;
    b[1] = (n >> 14) & 0x7F;
    b[2] = (n >> 7) & 0x7F;
    b[3] = n & 0x7F;
}

int edit_tag(EditInfo *editInfo)
{
    FILE *in = NULL, *out = NULL;
    unsigned char header[10];
    unsigned char frame_header[10];
    unsigned char size_bytes[4];
    unsigned char buffer[4096];

    unsigned int tag_size, consumed = 0;
    unsigned int new_frame_size;
    int found = 0;
    int success = 0;
    long audio_start;
    size_t n;

    if (!editInfo || !editInfo->filename ||
        !editInfo->tag || !editInfo->new_data ||
        strlen(editInfo->tag) != 4) {
        return 0;
    }

    in = fopen(editInfo->filename, "rb");
    if (!in) {
        perror("Cannot open MP3");
        return 0;
    }

    if (fread(header, 1, 10, in) != 10 ||
        memcmp(header, "ID3", 3) != 0 ||
        header[3] != 3 || (header[5] & 0xC0) != 0) {
        printf("Unsupported or invalid ID3v2.3 file.\n");
        goto cleanup;
    }

    /* This implementation supports ID3v2.3 files
       without an extended header or experimental flag. */
    if (header[5] & 0x40) {
        printf("Extended headers are not supported.\n");
        goto cleanup;
    }

    tag_size = read_syncsafe(&header[6]);

    if (tag_size > 10000000) {
        printf("Invalid ID3 tag size.\n");
        goto cleanup;
    }

    audio_start = 10L + tag_size;

    if (fseek(in, 0, SEEK_END) != 0 ||
        ftell(in) < audio_start ||
        fseek(in, 10, SEEK_SET) != 0) {
        printf("Invalid MP3 file size.\n");
        goto cleanup;
    }

    out = fopen("temp.mp3", "wb");
    if (!out) {
        perror("Cannot create temp.mp3");
        goto cleanup;
    }

    /* Write a placeholder header. */
    memset(header + 6, 0, 4);
    if (fwrite(header, 1, 10, out) != 10)
        goto cleanup;

    new_frame_size = (unsigned int)strlen(editInfo->new_data) + 1;

    if (new_frame_size > 1000000) {
        printf("New value is too large.\n");
        goto cleanup;
    }

    while (consumed < tag_size) {
        unsigned int old_size;
        unsigned int remaining = tag_size - consumed;

        /* The remaining bytes may be tag padding. */
        if (remaining < 10)
            break;

        if (fread(frame_header, 1, 10, in) != 10)
            goto cleanup;

        consumed += 10;

        /* Zero frame ID means padding, not another frame. */
        if (frame_header[0] == 0)
            break;

        old_size = read_be32(&frame_header[4]);

        if (old_size > tag_size - consumed) {
            printf("Invalid frame size.\n");
            goto cleanup;
        }

        if (memcmp(frame_header, editInfo->tag, 4) == 0) {
            found = 1;

            /* Write frame ID, updated size and original flags. */
            if (fwrite(frame_header, 1, 4, out) != 4)
                goto cleanup;

            write_be32(new_frame_size, size_bytes);
            if (fwrite(size_bytes, 1, 4, out) != 4 ||
                fwrite(frame_header + 8, 1, 2, out) != 2)
                goto cleanup;

            /* Text encoding: ISO-8859-1. */
            fputc(0, out);

            if (fwrite(editInfo->new_data, 1,
                       new_frame_size - 1, out) !=
                       new_frame_size - 1)
                goto cleanup;

            /* Skip only the old frame data. */
            if (fseek(in, old_size, SEEK_CUR) != 0)
                goto cleanup;
        } else {
            /* Preserve all other frame headers and data. */
            if (fwrite(frame_header, 1, 10, out) != 10)
                goto cleanup;

            while (old_size > 0) {
                size_t chunk = old_size < sizeof(buffer)
                             ? old_size : sizeof(buffer);

                if (fread(buffer, 1, chunk, in) != chunk ||
                    fwrite(buffer, 1, chunk, out) != chunk)
                    goto cleanup;

                old_size -= (unsigned int)chunk;
            }
        }

        consumed += read_be32(&frame_header[4]);
    }

    /* If the requested tag does not exist, append a new frame. */
    if (!found) {
        if (fwrite(editInfo->tag, 1, 4, out) != 4)
            goto cleanup;

        write_be32(new_frame_size, size_bytes);

        if (fwrite(size_bytes, 1, 4, out) != 4)
            goto cleanup;

        memset(frame_header, 0, 10);

        /* Two frame flags. */
        if (fwrite(frame_header, 1, 2, out) != 2)
            goto cleanup;

        fputc(0, out);

        if (fwrite(editInfo->new_data, 1,
                   new_frame_size - 1, out) !=
                   new_frame_size - 1)
            goto cleanup;
    }

    /* The new tag size includes frames, but not audio or header. */
    {
        long end = ftell(out);

        if (end < 10)
            goto cleanup;

        write_syncsafe((unsigned int)(end - 10), size_bytes);

        if (fseek(out, 6, SEEK_SET) != 0 ||
            fwrite(size_bytes, 1, 4, out) != 4 ||
            fseek(out, 0, SEEK_END) != 0)
            goto cleanup;
    }

    /* Jump to the original audio data and preserve it. */
    if (fseek(in, audio_start, SEEK_SET) != 0)
        goto cleanup;

    while ((n = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        if (fwrite(buffer, 1, n, out) != n)
            goto cleanup;
    }

    if (ferror(in) || fflush(out) != 0)
        goto cleanup;

    if (fclose(out) != 0) {
        out = NULL;
        goto cleanup;
    }
    out = NULL;

    fclose(in);
    in = NULL;

    /* Replace the original only after successful writing. */
    if (remove(editInfo->filename) != 0) {
        perror("Cannot replace original file");
        goto cleanup;
    }

    if (rename("temp.mp3", editInfo->filename) != 0) {
        perror("Cannot rename temporary file");
        goto cleanup;
    }

    printf("Tag %.4s %s successfully!\n",
           editInfo->tag, found ? "edited" : "added");

    success = 1;

cleanup:
    if (in)
        fclose(in);

    if (out)
        fclose(out);

    if (!success)
        remove("temp.mp3");

    return success;
}
