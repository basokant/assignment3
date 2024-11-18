#include <stdio.h>
#include <stdlib.h>

#include "lz77_decoding_function.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Expected 1 arguments. Received %d\n", argc - 1);
        exit(0);
    }

    char *input_lz_encoded_file_name = argv[1];
    Decode_Using_LZ77(input_lz_encoded_file_name);
}
