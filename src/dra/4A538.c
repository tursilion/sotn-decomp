#include "dra.h"

void func_800EA538(s32 arg0) {
    Unkstruct_8006C3C4* var_v0;
    s32 temp;
    s32 v1;
    s32 i;

    // !FAKE: 0x8000 inside a temp is weird
    temp = 0x8000;
    v1 = (temp >> (arg0 - 1));

    if (arg0 != 0) {
        for (i = 0; i < LEN(D_8006C3C4); i++) {
            if (v1 & D_8006C3C4[i].unk8) {
                D_8006C3C4[i].unk8 = 0;
            }
        }
        return;
    }

    D_8003C0F8 = 0;
    var_v0 = &D_8006C3C4;

    for (i = 0; i < LEN(D_8006C3C4); i++) {
        var_v0->unk8 = 0;
        var_v0++;
    }
}

void func_800EA5AC(u16 arg0, u8 arg1, u8 arg2, u8 arg3) {
    D_8003C0EC[3] = arg0;
    D_8003C0EC[0] = arg1;
    D_8003C0EC[1] = arg2;
    D_8003C0EC[2] = arg3;
}

s32 func_800EA5E4(u32 arg0) {
    u16 temp_v0;
    s32 i;
    s32 j;
    u32 start;
    u32 count;
    u_long* clut;
    Unkstruct_8006C3C4* clutAnim;

    temp_v0 = arg0 & 0xFF00;
    arg0 = arg0 & 0xFF;

    if (temp_v0 & 0x8000) {
        clut = g_api.o.cluts[arg0];
    } else {
        clut = D_800A3BB8[arg0];
    }

    if (clut[0] == 0) {
        return 1;
    }
    if (clut[0] == -1) {
        return 1;
    }

    clutAnim = &D_8006C3C4;
    for (j = 0; j < LEN(D_8006C3C4); clutAnim++) {
        j++;
        if (clutAnim->unk8 != 0) {
            continue;
        }
        clutAnim->desc = clut;
        clutAnim->data = clut + 3;
        clutAnim->unk8 = (temp_v0 | GET_PAL_OP_KIND(clut[0]));
        clutAnim->index = 0;
        clutAnim->unkC = 0;

        // Set unkStruct's array to all zeros, except within this range
        start = clut[1];
        count = clut[1] + clut[2] - 1;
        start >>= 8;
        count >>= 8;
        for (i = 0; i < LEN(clutAnim->unkArray); i++) {
            clutAnim->unkArray[i] = 0;
        }
        for (i = start; count >= i; i++) {
            clutAnim->unkArray[i] = 1;
        }

        if ((u8)clutAnim->unk8 == 2 || (u8)clutAnim->unk8 == 16) {
            clutAnim->unkE = 0x1F;
        }
        return 0;
    }
    return -1;
}

u16 func_800EA720(u32 colorDst, u32 colorSrc) {
    u16 colorRes = colorSrc;

    if (GET_RED(colorRes) < GET_RED(colorDst)) {
        colorRes = (colorRes & ~RED_MASK) | (GET_RED(colorRes) + 1);
    }
    if (GET_RED(colorDst) < GET_RED(colorRes)) {
        colorRes = (colorRes & ~RED_MASK) | (GET_RED(colorRes) - 1);
    }

    if (GET_GREEN(colorRes) < GET_GREEN(colorDst)) {
        colorRes = (colorRes & ~GREEN_MASK) | (GET_GREEN(colorRes) + 0x20);
    }
    if (GET_GREEN(colorDst) < GET_GREEN(colorRes)) {
        colorRes = (colorRes & ~GREEN_MASK) | (GET_GREEN(colorRes) - 0x20);
    }

    if (GET_BLUE(colorRes) < GET_BLUE(colorDst)) {
        colorRes = (colorRes & ~BLUE_MASK) | (GET_BLUE(colorRes) + 0x400);
    }
    if (GET_BLUE(colorDst) < GET_BLUE(colorRes)) {
        colorRes = (colorRes & ~BLUE_MASK) | (GET_BLUE(colorRes) - 0x400);
    }

    return colorRes;
}

INCLUDE_ASM("dra/nonmatchings/4A538", func_800EA7CC);

s32 func_800EAD0C(void) { // the return type is needed for matching
    func_800EA5E4(4);
    func_800EA5E4(5);
    func_800EA5E4(6);
    func_800EA5E4(7);
    func_800EA5E4(8);

    if (g_PlayableCharacter == PLAYER_ALUCARD && g_StageId != STAGE_ST0) {
        func_800EA5E4(0x17);
    }
}

void func_800EAD7C(void) {
    s32 index = 0;
    s32 i;
    s32 j;

    for (i = 0xF0; i < 0x100; i++) {
        for (j = 0x200; j < 0x300; j += 0x10) {
            D_8003C104[index++] = GetClut(j, i);
        }
    }

    for (i = 0xF0; i < 0x100; i++) {
        for (j = 0; j < 0x100; j += 0x10) {
            D_8003C104[index++] = GetClut(j, i);
        }
    }

    for (i = 0xF0; i < 0x100; i++) {
        for (j = 0x100; j < 0x200; j += 0x10) {
            D_8003C104[index++] = GetClut(j, i);
        }
    }
}

void func_800EAEA4(void) {
    u16* ptr;
    s32 i;

    for (ptr = D_801374F8, i = 0; i < 32; i++) {
        *ptr++ = ~0;
    }

    for (ptr = D_80137538, i = 0; i < 32; i++) {
        *ptr++ = ~0;
    }
}

void ResetPendingGfxLoad(void) {
    GfxLoad* gfxLoad = g_GfxLoad;
    s32 i;

    for (i = 0; i < LEN(g_GfxLoad); i++, gfxLoad++) {
        gfxLoad->kind = GFX_BANK_NONE;
    }

    func_800EAEA4();
}

void LoadGfxAsync(s32 gfxId) {
    // Schedule the load of new graphics into the video RAM in the very next
    // frame by LoadPendingGfx. A maximum amount of 16 transfers can be chained.

    s32 i;
    GfxBank* gfxBank;
    GfxLoad* gfxLoad;

    if (gfxId & ANIMSET_OVL_FLAG) {
        gfxBank = g_api.o.gfxBanks[gfxId & 0x7FFF];
    } else {
        gfxBank = g_GfxSharedBank[gfxId];
    }

    if (gfxBank->kind != GFX_BANK_NONE && gfxBank->kind != -1) {
        for (i = 0; i < LEN(g_GfxLoad); i++) {
            gfxLoad = &g_GfxLoad[i];
            if (gfxLoad->kind == GFX_BANK_NONE) {
                gfxLoad->kind = gfxBank->kind;
                gfxLoad->unk6 = 0;
                gfxLoad->unk8 = 0;
                gfxLoad->next = gfxBank->entries;
                break;
            }
        }
    }
}

void DecompressWriteNibble(s32 ch) {
    u8 temp = ch;

    if (g_DecWriteNibbleFlag == 0) {
        g_DecWriteNibbleFlag = 1;
        *g_DecDstPtr = temp;
    } else {
        g_DecWriteNibbleFlag = 0;
        *g_DecDstPtr += ch * 16;
        g_DecDstPtr++;
    }
}

