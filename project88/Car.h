#pragma once

#define SCALE_FACTOR int16_t /* sign.1.14, the [0., 1.] range is mapped on [0, 1<<14] */
#define SCALE_FACTOR_SHIFT 14
#define SCALE_FACTOR_HALF (1 << (SCALE_FACTOR_SHIFT-1))
#define SCALE_FACTOR_QUARTER (1 << (SCALE_FACTOR_SHIFT-2))
#define SCALE_FACTOR_EIGHTH (1 << (SCALE_FACTOR_SHIFT-3))
 
const SCALE_FACTOR ScaleFactor[] = 
{
  16384,  16199,  16015,  15831,  15646,  15462,  15278,  15093,  14909,  14725,
  14540,  14356,  14172,  13987,  13803,  13619,  13434,  13250,  13066,  12881,
  12697,  12513,  12328,  12144,  11960,  11776,  11591,  11407,  11223,  11038,
  10854,  10670,  10485,  10301,  10117,  9932,   9748,   9564,   9379,   9195,
  9011,   8826,   8642,   8458,   8273,   8089,   7905,   7720,   7536,   7352,
  7168,   6983,   6799,   6615,   6430,   6246,   6062,   5877,   5693,   5509,
  5324,   5140,   4956,   4771,   4587,   4403,   4218,   4034,   3850,   3665,
  3481,   3297,   3112,   2928,   2744,   2560,   2375,   2191,   2007,   1822
};

