#include "cpudefs.h"
void op_b000(ULONG opcode) /* CMP */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	BYTE src = regs.d[srcreg];
{	BYTE dst = regs.d[dstreg];
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b010(ULONG opcode) /* CMP */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = regs.a[srcreg];
	BYTE src = get_byte(srca);
{	BYTE dst = regs.d[dstreg];
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b018(ULONG opcode) /* CMP */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = regs.a[srcreg];
	BYTE src = get_byte(srca);
{	regs.a[srcreg] += areg_byteinc[srcreg];
{	BYTE dst = regs.d[dstreg];
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b020(ULONG opcode) /* CMP */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	regs.a[srcreg] -= areg_byteinc[srcreg];
{	CPTR srca = regs.a[srcreg];
	BYTE src = get_byte(srca);
{	BYTE dst = regs.d[dstreg];
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b028(ULONG opcode) /* CMP */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	BYTE src = get_byte(srca);
{	BYTE dst = regs.d[dstreg];
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b030(ULONG opcode) /* CMP */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = get_disp_ea(regs.a[srcreg]);
{	BYTE src = get_byte(srca);
{	BYTE dst = regs.d[dstreg];
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b038(ULONG opcode) /* CMP */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = (LONG)(WORD)nextiword();
	BYTE src = get_byte(srca);
{	BYTE dst = regs.d[dstreg];
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b039(ULONG opcode) /* CMP */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = nextilong();
	BYTE src = get_byte(srca);
{	BYTE dst = regs.d[dstreg];
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b03a(ULONG opcode) /* CMP */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = m68k_getpc();
	srca += (LONG)(WORD)nextiword();
{	BYTE src = get_byte(srca);
{	BYTE dst = regs.d[dstreg];
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b03b(ULONG opcode) /* CMP */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = get_disp_ea(m68k_getpc());
{	BYTE src = get_byte(srca);
{	BYTE dst = regs.d[dstreg];
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b03c(ULONG opcode) /* CMP */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	BYTE src = nextiword();
{	BYTE dst = regs.d[dstreg];
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b040(ULONG opcode) /* CMP */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	WORD src = regs.d[srcreg];
{	WORD dst = regs.d[dstreg];
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b048(ULONG opcode) /* CMP */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	WORD src = regs.a[srcreg];
{	WORD dst = regs.d[dstreg];
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b050(ULONG opcode) /* CMP */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
{	WORD dst = regs.d[dstreg];
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b058(ULONG opcode) /* CMP */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
{	regs.a[srcreg] += 2;
{	WORD dst = regs.d[dstreg];
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b060(ULONG opcode) /* CMP */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	regs.a[srcreg] -= 2;
{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
{	WORD dst = regs.d[dstreg];
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b068(ULONG opcode) /* CMP */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	WORD src = get_word(srca);
{	WORD dst = regs.d[dstreg];
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b070(ULONG opcode) /* CMP */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = get_disp_ea(regs.a[srcreg]);
{	WORD src = get_word(srca);
{	WORD dst = regs.d[dstreg];
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b078(ULONG opcode) /* CMP */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = (LONG)(WORD)nextiword();
	WORD src = get_word(srca);
{	WORD dst = regs.d[dstreg];
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b079(ULONG opcode) /* CMP */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = nextilong();
	WORD src = get_word(srca);
{	WORD dst = regs.d[dstreg];
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b07a(ULONG opcode) /* CMP */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = m68k_getpc();
	srca += (LONG)(WORD)nextiword();
{	WORD src = get_word(srca);
{	WORD dst = regs.d[dstreg];
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b07b(ULONG opcode) /* CMP */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = get_disp_ea(m68k_getpc());
{	WORD src = get_word(srca);
{	WORD dst = regs.d[dstreg];
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b07c(ULONG opcode) /* CMP */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	WORD src = nextiword();
{	WORD dst = regs.d[dstreg];
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b080(ULONG opcode) /* CMP */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	LONG src = regs.d[srcreg];
{	LONG dst = regs.d[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b088(ULONG opcode) /* CMP */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	LONG src = regs.a[srcreg];
{	LONG dst = regs.d[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b090(ULONG opcode) /* CMP */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = regs.a[srcreg];
	LONG src = get_long(srca);
{	LONG dst = regs.d[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b098(ULONG opcode) /* CMP */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = regs.a[srcreg];
	LONG src = get_long(srca);
{	regs.a[srcreg] += 4;
{	LONG dst = regs.d[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b0a0(ULONG opcode) /* CMP */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	regs.a[srcreg] -= 4;
{	CPTR srca = regs.a[srcreg];
	LONG src = get_long(srca);
{	LONG dst = regs.d[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b0a8(ULONG opcode) /* CMP */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	LONG src = get_long(srca);
{	LONG dst = regs.d[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b0b0(ULONG opcode) /* CMP */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = get_disp_ea(regs.a[srcreg]);
{	LONG src = get_long(srca);
{	LONG dst = regs.d[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b0b8(ULONG opcode) /* CMP */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = (LONG)(WORD)nextiword();
	LONG src = get_long(srca);
{	LONG dst = regs.d[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b0b9(ULONG opcode) /* CMP */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = nextilong();
	LONG src = get_long(srca);
{	LONG dst = regs.d[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b0ba(ULONG opcode) /* CMP */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = m68k_getpc();
	srca += (LONG)(WORD)nextiword();
{	LONG src = get_long(srca);
{	LONG dst = regs.d[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b0bb(ULONG opcode) /* CMP */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = get_disp_ea(m68k_getpc());
{	LONG src = get_long(srca);
{	LONG dst = regs.d[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b0bc(ULONG opcode) /* CMP */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	LONG src = nextilong();
{	LONG dst = regs.d[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b0c0(ULONG opcode) /* CMPA */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	WORD src = regs.d[srcreg];
{	LONG dst = regs.a[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b0c8(ULONG opcode) /* CMPA */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	WORD src = regs.a[srcreg];
{	LONG dst = regs.a[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b0d0(ULONG opcode) /* CMPA */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
{	LONG dst = regs.a[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b0d8(ULONG opcode) /* CMPA */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
{	regs.a[srcreg] += 2;
{	LONG dst = regs.a[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b0e0(ULONG opcode) /* CMPA */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	regs.a[srcreg] -= 2;
{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
{	LONG dst = regs.a[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b0e8(ULONG opcode) /* CMPA */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	WORD src = get_word(srca);
{	LONG dst = regs.a[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b0f0(ULONG opcode) /* CMPA */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = get_disp_ea(regs.a[srcreg]);
{	WORD src = get_word(srca);
{	LONG dst = regs.a[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b0f8(ULONG opcode) /* CMPA */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = (LONG)(WORD)nextiword();
	WORD src = get_word(srca);
{	LONG dst = regs.a[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b0f9(ULONG opcode) /* CMPA */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = nextilong();
	WORD src = get_word(srca);
{	LONG dst = regs.a[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b0fa(ULONG opcode) /* CMPA */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = m68k_getpc();
	srca += (LONG)(WORD)nextiword();
{	WORD src = get_word(srca);
{	LONG dst = regs.a[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b0fb(ULONG opcode) /* CMPA */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = get_disp_ea(m68k_getpc());
{	WORD src = get_word(srca);
{	LONG dst = regs.a[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b0fc(ULONG opcode) /* CMPA */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	WORD src = nextiword();
{	LONG dst = regs.a[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b100(ULONG opcode) /* EOR */
{
	ULONG srcreg = ((opcode >> 9) & 7);
	ULONG dstreg = opcode & 7;
{{	BYTE src = regs.d[srcreg];
{	BYTE dst = regs.d[dstreg];
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	regs.d[dstreg] = (regs.d[dstreg] & ~0xff) | ((src) & 0xff);
}}}}
void op_b108(ULONG opcode) /* CMPM */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = regs.a[srcreg];
	BYTE src = get_byte(srca);
{	regs.a[srcreg] += areg_byteinc[srcreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
{	regs.a[dstreg] += areg_byteinc[dstreg];
{{ULONG newv = ((BYTE)(dst)) - ((BYTE)(src));
{	int flgs = ((BYTE)(src)) < 0;
	int flgo = ((BYTE)(dst)) < 0;
	int flgn = ((BYTE)(newv)) < 0;
	ZFLG = ((BYTE)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UBYTE)(src)) > ((UBYTE)(dst));
	NFLG = flgn != 0;
}}}}}}}}}
void op_b110(ULONG opcode) /* EOR */
{
	ULONG srcreg = ((opcode >> 9) & 7);
	ULONG dstreg = opcode & 7;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}
void op_b118(ULONG opcode) /* EOR */
{
	ULONG srcreg = ((opcode >> 9) & 7);
	ULONG dstreg = opcode & 7;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
{	regs.a[dstreg] += areg_byteinc[dstreg];
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void op_b120(ULONG opcode) /* EOR */
{
	ULONG srcreg = ((opcode >> 9) & 7);
	ULONG dstreg = opcode & 7;
{{	BYTE src = regs.d[srcreg];
{	regs.a[dstreg] -= areg_byteinc[dstreg];
{	CPTR dsta = regs.a[dstreg];
	BYTE dst = get_byte(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void op_b128(ULONG opcode) /* EOR */
{
	ULONG srcreg = ((opcode >> 9) & 7);
	ULONG dstreg = opcode & 7;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}
void op_b130(ULONG opcode) /* EOR */
{
	ULONG srcreg = ((opcode >> 9) & 7);
	ULONG dstreg = opcode & 7;
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = get_disp_ea(regs.a[dstreg]);
{	BYTE dst = get_byte(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}}
void op_b138(ULONG opcode) /* EOR */
{
	ULONG srcreg = ((opcode >> 9) & 7);
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = (LONG)(WORD)nextiword();
	BYTE dst = get_byte(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}
void op_b139(ULONG opcode) /* EOR */
{
	ULONG srcreg = ((opcode >> 9) & 7);
{{	BYTE src = regs.d[srcreg];
{	CPTR dsta = nextilong();
	BYTE dst = get_byte(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((BYTE)(src)) == 0;
	NFLG = ((BYTE)(src)) < 0;
	put_byte(dsta,src);
}}}}
void op_b140(ULONG opcode) /* EOR */
{
	ULONG srcreg = ((opcode >> 9) & 7);
	ULONG dstreg = opcode & 7;
{{	WORD src = regs.d[srcreg];
{	WORD dst = regs.d[dstreg];
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	regs.d[dstreg] = (regs.d[dstreg] & ~0xffff) | ((src) & 0xffff);
}}}}
void op_b148(ULONG opcode) /* CMPM */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = regs.a[srcreg];
	WORD src = get_word(srca);
{	regs.a[srcreg] += 2;
{	CPTR dsta = regs.a[dstreg];
	WORD dst = get_word(dsta);
{	regs.a[dstreg] += 2;
{{ULONG newv = ((WORD)(dst)) - ((WORD)(src));
{	int flgs = ((WORD)(src)) < 0;
	int flgo = ((WORD)(dst)) < 0;
	int flgn = ((WORD)(newv)) < 0;
	ZFLG = ((WORD)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((UWORD)(src)) > ((UWORD)(dst));
	NFLG = flgn != 0;
}}}}}}}}}
void op_b150(ULONG opcode) /* EOR */
{
	ULONG srcreg = ((opcode >> 9) & 7);
	ULONG dstreg = opcode & 7;
{{	WORD src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg];
	WORD dst = get_word(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}
void op_b158(ULONG opcode) /* EOR */
{
	ULONG srcreg = ((opcode >> 9) & 7);
	ULONG dstreg = opcode & 7;
{{	WORD src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg];
	WORD dst = get_word(dsta);
{	regs.a[dstreg] += 2;
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}}
void op_b160(ULONG opcode) /* EOR */
{
	ULONG srcreg = ((opcode >> 9) & 7);
	ULONG dstreg = opcode & 7;
{{	WORD src = regs.d[srcreg];
{	regs.a[dstreg] -= 2;
{	CPTR dsta = regs.a[dstreg];
	WORD dst = get_word(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}}
void op_b168(ULONG opcode) /* EOR */
{
	ULONG srcreg = ((opcode >> 9) & 7);
	ULONG dstreg = opcode & 7;
{{	WORD src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	WORD dst = get_word(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}
void op_b170(ULONG opcode) /* EOR */
{
	ULONG srcreg = ((opcode >> 9) & 7);
	ULONG dstreg = opcode & 7;
{{	WORD src = regs.d[srcreg];
{	CPTR dsta = get_disp_ea(regs.a[dstreg]);
{	WORD dst = get_word(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}}
void op_b178(ULONG opcode) /* EOR */
{
	ULONG srcreg = ((opcode >> 9) & 7);
{{	WORD src = regs.d[srcreg];
{	CPTR dsta = (LONG)(WORD)nextiword();
	WORD dst = get_word(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}
void op_b179(ULONG opcode) /* EOR */
{
	ULONG srcreg = ((opcode >> 9) & 7);
{{	WORD src = regs.d[srcreg];
{	CPTR dsta = nextilong();
	WORD dst = get_word(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((WORD)(src)) == 0;
	NFLG = ((WORD)(src)) < 0;
	put_word(dsta,src);
}}}}
void op_b180(ULONG opcode) /* EOR */
{
	ULONG srcreg = ((opcode >> 9) & 7);
	ULONG dstreg = opcode & 7;
{{	LONG src = regs.d[srcreg];
{	LONG dst = regs.d[dstreg];
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	regs.d[dstreg] = (src);
}}}}
void op_b188(ULONG opcode) /* CMPM */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = regs.a[srcreg];
	LONG src = get_long(srca);
{	regs.a[srcreg] += 4;
{	CPTR dsta = regs.a[dstreg];
	LONG dst = get_long(dsta);
{	regs.a[dstreg] += 4;
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}}}
void op_b190(ULONG opcode) /* EOR */
{
	ULONG srcreg = ((opcode >> 9) & 7);
	ULONG dstreg = opcode & 7;
{{	LONG src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg];
	LONG dst = get_long(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}
void op_b198(ULONG opcode) /* EOR */
{
	ULONG srcreg = ((opcode >> 9) & 7);
	ULONG dstreg = opcode & 7;
{{	LONG src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg];
	LONG dst = get_long(dsta);
{	regs.a[dstreg] += 4;
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}}
void op_b1a0(ULONG opcode) /* EOR */
{
	ULONG srcreg = ((opcode >> 9) & 7);
	ULONG dstreg = opcode & 7;
{{	LONG src = regs.d[srcreg];
{	regs.a[dstreg] -= 4;
{	CPTR dsta = regs.a[dstreg];
	LONG dst = get_long(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}}
void op_b1a8(ULONG opcode) /* EOR */
{
	ULONG srcreg = ((opcode >> 9) & 7);
	ULONG dstreg = opcode & 7;
{{	LONG src = regs.d[srcreg];
{	CPTR dsta = regs.a[dstreg] + (LONG)(WORD)nextiword();
	LONG dst = get_long(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}
void op_b1b0(ULONG opcode) /* EOR */
{
	ULONG srcreg = ((opcode >> 9) & 7);
	ULONG dstreg = opcode & 7;
{{	LONG src = regs.d[srcreg];
{	CPTR dsta = get_disp_ea(regs.a[dstreg]);
{	LONG dst = get_long(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}}
void op_b1b8(ULONG opcode) /* EOR */
{
	ULONG srcreg = ((opcode >> 9) & 7);
{{	LONG src = regs.d[srcreg];
{	CPTR dsta = (LONG)(WORD)nextiword();
	LONG dst = get_long(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}
void op_b1b9(ULONG opcode) /* EOR */
{
	ULONG srcreg = ((opcode >> 9) & 7);
{{	LONG src = regs.d[srcreg];
{	CPTR dsta = nextilong();
	LONG dst = get_long(dsta);
	src ^= dst;
	VFLG = CFLG = 0;
	ZFLG = ((LONG)(src)) == 0;
	NFLG = ((LONG)(src)) < 0;
	put_long(dsta,src);
}}}}
void op_b1c0(ULONG opcode) /* CMPA */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	LONG src = regs.d[srcreg];
{	LONG dst = regs.a[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b1c8(ULONG opcode) /* CMPA */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	LONG src = regs.a[srcreg];
{	LONG dst = regs.a[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b1d0(ULONG opcode) /* CMPA */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = regs.a[srcreg];
	LONG src = get_long(srca);
{	LONG dst = regs.a[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b1d8(ULONG opcode) /* CMPA */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = regs.a[srcreg];
	LONG src = get_long(srca);
{	regs.a[srcreg] += 4;
{	LONG dst = regs.a[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b1e0(ULONG opcode) /* CMPA */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	regs.a[srcreg] -= 4;
{	CPTR srca = regs.a[srcreg];
	LONG src = get_long(srca);
{	LONG dst = regs.a[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b1e8(ULONG opcode) /* CMPA */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = regs.a[srcreg] + (LONG)(WORD)nextiword();
	LONG src = get_long(srca);
{	LONG dst = regs.a[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b1f0(ULONG opcode) /* CMPA */
{
	ULONG srcreg = (opcode & 7);
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = get_disp_ea(regs.a[srcreg]);
{	LONG src = get_long(srca);
{	LONG dst = regs.a[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b1f8(ULONG opcode) /* CMPA */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = (LONG)(WORD)nextiword();
	LONG src = get_long(srca);
{	LONG dst = regs.a[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b1f9(ULONG opcode) /* CMPA */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = nextilong();
	LONG src = get_long(srca);
{	LONG dst = regs.a[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
void op_b1fa(ULONG opcode) /* CMPA */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = m68k_getpc();
	srca += (LONG)(WORD)nextiword();
{	LONG src = get_long(srca);
{	LONG dst = regs.a[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b1fb(ULONG opcode) /* CMPA */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	CPTR srca = get_disp_ea(m68k_getpc());
{	LONG src = get_long(srca);
{	LONG dst = regs.a[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}}
void op_b1fc(ULONG opcode) /* CMPA */
{
	ULONG dstreg = (opcode >> 9) & 7;
{{	LONG src = nextilong();
{	LONG dst = regs.a[dstreg];
{{ULONG newv = ((LONG)(dst)) - ((LONG)(src));
{	int flgs = ((LONG)(src)) < 0;
	int flgo = ((LONG)(dst)) < 0;
	int flgn = ((LONG)(newv)) < 0;
	ZFLG = ((LONG)(newv)) == 0;
	VFLG = (flgs != flgo) && (flgn != flgo);
	CFLG = ((ULONG)(src)) > ((ULONG)(dst));
	NFLG = flgn != 0;
}}}}}}}
