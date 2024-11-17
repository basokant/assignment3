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
                     size_t num_symbols, unsigned int *offsets,
                     unsigned int *matching_lengths,
                     unsigned char *next_symbols) {
    size_t num_tokens = 0;

    size_t buffer_start = 0;
    size_t data_start = 0;

    while (data_start < num_symbols) {
        printf("loop start (num_symbols: %zu, num_tokens: %zu)\n", num_symbols,
               num_tokens);
        printf("data_start: %zu, buffer_start: %zu, num_tokens: %zu\n",
               data_start, buffer_start, num_tokens);

        // encode the next symbol(s) to a token
        unsigned int offset = 0;
        unsigned int matching_length = 0;
        unsigned char next_symbol = symbols[data_start];
        /* printf("initial match. start: %zu, length: %u, next_symbol: %c\n", */
        /*        data_start, matching_length, next_symbol); */

        // the longest match in searching buffer is the next token
        for (int start = buffer_start; start < data_start; start++) {
            /* printf("start: %d\n", start); */

            int length;
            for (length = 0;
                 length < data_start - start && start + length < num_symbols &&
                 data_start + length < num_symbols;
                 length++) {
                unsigned char search_symbol = symbols[start + length];
                unsigned char symbol = symbols[data_start + length];
                /* printf("search_symbol (%d): %c, symbol (%zu): %c\n", */
                /*        start + length, search_symbol, data_start + length, */
                /*        symbol); */
                if (search_symbol != symbol) break;
            }

            // TODO: handle cycling match

            if (length > 0 && length >= matching_length) {
                offset = data_start - start;
                matching_length = length;
                next_symbol = symbols[data_start + length];
                /* printf("longer match. start: %d, length: %d, next_symbol:
                 * %c\n", */
                /*        start, length, next_symbol); */
            }
        }

        if (num_tokens >= num_symbols) {
            fprintf(stderr,
                    "Error: num_tokens (%zu) exceeds num_symbols (%zu)\n",
                    num_tokens, num_symbols);
            exit(EXIT_FAILURE);
        }

        offsets[num_tokens] = offset;
        matching_lengths[num_tokens] = matching_length;
        next_symbols[num_tokens] = next_symbol;
        /* printf("offset: %u, matching_length: %u, next_symbol %c\n", offset,
         */
        /*        matching_length, next_symbol); */
        printf("%u %u '%c'\n", offset, matching_length, next_symbol);

        num_tokens += 1;

        // update buffer to include encoded symbols in searching buffer.
        data_start = data_start + matching_length + 1;
        if (data_start - buffer_start > searching_buffer_size) {
            buffer_start = data_start - searching_buffer_size;
        }
        printf("loop end data_start=%zu\n\n", data_start);
    }

    printf("out of the loop");
    return num_tokens;
}

void save_LZ77_encoded_image(char *encoded_image_name, size_t num_tokens,
                             int width, int height, int max_gray_value,
                             unsigned int *offsets,
                             unsigned int *matching_lengths,
                             unsigned char *next_symbols) {
    FILE *encoded_file = fopen(encoded_image_name, "w");

    struct LZ77_Header header = {num_tokens, width, height, max_gray_value};
    fwrite(&header, sizeof(header), 1, encoded_file);

    fwrite(offsets, sizeof(*offsets), num_tokens, encoded_file);
    fwrite(matching_lengths, sizeof(*matching_lengths), num_tokens,
           encoded_file);
    fwrite(next_symbols, sizeof(*next_symbols), num_tokens, encoded_file);

    fclose(encoded_file);
}

void Encode_Using_LZ77(char *in_PGM_filename_Ptr,
                       unsigned int searching_buffer_size,
                       float *avg_offset_Ptr, float *std_offset_Ptr,
                       float *avg_length_Ptr, float *std_length_Ptr) {
    struct PGM_Image original_image;
    load_PGM_Image(&original_image, in_PGM_filename_Ptr);

    size_t num_symbols = original_image.width * original_image.height;
    unsigned char *symbols = flatten_image(&original_image);

    printf("\n");
    for (int i = 0; i < num_symbols; i++) {
        printf("%c", symbols[i]);
    }
    printf("\n\n");

    unsigned int *offsets = calloc(num_symbols, sizeof(*offsets));
    unsigned int *matching_lengths =
        calloc(num_symbols, sizeof(*matching_lengths));
    unsigned char *next_symbols = calloc(num_symbols, sizeof(*next_symbols));

    size_t num_tokens =
        LZ77_tokenize(searching_buffer_size, symbols, num_symbols, offsets,
                      matching_lengths, next_symbols);
    printf("done tokenizing");

    char *encoded_image_name = NULL;
    sprintf(encoded_image_name, "%s.%u.lz", in_PGM_filename_Ptr,
            searching_buffer_size);

    save_LZ77_encoded_image(in_PGM_filename_Ptr, num_tokens,
                            original_image.width, original_image.height,
                            original_image.maxGrayValue, offsets,
                            matching_lengths, next_symbols);

    // TODO: write offset histogram data to another file

    // TODO: write match length histogram data to another file

    // TODO: calculate average offset and store
    // calculate standard deviation of the offset and store

    // TODO: calculate average match length and store

    // TODO: calculate standard deviation of the match length and store

    free_PGM_Image(&original_image);
    free(symbols);
    free(offsets);
    free(matching_lengths);
    free(next_symbols);
}
