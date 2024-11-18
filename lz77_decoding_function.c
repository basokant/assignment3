#include <stdio.h>

#include "lz77_encoding_function.h"

void Decode_Using_LZ77(char *in_compressed_filename_Ptr) {
    struct LZ77_Header header;
    FILE *compressed_file = fopen(in_compressed_filename_Ptr, "rb");

    fread(&header, sizeof(header), 1, compressed_file);
    printf("%d, %d, %d, %d\n", header.height, header.width,
           header.max_gray_value, header.num_tokens);

    unsigned int *offsets = calloc(header.num_tokens, sizeof(*offsets));
    unsigned int *matching_lengths =
        calloc(header.num_tokens, sizeof(*matching_lengths));
    unsigned char *next_symbols =
        calloc(header.num_tokens, sizeof(*next_symbols));

    fread(offsets, sizeof(*offsets), header.num_tokens, compressed_file);
    fread(matching_lengths, sizeof(*matching_lengths), header.num_tokens,
          compressed_file);
    fread(next_symbols, sizeof(*next_symbols), header.num_tokens,
          compressed_file);
    fclose(compressed_file);

    for (int i = 0; i < header.num_tokens; i++) {
        printf("%u %u '%c'\n", offsets[i], matching_lengths[i],
               next_symbols[i]);
    }
}
