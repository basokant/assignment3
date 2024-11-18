#include <stdio.h>

#include "libpnm.h"
#include "lz77_encoding_function.h"

unsigned char *decode_lz77_tokens(size_t num_tokens, int num_symbols,
                                  unsigned int *offsets,
                                  unsigned int *matching_lengths,
                                  unsigned char *next_symbols) {
    unsigned char *symbols = calloc(num_symbols, sizeof(*symbols));

    size_t data_start = 0;

    for (size_t i = 0; i < num_tokens; i++) {
        unsigned int offset = offsets[i];
        unsigned int matching_length = matching_lengths[i];
        unsigned char next_symbol = next_symbols[i];

        // copy everything from match
        for (int j = 0; j < matching_length; j++) {
            symbols[data_start + j] = symbols[data_start - offset + j];
        }

        // add next symbol
        symbols[data_start + matching_length] = next_symbol;
        data_start += matching_length + 1;
    }

    return symbols;
}

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

    size_t num_pixels = header.height * header.width;
    unsigned char *flattened_image = decode_lz77_tokens(
        header.num_tokens, num_pixels, offsets, matching_lengths, next_symbols);

    struct PGM_Image decoded_image;
    create_PGM_Image(&decoded_image, header.width, header.height,
                     header.max_gray_value);

    for (int row = 0; row < decoded_image.height; row++) {
        for (int col = 0; col < decoded_image.width; col++) {
            decoded_image.image[row][col] =
                flattened_image[row * decoded_image.width + col];
        }
    }

    char decoded_image_name[1024];
    sprintf(decoded_image_name, "%s.pgm", in_compressed_filename_Ptr);

    save_PGM_Image(&decoded_image, decoded_image_name, false);

    free(offsets);
    free(matching_lengths);
    free(next_symbols);
}
