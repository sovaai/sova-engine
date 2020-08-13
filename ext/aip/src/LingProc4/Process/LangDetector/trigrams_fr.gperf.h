/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: /usr/local/bin/gperf -r -m 20 trigrams_fr.gperf  */
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

#line 9 "trigrams_fr.gperf"

#ifndef _TRIGRAM_FR_H_
#define _TRIGRAM_FR_H_
#include <_include/_string.h>
#line 14 "trigrams_fr.gperf"
struct trigram_fr
{
    const char  *trigram;
    const float  freq;
};

#define TOTAL_KEYWORDS 256
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 4
#define MIN_HASH_VALUE 6
#define MAX_HASH_VALUE 553
/* maximum key range = 548, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash_fr (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      554, 554, 554, 554, 554, 554, 554, 554, 554, 554,
      554, 554, 554, 554, 554, 554, 554, 554, 554, 554,
      554, 554, 554, 554, 554, 554, 554, 554, 554, 554,
      554, 554, 554, 554, 554, 554, 554, 554, 554, 554,
      554, 554, 554, 554, 554, 554, 554, 554, 554, 554,
      554, 554, 554, 554, 554, 554, 554, 554, 554, 554,
      554, 554, 554, 554, 554, 554, 554, 554, 554, 554,
      554, 554, 554, 554, 554, 554, 554, 554, 554, 554,
      554, 554, 554, 554, 554, 554, 554, 554, 554, 554,
      554, 554, 554, 554, 554, 554, 554, 554, 202,   2,
       46,  38,  10,  81, 168,  13,  87,   2, 554,  52,
      298, 177,  60,   2,  59,   7,  22,   5,   1,   0,
      144,   3,  43, 554, 289, 103,  42, 164, 215,  43,
      150,  76, 228, 179,  28, 554, 554, 554, 554, 554,
      554, 554, 554, 554, 554, 554, 554, 554, 554, 554,
      554, 554, 554, 554, 554, 554, 554, 554, 554, 554,
      554, 554, 554, 554, 554, 554, 554, 554, 554,   0,
       39,   7,   4,  10,   0, 554, 554, 554, 554, 554,
      554, 554, 554, 554, 554, 554, 554, 554, 554, 554,
      554, 554, 554, 554, 554, 554,   3, 554, 554,  11,
      554, 554, 554, 554, 554, 554, 554, 554, 554, 554,
      554,  39, 554, 554, 554, 554, 554, 554, 554, 554,
      554, 554, 554, 554, 554, 554, 554, 554, 554, 554,
      554, 554, 554, 554, 554, 554, 554, 554, 554, 554,
      554, 554, 554, 554, 554, 554, 554, 554, 554, 554,
      554, 554, 554, 554, 554, 554, 554, 554, 554, 554,
      554, 554, 554, 554, 554, 554, 554, 554, 554, 554,
      554, 554
    };
  return len + asso_values[(unsigned char)str[2]+4] + asso_values[(unsigned char)str[1]+1] + asso_values[(unsigned char)str[0]+16];
}

#ifdef __GNUC__
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#else
__inline
#endif
#endif
const struct trigram_fr *
get_trigram_fr (register const char *str, register unsigned int len)
{
  static const struct trigram_fr wordlist[] =
    {
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0},
#line 256 "trigrams_fr.gperf"
      {"aus", 9.0676476e-4},
      {"",0.0}, {"",0.0},
#line 249 "trigrams_fr.gperf"
      {"eus", 9.2154584e-4},
#line 29 "trigrams_fr.gperf"
      {"eur", 0.005823483},
      {"",0.0},
#line 244 "trigrams_fr.gperf"
      {"avo", 9.2960824e-4},
#line 224 "trigrams_fr.gperf"
      {"arr", 0.0010085178},
#line 207 "trigrams_fr.gperf"
      {"i\303\250r", 0.0011411303},
#line 125 "trigrams_fr.gperf"
      {"ers", 0.0019327974},
#line 178 "trigrams_fr.gperf"
      {"err", 0.0012950572},
#line 100 "trigrams_fr.gperf"
      {"ier", 0.0022343728},
#line 79 "trigrams_fr.gperf"
      {"ave", 0.0026126103},
#line 101 "trigrams_fr.gperf"
      {"ieu", 0.0022109733},
#line 120 "trigrams_fr.gperf"
      {"ces", 0.001988632},
      {"",0.0}, {"",0.0},
#line 194 "trigrams_fr.gperf"
      {"it\303\251", 0.0012250905},
#line 134 "trigrams_fr.gperf"
      {"ite", 0.0017838052},
      {"",0.0},
#line 49 "trigrams_fr.gperf"
      {"ire", 0.0038309125},
#line 106 "trigrams_fr.gperf"
      {"ass", 0.0021458718},
#line 109 "trigrams_fr.gperf"
      {"iss", 0.0021209433},
#line 162 "trigrams_fr.gperf"
      {"app", 0.0014598034},
#line 107 "trigrams_fr.gperf"
      {"ess", 0.002135122},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 34 "trigrams_fr.gperf"
      {"des", 0.005099188},
#line 63 "trigrams_fr.gperf"
      {"che", 0.0032332758},
#line 236 "trigrams_fr.gperf"
      {"dem", 9.6058357e-4},
#line 232 "trigrams_fr.gperf"
      {"deu", 9.6593535e-4},
      {"",0.0}, {"",0.0},
#line 191 "trigrams_fr.gperf"
      {"ise", 0.0012329675},
#line 111 "trigrams_fr.gperf"
      {"ver", 0.002103405},
#line 66 "trigrams_fr.gperf"
      {"ava", 0.0030428828},
#line 69 "trigrams_fr.gperf"
      {"\303\250re", 0.0029425663},
#line 155 "trigrams_fr.gperf"
      {"dre", 0.0015347746},
#line 269 "trigrams_fr.gperf"
      {"eva", 8.647847e-4},
      {"",0.0},
#line 175 "trigrams_fr.gperf"
      {"gra", 0.0013059924},
#line 245 "trigrams_fr.gperf"
      {"fra", 9.293534e-4},
#line 187 "trigrams_fr.gperf"
      {"nts", 0.0012549538},
#line 132 "trigrams_fr.gperf"
      {"ntr", 0.0018084095},
#line 212 "trigrams_fr.gperf"
      {"\303\252me", 0.0010809635},
#line 188 "trigrams_fr.gperf"
      {"era", 0.0012533783},
      {"",0.0},
#line 219 "trigrams_fr.gperf"
      {"nes", 0.0010479958},
#line 248 "trigrams_fr.gperf"
      {"ner", 9.217312e-4},
      {"",0.0},
#line 171 "trigrams_fr.gperf"
      {"ils", 0.0013193602},
      {"",0.0},
#line 21 "trigrams_fr.gperf"
      {"que", 0.010847376},
#line 77 "trigrams_fr.gperf"
      {"cha", 0.002677179},
#line 94 "trigrams_fr.gperf"
      {"ati", 0.0023339943},
#line 64 "trigrams_fr.gperf"
      {"nte", 0.0032179388},
#line 267 "trigrams_fr.gperf"
      {"ari", 8.7518705e-4},
#line 199 "trigrams_fr.gperf"
      {"for", 0.0011880683},
#line 229 "trigrams_fr.gperf"
      {"isa", 9.853732e-4},
#line 167 "trigrams_fr.gperf"
      {"abl", 0.0013567067},
#line 153 "trigrams_fr.gperf"
      {"iqu", 0.0015469376},
#line 270 "trigrams_fr.gperf"
      {"cri", 8.635105e-4},
#line 222 "trigrams_fr.gperf"
      {"ale", 0.0010197079},
#line 196 "trigrams_fr.gperf"
      {"cor", 0.0012080852},
#line 53 "trigrams_fr.gperf"
      {"com", 0.0035344574},
#line 102 "trigrams_fr.gperf"
      {"cou", 0.0021874812},
      {"",0.0},
#line 30 "trigrams_fr.gperf"
      {"ien", 0.0056053116},
      {"",0.0}, {"",0.0},
#line 268 "trigrams_fr.gperf"
      {"dev", 8.744689e-4},
      {"",0.0},
#line 214 "trigrams_fr.gperf"
      {"cel", 0.0010716964},
#line 81 "trigrams_fr.gperf"
      {"sur", 0.002571001},
#line 226 "trigrams_fr.gperf"
      {"bre", 0.0010017528},
#line 227 "trigrams_fr.gperf"
      {"\303\251ri", 9.915358e-4},
#line 168 "trigrams_fr.gperf"
      {"ndr", 0.001351077},
#line 174 "trigrams_fr.gperf"
      {"qua", 0.0013135683},
      {"",0.0}, {"",0.0},
#line 261 "trigrams_fr.gperf"
      {"nta", 8.867246e-4},
#line 92 "trigrams_fr.gperf"
      {"ses", 0.0023530615},
#line 105 "trigrams_fr.gperf"
      {"ser", 0.0021562278},
#line 252 "trigrams_fr.gperf"
      {"sem", 9.1383094e-4},
#line 27 "trigrams_fr.gperf"
      {"ais", 0.0065034116},
#line 99 "trigrams_fr.gperf"
      {"air", 0.0022440106},
#line 62 "trigrams_fr.gperf"
      {"vou", 0.0032405274},
#line 264 "trigrams_fr.gperf"
      {"\303\251es", 8.797048e-4},
#line 159 "trigrams_fr.gperf"
      {"nde", 0.001491775},
#line 190 "trigrams_fr.gperf"
      {"ste", 0.001244783},
#line 239 "trigrams_fr.gperf"
      {"nco", 9.383888e-4},
#line 40 "trigrams_fr.gperf"
      {"qui", 0.004421738},
#line 140 "trigrams_fr.gperf"
      {"ven", 0.001737794},
      {"",0.0},
#line 181 "trigrams_fr.gperf"
      {"nti", 0.0012802993},
      {"",0.0},
#line 74 "trigrams_fr.gperf"
      {"nce", 0.002744435},
#line 98 "trigrams_fr.gperf"
      {"aie", 0.0022519804},
      {"",0.0}, {"",0.0},
#line 95 "trigrams_fr.gperf"
      {"nou", 0.0022633094},
#line 152 "trigrams_fr.gperf"
      {"jou", 0.0015905858},
#line 225 "trigrams_fr.gperf"
      {"m\303\252m", 0.0010080081},
      {"",0.0},
#line 189 "trigrams_fr.gperf"
      {"dis", 0.0012453854},
      {"",0.0},
#line 65 "trigrams_fr.gperf"
      {"sse", 0.0030555094},
      {"",0.0},
#line 119 "trigrams_fr.gperf"
      {"mes", 0.0019902072},
#line 204 "trigrams_fr.gperf"
      {"all", 0.0011672868},
#line 57 "trigrams_fr.gperf"
      {"ill", 0.0034266575},
#line 240 "trigrams_fr.gperf"
      {"nsi", 9.37717e-4},
#line 35 "trigrams_fr.gperf"
      {"ell", 0.004862992},
#line 170 "trigrams_fr.gperf"
      {"nda", 0.00133713},
#line 146 "trigrams_fr.gperf"
      {"vec", 0.0016549691},
      {"",0.0}, {"",0.0},
#line 38 "trigrams_fr.gperf"
      {"ion", 0.004658258},
      {"",0.0},
#line 89 "trigrams_fr.gperf"
      {"ble", 0.0023806312},
      {"",0.0},
#line 48 "trigrams_fr.gperf"
      {"con", 0.003912788},
      {"",0.0},
#line 186 "trigrams_fr.gperf"
      {"vie", 0.001259541},
#line 253 "trigrams_fr.gperf"
      {"sui", 9.1383094e-4},
      {"",0.0}, {"",0.0},
#line 223 "trigrams_fr.gperf"
      {"ndi", 0.0010126417},
#line 250 "trigrams_fr.gperf"
      {"sti", 9.161014e-4},
      {"",0.0}, {"",0.0},
#line 139 "trigrams_fr.gperf"
      {"ssa", 0.001755749},
      {"",0.0},
#line 211 "trigrams_fr.gperf"
      {"squ", 0.0011048727},
#line 149 "trigrams_fr.gperf"
      {"sou", 0.0016369445},
#line 108 "trigrams_fr.gperf"
      {"voi", 0.0021298395},
#line 129 "trigrams_fr.gperf"
      {"don", 0.0018536793},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 169 "trigrams_fr.gperf"
      {"sen", 0.0013493625},
      {"",0.0},
#line 103 "trigrams_fr.gperf"
      {"aut", 0.0021709392},
#line 78 "trigrams_fr.gperf"
      {"ain", 0.0026594326},
      {"",0.0},
#line 127 "trigrams_fr.gperf"
      {"ssi", 0.0018905161},
#line 230 "trigrams_fr.gperf"
      {"att", 9.810408e-4},
#line 201 "trigrams_fr.gperf"
      {"eil", 0.0011741213},
#line 150 "trigrams_fr.gperf"
      {"art", 0.0016062008},
#line 87 "trigrams_fr.gperf"
      {"ett", 0.0024024784},
#line 243 "trigrams_fr.gperf"
      {"her", 9.337089e-4},
#line 215 "trigrams_fr.gperf"
      {"ert", 0.001069704},
#line 271 "trigrams_fr.gperf"
      {"heu", 8.611705e-4},
#line 203 "trigrams_fr.gperf"
      {"r\303\250s", 0.001170785},
#line 161 "trigrams_fr.gperf"
      {"bie", 0.0014613557},
#line 41 "trigrams_fr.gperf"
      {"res", 0.0043682205},
#line 151 "trigrams_fr.gperf"
      {"cet", 0.0016056448},
#line 173 "trigrams_fr.gperf"
      {"rem", 0.0013142865},
#line 241 "trigrams_fr.gperf"
      {"reu", 9.3563186e-4},
#line 272 "trigrams_fr.gperf"
      {"r\303\251s", 8.6015114e-4},
#line 28 "trigrams_fr.gperf"
      {"ous", 0.005965988},
#line 26 "trigrams_fr.gperf"
      {"our", 0.007007173},
      {"",0.0},
#line 184 "trigrams_fr.gperf"
      {"rte", 0.001270731},
#line 176 "trigrams_fr.gperf"
      {"ist", 0.0013058765},
#line 246 "trigrams_fr.gperf"
      {"rre", 9.290522e-4},
#line 39 "trigrams_fr.gperf"
      {"est", 0.0046016127},
#line 33 "trigrams_fr.gperf"
      {"men", 0.005295813},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 154 "trigrams_fr.gperf"
      {"ard", 0.0015363035},
#line 273 "trigrams_fr.gperf"
      {"bon", 8.5491524e-4},
#line 37 "trigrams_fr.gperf"
      {"ans", 0.004666089},
#line 126 "trigrams_fr.gperf"
      {"ins", 0.001907058},
#line 198 "trigrams_fr.gperf"
      {"ang", 0.0011997449},
#line 130 "trigrams_fr.gperf"
      {"ens", 0.0018471692},
#line 143 "trigrams_fr.gperf"
      {"age", 0.0017127728},
#line 263 "trigrams_fr.gperf"
      {"ore", 8.826471e-4},
#line 220 "trigrams_fr.gperf"
      {"utr", 0.0010419721},
#line 88 "trigrams_fr.gperf"
      {"urs", 0.0023940452},
#line 251 "trigrams_fr.gperf"
      {"oup", 9.154759e-4},
      {"",0.0},
#line 172 "trigrams_fr.gperf"
      {"ues", 0.0013160009},
#line 247 "trigrams_fr.gperf"
      {"gne", 9.2900585e-4},
#line 255 "trigrams_fr.gperf"
      {"\303\252tr", 9.1074966e-4},
#line 131 "trigrams_fr.gperf"
      {"uve", 0.0018129504},
#line 123 "trigrams_fr.gperf"
      {"ine", 0.0019608769},
      {"",0.0},
#line 43 "trigrams_fr.gperf"
      {"son", 0.004177503},
      {"",0.0},
#line 133 "trigrams_fr.gperf"
      {"ute", 0.0018000227},
      {"",0.0},
#line 82 "trigrams_fr.gperf"
      {"ure", 0.0025327045},
      {"",0.0},
#line 195 "trigrams_fr.gperf"
      {"uss", 0.0012250905},
      {"",0.0},
#line 259 "trigrams_fr.gperf"
      {"gar", 9.0231653e-4},
      {"",0.0}, {"",0.0},
#line 257 "trigrams_fr.gperf"
      {"hom", 9.0616243e-4},
#line 210 "trigrams_fr.gperf"
      {"rti", 0.0011282953},
#line 76 "trigrams_fr.gperf"
      {"ouv", 0.0027279165},
#line 265 "trigrams_fr.gperf"
      {"rri", 8.7711e-4},
#line 144 "trigrams_fr.gperf"
      {"eau", 0.0017116839},
      {"",0.0}, {"",0.0},
#line 157 "trigrams_fr.gperf"
      {"rou", 0.0014971036},
#line 200 "trigrams_fr.gperf"
      {"use", 0.0011776196},
#line 148 "trigrams_fr.gperf"
      {"moi", 0.0016459336},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 72 "trigrams_fr.gperf"
      {"ren", 0.0028466515},
#line 228 "trigrams_fr.gperf"
      {"ena", 9.871571e-4},
      {"",0.0},
#line 84 "trigrams_fr.gperf"
      {"mon", 0.0024785383},
#line 67 "trigrams_fr.gperf"
      {"\303\251ta", 0.003003706},
#line 237 "trigrams_fr.gperf"
      {"ura", 9.5354056e-4},
#line 231 "trigrams_fr.gperf"
      {"oul", 9.755268e-4},
#line 163 "trigrams_fr.gperf"
      {"per", 0.0014544285},
      {"",0.0},
#line 179 "trigrams_fr.gperf"
      {"peu", 0.0012922771},
#line 122 "trigrams_fr.gperf"
      {"pro", 0.0019713254},
#line 208 "trigrams_fr.gperf"
      {"ign", 0.0011355468},
      {"",0.0},
#line 110 "trigrams_fr.gperf"
      {"nne", 0.0021154988},
#line 22 "trigrams_fr.gperf"
      {"ait", 0.010159222},
#line 218 "trigrams_fr.gperf"
      {"pr\303\251", 0.0010484591},
#line 138 "trigrams_fr.gperf"
      {"pre", 0.0017652247},
      {"",0.0},
#line 165 "trigrams_fr.gperf"
      {"ris", 0.001398293},
#line 91 "trigrams_fr.gperf"
      {"tes", 0.0023590156},
#line 93 "trigrams_fr.gperf"
      {"ter", 0.002343354},
#line 193 "trigrams_fr.gperf"
      {"tem", 0.0012263415},
      {"",0.0},
#line 142 "trigrams_fr.gperf"
      {"tro", 0.0017186806},
      {"",0.0},
#line 221 "trigrams_fr.gperf"
      {"ule", 0.0010317551},
      {"",0.0},
#line 80 "trigrams_fr.gperf"
      {"tte", 0.0025907168},
      {"",0.0},
#line 31 "trigrams_fr.gperf"
      {"tre", 0.0055988478},
#line 135 "trigrams_fr.gperf"
      {"uel", 0.0017821372},
#line 128 "trigrams_fr.gperf"
      {"rie", 0.0018889871},
#line 112 "trigrams_fr.gperf"
      {"ois", 0.002102849},
#line 97 "trigrams_fr.gperf"
      {"oir", 0.002255687},
#line 137 "trigrams_fr.gperf"
      {"dit", 0.0017760672},
      {"",0.0},
#line 85 "trigrams_fr.gperf"
      {"fai", 0.0024683445},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 114 "trigrams_fr.gperf"
      {"anc", 0.0020714568},
      {"",0.0},
#line 164 "trigrams_fr.gperf"
      {"roi", 0.0014529227},
#line 121 "trigrams_fr.gperf"
      {"enc", 0.001982747},
      {"",0.0}, {"",0.0},
#line 136 "trigrams_fr.gperf"
      {"uis", 0.0017773415},
      {"",0.0},
#line 258 "trigrams_fr.gperf"
      {"pui", 9.03614e-4},
#line 233 "trigrams_fr.gperf"
      {"ron", 9.615103e-4},
#line 86 "trigrams_fr.gperf"
      {"plu", 0.0024139},
      {"",0.0}, {"",0.0},
#line 118 "trigrams_fr.gperf"
      {"tra", 0.0019937288},
#line 141 "trigrams_fr.gperf"
      {"pri", 0.0017206961},
#line 213 "trigrams_fr.gperf"
      {"pos", 0.0010750788},
#line 192 "trigrams_fr.gperf"
      {"por", 0.0012273378},
      {"",0.0},
#line 46 "trigrams_fr.gperf"
      {"pou", 0.0040596714},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 55 "trigrams_fr.gperf"
      {"vai", 0.0035071427},
#line 42 "trigrams_fr.gperf"
      {"dan", 0.004274252},
#line 197 "trigrams_fr.gperf"
      {"pen", 0.0012003472},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 185 "trigrams_fr.gperf"
      {"van", 0.0012694105},
#line 90 "trigrams_fr.gperf"
      {"lus", 0.0023654331},
#line 44 "trigrams_fr.gperf"
      {"tou", 0.0041365656},
#line 156 "trigrams_fr.gperf"
      {"aux", 0.0015252758},
      {"",0.0}, {"",0.0},
#line 68 "trigrams_fr.gperf"
      {"eux", 0.0029863766},
#line 177 "trigrams_fr.gperf"
      {"nai", 0.0012956132},
#line 96 "trigrams_fr.gperf"
      {"ten", 0.002256637},
      {"",0.0},
#line 23 "trigrams_fr.gperf"
      {"les", 0.009093016},
#line 238 "trigrams_fr.gperf"
      {"ler", 9.465207e-4},
#line 234 "trigrams_fr.gperf"
      {"lem", 9.612323e-4},
#line 116 "trigrams_fr.gperf"
      {"leu", 0.0020457867},
      {"",0.0}, {"",0.0},
#line 254 "trigrams_fr.gperf"
      {"pla", 9.112825e-4},
#line 183 "trigrams_fr.gperf"
      {"mar", 0.0012711016},
      {"",0.0}, {"",0.0},
#line 59 "trigrams_fr.gperf"
      {"out", 0.003373835},
#line 166 "trigrams_fr.gperf"
      {"oin", 0.001395397},
      {"",0.0}, {"",0.0},
#line 260 "trigrams_fr.gperf"
      {"emb", 9.010655e-4},
#line 275 "trigrams_fr.gperf"
      {"ime", 8.528996e-4},
#line 83 "trigrams_fr.gperf"
      {"ort", 0.0024858592},
#line 60 "trigrams_fr.gperf"
      {"eme", 0.0033490222},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 61 "trigrams_fr.gperf"
      {"tio", 0.0033421647},
#line 25 "trigrams_fr.gperf"
      {"ant", 0.0072684824},
#line 113 "trigrams_fr.gperf"
      {"int", 0.0020989568},
#line 160 "trigrams_fr.gperf"
      {"emp", 0.0014680744},
#line 20 "trigrams_fr.gperf"
      {"ent", 0.013850503},
      {"",0.0},
#line 242 "trigrams_fr.gperf"
      {"tie", 9.3435764e-4},
      {"",0.0},
#line 124 "trigrams_fr.gperf"
      {"sai", 0.0019479955},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0},
#line 115 "trigrams_fr.gperf"
      {"san", 0.002053687},
      {"",0.0},
#line 235 "trigrams_fr.gperf"
      {"ord", 9.6069946e-4},
      {"",0.0},
#line 50 "trigrams_fr.gperf"
      {"ons", 0.0038294995},
#line 104 "trigrams_fr.gperf"
      {"lui", 0.0021606528},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 73 "trigrams_fr.gperf"
      {"and", 0.0028003159},
      {"",0.0}, {"",0.0},
#line 70 "trigrams_fr.gperf"
      {"end", 0.002940458},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 24 "trigrams_fr.gperf"
      {"lle", 0.007533406},
#line 266 "trigrams_fr.gperf"
      {"emi", 8.7606744e-4},
      {"",0.0},
#line 54 "trigrams_fr.gperf"
      {"mai", 0.0035285729},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0},
#line 158 "trigrams_fr.gperf"
      {"man", 0.0014950186},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 36 "trigrams_fr.gperf"
      {"une", 0.004679897},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 274 "trigrams_fr.gperf"
      {"tin", 8.5484574e-4},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 262 "trigrams_fr.gperf"
      {"lla", 8.8556623e-4},
      {"",0.0},
#line 180 "trigrams_fr.gperf"
      {"rit", 0.0012920453},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0},
#line 202 "trigrams_fr.gperf"
      {"lie", 0.0011726385},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 217 "trigrams_fr.gperf"
      {"oit", 0.001066252},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 71 "trigrams_fr.gperf"
      {"onn", 0.002880013},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 56 "trigrams_fr.gperf"
      {"rai", 0.0034915274},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 209 "trigrams_fr.gperf"
      {"lon", 0.0011294769},
      {"",0.0},
#line 205 "trigrams_fr.gperf"
      {"uit", 0.0011582745},
#line 75 "trigrams_fr.gperf"
      {"ran", 0.0027354923},
      {"",0.0}, {"",0.0},
#line 51 "trigrams_fr.gperf"
      {"mme", 0.0037896507},
      {"",0.0}, {"",0.0},
#line 52 "trigrams_fr.gperf"
      {"pas", 0.0035438403},
#line 32 "trigrams_fr.gperf"
      {"par", 0.005389689},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 216 "trigrams_fr.gperf"
      {"tit", 0.001069009},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 58 "trigrams_fr.gperf"
      {"omm", 0.0033787002},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0},
#line 45 "trigrams_fr.gperf"
      {"tai", 0.0041070036},
      {"",0.0},
#line 182 "trigrams_fr.gperf"
      {"omp", 0.0012733489},
#line 47 "trigrams_fr.gperf"
      {"ont", 0.0039808545},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 145 "trigrams_fr.gperf"
      {"tan", 0.0016948872},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 147 "trigrams_fr.gperf"
      {"ond", 0.001647648},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 117 "trigrams_fr.gperf"
      {"lai", 0.0019987097},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0},
#line 206 "trigrams_fr.gperf"
      {"lan", 0.0011470149}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash_fr (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].trigram;

          if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
            return &wordlist[key];
        }
    }
  return 0;
}
#line 276 "trigrams_fr.gperf"


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

#endif /* _TRIGRAM_FR_H_ */