#define CAR_WIDTH (40)
#define CAR_HEIGHT (26)
const uint16_t CAR[] = {0x1ff8,  0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x5bde, 0xfad6, 0x3ad6, 0xb9ce, 0x79ce, 0x59ce, 0x19ce, 0xf8c6, 0xb8c6, 0x98c6, 0x58c6, 0x38c6, 0xf7bd, 0xd7bd, 0x97bd, 0x57bd, 0x37bd, 0xf6b5, 0xd6b5, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x16b5, 0x5ad6, 0xfbde, 0x1bde, 0x5ad6, 0x1ad6, 0xd9ce, 0xb9ce, 0x79ce, 0x39ce, 0x19ce, 0xd8c6, 0xb8c6, 0x78c6, 0x58c6, 0x18c6, 0xf7bd, 0xb7bd, 0x97bd, 0x57bd, 0x17bd, 0xd6b5, 0x76b5, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0xb39c, 0xd084, 0x5084, 0x5084, 0xcf7b, 0x8f7b, 0x6f7b, 0x4f7b, 0x2f7b, 0x0f7b, 0xee73, 0xce73, 0xae73, 0x8e73, 0x8e73, 0x4e73, 0x2e73, 0x2e73, 0x2e73, 0xed6b, 0xcd6b, 0xcd6b, 0xae73, 0x34a5, 0x36b5, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x718c, 0x1084, 0x894a, 0x294a, 0x094a, 0x094a, 0x094a, 0x094a, 0x094a, 0x094a, 0x094a, 0x094a, 0x094a, 0x094a, 0x094a, 0x094a, 0x094a, 0x094a, 0x094a, 0x094a, 0x094a, 0x094a, 0x694a, 0x0a52, 0xcc63, 0xb5ad, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x3294, 0xd5ad, 0x1294, 0xd294, 0xcc63, 0x6c63, 0xab5a, 0x8b5a, 0x8b5a, 0x6b5a, 0x6b5a, 0x6b5a, 0x6b5a, 0x6b5a, 0x6b5a, 0x6b5a, 0x6b5a, 0x6b5a, 0x6b5a, 0x6b5a, 0x6b5a, 0x6b5a, 0x6b5a, 0x6b5a, 0x6b5a, 0xab5a, 0x4c63, 0x6c63, 0xf39c, 0xf4a5, 0x56b5, 0x16b5, 0x4e73, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0xc840, 0xddef, 0xdce7, 0x339c, 0xf084, 0x4a52, 0xab5a, 0xcc63, 0x4d6b, 0x8d6b, 0xcd6b, 0xed6b, 0x0e73, 0x2e73, 0xed6b, 0x0e73, 0x0e73, 0x4e73, 0x4e73, 0x0e73, 0x0e73, 0x0e73, 0x0e73, 0x2e73, 0x0e73, 0x0e73, 0xed6b, 0xcd6b, 0x0c63, 0x4d6b, 0xd5ad, 0x7ad6, 0x1def, 0x7ce7, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x6f7b, 0x3294, 0x0b5a, 0x8b5a, 0x2b5a, 0x6a52, 0x6a52, 0x8a52, 0x8a52, 0x8a52, 0x8a52, 0xaa52, 0xaa52, 0xea52, 0xea52, 0xea52, 0xea52, 0xea52, 0xea52, 0xea52, 0xea52, 0xea52, 0xca52, 0xaa52, 0xca52, 0x2b5a, 0xab5a, 0xea52, 0xf294, 0x35ad, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x9084, 0xee73, 0x8e73, 0x0f7b, 0xad6b, 0x6c63, 0x2c63, 0xeb5a, 0xab5a, 0xab5a, 0xab5a, 0x6b5a, 0x6b5a, 0x4b5a, 0x2b5a, 0x0b5a, 0x2b5a, 0x4b5a, 0x4b5a, 0x4b5a, 0x4b5a, 0x4b5a, 0x4b5a, 0x8b5a, 0xcb5a, 0xcc63, 0x8e73, 0x2f7b, 0x4e73, 0x55ad, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0xf083, 0x3294, 0xcd6b, 0x0f7b, 0xae73, 0x4e73, 0xed6b, 0xed6b, 0xed6b, 0xed6b, 0xed6b, 0xed6b, 0xed6b, 0xcd6b, 0xcd6b, 0xad6b, 0xad6b, 0xad6b, 0xad6b, 0xad6b, 0xad6b, 0xad6b, 0xcd6b, 0xcd6b, 0xcd6b, 0x0e73, 0x6e73, 0xce73, 0x4f7b, 0x8e73, 0x74a5, 0x15ad, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0xd084, 0x2f7b, 0x4c63, 0xca52, 0x0a52, 0x694a, 0xc842, 0xc842, 0xc842, 0xc842, 0xc842, 0xc842, 0xc842, 0xc842, 0xc842, 0xc842, 0xc842, 0xc842, 0xc842, 0xc842, 0xc842, 0xc842, 0xc842, 0xc842, 0xc842, 0x094a, 0xa94a, 0x4a52, 0xea52, 0xab5a, 0xee73, 0x35ad, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x3084, 0xb084, 0x4c63, 0xab5a, 0x2b5a, 0x6a52, 0xc94a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x094a, 0x094a, 0x694a, 0x0a52, 0x8a52, 0x2b5a, 0xab5a, 0x4c63, 0x739c, 0x94a5, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x7084, 0xb18c, 0x5084, 0x1084, 0xaf7b, 0x4f7b, 0xee73, 0xae73, 0xae73, 0xce73, 0xee73, 0x0f7b, 0x2f7b, 0x4f7b, 0x6f7b, 0x6f7b, 0x8f7b, 0xaf7b, 0xcf7b, 0xef7b, 0x3084, 0x5084, 0x7084, 0x9084, 0xb084, 0xd084, 0x118c, 0x918c, 0x1294, 0x9294, 0xf294, 0x1294, 0x139c, 0x94a5, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1084, 0x54a5, 0xd7bd, 0x1ad6, 0x1ad6, 0x1ad6, 0x1ad6, 0x1ad6, 0x1ad6, 0x1ad6, 0x1ad6, 0x1ad6, 0x1ad6, 0x1ad6, 0x1ad6, 0x1ad6, 0x1ad6, 0x3ad6, 0x3ad6, 0x3ad6, 0x3ad6, 0x3ad6, 0x3ad6, 0x3ad6, 0x3ad6, 0x3ad6, 0x3ad6, 0x3ad6, 0x3ad6, 0x3ad6, 0x3ad6, 0x3ad6, 0xb7bd, 0x15ad, 0x34a5, 0x34a5, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x318c, 0x939c, 0xb094, 0x2f9c, 0x6f93, 0x6f93, 0x6f93, 0x4f93, 0x0f93, 0x5294, 0xaf7b, 0x8e73, 0x8e73, 0x6e73, 0x4e73, 0x0e73, 0xed6b, 0xed6b, 0xcd6b, 0xad6b, 0x8d6b, 0x4d6b, 0x2d6b, 0x0d6b, 0xac63, 0x318c, 0x2e83, 0x6c83, 0x4c83, 0x2c7b, 0x0c7b, 0x4c7b, 0x8b83, 0x2d83, 0xd294, 0x14a5, 0xf39c, 0x1ff8, 
0x1ff8, 0x1084, 0xb294, 0x518c, 0x24c4, 0xc4b2, 0x2499, 0x8499, 0x8499, 0x8499, 0xa398, 0x35c5, 0x494a, 0x4421, 0x8421, 0x8421, 0x8421, 0x8421, 0x8421, 0x8421, 0x8421, 0x8421, 0x8421, 0x8421, 0x8421, 0x8421, 0x4318, 0xf6b5, 0x6aaa, 0x8499, 0x8499, 0x8499, 0x8499, 0xe4a1, 0xa4c3, 0x06bc, 0x318c, 0x339c, 0xf39c, 0x1ff8, 
0x1ff8, 0xd084, 0x7294, 0x29c4, 0x64f4, 0x24db, 0x64b9, 0xe4c1, 0xe4c1, 0xe4c1, 0xc3b8, 0x1aee, 0x0b5a, 0x8421, 0xe421, 0xe421, 0xe421, 0xe421, 0xe421, 0xe421, 0xe421, 0xe421, 0xe421, 0xe421, 0xe421, 0xe421, 0x4318, 0x3ce7, 0x6cd3, 0xc4c1, 0xe4c1, 0xe4c1, 0xe4c1, 0x84c1, 0xe4ec, 0x24f4, 0xccac, 0x128c, 0x739c, 0x1ff8, 
0x0d6b, 0x3294, 0xb094, 0xe1f4, 0x41f4, 0x41e3, 0xa1b8, 0x41b8, 0x41b8, 0x41b8, 0x00b8, 0xf9f6, 0xa842, 0xe000, 0x4108, 0x4108, 0x4108, 0x4108, 0x4108, 0x4108, 0x4108, 0x4108, 0x4108, 0x4108, 0x4108, 0x4108, 0x0000, 0x5ce7, 0x2ad2, 0x21b8, 0x41b8, 0x41b8, 0x41b8, 0x21c0, 0xc1f4, 0xe1f4, 0xe2ec, 0x918c, 0xb294, 0x518c, 
0x0a52, 0xf18c, 0xf09c, 0xecac, 0x0cac, 0x0cac, 0x6c9b, 0xac9b, 0xac9b, 0xac9b, 0x4c9b, 0x34ad, 0x2f7b, 0xec63, 0xec63, 0xec63, 0xec63, 0x0d6b, 0x2d6b, 0x2d6b, 0x2d6b, 0x2d6b, 0x2d6b, 0x2d6b, 0x4d6b, 0x6d6b, 0xec63, 0x94a5, 0xaf9b, 0x6d9b, 0x8d9b, 0xad9b, 0xad9b, 0x2d9b, 0xcda4, 0xcda4, 0xcda4, 0xd194, 0xf18c, 0x2c63, 
0xa739, 0xcf7b, 0xcf7b, 0xd07b, 0xcf7b, 0xef7b, 0xef7b, 0xef7b, 0xef7b, 0xef7b, 0xef7b, 0xaf7b, 0xcf7b, 0xef7b, 0xef7b, 0xef7b, 0xef7b, 0xef7b, 0xef7b, 0xef7b, 0xcf7b, 0xcf7b, 0xcf7b, 0xcf7b, 0xcf7b, 0xaf7b, 0xcf7b, 0xaf7b, 0xaf7b, 0xaf7b, 0xaf7b, 0xaf7b, 0xaf7b, 0x8f7b, 0x8f7b, 0x6f7b, 0x4f7b, 0x4f7b, 0x6f7b, 0x294a, 
0x0529, 0xe529, 0x6631, 0x6631, 0x6631, 0x8631, 0x8631, 0x8631, 0x8631, 0x8631, 0x6631, 0xa631, 0xa631, 0xa631, 0xa631, 0x8631, 0xc631, 0xa631, 0xa631, 0xa631, 0xa631, 0xc631, 0xc631, 0xc631, 0xc631, 0xc631, 0xe631, 0xe631, 0xe631, 0xe631, 0xe631, 0xe631, 0x0739, 0x0739, 0x0739, 0xe631, 0xe631, 0x0739, 0x0739, 0x0739, 
0x2529, 0xa529, 0x2631, 0x2631, 0x2631, 0x4631, 0x4631, 0x4631, 0x4631, 0x4631, 0x6631, 0x6631, 0x6631, 0x6631, 0x6631, 0x6631, 0x8631, 0x8631, 0x8631, 0x8631, 0x8631, 0xa631, 0xa631, 0xa631, 0xa631, 0xa631, 0xc631, 0xc631, 0xc631, 0xc631, 0xc631, 0xc631, 0xe631, 0xe631, 0xe631, 0xe631, 0xe631, 0x0739, 0x0739, 0x0739, 
0x2529, 0xe529, 0x2631, 0x2631, 0x2631, 0x4631, 0x4631, 0x4631, 0x4631, 0x4631, 0x6631, 0x6631, 0x6631, 0x6631, 0x6631, 0x8631, 0x8631, 0x8631, 0x8631, 0x8631, 0x8631, 0xa631, 0xa631, 0xa631, 0xa631, 0xa631, 0xc631, 0xc631, 0xc631, 0xc631, 0xc631, 0xc631, 0xe631, 0xe631, 0xe631, 0xe631, 0xe631, 0x0739, 0x0739, 0x0739, 
0x2529, 0xa529, 0x0631, 0x0631, 0x2631, 0x4631, 0x6631, 0x6631, 0xa631, 0xa631, 0xc631, 0x0739, 0x0739, 0x4739, 0x4739, 0x6739, 0xa739, 0xa739, 0xc739, 0xc739, 0xa739, 0xc739, 0xc739, 0xe739, 0xe739, 0xe739, 0xe739, 0x0842, 0x0842, 0x0842, 0x2842, 0x2842, 0x4842, 0x6842, 0x4842, 0x8842, 0x8842, 0xa842, 0xa842, 0xc739, 
0x1ff8, 0xf294, 0xf294, 0xd294, 0xd294, 0xd294, 0xf294, 0xf294, 0xd294, 0xd294, 0xd294, 0xd294, 0xb294, 0x9294, 0x9294, 0x7294, 0x7294, 0x7294, 0x5294, 0x5294, 0x1294, 0x3294, 0xf18c, 0xf18c, 0xd18c, 0xd18c, 0xb18c, 0xb18c, 0x918c, 0x918c, 0x718c, 0x718c, 0x518c, 0x518c, 0xf084, 0xf084, 0xd084, 0xd084, 0xb084, 0x1ff8, 
0x1ff8, 0xc94a, 0xc94a, 0xa94a, 0xa94a, 0xa94a, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0xa842, 0xa842, 0xa842, 0xa842, 0x6842, 0x1ff8, 
0x1ff8, 0xe210, 0xa210, 0xa210, 0x8210, 0xc210, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x8210, 0x4210, 0x4210, 0x2210, 0x4210, 0x1ff8};