u8 DecompressReadNibble(void) {
    u8 ret;

    if (g_DecReadNibbleFlag != 0) {
        g_DecReadNibbleFlag = 0;
        ret = *g_DecSrcPtr;
        g_DecSrcPtr++;
        return (ret >> 0) & 0xF;
    } else {
        g_DecReadNibbleFlag = 1;
        ret = *g_DecSrcPtr;
        return (ret >> 4) & 0xF;
    }
}

s32 DecompressData(u8* dst, u8* src) {
    u32 buf[8];
    u8 ch;
    s32 len;
    s32 count;
    s32 i;
    s32 var_v1;
    u32* var_a0;
    u32 op;

    var_a0 = buf;
    for (var_v1 = 0; var_v1 < LEN(buf); var_v1++) {
        *var_a0++ = *src++;
    }

    g_DecReadNibbleFlag = 0;
    g_DecWriteNibbleFlag = 0;
    g_DecSrcPtr = src;
    g_DecDstPtr = dst;

    while (count = 1) {
        op = DecompressReadNibble();
        switch (op) {
        case 0:
            len = DecompressReadNibble();
            op = DecompressReadNibble();
            count = len * 0x10 + op + 0x13;
            for (i = 0; i < count; i++) {
                DecompressWriteNibble(0);
            }
            break;
        case 2:
            ch = DecompressReadNibble();
            DecompressWriteNibble(ch);
            DecompressWriteNibble(ch);
            break;
        case 4:
            DecompressWriteNibble(DecompressReadNibble());
        case 3:
            DecompressWriteNibble(DecompressReadNibble());
        case 1:
            DecompressWriteNibble(DecompressReadNibble());
            break;
        case 5:
            len = DecompressReadNibble();
            op = DecompressReadNibble();
            count = op + 3;
            ch = len;
            for (i = 0; i < count; i++) {
                DecompressWriteNibble(ch);
            }
            break;
        case 6:
            len = DecompressReadNibble();
            count = len + 3;
            for (i = 0; i < count; i++) {
                DecompressWriteNibble(0);
            }
            break;
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14: {
            len = buf[op - 7];
            switch (len & 0xF0) {
            case 0x20:
                DecompressWriteNibble(len & 0xF);
            case 0x10:
                DecompressWriteNibble(len & 0xF);
                break;
            case 0x60:
                count = (len & 0xF) + 3;
                for (i = 0; i < count; i++) {
                    DecompressWriteNibble(0);
                }
                break;
            }
            break;
        }
        case 15:
            if (dst + 0x2000 >= g_DecDstPtr) {
                return 0;
            }
            return g_DecDstPtr - dst + 0x2000;
        }
    }
}

void LoadPendingGfx(void) {
    // Called every frame, it continuously checks if there is any new graphics
    // to transfer from RAM to Video RAM. Compressed graphics are loaded as
    // 128x128 images into a 256x256 tpage (hence the `j < 4`). Each pending
    // GFX load contains a descriptor to instruct this function in which area
    // of the Video RAM the texture will be transferred to.

    char buf[0x100];
    s32 i;
    s32 j;
    u32 xy;
    u32 wh;
    u8* cmp;
    u8* src;
    u8* dst;
    s32 over;
    GfxLoad* gfxLoad;
    GfxEntry* gfxEntry;

    j = 0;
    gfxLoad = g_GfxLoad;
    for (i = 0; i < LEN(g_GfxLoad); i++, gfxLoad++) {
        switch (gfxLoad->kind) {
        case GFX_BANK_NONE:
            break;
        case GFX_BANK_4BPP:
        case GFX_BANK_8BPP:
        case GFX_BANK_16BPP:
            for (gfxEntry = gfxLoad->next; gfxEntry->xy != -1; gfxEntry++) {
                xy = (u32)gfxEntry->xy;
                wh = (u32)gfxEntry->wh;
                src = (u8*)gfxEntry->data;
                LoadTPage(src, gfxLoad->kind - 1, 0, xy >> 0x10, (u16)xy,
                          wh >> 0x10, (u16)wh);
            }
            gfxLoad->kind = GFX_BANK_NONE;
            break;
        case GFX_BANK_COMPRESSED:
            gfxEntry = gfxLoad->next;
            for (; j < 4; j++) {
                dst = g_Pix[j];
                xy = (u32)gfxEntry->xy;
                wh = (u32)gfxEntry->wh;
                cmp = (u8*)gfxEntry->data;
                over = DecompressData(dst, cmp);
                if (over) {
                    sprintf(buf, "over:%08x(%04x)", cmp, over);
                    DebugInputWait(buf);
                }
                LoadTPage(dst, 0, 0, xy >> 0x10, (u16)xy, wh >> 0x10, (u16)wh);
                gfxLoad->next = ++gfxEntry;
                if (gfxEntry->xy == -1) {
                    gfxLoad->kind = GFX_BANK_NONE;
                    break;
                }
            }
            break;
        }
    }
}

void func_800EB4F8(PixPattern* pix, s32 bitDepth, s32 x, s32 y) {
    LoadTPage(pix + 1, bitDepth, 0, x, y, (int)pix->w, (int)pix->h);
}

void LoadEquipIcon(s32 equipIcon, s32 palette, s32 index) {
    u8* iconGfx;
    s32 vramX;
    s32 var_t0;
    s32 i;

    if (D_801374F8[index] != equipIcon) {
        iconGfx = g_GfxEquipIcon[equipIcon];
        vramX = ((index & 7) * 4) + 0x280;
        var_t0 = index;
        if (index < 0) {
            var_t0 = index + 7;
        }

        LoadTPage(iconGfx, 0, 0, vramX, (var_t0 >> 3) * 0x10 + 0x180, 16, 16);
    }

    if (D_80137538[index] != palette) {
        for (i = 0; i < 16; i++) {
            if (D_800705CC) { // FAKE
            }
            D_800705CC[index * 0x10 + i] = g_PalEquipIcon[palette * 0x10 + i];
        }

        LoadClut2(D_800705CC, 0, 0xFD);
        LoadClut2(D_800705CC + 0x100, 0, 0xFE);
    }
    if (D_800973EC == 0) {
        D_80137478[index] = equipIcon;
        D_801374B8[index] = palette;
    }
    D_801374F8[index] = equipIcon;
    D_80137538[index] = palette;
}

void func_800EB6B4(void) {
    s32 i;

    for (i = 0; i < 32; i++) {
        LoadEquipIcon(D_80137478[i], D_801374B8[i], i);
    }
}

bool func_800EB720(void) {
    GfxLoad* temp = g_GfxLoad;
    s32 i;

    for (i = 0; i < 16; i++) {
        if (temp[i].kind != 0) {
            return true;
        }
    }

    return false;
}

