/* Star Wars Episode I Racer: smooth (bilinear) cutscene stretching.
 *
 * The movie callback draws each frame into the 640x480 surface 004EB480, then stretches it to
 * the back buffer 00EC8E00 with IDirectDrawSurface::Blt, which looks blocky. The Blt at 00425214
 * now calls movie_present, which does that stretch itself. If a surface can't be locked or a
 * rectangle looks wrong, it makes the game's original Blt call instead.
 *
 * Built freestanding: no CRT. Lives in a section of its own at 00ED0000. */
#include "moviescale_core.h"

#define G(t, a) (*(t *)(a))
#define BACK_BUFFER G(void **, 0xEC8E00)
#define MOVIE_SURF  G(void **, 0x4EB480)
#define SRC_RECT    ((int *)0xE9F2F0)   /* left, top, right, bottom */
#define DST_RECT    ((int *)0xE9F350)

#define MAX_DST 4096
#define ROW_A   ((u32 *)0xED1000)       /* buffers in the section's zero-filled tail */
#define ROW_B   (ROW_A + MAX_DST)
#define XIDX    ((int *)(ROW_B + MAX_DST))

/* DDSURFACEDESC, in 32-bit words */
#define DESC_WORDS  31
#define D_SIZE      0
#define D_HEIGHT    2
#define D_WIDTH     3
#define D_PITCH     4
#define D_SURFACE   9
#define D_BITCOUNT  21
#define D_RMASK     22

typedef long (__attribute__((stdcall)) *lock_fn)(void *, void *, u32 *, u32, void *);
typedef long (__attribute__((stdcall)) *unlock_fn)(void *, void *);
typedef long (__attribute__((stdcall)) *blt_fn)(void *, int *, void *, int *, u32, void *);
#define VTBL(obj) (*(void ***)(obj))

static int lock_surface(void *s, u32 *d)
{
    if (!s) return 0;
    for (int i = 0; i < DESC_WORDS; i++) d[i] = 0;
    d[D_SIZE] = DESC_WORDS * 4;
    if (((lock_fn)VTBL(s)[25])(s, 0, d, 1 /* DDLOCK_WAIT */, 0) != 0) return 0;
    return 1;
}

static void unlock_surface(void *s) { ((unlock_fn)VTBL(s)[32])(s, 0); }

static int stretch(const u32 *sd, const u32 *dd)
{
    const int *r = SRC_RECT, *q = DST_RECT;
    int sw = r[2] - r[0], sh = r[3] - r[1], dw = q[2] - q[0], dh = q[3] - q[1];
    if (sd[D_BITCOUNT] != 16 || dd[D_BITCOUNT] != 16) return 0;
    if (sd[D_RMASK] != dd[D_RMASK] || (dd[D_RMASK] != 0xF800 && dd[D_RMASK] != 0x7C00)) return 0;
    if (sw <= 0 || sh <= 0 || dw <= 0 || dh <= 0 || dw > MAX_DST) return 0;
    if (r[0] < 0 || r[1] < 0 || r[2] > (int)sd[D_WIDTH] || r[3] > (int)sd[D_HEIGHT]) return 0;
    if (q[0] < 0 || q[1] < 0 || q[2] > (int)dd[D_WIDTH] || q[3] > (int)dd[D_HEIGHT]) return 0;
    if (!sd[D_SURFACE] || !dd[D_SURFACE]) return 0;

    const unsigned char *src = (const unsigned char *)sd[D_SURFACE] + r[1] * (int)sd[D_PITCH] + r[0] * 2;
    unsigned char *dst = (unsigned char *)dd[D_SURFACE] + q[1] * (int)dd[D_PITCH] + q[0] * 2;
    scale_bilinear(src, (int)sd[D_PITCH], sw, sh, dst, (int)dd[D_PITCH], dw, dh,
                   dd[D_RMASK] == 0xF800 ? MASK_565 : MASK_555, ROW_A, ROW_B, XIDX);
    return 1;
}

void movie_present(void)
{
    void *s = MOVIE_SURF, *b = BACK_BUFFER;
    u32 sd[DESC_WORDS], dd[DESC_WORDS];
    int done = 0;
    if (lock_surface(s, sd)) {
        if (lock_surface(b, dd)) {
            done = stretch(sd, dd);
            unlock_surface(b);
        }
        unlock_surface(s);
    }
    if (!done)
        ((blt_fn)VTBL(b)[5])(b, DST_RECT, s, SRC_RECT, 0x1000000 /* DDBLT_WAIT */, 0);
}
