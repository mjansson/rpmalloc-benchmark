#include "generated_constants.h"
#include "malloc_internal.h"
const struct static_bin_s static_bin_info[] __attribute__((aligned(64))) = {
// The first class of small objects try to get a maximum of 25% internal fragmentation by having sizes of the form c<<k where c is 4, 5, 6 or 7.
// We stop at when we have 4 cachelines, so that the ones that happen to be multiples of cache lines are either a power of two or odd.
//{ objsize, folio_size, objects_per_folio, folios_per_chunk, overhead_pages_per_chunk, magic: object_shift, folio_shift, object_multiply, folio_multiply},  // fragmentation(overhead bins net)
  {       8,       4*Ki,               512,              472,                       40,                   3,          12,             1lu,            1lu},  //   0    7.8%        7.8%
  {      10,      20*Ki,              2048,              100,                        8,                  36,          47,    6871947674lu,   6871947674lu},  //   1    2.3% 10.0% 12.6%
  {      12,      12*Ki,              1024,              166,                       14,                  36,          46,    5726623062lu,   5726623062lu},  //   2    2.7%  8.3% 11.3%
  {      14,      28*Ki,              2048,               72,                        6,                  36,          47,    4908534053lu,   4908534053lu},  //   3    1.6%  7.1%  8.8%
  {      16,       4*Ki,               256,              472,                       40,                   4,          12,             1lu,            1lu},  //   4    7.8%  6.2% 14.6%
  {      20,      20*Ki,              1024,              100,                        8,                  37,          47,    6871947674lu,   6871947674lu},  //   5    2.3% 15.0% 17.7%
  {      24,      12*Ki,               512,              166,                       14,                  37,          46,    5726623062lu,   5726623062lu},  //   6    2.7% 12.5% 15.6%
  {      28,      28*Ki,              1024,               72,                        6,                  37,          47,    4908534053lu,   4908534053lu},  //   7    1.6% 10.7% 12.4%
  {      32,       4*Ki,               128,              472,                       40,                   5,          12,             1lu,            1lu},  //   8    7.8%  9.4% 17.9%
  {      40,      20*Ki,               512,              100,                        8,                  38,          47,    6871947674lu,   6871947674lu},  //   9    2.3% 17.5% 20.3%
  {      48,      12*Ki,               256,              166,                       14,                  38,          46,    5726623062lu,   5726623062lu},  //  10    2.7% 14.6% 17.7%
  {      56,      28*Ki,               512,               72,                        6,                  38,          47,    4908534053lu,   4908534053lu},  //  11    1.6% 12.5% 14.3%
  {      64,       4*Ki,                64,              472,                       40,                   6,          12,             1lu,            1lu},  //  12    7.8% 10.9% 19.6%
  {      80,      20*Ki,               256,              100,                        8,                  39,          47,    6871947674lu,   6871947674lu},  //  13    2.3% 18.8% 21.5%
  {      96,      12*Ki,               128,              166,                       14,                  39,          46,    5726623062lu,   5726623062lu},  //  14    2.7% 15.6% 18.8%
  {     112,      28*Ki,               256,               72,                        6,                  39,          47,    4908534053lu,   4908534053lu},  //  15    1.6% 13.4% 15.2%
  {     128,       4*Ki,                32,              472,                       40,                   7,          12,             1lu,            1lu},  //  16    7.8%        7.8%
  {     160,      20*Ki,               128,              100,                        8,                  40,          47,    6871947674lu,   6871947674lu},  //  17    2.3% 29.4% 32.4%
  {     192,      12*Ki,                64,              166,                       14,                  40,          46,    5726623062lu,   5726623062lu},  //  18    2.7% 16.1% 19.3%
  {     224,      28*Ki,               128,               72,                        6,                  40,          47,    4908534053lu,   4908534053lu},  //  19    1.6% 13.8% 15.6%
  {     256,       4*Ki,                16,              472,                       40,                   8,          12,             1lu,            1lu},  //  20    7.8%        7.8%
// Class 2 small objects are prime multiples of a cache line.
// The folio size is such that the number of 4K pages equals the
// number of cache lines in the object.  Namely, the folio size is 64 times
// the object size.  The small_chunk_header fits into 8 pages.
//{ objsize, folio_size, objects_per_folio, folios_per_chunk, overhead_pages_per_chunk, magic: object_shift, folio_shift, object_multiply, folio_multiply},  // fragmentation(overhead bins net)
  {     320,      20*Ki,                64,              100,                        8,                  41,          47,    6871947674lu,   6871947674lu},  //  21     0.784 ( 5 cache lines, 100 folios/chunk, at least 16448 bytes used/folio)
  {     448,      28*Ki,                64,               72,                        6,                  41,          47,    4908534053lu,   4908534053lu},  //  22     0.705 ( 7 cache lines, 72 folios/chunk, at least 20544 bytes used/folio)
  {     512,       4*Ki,                 8,              472,                       40,                   9,          12,             1lu,            1lu},  //  23     0.863 ( 8 cache lines, 504 folios/chunk, at least 3592 bytes used/folio)
  {     576,      36*Ki,                64,               56,                        5,                  42,          48,    7635497416lu,   7635497416lu},  //  24     0.767 ( 9 cache lines, 56 folios/chunk, at least 28736 bytes used/folio)
  {     704,      44*Ki,                64,               46,                        4,                  42,          48,    6247225158lu,   6247225158lu},  //  25     0.792 (11 cache lines, 45 folios/chunk, at least 36928 bytes used/folio)
  {     960,      60*Ki,                64,               33,                        3,                  42,          48,    4581298450lu,   4581298450lu},  //  26     0.710 (15 cache lines, 33 folios/chunk, at least 45120 bytes used/folio)
  {    1024,       4*Ki,                 4,              472,                       40,                  10,          12,             1lu,            1lu},  //  27     0.924 (16 cache lines, 504 folios/chunk, at least 3844 bytes used/folio)
  {    1216,      76*Ki,                64,               26,                        3,                  43,          49,    7233629131lu,   7233629131lu},  //  28     0.763 (19 cache lines, 26 folios/chunk, at least 61504 bytes used/folio)
  {    1472,      92*Ki,                64,               22,                        2,                  43,          49,    5975606673lu,   5975606673lu},  //  29     0.780 (23 cache lines, 21 folios/chunk, at least 77888 bytes used/folio)
  {    1984,     124*Ki,                64,               16,                        2,                  43,          49,    4433514629lu,   4433514629lu},  //  30     0.719 (31 cache lines, 16 folios/chunk, at least 94272 bytes used/folio)
  {    2*Ki,       4*Ki,                 2,              472,                       40,                  11,          12,             1lu,            1lu},  //  31     0.954 (32 cache lines, 504 folios/chunk, at least 3970 bytes used/folio)
  {    2752,     172*Ki,                64,               11,                        1,                  44,          50,    6392509464lu,   6392509464lu},  //  32     0.666 (43 cache lines, 11 folios/chunk, at least 127040 bytes used/folio)
  {    3904,     244*Ki,                64,                8,                        1,                  44,          50,    4506195196lu,   4506195196lu},  //  33     0.672 (61 cache lines, 8 folios/chunk, at least 176192 bytes used/folio)
  {    4*Ki,       4*Ki,                 1,              472,                       40,                  12,          12,             1lu,            1lu},  //  34     0.938 (64 cache lines, 504 folios/chunk, at least 3905 bytes used/folio)
  {    5312,     332*Ki,                64,                6,                        1,                  45,          51,    6623564023lu,   6623564023lu},  //  35     0.715 (83 cache lines, 6 folios/chunk, at least 249920 bytes used/folio)
  {    7232,     452*Ki,                64,                4,                        1,                  45,          51,    4865095699lu,   4865095699lu},  //  36     0.649 (113 cache lines, 4 folios/chunk, at least 340032 bytes used/folio)
  {    8*Ki,       8*Ki,                 1,              246,                       20,                  13,          13,             1lu,            1lu},  //  37     0.869 (128 cache lines, 252 folios/chunk, at least 7233 bytes used/folio)
  {   10048,     628*Ki,                64,                3,                        1,                  46,          52,    7003258776lu,   7003258776lu},  //  38     0.662 (157 cache lines, 3 folios/chunk, at least 462912 bytes used/folio)
  {   14272,     892*Ki,                64,                2,                        1,                  46,          52,    4930545417lu,   4930545417lu},  //  39     0.613 (223 cache lines, 2 folios/chunk, at least 643136 bytes used/folio)
// large objects (page allocated):
//  So that we can return an accurate malloc_usable_size(), we maintain (in the first page of each largepage chunk) information about each object (large_object_list_cell)
//   For unallocated objects we maintain a next pointer to the next large_object_list_cell for an free object of the same size.
//   For allocated objects, we maintain the footprint.
//  This extra information always fits within one page.
//  This introduces fragmentation.  This fragmentation doesn't matter much since it will be purged. For sizes up to 1<<17 we waste the last potential object.
//   for the larger stuff, we reduce the size of the object slightly which introduces some other fragmentation
  {   16*Ki,      16*Ki,                 1,              127,                        1,                  14,          14,             1lu,            1lu},  //  40 
  {   32*Ki,      32*Ki,                 1,               63,                        1,                  15,          15,             1lu,            1lu},  //  41 
  {   64*Ki,      64*Ki,                 1,               31,                        1,                  16,          16,             1lu,            1lu},  //  42 
  {  128*Ki,     128*Ki,                 1,               15,                        1,                  17,          17,             1lu,            1lu},  //  43 
  {  252*Ki,     252*Ki,                 1,                8,                        1,                  50,          50,    4363141381lu,   4363141381lu},  //  44  (reserve a page for the list of sizes)
  {  508*Ki,     508*Ki,                 1,                4,                        1,                  51,          51,    4328785937lu,   4328785937lu},  //  45  (reserve a page for the list of sizes)
  { 1020*Ki,    1020*Ki,                 1,                2,                        1,                  52,          52,    4311810306lu,   4311810306lu},  //  46  (reserve a page for the list of sizes)
// huge objects (chunk allocated) start  at this size.
  { 1ul<<21, 1ul<<   21,                 1,                1,                        0,                  21,          21,             1lu,            1lu},  //  47
  { 1ul<<22, 1ul<<   22,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  48
  { 1ul<<23, 1ul<<   23,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  49
  { 1ul<<24, 1ul<<   24,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  50
  { 1ul<<25, 1ul<<   25,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  51
  { 1ul<<26, 1ul<<   26,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  52
  { 1ul<<27, 1ul<<   27,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  53
  { 1ul<<28, 1ul<<   28,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  54
  { 1ul<<29, 1ul<<   29,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  55
  { 1ul<<30, 1ul<<   30,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  56
  { 1ul<<31, 1ul<<   31,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  57
  { 1ul<<32, 1ul<<   32,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  58
  { 1ul<<33, 1ul<<   33,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  59
  { 1ul<<34, 1ul<<   34,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  60
  { 1ul<<35, 1ul<<   35,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  61
  { 1ul<<36, 1ul<<   36,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  62
  { 1ul<<37, 1ul<<   37,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  63
  { 1ul<<38, 1ul<<   38,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  64
  { 1ul<<39, 1ul<<   39,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  65
  { 1ul<<40, 1ul<<   40,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  66
  { 1ul<<41, 1ul<<   41,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  67
  { 1ul<<42, 1ul<<   42,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  68
  { 1ul<<43, 1ul<<   43,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  69
  { 1ul<<44, 1ul<<   44,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  70
  { 1ul<<45, 1ul<<   45,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  71
  { 1ul<<46, 1ul<<   46,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  72
  { 1ul<<47, 1ul<<   47,                 1,                1,                        0,                   1,           1,             1lu,            1lu},  //  73

};
