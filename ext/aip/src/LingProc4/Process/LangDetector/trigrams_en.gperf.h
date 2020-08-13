/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: /usr/local/bin/gperf -r -m 20 trigrams_en.gperf  */
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

#line 9 "trigrams_en.gperf"

#ifndef _TRIGRAM_EN_H_
#define _TRIGRAM_EN_H_
#include <_include/_string.h>
#line 14 "trigrams_en.gperf"
struct trigram_en
{
    const char  *trigram;
    const float  freq;
};

#define TOTAL_KEYWORDS 256
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 3
#define MIN_HASH_VALUE 6
#define MAX_HASH_VALUE 584
/* maximum key range = 579, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash_en (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      585, 585, 585, 585, 585, 585, 585, 585, 585, 585,
      585, 585, 585, 585, 585, 585, 585, 585, 585, 585,
      585, 585, 585, 585, 585, 585, 585, 585, 585, 585,
      585, 585, 585, 585, 585, 585, 585, 585, 585, 585,
      585, 585, 585, 585, 585, 585, 585, 585, 585, 585,
      585, 585, 585, 585, 585, 585, 585, 585, 585, 585,
      585, 585, 585, 585, 585, 585, 585, 585, 585, 585,
      585, 585, 585, 585, 585, 585, 585, 585, 585, 585,
      585, 585, 585, 585, 585, 585, 585, 585, 585, 585,
      585, 585, 585, 585, 585, 585, 585, 585,  42,  41,
      175, 172,  32, 211,  37,   1,  50,  70, 233,  82,
      266,  59, 114, 257,   8,   0,  84,  21,  23,   2,
        5,  68, 184,  12, 585, 230,  98,  14,  45, 149,
       33, 585,  37, 208,  84,  57,   2,  23, 585, 585,
      585, 585, 585, 585, 585, 585, 585, 585, 585, 585,
      585, 585, 585, 585, 585, 585, 585, 585, 585, 585,
      585, 585, 585, 585, 585, 585, 585, 585, 585, 585,
      585, 585, 585, 585, 585, 585, 585, 585, 585, 585,
      585, 585, 585, 585, 585, 585, 585, 585, 585, 585,
      585, 585, 585, 585, 585, 585, 585, 585, 585, 585,
      585, 585, 585, 585, 585, 585, 585, 585, 585, 585,
      585, 585, 585, 585, 585, 585, 585, 585, 585, 585,
      585, 585, 585, 585, 585, 585, 585, 585, 585, 585,
      585, 585, 585, 585, 585, 585, 585, 585, 585, 585,
      585, 585, 585, 585, 585, 585, 585, 585, 585, 585,
      585, 585, 585, 585, 585, 585, 585, 585, 585, 585,
      585, 585, 585, 585, 585, 585, 585, 585, 585, 585,
      585, 585, 585, 585
    };
  return len + asso_values[(unsigned char)str[2]+4] + asso_values[(unsigned char)str[1]+18] + asso_values[(unsigned char)str[0]+1];
}

#ifdef __GNUC__
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#else
__inline
#endif
#endif
const struct trigram_en *
get_trigram_en (register const char *str, register unsigned int len)
{
  static const struct trigram_en wordlist[] =
    {
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0},
#line 182 "trigrams_en.gperf"
      {"ree", 0.0012025812},
#line 78 "trigrams_en.gperf"
      {"res", 0.0021939508},
#line 236 "trigrams_en.gperf"
      {"rat", 9.886672e-4},
#line 24 "trigrams_en.gperf"
      {"hat", 0.0070028645},
#line 28 "trigrams_en.gperf"
      {"was", 0.0057947114},
#line 158 "trigrams_en.gperf"
      {"ran", 0.0013522133},
#line 80 "trigrams_en.gperf"
      {"han", 0.0021539899},
#line 273 "trigrams_en.gperf"
      {"ren", 8.772742e-4},
#line 74 "trigrams_en.gperf"
      {"hen", 0.0022941998},
#line 161 "trigrams_en.gperf"
      {"ven", 0.0013334644},
#line 172 "trigrams_en.gperf"
      {"rie", 0.0012951776},
      {"",0.0},
#line 26 "trigrams_en.gperf"
      {"his", 0.006133578},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 105 "trigrams_en.gperf"
      {"rin", 0.0018318157},
#line 41 "trigrams_en.gperf"
      {"hin", 0.0034059065},
#line 37 "trigrams_en.gperf"
      {"wit", 0.0039991736},
      {"",0.0},
#line 263 "trigrams_en.gperf"
      {"har", 9.0011244e-4},
#line 264 "trigrams_en.gperf"
      {"win", 8.9681224e-4},
#line 23 "trigrams_en.gperf"
      {"her", 0.008757891},
#line 31 "trigrams_en.gperf"
      {"ver", 0.0045866775},
#line 216 "trigrams_en.gperf"
      {"war", 0.0010913794},
#line 229 "trigrams_en.gperf"
      {"tan", 0.0010127012},
#line 67 "trigrams_en.gperf"
      {"wer", 0.0023979642},
#line 167 "trigrams_en.gperf"
      {"ten", 0.0013159591},
      {"",0.0},
#line 55 "trigrams_en.gperf"
      {"tio", 0.0027030397},
#line 191 "trigrams_en.gperf"
      {"eas", 0.0011541068},
#line 46 "trigrams_en.gperf"
      {"eve", 0.003278658},
      {"",0.0},
#line 120 "trigrams_en.gperf"
      {"eat", 0.0016988041},
#line 43 "trigrams_en.gperf"
      {"had", 0.0033316524},
#line 59 "trigrams_en.gperf"
      {"red", 0.0025809568},
#line 205 "trigrams_en.gperf"
      {"hed", 0.0011177809},
#line 96 "trigrams_en.gperf"
      {"tin", 0.0019391194},
#line 106 "trigrams_en.gperf"
      {"een", 0.0018277502},
      {"",0.0},
#line 217 "trigrams_en.gperf"
      {"bee", 0.0010878162},
#line 53 "trigrams_en.gperf"
      {"ave", 0.002851739},
#line 35 "trigrams_en.gperf"
      {"ter", 0.004278422},
#line 242 "trigrams_en.gperf"
      {"eme", 9.60807e-4},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 104 "trigrams_en.gperf"
      {"ive", 0.0018530277},
      {"",0.0},
#line 51 "trigrams_en.gperf"
      {"ear", 0.0029680107},
      {"",0.0},
#line 123 "trigrams_en.gperf"
      {"ame", 0.0016435619},
#line 162 "trigrams_en.gperf"
      {"ect", 0.0013277489},
#line 265 "trigrams_en.gperf"
      {"tre", 8.959513e-4},
#line 56 "trigrams_en.gperf"
      {"ted", 0.0027009828},
#line 173 "trigrams_en.gperf"
      {"ure", 0.0012842487},
#line 58 "trigrams_en.gperf"
      {"ain", 0.0026360077},
#line 210 "trigrams_en.gperf"
      {"nes", 0.0011054889},
#line 157 "trigrams_en.gperf"
      {"ace", 0.0013564222},
#line 164 "trigrams_en.gperf"
      {"ime", 0.001318781},
#line 240 "trigrams_en.gperf"
      {"ade", 9.746056e-4},
#line 207 "trigrams_en.gperf"
      {"eir", 0.0011113479},
#line 247 "trigrams_en.gperf"
      {"urn", 9.4045594e-4},
#line 116 "trigrams_en.gperf"
      {"ead", 0.001721451},
#line 25 "trigrams_en.gperf"
      {"ere", 0.006473544},
#line 61 "trigrams_en.gperf"
      {"ers", 0.0025394175},
#line 213 "trigrams_en.gperf"
      {"ice", 0.0010987212},
#line 245 "trigrams_en.gperf"
      {"unt", 9.448323e-4},
#line 109 "trigrams_en.gperf"
      {"ide", 0.0018047924},
#line 228 "trigrams_en.gperf"
      {"gre", 0.0010136339},
      {"",0.0},
#line 274 "trigrams_en.gperf"
      {"air", 8.736153e-4},
#line 194 "trigrams_en.gperf"
      {"ene", 0.001137845},
#line 177 "trigrams_en.gperf"
      {"nin", 0.0012482097},
#line 71 "trigrams_en.gperf"
      {"are", 0.0023436307},
#line 66 "trigrams_en.gperf"
      {"nce", 0.0024115713},
#line 33 "trigrams_en.gperf"
      {"ent", 0.004476528},
#line 108 "trigrams_en.gperf"
      {"nde", 0.0018086426},
#line 129 "trigrams_en.gperf"
      {"art", 0.0015873632},
#line 196 "trigrams_en.gperf"
      {"hei", 0.0011355731},
#line 175 "trigrams_en.gperf"
      {"les", 0.001267963},
#line 145 "trigrams_en.gperf"
      {"see", 0.0014403855},
#line 272 "trigrams_en.gperf"
      {"ire", 8.83755e-4},
#line 254 "trigrams_en.gperf"
      {"ied", 9.2395506e-4},
#line 222 "trigrams_en.gperf"
      {"lan", 0.0010557232},
#line 60 "trigrams_en.gperf"
      {"aid", 0.002573902},
#line 118 "trigrams_en.gperf"
      {"ant", 0.001711096},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 119 "trigrams_en.gperf"
      {"ine", 0.0017023912},
#line 204 "trigrams_en.gperf"
      {"ins", 0.0011231377},
#line 111 "trigrams_en.gperf"
      {"ned", 0.0017555528},
#line 189 "trigrams_en.gperf"
      {"lit", 0.0011663749},
#line 77 "trigrams_en.gperf"
      {"int", 0.0022609348},
#line 135 "trigrams_en.gperf"
      {"tur", 0.0015250662},
#line 117 "trigrams_en.gperf"
      {"lin", 0.001714468},
#line 54 "trigrams_en.gperf"
      {"but", 0.0027589751},
#line 233 "trigrams_en.gperf"
      {"sin", 9.939763e-4},
#line 73 "trigrams_en.gperf"
      {"und", 0.0023122074},
#line 143 "trigrams_en.gperf"
      {"tte", 0.0014579865},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 237 "trigrams_en.gperf"
      {"age", 9.847932e-4},
      {"",0.0}, {"",0.0},
#line 148 "trigrams_en.gperf"
      {"end", 0.0013980811},
      {"",0.0},
#line 103 "trigrams_en.gperf"
      {"ard", 0.0018583846},
#line 91 "trigrams_en.gperf"
      {"ove", 0.001970495},
#line 253 "trigrams_en.gperf"
      {"spe", 9.3029236e-4},
      {"",0.0},
#line 170 "trigrams_en.gperf"
      {"tle", 0.0013090958},
#line 127 "trigrams_en.gperf"
      {"led", 0.0015972637},
#line 130 "trigrams_en.gperf"
      {"ack", 0.0015725363},
#line 137 "trigrams_en.gperf"
      {"sed", 0.0015070827},
#line 21 "trigrams_en.gperf"
      {"and", 0.01598371},
      {"",0.0},
#line 198 "trigrams_en.gperf"
      {"abo", 0.0011343296},
#line 68 "trigrams_en.gperf"
      {"ate", 0.0023973184},
#line 223 "trigrams_en.gperf"
      {"nge", 0.0010517773},
#line 48 "trigrams_en.gperf"
      {"ome", 0.0031115208},
#line 246 "trigrams_en.gperf"
      {"ick", 9.424169e-4},
#line 232 "trigrams_en.gperf"
      {"wil", 0.001001605},
#line 110 "trigrams_en.gperf"
      {"ind", 0.0018036446},
#line 256 "trigrams_en.gperf"
      {"sid", 9.1878953e-4},
#line 261 "trigrams_en.gperf"
      {"oin", 9.005668e-4},
#line 200 "trigrams_en.gperf"
      {"ite", 0.0011303837},
      {"",0.0}, {"",0.0},
#line 266 "trigrams_en.gperf"
      {"owe", 8.952339e-4},
#line 219 "trigrams_en.gperf"
      {"itt", 0.0010636388},
#line 115 "trigrams_en.gperf"
      {"ble", 0.001729247},
      {"",0.0}, {"",0.0},
#line 215 "trigrams_en.gperf"
      {"nto", 0.0010930535},
#line 138 "trigrams_en.gperf"
      {"nte", 0.0014897448},
#line 113 "trigrams_en.gperf"
      {"own", 0.0017513917},
      {"",0.0},
#line 268 "trigrams_en.gperf"
      {"til", 8.9353597e-4},
      {"",0.0},
#line 218 "trigrams_en.gperf"
      {"ile", 0.0010713153},
      {"",0.0},
#line 199 "trigrams_en.gperf"
      {"eri", 0.0011316512},
#line 81 "trigrams_en.gperf"
      {"ore", 0.0021495658},
#line 188 "trigrams_en.gperf"
      {"lik", 0.0011729035},
#line 250 "trigrams_en.gperf"
      {"ink", 9.328033e-4},
      {"",0.0},
#line 156 "trigrams_en.gperf"
      {"ort", 0.0013622813},
#line 234 "trigrams_en.gperf"
      {"ang", 9.931632e-4},
#line 50 "trigrams_en.gperf"
      {"uld", 0.0030202395},
#line 225 "trigrams_en.gperf"
      {"app", 0.0010310913},
#line 42 "trigrams_en.gperf"
      {"one", 0.003363841},
#line 141 "trigrams_en.gperf"
      {"ons", 0.0014747026},
#line 238 "trigrams_en.gperf"
      {"how", 9.84004e-4},
      {"",0.0}, {"",0.0},
#line 22 "trigrams_en.gperf"
      {"ing", 0.014340841},
#line 70 "trigrams_en.gperf"
      {"sai", 0.0023704625},
      {"",0.0},
#line 163 "trigrams_en.gperf"
      {"sto", 0.0013236357},
#line 101 "trigrams_en.gperf"
      {"ste", 0.0018755072},
#line 192 "trigrams_en.gperf"
      {"too", 0.0011453303},
      {"",0.0}, {"",0.0},
#line 125 "trigrams_en.gperf"
      {"ous", 0.0016089818},
#line 52 "trigrams_en.gperf"
      {"rea", 0.0029394329},
#line 121 "trigrams_en.gperf"
      {"hea", 0.0016921798},
#line 45 "trigrams_en.gperf"
      {"out", 0.0032873151},
#line 149 "trigrams_en.gperf"
      {"wor", 0.0013964309},
      {"",0.0},
#line 76 "trigrams_en.gperf"
      {"oun", 0.0022814055},
#line 241 "trigrams_en.gperf"
      {"elf", 9.7063585e-4},
#line 206 "trigrams_en.gperf"
      {"lle", 0.0011118023},
#line 244 "trigrams_en.gperf"
      {"den", 9.5482846e-4},
#line 185 "trigrams_en.gperf"
      {"can", 0.0011863911},
      {"",0.0},
#line 83 "trigrams_en.gperf"
      {"hav", 0.0020914539},
#line 262 "trigrams_en.gperf"
      {"dis", 9.00232e-4},
      {"",0.0},
#line 208 "trigrams_en.gperf"
      {"ord", 0.0011097457},
#line 152 "trigrams_en.gperf"
      {"who", 0.0013813649},
#line 87 "trigrams_en.gperf"
      {"whe", 0.0020497711},
#line 128 "trigrams_en.gperf"
      {"str", 0.0015911656},
#line 184 "trigrams_en.gperf"
      {"din", 0.0011950242},
#line 257 "trigrams_en.gperf"
      {"tor", 9.1697206e-4},
#line 57 "trigrams_en.gperf"
      {"our", 0.0026844342},
      {"",0.0}, {"",0.0},
#line 85 "trigrams_en.gperf"
      {"der", 0.0020797357},
#line 259 "trigrams_en.gperf"
      {"car", 9.124283e-4},
      {"",0.0},
#line 150 "trigrams_en.gperf"
      {"sel", 0.0013937525},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 95 "trigrams_en.gperf"
      {"tho", 0.0019523441},
#line 20 "trigrams_en.gperf"
      {"the", 0.035693683},
#line 38 "trigrams_en.gperf"
      {"ion", 0.0036946004},
#line 107 "trigrams_en.gperf"
      {"ati", 0.0018252871},
#line 252 "trigrams_en.gperf"
      {"rse", 9.30388e-4},
      {"",0.0},
#line 195 "trigrams_en.gperf"
      {"ded", 0.0011376298},
      {"",0.0},
#line 40 "trigrams_en.gperf"
      {"not", 0.0035309545},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 102 "trigrams_en.gperf"
      {"rou", 0.001872781},
#line 62 "trigrams_en.gperf"
      {"hou", 0.0025355914},
#line 251 "trigrams_en.gperf"
      {"ttl", 9.3170325e-4},
#line 93 "trigrams_en.gperf"
      {"now", 0.0019613598},
#line 220 "trigrams_en.gperf"
      {"did", 0.0010570145},
#line 154 "trigrams_en.gperf"
      {"wou", 0.0013715122},
      {"",0.0},
#line 176 "trigrams_en.gperf"
      {"hic", 0.0012542362},
#line 221 "trigrams_en.gperf"
      {"nti", 0.0010562493},
#line 39 "trigrams_en.gperf"
      {"ght", 0.0036706382},
      {"",0.0},
#line 187 "trigrams_en.gperf"
      {"thr", 0.0011768015},
#line 160 "trigrams_en.gperf"
      {"ong", 0.0013389648},
#line 165 "trigrams_en.gperf"
      {"tra", 0.0013181114},
#line 132 "trigrams_en.gperf"
      {"loo", 0.0015528308},
#line 166 "trigrams_en.gperf"
      {"use", 0.0013169874},
#line 75 "trigrams_en.gperf"
      {"hey", 0.0022906843},
      {"",0.0},
#line 133 "trigrams_en.gperf"
      {"way", 0.0015517786},
#line 134 "trigrams_en.gperf"
      {"ust", 0.0015343928},
      {"",0.0}, {"",0.0},
#line 193 "trigrams_en.gperf"
      {"lon", 0.0011446367},
#line 230 "trigrams_en.gperf"
      {"abl", 0.0010075357},
#line 86 "trigrams_en.gperf"
      {"oug", 0.0020518277},
#line 65 "trigrams_en.gperf"
      {"ess", 0.002413102},
#line 190 "trigrams_en.gperf"
      {"low", 0.0011622855},
#line 79 "trigrams_en.gperf"
      {"ell", 0.0021890723},
#line 89 "trigrams_en.gperf"
      {"est", 0.0019984748},
      {"",0.0}, {"",0.0},
#line 90 "trigrams_en.gperf"
      {"fro", 0.001994505},
#line 140 "trigrams_en.gperf"
      {"old", 0.0014753244},
#line 142 "trigrams_en.gperf"
      {"sti", 0.0014583691},
      {"",0.0},
#line 226 "trigrams_en.gperf"
      {"ass", 0.0010277672},
#line 98 "trigrams_en.gperf"
      {"kin", 0.0018892819},
#line 32 "trigrams_en.gperf"
      {"all", 0.0044870265},
#line 124 "trigrams_en.gperf"
      {"ast", 0.0016223498},
#line 155 "trigrams_en.gperf"
      {"lea", 0.0013703643},
      {"",0.0},
#line 214 "trigrams_en.gperf"
      {"bou", 0.0010950384},
      {"",0.0},
#line 147 "trigrams_en.gperf"
      {"hem", 0.0014038683},
      {"",0.0},
#line 69 "trigrams_en.gperf"
      {"ill", 0.0023850743},
#line 144 "trigrams_en.gperf"
      {"ist", 0.0014569821},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 146 "trigrams_en.gperf"
      {"sho", 0.0014245064},
#line 44 "trigrams_en.gperf"
      {"she", 0.00332223},
#line 64 "trigrams_en.gperf"
      {"him", 0.002422285},
#line 100 "trigrams_en.gperf"
      {"whi", 0.0018832554},
#line 136 "trigrams_en.gperf"
      {"ked", 0.0015132286},
#line 159 "trigrams_en.gperf"
      {"ake", 0.0013404713},
#line 72 "trigrams_en.gperf"
      {"man", 0.0023159618},
      {"",0.0},
#line 94 "trigrams_en.gperf"
      {"men", 0.001958179},
#line 231 "trigrams_en.gperf"
      {"uch", 0.0010068422},
#line 203 "trigrams_en.gperf"
      {"kno", 0.0011251225},
      {"",0.0},
#line 209 "trigrams_en.gperf"
      {"par", 0.0011062063},
#line 179 "trigrams_en.gperf"
      {"ike", 0.0012377114},
#line 114 "trigrams_en.gperf"
      {"per", 0.0017390997},
      {"",0.0}, {"",0.0},
#line 47 "trigrams_en.gperf"
      {"oul", 0.0031166144},
#line 174 "trigrams_en.gperf"
      {"min", 0.001271048},
#line 34 "trigrams_en.gperf"
      {"thi", 0.0044695693},
#line 224 "trigrams_en.gperf"
      {"enc", 0.0010509641},
#line 248 "trigrams_en.gperf"
      {"cal", 9.357687e-4},
#line 181 "trigrams_en.gperf"
      {"tim", 0.0012070053},
#line 227 "trigrams_en.gperf"
      {"ppe", 0.0010237974},
      {"",0.0},
#line 258 "trigrams_en.gperf"
      {"sse", 9.160872e-4},
#line 169 "trigrams_en.gperf"
      {"pro", 0.0013109611},
#line 186 "trigrams_en.gperf"
      {"pre", 0.0011862715},
      {"",0.0}, {"",0.0},
#line 197 "trigrams_en.gperf"
      {"anc", 0.0011345687},
#line 131 "trigrams_en.gperf"
      {"ery", 0.0015557723},
#line 168 "trigrams_en.gperf"
      {"ood", 0.0013138786},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 171 "trigrams_en.gperf"
      {"ich", 0.0013018975},
#line 260 "trigrams_en.gperf"
      {"med", 9.009255e-4},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0},
#line 139 "trigrams_en.gperf"
      {"any", 0.0014844119},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 112 "trigrams_en.gperf"
      {"ook", 0.0017517265},
#line 151 "trigrams_en.gperf"
      {"ose", 0.0013835172},
#line 63 "trigrams_en.gperf"
      {"ugh", 0.0024447646},
      {"",0.0}, {"",0.0},
#line 243 "trigrams_en.gperf"
      {"ost", 9.5659815e-4},
      {"",0.0},
#line 235 "trigrams_en.gperf"
      {"don", 9.922066e-4},
      {"",0.0}, {"",0.0},
#line 97 "trigrams_en.gperf"
      {"con", 0.0018925103},
#line 202 "trigrams_en.gperf"
      {"dow", 0.0011257683},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0},
#line 84 "trigrams_en.gperf"
      {"sta", 0.0020892776},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 271 "trigrams_en.gperf"
      {"oke", 8.8406587e-4},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 255 "trigrams_en.gperf"
      {"tly", 9.2161144e-4},
      {"",0.0},
#line 49 "trigrams_en.gperf"
      {"igh", 0.003049176},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 201 "trigrams_en.gperf"
      {"ple", 0.0011285424},
      {"",0.0}, {"",0.0},
#line 211 "trigrams_en.gperf"
      {"ath", 0.0011029779},
#line 180 "trigrams_en.gperf"
      {"che", 0.001217145},
#line 126 "trigrams_en.gperf"
      {"wha", 0.0015990095},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 36 "trigrams_en.gperf"
      {"ith", 0.0042363564},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0},
#line 267 "trigrams_en.gperf"
      {"ssi", 8.9384685e-4},
      {"",0.0}, {"",0.0},
#line 27 "trigrams_en.gperf"
      {"tha", 0.0059860977},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 269 "trigrams_en.gperf"
      {"ful", 8.8839437e-4},
#line 30 "trigrams_en.gperf"
      {"for", 0.0050047245},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 239 "trigrams_en.gperf"
      {"nly", 9.7649486e-4},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 88 "trigrams_en.gperf"
      {"cou", 0.0020356856},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 29 "trigrams_en.gperf"
      {"you", 0.0057697687},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 99 "trigrams_en.gperf"
      {"rom", 0.001884499},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 178 "trigrams_en.gperf"
      {"lly", 0.0012452683},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0},
#line 275 "trigrams_en.gperf"
      {"mon", 8.682585e-4},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0},
#line 82 "trigrams_en.gperf"
      {"oth", 0.002141411},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0},
#line 153 "trigrams_en.gperf"
      {"mor", 0.0013759603},
      {"",0.0}, {"",0.0},
#line 270 "trigrams_en.gperf"
      {"chi", 8.8636164e-4},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 122 "trigrams_en.gperf"
      {"som", 0.0016451164},
#line 249 "trigrams_en.gperf"
      {"ish", 9.338556e-4},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 212 "trigrams_en.gperf"
      {"pla", 0.001099869},
      {"",0.0}, {"",0.0}, {"",0.0},
#line 183 "trigrams_en.gperf"
      {"cha", 0.0012018159},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
      {"",0.0}, {"",0.0}, {"",0.0}, {"",0.0},
#line 92 "trigrams_en.gperf"
      {"com", 0.0019682948}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash_en (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].trigram;

          if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
            return &wordlist[key];
        }
    }
  return 0;
}
#line 276 "trigrams_en.gperf"


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

#endif /* _TRIGRAM_EN_H_ */
