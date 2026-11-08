/* Bilinear scaling of a 16-bit picture. Shared by the game code (moviescale.c) and the test
 * (test_moviescale.c). Pixels are widened to 32 bits with spare bits above each channel so a
 * channel can be multiplied by a 5-bit weight without reaching its neighbour. */
typedef unsigned int u32;
typedef unsigned short u16;

#define MASK_565 0x07E0F81Fu
#define MASK_555 0x03E07C1Fu

static inline u32 spread(u32 p, u32 m) { return (p | (p << 16)) & m; }
static inline u32 mix(u32 a, u32 b, u32 w, u32 m) { return ((a * (32 - w) + b * w) >> 5) & m; }

static void hrow(const u16 *s, u32 *out, int dw, const int *xidx, u32 m)
{
    for (int i = 0; i < dw; i++) {
        int e = xidx[i], x = e & 0xFFFF;
        u32 w = (u32)e >> 16;
        out[i] = mix(spread(s[x], m), spread(s[x + (w ? 1 : 0)], m), w, m);
    }
}

/* Writes dw x dh pixels at dst. row_a, row_b and xidx each hold at least dw entries. */
static void scale_bilinear(const unsigned char *src, int spitch, int sw, int sh,
                           unsigned char *dst, int dpitch, int dw, int dh, u32 m,
                           u32 *row_a, u32 *row_b, int *xidx)
{
    int stepx = (sw << 16) / dw, stepy = (sh << 16) / dh;
    for (int i = 0; i < dw; i++) {
        int f = i * stepx + stepx / 2 - 0x8000;
        if (f < 0) f = 0;
        int x = f >> 16, w = (f >> 11) & 31;
        if (x >= sw - 1) { x = sw - 1; w = 0; }
        xidx[i] = x | (w << 16);
    }
    int ya = -1, yb = -1;
    for (int j = 0; j < dh; j++) {
        int f = j * stepy + stepy / 2 - 0x8000;
        if (f < 0) f = 0;
        int y = f >> 16;
        u32 w = (f >> 11) & 31;
        if (y >= sh - 1) { y = sh - 1; w = 0; }
        int y2 = w ? y + 1 : y;
        if (ya != y) {
            if (yb == y) { u32 *t = row_a; row_a = row_b; row_b = t; yb = -1; }
            else hrow((const u16 *)(src + y * spitch), row_a, dw, xidx, m);
            ya = y;
        }
        if (yb != y2) { hrow((const u16 *)(src + y2 * spitch), row_b, dw, xidx, m); yb = y2; }
        u16 *o = (u16 *)(dst + j * dpitch);
        for (int x = 0; x < dw; x++) {
            u32 v = mix(row_a[x], row_b[x], w, m);
            o[x] = (u16)(v | (v >> 16));
        }
    }
}