#define CAR_LEFT_WIDTH (44)
#define CAR_LEFT_HEIGHT (26)
const uint16_t CAR_LEFT[] = {0x1ff8,  0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x7bde, 0x3bde, 0xfad6, 0xdad6, 0xbad6, 0x7ad6, 0x5ad6, 0x1ad6, 0xf9ce, 0xd9ce, 0x99ce, 0x59ce, 0x39ce, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x59ce, 0xf9ce, 0x9ad6, 0x3bde, 0xbbde, 0xbbde, 0x7bde, 0x5bde, 0x3bde, 0xfad6, 0xdad6, 0x9ad6, 0x7ad6, 0x5ad6, 0x1ad6, 0xd9ce, 0xd9ce, 0x99ce, 0x59ce, 0x39ce, 0xf8c6, 0xf8c6, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x97bd, 0x57bd, 0xd5ad, 0x59ce, 0x1ad6, 0x5ad6, 0x1ad6, 0xd9ce, 0xd8c6, 0xb7bd, 0x96b5, 0x35ad, 0x34a5, 0x339c, 0x3294, 0x718c, 0x7084, 0x8f7b, 0x8e73, 0xed6b, 0x8d6b, 0x2d6b, 0xac63, 0x4c63, 0x4c63, 0x2a52, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x35ad, 0xf5ad, 0x18c6, 0x19ce, 0xf7bd, 0xf6b5, 0xb6b5, 0x79ce, 0x6d6b, 0xcb5a, 0x2b5a, 0x8a52, 0xe94a, 0x294a, 0xa842, 0x6842, 0x8842, 0xa842, 0xa842, 0xc842, 0xe842, 0x694a, 0xc94a, 0x2a52, 0x8a52, 0xaa52, 0xaa52, 0xaa52, 0xaa52, 0xea52, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x34a5, 0xb4a5, 0x76b5, 0x9ad6, 0x5ad6, 0x19ce, 0x56b5, 0xd39c, 0x19ce, 0x2e73, 0x8c63, 0xeb5a, 0x4b5a, 0xab5a, 0x4c63, 0x0d6b, 0xed6b, 0x0e73, 0x0e73, 0x0e73, 0x0e73, 0xcd6b, 0xcc63, 0xcb5a, 0xea52, 0xc94a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x6d6b, 0x18c6, 0xf7bd, 0x94a5, 0x77bd, 0x7ad6, 0x59ce, 0xf084, 0x35ad, 0x78c6, 0x1084, 0x0d6b, 0x6d6b, 0x8c63, 0xab5a, 0x6b5a, 0x6b5a, 0xcb5a, 0xac63, 0xec63, 0xec63, 0xcc63, 0xec63, 0xec63, 0x8c63, 0x2c63, 0xcb5a, 0x4b5a, 0xea52, 0xca52, 0xaa52, 0xaa52, 0x8a52, 0x2a52, 0xe842, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0xef7b, 0x59ce, 0x39ce, 0x98c6, 0xac63, 0x38c6, 0x1294, 0xb7bd, 0x75ad, 0x17bd, 0x5294, 0xac63, 0x2f7b, 0xee73, 0x2e73, 0xac63, 0xea52, 0xe94a, 0x494a, 0x894a, 0xe94a, 0x4a52, 0x6a52, 0x6a52, 0x8a52, 0xaa52, 0xca52, 0x2b5a, 0xcb5a, 0x4c63, 0xac63, 0x0d6b, 0x2d6b, 0xcc63, 0x6b5a, 0xe94a, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x5294, 0x139c, 0xb39c, 0x54a5, 0xf084, 0xb5ad, 0x5ad6, 0xb7bd, 0xb6b5, 0x54a5, 0x2c63, 0xec63, 0xaf7b, 0x8f7b, 0x2f7b, 0xae73, 0x2e73, 0x0d6b, 0xeb5a, 0xca52, 0x894a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0xa94a, 0x8a52, 0x6b5a, 0x0c63, 0x0c63, 0xea52, 0xca52, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x4f7b, 0xd294, 0x339c, 0xd39c, 0xd39c, 0x76b5, 0xb9ce, 0x1ad6, 0xd7bd, 0x16b5, 0x6c63, 0x0d6b, 0x2c63, 0xac63, 0x2d6b, 0x6d6b, 0x4d6b, 0x4d6b, 0x6d6b, 0x4d6b, 0xec63, 0x6c63, 0x0c63, 0xab5a, 0x4b5a, 0x6b5a, 0x4b5a, 0x4b5a, 0x4b5a, 0x0b5a, 0xaa52, 0x6a52, 0x0a52, 0x6a52, 0xab5a, 0xea52, 0x6b5a, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0xcc63, 0x518c, 0xb294, 0x539c, 0xf39c, 0x74a5, 0xb6b5, 0x3ad6, 0xb294, 0x57bd, 0xed6b, 0x3084, 0x0e73, 0x2c63, 0x6a52, 0xe842, 0xa842, 0xc842, 0xc842, 0xc842, 0xe842, 0xe842, 0x094a, 0x094a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x094a, 0x094a, 0x094a, 0x494a, 0xe94a, 0x8a52, 0x2b5a, 0xeb5a, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x0f7b, 0xed6b, 0xf18c, 0xd294, 0x739c, 0xf39c, 0x54a5, 0xef7b, 0x8c63, 0xf6b5, 0x4d6b, 0xeb5a, 0x6b5a, 0xca52, 0x0a52, 0x694a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x094a, 0x094a, 0x094a, 0x094a, 0x294a, 0x4a52, 0xea52, 0xab5a, 0xac63, 0x0d6b, 0x6e73, 0x2f7b, 0x3084, 0xf084, 0xf084, 0x118c, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x6d6b, 0x8a52, 0x5084, 0x3294, 0xd294, 0x739c, 0x34a5, 0xec63, 0x118c, 0x36b5, 0xce73, 0xcc63, 0xcc63, 0x8c63, 0xec63, 0xec63, 0x8d6b, 0x2e73, 0x4e73, 0xce73, 0xee73, 0x4f7b, 0x8f7b, 0x1084, 0x5084, 0x9084, 0x118c, 0x318c, 0x318c, 0x718c, 0x718c, 0x718c, 0x518c, 0xd18c, 0xf18c, 0x128c, 0x328c, 0x328c, 0xd28c, 0x718c, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0xce73, 0xeb5a, 0x4b5a, 0x1084, 0xf18c, 0xf294, 0x939c, 0x74a5, 0xf4a5, 0x35ad, 0xf084, 0xf184, 0x1184, 0x3184, 0x3184, 0x3184, 0x3184, 0xf084, 0xd084, 0x7084, 0x3084, 0xaf7b, 0x2f7b, 0xee73, 0x6e73, 0x0e73, 0xcd6b, 0x6d6b, 0x0d6b, 0xab5a, 0xaa52, 0x2b5a, 0x518c, 0xa679, 0x2679, 0xe479, 0x0479, 0xa280, 0x21a3, 0xb094, 0x918c, 0x1ff8, 0x1ff8, 
0x1ff8, 0x0842, 0xec63, 0x4a52, 0x6b5a, 0x2f7b, 0x7294, 0x139c, 0x939c, 0x34a5, 0xb18c, 0xd084, 0xab83, 0x697a, 0x8771, 0x4671, 0x4571, 0x0370, 0x6b8a, 0x494a, 0x6210, 0x2318, 0x8318, 0xe318, 0x4421, 0x8421, 0xe421, 0x2529, 0x6529, 0x8529, 0xa529, 0xe529, 0xa94a, 0x1be6, 0x05b9, 0x26b9, 0x06b9, 0x06c1, 0x05c1, 0x25ec, 0xc9cc, 0xd18c, 0x1ff8, 0x1ff8, 
0x1ff8, 0x8108, 0x4c63, 0x8d6b, 0x6c63, 0xed6b, 0xf084, 0x7294, 0x139c, 0x539c, 0xd084, 0x0f94, 0x84d4, 0x85ba, 0x45b1, 0xc5b1, 0xc5b9, 0x45b9, 0xb2d4, 0xef7b, 0x4529, 0x8529, 0x6529, 0x4529, 0x0529, 0xe421, 0xc421, 0x8421, 0x4421, 0x0421, 0xe318, 0xa318, 0xa739, 0x7cef, 0xa1b8, 0xe2b8, 0x82b8, 0x22b8, 0x61c0, 0xe1f4, 0x00f4, 0xecac, 0xfad1, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0xaf7b, 0x4f7b, 0xad6b, 0xce73, 0x118c, 0xb294, 0x918c, 0xf184, 0xeabc, 0xc3ec, 0xa3ca, 0x23b8, 0x23b8, 0xe2b8, 0xe1b8, 0xb1dc, 0x8e73, 0x8108, 0xc108, 0x8108, 0x4108, 0x2108, 0x0108, 0x4108, 0xa108, 0xe108, 0x6210, 0xa210, 0x2318, 0x6739, 0x1be6, 0x23b0, 0xc4b1, 0x26b1, 0xa7a9, 0xc9aa, 0xcabc, 0xecb4, 0x2dac, 0x3294, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0xb084, 0xcf7b, 0x885a, 0x2b73, 0xef73, 0x318c, 0x318c, 0x3184, 0xc1ec, 0xe0f4, 0x40d1, 0x00c0, 0x20c0, 0x81b8, 0xa2b0, 0x11cc, 0xcf7b, 0xe739, 0x2a52, 0xcb5a, 0xad6b, 0x6f7b, 0x318c, 0x518c, 0x918c, 0x918c, 0x918c, 0xd18c, 0xd18c, 0xf18c, 0x3294, 0x1294, 0x3294, 0x3294, 0x5294, 0x5294, 0x3294, 0x718c, 0xef7b, 0xae73, 0xcc63, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0xcf7b, 0xec93, 0x40dc, 0x2742, 0x3084, 0x718c, 0x718c, 0x6f9c, 0x9094, 0x308c, 0x918c, 0x728c, 0x928c, 0x928c, 0x328c, 0x5294, 0xd294, 0xf294, 0xf294, 0x9294, 0x918c, 0x9084, 0x8f7b, 0xae73, 0x8d6b, 0xac63, 0xeb5a, 0xca52, 0x6a52, 0xa94a, 0x494a, 0x8842, 0x4842, 0x8739, 0x2739, 0x8631, 0x6631, 0xa631, 0xa631, 0xc631, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x6f7b, 0xd28c, 0x088b, 0xc421, 0xc739, 0xb084, 0x5084, 0xef7b, 0xcf7b, 0x6f7b, 0x0f7b, 0x0e73, 0xec63, 0xeb5a, 0xaa52, 0xa94a, 0x6842, 0x6739, 0x4631, 0xa529, 0xe529, 0xe529, 0x0631, 0x4631, 0x6631, 0x6631, 0x8631, 0xa631, 0xc631, 0xc631, 0xc631, 0xc631, 0xe631, 0xe631, 0xe631, 0xe631, 0xe631, 0x0739, 0x0739, 0x0739, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0xce73, 0xf294, 0xb18c, 0x4631, 0x4529, 0x2631, 0x6631, 0x4631, 0x4631, 0x4631, 0x4631, 0x4631, 0x4631, 0x4631, 0x6631, 0x6631, 0x6631, 0x6631, 0x8631, 0x8631, 0x8631, 0x8631, 0x8631, 0xa631, 0xa631, 0xa631, 0xa631, 0xa631, 0xc631, 0xc631, 0xc631, 0xc631, 0xc631, 0xe631, 0xe631, 0xe631, 0xe631, 0x0739, 0xe631, 0xe631, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0xcd6b, 0x339c, 0x94a5, 0x6b5a, 0xe421, 0xe529, 0x2631, 0x2631, 0x2631, 0x4631, 0x4631, 0x4631, 0x4631, 0x4631, 0x6631, 0x6631, 0x6631, 0x6631, 0x8631, 0x8631, 0x6631, 0x2631, 0x0631, 0x0631, 0xc529, 0xa529, 0x6529, 0x4529, 0xe529, 0x8739, 0x294a, 0xea52, 0x6c63, 0x0e73, 0xaf7b, 0x318c, 0x9294, 0x939c, 0x54a5, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x6b5a, 0xd18c, 0xb5ad, 0x2c63, 0xa421, 0xc529, 0x0631, 0x0631, 0x0631, 0x0631, 0x0631, 0x2631, 0x8631, 0xe631, 0x8739, 0x0842, 0x8842, 0x094a, 0xa94a, 0x6a52, 0x2c63, 0x6e73, 0xb084, 0x139c, 0x55ad, 0xb7bd, 0xd9ce, 0x9bde, 0xbce7, 0x7ce7, 0x3ad6, 0x19ce, 0x58c6, 0x57bd, 0x95ad, 0x539c, 0xf8c6, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0xe739, 0x2f7b, 0x16b5, 0xce73, 0xc739, 0x694a, 0xa94a, 0x694a, 0x6b5a, 0x6e73, 0x318c, 0x7294, 0x339c, 0xd39c, 0x74a5, 0x15ad, 0xd5ad, 0x76b5, 0x37bd, 0xd7bd, 0x58c6, 0xf8c6, 0x99ce, 0x3ad6, 0x7dea, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x094a, 0x4842, 0x6739, 0xa631, 0xc529, 0x4529, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0xc529, 0xca52, 0x6e73, 0xd6b5, 0x17bd, 0xf39c, 0x9084, 0x8d6b, 0x6c63, 0xce73, 0x518c, 0x9294, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x4421, 0x6318, 0x8318, 0x4318, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0xe108, 0x6108, 0x4108, 0x0108, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8};

