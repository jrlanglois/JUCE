/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2020 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 6 End-User License
   Agreement and JUCE Privacy Policy (both effective as of the 16th June 2020).

   End User License Agreement: www.juce.com/juce-6-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

//==============================================================================
namespace libwebpNamespace
{
    JUCE_BEGIN_IGNORE_WARNINGS_MSVC (4127 4242 4244 4245 4309 4310 4459 4701)

    #undef HAVE_CONFIG_H
    #undef USE_DITHERING
    #undef WEBP_HAVE_GIF
    #undef WEBP_HAVE_GL
    #undef WEBP_HAVE_JPEG
    #undef WEBP_HAVE_NEON_RTCD
    #undef WEBP_HAVE_PNG
    #undef WEBP_HAVE_SDL
    #undef WEBP_HAVE_TIFF
    #undef WEBP_USE_THREAD
    #undef HAVE_GLUT_GLUT_H
    #undef HAVE_OPENGL_GLUT_H

    #undef WEBP_DISABLE_STATS
    #define WEBP_DISABLE_STATS 1

    #undef AC_APPLE_UNIVERSAL_BUILD
   #if (JUCE_MAC || JUCE_IOS) && JUCE_INTEL
    #define AC_APPLE_UNIVERSAL_BUILD 1
   #endif

    #undef HAVE_BUILTIN_BSWAP16
    #undef HAVE_BUILTIN_BSWAP32
    #undef HAVE_BUILTIN_BSWAP64
   #if ! JUCE_WINDOWS
    #define HAVE_BUILTIN_BSWAP16 1
    #define HAVE_BUILTIN_BSWAP32 1
    #define HAVE_BUILTIN_BSWAP64 1
   #endif

    #undef HAVE_CPU_FEATURES_H
    #define HAVE_CPU_FEATURES_H 1

    #undef HAVE_DLFCN_H
    #define HAVE_DLFCN_H 1

    #undef HAVE_GL_GLUT_H
    #define HAVE_GL_GLUT_H 1

    #undef HAVE_INTTYPES_H
    #define HAVE_INTTYPES_H 1

    #undef HAVE_MEMORY_H
    #define HAVE_MEMORY_H 1

    #undef HAVE_PTHREAD_PRIO_INHERIT
    #define HAVE_PTHREAD_PRIO_INHERIT (! JUCE_WINDOWS)

    #undef HAVE_SHLWAPI_H
    #define HAVE_SHLWAPI_H 1

    #undef HAVE_STDINT_H
    #define HAVE_STDINT_H 1

    #undef HAVE_STDLIB_H
    #define HAVE_STDLIB_H 1

    #undef HAVE_STRINGS_H
    #define HAVE_STRINGS_H 1

    #undef HAVE_STRING_H
    #define HAVE_STRING_H 1

    #undef HAVE_SYS_STAT_H
    #undef HAVE_SYS_TYPES_H
    #undef HAVE_UNISTD_H

   #if ! JUCE_WINDOWS
    #define HAVE_SYS_STAT_H 1
    #define HAVE_SYS_TYPES_H 1
    #define HAVE_UNISTD_H 1
   #endif

    #undef HAVE_WINCODEC_H
   #if JUCE_WINDOWS
    #define HAVE_WINCODEC_H 1
   #endif

    #undef HAVE_WINDOWS_H
   #if JUCE_WINDOWS
    #define HAVE_WINDOWS_H 1
   #endif

    #undef STDC_HEADERS
    #define STDC_HEADERS 1

    #undef WEBP_HAVE_NEON
   #if JUCE_MAC
    #define WEBP_HAVE_NEON 1
   #endif

    #undef WEBP_HAVE_SSE2
   #if JUCE_INTEL && JUCE_64BIT
    #define WEBP_HAVE_SSE2 1
   #endif

    #undef WEBP_HAVE_SSE41
   #if JUCE_MSVC
    #define WEBP_HAVE_SSE41 1
   #endif

    #undef WEBP_NEAR_LOSSLESS
    #define WEBP_NEAR_LOSSLESS 1

    #undef WORDS_BIGENDIAN
   #if JUCE_BIG_ENDIAN
    #define WORDS_BIGENDIAN 1
   #endif

