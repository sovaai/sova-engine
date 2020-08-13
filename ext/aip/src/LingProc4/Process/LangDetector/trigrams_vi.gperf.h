/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: /usr/local/bin/gperf -r -m 20 trigrams_vi.gperf  */
/* Computed positions: -k'1-5,$' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 9 "trigrams_vi.gperf"

#ifndef _TRIGRAM_VI_H_
#define _TRIGRAM_VI_H_
#include <_include/_string.h>
#line 14 "trigrams_vi.gperf"
struct trigram_vi
{
    const char  *trigram;
    const float  freq;
};

#define TOTAL_KEYWORDS 256
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 7
#define MIN_HASH_VALUE 7
#define MAX_HASH_VALUE 388
/* maximum key range = 382, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash_vi (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      389, 389, 389, 389, 389, 389, 389, 389, 389, 389,
      389, 389, 389, 389, 389, 389, 389, 389, 389, 389,
      389, 389, 389, 389, 389, 389, 389, 389, 389, 389,
      389, 389, 389, 389, 389, 389, 389, 389, 389, 389,
      389, 389, 389, 389, 389, 389, 389, 389, 389, 389,
      389, 389, 389, 389, 389, 389, 389, 389, 389, 389,
      389, 389, 389, 389, 389, 389, 389, 389, 389, 389,
      389, 389, 389, 389, 389, 389, 389, 389, 389, 389,
      389, 389, 389, 389, 389, 389, 389, 389, 389, 389,
      389, 389, 389, 389, 389, 389, 389,   7, 150,  10,
      167,  22, 389,   1,   0,   4, 389,  90, 141,  38,
        3,  31,  57, 117,  36, 178,  44,  43, 108, 389,
      389,  47, 389, 389, 389, 389, 389, 389, 389,  81,
      389, 123, 389,  28, 389,  34, 389,  23, 389,  15,
      389, 181, 389, 389, 389,  81, 389, 122, 389, 389,
      389, 389, 389,  94, 389, 143, 389,  90, 389, 154,
       13,   2,  95,  77, 389,   3, 389,  88, 389, 115,
       29,  47, 119,  43, 389, 144,   0, 136,  31, 183,
       81, 389, 389, 389, 389,   9,  37,   3, 389, 389,
      389,  29, 389, 389, 389,  20,  21,  78,  16, 389,
      389, 389, 389, 389, 389, 389, 389, 389, 389, 389,
      389, 389, 389, 389, 389, 389, 389, 389, 389, 389,
      389, 389, 389, 389, 389,  32, 389, 389, 389, 389,
      389, 389, 389, 389, 389, 389, 389, 389, 389, 389,
      389, 389, 389, 389, 389, 389, 389, 389, 389, 389,
      389, 389, 389, 389, 389, 389
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      /*FALLTHROUGH*/
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      /*FALLTHROUGH*/
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      /*FALLTHROUGH*/
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