void func_800EB758(
    s16 pivotX, s16 pivotY, Entity* e, u16 flags, POLY_GT4* p, u8 flipX) {
    const int H_CENTER = FLT(STAGE_WIDTH / 2);
    s16 px, py;
    s16 dx, dy;
    s32 rot;
    s32 distance;
    s32 angle;
    s16 rx0, ry0;
    s16 rx1, ry1;
    s16 rx2, ry2;
    s16 rx3, ry3;
    s16 scaledValue;

    if (flipX) {
        px = pivotX - e->rotPivotX;
    } else {
        px = e->rotPivotX + pivotX;
    }
    py = pivotY + e->rotPivotY;

    if (flags & FLAG_DRAW_ROTX) {
        scaledValue = (e->rotX * (s16)(p->x0 - px) - 0x80000000) >> 8;
        p->x0 = p->x2 = scaledValue + px;
        scaledValue = (e->rotX * (s16)(p->x1 - px) - 0x80000000) >> 8;
        p->x1 = p->x3 = scaledValue + px;
    }
    if (flags & FLAG_DRAW_ROTY) {
        scaledValue = (e->rotY * (s16)(p->y0 - py) - 0x80000000) >> 8;
        p->y0 = p->y1 = scaledValue + py;
        scaledValue = (e->rotY * (s16)(p->y2 - py) - 0x80000000) >> 8;
        p->y2 = p->y3 = scaledValue + py;
    }
    if (flags & FLAG_DRAW_ROTZ) {
        if (flipX) {
            rot = -e->rotZ;
        } else {
            rot = e->rotZ;
        }

        dx = p->x0 - px;
        dy = p->y0 - py;
        distance = SquareRoot12((dx * dx + dy * dy) * FLT(1.0));
        angle = ratan2(dy, dx) + rot;
        rx0 = ((rcos(angle) >> 4) * distance + H_CENTER) >> 0x14;
        ry0 = ((rsin(angle) >> 4) * distance + H_CENTER) >> 0x14;
        p->x0 = rx0 + px;
        p->y0 = ry0 + py;

        dx = p->x1 - px;
        dy = p->y1 - py;
        distance = SquareRoot12((dx * dx + dy * dy) * FLT(1.0));
        angle = ratan2(dy, dx) + rot;
        rx1 = ((rcos(angle) >> 4) * distance + H_CENTER) >> 0x14;
        ry1 = ((rsin(angle) >> 4) * distance + H_CENTER) >> 0x14;
        p->x1 = rx1 + px;
        p->y1 = ry1 + py;

        dx = p->x2 - px;
        dy = p->y2 - py;
        distance = SquareRoot12((dx * dx + dy * dy) * 4096);
        angle = ratan2(dy, dx) + rot;
        rx2 = ((rcos(angle) >> 4) * distance + H_CENTER) >> 0x14;
        ry2 = ((rsin(angle) >> 4) * distance + H_CENTER) >> 0x14;
        p->x2 = rx2 + px;
        p->y2 = ry2 + py;

        dx = p->x3 - px;
        dy = p->y3 - py;
        distance = SquareRoot12((dx * dx + dy * dy) * 4096);
        angle = ratan2(dy, dx) + rot;
        rx3 = ((rcos(angle) >> 4) * distance + H_CENTER) >> 0x14;
        ry3 = ((rsin(angle) >> 4) * distance + H_CENTER) >> 0x14;
        p->x3 = rx3 + px;
        p->y3 = ry3 + py;
    }
}

void ResetEntityArray(void) {
    Entity* entity;
    u8* ch;
    s32 i;
    u32 j;

    entity = &g_Entities[0];
    for (i = 0; i < ARRAY_COUNT(g_Entities); i++) {
        ch = (s8*)entity;
        for (j = 0; j < sizeof(Entity); j++) {
            *ch++ = 0;
        }
        entity++;
    }
}

INCLUDE_ASM("dra/nonmatchings/4A538", RenderEntities);

#define PL_SPRT(x, y, flipx) (x), ((y)&0x1FF) | ((flipx) << 9)
s16 D_800A21B8[] = {
    PL_SPRT(0x0201, 0x0101, false), PL_SPRT(0x0221, 0x0101, false),
    PL_SPRT(0x0231, 0x0101, false), PL_SPRT(0x0201, 0x0181, false),
    PL_SPRT(0x0201, 0x0101, false), PL_SPRT(0x0201, 0x0101, false),
    PL_SPRT(0x0201, 0x0101, false), PL_SPRT(0x0201, 0x0101, false),
    PL_SPRT(0x0001, 0x0101, false), PL_SPRT(0x0021, 0x0101, false),
    PL_SPRT(0x0001, 0x0181, false), PL_SPRT(0x0021, 0x0181, false),
    PL_SPRT(0x0221, 0x0181, false), PL_SPRT(0x0221, 0x0101, false),
    PL_SPRT(0x0221, 0x0181, false), PL_SPRT(0x0221, 0x0181, false),
    PL_SPRT(0x0101, 0x0101, false),
};

s16 unused_800A21FC[286] = {0};

void InitRenderer(void) {
    int i;
    POLY_GT4 *a1, *a2;
    SPRT_16 *b1, *b2;
    TILE *c1, *c2;
    LINE_G2 *d1, *d2;
    POLY_G4 *e1, *e2;
    SPRT *f1, *f2;
    POLY_GT3 *g1, *g2;
    PlayerDraw* plDraw;

    a1 = g_GpuBuffers[0].polyGT4;
    a2 = g_GpuBuffers[1].polyGT4;
    for (i = 0; i < 0x300; i++, a1++, a2++) {
        SetPolyGT4(a1);
        SetPolyGT4(a2);
    }

    b1 = g_GpuBuffers[0].sprite16;
    b2 = g_GpuBuffers[1].sprite16;
    for (i = 0; i < 0x280; i++, b1++, b2++) {
        SetSprt16(b1);
        SetSprt16(b2);
    }

    c1 = g_GpuBuffers[0].tiles;
    c2 = g_GpuBuffers[1].tiles;
    for (i = 0; i < 0x100; i++, c1++, c2++) {
        SetTile(c1);
        SetTile(c2);
    }

    d1 = g_GpuBuffers[0].lineG2;
    d2 = g_GpuBuffers[1].lineG2;
    for (i = 0; i < 0x100; i++, d1++, d2++) {
        SetLineG2(d1);
        SetLineG2(d2);
    }

    e1 = g_GpuBuffers[0].polyG4;
    e2 = g_GpuBuffers[1].polyG4;
    for (i = 0; i < 0x100; i++, e1++, e2++) {
        SetPolyG4(e1);
        SetPolyG4(e2);
    }

    f1 = g_GpuBuffers[0].sprite;
    f2 = g_GpuBuffers[1].sprite;
    for (i = 0; i < 0x200; i++, f1++, f2++) {
        SetSprt(f1);
        SetSprt(f2);
    }

    g1 = g_GpuBuffers[0].polyGT3;
    g2 = g_GpuBuffers[1].polyGT3;
    for (i = 0; i < 0x30; i++, g1++, g2++) {
        SetPolyGT3(g1);
        SetPolyGT3(g2);
    }

    for (plDraw = g_PlayerDraw, i = 0; i < 16; i++, plDraw++) {
        plDraw->rect0.x = D_800A21B8[i * 2 + 0];
        plDraw->rect0.y = D_800A21B8[i * 2 + 1] & 0x1FF;
        plDraw->flipX = (D_800A21B8[i * 2 + 1] >> 8) & 0xFE;
        plDraw->tpage = (plDraw->rect0.x >> 6) + 0x10;
    }
}

void HideAllBackgroundLayers(void) {
    s32 i;

    g_Tilemap.flags = 0;
    for (i = 0; i < MAX_BG_LAYER_COUNT; i++) {
        g_Tilemap.bg[i].flags = 0;
    }
}

