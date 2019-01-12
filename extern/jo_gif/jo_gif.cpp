// modified version of https://www.jonolick.com/home/gif-writer
/* public domain, Simple, Minimalistic GIF writer - http://jonolick.com
 *
 * Quick Notes:
 * 	Supports only 4 component input, alpha is used. (RGBA)
 *
 * Latest revisions:
 *      modified version (2019) 
 * 	1.00 (2015-11-03) initial release
 *
 * Basic usage:
 *	char *frame = new char[128*128*4]; // 4 component. RGBA format
 *	jo_gif_t gif = jo_gif_start("foo.gif", 128, 128, 0, 32);
 *	jo_gif_frame(&gif, frame, 4, false); // frame 1
 *	jo_gif_frame(&gif, frame, 4, false); // frame 2
 *	jo_gif_frame(&gif, frame, 4, false); // frame 3, ...
 *	jo_gif_end(&gif);
 * */

#ifndef JO_INCLUDE_GIF_H
#define JO_INCLUDE_GIF_H

#include <stdio.h>

// To get a header file for this, either cut and paste the header,
// or create jo_gif.h, #define JO_GIF_HEADER_FILE_ONLY, and
// then include jo_gif.cpp from it.

typedef struct {
	FILE *fp;
	unsigned char palette[0x300];
	short width, height, repeat;
	int numColors, palSize;
	int frame;
} jo_gif_t;

// width/height	| the same for every frame
// repeat       | 0 = loop forever, 1 = loop once, etc...
// palSize	| must be power of 2 - 1. so, 255 not 256.
extern jo_gif_t jo_gif_start(const char *filename, short width, short height, short repeat, int palSize);

// gif		| the state (returned from jo_gif_start)
// rgba         | the pixels
// delayCsec    | amount of time in between frames (in centiseconds)
// localPalette | true if you want a unique palette generated for this frame (does not effect future frames)
// tc_r         | red value for the color to be used as transparency
// tc_g         | green value for the color to be used as transparency
// tc_b         | blue value for the color to be used as transparency
extern void jo_gif_frame(jo_gif_t *gif, unsigned char *rgba, short delayCsec, bool localPalette, unsigned char tc_r, unsigned char tc_g, unsigned char tc_b);

// gif          | the state (returned from jo_gif_start)
extern void jo_gif_end(jo_gif_t *gif);

#endif

#ifndef JO_GIF_HEADER_FILE_ONLY

#if defined(_MSC_VER) && _MSC_VER >= 0x1400
#define _CRT_SECURE_NO_WARNINGS // suppress warnings about fopen()
#endif

#include <stdlib.h>
#include <memory.h>
#include <math.h>

bool in_palette(unsigned char *palette, int pal_size, unsigned char *color) {
    for (int i = 0; i < pal_size; i++) {
        if (palette[i * 3 + 0] == color[0] &&
            palette[i * 3 + 1] == color[1] &&
            palette[i * 3 + 2] == color[2]) {
            return true;
        }
    }
    
    return false;
}

int get_index(unsigned char *palette, int pal_size, unsigned char *color) {
    for (int i = 0; i < pal_size; i++) {
        if (palette[i*3+0] == color[0] &&
            palette[i*3+1] == color[1] &&
            palette[i*3+2] == color[2]) {
            return i;
        }
    }

    printf("index not found for color: %x|%x|%x\n", color[0], color[1], color[2]);
    printf("using transparent index as a fallback\n");
    return 0;
}

static void index_pixels(unsigned char *rgba, int img_size, unsigned char *palette, int pal_size, unsigned char *indexedPixels) {
    for(int k = 0; k < img_size*4; k+=4) {
        // if pixel is fully transparent, assign it 0 (transparency index)
        if (rgba[k+3] == 0x00) {
            indexedPixels[k/4] = 0;
        }
        
        // else assign its palette index
        else {
            unsigned char color[3] = {rgba[k+0], rgba[k+1], rgba[k+2]};
            int index = get_index(palette, pal_size, color);
            indexedPixels[k/4] = index;
        }
    }
}

static void make_palette(int img_size, unsigned char *rgba, unsigned char *palette, int final_pal_size, unsigned char *trans_color) {
    // set first color of palette as transparent
    palette[0] = trans_color[0];
    palette[1] = trans_color[1];
    palette[2] = trans_color[2];
    
    // walk through image and assign all colors to palette
    int pal_size = 1;
    for(int k = 0; k < img_size*4; k+=4) {
        unsigned char color[3] = {rgba[k+0], rgba[k+1], rgba[k+2]};
        if (!in_palette(palette, pal_size, color)) {
            palette[pal_size * 3 + 0] = color[0];
            palette[pal_size * 3 + 1] = color[1];
            palette[pal_size * 3 + 2] = color[2];
            pal_size++;
        }
    }
    
    // fill any leftover spots in the palette with black
    for (; pal_size < final_pal_size; pal_size++) {
        palette[pal_size * 3 + 0] = 0x00;
        palette[pal_size * 3 + 1] = 0x00;
        palette[pal_size * 3 + 2] = 0x00;
    }
}

typedef struct {
	FILE *fp;
	int numBits;
	unsigned char buf[256];
	unsigned char idx;
	unsigned tmp;
	int outBits;
	int curBits;
} jo_gif_lzw_t;

static void jo_gif_lzw_write(jo_gif_lzw_t *s, int code) {
	s->outBits |= code << s->curBits;
	s->curBits += s->numBits;
	while(s->curBits >= 8) {
		s->buf[s->idx++] = s->outBits & 255;
		s->outBits >>= 8;
		s->curBits -= 8;
		if (s->idx >= 255) {
			putc(s->idx, s->fp);
			fwrite(s->buf, s->idx, 1, s->fp);
			s->idx = 0;
		}
	}
}

