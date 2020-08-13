#!/usr/bin/env python
import codecs
import sys
import itertools



pyout = codecs.getwriter('utf8')(sys.stdout)

arabic_cp1256_hex_string    =   "\\x81\\x8D\\x8E\\x90\\xA1\\xBA\\xBF\\xC1\\xC2\\xC3\\xC4\\xC5\\xC6\\xC7\\xC8\\xC9\\xCA\\xCB\\xCC\\xCD\\xCE\\xCF\\xD0\\xD1\\xD2\\xD3\\xD4\\xD5\\xD6\\xD8\\xD9\\xDA\\xDB\\xDC\\xDD\\xDE\\xDF\\xE1\\xE3\\xE4\\xE5\\xE6\\xEC\\xED\\xF0\\xF1\\xF2\\xF3\\xF5\\xF6\\xF8\\xFA"
latin_ascii_char_string     =   "PJRG,;?'|>&<}AbptvjHxd*rzs$SDTZEg_fqklmnhwYyFNKaui~o";
latin_ascii_char_string_len =   len(latin_ascii_char_string);

arabic_cp1256_hex_symbols       =   map(lambda x: '0x' + x, itertools.ifilter(lambda x: x,  arabic_cp1256_hex_string.split("\\x")))
arabic_cp1256_hex_symbols_len   =   len(arabic_cp1256_hex_symbols);

if arabic_cp1256_hex_symbols_len != latin_ascii_char_string_len:
    print "Error: not equal symbols for latin (%d) and arabic (%d) strings" % ( latin_ascii_char_string_len, arabic_cp1256_hex_symbols_len)
    quit();

def hex_in_enc_to_hex_in_utf8(symbol_hex, enc):
    symbol_int  =   int(symbol_hex, 16)
    decoder     =   codecs.getdecoder(enc)
    symbol      =   decoder(chr(symbol_int))[0]
    return symbol

arabic_utf8_symbols     =   map(lambda x: hex_in_enc_to_hex_in_utf8(x, 'cp1256'), arabic_cp1256_hex_symbols)
arabic_int32_symbols    =   map(lambda x: ord(x[0]), arabic_utf8_symbols)
latin_int32_symbols     =   map(lambda x: ord(x[0]), latin_ascii_char_string)

#handwritten as our source was in cp1256 and
#it has not some arabic from dictstems so hardcode in
def add_symbol_not_from_cp1256(arabic_int, translit, latin_ascii_char_string):
    arabic_int32_symbols.append(arabic_int)
    latin_int32_symbols.append(ord(translit[0]))
    latin_ascii_char_string +=  translit
    arabic_utf8_symbols.append(unichr(arabic_int))
    arabic_cp1256_hex_symbols.append('NOT_IN_ALPHABET')
    return latin_ascii_char_string

latin_ascii_char_string =   add_symbol_not_from_cp1256(int('671', 16), '{', latin_ascii_char_string)
latin_ascii_char_string =   add_symbol_not_from_cp1256(int('670', 16), '`', latin_ascii_char_string)
latin_ascii_char_string =   add_symbol_not_from_cp1256(int('6A4', 16), 'V', latin_ascii_char_string)



CONNECTION_NAME='BUCKWALTER_CONNECT_SYMBOLS'
pyout.write('//in utf-8\n#ifndef %s\n#define %s\n#endif\n\n' % (CONNECTION_NAME, CONNECTION_NAME))
zipped_symbols      =   zip(arabic_int32_symbols, latin_int32_symbols, latin_ascii_char_string, arabic_utf8_symbols, arabic_cp1256_hex_symbols)
for record in zipped_symbols:
    pyout.write("BUCKWALTER_CONNECT(%s, %s)\t//%s <-> %s (%s)\n" % record)