// internal strucutre that holds all the information to render room layers
typedef struct {
    /* 0x00 */ u16 x;
    /* 0x02 */ u16 y;
    /* 0x04 */ u16 flags;
    /* 0x06 */ u16 clutAlt;
    /* 0x08 */ u16 order;
    /* 0x0A */ u16 isSemiTrans;
    /* 0x0C */ u16 tpage;
    /* 0x0E */ u16 roomTileWidth;
    /* 0x10 */ u16 roomTileHeight;
    /* 0x12 */ u16 pad12;
    /* 0x14 */ u16* tiles;
    /* 0x18 */ u8* page;
    /* 0x1C */ u8* gfx;
    /* 0x20 */ u8* clut;
    /* 0x24 */ OT_TYPE* ot;
    /* 0x28 */ SPRT_16* sp16;
    /* 0x2C */ DR_MODE* dr;
    /* 0x30 */ RECT rect;
} TilemapRenderer;

void RenderTilemap(void) {
#ifdef VERSION_PC
    SPRT_16 _sp16;
    TilemapRenderer _r;
    SPRT_16* sp16 = &_sp16;
    TilemapRenderer* r = &_r;
#else
    SPRT_16* sp16 = (SPRT_16*)0x1F800000;
    TilemapRenderer* r = (TilemapRenderer*)0x1F800010;
#endif

    Tilemap* t = &g_Tilemap;
    BgLayer* bg;
    s32 subX;
    s32 subY;
    s32 row;
    s32 tx;
    s32 ty;
    u16 tpage;
    s32 startTx;
    s32 startTy;
    s32 i;
    s32 j;
    s32 l;
    u16 tile;
    u16 gfx;
    u16 page;
    u16 clut;
    s32 x0;
    s32 y0;
    u8 u;
    u8 v;

    r->rect.x = 0;
    r->rect.y = 0;
    r->rect.w = 255;
    r->rect.h = 255;
    setSprt16(sp16);
    r->ot = g_CurrentBuffer->ot;
    r->sp16 = &g_CurrentBuffer->sprite16[g_GpuUsage.sp16];
    r->dr = &g_CurrentBuffer->drawModes[g_GpuUsage.drawModes];
    r->x = g_backbufferX;
    r->y = g_backbufferY;
    if (t->hideTimer > 0) {
        t->hideTimer--;
    } else {
        r->flags = t->flags;
        if (r->flags & LAYER_SHOW) {
            r->clutAlt = r->flags & LAYER_CLUT_ALT;
            r->page = t->tileDef->gfxPage;
            r->gfx = t->tileDef->gfxIndex;
            r->clut = t->tileDef->clut;
            r->order = t->order;
            r->roomTileWidth = t->hSize * 16;
            r->roomTileHeight = t->vSize * 16;
            subX = t->scrollX.i.hi & 0xF;
            startTx = t->scrollX.i.hi >> 4;
            subY = t->scrollY.i.hi & 0xF;
            startTy = t->scrollY.i.hi >> 4;
            r->tpage = 0;
            r->isSemiTrans = false;
            if (r->flags & LAYER_SEMI_TRANS) {
                r->isSemiTrans = true;
                r->tpage = r->flags & LAYER_TPAGE_ID;
            }
            y0 = r->y - subY;
            for (i = 0; i < N_VERTI_TILES; i++, y0 += TILE_SIZE) {
                x0 = r->x - subX;
                ty = startTy + i;
                if (ty < 0) {
                    continue;
                }
                if (ty >= r->roomTileHeight) {
                    break;
                }
                row = ty * r->roomTileWidth;
                r->tiles = &t->fg[startTx + ty * r->roomTileWidth];
                j = 0;
                tx = startTx;
                for (; j < N_HORIZ_TILES; j++, x0 += TILE_SIZE, tx++) {
                    if (tx < 0) {
                        continue;
                    }
                    if (tx >= r->roomTileWidth) {
                        break;
                    }

                    tile = *r->tiles++;
                    if (tile == 0) {
                        continue;
                    }
                    page = r->page[tile];
                    gfx = r->gfx[tile];
                    u = gfx * 0x10;
                    v = gfx & 0xF0;
                    clut = D_8003C104[r->clut[tile]];
                    if (r->clutAlt) {
                        clut = D_8003C104[0x100 + r->clut[tile]];
                    }
                    if (g_GpuUsage.sp16 >= MAXSPRT16) {
                        continue;
                    }
                    sp16->clut = clut;
                    sp16->x0 = x0;
                    sp16->y0 = y0;
                    sp16->u0 = u;
                    sp16->v0 = v;
                    setSemiTrans(sp16, r->isSemiTrans);
                    setShadeTex(sp16, true);
                    __builtin_memcpy(r->sp16, sp16, sizeof(SPRT_16));
                    addPrim(r->order + page + r->ot, r->sp16);
                    r->sp16++;
                    g_GpuUsage.sp16++;
                }
            }

            for (i = 0; i < 24; i++) {
                if (g_GpuUsage.drawModes >= MAX_DRAW_MODES) {
                    break;
                }
                tpage = i + 8 + r->tpage;
                if (r->flags & LAYER_TPAGE_ALT) {
                    tpage += 0x80;
                }
                SetDrawMode(r->dr, 0, 0, tpage, &r->rect);
                AddPrim(i + r->order + r->ot, r->dr);
                r->dr++;
                g_GpuUsage.drawModes++;
            }
        }
    }

    l = 0;
    bg = g_Tilemap.bg;
    for (; l < MAX_BG_LAYER_COUNT; l++, bg++) {
        if (bg->hideTimer > 0) {
            bg->hideTimer--;
            continue;
        }
        r->flags = bg->flags;
        if (!(r->flags & LAYER_SHOW)) {
            continue;
        }
        r->clutAlt = r->flags & LAYER_CLUT_ALT;
        r->page = bg->tileDef->gfxPage;
        r->gfx = bg->tileDef->gfxIndex;
        r->clut = bg->tileDef->clut;
        r->order = bg->order;
        r->roomTileWidth = bg->w * 16;
        r->roomTileHeight = bg->h * 16;
        subX = bg->scrollX.i.hi & 0xF;
        startTx = bg->scrollX.i.hi >> 4;
        subY = bg->scrollY.i.hi & 0xF;
        startTy = bg->scrollY.i.hi >> 4;
        if (r->flags & LAYER_WRAP_BG) {
            startTx &= TILE_SIZE - 1;
            startTy &= TILE_SIZE - 1;
        }
        r->tpage = 0;
        r->isSemiTrans = false;
        if (r->flags & LAYER_SEMI_TRANS) {
            r->isSemiTrans = true;
            r->tpage = r->flags & LAYER_TPAGE_ID;
        }
        y0 = r->y - subY;
        for (i = 0; i < N_VERTI_TILES; i++, y0 += TILE_SIZE) {
            x0 = r->x - subX;
            ty = startTy + i;
            if (r->flags & LAYER_WRAP_BG) {
                ty &= TILE_SIZE - 1;
            }
            if (ty < 0) {
                continue;
            }
            if (ty >= r->roomTileHeight) {
                break;
            }
            row = ty * r->roomTileWidth;
            r->tiles = &bg->layout[startTx + row];
            j = 0;
            tx = startTx;
            for (; j < N_HORIZ_TILES; j++, x0 += TILE_SIZE, tx++) {
                if (r->flags & LAYER_WRAP_BG) {
                    tx &= 0xF;
                    r->tiles = &bg->layout[tx + row];
                }
                if (tx < 0) {
                    continue;
                }
                if (tx >= r->roomTileWidth) {
                    break;
                }

                tile = *r->tiles++;
                if (tile == 0) {
                    continue;
                }
                page = r->page[tile];
                gfx = r->gfx[tile];
                u = gfx * 0x10;
                v = gfx & 0xF0;
                clut = D_8003C104[r->clut[tile]];
                if (r->clutAlt) {
                    clut = D_8003C104[0x100 + r->clut[tile]];
                }
                if (g_GpuUsage.sp16 >= MAXSPRT16) {
                    continue;
                }
                sp16->clut = clut;
                sp16->x0 = x0;
                sp16->y0 = y0;
                sp16->u0 = u;
                sp16->v0 = v;
                setSemiTrans(sp16, r->isSemiTrans);
                setShadeTex(sp16, true);
                __builtin_memcpy(r->sp16, sp16, sizeof(SPRT_16));
                addPrim(r->order + page + r->ot, r->sp16);
                r->sp16++;
                g_GpuUsage.sp16++;
            }
        }
        for (i = 0; i < 24; i++) {
            if (g_GpuUsage.drawModes >= MAX_DRAW_MODES) {
                break;
            }
            tpage = i + 8 + r->tpage;
            if (r->flags & LAYER_TPAGE_ALT) {
                tpage += 0x80;
            }
            SetDrawMode(r->dr, 0, 0, tpage, &r->rect);
            AddPrim(i + r->order + r->ot, r->dr);
            r->dr++;
            g_GpuUsage.drawModes++;
        }
    }
}