    #undef LT_OBJDIR
    #define LT_OBJDIR ""

    #undef PACKAGE
    #define PACKAGE "WebP"
    #undef PACKAGE_NAME
    #define PACKAGE_NAME PACKAGE
    #undef PACKAGE_TARNAME
    #define PACKAGE_TARNAME PACKAGE

    #undef PACKAGE_VERSION
    #define PACKAGE_VERSION "1.1.0"
    #undef VERSION
    #define VERSION PACKAGE_VERSION

    #undef PACKAGE_BUGREPORT
    #define PACKAGE_BUGREPORT "https://bugs.chromium.org/p/webp"

    #undef PACKAGE_STRING
    #define PACKAGE_STRING (PACKAGE_NAME " " PACKAGE_VERSION)

    #undef PACKAGE_URL
    #define PACKAGE_URL "http://developers.google.com/speed/webp"

    #include "libwebp/webp/decode.h"
    #include "libwebp/webp/encode.h"
    #include "libwebp/webp/demux.h"
    #include "libwebp/dsp/dsp.h"
    #include "libwebp/dec/common_dec.h"

    uint8_t clip_8b (int v)                                     { return (!(v & ~0xff)) ? v : (v < 0) ? 0 : 255; }
    uint8_t clip_8b (int16_t v)                                 { return (!(v & ~0xff)) ? (uint8_t)v : (v < 0) ? 0u : 255u; }
    int clip (int v, int M)                                     { return v < 0 ? 0 : v > M ? M : v; }
    int clip (int v, int m, int M)                              { return (v < m) ? m : (v > M) ? M : v; }
    int ColorTransformDelta(int8_t color_pred, int8_t color)    { return ((int)color_pred * color) >> 5; }

    static const uint8_t kZigzag[16] = { 0, 1, 4, 8, 5, 2, 3, 6, 9, 12, 13, 10, 7, 11, 14, 15 };

    static const uint8_t kDcTable[128] =
    {
        4,   5,   6,   7,   8,   9,   10,  10,
        11,  12,  13,  14,  15,  16,  17,  17,
        18,  19,  20,  20,  21,  21,  22,  22,
        23,  23,  24,  25,  25,  26,  27,  28,
        29,  30,  31,  32,  33,  34,  35,  36,
        37,  37,  38,  39,  40,  41,  42,  43,
        44,  45,  46,  46,  47,  48,  49,  50,
        51,  52,  53,  54,  55,  56,  57,  58,
        59,  60,  61,  62,  63,  64,  65,  66,
        67,  68,  69,  70,  71,  72,  73,  74,
        75,  76,  76,  77,  78,  79,  80,  81,
        82,  83,  84,  85,  86,  87,  88,  89,
        91,  93,  95,  96,  98,  100, 101, 102,
        104, 106, 108, 110, 112, 114, 116, 118,
        122, 124, 126, 128, 130, 132, 134, 136,
        138, 140, 143, 145, 148, 151, 154, 157
    };

    static const uint16_t kAcTable[128] =
    {
        4,   5,   6,   7,   8,   9,   10,  11,
        12,  13,  14,  15,  16,  17,  18,  19,
        20,  21,  22,  23,  24,  25,  26,  27,
        28,  29,  30,  31,  32,  33,  34,  35,
        36,  37,  38,  39,  40,  41,  42,  43,
        44,  45,  46,  47,  48,  49,  50,  51,
        52,  53,  54,  55,  56,  57,  58,  60,
        62,  64,  66,  68,  70,  72,  74,  76,
        78,  80,  82,  84,  86,  88,  90,  92,
        94,  96,  98,  100, 102, 104, 106, 108,
        110, 112, 114, 116, 119, 122, 125, 128,
        131, 134, 137, 140, 143, 146, 149, 152,
        155, 158, 161, 164, 167, 170, 173, 177,
        181, 185, 189, 193, 197, 201, 205, 209,
        213, 217, 221, 225, 229, 234, 239, 245,
        249, 254, 259, 264, 269, 274, 279, 284
    };

