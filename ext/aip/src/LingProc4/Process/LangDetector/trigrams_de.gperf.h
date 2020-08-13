/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: /usr/local/bin/gperf -r -m 20 trigrams_de.gperf  */
/* Computed positions: -k'1-3' */

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

#line 9 "trigrams_de.gperf"

#ifndef _TRIGRAM_DE_H_
#define _TRIGRAM_DE_H_
#include <_include/_string.h>
#line 14 "trigrams_de.gperf"
struct trigram_de
{
    const char  *trigram;
    const float  freq;
};

#define TOTAL_KEYWORDS 256
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 4
#define MIN_HASH_VALUE 5
#define MAX_HASH_VALUE 549
/* maximum key range = 545, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash_de (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      550, 550, 550, 550, 550, 550, 550, 550, 550, 550,
      550, 550, 550, 550, 550, 550, 550, 550, 550, 550,
      550, 550, 550, 550, 550, 550, 550, 550, 550, 550,
      550, 550, 550, 550, 550, 550, 550, 550, 550, 550,
      550, 550, 550, 550, 550, 550, 550, 550, 550, 550,
      550, 550, 550, 550, 550, 550, 550, 550, 550, 550,
      550, 550, 550, 550, 550, 550, 550, 550, 550, 550,
      550, 550, 550, 550, 550, 550, 550, 550, 550, 550,
      550, 550, 550, 550, 550, 550, 550, 550, 550, 550,
      550, 550, 550, 550, 550, 550, 550,  92, 111, 140,
       87,   4, 221, 113,   2, 204, 276, 189,   1,  13,
       29, 139,  19,  70,   0,  14,  60,  18,   7,  51,
       61, 238,  55, 550,   1, 140,  65, 118,  29, 550,
      550, 100,  39, 156,   5,   2, 274, 550, 550, 125,
      550, 550, 550, 550, 550, 550, 550, 550, 550, 550,
      550, 550, 550, 550, 550, 550, 550, 550, 550, 550,
      550, 550, 550, 550, 550, 550, 550, 550, 550, 550,
      550, 550, 550, 550, 550, 550, 550, 550, 550, 550,
      550, 550, 550, 550, 550, 550, 550, 550,   0, 550,
      550,   6, 550, 550, 550,   3, 550, 550,   0, 550,
      550, 550, 550, 550, 550, 550, 550, 550, 550, 550,
      550, 550,   5, 550, 550, 550, 550, 550, 550, 550,
      550, 550, 550, 550, 550, 550, 550, 550, 550, 550,
      550, 550, 550, 550, 550, 550, 550, 550, 550, 550,
      550, 550, 550, 550, 550, 550, 550, 550, 550, 550,
      550, 550, 550, 550, 550, 550, 550, 550, 550, 550,
      550, 550, 550, 550, 550, 550, 550, 550, 550, 550,
      550, 550, 550
    };
  return len + asso_values[(unsigned char)str[2]] + asso_values[(unsigned char)str[1]+3] + asso_values[(unsigned char)str[0]+17];
}

#ifdef __GNUC__
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#else
__inline
#endif
#endif
const struct trigram_de *
get_trigram_de (register const char *str, register unsigned int len)
{
  static const struct trigram_de wordlist[] =
    {
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 105 "trigrams_de.gperf"
      {"vor", 0.002095853},
      {"",0.0},
#line 38 "trigrams_de.gperf"
      {"ver", 0.004996526},
      {"",0.0}, {"",0.0},
#line 260 "trigrams_de.gperf"
      {"vie", 8.7533117e-4},
#line 62 "trigrams_de.gperf"
      {"abe", 0.0032577196},
#line 147 "trigrams_de.gperf"
      {"eil", 0.0014903344},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 265 "trigrams_de.gperf"
      {"kom", 8.5973146e-4},
#line 90 "trigrams_de.gperf"
      {"ebe", 0.0023785196},
#line 37 "trigrams_de.gperf"
      {"ber", 0.005036836},
      {"",0.0},
#line 66 "trigrams_de.gperf"
      {"ese", 0.0030848747},
      {"",0.0},
#line 22 "trigrams_de.gperf"
      {"der", 0.013593841},
#line 181 "trigrams_de.gperf"
      {"ass", 0.0012200224},
#line 140 "trigrams_de.gperf"
      {"eis", 0.0015521093},
#line 24 "trigrams_de.gperf"
      {"die", 0.011709021},
      {"",0.0},
#line 157 "trigrams_de.gperf"
      {"err", 0.0013775172},
#line 149 "trigrams_de.gperf"
      {"erl", 0.0014796017},
#line 194 "trigrams_de.gperf"
      {"erh", 0.0011528813},
#line 246 "trigrams_de.gperf"
      {"ess", 9.515826e-4},
#line 54 "trigrams_de.gperf"
      {"ere", 0.0035066912},
#line 134 "trigrams_de.gperf"
      {"bes", 0.0016191256},
#line 70 "trigrams_de.gperf"
      {"von", 0.002854623},
#line 155 "trigrams_de.gperf"
      {"ken", 0.0014012288},
#line 73 "trigrams_de.gperf"
      {"dem", 0.002781741},
#line 78 "trigrams_de.gperf"
      {"des", 0.0026834006},
      {"",0.0}, {"",0.0},
#line 21 "trigrams_de.gperf"
      {"ein", 0.015597843},
      {"",0.0},
#line 47 "trigrams_de.gperf"
      {"ers", 0.003806081},
#line 210 "trigrams_de.gperf"
      {"sol", 0.0010693916},
#line 132 "trigrams_de.gperf"
      {"ser", 0.0016865163},
#line 153 "trigrams_de.gperf"
      {"sel", 0.0014392921},
#line 241 "trigrams_de.gperf"
      {"seh", 9.684303e-4},
#line 43 "trigrams_de.gperf"
      {"sie", 0.004373037},
#line 46 "trigrams_de.gperf"
      {"ben", 0.0038844538},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 28 "trigrams_de.gperf"
      {"den", 0.009165519},
#line 69 "trigrams_de.gperf"
      {"sse", 0.0029876574},
      {"",0.0},
#line 152 "trigrams_de.gperf"
      {"f\303\274r", 0.0014486519},
#line 259 "trigrams_de.gperf"
      {"spr", 8.7857596e-4},
#line 80 "trigrams_de.gperf"
      {"ern", 0.0025533612},
#line 254 "trigrams_de.gperf"
      {"ate", 9.0478343e-4},
      {"",0.0},
#line 117 "trigrams_de.gperf"
      {"ier", 0.0018833224},
#line 166 "trigrams_de.gperf"
      {"iel", 0.0013090032},
      {"",0.0},
#line 239 "trigrams_de.gperf"
      {"ute", 9.744206e-4},
#line 214 "trigrams_de.gperf"
      {"omm", 0.0010556639},
#line 145 "trigrams_de.gperf"
      {"ete", 0.0015015661},
#line 104 "trigrams_de.gperf"
      {"ger", 0.0020997217},
#line 177 "trigrams_de.gperf"
      {"gel", 0.0012442332},
#line 206 "trigrams_de.gperf"
      {"geh", 0.0010828698},
#line 270 "trigrams_de.gperf"
      {"ise", 8.428838e-4},
#line 151 "trigrams_de.gperf"
      {"mer", 0.0014497751},
#line 41 "trigrams_de.gperf"
      {"eit", 0.004616517},
#line 237 "trigrams_de.gperf"
      {"sin", 9.807852e-4},
#line 60 "trigrams_de.gperf"
      {"sen", 0.0033032708},
#line 83 "trigrams_de.gperf"
      {"ies", 0.0024655038},
      {"",0.0},
#line 231 "trigrams_de.gperf"
      {"fre", 9.995049e-4},
#line 112 "trigrams_de.gperf"
      {"est", 0.0019408541},
#line 59 "trigrams_de.gperf"
      {"aus", 0.0033236127},
#line 264 "trigrams_de.gperf"
      {"erw", 8.6097943e-4},
#line 68 "trigrams_de.gperf"
      {"ges", 0.0029991388},
#line 222 "trigrams_de.gperf"
      {"art", 0.0010208454},
#line 179 "trigrams_de.gperf"
      {"dur", 0.0012291328},
#line 217 "trigrams_de.gperf"
      {"erz", 0.0010430593},
#line 110 "trigrams_de.gperf"
      {"ene", 0.0019725529},
#line 141 "trigrams_de.gperf"
      {"fen", 0.0015265257},
      {"",0.0}, {"",0.0},
#line 103 "trigrams_de.gperf"
      {"ert", 0.0021145726},
#line 201 "trigrams_de.gperf"
      {"ien", 0.0011180628},
#line 178 "trigrams_de.gperf"
      {"imm", 0.001233875},
#line 92 "trigrams_de.gperf"
      {"mme", 0.002311628},
#line 188 "trigrams_de.gperf"
      {"uns", 0.0011669835},
#line 216 "trigrams_de.gperf"
      {"str", 0.0010481761},
#line 100 "trigrams_de.gperf"
      {"ens", 0.0021746003},
#line 29 "trigrams_de.gperf"
      {"gen", 0.008347345},
#line 256 "trigrams_de.gperf"
      {"cke", 8.8706217e-4},
#line 39 "trigrams_de.gperf"
      {"ste", 0.0048845825},
#line 212 "trigrams_de.gperf"
      {"eid", 0.0010612798},
#line 79 "trigrams_de.gperf"
      {"men", 0.0026630585},
      {"",0.0}, {"",0.0},
#line 99 "trigrams_de.gperf"
      {"ann", 0.0021978125},
      {"",0.0}, {"",0.0},
#line 229 "trigrams_de.gperf"
      {"rer", 0.001003124},
      {"",0.0}, {"",0.0},
#line 131 "trigrams_de.gperf"
      {"rie", 0.0016940043},
#line 118 "trigrams_de.gperf"
      {"enn", 0.0018800776},
#line 170 "trigrams_de.gperf"
      {"ort", 0.0012967731},
#line 205 "trigrams_de.gperf"
      {"rbe", 0.0010979703},
#line 114 "trigrams_de.gperf"
      {"da\303\237", 0.0019306208},
#line 125 "trigrams_de.gperf"
      {"ite", 0.0017270757},
#line 138 "trigrams_de.gperf"
      {"att", 0.0015688321},
#line 144 "trigrams_de.gperf"
      {"erd", 0.0015106763},
#line 183 "trigrams_de.gperf"
      {"etz", 0.0012001797},
#line 169 "trigrams_de.gperf"
      {"gew", 0.0012982707},
      {"",0.0},
#line 163 "trigrams_de.gperf"
      {"gte", 0.0013211087},
#line 251 "trigrams_de.gperf"
      {"era", 9.085274e-4},
      {"",0.0},
#line 56 "trigrams_de.gperf"
      {"das", 0.0034588936},
#line 72 "trigrams_de.gperf"
      {"ner", 0.0028198045},
#line 248 "trigrams_de.gperf"
      {"eig", 9.3885325e-4},
#line 67 "trigrams_de.gperf"
      {"ist", 0.0030272182},
#line 122 "trigrams_de.gperf"
      {"\303\274be", 0.0017739995},
#line 94 "trigrams_de.gperf"
      {"ede", 0.0022954044},
#line 173 "trigrams_de.gperf"
      {"anz", 0.0012602074},
#line 58 "trigrams_de.gperf"
      {"mit", 0.003367916},
      {"",0.0},
#line 211 "trigrams_de.gperf"
      {"eut", 0.001063027},
#line 30 "trigrams_de.gperf"
      {"ine", 0.008308533},
#line 182 "trigrams_de.gperf"
      {"rin", 0.0012046724},
#line 48 "trigrams_de.gperf"
      {"ren", 0.0037077402},
#line 247 "trigrams_de.gperf"
      {"dig", 9.4996026e-4},
#line 193 "trigrams_de.gperf"
      {"nem", 0.0011528813},
#line 263 "trigrams_de.gperf"
      {"dan", 8.645986e-4},
#line 191 "trigrams_de.gperf"
      {"unt", 0.0011589964},
#line 238 "trigrams_de.gperf"
      {"erb", 9.772909e-4},
#line 98 "trigrams_de.gperf"
      {"ent", 0.002199934},
#line 200 "trigrams_de.gperf"
      {"erg", 0.0011226803},
#line 273 "trigrams_de.gperf"
      {"ins", 8.317768e-4},
#line 81 "trigrams_de.gperf"
      {"all", 0.002545499},
      {"",0.0},
#line 268 "trigrams_de.gperf"
      {"ler", 8.5186923e-4},
      {"",0.0},
#line 146 "trigrams_de.gperf"
      {"ied", 0.0014947023},
#line 113 "trigrams_de.gperf"
      {"lie", 0.0019329919},
#line 272 "trigrams_de.gperf"
      {"ode", 8.321512e-4},
#line 142 "trigrams_de.gperf"
      {"ell", 0.0015217834},
#line 111 "trigrams_de.gperf"
      {"eic", 0.0019628184},
#line 40 "trigrams_de.gperf"
      {"nen", 0.004850887},
#line 209 "trigrams_de.gperf"
      {"ele", 0.0010748828},
      {"",0.0},
#line 192 "trigrams_de.gperf"
      {"\303\274ck", 0.0011577484},
#line 93 "trigrams_de.gperf"
      {"als", 0.0023027675},
#line 167 "trigrams_de.gperf"
      {"esc", 0.00130788},
#line 87 "trigrams_de.gperf"
      {"rte", 0.0024256932},
#line 233 "trigrams_de.gperf"
      {"zen", 9.975082e-4},
#line 51 "trigrams_de.gperf"
      {"and", 0.003557858},
#line 35 "trigrams_de.gperf"
      {"ter", 0.0055787074},
#line 172 "trigrams_de.gperf"
      {"tel", 0.0012804245},
      {"",0.0},
#line 150 "trigrams_de.gperf"
      {"fra", 0.001459759},
#line 25 "trigrams_de.gperf"
      {"und", 0.01048825},
#line 176 "trigrams_de.gperf"
      {"urc", 0.0012458556},
#line 52 "trigrams_de.gperf"
      {"end", 0.0035569845},
      {"",0.0},
#line 159 "trigrams_de.gperf"
      {"itt", 0.0013503113},
#line 123 "trigrams_de.gperf"
      {"rst", 0.0017432993},
#line 196 "trigrams_de.gperf"
      {"ieb", 0.0011450191},
#line 130 "trigrams_de.gperf"
      {"oll", 0.0017004937},
#line 242 "trigrams_de.gperf"
      {"ieg", 9.650608e-4},
#line 102 "trigrams_de.gperf"
      {"len", 0.002131545},
#line 213 "trigrams_de.gperf"
      {"ide", 0.0010601566},
#line 71 "trigrams_de.gperf"
      {"nte", 0.0028284155},
#line 161 "trigrams_de.gperf"
      {"geb", 0.001322731},
      {"",0.0},
#line 165 "trigrams_de.gperf"
      {"geg", 0.0013179887},
#line 186 "trigrams_de.gperf"
      {"gan", 0.0011692299},
#line 224 "trigrams_de.gperf"
      {"tre", 0.0010152294},
#line 235 "trigrams_de.gperf"
      {"nur", 9.864012e-4},
#line 53 "trigrams_de.gperf"
      {"sic", 0.003516675},
#line 109 "trigrams_de.gperf"
      {"man", 0.0019805399},
#line 116 "trigrams_de.gperf"
      {"sta", 0.001897674},
#line 101 "trigrams_de.gperf"
      {"ang", 0.0021516376},
      {"",0.0},
#line 154 "trigrams_de.gperf"
      {"nst", 0.0014179517},
#line 271 "trigrams_de.gperf"
      {"zur", 8.378919e-4},
#line 31 "trigrams_de.gperf"
      {"ten", 0.007890586},
#line 34 "trigrams_de.gperf"
      {"ung", 0.005708497},
#line 225 "trigrams_de.gperf"
      {"ahr", 0.0010149798},
#line 187 "trigrams_de.gperf"
      {"run", 0.0011673579},
      {"",0.0},
#line 135 "trigrams_de.gperf"
      {"nne", 0.0016155065},
      {"",0.0}, {"",0.0},
#line 89 "trigrams_de.gperf"
      {"lte", 0.0024183302},
#line 119 "trigrams_de.gperf"
      {"ehr", 0.0018497518},
      {"",0.0}, {"",0.0},
#line 148 "trigrams_de.gperf"
      {"alt", 0.0014822225},
#line 129 "trigrams_de.gperf"
      {"ehe", 0.0017004937},
#line 128 "trigrams_de.gperf"
      {"auc", 0.0017120999},
#line 106 "trigrams_de.gperf"
      {"isc", 0.0020293356},
      {"",0.0}, {"",0.0},
#line 115 "trigrams_de.gperf"
      {"rau", 0.001929872},
#line 215 "trigrams_de.gperf"
      {"elt", 0.0010505472},
#line 143 "trigrams_de.gperf"
      {"kei", 0.0015180395},
#line 208 "trigrams_de.gperf"
      {"nun", 0.0010751324},
      {"",0.0}, {"",0.0},
#line 61 "trigrams_de.gperf"
      {"tte", 0.0032950342},
#line 96 "trigrams_de.gperf"
      {"ind", 0.002253098},
      {"",0.0},
#line 228 "trigrams_de.gperf"
      {"erk", 0.0010042472},
      {"",0.0},
#line 226 "trigrams_de.gperf"
      {"ran", 0.0010121095},
#line 76 "trigrams_de.gperf"
      {"rde", 0.0027249581},
#line 189 "trigrams_de.gperf"
      {"tet", 0.001163614},
      {"",0.0},
#line 95 "trigrams_de.gperf"
      {"bei", 0.00225497},
      {"",0.0}, {"",0.0},
#line 42 "trigrams_de.gperf"
      {"ach", 0.004566848},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 75 "trigrams_de.gperf"
      {"uch", 0.0027736293},
#line 195 "trigrams_de.gperf"
      {"eri", 0.0011472654},
#line 156 "trigrams_de.gperf"
      {"ech", 0.0013826339},
      {"",0.0},
#line 198 "trigrams_de.gperf"
      {"nig", 0.0011381552},
      {"",0.0}, {"",0.0},
#line 32 "trigrams_de.gperf"
      {"nde", 0.0065603666},
      {"",0.0}, {"",0.0},
#line 121 "trigrams_de.gperf"
      {"ing", 0.0017930936},
#line 255 "trigrams_de.gperf"
      {"sag", 8.8818534e-4},
#line 45 "trigrams_de.gperf"
      {"her", 0.0039282576},
#line 240 "trigrams_de.gperf"
      {"ric", 9.7117585e-4},
#line 218 "trigrams_de.gperf"
      {"rec", 0.0010414369},
#line 160 "trigrams_de.gperf"
      {"hie", 0.0013244782},
#line 84 "trigrams_de.gperf"
      {"ihr", 0.0024507777},
#line 49 "trigrams_de.gperf"
      {"sei", 0.0036336104},
#line 244 "trigrams_de.gperf"
      {"tun", 9.629392e-4},
#line 180 "trigrams_de.gperf"
      {"rsc", 0.0012235169},
#line 223 "trigrams_de.gperf"
      {"nnt", 0.0010187238},
#line 230 "trigrams_de.gperf"
      {"chr", 0.0010015017},
#line 124 "trigrams_de.gperf"
      {"chl", 0.0017380578},
      {"",0.0},
#line 88 "trigrams_de.gperf"
      {"och", 0.0024212005},
#line 26 "trigrams_de.gperf"
      {"che", 0.009706142},
      {"",0.0}, {"",0.0},
#line 158 "trigrams_de.gperf"
      {"lan", 0.0013631654},
#line 227 "trigrams_de.gperf"
      {"elb", 0.0010104871},
#line 204 "trigrams_de.gperf"
      {"noc", 0.0010994679},
#line 64 "trigrams_de.gperf"
      {"nic", 0.0031757276},
#line 63 "trigrams_de.gperf"
      {"hre", 0.0032172853},
      {"",0.0},
#line 23 "trigrams_de.gperf"
      {"sch", 0.012235543},
#line 234 "trigrams_de.gperf"
      {"chs", 9.923915e-4},
#line 250 "trigrams_de.gperf"
      {"aft", 9.3099097e-4},
#line 258 "trigrams_de.gperf"
      {"nsc", 8.863134e-4},
#line 168 "trigrams_de.gperf"
      {"tra", 0.0012985202},
      {"",0.0},
#line 197 "trigrams_de.gperf"
      {"hin", 0.0011402768},
#line 33 "trigrams_de.gperf"
      {"hen", 0.0061622616},
#line 127 "trigrams_de.gperf"
      {"tig", 0.0017158438},
#line 133 "trigrams_de.gperf"
      {"mei", 0.0016568145},
#line 236 "trigrams_de.gperf"
      {"tan", 9.850283e-4},
#line 221 "trigrams_de.gperf"
      {"ihn", 0.0010248389},
#line 266 "trigrams_de.gperf"
      {"wor", 8.589827e-4},
#line 97 "trigrams_de.gperf"
      {"wir", 0.002237124},
#line 126 "trigrams_de.gperf"
      {"wer", 0.0017243301},
#line 202 "trigrams_de.gperf"
      {"wel", 0.0011174388},
#line 20 "trigrams_de.gperf"
      {"ich", 0.017852314},
#line 91 "trigrams_de.gperf"
      {"wie", 0.0023601744},
#line 137 "trigrams_de.gperf"
      {"age", 0.0015766944},
#line 36 "trigrams_de.gperf"
      {"lic", 0.005201444},
#line 50 "trigrams_de.gperf"
      {"auf", 0.0036263722},
#line 55 "trigrams_de.gperf"
      {"lle", 0.0034706246},
#line 262 "trigrams_de.gperf"
      {"gef", 8.692161e-4},
#line 275 "trigrams_de.gperf"
      {"uge", 8.222922e-4},
#line 253 "trigrams_de.gperf"
      {"elc", 9.0565707e-4},
#line 85 "trigrams_de.gperf"
      {"ege", 0.0024414177},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 274 "trigrams_de.gperf"
      {"mac", 8.2840724e-4},
#line 82 "trigrams_de.gperf"
      {"hte", 0.0025083094},
#line 190 "trigrams_de.gperf"
      {"sti", 0.0011622411},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 203 "trigrams_de.gperf"
      {"chw", 0.0011092022},
      {"",0.0}, {"",0.0},
#line 261 "trigrams_de.gperf"
      {"tsc", 8.703393e-4},
      {"",0.0},
#line 136 "trigrams_de.gperf"
      {"wen", 0.0015829343},
#line 65 "trigrams_de.gperf"
      {"rei", 0.0031602527},
      {"",0.0}, {"",0.0},
#line 27 "trigrams_de.gperf"
      {"cht", 0.009239274},
      {"",0.0}, {"",0.0},
#line 164 "trigrams_de.gperf"
      {"hne", 0.0013197359},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 139 "trigrams_de.gperf"
      {"rch", 0.0015549796},
      {"",0.0}, {"",0.0},
#line 219 "trigrams_de.gperf"
      {"hal", 0.0010411873},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 220 "trigrams_de.gperf"
      {"zei", 0.0010350723},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 57 "trigrams_de.gperf"
      {"ige", 0.0033949972},
#line 267 "trigrams_de.gperf"
      {"agt", 8.527428e-4},
      {"",0.0}, {"",0.0},
#line 245 "trigrams_de.gperf"
      {"llt", 9.5445296e-4},
      {"",0.0},
#line 171 "trigrams_de.gperf"
      {"cha", 0.00129253},
      {"",0.0},
#line 185 "trigrams_de.gperf"
      {"hau", 0.0011757193},
      {"",0.0},
#line 120 "trigrams_de.gperf"
      {"nac", 0.0018373968},
#line 74 "trigrams_de.gperf"
      {"lei", 0.0027758756},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 232 "trigrams_de.gperf"
      {"tzt", 9.98257e-4},
#line 249 "trigrams_de.gperf"
      {"bli", 9.3872845e-4},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 77 "trigrams_de.gperf"
      {"war", 0.0027051154},
#line 184 "trigrams_de.gperf"
      {"tei", 0.0011935654},
#line 162 "trigrams_de.gperf"
      {"lch", 0.001322731},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0},
#line 199 "trigrams_de.gperf"
      {"was", 0.0011302929},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 175 "trigrams_de.gperf"
      {"rge", 0.001253094},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 108 "trigrams_de.gperf"
      {"hat", 0.0020017554},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0},
#line 44 "trigrams_de.gperf"
      {"nge", 0.0041857157},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0},
#line 174 "trigrams_de.gperf"
      {"hab", 0.0012549659},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0},
#line 257 "trigrams_de.gperf"
      {"rli", 8.86563e-4},
#line 107 "trigrams_de.gperf"
      {"hei", 0.0020254669},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 207 "trigrams_de.gperf"
      {"chi", 0.0010824954},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0},
#line 269 "trigrams_de.gperf"
      {"ick", 8.491237e-4},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 86 "trigrams_de.gperf"
      {"wei", 0.0024334306},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 243 "trigrams_de.gperf"
      {"tli", 9.64312e-4},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0},
#line 252 "trigrams_de.gperf"
      {"haf", 9.080282e-4}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash_de (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].trigram;

          if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
            return &wordlist[key];
        }
    }
  return 0;
}
#line 276 "trigrams_de.gperf"


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

#endif /* _TRIGRAM_DE_H_ */