void SetRoomForegroundLayer(LayerDef* layerDef) {
    D_8003C708.flags = 0;
    D_8013AED0 = 1;
    g_Tilemap.tileDef = layerDef->tileDef;
    g_Tilemap.flags = 0;
    if (g_Tilemap.tileDef == 0) {
        return;
    }

    g_Tilemap.fg = layerDef->layout;
    g_Tilemap.order = layerDef->zPriority;
    if (layerDef->rect.params & 0x40) {
        g_Tilemap.order = 0x60;
        D_8003C708.flags = layerDef->rect.params;
        D_8003C708.unk2 = 0;
        D_8003C708.unk4 = 0;
        D_8003C708.zPriority = layerDef->zPriority;
    }
    if (layerDef->rect.params & 0x20) {
        g_Tilemap.order = 0x60;
        D_8003C708.flags = layerDef->rect.params;
    }
    if (layerDef->rect.params & 0x10) {
        g_Tilemap.order = 0x60;
        D_8013AED0 = 0;
    };
    g_Tilemap.flags = layerDef->flags;
    g_Tilemap.left = layerDef->rect.left;
    g_Tilemap.top = layerDef->rect.top;
    g_Tilemap.right = layerDef->rect.right;
    g_Tilemap.bottom = layerDef->rect.bottom;
    g_Tilemap.hSize = g_Tilemap.right - g_Tilemap.left + 1;
    g_Tilemap.vSize = g_Tilemap.bottom - g_Tilemap.top + 1;
    g_Tilemap.y = 0;
    g_Tilemap.x = 0;
    g_Tilemap.width = g_Tilemap.hSize << 8;
    g_Tilemap.height = g_Tilemap.vSize << 8;
    g_Tilemap.hideTimer = 1;
}

void SetRoomBackgroundLayer(s32 index, LayerDef* layerDef) {
    g_Tilemap.bg[index].flags = 0;
    g_Tilemap.bg[index].tileDef = layerDef->tileDef;
    g_Tilemap.bg[index].layout = layerDef->layout;
    if (g_Tilemap.bg[index].tileDef != 0) {
        g_Tilemap.bg[index].order = layerDef->zPriority;
        g_Tilemap.bg[index].flags = layerDef->flags;
        g_Tilemap.bg[index].w = layerDef->rect.right - layerDef->rect.left + 1;
        g_Tilemap.bg[index].h = layerDef->rect.bottom - layerDef->rect.top + 1;
        g_Tilemap.bg[index].scrollKind = layerDef->rect.params;
        g_Tilemap.bg[index].hideTimer = 1;
    }
}

void LoadRoomLayer(s32 layerIndex) {
    s32 i;

    SetRoomForegroundLayer(g_api.o.tileLayers[layerIndex].fg);
    SetRoomBackgroundLayer(0, g_api.o.tileLayers[layerIndex].bg);

    for (i = 1; i < MAX_BG_LAYER_COUNT; i++) {
        g_Tilemap.bg[i].flags = 0;
    }
}

void DestroyPrimitive(Primitive* prim) {
    s32 i;
    s32 n;
    u32* primData = (u32*)prim;

    for (n = sizeof(Primitive) / sizeof(*primData), i = 0; i < n; i++) {
        *primData++ = 0;
    }
}

void DestroyAllPrimitives(void) {
    Primitive* prim;
    s32 i;

    for (i = 0, prim = g_PrimBuf; i < MAX_PRIM_COUNT; i++) {
        DestroyPrimitive(prim);
        prim->type = PRIM_NONE;
        prim++;
    }
}

void func_800EDAE4(void) {
    s32 i;
    DR_ENV* ptr = &D_800974AC;

    for (i = 0; i < 16; i++, ptr++) {
        ptr->tag = 0;
    }
}

DR_ENV* func_800EDB08(Primitive* prim) {
    DR_ENV* dr = &D_800974AC;
    s32 i;

    for (i = 0; i < LEN(D_800974AC); i++, dr++) {
        if (dr->tag == 0) {
            dr->tag = 1;
            setcode(prim, 7);
            *(DR_ENV**)&prim->r1 = dr;
            return dr;
        }
    }

    return NULL;
}

s32 D_800A2438 = 0;

// This function casts its return value as an s16, but at least one caller
// (EntityGravityBootBeam) needs to receive a returned s32 so we use that here.
s32 func_800EDB58(u8 primType, s32 count) {
    Primitive* prim;
    Primitive* temp_v0;
    bool isLooping;
    s32 primStartIdx;
    s32 var_s1;
    s32 i;
    s32 var_v1;

    var_v1 = count;
    primStartIdx = 0;
    i = 0;
    prim = g_PrimBuf;
    isLooping = 1;
    while (isLooping) {
        var_v1--;
        if (prim->type != 0) {
            var_v1 = i;
            primStartIdx = var_v1 + 1;
            var_v1 = count;
        } else if (var_v1 == 0) {
            break;
        }
        var_s1 = i + 1;
        prim++;
        i++;
        isLooping = i < 0x400;
        if (isLooping) {
            continue;
        }
        if (var_v1 != 0) {
            return -1;
        }
    }

    for (i = 0, prim = &g_PrimBuf[primStartIdx]; i < count; i++, prim++) {
        DestroyPrimitive(prim);
        var_s1 = 0;
        temp_v0 = &g_PrimBuf[i];
        prim->type = primType;
        prim->next = temp_v0;
        prim->next = prim->next + primStartIdx + 1;
    }
    prim[-1].next = NULL;
    prim[-1].type &= 0xEF;
    // Casted return value as mentioned above
    return (s16)primStartIdx;
}

