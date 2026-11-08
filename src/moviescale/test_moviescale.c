/* Host test of the scaler. Writes before/after BMP files for a look by eye.
 * Usage: test_moviescale <output dir> */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "moviescale_core.h"

#define SW 640
#define SH 272
#define GUARD 0x5A5A
static u32 ra[4096], rb[4096];
static int xi[4096];
static int fails;
#define CHECK(c, ...) do { if (!(c)) { fails++; printf("FAIL: " __VA_ARGS__); printf("\n"); } } while (0)

static u16 rgb(int r, int g, int b, int is565)
{
    return is565 ? (u16)((r >> 3) << 11 | (g >> 2) << 5 | (b >> 3))
                 : (u16)((r >> 3) << 10 | (g >> 3) << 5 | (b >> 3));
}

static void bmp(const char *dir, const char *name, const u16 *p, int w, int h, int pitchpx)
{
    char path[512]; snprintf(path, sizeof path, "%s/%s", dir, name);
    FILE *f = fopen(path, "wb");
    int row = w * 3 + (4 - (w * 3) % 4) % 4, size = 54 + row * h;
    unsigned char hd[54] = {'B','M'};
    *(int *)(hd + 2) = size; *(int *)(hd + 10) = 54; *(int *)(hd + 14) = 40;
    *(int *)(hd + 18) = w; *(int *)(hd + 22) = h; *(short *)(hd + 26) = 1; *(short *)(hd + 28) = 24;
    fwrite(hd, 1, 54, f);
    unsigned char *l = calloc(row, 1);
    for (int y = h - 1; y >= 0; y--) {
        for (int x = 0; x < w; x++) {
            u16 v = p[y * pitchpx + x];
            l[x*3] = (v & 31) << 3; l[x*3+1] = ((v >> 5) & 63) << 2; l[x*3+2] = (v >> 11) << 3;
        }
        fwrite(l, 1, row, f);
    }
    free(l); fclose(f);
}

/* A busy test picture: gradient, diagonal lines, a hard-edged shape, one-pixel detail. */
static void picture(u16 *s, int is565)
{
    for (int y = 0; y < SH; y++) for (int x = 0; x < SW; x++) {
        int r = x * 255 / (SW - 1), g = y * 255 / (SH - 1), b = 96;
        if (((x + y) / 12) % 4 == 0) { r = 230; g = 200; b = 60; }
        int dx = x - 180, dy = y - 136;
        if (dx * dx + dy * dy < 80 * 80) { r = 20; g = 40; b = 200; }
        if (x > 380 && x < 600 && y > 60 && y < 210 && ((x / 9 + y / 9) & 1)) { r = 255; g = 255; b = 255; }
        if (y > 230 && y < 250 && x % 2) { r = g = b = 255; }
        s[y * SW + x] = rgb(r, g, b, is565);
    }
}

/* Runs one scale inside a guarded buffer with pitches wider than the picture. */
static u16 *run(const u16 *src, int spitchpx, int dw, int dh, u32 m, int *pitch_out)
{
    int dpitch = dw + 37;
    u16 *d = malloc((dh + 4) * dpitch * 2);
    for (int i = 0; i < (dh + 4) * dpitch; i++) d[i] = GUARD;
    u16 *o = d + 2 * dpitch + 11;
    scale_bilinear((const unsigned char *)src, spitchpx * 2, SW, SH, (unsigned char *)o, dpitch * 2, dw, dh, m, ra, rb, xi);
    int bad = 0;
    for (int y = 0; y < dh + 4; y++) for (int x = 0; x < dpitch; x++) {
        int in = y >= 2 && y < dh + 2 && x >= 11 && x < 11 + dw;
        if (!in && d[y * dpitch + x] != GUARD) bad++;
    }
    CHECK(bad == 0, "%dx%d wrote %d pixels outside its rectangle", dw, dh, bad);
    *pitch_out = dpitch;
    return d;
}

