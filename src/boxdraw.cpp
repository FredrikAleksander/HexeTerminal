#include "Hexe/Terminal/TerminalEmulator.h"
#include "boxdraw_data.h"

/*
 * 1: render most of the lines/blocks characters without using the font for
 *    perfect alignment between cells (U2500 - U259F except dashes/diagonals).
 *    Bold affects lines thickness if boxdraw_bold is not 0. Italic is ignored.
 * 0: disable (render all U25XX glyphs normally from the font).
 */
const int boxdraw = 1;
const int boxdraw_bold = 0;

/* braille (U28XX):  1: render as adjacent "pixels",  0: use font */
const int boxdraw_braille = 1;

using namespace Hexe::Terminal;

int Hexe::Terminal::isboxdraw(Rune u)
{
    Rune block = u & ~0xff;
    return (boxdraw && block == 0x2500 && boxdata[u & 0xFF]) ||
           (boxdraw_braille && block == 0x2800);
}

/* the "index" is actually the entire shape data encoded as ushort */
ushort
Hexe::Terminal::boxdrawindex(const Glyph *g)
{
    if (boxdraw_braille && (g->u & ~0xff) == 0x2800)
        return BRL | (g->u & 0xFF);
    if (boxdraw_bold && (g->mode & ATTR_BOLD))
        return BDB | boxdata[g->u & 0xFF];
    return boxdata[g->u & 0xFF];
}