static void jo_gif_lzw_encode(unsigned char *in, int len, FILE *fp) {
	jo_gif_lzw_t state = {fp, 9};
	int maxcode = 511;

	// Note: 30k stack space for dictionary =|
	const int hashSize = 5003;
	short codetab[hashSize];
	int hashTbl[hashSize];
	memset(hashTbl, 0xFF, sizeof(hashTbl));

	jo_gif_lzw_write(&state, 0x100);

	int free_ent = 0x102;
	int ent = *in++;
CONTINUE:
	while (--len) {
		int c = *in++;
		int fcode = (c << 12) + ent;
		int key = (c << 4) ^ ent; // xor hashing
		while(hashTbl[key] >= 0) {
			if(hashTbl[key] == fcode) {
				ent = codetab[key];
				goto CONTINUE;
			}
			++key;
			key = key >= hashSize ? key - hashSize : key;
		}
		jo_gif_lzw_write(&state, ent);
		ent = c;
		if(free_ent < 4096) {
			if(free_ent > maxcode) {
				++state.numBits;
				if(state.numBits == 12) {
					maxcode = 4096;
				} else {
					maxcode = (1<<state.numBits)-1;
				}
			}
			codetab[key] = free_ent++;
			hashTbl[key] = fcode;
		} else {
			memset(hashTbl, 0xFF, sizeof(hashTbl));
			free_ent = 0x102;
			jo_gif_lzw_write(&state, 0x100);
			state.numBits = 9;
			maxcode = 511;
		}
	}
	jo_gif_lzw_write(&state, ent);
	jo_gif_lzw_write(&state, 0x101);
	jo_gif_lzw_write(&state, 0);
	if(state.idx) {
		putc(state.idx, fp);
		fwrite(state.buf, state.idx, 1, fp);
	}
}

jo_gif_t jo_gif_start(const char *filename, short width, short height, short repeat, int numColors) {
	numColors = numColors > 255 ? 255 : numColors < 2 ? 2 : numColors;
	jo_gif_t gif = {};
	gif.width = width;
	gif.height = height;
	gif.repeat = repeat;
	gif.numColors = numColors;
	gif.palSize = log2(numColors);

	gif.fp = fopen(filename, "wb");
	if(!gif.fp) {
		printf("Error: Could not WriteGif to %s\n", filename);
		return gif;
	}

	fwrite("GIF89a", 6, 1, gif.fp);
	// Logical Screen Descriptor
	fwrite(&gif.width, 2, 1, gif.fp);
	fwrite(&gif.height, 2, 1, gif.fp);
	putc(0xF0 | gif.palSize, gif.fp);
	fwrite("\x00\x00", 2, 1, gif.fp); // bg color index (unused), aspect ratio
	return gif;
}

void jo_gif_frame(jo_gif_t *gif, unsigned char * rgba, short delayCsec, bool localPalette, unsigned char tc_r, unsigned char tc_g, unsigned char tc_b) {
	if(!gif->fp) {
		return;
	}
        
        unsigned char trans_color[3] = {tc_r, tc_g, tc_b};
        
	short width = gif->width;
	short height = gif->height;
	int img_size = width * height;
        int pal_size = gif->numColors+1;

	unsigned char localPalTbl[0x300];
	unsigned char *palette = gif->frame == 0 || !localPalette ? gif->palette : localPalTbl;
	if(gif->frame == 0 || localPalette) {
                make_palette(img_size, rgba, palette, pal_size, trans_color);
	}

	unsigned char *indexedPixels = (unsigned char *)malloc(img_size);
        index_pixels(rgba, img_size, palette, pal_size, indexedPixels);

	if(gif->frame == 0) {
		// Global Color Table
		fwrite(palette, 3*(1<<(gif->palSize+1)), 1, gif->fp);
		if(gif->repeat >= 0) {
			// Netscape Extension
			fwrite("\x21\xff\x0bNETSCAPE2.0\x03\x01", 16, 1, gif->fp);
			fwrite(&gif->repeat, 2, 1, gif->fp); // loop count (extra iterations, 0=repeat forever)
			putc(0, gif->fp); // block terminator
		}
	}
	// Graphic Control Extension
	fwrite("\x21\xf9\x04\x09", 4, 1, gif->fp); // last byte sets disposal method to replace and transparency on
	fwrite(&delayCsec, 2, 1, gif->fp); // delayCsec x 1/100 sec
	fwrite("\x00\x00", 2, 1, gif->fp); // transparent color index (first byte)
	// Image Descriptor
	fwrite("\x2c\x00\x00\x00\x00", 5, 1, gif->fp); // header, x,y
	fwrite(&width, 2, 1, gif->fp);
	fwrite(&height, 2, 1, gif->fp);
	if (gif->frame == 0 || !localPalette) {
		putc(0, gif->fp);
	} else {
		putc(0x80|gif->palSize, gif->fp );
		fwrite(palette, 3*(1<<(gif->palSize+1)), 1, gif->fp);
	}
	putc(8, gif->fp); // block terminator
	jo_gif_lzw_encode(indexedPixels, img_size, gif->fp);
	putc(0, gif->fp); // block terminator
	++gif->frame;
	free(indexedPixels);
}

void jo_gif_end(jo_gif_t *gif) {
	if(!gif->fp) {
		return;
	}
	putc(0x3b, gif->fp); // gif trailer
	fclose(gif->fp);
}
#endif
