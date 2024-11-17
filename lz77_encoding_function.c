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

    unsigned char *symbols = calloc(num_symbols, sizeof(*symbols));
    for (int row = 0; row < original_image.height; row++) {
        for (int col = 0; col < original_image.width; col++) {
            symbols[row * original_image.width + col] =
                original_image.image[row][col];
        }
    }

    unsigned int *offsets = calloc(num_symbols, sizeof(*offsets));
    unsigned int *matching_lengths =
        calloc(num_symbols, sizeof(*matching_lengths));
    char *next_symbols = calloc(num_symbols, sizeof(*next_symbols));
    size_t num_tokens = 0;

    size_t buffer_start = 0;
    size_t buffer_end = 0;
    size_t data_start = 0;

    size_t next_token_idx = 0;

    while (data_start < num_symbols) {
        // encode the next symbol(s) to a token
        unsigned int offset = data_start;
        unsigned int matching_length = 0;
        char next_symbol = symbols[data_start];

        // find longest match in searching buffer
        size_t start;
        for (start = buffer_start; start < buffer_end; start++) {
            size_t length;
            for (length = 0; length < buffer_end - start; length++) {
                char search_symbol = symbols[start + length];
                char symbol = symbols[data_start + length];
                if (search_symbol != symbol) break;
            }

            // handle cycling match

            if (length < matching_length) {
                offset = data_start - start;
                matching_length = length;
                next_symbol = symbols[data_start + length];
            }
        }

        // add that token to the tokens list
        offsets[next_token_idx] = offset;
        matching_lengths[next_token_idx] = matching_length;
        next_symbols[next_token_idx] = next_symbol;

        next_token_idx += 1;
        num_tokens += 1;

        // update buffer_start, buffer_end to include encoded symbols in
        // searching buffer.

        // update data_start to exclude encoded symbols from the data left to be
        // encoded
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