#define CAR_RIGHT_WIDTH (44)
#define CAR_RIGHT_HEIGHT (26)
const uint16_t CAR_RIGHT[] = {0x1ff8,  0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x39ce, 0x59ce, 0x99ce, 0xd9ce, 0xf9ce, 0x1ad6, 0x5ad6, 0x7ad6, 0xbad6, 0xdad6, 0xfad6, 0x3bde, 0x7bde, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0xf8c6, 0xf8c6, 0x39ce, 0x59ce, 0x99ce, 0xd9ce, 0xd9ce, 0x1ad6, 0x5ad6, 0x7ad6, 0x9ad6, 0xdad6, 0xfad6, 0x3bde, 0x5bde, 0x7bde, 0xbbde, 0xbbde, 0x3bde, 0x9ad6, 0xf9ce, 0x59ce, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x2a52, 0x4c63, 0x4c63, 0xac63, 0x2d6b, 0x8d6b, 0xed6b, 0x8e73, 0x8f7b, 0x7084, 0x718c, 0x3294, 0x339c, 0x34a5, 0x35ad, 0x96b5, 0xb7bd, 0xd8c6, 0xd9ce, 0x1ad6, 0x5ad6, 0x1ad6, 0x59ce, 0xd5ad, 0x57bd, 0x97bd, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0xea52, 0xaa52, 0xaa52, 0xaa52, 0xaa52, 0x8a52, 0x2a52, 0xc94a, 0x694a, 0xe842, 0xc842, 0xa842, 0xa842, 0x8842, 0x6842, 0xa842, 0x294a, 0xe94a, 0x8a52, 0x2b5a, 0xcb5a, 0x6d6b, 0x79ce, 0xb6b5, 0xf6b5, 0xf7bd, 0x19ce, 0x18c6, 0xf5ad, 0x35ad, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0xc94a, 0xea52, 0xcb5a, 0xcc63, 0xcd6b, 0x0e73, 0x0e73, 0x0e73, 0x0e73, 0xed6b, 0x0d6b, 0x4c63, 0xab5a, 0x4b5a, 0xeb5a, 0x8c63, 0x2e73, 0x19ce, 0xd39c, 0x56b5, 0x19ce, 0x5ad6, 0x9ad6, 0x76b5, 0xb4a5, 0x34a5, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0xe842, 0x2a52, 0x8a52, 0xaa52, 0xaa52, 0xca52, 0xea52, 0x4b5a, 0xcb5a, 0x2c63, 0x8c63, 0xec63, 0xec63, 0xcc63, 0xec63, 0xec63, 0xac63, 0xcb5a, 0x6b5a, 0x6b5a, 0xab5a, 0x8c63, 0x6d6b, 0x0d6b, 0x1084, 0x78c6, 0x35ad, 0xf084, 0x59ce, 0x7ad6, 0x77bd, 0x94a5, 0xf7bd, 0x18c6, 0x6d6b, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0xe94a, 0x6b5a, 0xcc63, 0x2d6b, 0x0d6b, 0xac63, 0x4c63, 0xcb5a, 0x2b5a, 0xca52, 0xaa52, 0x8a52, 0x6a52, 0x6a52, 0x4a52, 0xe94a, 0x894a, 0x494a, 0xe94a, 0xea52, 0xac63, 0x2e73, 0xee73, 0x2f7b, 0xac63, 0x5294, 0x17bd, 0x75ad, 0xb7bd, 0x1294, 0x38c6, 0xac63, 0x98c6, 0x39ce, 0x59ce, 0xef7b, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0xca52, 0xea52, 0x0c63, 0x0c63, 0x6b5a, 0x8a52, 0xa94a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x894a, 0xca52, 0xeb5a, 0x0d6b, 0x2e73, 0xae73, 0x2f7b, 0x8f7b, 0xaf7b, 0xec63, 0x2c63, 0x54a5, 0xb6b5, 0xb7bd, 0x5ad6, 0xb5ad, 0xf084, 0x54a5, 0xb39c, 0x139c, 0x5294, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x6b5a, 0xea52, 0xab5a, 0x6a52, 0x0a52, 0x6a52, 0xaa52, 0x0b5a, 0x4b5a, 0x4b5a, 0x4b5a, 0x6b5a, 0x4b5a, 0xab5a, 0x0c63, 0x6c63, 0xec63, 0x4d6b, 0x6d6b, 0x4d6b, 0x4d6b, 0x6d6b, 0x2d6b, 0xac63, 0x2c63, 0x0d6b, 0x6c63, 0x16b5, 0xd7bd, 0x1ad6, 0xb9ce, 0x76b5, 0xd39c, 0xd39c, 0x339c, 0xd294, 0x4f7b, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0xeb5a, 0x2b5a, 0x8a52, 0xe94a, 0x494a, 0x094a, 0x094a, 0x094a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x094a, 0x094a, 0xe842, 0xe842, 0xc842, 0xc842, 0xc842, 0xa842, 0xe842, 0x6a52, 0x2c63, 0x0e73, 0x3084, 0xed6b, 0x57bd, 0xb294, 0x3ad6, 0xb6b5, 0x74a5, 0xf39c, 0x539c, 0xb294, 0x518c, 0xcc63, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x118c, 0xf084, 0xf084, 0x3084, 0x2f7b, 0x6e73, 0x0d6b, 0xac63, 0xab5a, 0xea52, 0x4a52, 0x294a, 0x094a, 0x094a, 0x094a, 0x094a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x294a, 0x694a, 0x0a52, 0xca52, 0x6b5a, 0xeb5a, 0x4d6b, 0xf6b5, 0x8c63, 0xef7b, 0x54a5, 0xf39c, 0x739c, 0xd294, 0xf18c, 0xed6b, 0x0f7b, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x718c, 0xd28c, 0x328c, 0x328c, 0x128c, 0xf18c, 0xd18c, 0x518c, 0x718c, 0x718c, 0x718c, 0x318c, 0x318c, 0x118c, 0x9084, 0x5084, 0x1084, 0x8f7b, 0x4f7b, 0xee73, 0xce73, 0x4e73, 0x2e73, 0x8d6b, 0xec63, 0xec63, 0x8c63, 0xcc63, 0xcc63, 0xce73, 0x36b5, 0x118c, 0xec63, 0x34a5, 0x739c, 0xd294, 0x3294, 0x5084, 0x8a52, 0x6d6b, 0x1ff8, 
0x1ff8, 0x1ff8, 0x918c, 0xb094, 0x21a3, 0xa280, 0x0479, 0xe479, 0x2679, 0xa679, 0x518c, 0x2b5a, 0xaa52, 0xab5a, 0x0d6b, 0x6d6b, 0xcd6b, 0x0e73, 0x6e73, 0xee73, 0x2f7b, 0xaf7b, 0x3084, 0x7084, 0xd084, 0xf084, 0x3184, 0x3184, 0x3184, 0x3184, 0x1184, 0xf184, 0xf084, 0x35ad, 0xf4a5, 0x74a5, 0x939c, 0xf294, 0xf18c, 0x1084, 0x4b5a, 0xeb5a, 0xce73, 0x1ff8, 
0x1ff8, 0x1ff8, 0xd18c, 0xc9cc, 0x25ec, 0x05c1, 0x06c1, 0x06b9, 0x26b9, 0x05b9, 0x1be6, 0xa94a, 0xe529, 0xa529, 0x8529, 0x6529, 0x2529, 0xe421, 0x8421, 0x4421, 0xe318, 0x8318, 0x2318, 0x6210, 0x494a, 0x6b8a, 0x0370, 0x4571, 0x4671, 0x8771, 0x697a, 0xab83, 0xd084, 0xb18c, 0x34a5, 0x939c, 0x139c, 0x7294, 0x2f7b, 0x6b5a, 0x4a52, 0xec63, 0x0842, 0x1ff8, 
0x1ff8, 0xfad1, 0xecac, 0x00f4, 0xe1f4, 0x61c0, 0x22b8, 0x82b8, 0xe2b8, 0xa1b8, 0x7cef, 0xa739, 0xa318, 0xe318, 0x0421, 0x4421, 0x8421, 0xc421, 0xe421, 0x0529, 0x4529, 0x6529, 0x8529, 0x4529, 0xef7b, 0xb2d4, 0x45b9, 0xc5b9, 0xc5b1, 0x45b1, 0x85ba, 0x84d4, 0x0f94, 0xd084, 0x539c, 0x139c, 0x7294, 0xf084, 0xed6b, 0x6c63, 0x8d6b, 0x4c63, 0x8108, 0x1ff8, 
0x1ff8, 0x3294, 0x2dac, 0xecb4, 0xcabc, 0xc9aa, 0xa7a9, 0x26b1, 0xc4b1, 0x23b0, 0x1be6, 0x6739, 0x2318, 0xa210, 0x6210, 0xe108, 0xa108, 0x4108, 0x0108, 0x2108, 0x4108, 0x8108, 0xc108, 0x8108, 0x8e73, 0xb1dc, 0xe1b8, 0xe2b8, 0x23b8, 0x23b8, 0xa3ca, 0xc3ec, 0xeabc, 0xf184, 0x918c, 0xb294, 0x118c, 0xce73, 0xad6b, 0x4f7b, 0xaf7b, 0x1ff8, 0x1ff8, 0x1ff8, 
0xcc63, 0xae73, 0xef7b, 0x718c, 0x3294, 0x5294, 0x5294, 0x3294, 0x3294, 0x1294, 0x3294, 0xf18c, 0xd18c, 0xd18c, 0x918c, 0x918c, 0x918c, 0x518c, 0x318c, 0x6f7b, 0xad6b, 0xcb5a, 0x2a52, 0xe739, 0xcf7b, 0x11cc, 0xa2b0, 0x81b8, 0x20c0, 0x00c0, 0x40d1, 0xe0f4, 0xc1ec, 0x3184, 0x318c, 0x318c, 0xef73, 0x2b73, 0x885a, 0xcf7b, 0xb084, 0x1ff8, 0x1ff8, 0x1ff8, 
0xc631, 0xa631, 0xa631, 0x6631, 0x8631, 0x2739, 0x8739, 0x4842, 0x8842, 0x494a, 0xa94a, 0x6a52, 0xca52, 0xeb5a, 0xac63, 0x8d6b, 0xae73, 0x8f7b, 0x9084, 0x918c, 0x9294, 0xf294, 0xf294, 0xd294, 0x5294, 0x328c, 0x928c, 0x928c, 0x728c, 0x918c, 0x308c, 0x9094, 0x6f9c, 0x718c, 0x718c, 0x3084, 0x2742, 0x40dc, 0xec93, 0xcf7b, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x0739, 0x0739, 0x0739, 0xe631, 0xe631, 0xe631, 0xe631, 0xe631, 0xc631, 0xc631, 0xc631, 0xc631, 0xa631, 0x8631, 0x6631, 0x6631, 0x4631, 0x0631, 0xe529, 0xe529, 0xa529, 0x4631, 0x6739, 0x6842, 0xa94a, 0xaa52, 0xeb5a, 0xec63, 0x0e73, 0x0f7b, 0x6f7b, 0xcf7b, 0xef7b, 0x5084, 0xb084, 0xc739, 0xc421, 0x088b, 0xd28c, 0x6f7b, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0xe631, 0xe631, 0x0739, 0xe631, 0xe631, 0xe631, 0xe631, 0xc631, 0xc631, 0xc631, 0xc631, 0xc631, 0xa631, 0xa631, 0xa631, 0xa631, 0xa631, 0x8631, 0x8631, 0x8631, 0x8631, 0x8631, 0x6631, 0x6631, 0x6631, 0x6631, 0x4631, 0x4631, 0x4631, 0x4631, 0x4631, 0x4631, 0x4631, 0x6631, 0x2631, 0x4529, 0x4631, 0xb18c, 0xf294, 0xce73, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x54a5, 0x939c, 0x9294, 0x318c, 0xaf7b, 0x0e73, 0x6c63, 0xea52, 0x294a, 0x8739, 0xe529, 0x4529, 0x6529, 0xa529, 0xc529, 0x0631, 0x0631, 0x2631, 0x6631, 0x8631, 0x8631, 0x6631, 0x6631, 0x6631, 0x6631, 0x4631, 0x4631, 0x4631, 0x4631, 0x4631, 0x2631, 0x2631, 0x2631, 0xe529, 0xe421, 0x6b5a, 0x94a5, 0x339c, 0xcd6b, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0xf8c6, 0x539c, 0x95ad, 0x57bd, 0x58c6, 0x19ce, 0x3ad6, 0x7ce7, 0xbce7, 0x9bde, 0xd9ce, 0xb7bd, 0x55ad, 0x139c, 0xb084, 0x6e73, 0x2c63, 0x6a52, 0xa94a, 0x094a, 0x8842, 0x0842, 0x8739, 0xe631, 0x8631, 0x2631, 0x0631, 0x0631, 0x0631, 0x0631, 0x0631, 0xc529, 0xa421, 0x2c63, 0xb5ad, 0xd18c, 0x6b5a, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x4529, 0xc529, 0xa631, 0x6739, 0x4842, 0x094a, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x7dea, 0x3ad6, 0x99ce, 0xf8c6, 0x58c6, 0xd7bd, 0x37bd, 0x76b5, 0xd5ad, 0x15ad, 0x74a5, 0xd39c, 0x339c, 0x7294, 0x318c, 0x6e73, 0x6b5a, 0x694a, 0xa94a, 0x694a, 0xc739, 0xce73, 0x16b5, 0x2f7b, 0xe739, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x9294, 0x518c, 0xce73, 0x6c63, 0x8d6b, 0x9084, 0xf39c, 0x17bd, 0xd6b5, 0x6e73, 0xca52, 0xc529, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x4318, 0x8318, 0x6318, 0x4421, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 
0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x0108, 0x4108, 0x6108, 0xe108, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8, 0x1ff8};