    static const uint16_t kAcTable2[128] =
    {
        8,   8,   9,   10,  12,  13,  15,  17,
        18,  20,  21,  23,  24,  26,  27,  29,
        31,  32,  34,  35,  37,  38,  40,  41,
        43,  44,  46,  48,  49,  51,  52,  54,
        55,  57,  58,  60,  62,  63,  65,  66,
        68,  69,  71,  72,  74,  75,  77,  79,
        80,  82,  83,  85,  86,  88,  89,  93,
        96,  99,  102, 105, 108, 111, 114, 117,
        120, 124, 127, 130, 133, 136, 139, 142,
        145, 148, 151, 155, 158, 161, 164, 167,
        170, 173, 176, 179, 184, 189, 193, 198,
        203, 207, 212, 217, 221, 226, 230, 235,
        240, 244, 249, 254, 258, 263, 268, 274,
        280, 286, 292, 299, 305, 311, 317, 323,
        330, 336, 342, 348, 354, 362, 370, 379,
        385, 393, 401, 409, 416, 424, 432, 440
    };

    static const uint8_t kBModesProba[NUM_BMODES][NUM_BMODES][NUM_BMODES - 1] = {
      { { 231, 120, 48, 89, 115, 113, 120, 152, 112 },
        { 152, 179, 64, 126, 170, 118, 46, 70, 95 },
        { 175, 69, 143, 80, 85, 82, 72, 155, 103 },
        { 56, 58, 10, 171, 218, 189, 17, 13, 152 },
        { 114, 26, 17, 163, 44, 195, 21, 10, 173 },
        { 121, 24, 80, 195, 26, 62, 44, 64, 85 },
        { 144, 71, 10, 38, 171, 213, 144, 34, 26 },
        { 170, 46, 55, 19, 136, 160, 33, 206, 71 },
        { 63, 20, 8, 114, 114, 208, 12, 9, 226 },
        { 81, 40, 11, 96, 182, 84, 29, 16, 36 } },
      { { 134, 183, 89, 137, 98, 101, 106, 165, 148 },
        { 72, 187, 100, 130, 157, 111, 32, 75, 80 },
        { 66, 102, 167, 99, 74, 62, 40, 234, 128 },
        { 41, 53, 9, 178, 241, 141, 26, 8, 107 },
        { 74, 43, 26, 146, 73, 166, 49, 23, 157 },
        { 65, 38, 105, 160, 51, 52, 31, 115, 128 },
        { 104, 79, 12, 27, 217, 255, 87, 17, 7 },
        { 87, 68, 71, 44, 114, 51, 15, 186, 23 },
        { 47, 41, 14, 110, 182, 183, 21, 17, 194 },
        { 66, 45, 25, 102, 197, 189, 23, 18, 22 } },
      { { 88, 88, 147, 150, 42, 46, 45, 196, 205 },
        { 43, 97, 183, 117, 85, 38, 35, 179, 61 },
        { 39, 53, 200, 87, 26, 21, 43, 232, 171 },
        { 56, 34, 51, 104, 114, 102, 29, 93, 77 },
        { 39, 28, 85, 171, 58, 165, 90, 98, 64 },
        { 34, 22, 116, 206, 23, 34, 43, 166, 73 },
        { 107, 54, 32, 26, 51, 1, 81, 43, 31 },
        { 68, 25, 106, 22, 64, 171, 36, 225, 114 },
        { 34, 19, 21, 102, 132, 188, 16, 76, 124 },
        { 62, 18, 78, 95, 85, 57, 50, 48, 51 } },
      { { 193, 101, 35, 159, 215, 111, 89, 46, 111 },
        { 60, 148, 31, 172, 219, 228, 21, 18, 111 },
        { 112, 113, 77, 85, 179, 255, 38, 120, 114 },
        { 40, 42, 1, 196, 245, 209, 10, 25, 109 },
        { 88, 43, 29, 140, 166, 213, 37, 43, 154 },
        { 61, 63, 30, 155, 67, 45, 68, 1, 209 },
        { 100, 80, 8, 43, 154, 1, 51, 26, 71 },
        { 142, 78, 78, 16, 255, 128, 34, 197, 171 },
        { 41, 40, 5, 102, 211, 183, 4, 1, 221 },
        { 51, 50, 17, 168, 209, 192, 23, 25, 82 } },
      { { 138, 31, 36, 171, 27, 166, 38, 44, 229 },
        { 67, 87, 58, 169, 82, 115, 26, 59, 179 },
        { 63, 59, 90, 180, 59, 166, 93, 73, 154 },
        { 40, 40, 21, 116, 143, 209, 34, 39, 175 },
        { 47, 15, 16, 183, 34, 223, 49, 45, 183 },
        { 46, 17, 33, 183, 6, 98, 15, 32, 183 },
        { 57, 46, 22, 24, 128, 1, 54, 17, 37 },
        { 65, 32, 73, 115, 28, 128, 23, 128, 205 },
        { 40, 3, 9, 115, 51, 192, 18, 6, 223 },
        { 87, 37, 9, 115, 59, 77, 64, 21, 47 } },
      { { 104, 55, 44, 218, 9, 54, 53, 130, 226 },
        { 64, 90, 70, 205, 40, 41, 23, 26, 57 },
        { 54, 57, 112, 184, 5, 41, 38, 166, 213 },
        { 30, 34, 26, 133, 152, 116, 10, 32, 134 },
        { 39, 19, 53, 221, 26, 114, 32, 73, 255 },
        { 31, 9, 65, 234, 2, 15, 1, 118, 73 },
        { 75, 32, 12, 51, 192, 255, 160, 43, 51 },
        { 88, 31, 35, 67, 102, 85, 55, 186, 85 },
        { 56, 21, 23, 111, 59, 205, 45, 37, 192 },
        { 55, 38, 70, 124, 73, 102, 1, 34, 98 } },
      { { 125, 98, 42, 88, 104, 85, 117, 175, 82 },
        { 95, 84, 53, 89, 128, 100, 113, 101, 45 },
        { 75, 79, 123, 47, 51, 128, 81, 171, 1 },
        { 57, 17, 5, 71, 102, 57, 53, 41, 49 },
        { 38, 33, 13, 121, 57, 73, 26, 1, 85 },
        { 41, 10, 67, 138, 77, 110, 90, 47, 114 },
        { 115, 21, 2, 10, 102, 255, 166, 23, 6 },
        { 101, 29, 16, 10, 85, 128, 101, 196, 26 },
        { 57, 18, 10, 102, 102, 213, 34, 20, 43 },
        { 117, 20, 15, 36, 163, 128, 68, 1, 26 } },
      { { 102, 61, 71, 37, 34, 53, 31, 243, 192 },
        { 69, 60, 71, 38, 73, 119, 28, 222, 37 },
        { 68, 45, 128, 34, 1, 47, 11, 245, 171 },
        { 62, 17, 19, 70, 146, 85, 55, 62, 70 },
        { 37, 43, 37, 154, 100, 163, 85, 160, 1 },
        { 63, 9, 92, 136, 28, 64, 32, 201, 85 },
        { 75, 15, 9, 9, 64, 255, 184, 119, 16 },
        { 86, 6, 28, 5, 64, 255, 25, 248, 1 },
        { 56, 8, 17, 132, 137, 255, 55, 116, 128 },
        { 58, 15, 20, 82, 135, 57, 26, 121, 40 } },
      { { 164, 50, 31, 137, 154, 133, 25, 35, 218 },
        { 51, 103, 44, 131, 131, 123, 31, 6, 158 },
        { 86, 40, 64, 135, 148, 224, 45, 183, 128 },
        { 22, 26, 17, 131, 240, 154, 14, 1, 209 },
        { 45, 16, 21, 91, 64, 222, 7, 1, 197 },
        { 56, 21, 39, 155, 60, 138, 23, 102, 213 },
        { 83, 12, 13, 54, 192, 255, 68, 47, 28 },
        { 85, 26, 85, 85, 128, 128, 32, 146, 171 },
        { 18, 11, 7, 63, 144, 171, 4, 4, 246 },
        { 35, 27, 10, 146, 174, 171, 12, 26, 128 } },
      { { 190, 80, 35, 99, 180, 80, 126, 54, 45 },
        { 85, 126, 47, 87, 176, 51, 41, 20, 32 },
        { 101, 75, 128, 139, 118, 146, 116, 128, 85 },
        { 56, 41, 15, 176, 236, 85, 37, 9, 62 },
        { 71, 30, 17, 119, 118, 255, 17, 18, 138 },
        { 101, 38, 60, 138, 55, 70, 43, 26, 142 },
        { 146, 36, 19, 30, 171, 255, 97, 27, 20 },
        { 138, 45, 61, 62, 219, 1, 81, 188, 64 },
        { 32, 41, 20, 117, 151, 142, 20, 21, 163 },
        { 112, 19, 12, 61, 195, 128, 48, 4, 24 } }
    };