s32 AllocPrimitives(u8 primType, s32 count) {
    s32 primIndex = 0;
    Primitive* prim = g_PrimBuf;
    u8* dstPrimType = &g_PrimBuf->type;
    s16 index;

    while (primIndex < MAX_PRIM_ALLOC_COUNT) {
        if (*dstPrimType == 0) {
            DestroyPrimitive(prim);
            if (count == 1) {
                *dstPrimType = primType;
                prim->next = NULL;
                if (D_800A2438 < primIndex) {
                    D_800A2438 = primIndex;
                }
            } else {
                *dstPrimType = primType;
                index = AllocPrimitives(primType, count - 1);
                if (index == -1) {
                    *dstPrimType = 0;
                    return -1;
                }
                prim->next = &g_PrimBuf[index];
            }
            return (s16)primIndex;
        }

        primIndex++;
        dstPrimType += sizeof(Primitive);
        prim++;
        if (primIndex >= 0x400) {
            return -1;
        }
    }
    return -1;
}

s32 func_800EDD9C(u8 type, s32 count) {
    Primitive* prim;
    s32 i;
    s16 foundPolyIndex;

    prim = &g_PrimBuf[LEN(g_PrimBuf) - 1];
    i = LEN(g_PrimBuf) - 1;

    while (i >= 0) {
        if (prim->type == 0) {
            DestroyPrimitive(prim);
            if (count == 1) {
                prim->type = type;
                prim->next = NULL;
            } else {
                prim->type = type;
                foundPolyIndex = func_800EDD9C(type, count - 1);
                prim->next = &g_PrimBuf[foundPolyIndex];
            }
            foundPolyIndex = i;
            return foundPolyIndex;
        }
        i--;
        prim--;
    }
}

void FreePrimitives(s32 primitiveIndex) {
    Primitive* prim = &g_PrimBuf[primitiveIndex];

    if (prim) {
        do {
            if (prim->type == PRIM_ENV) {
                **(DR_ENV***)&prim->r1 = NULL;
                prim->type = PRIM_NONE;
            } else
                prim->type = PRIM_NONE;
            prim = prim->next;
        } while (prim);
    }
}

#ifndef NON_MATCHING
INCLUDE_ASM("dra/nonmatchings/4A538", RenderPrimitives);
#else
typedef struct {
    /* 0x1F800000 */ OT_TYPE* ot;
    /* 0x1F800004 */ POLY_GT4* gt4;
    /* 0x1F800008 */ POLY_G4* g4;
    /* 0x1F80000C */ POLY_GT3* gt3;
    /* 0x1F800010 */ LINE_G2* g2;
    /* 0x1F800014 */ TILE* tile;
    /* 0x1F800018 */ DR_MODE* dr;
    /* 0x1F80001C */ SPRT* sprt;
    /* 0x1F800020 */ DR_ENV* env;
} PrimitivesRenderer;

typedef union {
    TILE tile;
    LINE_G2 g2;
    POLY_G4 g4;
    POLY_GT4 gt4;
    POLY_GT3 gt3;
    SPRT sprt;
} PrimBuf;