int main(int argc, char **argv)
{
    const char *dir = argc > 1 ? argv[1] : ".";
    static u16 src[SH * (SW + 13)];
    static const int sizes[][2] = {{1920,816},{2560,1088},{3840,1632},{1280,544},{640,272},{320,136},{1707,725},{1440,612}};

    for (int fmt = 0; fmt < 2; fmt++) {
        int is565 = fmt == 0; u32 m = is565 ? MASK_565 : MASK_555;
        /* Solid colours must come out unchanged at every size. */
        static const int cols[][3] = {{0,0,0},{255,255,255},{255,0,0},{0,255,0},{0,0,255},{123,45,200}};
        for (unsigned c = 0; c < 6; c++) for (unsigned z = 0; z < 8; z++) {
            u16 v = rgb(cols[c][0], cols[c][1], cols[c][2], is565);
            for (int i = 0; i < SH * (SW + 13); i++) src[i] = v;
            int dw = sizes[z][0], dh = sizes[z][1], dp;
            u16 *d = run(src, SW + 13, dw, dh, m, &dp), *o = d + 2 * dp + 11;
            int bad = 0;
            for (int y = 0; y < dh; y++) for (int x = 0; x < dw; x++) if (o[y * dp + x] != v) bad++;
            CHECK(bad == 0, "fmt %d colour %u at %dx%d: %d pixels changed", fmt, c, dw, dh, bad);
            free(d);
        }
        /* The same size must be an exact copy. */
        for (int i = 0; i < SH * (SW + 13); i++) src[i] = 0;
        { static u16 p[SH * SW]; picture(p, is565);
          for (int y = 0; y < SH; y++) memcpy(src + y * (SW + 13), p + y * SW, SW * 2);
          int dp; u16 *d = run(src, SW + 13, SW, SH, m, &dp), *o = d + 2 * dp + 11;
          int bad = 0;
          for (int y = 0; y < SH; y++) for (int x = 0; x < SW; x++) if (o[y * dp + x] != p[y * SW + x]) bad++;
          CHECK(bad == 0, "fmt %d 1:1 copy differs in %d pixels", fmt, bad);
          free(d);
          /* Every size: no crash or overrun on the real shape of picture. */
          for (unsigned z = 0; z < 8; z++) { d = run(src, SW + 13, sizes[z][0], sizes[z][1], m, &dp); free(d); }
        }
    }

    /* A ramp must stay in order when enlarged. */
    { for (int y = 0; y < SH; y++) for (int x = 0; x < SW + 13; x++) src[y * (SW + 13) + x] = (u16)((x * 31 / (SW - 1) > 31 ? 31 : x * 31 / (SW - 1)) << 11);
      int dp; u16 *d = run(src, SW + 13, 1920, 816, MASK_565, &dp), *o = d + 2 * dp + 11;
      int bad = 0;
      for (int x = 1; x < 1920; x++) { int a = o[500 * dp + x - 1] >> 11, b = o[500 * dp + x] >> 11; if (b < a) bad++; }
      CHECK(bad == 0, "ramp went backwards %d times", bad);
      CHECK(o[500 * dp] >> 11 == 0 && o[500 * dp + 1919] >> 11 == 31, "ramp lost its end values");
      free(d); }

    /* Before and after pictures, nearest-neighbour being what the blocky stretch looks like. */
    { static u16 p[SH * SW]; picture(p, 1);
      int dp; u16 *d = run(p, SW, 1920, 816, MASK_565, &dp), *o = d + 2 * dp + 11;
      u16 *n = malloc(1920 * 816 * 2);
      for (int y = 0; y < 816; y++) for (int x = 0; x < 1920; x++) n[y * 1920 + x] = p[(y / 3) * SW + x / 3];
      int softened = 0, hardn = 0;
      for (int y = 0; y < 816; y++) for (int x = 1; x < 1920; x++) {
          int ga = (o[y * dp + x - 1] >> 5) & 63, gb = (o[y * dp + x] >> 5) & 63, na = (n[y * 1920 + x - 1] >> 5) & 63, nb = (n[y * 1920 + x] >> 5) & 63;
          if (abs(ga - gb) > 30) softened++;
          if (abs(na - nb) > 30) hardn++;
      }
      printf("hard edges (green jump > 30): blocky %d, smooth %d\n", hardn, softened);
      CHECK(softened < hardn / 2, "not visibly smoother");
      bmp(dir, "smooth.bmp", o, 1920, 816, dp);
      bmp(dir, "blocky.bmp", n, 1920, 816, 1920);
      clock_t t = clock(); int frames = 200;
      for (int i = 0; i < frames; i++) scale_bilinear((const unsigned char *)p, SW * 2, SW, SH, (unsigned char *)o, dp * 2, 1920, 816, MASK_565, ra, rb, xi);
      printf("speed at 1920x816 (64-bit test build): %.1f ms per frame\n", (double)(clock() - t) * 1000 / CLOCKS_PER_SEC / frames);
      free(n); free(d); }

    printf(fails ? "%d FAILED\n" : "ALL PASS\n", fails);
    return fails != 0;
}