    #include "libwebp/dec/alpha_dec.c"
    #include "libwebp/dec/buffer_dec.c"
    #include "libwebp/dec/frame_dec.c"
    #include "libwebp/dec/io_dec.c"
    #include "libwebp/dec/quant_dec.c"
    #undef MAX_ALPHA
    #include "libwebp/dec/tree_dec.c"
    #include "libwebp/dec/vp8_dec.c"
    #include "libwebp/dec/vp8l_dec.c"
    #include "libwebp/dec/webp_dec.c"

    #include "libwebp/enc/frame_enc.c"
    #include "libwebp/enc/alpha_enc.c"
    #include "libwebp/enc/iterator_enc.c"
    #include "libwebp/enc/analysis_enc.c"
    #include "libwebp/enc/config_enc.c"
    #include "libwebp/enc/filter_enc.c"
    #include "libwebp/enc/predictor_enc.c"
    #include "libwebp/enc/near_lossless_enc.c"
    #include "libwebp/enc/picture_enc.c"
    #include "libwebp/enc/picture_csp_enc.c"
    #include "libwebp/enc/picture_tools_enc.c"
    #undef MAX_ALPHA
    #include "libwebp/enc/quant_enc.c"
    #include "libwebp/enc/syntax_enc.c"
    #include "libwebp/enc/token_enc.c"
    #include "libwebp/enc/tree_enc.c"
    #include "libwebp/enc/vp8l_enc.c"
    #include "libwebp/enc/webp_enc.c"
    #include "libwebp/enc/cost_enc.c"
    #undef MAX_COST
    #include "libwebp/enc/histogram_enc.c"
    #undef MAX_COST 
    #include "libwebp/enc/backward_references_enc.c"
    #include "libwebp/enc/backward_references_cost_enc.c"

