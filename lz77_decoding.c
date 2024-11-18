#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "lz77_decoding_function.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Expected 1 arguments. Received %d\n", argc - 1);
        exit(0);
    }

    struct timeval start;
    struct timeval end;
    unsigned long e_usec = 0;  // elapsed compression time in microseconds

    gettimeofday(&start, 0);

    char *input_lz_encoded_file_name = argv[1];
    Decode_Using_LZ77(input_lz_encoded_file_name);

    gettimeofday(&end, 0);
    e_usec = ((end.tv_sec * 1000000) + end.tv_usec) -
             ((start.tv_sec * 1000000) + start.tv_usec);
    printf("Decompression Time: %lu microseconds\n", e_usec);
}