void RenderPrimitives(void) {
#ifdef VERSION_PC
    RECT _rect;
    RECT* rect = &_rect;
    PrimitivesRenderer _r;
    PrimitivesRenderer* r = &_r;
    PrimBuf _primbuf;
    PrimBuf* primbuf = &_primbuf;
#else
    RECT* rect = (RECT*)0x1F800128;
    PrimitivesRenderer* r = (PrimitivesRenderer*)0x1F800000;
    PrimBuf* primbuf = (PrimBuf*)0x1F800024;
#endif

    DRAWENV sp18;
    s32 i;
    s16 sp80;
    Primitive* prim;
    DR_ENV* env;
    s32 useShadeTex;
    s32 var_a1;
    s32 tpage;
    s32 dtd;
    u16 var_v0;
    u16 drawMode;
    u8 primType;

    rect->x = 0;
    rect->y = 0;
    rect->w = 255;
    rect->h = 255;
    r->ot = g_CurrentBuffer->ot;
    r->gt4 = &g_CurrentBuffer->polyGT4[g_GpuUsage.gt4];
    r->g4 = &g_CurrentBuffer->polyG4[g_GpuUsage.g4];
    r->gt3 = &g_CurrentBuffer->polyGT3[g_GpuUsage.gt3];
    r->g2 = &g_CurrentBuffer->lineG2[g_GpuUsage.line];
    r->tile = &g_CurrentBuffer->tiles[g_GpuUsage.tile];
    r->dr = &g_CurrentBuffer->drawModes[g_GpuUsage.drawModes];
    r->sprt = &g_CurrentBuffer->sprite[g_GpuUsage.sp];
    r->env = &g_CurrentBuffer->env[g_GpuUsage.env];
    if (D_8003C0EC[3]) {
        if (g_GpuUsage.tile < MAX_TILE_COUNT) {
            setSemiTrans(r->tile, false);
            r->tile->r0 = D_8003C0EC[0];
            r->tile->g0 = D_8003C0EC[1];
            r->tile->b0 = D_8003C0EC[2];
            r->tile->x0 = 0;
            r->tile->y0 = 0;
            r->tile->w = DISP_STAGE_W;
            r->tile->h = DISP_STAGE_H;
            addPrim(r->ot, r->tile);
            g_GpuUsage.tile++;
            D_8003C0EC[3]--;
        }
    } else {
        var_a1 = 0;
        i = 0;
        prim = g_PrimBuf + i;
        for (; i < MAX_PRIM_COUNT; prim++, i++) {
            primType = prim->type;
            if (!primType) {
                continue;
            }
            drawMode = prim->drawMode;
            if (drawMode & DRAW_HIDE) {
                continue;
            }
            var_v0 = drawMode & DRAW_COLORS;
            useShadeTex = var_v0 < 1;
            if (D_800973EC != 0 && !(drawMode & DRAW_MENU)) {
                continue;
            }
            if (prim->x0 < -512 || prim->x0 > 512) {
                continue;
            }
            if (prim->y0 < -512 || prim->y0 > 512) {
                continue;
            }
            if (drawMode & DRAW_UNK_400) {
                dtd = 1;
                if (var_a1 == 0) {
                    SetDrawMode(r->dr, 0, 0, 0, rect);
                    addPrim(&r->ot[prim->priority], r->dr);
                    r->dr++;
                    g_GpuUsage.drawModes++;
                }
            } else {
                dtd = 0;
            }
            switch (primType) {
            case PRIM_TILE:
            case PRIM_TILE_ALT:
                setTile(&primbuf->tile);
                if (g_GpuUsage.tile < MAX_TILE_COUNT) {
                    if (prim->drawMode & DRAW_TRANSP) {
                        setSemiTrans(&primbuf->tile, true);
                    }
                    primbuf->tile.r0 = prim->r0;
                    primbuf->tile.g0 = prim->g0;
                    primbuf->tile.b0 = prim->b0;
                    if (prim->drawMode & (DRAW_ABSPOS | DRAW_MENU)) {
                        primbuf->tile.x0 = prim->x0;
                        primbuf->tile.y0 = prim->y0;
                    } else {
                        primbuf->tile.x0 = prim->x0 + g_backbufferX;
                        primbuf->tile.y0 = prim->y0 + g_backbufferY;
                    }
                    primbuf->tile.w = prim->u0;
                    primbuf->tile.h = prim->v0;
                    __builtin_memcpy(r->tile, &primbuf->tile, sizeof(TILE));
                    addPrim(&r->ot[prim->priority], r->tile);
                    r->tile++;
                    g_GpuUsage.tile++;
                    if ((primType & 0x10) == 0 &&
                        g_GpuUsage.drawModes < MAX_DRAW_MODES) {
                        tpage = prim->drawMode & 0x60;
                        SetDrawMode(r->dr, 0, dtd, tpage, rect);
                        addPrim(&r->ot[prim->priority], r->dr);
                        r->dr++;
                        g_GpuUsage.drawModes++;
                    }
                }
                break;
            case 2:
            case 18:
                setLineG2(&primbuf->g2);
                if (g_GpuUsage.line < MAX_LINE_G2_COUNT) {
                    if (prim->drawMode & DRAW_TRANSP) {
                        setSemiTrans(&primbuf->g2, true);
                    }
                    setShadeTex(&primbuf->g2, !!useShadeTex);
                    primbuf->g2.r0 = prim->r0;
                    primbuf->g2.g0 = prim->g0;
                    primbuf->g2.b0 = prim->b0;
                    primbuf->g2.r1 = prim->r1;
                    primbuf->g2.g1 = prim->g1;
                    primbuf->g2.b1 = prim->b1;
                    if (prim->drawMode & (DRAW_ABSPOS | DRAW_MENU)) {
                        primbuf->g2.x0 = prim->x0;
                        primbuf->g2.y0 = prim->y0;
                        primbuf->g2.x1 = prim->x1;
                        primbuf->g2.y1 = prim->y1;
                    } else {
                        primbuf->g2.x0 = prim->x0 + g_backbufferX;
                        primbuf->g2.y0 = prim->y0 + g_backbufferY;
                        primbuf->g2.x1 = prim->x1 + g_backbufferX;
                        primbuf->g2.y1 = prim->y1 + g_backbufferY;
                    }
                    __builtin_memcpy(r->g2, &primbuf->g2, sizeof(LINE_G2));
                    addPrim(&r->ot[prim->priority], r->g2);
                    r->g2++;
                    g_GpuUsage.line++;
                    if ((primType & 0x10) == 0 &&
                        g_GpuUsage.drawModes < MAX_DRAW_MODES) {
                        tpage = prim->drawMode & 0x60;
                        SetDrawMode(r->dr, 0, dtd, tpage, rect);
                        addPrim(&r->ot[prim->priority], r->dr);
                        r->dr++;
                        g_GpuUsage.drawModes++;
                    }
                }
                break;
            case PRIM_G4:
            case 19:
                setPolyG4(&primbuf->g4);
                if (g_GpuUsage.g4 < MAX_POLY_G4_COUNT) {
                    if (prim->drawMode & DRAW_TRANSP) {
                        setSemiTrans(&primbuf->g4, true);
                    }
                    setShadeTex(&primbuf->g4, !!useShadeTex);
                    primbuf->g4.r0 = prim->r0;
                    primbuf->g4.g0 = prim->g0;
                    primbuf->g4.b0 = prim->b0;
                    primbuf->g4.r1 = prim->r1;
                    primbuf->g4.g1 = prim->g1;
                    primbuf->g4.b1 = prim->b1;
                    primbuf->g4.r2 = prim->r2;
                    primbuf->g4.g2 = prim->g2;
                    primbuf->g4.b2 = prim->b2;
                    primbuf->g4.r3 = prim->r3;
                    primbuf->g4.g3 = prim->g3;
                    primbuf->g4.b3 = prim->b3;
                    if (prim->drawMode & (DRAW_ABSPOS | DRAW_MENU)) {
                        primbuf->g4.x0 = prim->x0;
                        primbuf->g4.y0 = prim->y0;
                        primbuf->g4.x1 = prim->x1;
                        primbuf->g4.y1 = prim->y1;
                        primbuf->g4.x2 = prim->x2;
                        primbuf->g4.y2 = prim->y2;
                        primbuf->g4.x3 = prim->x3;
                        primbuf->g4.y3 = prim->y3;
                    } else {
                        primbuf->g4.x0 = prim->x0 + g_backbufferX;
                        primbuf->g4.y0 = prim->y0 + g_backbufferY;
                        primbuf->g4.x1 = prim->x1 + g_backbufferX;
                        primbuf->g4.y1 = prim->y1 + g_backbufferY;
                        primbuf->g4.x2 = prim->x2 + g_backbufferX;
                        primbuf->g4.y2 = prim->y2 + g_backbufferY;
                        primbuf->g4.x3 = prim->x3 + g_backbufferX;
                        primbuf->g4.y3 = prim->y3 + g_backbufferY;
                    }
                    __builtin_memcpy(r->g4, &primbuf->g4, sizeof(POLY_G4));
                    addPrim(&r->ot[prim->priority], r->g4);
                    r->g4++;
                    g_GpuUsage.g4++;
                    if ((primType & 0x10) == 0 &&
                        g_GpuUsage.drawModes < MAX_DRAW_MODES) {
                        tpage = prim->drawMode & 0x60;
                        SetDrawMode(r->dr, 0, dtd, tpage, rect);
                        addPrim(&r->ot[prim->priority], r->dr);
                        r->dr++;
                        g_GpuUsage.drawModes++;
                    }
                }
                break;
            case PRIM_GT4:
                setPolyGT4(&primbuf->gt4);
                if (g_GpuUsage.gt4 < MAX_POLY_GT4_COUNT) {
                    if (prim->drawMode & DRAW_TRANSP) {
                        setSemiTrans(&primbuf->gt4, true);
                    }
                    setShadeTex(&primbuf->gt4, !!useShadeTex);
                    primbuf->gt4.r0 = prim->r0;
                    primbuf->gt4.g0 = prim->g0;
                    primbuf->gt4.b0 = prim->b0;
                    primbuf->gt4.r1 = prim->r1;
                    primbuf->gt4.g1 = prim->g1;
                    primbuf->gt4.b1 = prim->b1;
                    primbuf->gt4.r2 = prim->r2;
                    primbuf->gt4.g2 = prim->g2;
                    primbuf->gt4.b2 = prim->b2;
                    primbuf->gt4.r3 = prim->r3;
                    primbuf->gt4.g3 = prim->g3;
                    primbuf->gt4.b3 = prim->b3;
                    if (prim->drawMode & (DRAW_ABSPOS | DRAW_MENU)) {
                        primbuf->gt4.x0 = prim->x0;
                        primbuf->gt4.y0 = prim->y0;
                        primbuf->gt4.x1 = prim->x1;
                        primbuf->gt4.y1 = prim->y1;
                        primbuf->gt4.x2 = prim->x2;
                        primbuf->gt4.y2 = prim->y2;
                        primbuf->gt4.x3 = prim->x3;
                        primbuf->gt4.y3 = prim->y3;
                    } else {
                        primbuf->gt4.x0 = prim->x0 + g_backbufferX;
                        primbuf->gt4.y0 = prim->y0 + g_backbufferY;
                        primbuf->gt4.x1 = prim->x1 + g_backbufferX;
                        primbuf->gt4.y1 = prim->y1 + g_backbufferY;
                        primbuf->gt4.x2 = prim->x2 + g_backbufferX;
                        primbuf->gt4.y2 = prim->y2 + g_backbufferY;
                        primbuf->gt4.x3 = prim->x3 + g_backbufferX;
                        primbuf->gt4.y3 = prim->y3 + g_backbufferY;
                    }
                    primbuf->gt4.u0 = prim->u0;
                    primbuf->gt4.v0 = prim->v0;
                    primbuf->gt4.u1 = prim->u1;
                    primbuf->gt4.v1 = prim->v1;
                    primbuf->gt4.u2 = prim->u2;
                    primbuf->gt4.v2 = prim->v2;
                    primbuf->gt4.u3 = prim->u3;
                    primbuf->gt4.v3 = prim->v3;
                    primbuf->gt4.tpage = prim->tpage + (prim->drawMode & 0x60);
                    primbuf->gt4.clut = D_8003C104[prim->clut];
                    __builtin_memcpy(r->gt4, &primbuf->gt4, sizeof(POLY_GT4));
                    addPrim(&r->ot[prim->priority], r->gt4);
                    r->gt4++;
                    g_GpuUsage.gt4++;
                    if (dtd && g_GpuUsage.drawModes < MAX_DRAW_MODES) {
                        tpage = 0;
                        SetDrawMode(r->dr, 0, dtd, tpage, rect);
                        addPrim(&r->ot[prim->priority], r->dr);
                        r->dr++;
                        g_GpuUsage.drawModes++;
                    }
                }
                break;
            case PRIM_GT3:
                setPolyGT3(&primbuf->gt3);
                if (g_GpuUsage.gt3 < MAX_POLY_GT3_COUNT) {
                    if (prim->drawMode & DRAW_TRANSP) {
                        setSemiTrans(&primbuf->gt3, true);
                    }
                    setShadeTex(&primbuf->gt3, !!useShadeTex);
                    primbuf->gt3.r0 = prim->r0;
                    primbuf->gt3.g0 = prim->g0;
                    primbuf->gt3.b0 = prim->b0;
                    primbuf->gt3.r1 = prim->r1;
                    primbuf->gt3.g1 = prim->g1;
                    primbuf->gt3.b1 = prim->b1;
                    primbuf->gt3.r2 = prim->r2;
                    primbuf->gt3.g2 = prim->g2;
                    primbuf->gt3.b2 = prim->b2;
                    if (prim->drawMode & (DRAW_ABSPOS | DRAW_MENU)) {
                        primbuf->gt3.x0 = prim->x0;
                        primbuf->gt3.y0 = prim->y0;
                        primbuf->gt3.x1 = prim->x1;
                        primbuf->gt3.y1 = prim->y1;
                        primbuf->gt3.x2 = prim->x2;
                        primbuf->gt3.y2 = prim->y2;
                    } else {
                        primbuf->gt3.x0 = prim->x0 + g_backbufferX;
                        primbuf->gt3.y0 = prim->y0 + g_backbufferY;
                        primbuf->gt3.x1 = prim->x1 + g_backbufferX;
                        primbuf->gt3.y1 = prim->y1 + g_backbufferY;
                        primbuf->gt3.x2 = prim->x2 + g_backbufferX;
                        primbuf->gt3.y2 = prim->y2 + g_backbufferY;
                    }
                    primbuf->gt3.u0 = prim->u0;
                    primbuf->gt3.v0 = prim->v0;
                    primbuf->gt3.u1 = prim->u1;
                    primbuf->gt3.v1 = prim->v1;
                    primbuf->gt3.u2 = prim->u2;
                    primbuf->gt3.v2 = prim->v2;
                    primbuf->gt3.tpage = prim->tpage + (prim->drawMode & 0x60);
                    primbuf->gt3.clut = D_8003C104[prim->clut];

                    __builtin_memcpy(r->gt3, &primbuf->gt3, sizeof(POLY_GT3));
                    addPrim(&r->ot[prim->priority], r->gt3);
                    r->gt3++;
                    g_GpuUsage.gt3++;
                    if (dtd && g_GpuUsage.drawModes < MAX_DRAW_MODES) {
                        tpage = 0;
                        SetDrawMode(r->dr, 0, dtd, tpage, rect);
                        addPrim(&r->ot[prim->priority], r->dr);
                        r->dr++;
                        g_GpuUsage.drawModes++;
                    }
                }
                break;
            case PRIM_SPRT:
            case 22:
                setSprt(&primbuf->sprt);
                if (g_GpuUsage.sp < MAX_SPRT_COUNT) {
                    if (prim->drawMode & DRAW_TRANSP) {
                        setSemiTrans(&primbuf->sprt, true);
                    }
                    setShadeTex(&primbuf->sprt, !!useShadeTex);
                    primbuf->sprt.r0 = prim->r0;
                    primbuf->sprt.g0 = prim->g0;
                    primbuf->sprt.b0 = prim->b0;
                    if (prim->drawMode & (DRAW_ABSPOS | DRAW_MENU)) {
                        primbuf->sprt.x0 = prim->x0;
                        primbuf->sprt.y0 = prim->y0;
                    } else {
                        primbuf->sprt.x0 = prim->x0 + g_backbufferX;
                        primbuf->sprt.y0 = prim->y0 + g_backbufferY;
                    }
                    primbuf->sprt.u0 = prim->u0;
                    primbuf->sprt.v0 = prim->v0;
                    primbuf->sprt.w = prim->u1;
                    primbuf->sprt.h = prim->v1;
                    primbuf->sprt.clut = D_8003C104[prim->clut];
                    __builtin_memcpy(r->sprt, &primbuf->sprt, sizeof(SPRT));
                    addPrim(&r->ot[prim->priority], r->sprt);
                    r->sprt++;
                    g_GpuUsage.sp++;
                    if ((primType & 0x10) == 0 &&
                        g_GpuUsage.drawModes < MAX_DRAW_MODES) {
                        tpage = prim->tpage + (prim->drawMode & 0x60);
                        SetDrawMode(r->dr, 0, dtd, tpage, rect);
                        addPrim(&r->ot[prim->priority], r->dr);
                        r->dr++;
                        g_GpuUsage.drawModes++;
                    }
                }
                break;
            case PRIM_ENV:
                if (g_GpuUsage.env < MAX_ENV_COUNT) {
                    env = *(DR_ENV**)&prim->r1;
                    if (prim->drawMode & DRAW_UNK_1000) {
                        sp80 = g_CurrentBuffer->draw.ofs[0];
                        *(s16*)&env->code[2] = sp80;
                        *(s16*)&env->code[7] = sp80;
                    }
                    if (prim->drawMode & DRAW_UNK_800) {
                        __builtin_memcpy(
                            &sp18, &g_CurrentBuffer->draw, sizeof(DRAWENV));
                        sp18.isbg = 0;
                        SetDrawEnv(env, &sp18);
                    }
                    __builtin_memcpy(r->env, env, sizeof(DR_ENV));
                    if (prim->drawMode & DRAW_UNK_1000) {
                        env = r->env;
                        *(s16*)&env->code[0] += sp80;
                        *(s16*)&env->code[1] += sp80;
                    }
                    addPrim(&r->ot[prim->priority], r->env);
                    r->env++;
                    g_GpuUsage.env++;
                }
                break;
            }
            var_v0 = primType & 0x10;
            var_a1 = var_v0 != 0;
        }
    }
}
#endif