    #include "libwebp/demux/demux.c"
    #undef MULTIPLIER
    #undef PREMULTIPLY
    #include "libwebp/dsp/alpha_processing.c"
    #undef MULTIPLIER
    #undef PREMULTIPLY
    #include "libwebp/dsp/cpu.c"
    #undef MAX_LEVEL
    #define MAX_LEVEL 2047
    #include "libwebp/dsp/cost.c"
    #undef MAX_LEVEL
    #include "libwebp/dsp/cost_neon.c"
    #include "libwebp/dsp/cost_sse2.c"
    #include "libwebp/dsp/dec.c"
    #include "libwebp/dsp/ssim.c"
    #include "libwebp/dsp/ssim_sse2.c"
    #include "libwebp/dsp/dec_neon.c"
    #include "libwebp/dsp/dec_sse2.c"
    #include "libwebp/dsp/dec_sse41.c"
    #include "libwebp/dsp/dec_clip_tables.c"
    #include "libwebp/dsp/enc.c"
    #include "libwebp/dsp/enc_neon.c"
    #include "libwebp/dsp/enc_sse2.c"
    #include "libwebp/dsp/enc_sse41.c"
    #include "libwebp/dsp/filters.c"
    #include "libwebp/dsp/filters_neon.c"
    #include "libwebp/dsp/filters_sse2.c"
    #include "libwebp/dsp/lossless.c"
    #include "libwebp/dsp/lossless_enc.c"
    #include "libwebp/dsp/lossless_neon.c"
    #include "libwebp/dsp/lossless_sse2.c"
    #include "libwebp/dsp/lossless_enc_neon.c"
    #include "libwebp/dsp/lossless_enc_sse2.c"
    #include "libwebp/dsp/lossless_enc_sse41.c"
    #include "libwebp/dsp/rescaler.c"
    #include "libwebp/dsp/rescaler_neon.c"
    #include "libwebp/dsp/rescaler_sse2.c"
    #include "libwebp/dsp/upsampling.c"
    #include "libwebp/dsp/upsampling_neon.c"
    #include "libwebp/dsp/upsampling_sse2.c"
    #include "libwebp/dsp/upsampling_sse41.c"
    #include "libwebp/dsp/yuv.c"
    #include "libwebp/dsp/yuv_neon.c"
    #include "libwebp/dsp/yuv_sse2.c"
    #include "libwebp/dsp/yuv_sse41.c"
    #include "libwebp/dsp/alpha_processing_neon.c"
    #include "libwebp/dsp/alpha_processing_sse2.c"
    #include "libwebp/dsp/alpha_processing_sse41.c"
    #include "libwebp/utils/bit_reader_utils.c"
    #include "libwebp/utils/bit_writer_utils.c"
    #include "libwebp/utils/color_cache_utils.c"
    #include "libwebp/utils/filters_utils.c"
    #include "libwebp/utils/huffman_utils.c"
    #include "libwebp/utils/huffman_encode_utils.c"
    #include "libwebp/utils/quant_levels_utils.c"
    #include "libwebp/utils/rescaler_utils.c"
    #include "libwebp/utils/random_utils.c"
    #include "libwebp/utils/thread_utils.c"
    #include "libwebp/utils/utils.c"

