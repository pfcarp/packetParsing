#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1000
#define MAX_HEX_VALUES 16

void strip_packet(FILE *input, FILE *output) {
    char line[MAX_LINE_LENGTH];
    char hex_values[MAX_HEX_VALUES * 3];
    int offset;

    while (fgets(line, sizeof(line), input)) {
        // Check if the line contains hex values (starts with an offset)
        if (sscanf(line, "%x", &offset) == 1) {
            // Extract the hex values
            char *start = strchr(line, ' ');
            if (start) {
                start++; // Move past the space
                char *end = strchr(start, '  '); // Find where the hex values end
                if (end) {
                    int len = end - start;
                    strncpy(hex_values, start, len);
                    hex_values[len] = '\0';

                    // Convert hex values to raw data and write to output
                    char *token = strtok(hex_values, " ");
                    while (token != NULL) {
                        int value;
                        sscanf(token, "%x", &value);
                        fputc(value, output);
                        token = strtok(NULL, " ");
                    }
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) {
        perror("Error opening input file");
        return 1;
    }

    FILE *output = fopen(argv[2], "wb");
    if (!output) {
        perror("Error opening output file");
        fclose(input);
        return 1;
    }

    strip_packet(input, output);

    fclose(input);
    fclose(output);
    printf("Packet data has been stripped and written to %s\n", argv[2]);

    return 0;
}
