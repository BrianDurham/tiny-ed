/* 
 * tiny-ed.c - Tiny Ed: A minimalist line editor
 * 
 * Purpose:
 *   Recreate basic 'ed' functionality with minimal code.
 * 
 * Author:
 *   Nroam Durham
 *
 * Created:
 *   2025-04-28
 *
 * Notes:
 *   Early development version. Only basic commands implemented.
 *
 * License:
 *   [MIT License]
 */

/*
 * tiny-ed - a minimal line editor in the style of Unix `ed`
 *
 * Author: Brian Durham
 * Date: April 2025
 *
 * Build:
 *   gcc -Wall -o tiny-ed tiny-ed.c
 *
 * Description:
 *   A simple line-based text editor with basic commands:
 *     r <file>   - read file into buffer
 *     w <file>   - write buffer to file
 *     1p         - print line 1
 *     2d         - delete line 2
 *     q          - quit the editor
 *
 * Notes:
 *   - Buffer is stored in RAM using malloc.
 *   - Line addressing starts at 1.
 *   - Input parsing is minimal; this is a learning project.
 *
 * License: MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 1024

char *buffer[MAX_LINES];
int line_count = 0;

void read_file(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("open");
        return;
    }

    char line[MAX_LINE_LENGTH];
    line_count = 0;

    while (fgets(line, sizeof(line), fp) && line_count < MAX_LINES) {
        buffer[line_count++] = strdup(line);
    }

    fclose(fp);
    printf("%d\n", line_count);
}

void write_file(const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("write");
        return;
    }

    int bytes_written = 0;

    for (int i = 0; i < line_count; i++) {
        fputs(buffer[i], fp);
        bytes_written += strlen(buffer[i]);
    }

    fclose(fp);
    printf("%d\n", bytes_written);
}

void print_lines(int start, int end) {
    if (start < 1 || end > line_count || start > end) {
        printf("?\n");
        return;
    }
    for (int i = start - 1; i < end; i++) {
        printf("%s", buffer[i]);
    }
}

void delete_lines(int start, int end) {
    if (start < 1 || end > line_count || start > end) {
        printf("?\n");
        return;
    }
    int n = end - start + 1;
    for (int i = start - 1; i + n < line_count; i++) {
        buffer[i] = buffer[i + n];
    }
    line_count -= n;
}

int parse_range(const char *input, char cmd, int *start, int *end) {
    int s = 0, e = 0;
    char *copy = strdup(input);
    char *pos = strchr(copy, cmd);
    if (!pos) {
        free(copy);
        return 0;
    }
    *pos = '\0';

    if (strchr(copy, ',')) {
        sscanf(copy, "%d,%d", &s, &e);
    } else if (strlen(copy) > 0) {
        sscanf(copy, "%d", &s);
        e = s;
    } else {
        s = line_count;
        e = line_count;
    }

    free(copy);
    *start = s;
    *end = e;
    return 1;
}

int parse_single_line(const char *input, char cmd, int *line) {
    int l = 0;
    char *copy = strdup(input);
    char *pos = strchr(copy, cmd);
    if (!pos) {
        free(copy);
        return 0;
    }
    *pos = '\0';

    if (strlen(copy) > 0) {
        sscanf(copy, "%d", &l);
    } else {
        l = (cmd == 'a') ? line_count : 1;
    }

    free(copy);
    *line = l;
    return 1;
}

void append_lines(int after) {
    char line[MAX_LINE_LENGTH];

    if (after < 0 || after > line_count || line_count >= MAX_LINES) {
        printf("?\n");
        return;
    }

    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, ".\n") == 0 || strcmp(line, ".") == 0)
            break;

        if (line_count >= MAX_LINES) {
            printf("?\n");
            break;
        }

        for (int i = line_count; i > after; i--) {
            buffer[i] = buffer[i - 1];
        }

        buffer[after] = strdup(line);
        after++;
        line_count++;
    }
}

void insert_lines(int before) {
    append_lines(before - 1);
}

void cmd_loop() {
    char input[256];

    while (fgets(input, sizeof(input), stdin)) {
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "q") == 0) {
            break;
        } else if (strncmp(input, "r ", 2) == 0) {
            char *filename = input + 2;
            read_file(filename);
        } else if (strncmp(input, "w ", 2) == 0) {
            char *filename = input + 2;
            write_file(filename);
        } else if (strchr(input, 'p')) {
            int start, end;
            if (parse_range(input, 'p', &start, &end)) {
                print_lines(start, end);
            } else {
                printf("?\n");
            }
        } else if (strchr(input, 'd')) {
            int start, end;
            if (parse_range(input, 'd', &start, &end)) {
                delete_lines(start, end);
            } else {
                printf("?\n");
            }
        } else if (strchr(input, 'a')) {
            int line_no;
            if (parse_single_line(input, 'a', &line_no)) {
                append_lines(line_no);
            } else {
                printf("?\n");
            }
        } else if (strchr(input, 'i')) {
            int line_no;
            if (parse_single_line(input, 'i', &line_no)) {
                insert_lines(line_no);
            } else {
                printf("?\n");
            }
        } else {
            printf("?\n");
        }
    }
}

int main() {
    cmd_loop();
    return 0;
}