    #undef CLIP_8b_MASK
    #include "libwebp/utils/quant_levels_dec_utils.c"
    #undef CLIP_8b_MASK

    #undef HAVE_CONFIG_H
    #undef USE_DITHERING
    #undef WEBP_DISABLE_STATS
    #undef AC_APPLE_UNIVERSAL_BUILD
    #undef HAVE_BUILTIN_BSWAP16
    #undef HAVE_BUILTIN_BSWAP32
    #undef HAVE_BUILTIN_BSWAP64
    #undef HAVE_CPU_FEATURES_H
    #undef HAVE_DLFCN_H
    #undef HAVE_GLUT_GLUT_H
    #undef HAVE_GL_GLUT_H
    #undef HAVE_INTTYPES_H
    #undef HAVE_MEMORY_H
    #undef HAVE_OPENGL_GLUT_H
    #undef HAVE_PTHREAD_PRIO_INHERIT
    #undef HAVE_SHLWAPI_H
    #undef HAVE_STDINT_H
    #undef HAVE_STDLIB_H
    #undef HAVE_STRINGS_H
    #undef HAVE_STRING_H
    #undef HAVE_SYS_STAT_H
    #undef HAVE_SYS_TYPES_H
    #undef HAVE_UNISTD_H
    #undef HAVE_WINCODEC_H
    #undef HAVE_WINDOWS_H
    #undef STDC_HEADERS
    #undef WEBP_HAVE_GIF
    #undef WEBP_HAVE_GL
    #undef WEBP_HAVE_JPEG
    #undef WEBP_HAVE_NEON
    #undef WEBP_HAVE_NEON_RTCD
    #undef WEBP_HAVE_PNG
    #undef WEBP_HAVE_SDL
    #undef WEBP_HAVE_SSE2
    #undef WEBP_HAVE_SSE41
    #undef WEBP_HAVE_TIFF
    #undef WEBP_NEAR_LOSSLESS
    #undef WEBP_USE_THREAD
    #undef WORDS_BIGENDIAN
    #undef LT_OBJDIR
    #undef PACKAGE
    #undef PACKAGE_NAME
    #undef PACKAGE_TARNAME
    #undef PACKAGE_VERSION
    #undef VERSION
    #undef PACKAGE_BUGREPORT
    #undef PACKAGE_STRING
    #undef PACKAGE_URL

    JUCE_END_IGNORE_WARNINGS_MSVC
} // libwebpNamespace

//==============================================================================
/** @returns a value in bytes, or -1 on failure. */
inline int getDataSizeOfWebPImage (InputStream& input)
{
    const bool startsWithRiff = input.readByte() == 'R'
                             && input.readByte() == 'I'
                             && input.readByte() == 'F'
                             && input.readByte() == 'F';
    if (! startsWithRiff)
        return -1;

    const auto sizeOfFileBytes = input.readInt();
    if (sizeOfFileBytes <= 0)
        return -1;

    const bool hasWebPMagic = input.readByte() == 'W'
                           && input.readByte() == 'E'
                           && input.readByte() == 'B'
                           && input.readByte() == 'P';
    if (! hasWebPMagic)
        return -1;

    return sizeOfFileBytes;
}