#ifdef __GNUC__
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#else
__inline
#endif
#endif
const struct trigram_vi *
get_trigram_vi (register const char *str, register unsigned int len)
{
  static const struct trigram_vi wordlist[] =
    {
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0},
#line 43 "trigrams_vi.gperf"
      {"ngh", 0.0052195415},
      {"",0.0}, {"",0.0},
#line 59 "trigrams_vi.gperf"
      {"inh", 0.0040162527},
      {"",0.0},
#line 125 "trigrams_vi.gperf"
      {"ghi", 0.0021383506},
#line 34 "trigrams_vi.gperf"
      {"anh", 0.0062586763},
#line 50 "trigrams_vi.gperf"
      {"nhi", 0.0045136814},
#line 49 "trigrams_vi.gperf"
      {"ang", 0.004535578},
#line 186 "trigrams_vi.gperf"
      {"han", 0.0014647866},
      {"",0.0},
#line 126 "trigrams_vi.gperf"
      {"hai", 0.002132699},
      {"",0.0},
#line 210 "trigrams_vi.gperf"
      {"nha", 0.0013448674},
#line 94 "trigrams_vi.gperf"
      {"chi", 0.0027815567},
#line 74 "trigrams_vi.gperf"
      {"gia", 0.0034310229},
#line 35 "trigrams_vi.gperf"
      {"nh\306\260", 0.00591357},
#line 30 "trigrams_vi.gperf"
      {"ng\306\260", 0.006788293},
#line 97 "trigrams_vi.gperf"
      {"\306\260ng", 0.0026856763},
#line 111 "trigrams_vi.gperf"
      {"h\306\260n", 0.0024005969},
#line 69 "trigrams_vi.gperf"
      {"\306\241ng", 0.003494107},
#line 154 "trigrams_vi.gperf"
      {"h\306\241n", 0.0017621275},
#line 160 "trigrams_vi.gperf"
      {"\303\241nh", 0.0017156809},
#line 162 "trigrams_vi.gperf"
      {"ch\306\260", 0.001692891},
#line 142 "trigrams_vi.gperf"
      {"\303\241ng", 0.0018620073},
#line 264 "trigrams_vi.gperf"
      {"h\303\241n", 0.0010975578},
#line 100 "trigrams_vi.gperf"
      {"gi\303\241", 0.0026389821},
#line 230 "trigrams_vi.gperf"
      {"h\306\260a", 0.001227408},
      {"",0.0},
#line 75 "trigrams_vi.gperf"
      {"\303\241ch", 0.0033984203},
      {"",0.0},
#line 86 "trigrams_vi.gperf"
      {"\303\271ng", 0.0029741465},
#line 27 "trigrams_vi.gperf"
      {"ong", 0.007099618},
#line 52 "trigrams_vi.gperf"
      {"\303\240nh", 0.0042827353},
#line 129 "trigrams_vi.gperf"
      {"h\306\260\306\241", 0.0020298264},
#line 53 "trigrams_vi.gperf"
      {"\303\240ng", 0.0042604944},
#line 37 "trigrams_vi.gperf"
      {"h\303\240n", 0.005784823},
#line 252 "trigrams_vi.gperf"
      {"c\303\241i", 0.0011345848},
#line 68 "trigrams_vi.gperf"
      {"\306\260\306\241n", 0.0035371624},
#line 110 "trigrams_vi.gperf"
      {"h\303\241c", 0.0024142312},
#line 258 "trigrams_vi.gperf"
      {"oan", 0.0011160417},
#line 202 "trigrams_vi.gperf"
      {"\341\273\245ng", 0.0013701225},
#line 206 "trigrams_vi.gperf"
      {"c\303\271n", 0.0013540717},
#line 104 "trigrams_vi.gperf"
      {"con", 0.0024973224},
#line 84 "trigrams_vi.gperf"
      {"ung", 0.0030391794},
#line 245 "trigrams_vi.gperf"
      {"ran", 0.0011635649},
#line 96 "trigrams_vi.gperf"
      {"nh\303\240", 0.0027169168},
#line 153 "trigrams_vi.gperf"
      {"ng\303\240", 0.0017655079},
#line 95 "trigrams_vi.gperf"
      {"thi", 0.00272139},
#line 25 "trigrams_vi.gperf"
      {"c\303\241c", 0.0076426053},
#line 175 "trigrams_vi.gperf"
      {"tin", 0.0015548001},
#line 159 "trigrams_vi.gperf"
      {"\341\273\213nh", 0.0017181193},
#line 147 "trigrams_vi.gperf"
      {"uan", 0.0018235162},
#line 151 "trigrams_vi.gperf"
      {"\303\262ng", 0.0017892343},
#line 116 "trigrams_vi.gperf"
      {"tha", 0.0022726362},
#line 117 "trigrams_vi.gperf"
      {"n\303\252n", 0.0022587383},
#line 48 "trigrams_vi.gperf"
      {"i\303\252n", 0.004816938},
#line 113 "trigrams_vi.gperf"
      {"th\306\260", 0.0023569008},
#line 214 "trigrams_vi.gperf"
      {"\341\273\213ch", 0.001316065},
#line 137 "trigrams_vi.gperf"
      {"\303\272ng", 0.001902237},
#line 236 "trigrams_vi.gperf"
      {"h\303\272n", 0.0012085743},
#line 184 "trigrams_vi.gperf"
      {"phi", 0.0014821078},
#line 211 "trigrams_vi.gperf"
      {"ngo", 0.0013339622},
#line 88 "trigrams_vi.gperf"
      {"\303\255nh", 0.0029171717},
#line 119 "trigrams_vi.gperf"
      {"c\303\262n", 0.0022031358},
#line 270 "trigrams_vi.gperf"
      {"th\303\241", 0.0010788102},
#line 169 "trigrams_vi.gperf"
      {"h\303\255n", 0.0016019087},
#line 120 "trigrams_vi.gperf"
      {"o\303\240n", 0.002197468},
#line 33 "trigrams_vi.gperf"
      {"cho", 0.0064451676},
#line 40 "trigrams_vi.gperf"
      {"ron", 0.0057309056},
#line 138 "trigrams_vi.gperf"
      {"\303\255ch", 0.0018814224},
#line 191 "trigrams_vi.gperf"
      {"i\341\273\205n", 0.001448951},
#line 212 "trigrams_vi.gperf"
      {"\341\272\241nh", 0.0013213777},
#line 198 "trigrams_vi.gperf"
      {"ch\341\273\213", 0.0014047328},
#line 268 "trigrams_vi.gperf"
      {"ho\303\240", 0.0010807317},
#line 244 "trigrams_vi.gperf"
      {"cao", 0.0011738563},
#line 135 "trigrams_vi.gperf"
      {"nh\341\272\245", 0.0019316045},
#line 47 "trigrams_vi.gperf"
      {"i\341\273\207n", 0.0049239714},
#line 115 "trigrams_vi.gperf"
      {"ph\303\241", 0.0023362963},
#line 164 "trigrams_vi.gperf"
      {"hi\303\252", 0.0016564669},
#line 174 "trigrams_vi.gperf"
      {"heo", 0.0015642896},
#line 223 "trigrams_vi.gperf"
      {"run", 0.0012466186},
#line 209 "trigrams_vi.gperf"
      {"nam", 0.0013452172},
#line 255 "trigrams_vi.gperf"
      {"ch\341\272\245", 0.0011294767},
#line 172 "trigrams_vi.gperf"
      {"the", 0.0015803567},
#line 179 "trigrams_vi.gperf"
      {"\341\273\253ng", 0.0015221222},
#line 187 "trigrams_vi.gperf"
      {"ngu", 0.001461923},
#line 127 "trigrams_vi.gperf"
      {"th\303\240", 0.0020705243},
#line 108 "trigrams_vi.gperf"
      {"r\303\252n", 0.0024435448},
#line 98 "trigrams_vi.gperf"
      {"ch\341\273\211", 0.0026811764},
#line 128 "trigrams_vi.gperf"
      {"tra", 0.0020586988},
#line 103 "trigrams_vi.gperf"
      {"i\341\273\207c", 0.0025220662},
#line 81 "trigrams_vi.gperf"
      {"chu", 0.003221171},
#line 73 "trigrams_vi.gperf"
      {"tr\306\260", 0.0034589586},
#line 44 "trigrams_vi.gperf"
      {"khi", 0.0052176574},
#line 181 "trigrams_vi.gperf"
      {"n\303\240o", 0.001500441},
#line 237 "trigrams_vi.gperf"
      {"ph\341\273\245", 0.0012050163},
#line 131 "trigrams_vi.gperf"
      {"hay", 0.0019706122},
      {"",0.0},
#line 157 "trigrams_vi.gperf"
      {"y\303\252n", 0.0017493385},
      {"",0.0},
#line 134 "trigrams_vi.gperf"
      {"ch\303\272", 0.0019316906},
#line 241 "trigrams_vi.gperf"
      {"ho\341\272\241", 0.0011877866},
#line 20 "trigrams_vi.gperf"
      {"\303\264ng", 0.014878705},
#line 21 "trigrams_vi.gperf"
      {"h\303\264n", 0.009353469},
#line 45 "trigrams_vi.gperf"
      {"hi\341\273\207", 0.0052046743},
#line 66 "trigrams_vi.gperf"
      {"i\341\272\277n", 0.0035892017},
#line 156 "trigrams_vi.gperf"
      {"h\303\241t", 0.0017500812},
#line 183 "trigrams_vi.gperf"
      {"o\341\272\241i", 0.0014837281},
      {"",0.0},
#line 224 "trigrams_vi.gperf"
      {"t\303\255n", 0.0012445408},
#line 77 "trigrams_vi.gperf"
      {"kh\303\241", 0.0033326664},
#line 143 "trigrams_vi.gperf"
      {"\304\221an", 0.0018551552},
#line 130 "trigrams_vi.gperf"
      {"ch\303\255", 0.002002601},
#line 78 "trigrams_vi.gperf"
      {"c\303\264n", 0.0032943422},
#line 188 "trigrams_vi.gperf"
      {"\341\273\243ng", 0.0014604967},
#line 144 "trigrams_vi.gperf"
      {"h\341\272\255n", 0.0018447345},
#line 112 "trigrams_vi.gperf"
      {"th\341\272\245", 0.0023889544},
#line 82 "trigrams_vi.gperf"
      {"h\303\242n", 0.0032003187},
#line 79 "trigrams_vi.gperf"
      {"\341\273\221ng", 0.0032827696},
#line 246 "trigrams_vi.gperf"
      {"y\341\273\207n", 0.0011592695},
#line 121 "trigrams_vi.gperf"
      {"t\341\272\241i", 0.0021890819},
      {"",0.0},
#line 217 "trigrams_vi.gperf"
      {"ti\303\252", 0.0013063279},
#line 152 "trigrams_vi.gperf"
      {"i\341\273\201n", 0.0017848905},
#line 205 "trigrams_vi.gperf"
      {"g\303\240y", 0.0013583185},
#line 83 "trigrams_vi.gperf"
      {"thu", 0.0030439699},
#line 64 "trigrams_vi.gperf"
      {"n\303\240y", 0.003764944},
#line 56 "trigrams_vi.gperf"
      {"\341\273\235ng", 0.004130897},
#line 61 "trigrams_vi.gperf"
      {"hi\341\272\277", 0.0038304233},
#line 180 "trigrams_vi.gperf"
      {"\306\260\341\273\243n", 0.001519038},
#line 197 "trigrams_vi.gperf"
      {"h\341\273\235i", 0.001413528},
#line 107 "trigrams_vi.gperf"
      {"\341\273\231ng", 0.0024482384},
#line 85 "trigrams_vi.gperf"
      {"huy", 0.0030317781},
#line 247 "trigrams_vi.gperf"
      {"guy", 0.0011589412},
      {"",0.0},
#line 240 "trigrams_vi.gperf"
      {"i\303\252u", 0.001188314},
#line 26 "trigrams_vi.gperf"
      {"\306\260\341\273\243c", 0.0071829623},
#line 41 "trigrams_vi.gperf"
      {"tro", 0.0057232194},
#line 23 "trigrams_vi.gperf"
      {"\303\254nh", 0.008178402},
#line 259 "trigrams_vi.gperf"
      {"h\306\260\341\273\235", 0.0011056802},
#line 32 "trigrams_vi.gperf"
      {"g\306\260\341\273\235", 0.006532635},
#line 124 "trigrams_vi.gperf"
      {"h\303\254n", 0.0021395993},
#line 55 "trigrams_vi.gperf"
      {"\306\260\341\273\235n", 0.004216497},
#line 31 "trigrams_vi.gperf"
      {"\306\260\341\273\235i", 0.006774368},
#line 22 "trigrams_vi.gperf"
      {"c\341\273\247a", 0.009124552},
#line 89 "trigrams_vi.gperf"
      {"\304\203ng", 0.0029171016},
#line 132 "trigrams_vi.gperf"
      {"\341\272\243nh", 0.0019538454},
#line 225 "trigrams_vi.gperf"
      {"kho", 0.0012433244},
#line 262 "trigrams_vi.gperf"
      {"\341\272\243ng", 0.0010989035},
#line 51 "trigrams_vi.gperf"
      {"t\303\264i", 0.0043835677},
#line 243 "trigrams_vi.gperf"
      {"m\303\241y", 0.0011846969},
#line 62 "trigrams_vi.gperf"
      {"h\341\272\243i", 0.003821036},
#line 150 "trigrams_vi.gperf"
      {"\341\273\251ng", 0.001799499},
#line 168 "trigrams_vi.gperf"
      {"\304\221\341\273\213n", 0.0016020809},
#line 105 "trigrams_vi.gperf"
      {"tr\303\252", 0.0024575181},
#line 102 "trigrams_vi.gperf"
      {"nh\341\272\255", 0.0025305601},
#line 173 "trigrams_vi.gperf"
      {"i\341\273\207u", 0.0015709479},
#line 93 "trigrams_vi.gperf"
      {"h\341\272\245t", 0.0027995778},
#line 114 "trigrams_vi.gperf"
      {"i\341\273\207t", 0.002340387},
#line 109 "trigrams_vi.gperf"
      {"\341\273\223ng", 0.0024335654},
#line 222 "trigrams_vi.gperf"
      {"h\341\272\247n", 0.0012598705},
#line 149 "trigrams_vi.gperf"
      {"tru", 0.0018087624},
#line 133 "trigrams_vi.gperf"
      {"u\341\273\221n", 0.0019370463},
#line 145 "trigrams_vi.gperf"
      {"h\341\272\245y", 0.0018413165},
#line 171 "trigrams_vi.gperf"
      {"uy\303\252", 0.0015821544},
#line 273 "trigrams_vi.gperf"
      {"i\341\273\203n", 0.0010612145},
#line 265 "trigrams_vi.gperf"
      {"y\341\273\201n", 0.001092299},
#line 176 "trigrams_vi.gperf"
      {"h\341\273\251c", 0.0015386414},
#line 155 "trigrams_vi.gperf"
      {"th\341\272\277", 0.0017546942},
#line 67 "trigrams_vi.gperf"
      {"qua", 0.0035637366},
#line 234 "trigrams_vi.gperf"
      {"c\341\272\247n", 0.0012108457},
#line 238 "trigrams_vi.gperf"
      {"khu", 0.0012014422},
#line 92 "trigrams_vi.gperf"
      {"ti\341\272\277", 0.0028063382},
#line 254 "trigrams_vi.gperf"
      {"\304\221i\341\273\207", 0.0011302572},
#line 251 "trigrams_vi.gperf"
      {"b\303\241n", 0.0011408072},
#line 193 "trigrams_vi.gperf"
      {"r\306\260\341\273\235", 0.0014355537},
#line 182 "trigrams_vi.gperf"
      {"u\341\273\221c", 0.0014984495},
#line 271 "trigrams_vi.gperf"
      {"r\303\254n", 0.0010764634},
#line 261 "trigrams_vi.gperf"
      {"y\303\252u", 0.0010998831},
#line 99 "trigrams_vi.gperf"
      {"m\303\254n", 0.0026704166},
#line 208 "trigrams_vi.gperf"
      {"qu\303\241", 0.0013467621},
#line 36 "trigrams_vi.gperf"
      {"\341\273\257ng", 0.005855443},
#line 38 "trigrams_vi.gperf"
      {"h\341\273\257n", 0.0057582064},
#line 207 "trigrams_vi.gperf"
      {"sin", 0.0013525699},
#line 256 "trigrams_vi.gperf"
      {"i\341\273\207p", 0.0011241048},
#line 185 "trigrams_vi.gperf"
      {"t\303\254n", 0.0014770266},
#line 200 "trigrams_vi.gperf"
      {"vi\303\252", 0.0013957331},
#line 72 "trigrams_vi.gperf"
      {"i\341\272\277t", 0.0034778407},
#line 239 "trigrams_vi.gperf"
      {"h\341\273\261c", 0.0011885401},
#line 140 "trigrams_vi.gperf"
      {"u\341\273\231c", 0.001880518},
#line 194 "trigrams_vi.gperf"
      {"uy\341\273\207", 0.001433729},
#line 213 "trigrams_vi.gperf"
      {"\341\273\237ng", 0.0013160704},
#line 196 "trigrams_vi.gperf"
      {"l\303\252n", 0.0014246646},
#line 122 "trigrams_vi.gperf"
      {"r\341\272\245t", 0.0021690046},
#line 54 "trigrams_vi.gperf"
      {"\305\251ng", 0.004251403},
#line 139 "trigrams_vi.gperf"
      {"n\306\260\341\273\233", 0.0018811747},
#line 269 "trigrams_vi.gperf"
      {"th\341\272\255", 0.0010797414},
#line 216 "trigrams_vi.gperf"
      {"h\341\272\255t", 0.0013065701},
#line 70 "trigrams_vi.gperf"
      {"hi\341\273\201", 0.003494037},
#line 90 "trigrams_vi.gperf"
      {"v\303\240o", 0.002905556},
#line 220 "trigrams_vi.gperf"
      {"u\341\272\245t", 0.0012698822},
#line 87 "trigrams_vi.gperf"
      {"\304\221\341\272\277n", 0.0029245242},
#line 60 "trigrams_vi.gperf"
      {"\306\260\341\273\233c", 0.0039771805},
#line 46 "trigrams_vi.gperf"
      {"i\341\273\201u", 0.005091263},
#line 163 "trigrams_vi.gperf"
      {"\303\263ng", 0.0016723832},
#line 58 "trigrams_vi.gperf"
      {"c\305\251n", 0.004072307},
#line 215 "trigrams_vi.gperf"
      {"\306\260\341\273\237n", 0.0013160489},
#line 101 "trigrams_vi.gperf"
      {"\341\272\261ng", 0.0026364469},
#line 235 "trigrams_vi.gperf"
      {"d\341\273\245n", 0.001208881},
#line 161 "trigrams_vi.gperf"
      {"nh\303\242", 0.0017058093},
#line 201 "trigrams_vi.gperf"
      {"n\303\263i", 0.0013849194},
      {"",0.0},
#line 57 "trigrams_vi.gperf"
      {"vi\341\273\207", 0.004081829},
#line 249 "trigrams_vi.gperf"
      {"i\341\272\277p", 0.0011531764},
#line 170 "trigrams_vi.gperf"
      {"uy\341\272\277", 0.001595245},
#line 275 "trigrams_vi.gperf"
      {"\341\272\257ng", 0.0010534581},
      {"",0.0},
#line 80 "trigrams_vi.gperf"
      {"l\341\272\241i", 0.003222748},
#line 219 "trigrams_vi.gperf"
      {"\341\273\215ng", 0.0012759645},
#line 250 "trigrams_vi.gperf"
      {"n\304\203m", 0.0011433477},
#line 178 "trigrams_vi.gperf"
      {"\304\221i\341\273\201", 0.0015277847},
#line 136 "trigrams_vi.gperf"
      {"m\341\273\233i", 0.0019032543},
#line 167 "trigrams_vi.gperf"
      {"th\303\264", 0.0016199243},
#line 232 "trigrams_vi.gperf"
      {"h\341\273\243p", 0.0012271712},
#line 91 "trigrams_vi.gperf"
      {"b\341\272\241n", 0.0028266683},
#line 106 "trigrams_vi.gperf"
      {"gi\341\272\243", 0.0024520117},
#line 29 "trigrams_vi.gperf"
      {"\304\221\306\260\341\273\243", 0.0069181},
#line 166 "trigrams_vi.gperf"
      {"v\341\272\253n", 0.001628418},
#line 192 "trigrams_vi.gperf"
      {"r\306\260\341\273\233", 0.0014387293},
#line 229 "trigrams_vi.gperf"
      {"c\341\272\243m", 0.0012302877},
#line 272 "trigrams_vi.gperf"
      {"y\341\272\277t", 0.0010631145},
#line 267 "trigrams_vi.gperf"
      {"t\303\242m", 0.0010920514},
#line 141 "trigrams_vi.gperf"
      {"\304\221\341\273\231n", 0.0018778914},
#line 148 "trigrams_vi.gperf"
      {"h\341\273\215c", 0.0018228757},
      {"",0.0},
#line 177 "trigrams_vi.gperf"
      {"i\341\273\203m", 0.0015347013},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 231 "trigrams_vi.gperf"
      {"\304\221\306\260\341\273\235", 0.0012273919},
      {"",0.0}, {"",0.0},
#line 204 "trigrams_vi.gperf"
      {"ti\341\273\201", 0.0013592175},
      {"",0.0},
#line 248 "trigrams_vi.gperf"
      {"r\341\272\261n", 0.0011559539},
#line 195 "trigrams_vi.gperf"
      {"i\341\273\203u", 0.0014307416},
#line 71 "trigrams_vi.gperf"
      {"l\303\240m", 0.0034822708},
      {"",0.0},
#line 253 "trigrams_vi.gperf"
      {"gh\304\251", 0.0011329646},
#line 158 "trigrams_vi.gperf"
      {"quy", 0.0017208967},
#line 274 "trigrams_vi.gperf"
      {"c\341\272\247u", 0.0010597127},
      {"",0.0},
#line 28 "trigrams_vi.gperf"
      {"m\341\273\231t", 0.007055583},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 189 "trigrams_vi.gperf"
      {"th\341\273\235", 0.001458322},
      {"",0.0}, {"",0.0},
#line 221 "trigrams_vi.gperf"
      {"di\341\273\205", 0.0012650378},
#line 203 "trigrams_vi.gperf"
      {"\304\221\341\273\223n", 0.0013623716},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 165 "trigrams_vi.gperf"
      {"sau", 0.001632229},
      {"",0.0},
#line 24 "trigrams_vi.gperf"
      {"kh\303\264", 0.007841687},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 146 "trigrams_vi.gperf"
      {"ch\341\273\251", 0.0018325159},
#line 226 "trigrams_vi.gperf"
      {"cu\341\273\231", 0.0012417473},
      {"",0.0},
#line 260 "trigrams_vi.gperf"
      {"mu\341\273\221", 0.0011044206},
      {"",0.0},
#line 63 "trigrams_vi.gperf"
      {"ph\341\272\243", 0.00377333},
#line 123 "trigrams_vi.gperf"
      {"bi\341\272\277", 0.0021426622},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 266 "trigrams_vi.gperf"
      {"uy\341\273\201", 0.0010920569},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0},
