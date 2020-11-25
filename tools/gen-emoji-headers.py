#!/bin/env python

# This is a simple script for generating the files include/Hexe/Terminal/EmojiGlyphRanges.h and src/emoji_blocks.h based on the emoji-sequences.txt
# file for Unicode 13.1
# src/emoji_blocks.h:
#   It generates 2 bitmaps, each covering the emojis from the basic multilingual plane, aswell as the supplementary multilingual plane.
#   To determine if a codepoint is a emoji, one would iterate over the bitmaps and check if the codepoint lies within the bitmap range,
#   and if so check if the bit is set. The bitmap starts at the lowest actual emoji code point, and extends to the highest actual
#   emoji code point, which ends up saving alot of space.
# include/Hexe/Terminal/EmojiGlyphRanges.h:
#   This file contains a list of codepoint ranges covering all emoji codepoints. This is not used by the library, but is used
#   by the example code to determine what glyphs to load for emojis

from BitVector import BitVector
import re
import sys


def stripcomments(code):
    code = code.strip()
    ofs = code.find('#')
    if(ofs >= 0):
        return code[:ofs].strip()

    return code.strip()


def parseline(lin):
    lin = lin.strip()
    if(lin.startswith('#')):
        return None

    comment = lin.split('#')[1].strip() if lin.__contains__('#') else ""
    tokens = list(map(lambda y: y.strip(), stripcomments(lin).split(';')))
    if(len(tokens) > 1 and tokens[0].find(' ') == -1 and (tokens[1] in ['Basic_Emoji'])):
        return [tokens[0], comment]


def parselines(lines):
    return list(map(parseline, lines))


def printtable(min_code, max_code, data):
    from math import ceil
    from io import StringIO
    file_str = StringIO()

    file_str.write('{ \n  ')
    file_str.write(hex(min_code))
    file_str.write(',\n  ')
    file_str.write(hex(max_code))
    file_str.write(',\n  {\n')

    withComma = False

    numbytes = ceil(data.length() / 8.0)
    for i in range(numbytes):
        ofs = i * 8
        val = data[ofs:ofs+8].reverse().get_bitvector_in_hex()

        if withComma:
            file_str.write(',0x')
        else:
            file_str.write(' 0x')
        withComma = True
        file_str.write(val)
        file_str.write('\n')

    file_str.write('  }\n};\n')

    return file_str.getvalue()


def gentable(min_code, max_code, data):
    actual_min = max_code
    actual_max = min_code

    for x in data:
        tokenrange = x[0].split('..')
        rangestart = int(tokenrange[0], base=16)
        rangeend = int(tokenrange[0] if len(tokenrange)
                       == 1 else tokenrange[1], base=16)

        for code in range(rangestart, rangeend+1):
            if code >= min_code and code <= max_code:
                if code < actual_min:
                    actual_min = code
                if code > actual_max:
                    actual_max = code
    numbits = (actual_max+1) - actual_min
    if (numbits % 8) != 0:
        numbits += 8 - (numbits % 8)

    bits = BitVector(size=numbits)

    for x in data:
        tokenrange = x[0].split('..')
        rangestart = int(tokenrange[0], base=16)
        rangeend = int(tokenrange[0] if len(tokenrange)
                       == 1 else tokenrange[1], base=16)

        for code in range(rangestart, rangeend+1):
            if code >= min_code and code <= max_code:
                bit = code - actual_min
                bits[bit] = 1

    return printtable(actual_min, actual_max, bits)


def getdata():
    if(len(sys.argv) > 1):
        with open(sys.argv[1], "r", encoding="utf-8") as fil:
            return fil.readlines()
    return []


if __name__ == "__main__":
    from io import StringIO

    data = getdata()
    parsed = list(filter(lambda x: x != None, parselines(data)))
#
#    with open('src/emoji_blocks.h', 'w', encoding='utf-8') as fil:
#        fil.write("""#include <stdint.h>
#
# typedef struct emoji_range {
#    int32_t min_code;
#    int32_t max_code;
#    uint8_t bitmap[];
# } emoji_range_t;
#
#    """)
#
#        blocks = [["bmp_emoji_block", 0x00, 0xFFFF],
#                  ["smp_emoji_block", 0x10000, 0x1FFFF]]
#
#        for name, mincode, maxcode in blocks:
#            fil.write('static const emoji_range_t {0} = '.format(name))
#            fil.write(gentable(mincode, maxcode, parsed))
#            fil.write("\n")
#
#        fil.write('\nstatic const emoji_range_t* emoji_ranges[] = {\n')
#        for name, mincode, maxcode in blocks:
#            fil.write('  &{0},\n'.format(name))
#        fil.write('  0\n};\n')
#    with open('include/Hexe/Terminal/EmojiGlyphRanges.h', 'w', encoding='utf-8') as fil:
#        fil.write("""#include <stdint.h>
#
# static const int32_t emojiGlyphRanges[] = {
# """)
#
#        for rangetoken, comment in parsed:
#            tokens = rangetoken.split('..')
#
#            rangestart = int(tokens[0], base=16)
#            rangeend = int(tokens[0] if len(tokens) ==
#                           1 else tokens[1], base=16)
#
#            fil.write(
#                "  {0}, {1}, // {2}\n".format(hex(rangestart), hex(rangeend), comment))
#        fil.write("  0\n};\n")
