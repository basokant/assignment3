#ifndef LZ77_ENCODING_FUNCTION
#define LZ77_ENCODING_FUNCTION

struct LZ77_Token {
  unsigned int offset;
  unsigned int match_length;
  char mismatched_symbol;
};

void Encode_Using_LZ77(char *in_PGM_filename_Ptr,
                       unsigned int searching_buffer_size,
                       float *avg_offset_Ptr, float *std_offset_Ptr,
                       float *avg_length_Ptr, float *std_length_Ptr);
#endif
