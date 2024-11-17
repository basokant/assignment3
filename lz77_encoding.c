#include <stdio.h>
#include <stdlib.h>

#include "lz77_encoding_function.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Expected 2 arguments. Received %d\n", argc - 1);
        exit(0);
    }

    char *input_image_name = argv[1];
    char *raw_searching_buffer_size = argv[2];

    unsigned int searching_buffer_size =
        strtoul(raw_searching_buffer_size, NULL, 0);

    float avg_offset;
    float std_offset;
    float avg_length;
    float std_length;

    Encode_Using_LZ77(input_image_name, searching_buffer_size, &avg_offset,
                      &std_offset, &avg_length, &std_length);

    printf("Offset Average: %.6f\nOffset Standard Deviation: %.6f\n",
           avg_offset, std_offset);
    printf(
        "Match Length Average: %.6f\nMatch Length Standard Deviation: %.6f\n",
        avg_length, std_length);
}