inline Image createImageFromData (bool hasAlphaChan, int width, int height, const uint8_t* data)
{
    Image image (hasAlphaChan ? Image::ARGB : Image::RGB, width, height, hasAlphaChan);

    const Image::BitmapData destData (image, Image::BitmapData::writeOnly);

    for (int y = 0; y < height; ++y)
    {
        auto* dest = destData.getLinePointer (y);

        if (hasAlphaChan)
        {
            for (int i = width; --i >= 0;)
            {
                ((PixelARGB*) dest)->setARGB (data[3], data[0], data[1], data[2]);
                ((PixelARGB*) dest)->premultiply();
                dest += destData.pixelStride;
                data += 4;
            }
        }
        else
        {
            for (int i = width; --i >= 0;)
            {
                ((PixelRGB*) dest)->setARGB (0, data[0], data[1], data[2]);
                dest += destData.pixelStride;
                data += 4;
            }
        }
    }

    return image;
}

//==============================================================================
String WebPImageFormat::getFormatName()                             { return "WebP"; }
bool WebPImageFormat::canUnderstand (InputStream& input)            { return getDataSizeOfWebPImage (input) > 0; }
bool WebPImageFormat::usesFileExtension (const File& possibleFile)  { return possibleFile.hasFileExtension ("webp"); }

Array<Image> WebPImageFormat::decodeFrames (InputStream& input)
{
    using namespace libwebpNamespace;

    MemoryBlock data;
    input.readIntoMemoryBlock (data);

    WebPBitstreamFeatures features;
    zerostruct (features);

    if (WebPGetFeatures ((uint8_t*) data.getData(), data.getSize(), &features) != VP8_STATUS_OK
        || features.width <= 0
        || features.height <= 0)
    {
        return {};
    }

    WebPData webpData;
    WebPDataInit (&webpData);
    webpData.bytes = (const uint8_t*) data.getData();
    webpData.size = data.getSize();

    if (auto* demux = WebPDemux (&webpData))
    {
        const auto flags = WebPDemuxGetI (demux, WEBP_FF_FORMAT_FLAGS);

        WebPChunkIterator chunkIter;

        if ((flags & XMP_FLAG) != 0 && WebPDemuxGetChunk (demux, "XMP ", 1, &chunkIter) > 0)
        {
            WebPDemuxReleaseChunkIterator (&chunkIter);
        }

        if ((flags & EXIF_FLAG) != 0 && WebPDemuxGetChunk (demux, "EXIF", 1, &chunkIter) > 0)
        {
            WebPDemuxReleaseChunkIterator (&chunkIter);
        }

        if ((flags & ICCP_FLAG) != 0 && WebPDemuxGetChunk (demux, "ICCP", 1, &chunkIter) > 0)
        {
            WebPDemuxReleaseChunkIterator (&chunkIter);
        }

        Array<Image> frames;
        WebPIterator iter;
        if (WebPDemuxGetFrame (demux, 1, &iter) > 0)
        {
            do
            {
                if (auto* pixelData = WebPDecodeRGBA (iter.fragment.bytes, iter.fragment.size, nullptr, nullptr))
                {
                    auto frame = createImageFromData (iter.has_alpha > 0, iter.width, iter.height, pixelData);

                    if (frame.isValid())
                        frames.add (std::move (frame));
                }
            }
            while (WebPDemuxNextFrame (&iter) > 0);

            WebPDemuxReleaseIterator (&iter);
        }

        WebPDemuxDelete (demux);
        return frames;
    }

    return {};
}

Image WebPImageFormat::decodeImage (InputStream& input)
{
    const auto frames = decodeFrames (input);
    if (! frames.isEmpty())
        return frames.getFirst();

    return {};
}

bool WebPImageFormat::writeImageToStream (const Image&, OutputStream&)
{
    jassertfalse; // Not quite there yet...
    return false;
}

//==============================================================================
} // namespace juce
