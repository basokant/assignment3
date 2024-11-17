#include "lz77_encoding_function.h"

#include <stdlib.h>

#include "libpnm.h"

void Encode_Using_LZ77(char *in_PGM_filename_Ptr,
                       unsigned int searching_buffer_size,
                       float *avg_offset_Ptr, float *std_offset_Ptr,
                       float *avg_length_Ptr, float *std_length_Ptr) {
    struct PGM_Image original_image;
    load_PGM_Image(&original_image, in_PGM_filename_Ptr);
    size_t num_symbols = original_image.width * original_image.height;

    unsigned int *offsets = calloc(num_symbols, sizeof(*offsets));
    unsigned int *matching_lengths =
        calloc(num_symbols, sizeof(*matching_lengths));
    char *next_symbols = calloc(num_symbols, sizeof(*next_symbols));
    size_t num_tokens = 0;

    int buffer_start = 0;
    int buffer_end = 0;
    int data_start = 0;

    int next_token_idx = 0;

    while (data_start < num_symbols) {
        // encode the next symbol to a token
        // add that token to the tokens list
    }

    // create header with number_of_tokens
    // open file for writing the encoded image (same name with searching buffer
    // size and .lz concatenated)

    // save header to file
    // save tokens to file

    // write offset histogram data to another file

    // write match length histogram data to another file

    // calculate average offset and store
    // calculate standard deviation of the offset and store

    // calculate average match length and store
    // calculate standard deviation of the match length and store

    free_PGM_Image(&original_image);
    free(offsets);
    free(matching_lengths);
    free(next_symbols);
}
