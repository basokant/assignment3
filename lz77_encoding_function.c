#include "lz77_encoding_function.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        // encode the next symbol(s) to a token
        unsigned int offset = 0;
        unsigned int matching_length = 0;
        unsigned char next_symbol = symbols[data_start];

        // the longest match in searching buffer is the next token
        for (int search_start = buffer_start; search_start < data_start;
             search_start++) {
            int length;
            for (length = 0; length < data_start - search_start &&
                             search_start + length < num_symbols &&
                             data_start + length < num_symbols;
                 length++) {
                unsigned char search_symbol = symbols[search_start + length];
                unsigned char symbol = symbols[data_start + length];

                if (search_symbol != symbol) break;
            }

            if (length > 0 && length >= matching_length) {
                offset = data_start - search_start;
                matching_length = length;
                next_symbol = symbols[data_start + length];
            }
        }

        if (matching_length > 0 && offset == matching_length) {
            int search_start = offset;
            int length;
            for (length = 0;
                 data_start - search_start + length < num_symbols &&
                 data_start + matching_length + length < num_symbols;
                 length++) {
                unsigned char search_symbol =
                    symbols[data_start - search_start + length];
                unsigned char symbol =
                    symbols[data_start + matching_length + length];

                if (search_symbol != symbol) break;
            }

            matching_length += length - 1;
            next_symbol = symbols[data_start + matching_length];
        }

        offsets[num_tokens] = offset;
        matching_lengths[num_tokens] = matching_length;
        next_symbols[num_tokens] = next_symbol;
        /* printf("%u %u '%u'\n", offset, matching_length, next_symbol); */

        num_tokens += 1;

        // update buffer to include encoded symbols in searching buffer.
        data_start = data_start + matching_length + 1;
        if (data_start - buffer_start > searching_buffer_size) {
            buffer_start = data_start - searching_buffer_size;
        }
    }

    return num_tokens;
}

void save_LZ77_encoded_image(char *encoded_image_name, size_t num_tokens,
                             int width, int height, int max_gray_value,
                             unsigned int *offsets,
                             unsigned int *matching_lengths,
                             unsigned char *next_symbols) {
    FILE *encoded_file = fopen(encoded_image_name, "wb");

    struct LZ77_Header header = {num_tokens, width, height, max_gray_value};
    fwrite(&header, sizeof(header), 1, encoded_file);

    fwrite(offsets, sizeof(*offsets), num_tokens, encoded_file);
    fwrite(matching_lengths, sizeof(*matching_lengths), num_tokens,
           encoded_file);
    fwrite(next_symbols, sizeof(*next_symbols), num_tokens, encoded_file);

    fclose(encoded_file);
}

float compute_avg(unsigned int *nums, size_t length) {
    float avg = 0;
    for (size_t i = 0; i < length; i++) {
        avg += nums[i];
    }

    return avg / length;
}

float compute_std_dev(float avg, unsigned int *nums, size_t length) {
    float std = 0;
    for (int i = 0; i < length; i++) {
        std += (nums[i] - avg) * (nums[i] - avg);
    }

    return sqrt(std / length);
}

void write_histogram(char *histogram_file_name, unsigned int *nums,
                     size_t length, unsigned int max_num) {
    size_t *frequencies = calloc(max_num, sizeof(*frequencies));

    for (int i = 0; i < length; i++) {
        unsigned int num = nums[i];
        frequencies[num] += 1;
    }

    FILE *histogram_file = fopen(histogram_file_name, "wb");
    char csv_row[1024];
    for (int num = 0; num < max_num; num++) {
        size_t freq = frequencies[num];
        if (freq <= 0) continue;

        sprintf(csv_row, "%d, %zu\n", num, freq);
        size_t row_length = strlen(csv_row);

        fwrite(csv_row, sizeof(*csv_row), row_length, histogram_file);
    }

    fclose(histogram_file);
}

void Encode_Using_LZ77(char *in_PGM_filename_Ptr,
                       unsigned int searching_buffer_size,
                       float *avg_offset_Ptr, float *std_offset_Ptr,
                       float *avg_length_Ptr, float *std_length_Ptr) {
    struct PGM_Image original_image;
    load_PGM_Image(&original_image, in_PGM_filename_Ptr);

    size_t num_symbols = original_image.width * original_image.height;
    unsigned char *symbols = flatten_image(&original_image);

    unsigned int *offsets = calloc(num_symbols, sizeof(*offsets));
    unsigned int *matching_lengths =
        calloc(num_symbols, sizeof(*matching_lengths));
    unsigned char *next_symbols = calloc(num_symbols, sizeof(*next_symbols));

    size_t num_tokens =
        LZ77_tokenize(searching_buffer_size, symbols, num_symbols, offsets,
                      matching_lengths, next_symbols);

    char encoded_image_name[1024];
    sprintf(encoded_image_name, "%s.%u.lz", in_PGM_filename_Ptr,
            searching_buffer_size);

    save_LZ77_encoded_image(encoded_image_name, num_tokens,
                            original_image.width, original_image.height,
                            original_image.maxGrayValue, offsets,
                            matching_lengths, next_symbols);

    char offset_histogram_file_name[1024];
    sprintf(offset_histogram_file_name, "%s.%u.offsets.csv",
            in_PGM_filename_Ptr, searching_buffer_size);

    write_histogram(offset_histogram_file_name, offsets, num_tokens,
                    searching_buffer_size);

    char length_histogram_file_name[1024];
    sprintf(length_histogram_file_name, "%s.%u.lengths.csv",
            in_PGM_filename_Ptr, searching_buffer_size);

    write_histogram(length_histogram_file_name, matching_lengths, num_tokens,
                    num_symbols);

    *avg_offset_Ptr = compute_avg(offsets, num_tokens);
    *std_offset_Ptr = compute_std_dev(*avg_offset_Ptr, offsets, num_tokens);

    *avg_length_Ptr = compute_avg(matching_lengths, num_tokens);
    *std_length_Ptr =
        compute_std_dev(*avg_length_Ptr, matching_lengths, num_tokens);

    free_PGM_Image(&original_image);
    free(symbols);
    free(offsets);
    free(matching_lengths);
    free(next_symbols);
}
