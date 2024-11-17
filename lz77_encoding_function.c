#include "lz77_encoding_function.h"

#include <stdlib.h>

#include "libpnm.h"

unsigned char *flatten_image(struct PGM_Image *image) {
    size_t num_pixels = image->width * image->height;
    unsigned char *flattened_image =
        calloc(num_pixels, sizeof(*flattened_image));

    for (int row = 0; row < image->height; row++) {
        for (int col = 0; col < image->width; col++) {
            flattened_image[row * image->width + col] = image->image[row][col];
        }
    }

    return flattened_image;
}

size_t LZ77_tokenize(unsigned int searching_buffer_size, unsigned char *symbols,
                     size_t num_symbols, unsigned int **offsets,
                     unsigned int **matching_lengths,
                     unsigned int **next_symbols) {
    size_t num_tokens = 0;

    *offsets = calloc(num_symbols, sizeof(*offsets));
    *matching_lengths = calloc(num_symbols, sizeof(*matching_lengths));
    *next_symbols = calloc(num_symbols, sizeof(*next_symbols));

    size_t buffer_start = 0;
    size_t data_start = 0;

    while (data_start < num_symbols) {
        // encode the next symbol(s) to a token
        unsigned int offset = data_start;
        unsigned int matching_length = 0;
        char next_symbol = symbols[data_start];

        // find longest match in searching buffer
        size_t start;
        for (start = buffer_start; start < data_start; start++) {
            size_t length;
            for (length = 0; length < data_start - start; length++) {
                char search_symbol = symbols[start + length];
                char symbol = symbols[data_start + length];
                if (search_symbol != symbol) break;
            }

            // handle cycling match

            if (length > matching_length) {
                offset = data_start - start;
                matching_length = length;
                next_symbol = symbols[data_start + length];
            }
        }

        // add that token to the tokens list
        *offsets[num_tokens] = offset;
        *matching_lengths[num_tokens] = matching_length;
        *next_symbols[num_tokens] = next_symbol;

        num_tokens += 1;

        // update buffer to include encoded symbols in searching buffer.
        data_start = data_start + matching_length;
        if (data_start - buffer_start > searching_buffer_size) {
            buffer_start = data_start - searching_buffer_size;
        }
    }

    return num_tokens;
}

void Encode_Using_LZ77(char *in_PGM_filename_Ptr,
                       unsigned int searching_buffer_size,
                       float *avg_offset_Ptr, float *std_offset_Ptr,
                       float *avg_length_Ptr, float *std_length_Ptr) {
    struct PGM_Image original_image;
    load_PGM_Image(&original_image, in_PGM_filename_Ptr);

    size_t num_symbols = original_image.width * original_image.height;
    unsigned char *symbols = flatten_image(&original_image);

    unsigned int *offsets;
    unsigned int *matching_lengths;
    unsigned int *next_symbols;

    size_t num_tokens =
        LZ77_tokenize(searching_buffer_size, symbols, num_symbols, &offsets,
                      &matching_lengths, &next_symbols);

    struct LZ77_Header header = {num_tokens, original_image.width,
                                 original_image.height,
                                 original_image.maxGrayValue};
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
    free(symbols);
    free(offsets);
    free(matching_lengths);
    free(next_symbols);
}