#line 42 "trigrams_vi.gperf"
      {"v\341\273\233i", 0.0055448986},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 257 "trigrams_vi.gperf"
      {"s\341\273\221n", 0.0011208107},
#line 65 "trigrams_vi.gperf"
      {"th\303\254", 0.0037444362},
#line 199 "trigrams_vi.gperf"
      {"b\341\272\243n", 0.0013982845},
#line 118 "trigrams_vi.gperf"
      {"\304\221\341\272\247u", 0.0022462988},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 190 "trigrams_vi.gperf"
      {"th\341\273\251", 0.0014567127},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 76 "trigrams_vi.gperf"
      {"th\341\273\203", 0.0033891783},
#line 39 "trigrams_vi.gperf"
      {"nh\341\273\257", 0.005739459},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 263 "trigrams_vi.gperf"
      {"s\341\272\243n", 0.0010977677},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0},
#line 233 "trigrams_vi.gperf"
      {"tr\303\254", 0.0012115454},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 228 "trigrams_vi.gperf"
      {"th\341\273\261", 0.00123236},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 242 "trigrams_vi.gperf"
      {"qu\341\273\221", 0.0011850145},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 218 "trigrams_vi.gperf"
      {"b\341\272\261n", 0.0012864338},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0},
#line 227 "trigrams_vi.gperf"
      {"qu\341\272\243", 0.0012352343}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash_vi (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].trigram;

          if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
            return &wordlist[key];
        }
    }
  return 0;
}
#line 276 "trigrams_vi.gperf"


#ifdef TOTAL_KEYWORDS
#undef TOTAL_KEYWORDS
#endif
#ifdef MIN_WORD_LENGTH
#undef MIN_WORD_LENGTH
#endif
#ifdef MAX_WORD_LENGTH
#undef MAX_WORD_LENGTH
#endif
#ifdef MIN_HASH_VALUE
#undef MIN_HASH_VALUE
#endif
#ifdef MAX_HASH_VALUE
#undef MAX_HASH_VALUE
#endif

#endif /* _TRIGRAM_VI_H_ */
