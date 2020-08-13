/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: /usr/local/bin/gperf -r -m 20 trigrams_es.gperf  */
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

#line 9 "trigrams_es.gperf"

#ifndef _TRIGRAM_ES_H_
#define _TRIGRAM_ES_H_
#include <_include/_string.h>
#line 14 "trigrams_es.gperf"
struct trigram_es
{
    const char  *trigram;
    const float  freq;
};

#define TOTAL_KEYWORDS 256
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 4
#define MIN_HASH_VALUE 6
#define MAX_HASH_VALUE 496
/* maximum key range = 491, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash_es (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      497, 497, 497, 497, 497, 497, 497, 497, 497, 497,
      497, 497, 497, 497, 497, 497, 497, 497, 497, 497,
      497, 497, 497, 497, 497, 497, 497, 497, 497, 497,
      497, 497, 497, 497, 497, 497, 497, 497, 497, 497,
      497, 497, 497, 497, 497, 497, 497, 497, 497, 497,
      497, 497, 497, 497, 497, 497, 497, 497, 497, 497,
      497, 497, 497, 497, 497, 497, 497, 497, 497, 497,
      497, 497, 497, 497, 497, 497, 497, 497, 497, 497,
      497, 497, 497, 497, 497, 497, 497, 497, 497, 497,
      497, 497, 497, 497, 497, 497, 497,  18, 144, 213,
       65,   4, 497,   1,   1,   2, 126, 226,   0, 124,
       60,  95,  84,  43,  44,   7,  42,  26, 165, 245,
      102,  54, 108, 131,  13, 172,  71,  94,   5,   6,
       23, 205, 207, 177, 497, 497, 497, 497, 497, 497,
      497, 497, 497, 497, 497, 497, 497, 497, 497, 497,
      497, 497, 497, 497, 497, 497, 497, 497, 497, 497,
      497,   7, 497, 497, 497, 497, 497, 497, 497, 497,
      497, 497, 497,   0, 497, 497, 497, 497, 497,   0,
      497, 497, 497, 497, 497, 497, 497, 497, 497, 497,
      497, 497, 497, 497, 497,   2, 497, 497, 497, 497,
      497, 497, 497, 497, 497, 497, 497, 497, 497, 497,
      497,   0, 497, 497, 497, 497, 497, 497, 497, 497,
      497, 497, 497, 497, 497, 497, 497, 497, 497, 497,
      497, 497, 497, 497, 497, 497, 497, 497, 497, 497,
      497, 497, 497, 497, 497, 497, 497, 497, 497, 497,
      497, 497, 497, 497, 497, 497, 497, 497, 497, 497,
      497, 497, 497, 497, 497, 497, 497, 497, 497, 497,
      497, 497
    };
  return len + asso_values[(unsigned char)str[2]] + asso_values[(unsigned char)str[1]+16] + asso_values[(unsigned char)str[0]+7];
}

#ifdef __GNUC__
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#else
__inline
#endif
#endif
const struct trigram_es *
get_trigram_es (register const char *str, register unsigned int len)
{
  static const struct trigram_es wordlist[] =
    {
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0},
#line 189 "trigrams_es.gperf"
      {"b\303\255a", 0.0012647568},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 223 "trigrams_es.gperf"
      {"ech", 0.0010920534},
#line 113 "trigrams_es.gperf"
      {"eci", 0.0017994639},
#line 45 "trigrams_es.gperf"
      {"aci", 0.00354561},
#line 176 "trigrams_es.gperf"
      {"ece", 0.0013167675},
#line 191 "trigrams_es.gperf"
      {"ace", 0.0012407366},
#line 83 "trigrams_es.gperf"
      {"ell", 0.0021780154},
#line 146 "trigrams_es.gperf"
      {"all", 0.0014993601},
#line 261 "trigrams_es.gperf"
      {"alg", 9.5325935e-4},
#line 178 "trigrams_es.gperf"
      {"ali", 0.0013100596},
      {"",0.0},
#line 117 "trigrams_es.gperf"
      {"ale", 0.001743236},
#line 163 "trigrams_es.gperf"
      {"ble", 0.0013760041},
      {"",0.0},
#line 252 "trigrams_es.gperf"
      {"uch", 9.943699e-4},
      {"",0.0}, {"",0.0},
#line 128 "trigrams_es.gperf"
      {"lle", 0.0016360332},
#line 161 "trigrams_es.gperf"
      {"eri", 0.0013853753},
#line 123 "trigrams_es.gperf"
      {"ari", 0.0016684629},
#line 147 "trigrams_es.gperf"
      {"ere", 0.0014812832},
#line 166 "trigrams_es.gperf"
      {"are", 0.0013598755},
#line 64 "trigrams_es.gperf"
      {"bre", 0.0025876155},
#line 239 "trigrams_es.gperf"
      {"ers", 0.0010195737},
#line 236 "trigrams_es.gperf"
      {"ela", 0.001035283},
#line 242 "trigrams_es.gperf"
      {"ala", 0.0010145428},
#line 159 "trigrams_es.gperf"
      {"aqu", 0.0013943027},
      {"",0.0},
#line 55 "trigrams_es.gperf"
      {"nci", 0.0030419761},
      {"",0.0},
#line 273 "trigrams_es.gperf"
      {"nce", 9.1811694e-4},
#line 57 "trigrams_es.gperf"
      {"lla", 0.002905081},
#line 118 "trigrams_es.gperf"
      {"uel", 0.0017399808},
#line 75 "trigrams_es.gperf"
      {"les", 0.0022993246},
#line 35 "trigrams_es.gperf"
      {"era", 0.0043595573},
#line 47 "trigrams_es.gperf"
      {"ara", 0.0034392702},
#line 172 "trigrams_es.gperf"
      {"bra", 0.0013353868},
#line 206 "trigrams_es.gperf"
      {"edi", 0.0011678376},
#line 197 "trigrams_es.gperf"
      {"abl", 0.0012006372},
#line 103 "trigrams_es.gperf"
      {"ues", 0.0018611172},
#line 226 "trigrams_es.gperf"
      {"ade", 0.0010766893},
#line 215 "trigrams_es.gperf"
      {"ab\303\255", 0.0011315362},
#line 263 "trigrams_es.gperf"
      {"abe", 9.427783e-4},
#line 116 "trigrams_es.gperf"
      {"m\303\241s", 0.0017452089},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 94 "trigrams_es.gperf"
      {"ura", 0.0019340902},
#line 179 "trigrams_es.gperf"
      {"r\303\255a", 0.001306385},
      {"",0.0},
#line 28 "trigrams_es.gperf"
      {"las", 0.005093972},
      {"",0.0},
#line 91 "trigrams_es.gperf"
      {"nes", 0.0020292583},
#line 209 "trigrams_es.gperf"
      {"bie", 0.001151216},
#line 53 "trigrams_es.gperf"
      {"ada", 0.0031488338},
      {"",0.0},
#line 34 "trigrams_es.gperf"
      {"aba", 0.0044478695},
      {"",0.0},
#line 100 "trigrams_es.gperf"
      {"ert", 0.0018977886},
#line 132 "trigrams_es.gperf"
      {"art", 0.0016009401},
#line 155 "trigrams_es.gperf"
      {"err", 0.0014047098},
#line 246 "trigrams_es.gperf"
      {"arr", 0.0010029273},
      {"",0.0},
#line 171 "trigrams_es.gperf"
      {"ndi", 0.0013378037},
#line 234 "trigrams_es.gperf"
      {"uie", 0.001044383},
#line 106 "trigrams_es.gperf"
      {"nde", 0.001849403},
      {"",0.0}, {"",0.0},
#line 111 "trigrams_es.gperf"
      {"ene", 0.0018077501},
#line 231 "trigrams_es.gperf"
      {"nas", 0.0010565164},
      {"",0.0},
#line 193 "trigrams_es.gperf"
      {"ens", 0.0012276168},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 76 "trigrams_es.gperf"
      {"uer", 0.002295428},
      {"",0.0},
#line 52 "trigrams_es.gperf"
      {"i\303\263n", 0.0031523355},
#line 213 "trigrams_es.gperf"
      {"nda", 0.0011406116},
#line 32 "trigrams_es.gperf"
      {"res", 0.00451993},
#line 255 "trigrams_es.gperf"
      {"erd", 9.6988113e-4},
#line 216 "trigrams_es.gperf"
      {"ena", 0.0011287248},
#line 192 "trigrams_es.gperf"
      {"ana", 0.0012374073},
      {"",0.0},
#line 219 "trigrams_es.gperf"
      {"mas", 0.0011160489},
#line 114 "trigrams_es.gperf"
      {"ici", 0.0017821763},
#line 170 "trigrams_es.gperf"
      {"lar", 0.0013405657},
#line 199 "trigrams_es.gperf"
      {"rra", 0.0011882819},
#line 214 "trigrams_es.gperf"
      {"ner", 0.0011405129},
#line 187 "trigrams_es.gperf"
      {"ill", 0.0012751145},
      {"",0.0},
#line 210 "trigrams_es.gperf"
      {"uen", 0.0011499829},
#line 188 "trigrams_es.gperf"
      {"mie", 0.0012711193},
#line 133 "trigrams_es.gperf"
      {"gra", 0.001594035},
#line 46 "trigrams_es.gperf"
      {"una", 0.0035373238},
#line 168 "trigrams_es.gperf"
      {"mis", 0.0013504795},
#line 82 "trigrams_es.gperf"
      {"ras", 0.0022332321},
#line 160 "trigrams_es.gperf"
      {"ban", 0.0013914173},
      {"",0.0}, {"",0.0},
#line 24 "trigrams_es.gperf"
      {"los", 0.0072975364},
#line 71 "trigrams_es.gperf"
      {"ica", 0.0024135066},
#line 180 "trigrams_es.gperf"
      {"lan", 0.0013022172},
      {"",0.0},
#line 235 "trigrams_es.gperf"
      {"mer", 0.0010353077},
#line 21 "trigrams_es.gperf"
      {"ent", 0.010167697},
#line 38 "trigrams_es.gperf"
      {"ant", 0.004177606},
#line 143 "trigrams_es.gperf"
      {"llo", 0.0015099398},
#line 202 "trigrams_es.gperf"
      {"uan", 0.0011815},
#line 220 "trigrams_es.gperf"
      {"spe", 0.0011092917},
#line 42 "trigrams_es.gperf"
      {"ero", 0.003728893},
#line 251 "trigrams_es.gperf"
      {"aro", 9.966387e-4},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0},
#line 169 "trigrams_es.gperf"
      {"ria", 0.0013449801},
#line 74 "trigrams_es.gperf"
      {"nos", 0.0023216184},
#line 43 "trigrams_es.gperf"
      {"men", 0.0036090636},
#line 142 "trigrams_es.gperf"
      {"mar", 0.0015223938},
#line 127 "trigrams_es.gperf"
      {"mbr", 0.001639905},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 211 "trigrams_es.gperf"
      {"nad", 0.0011486264},
      {"",0.0},
#line 65 "trigrams_es.gperf"
      {"end", 0.002585914},
#line 48 "trigrams_es.gperf"
      {"and", 0.0033936468},
#line 26 "trigrams_es.gperf"
      {"ado", 0.005644635},
      {"",0.0},
#line 165 "trigrams_es.gperf"
      {"ren", 0.001370332},
#line 203 "trigrams_es.gperf"
      {"rar", 0.0011778255},
#line 237 "trigrams_es.gperf"
      {"mpo", 0.0010316825},
#line 96 "trigrams_es.gperf"
      {"mos", 0.0019097247},
#line 49 "trigrams_es.gperf"
      {"ida", 0.0033610198},
#line 85 "trigrams_es.gperf"
      {"man", 0.0021555736},
#line 228 "trigrams_es.gperf"
      {"gen", 0.0010702034},
#line 266 "trigrams_es.gperf"
      {"erm", 9.350593e-4},
      {"",0.0},
#line 240 "trigrams_es.gperf"
      {"und", 0.0010167377},
      {"",0.0},
#line 225 "trigrams_es.gperf"
      {"sal", 0.0010866033},
      {"",0.0}, {"",0.0},
#line 56 "trigrams_es.gperf"
      {"ier", 0.0029868085},
#line 81 "trigrams_es.gperf"
      {"ros", 0.002242332},
#line 232 "trigrams_es.gperf"
      {"min", 0.0010488221},
#line 67 "trigrams_es.gperf"
      {"ran", 0.002558614},
#line 139 "trigrams_es.gperf"
      {"tre", 0.0015526286},
#line 164 "trigrams_es.gperf"
      {"ces", 0.001374401},
      {"",0.0}, {"",0.0},
#line 125 "trigrams_es.gperf"
      {"rad", 0.0016531235},
#line 30 "trigrams_es.gperf"
      {"ndo", 0.004791599},
#line 105 "trigrams_es.gperf"
      {"tes", 0.0018535461},
      {"",0.0},
#line 271 "trigrams_es.gperf"
      {"eno", 9.231479e-4},
#line 157 "trigrams_es.gperf"
      {"ano", 0.0013968182},
      {"",0.0}, {"",0.0},
#line 27 "trigrams_es.gperf"
      {"ien", 0.005228253},
      {"",0.0},
#line 36 "trigrams_es.gperf"
      {"tra", 0.0043057217},
#line 107 "trigrams_es.gperf"
      {"ina", 0.0018449147},
#line 205 "trigrams_es.gperf"
      {"tal", 0.0011689968},
#line 218 "trigrams_es.gperf"
      {"ami", 0.0011175532},
#line 104 "trigrams_es.gperf"
      {"cas", 0.001855963},
#line 141 "trigrams_es.gperf"
      {"ame", 0.0015246132},
#line 122 "trigrams_es.gperf"
      {"ser", 0.0016788453},
      {"",0.0},
#line 275 "trigrams_es.gperf"
      {"mor", 9.178457e-4},
#line 148 "trigrams_es.gperf"
      {"tas", 0.001470161},
      {"",0.0},
#line 61 "trigrams_es.gperf"
      {"ci\303\263", 0.0026283315},
#line 129 "trigrams_es.gperf"
      {"cie", 0.0016263906},
#line 95 "trigrams_es.gperf"
      {"ios", 0.0019246201},
#line 167 "trigrams_es.gperf"
      {"ico", 0.0013556831},
      {"",0.0}, {"",0.0},
#line 124 "trigrams_es.gperf"
      {"tie", 0.0016668598},
#line 222 "trigrams_es.gperf"
      {"ori", 0.0010933111},
#line 217 "trigrams_es.gperf"
      {"ama", 0.0011232992},
#line 262 "trigrams_es.gperf"
      {"ore", 9.4566366e-4},
      {"",0.0},
#line 134 "trigrams_es.gperf"
      {"sen", 0.0015902125},
      {"",0.0},
#line 130 "trigrams_es.gperf"
      {"cer", 0.0016181044},
#line 162 "trigrams_es.gperf"
      {"int", 0.0013774344},
#line 54 "trigrams_es.gperf"
      {"cia", 0.0031221993},
      {"",0.0}, {"",0.0},
#line 86 "trigrams_es.gperf"
      {"ter", 0.0021505426},
#line 257 "trigrams_es.gperf"
      {"sol", 9.638884e-4},
#line 144 "trigrams_es.gperf"
      {"rio", 0.0015091259},
      {"",0.0}, {"",0.0},
#line 108 "trigrams_es.gperf"
      {"ora", 0.0018424239},
#line 272 "trigrams_es.gperf"
      {"esi", 9.2228473e-4},
#line 98 "trigrams_es.gperf"
      {"ron", 0.0019051377},
#line 110 "trigrams_es.gperf"
      {"ese", 0.0018215111},
      {"",0.0}, {"",0.0},
#line 270 "trigrams_es.gperf"
      {"cen", 9.2526875e-4},
#line 126 "trigrams_es.gperf"
      {"car", 0.0016442208},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 70 "trigrams_es.gperf"
      {"ten", 0.0024265524},
#line 115 "trigrams_es.gperf"
      {"tar", 0.0017530019},
      {"",0.0}, {"",0.0},
#line 62 "trigrams_es.gperf"
      {"ido", 0.0026233993},
#line 140 "trigrams_es.gperf"
      {"sin", 0.0015294715},
#line 136 "trigrams_es.gperf"
      {"esa", 0.0015738866},
#line 200 "trigrams_es.gperf"
      {"asa", 0.0011873448},
      {"",0.0}, {"",0.0},
#line 156 "trigrams_es.gperf"
      {"cos", 0.0014000242},
      {"",0.0},
#line 244 "trigrams_es.gperf"
      {"can", 0.001004629},
      {"",0.0}, {"",0.0},
#line 90 "trigrams_es.gperf"
      {"tos", 0.002041145},
      {"",0.0},
#line 77 "trigrams_es.gperf"
      {"tan", 0.0022949101},
#line 150 "trigrams_es.gperf"
      {"nti", 0.0014582003},
      {"",0.0},
#line 23 "trigrams_es.gperf"
      {"nte", 0.007332531},
#line 88 "trigrams_es.gperf"
      {"ion", 0.0020894564},
#line 151 "trigrams_es.gperf"
      {"tad", 0.0014507279},
#line 102 "trigrams_es.gperf"
      {"one", 0.0018758153},
#line 268 "trigrams_es.gperf"
      {"rab", 9.3158206e-4},
#line 99 "trigrams_es.gperf"
      {"ver", 0.0019036334},
#line 260 "trigrams_es.gperf"
      {"ons", 9.5390057e-4},
#line 254 "trigrams_es.gperf"
      {"rma", 9.7375293e-4},
      {"",0.0},
#line 153 "trigrams_es.gperf"
      {"cha", 0.0014220468},
#line 25 "trigrams_es.gperf"
      {"est", 0.007247795},
#line 173 "trigrams_es.gperf"
      {"ast", 0.0013286542},
#line 78 "trigrams_es.gperf"
      {"tro", 0.0022807051},
#line 177 "trigrams_es.gperf"
      {"ino", 0.0013153125},
#line 58 "trigrams_es.gperf"
      {"nta", 0.0028627869},
#line 37 "trigrams_es.gperf"
      {"del", 0.004291788},
#line 158 "trigrams_es.gperf"
      {"obr", 0.0013964976},
#line 208 "trigrams_es.gperf"
      {"ona", 0.0011631767},
#line 250 "trigrams_es.gperf"
      {"gua", 0.0010005598},
#line 138 "trigrams_es.gperf"
      {"emp", 0.0015559085},
      {"",0.0},
#line 174 "trigrams_es.gperf"
      {"ven", 0.0013230807},
#line 39 "trigrams_es.gperf"
      {"des", 0.00392791},
#line 184 "trigrams_es.gperf"
      {"ust", 0.0012924267},
      {"",0.0},
#line 238 "trigrams_es.gperf"
      {"son", 0.0010307453},
#line 207 "trigrams_es.gperf"
      {"rse", 0.0011651003},
#line 186 "trigrams_es.gperf"
      {"cor", 0.0012765449},
#line 152 "trigrams_es.gperf"
      {"rte", 0.0014285573},
      {"",0.0},
#line 269 "trigrams_es.gperf"
      {"emo", 9.2674844e-4},
#line 221 "trigrams_es.gperf"
      {"amo", 0.0010990326},
#line 190 "trigrams_es.gperf"
      {"tor", 0.0012530426},
#line 256 "trigrams_es.gperf"
      {"pri", 9.654914e-4},
#line 274 "trigrams_es.gperf"
      {"egu", 9.1801834e-4},
#line 59 "trigrams_es.gperf"
      {"pre", 0.0026596515},
      {"",0.0},
#line 259 "trigrams_es.gperf"
      {"ima", 9.557008e-4},
#line 68 "trigrams_es.gperf"
      {"cio", 0.0025318563},
#line 137 "trigrams_es.gperf"
      {"das", 0.0015561552},
#line 72 "trigrams_es.gperf"
      {"ntr", 0.0023559222},
#line 93 "trigrams_es.gperf"
      {"ont", 0.0019592447},
#line 195 "trigrams_es.gperf"
      {"rta", 0.0012165685},
#line 22 "trigrams_es.gperf"
      {"con", 0.008469986},
#line 87 "trigrams_es.gperf"
      {"qui", 0.0021192967},
#line 80 "trigrams_es.gperf"
      {"hab", 0.0022462779},
#line 20 "trigrams_es.gperf"
      {"que", 0.015559529},
#line 66 "trigrams_es.gperf"
      {"enc", 0.0025838178},
#line 201 "trigrams_es.gperf"
      {"anc", 0.0011853719},
      {"",0.0},
#line 258 "trigrams_es.gperf"
      {"ijo", 9.590548e-4},
      {"",0.0},
#line 60 "trigrams_es.gperf"
      {"esp", 0.0026381714},
#line 84 "trigrams_es.gperf"
      {"tod", 0.0021752287},
#line 243 "trigrams_es.gperf"
      {"vid", 0.0010106956},
#line 109 "trigrams_es.gperf"
      {"sus", 0.0018227441},
#line 63 "trigrams_es.gperf"
      {"rec", 0.0026171599},
      {"",0.0},
#line 230 "trigrams_es.gperf"
      {"pas", 0.0010577001},
#line 149 "trigrams_es.gperf"
      {"der", 0.0014692486},
#line 253 "trigrams_es.gperf"
      {"gun", 9.821871e-4},
#line 248 "trigrams_es.gperf"
      {"tam", 0.0010023601},
      {"",0.0}, {"",0.0},
#line 175 "trigrams_es.gperf"
      {"ond", 0.0013198009},
#line 120 "trigrams_es.gperf"
      {"odo", 0.0017128285},
      {"",0.0}, {"",0.0},
#line 181 "trigrams_es.gperf"
      {"fue", 0.0013020447},
      {"",0.0},
#line 249 "trigrams_es.gperf"
      {"cue", 0.0010014477},
      {"",0.0},
#line 119 "trigrams_es.gperf"
      {"ita", 0.0017184761},
#line 267 "trigrams_es.gperf"
      {"rac", 9.3318505e-4},
      {"",0.0},
#line 229 "trigrams_es.gperf"
      {"den", 0.0010624845},
#line 247 "trigrams_es.gperf"
      {"cab", 0.001002878},
      {"",0.0},
#line 44 "trigrams_es.gperf"
      {"per", 0.0035581873},
      {"",0.0},
#line 131 "trigrams_es.gperf"
      {"sti", 0.0016151697},
#line 182 "trigrams_es.gperf"
      {"tab", 0.0012987647},
#line 69 "trigrams_es.gperf"
      {"ste", 0.0024899812},
      {"",0.0},
#line 112 "trigrams_es.gperf"
      {"cua", 0.0018070842},
      {"",0.0},
#line 135 "trigrams_es.gperf"
      {"cho", 0.0015891274},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 50 "trigrams_es.gperf"
      {"dos", 0.0033388985},
#line 41 "trigrams_es.gperf"
      {"nto", 0.0037889434},
      {"",0.0}, {"",0.0},
#line 73 "trigrams_es.gperf"
      {"ist", 0.0023348616},
#line 33 "trigrams_es.gperf"
      {"par", 0.0045009903},
#line 31 "trigrams_es.gperf"
      {"sta", 0.0046742605},
#line 51 "trigrams_es.gperf"
      {"dad", 0.0031790438},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 40 "trigrams_es.gperf"
      {"com", 0.0038528654},
      {"",0.0},
#line 265 "trigrams_es.gperf"
      {"sob", 9.3737745e-4},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 185 "trigrams_es.gperf"
      {"hac", 0.0012905279},
#line 227 "trigrams_es.gperf"
      {"tur", 0.0010730642},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0},
#line 92 "trigrams_es.gperf"
      {"str", 0.002026274},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 89 "trigrams_es.gperf"
      {"pro", 0.0020507136},
#line 204 "trigrams_es.gperf"
      {"dor", 0.0011704271},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 245 "trigrams_es.gperf"
      {"inc", 0.0010038151},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0},
#line 194 "trigrams_es.gperf"
      {"dio", 0.0012181961},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 224 "trigrams_es.gperf"
      {"don", 0.0010890447},
      {"",0.0}, {"",0.0},
#line 29 "trigrams_es.gperf"
      {"por", 0.0049898764},
      {"",0.0}, {"",0.0},
#line 241 "trigrams_es.gperf"
      {"ito", 0.0010160471},
      {"",0.0},
#line 145 "trigrams_es.gperf"
      {"osa", 0.0015051801},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 198 "trigrams_es.gperf"
      {"tic", 0.001195779},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0},
#line 97 "trigrams_es.gperf"
      {"sto", 0.0019094041},
      {"",0.0}, {"",0.0},
#line 233 "trigrams_es.gperf"
      {"ism", 0.0010464792},
      {"",0.0}, {"",0.0},
#line 154 "trigrams_es.gperf"
      {"otr", 0.0014154129},
      {"",0.0},
#line 121 "trigrams_es.gperf"
      {"esc", 0.0016885619},
      {"",0.0}, {"",0.0},
#line 264 "trigrams_es.gperf"
      {"omp", 9.4038615e-4},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 101 "trigrams_es.gperf"
      {"pue", 0.0018842248},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 79 "trigrams_es.gperf"
      {"omo", 0.0022751563},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0},
#line 212 "trigrams_es.gperf"
      {"oso", 0.0011431763},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 183 "trigrams_es.gperf"
      {"omb", 0.001297507},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0},
#line 196 "trigrams_es.gperf"
      {"dic", 0.0012127706}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash_es (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].trigram;

          if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
            return &wordlist[key];
        }
    }
  return 0;
}
#line 276 "trigrams_es.gperf"


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

#endif /* _TRIGRAM_ES_H_ */
