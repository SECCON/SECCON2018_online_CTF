/*
 * QEMU Cydf CLGD 54xx VGA Emulator.
 *
 * Copyright (c) 2004 Fabrice Bellard
 * Copyright (c) 2004 Makoto Suzuki (suzu)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
/*
 * Reference: Finn Thogersons' VGADOC4b
 *   available at http://home.worldonline.dk/~finth/
 */
#include "qemu/osdep.h"
#include "qemu/units.h"
#include "qapi/error.h"
#include "trace.h"
#include "hw/hw.h"
#include "hw/pci/pci.h"
#include "ui/pixel_ops.h"
#include "vga_int.h"
#include "hw/loader.h"

/*
 * TODO:
 *    - destination write mask support not complete (bits 5..7)
 *    - optimize linear mappings
 *    - optimize bitblt functions
 */

//#define DEBUG_CYDF
//#define DEBUG_BITBLT

/***************************************
 *
 *  definitions
 *
 ***************************************/

// ID
#define CYDF_ID_CLGD5422  (0x23<<2)
#define CYDF_ID_CLGD5426  (0x24<<2)
#define CYDF_ID_CLGD5424  (0x25<<2)
#define CYDF_ID_CLGD5428  (0x26<<2)
#define CYDF_ID_CLGD5430  (0x28<<2)
#define CYDF_ID_CLGD5434  (0x2A<<2)
#define CYDF_ID_CLGD5436  (0x2B<<2)
#define CYDF_ID_CLGD5446  (0x2E<<2)

// CYDF
#define CYDF_VULN_IDX   0xcc
#define CYDF_X          0xcd
#define CYDF_Y          0xce

// sequencer 0x07
#define CYDF_SR7_BPP_VGA            0x00
#define CYDF_SR7_BPP_SVGA           0x01
#define CYDF_SR7_BPP_MASK           0x0e
#define CYDF_SR7_BPP_8              0x00
#define CYDF_SR7_BPP_16_DOUBLEVCLK  0x02
#define CYDF_SR7_BPP_24             0x04
#define CYDF_SR7_BPP_16             0x06
#define CYDF_SR7_BPP_32             0x08
#define CYDF_SR7_ISAADDR_MASK       0xe0

// sequencer 0x0f
#define CYDF_MEMSIZE_512k        0x08
#define CYDF_MEMSIZE_1M          0x10
#define CYDF_MEMSIZE_2M          0x18
#define CYDF_MEMFLAGS_BANKSWITCH 0x80	// bank switching is enabled.

// sequencer 0x12
#define CYDF_CURSOR_SHOW         0x01
#define CYDF_CURSOR_HIDDENPEL    0x02
#define CYDF_CURSOR_LARGE        0x04	// 64x64 if set, 32x32 if clear

// sequencer 0x17
#define CYDF_BUSTYPE_VLBFAST   0x10
#define CYDF_BUSTYPE_PCI       0x20
#define CYDF_BUSTYPE_VLBSLOW   0x30
#define CYDF_BUSTYPE_ISA       0x38
#define CYDF_MMIO_ENABLE       0x04
#define CYDF_MMIO_USE_PCIADDR  0x40	// 0xb8000 if cleared.
#define CYDF_MEMSIZEEXT_DOUBLE 0x80

// control 0x0b
#define CYDF_BANKING_DUAL             0x01
#define CYDF_BANKING_GRANULARITY_16K  0x20	// set:16k, clear:4k

// control 0x30
#define CYDF_BLTMODE_BACKWARDS        0x01
#define CYDF_BLTMODE_MEMSYSDEST       0x02
#define CYDF_BLTMODE_MEMSYSSRC        0x04
#define CYDF_BLTMODE_TRANSPARENTCOMP  0x08
#define CYDF_BLTMODE_PATTERNCOPY      0x40
#define CYDF_BLTMODE_COLOREXPAND      0x80
#define CYDF_BLTMODE_PIXELWIDTHMASK   0x30
#define CYDF_BLTMODE_PIXELWIDTH8      0x00
#define CYDF_BLTMODE_PIXELWIDTH16     0x10
#define CYDF_BLTMODE_PIXELWIDTH24     0x20
#define CYDF_BLTMODE_PIXELWIDTH32     0x30

// control 0x31
#define CYDF_BLT_BUSY                 0x01
#define CYDF_BLT_START                0x02
#define CYDF_BLT_RESET                0x04
#define CYDF_BLT_FIFOUSED             0x10
#define CYDF_BLT_AUTOSTART            0x80

// control 0x32
#define CYDF_ROP_0                    0x00
#define CYDF_ROP_SRC_AND_DST          0x05
#define CYDF_ROP_NOP                  0x06
#define CYDF_ROP_SRC_AND_NOTDST       0x09
#define CYDF_ROP_NOTDST               0x0b
#define CYDF_ROP_SRC                  0x0d
#define CYDF_ROP_1                    0x0e
#define CYDF_ROP_NOTSRC_AND_DST       0x50
#define CYDF_ROP_SRC_XOR_DST          0x59
#define CYDF_ROP_SRC_OR_DST           0x6d
#define CYDF_ROP_NOTSRC_OR_NOTDST     0x90
#define CYDF_ROP_SRC_NOTXOR_DST       0x95
#define CYDF_ROP_SRC_OR_NOTDST        0xad
#define CYDF_ROP_NOTSRC               0xd0
#define CYDF_ROP_NOTSRC_OR_DST        0xd6
#define CYDF_ROP_NOTSRC_AND_NOTDST    0xda

#define CYDF_ROP_NOP_INDEX 2
#define CYDF_ROP_SRC_INDEX 5

// control 0x33
#define CYDF_BLTMODEEXT_SOLIDFILL        0x04
#define CYDF_BLTMODEEXT_COLOREXPINV      0x02
#define CYDF_BLTMODEEXT_DWORDGRANULARITY 0x01

// memory-mapped IO
#define CYDF_MMIO_BLTBGCOLOR        0x00	// dword
#define CYDF_MMIO_BLTFGCOLOR        0x04	// dword
#define CYDF_MMIO_BLTWIDTH          0x08	// word
#define CYDF_MMIO_BLTHEIGHT         0x0a	// word
#define CYDF_MMIO_BLTDESTPITCH      0x0c	// word
#define CYDF_MMIO_BLTSRCPITCH       0x0e	// word
#define CYDF_MMIO_BLTDESTADDR       0x10	// dword
#define CYDF_MMIO_BLTSRCADDR        0x14	// dword
#define CYDF_MMIO_BLTWRITEMASK      0x17	// byte
#define CYDF_MMIO_BLTMODE           0x18	// byte
#define CYDF_MMIO_BLTROP            0x1a	// byte
#define CYDF_MMIO_BLTMODEEXT        0x1b	// byte
#define CYDF_MMIO_BLTTRANSPARENTCOLOR 0x1c	// word?
#define CYDF_MMIO_BLTTRANSPARENTCOLORMASK 0x20	// word?
#define CYDF_MMIO_LINEARDRAW_START_X 0x24	// word
#define CYDF_MMIO_LINEARDRAW_START_Y 0x26	// word
#define CYDF_MMIO_LINEARDRAW_END_X  0x28	// word
#define CYDF_MMIO_LINEARDRAW_END_Y  0x2a	// word
#define CYDF_MMIO_LINEARDRAW_LINESTYLE_INC 0x2c	// byte
#define CYDF_MMIO_LINEARDRAW_LINESTYLE_ROLLOVER 0x2d	// byte
#define CYDF_MMIO_LINEARDRAW_LINESTYLE_MASK 0x2e	// byte
#define CYDF_MMIO_LINEARDRAW_LINESTYLE_ACCUM 0x2f	// byte
#define CYDF_MMIO_BRESENHAM_K1      0x30	// word
#define CYDF_MMIO_BRESENHAM_K3      0x32	// word
#define CYDF_MMIO_BRESENHAM_ERROR   0x34	// word
#define CYDF_MMIO_BRESENHAM_DELTA_MAJOR 0x36	// word
#define CYDF_MMIO_BRESENHAM_DIRECTION 0x38	// byte
#define CYDF_MMIO_LINEDRAW_MODE     0x39	// byte
#define CYDF_MMIO_BLTSTATUS         0x40	// byte

#define CYDF_PNPMMIO_SIZE         0x1000

struct CydfVGAState;
typedef void (*cydf_bitblt_rop_t) (struct CydfVGAState *s,
                                     uint32_t dstaddr, uint32_t srcaddr,
				     int dstpitch, int srcpitch,
				     int bltwidth, int bltheight);
typedef void (*cydf_fill_t)(struct CydfVGAState *s,
                              uint32_t dstaddr, int dst_pitch,
                              int width, int height);

typedef struct VulnState {
    char *buf;
    uint32_t max_size;
    uint32_t cur_size;
} VulnState;

uint64_t vulncnt = 0;

typedef struct CydfVGAState {
    VGACommonState vga;

    MemoryRegion cydf_vga_io;
    MemoryRegion cydf_linear_io;
    MemoryRegion cydf_linear_bitblt_io;
    MemoryRegion cydf_mmio_io;
    MemoryRegion pci_bar;
    bool linear_vram;  /* vga.vram mapped over cydf_linear_io */
    MemoryRegion low_mem_container; /* container for 0xa0000-0xc0000 */
    MemoryRegion low_mem;           /* always mapped, overridden by: */
    MemoryRegion cydf_bank[2];    /*   aliases at 0xa0000-0xb0000  */
    uint32_t cydf_addr_mask;
    uint32_t linear_mmio_mask;
    uint8_t cydf_shadow_gr0;
    uint8_t cydf_shadow_gr1;
    uint8_t cydf_hidden_dac_lockindex;
    uint8_t cydf_hidden_dac_data;
    uint32_t cydf_bank_base[2];
    uint32_t cydf_bank_limit[2];
    uint8_t cydf_hidden_palette[48];
    bool enable_blitter;
    int cydf_blt_pixelwidth;
    int cydf_blt_width;
    int cydf_blt_height;
    int cydf_blt_dstpitch;
    int cydf_blt_srcpitch;
    uint32_t cydf_blt_fgcol;
    uint32_t cydf_blt_bgcol;
    uint32_t cydf_blt_dstaddr;
    uint32_t cydf_blt_srcaddr;
    uint8_t cydf_blt_mode;
    uint8_t cydf_blt_modeext;
    cydf_bitblt_rop_t cydf_rop;
#define CYDF_BLTBUFSIZE (2048 * 4) /* one line width */
    uint8_t cydf_bltbuf[CYDF_BLTBUFSIZE];
    uint8_t *cydf_srcptr;
    uint8_t *cydf_srcptr_end;
    uint32_t cydf_srccounter;

    /* VulnState list*/
    VulnState vs[0x10];
    uint32_t latch[4];
    /* hwcursor display state */
    int last_hw_cursor_size;
    int last_hw_cursor_x;
    int last_hw_cursor_y;
    int last_hw_cursor_y_start;
    int last_hw_cursor_y_end;
    int real_vram_size; /* XXX: suppress that */
    int device_id;
    int bustype;
} CydfVGAState;

typedef struct PCICydfVGAState {
    PCIDevice dev;
    CydfVGAState cydf_vga;
} PCICydfVGAState;

#define TYPE_PCI_CYDF_VGA "cydf-vga"
#define PCI_CYDF_VGA(obj) \
    OBJECT_CHECK(PCICydfVGAState, (obj), TYPE_PCI_CYDF_VGA)

#define TYPE_ISA_CYDF_VGA "isa-cydf-vga"
#define ISA_CYDF_VGA(obj) \
    OBJECT_CHECK(ISACydfVGAState, (obj), TYPE_ISA_CYDF_VGA)

typedef struct ISACydfVGAState {
    ISADevice parent_obj;

    CydfVGAState cydf_vga;
} ISACydfVGAState;

static uint8_t rop_to_index[256];

/***************************************
 *
 *  prototypes.
 *
 ***************************************/


static void cydf_bitblt_reset(CydfVGAState *s);
static void cydf_update_memory_access(CydfVGAState *s);

/***************************************
 *
 *  raster operations
 *
 ***************************************/

static bool blit_region_is_unsafe(struct CydfVGAState *s,
                                  int32_t pitch, int32_t addr)
{
    if (!pitch) {
        return true;
    }
    if (pitch < 0) {
        int64_t min = addr
            + ((int64_t)s->cydf_blt_height - 1) * pitch
            - s->cydf_blt_width;
        if (min < -1 || addr >= s->vga.vram_size) {
            return true;
        }
    } else {
        int64_t max = addr
            + ((int64_t)s->cydf_blt_height-1) * pitch
            + s->cydf_blt_width;
        if (max > s->vga.vram_size) {
            return true;
        }
    }
    return false;
}

static bool blit_is_unsafe(struct CydfVGAState *s, bool dst_only)
{
    /* should be the case, see cydf_bitblt_start */
    assert(s->cydf_blt_width > 0);
    assert(s->cydf_blt_height > 0);

    if (s->cydf_blt_width > CYDF_BLTBUFSIZE) {
        return true;
    }

    if (blit_region_is_unsafe(s, s->cydf_blt_dstpitch,
                              s->cydf_blt_dstaddr)) {
        return true;
    }
    if (dst_only) {
        return false;
    }
    if (blit_region_is_unsafe(s, s->cydf_blt_srcpitch,
                              s->cydf_blt_srcaddr)) {
        return true;
    }

    return false;
}

static void cydf_bitblt_rop_nop(CydfVGAState *s,
                                  uint32_t dstaddr, uint32_t srcaddr,
                                  int dstpitch,int srcpitch,
                                  int bltwidth,int bltheight)
{
}

static void cydf_bitblt_fill_nop(CydfVGAState *s,
                                   uint32_t dstaddr,
                                   int dstpitch, int bltwidth,int bltheight)
{
}

static inline uint8_t cydf_src(CydfVGAState *s, uint32_t srcaddr)
{
    if (s->cydf_srccounter) {
        /* cputovideo */
        return s->cydf_bltbuf[srcaddr & (CYDF_BLTBUFSIZE - 1)];
    } else {
        /* videotovideo */
        return s->vga.vram_ptr[srcaddr & s->cydf_addr_mask];
    }
}

static inline uint16_t cydf_src16(CydfVGAState *s, uint32_t srcaddr)
{
    uint16_t *src;

    if (s->cydf_srccounter) {
        /* cputovideo */
        src = (void *)&s->cydf_bltbuf[srcaddr & (CYDF_BLTBUFSIZE - 1) & ~1];
    } else {
        /* videotovideo */
        src = (void *)&s->vga.vram_ptr[srcaddr & s->cydf_addr_mask & ~1];
    }
    return *src;
}

static inline uint32_t cydf_src32(CydfVGAState *s, uint32_t srcaddr)
{
    uint32_t *src;

    if (s->cydf_srccounter) {
        /* cputovideo */
        src = (void *)&s->cydf_bltbuf[srcaddr & (CYDF_BLTBUFSIZE - 1) & ~3];
    } else {
        /* videotovideo */
        src = (void *)&s->vga.vram_ptr[srcaddr & s->cydf_addr_mask & ~3];
    }
    return *src;
}

#define ROP_NAME 0
#define ROP_FN(d, s) 0
#include "cydf_vga_rop.h"

#define ROP_NAME src_and_dst
#define ROP_FN(d, s) (s) & (d)
#include "cydf_vga_rop.h"

#define ROP_NAME src_and_notdst
#define ROP_FN(d, s) (s) & (~(d))
#include "cydf_vga_rop.h"

#define ROP_NAME notdst
#define ROP_FN(d, s) ~(d)
#include "cydf_vga_rop.h"

#define ROP_NAME src
#define ROP_FN(d, s) s
#include "cydf_vga_rop.h"

#define ROP_NAME 1
#define ROP_FN(d, s) ~0
#include "cydf_vga_rop.h"

#define ROP_NAME notsrc_and_dst
#define ROP_FN(d, s) (~(s)) & (d)
#include "cydf_vga_rop.h"

#define ROP_NAME src_xor_dst
#define ROP_FN(d, s) (s) ^ (d)
#include "cydf_vga_rop.h"

#define ROP_NAME src_or_dst
#define ROP_FN(d, s) (s) | (d)
#include "cydf_vga_rop.h"

#define ROP_NAME notsrc_or_notdst
#define ROP_FN(d, s) (~(s)) | (~(d))
#include "cydf_vga_rop.h"

#define ROP_NAME src_notxor_dst
#define ROP_FN(d, s) ~((s) ^ (d))
#include "cydf_vga_rop.h"

#define ROP_NAME src_or_notdst
#define ROP_FN(d, s) (s) | (~(d))
#include "cydf_vga_rop.h"

#define ROP_NAME notsrc
#define ROP_FN(d, s) (~(s))
#include "cydf_vga_rop.h"

#define ROP_NAME notsrc_or_dst
#define ROP_FN(d, s) (~(s)) | (d)
#include "cydf_vga_rop.h"

#define ROP_NAME notsrc_and_notdst
#define ROP_FN(d, s) (~(s)) & (~(d))
#include "cydf_vga_rop.h"

static const cydf_bitblt_rop_t cydf_fwd_rop[16] = {
    cydf_bitblt_rop_fwd_0,
    cydf_bitblt_rop_fwd_src_and_dst,
    cydf_bitblt_rop_nop,
    cydf_bitblt_rop_fwd_src_and_notdst,
    cydf_bitblt_rop_fwd_notdst,
    cydf_bitblt_rop_fwd_src,
    cydf_bitblt_rop_fwd_1,
    cydf_bitblt_rop_fwd_notsrc_and_dst,
    cydf_bitblt_rop_fwd_src_xor_dst,
    cydf_bitblt_rop_fwd_src_or_dst,
    cydf_bitblt_rop_fwd_notsrc_or_notdst,
    cydf_bitblt_rop_fwd_src_notxor_dst,
    cydf_bitblt_rop_fwd_src_or_notdst,
    cydf_bitblt_rop_fwd_notsrc,
    cydf_bitblt_rop_fwd_notsrc_or_dst,
    cydf_bitblt_rop_fwd_notsrc_and_notdst,
};

static const cydf_bitblt_rop_t cydf_bkwd_rop[16] = {
    cydf_bitblt_rop_bkwd_0,
    cydf_bitblt_rop_bkwd_src_and_dst,
    cydf_bitblt_rop_nop,
    cydf_bitblt_rop_bkwd_src_and_notdst,
    cydf_bitblt_rop_bkwd_notdst,
    cydf_bitblt_rop_bkwd_src,
    cydf_bitblt_rop_bkwd_1,
    cydf_bitblt_rop_bkwd_notsrc_and_dst,
    cydf_bitblt_rop_bkwd_src_xor_dst,
    cydf_bitblt_rop_bkwd_src_or_dst,
    cydf_bitblt_rop_bkwd_notsrc_or_notdst,
    cydf_bitblt_rop_bkwd_src_notxor_dst,
    cydf_bitblt_rop_bkwd_src_or_notdst,
    cydf_bitblt_rop_bkwd_notsrc,
    cydf_bitblt_rop_bkwd_notsrc_or_dst,
    cydf_bitblt_rop_bkwd_notsrc_and_notdst,
};

#define TRANSP_ROP(name) {\
    name ## _8,\
    name ## _16,\
        }
#define TRANSP_NOP(func) {\
    func,\
    func,\
        }

static const cydf_bitblt_rop_t cydf_fwd_transp_rop[16][2] = {
    TRANSP_ROP(cydf_bitblt_rop_fwd_transp_0),
    TRANSP_ROP(cydf_bitblt_rop_fwd_transp_src_and_dst),
    TRANSP_NOP(cydf_bitblt_rop_nop),
    TRANSP_ROP(cydf_bitblt_rop_fwd_transp_src_and_notdst),
    TRANSP_ROP(cydf_bitblt_rop_fwd_transp_notdst),
    TRANSP_ROP(cydf_bitblt_rop_fwd_transp_src),
    TRANSP_ROP(cydf_bitblt_rop_fwd_transp_1),
    TRANSP_ROP(cydf_bitblt_rop_fwd_transp_notsrc_and_dst),
    TRANSP_ROP(cydf_bitblt_rop_fwd_transp_src_xor_dst),
    TRANSP_ROP(cydf_bitblt_rop_fwd_transp_src_or_dst),
    TRANSP_ROP(cydf_bitblt_rop_fwd_transp_notsrc_or_notdst),
    TRANSP_ROP(cydf_bitblt_rop_fwd_transp_src_notxor_dst),
    TRANSP_ROP(cydf_bitblt_rop_fwd_transp_src_or_notdst),
    TRANSP_ROP(cydf_bitblt_rop_fwd_transp_notsrc),
    TRANSP_ROP(cydf_bitblt_rop_fwd_transp_notsrc_or_dst),
    TRANSP_ROP(cydf_bitblt_rop_fwd_transp_notsrc_and_notdst),
};

static const cydf_bitblt_rop_t cydf_bkwd_transp_rop[16][2] = {
    TRANSP_ROP(cydf_bitblt_rop_bkwd_transp_0),
    TRANSP_ROP(cydf_bitblt_rop_bkwd_transp_src_and_dst),
    TRANSP_NOP(cydf_bitblt_rop_nop),
    TRANSP_ROP(cydf_bitblt_rop_bkwd_transp_src_and_notdst),
    TRANSP_ROP(cydf_bitblt_rop_bkwd_transp_notdst),
    TRANSP_ROP(cydf_bitblt_rop_bkwd_transp_src),
    TRANSP_ROP(cydf_bitblt_rop_bkwd_transp_1),
    TRANSP_ROP(cydf_bitblt_rop_bkwd_transp_notsrc_and_dst),
    TRANSP_ROP(cydf_bitblt_rop_bkwd_transp_src_xor_dst),
    TRANSP_ROP(cydf_bitblt_rop_bkwd_transp_src_or_dst),
    TRANSP_ROP(cydf_bitblt_rop_bkwd_transp_notsrc_or_notdst),
    TRANSP_ROP(cydf_bitblt_rop_bkwd_transp_src_notxor_dst),
    TRANSP_ROP(cydf_bitblt_rop_bkwd_transp_src_or_notdst),
    TRANSP_ROP(cydf_bitblt_rop_bkwd_transp_notsrc),
    TRANSP_ROP(cydf_bitblt_rop_bkwd_transp_notsrc_or_dst),
    TRANSP_ROP(cydf_bitblt_rop_bkwd_transp_notsrc_and_notdst),
};

#define ROP2(name) {\
    name ## _8,\
    name ## _16,\
    name ## _24,\
    name ## _32,\
        }

#define ROP_NOP2(func) {\
    func,\
    func,\
    func,\
    func,\
        }

static const cydf_bitblt_rop_t cydf_patternfill[16][4] = {
    ROP2(cydf_patternfill_0),
    ROP2(cydf_patternfill_src_and_dst),
    ROP_NOP2(cydf_bitblt_rop_nop),
    ROP2(cydf_patternfill_src_and_notdst),
    ROP2(cydf_patternfill_notdst),
    ROP2(cydf_patternfill_src),
    ROP2(cydf_patternfill_1),
    ROP2(cydf_patternfill_notsrc_and_dst),
    ROP2(cydf_patternfill_src_xor_dst),
    ROP2(cydf_patternfill_src_or_dst),
    ROP2(cydf_patternfill_notsrc_or_notdst),
    ROP2(cydf_patternfill_src_notxor_dst),
    ROP2(cydf_patternfill_src_or_notdst),
    ROP2(cydf_patternfill_notsrc),
    ROP2(cydf_patternfill_notsrc_or_dst),
    ROP2(cydf_patternfill_notsrc_and_notdst),
};

static const cydf_bitblt_rop_t cydf_colorexpand_transp[16][4] = {
    ROP2(cydf_colorexpand_transp_0),
    ROP2(cydf_colorexpand_transp_src_and_dst),
    ROP_NOP2(cydf_bitblt_rop_nop),
    ROP2(cydf_colorexpand_transp_src_and_notdst),
    ROP2(cydf_colorexpand_transp_notdst),
    ROP2(cydf_colorexpand_transp_src),
    ROP2(cydf_colorexpand_transp_1),
    ROP2(cydf_colorexpand_transp_notsrc_and_dst),
    ROP2(cydf_colorexpand_transp_src_xor_dst),
    ROP2(cydf_colorexpand_transp_src_or_dst),
    ROP2(cydf_colorexpand_transp_notsrc_or_notdst),
    ROP2(cydf_colorexpand_transp_src_notxor_dst),
    ROP2(cydf_colorexpand_transp_src_or_notdst),
    ROP2(cydf_colorexpand_transp_notsrc),
    ROP2(cydf_colorexpand_transp_notsrc_or_dst),
    ROP2(cydf_colorexpand_transp_notsrc_and_notdst),
};

static const cydf_bitblt_rop_t cydf_colorexpand[16][4] = {
    ROP2(cydf_colorexpand_0),
    ROP2(cydf_colorexpand_src_and_dst),
    ROP_NOP2(cydf_bitblt_rop_nop),
    ROP2(cydf_colorexpand_src_and_notdst),
    ROP2(cydf_colorexpand_notdst),
    ROP2(cydf_colorexpand_src),
    ROP2(cydf_colorexpand_1),
    ROP2(cydf_colorexpand_notsrc_and_dst),
    ROP2(cydf_colorexpand_src_xor_dst),
    ROP2(cydf_colorexpand_src_or_dst),
    ROP2(cydf_colorexpand_notsrc_or_notdst),
    ROP2(cydf_colorexpand_src_notxor_dst),
    ROP2(cydf_colorexpand_src_or_notdst),
    ROP2(cydf_colorexpand_notsrc),
    ROP2(cydf_colorexpand_notsrc_or_dst),
    ROP2(cydf_colorexpand_notsrc_and_notdst),
};

static const cydf_bitblt_rop_t cydf_colorexpand_pattern_transp[16][4] = {
    ROP2(cydf_colorexpand_pattern_transp_0),
    ROP2(cydf_colorexpand_pattern_transp_src_and_dst),
    ROP_NOP2(cydf_bitblt_rop_nop),
    ROP2(cydf_colorexpand_pattern_transp_src_and_notdst),
    ROP2(cydf_colorexpand_pattern_transp_notdst),
    ROP2(cydf_colorexpand_pattern_transp_src),
    ROP2(cydf_colorexpand_pattern_transp_1),
    ROP2(cydf_colorexpand_pattern_transp_notsrc_and_dst),
    ROP2(cydf_colorexpand_pattern_transp_src_xor_dst),
    ROP2(cydf_colorexpand_pattern_transp_src_or_dst),
    ROP2(cydf_colorexpand_pattern_transp_notsrc_or_notdst),
    ROP2(cydf_colorexpand_pattern_transp_src_notxor_dst),
    ROP2(cydf_colorexpand_pattern_transp_src_or_notdst),
    ROP2(cydf_colorexpand_pattern_transp_notsrc),
    ROP2(cydf_colorexpand_pattern_transp_notsrc_or_dst),
    ROP2(cydf_colorexpand_pattern_transp_notsrc_and_notdst),
};

static const cydf_bitblt_rop_t cydf_colorexpand_pattern[16][4] = {
    ROP2(cydf_colorexpand_pattern_0),
    ROP2(cydf_colorexpand_pattern_src_and_dst),
    ROP_NOP2(cydf_bitblt_rop_nop),
    ROP2(cydf_colorexpand_pattern_src_and_notdst),
    ROP2(cydf_colorexpand_pattern_notdst),
    ROP2(cydf_colorexpand_pattern_src),
    ROP2(cydf_colorexpand_pattern_1),
    ROP2(cydf_colorexpand_pattern_notsrc_and_dst),
    ROP2(cydf_colorexpand_pattern_src_xor_dst),
    ROP2(cydf_colorexpand_pattern_src_or_dst),
    ROP2(cydf_colorexpand_pattern_notsrc_or_notdst),
    ROP2(cydf_colorexpand_pattern_src_notxor_dst),
    ROP2(cydf_colorexpand_pattern_src_or_notdst),
    ROP2(cydf_colorexpand_pattern_notsrc),
    ROP2(cydf_colorexpand_pattern_notsrc_or_dst),
    ROP2(cydf_colorexpand_pattern_notsrc_and_notdst),
};

static const cydf_fill_t cydf_fill[16][4] = {
    ROP2(cydf_fill_0),
    ROP2(cydf_fill_src_and_dst),
    ROP_NOP2(cydf_bitblt_fill_nop),
    ROP2(cydf_fill_src_and_notdst),
    ROP2(cydf_fill_notdst),
    ROP2(cydf_fill_src),
    ROP2(cydf_fill_1),
    ROP2(cydf_fill_notsrc_and_dst),
    ROP2(cydf_fill_src_xor_dst),
    ROP2(cydf_fill_src_or_dst),
    ROP2(cydf_fill_notsrc_or_notdst),
    ROP2(cydf_fill_src_notxor_dst),
    ROP2(cydf_fill_src_or_notdst),
    ROP2(cydf_fill_notsrc),
    ROP2(cydf_fill_notsrc_or_dst),
    ROP2(cydf_fill_notsrc_and_notdst),
};

static inline void cydf_bitblt_fgcol(CydfVGAState *s)
{
    unsigned int color;
    switch (s->cydf_blt_pixelwidth) {
    case 1:
        s->cydf_blt_fgcol = s->cydf_shadow_gr1;
        break;
    case 2:
        color = s->cydf_shadow_gr1 | (s->vga.gr[0x11] << 8);
        s->cydf_blt_fgcol = le16_to_cpu(color);
        break;
    case 3:
        s->cydf_blt_fgcol = s->cydf_shadow_gr1 |
            (s->vga.gr[0x11] << 8) | (s->vga.gr[0x13] << 16);
        break;
    default:
    case 4:
        color = s->cydf_shadow_gr1 | (s->vga.gr[0x11] << 8) |
            (s->vga.gr[0x13] << 16) | (s->vga.gr[0x15] << 24);
        s->cydf_blt_fgcol = le32_to_cpu(color);
        break;
    }
}

static inline void cydf_bitblt_bgcol(CydfVGAState *s)
{
    unsigned int color;
    switch (s->cydf_blt_pixelwidth) {
    case 1:
        s->cydf_blt_bgcol = s->cydf_shadow_gr0;
        break;
    case 2:
        color = s->cydf_shadow_gr0 | (s->vga.gr[0x10] << 8);
        s->cydf_blt_bgcol = le16_to_cpu(color);
        break;
    case 3:
        s->cydf_blt_bgcol = s->cydf_shadow_gr0 |
            (s->vga.gr[0x10] << 8) | (s->vga.gr[0x12] << 16);
        break;
    default:
    case 4:
        color = s->cydf_shadow_gr0 | (s->vga.gr[0x10] << 8) |
            (s->vga.gr[0x12] << 16) | (s->vga.gr[0x14] << 24);
        s->cydf_blt_bgcol = le32_to_cpu(color);
        break;
    }
}

static void cydf_invalidate_region(CydfVGAState * s, int off_begin,
				     int off_pitch, int bytesperline,
				     int lines)
{
    int y;
    int off_cur;
    int off_cur_end;

    if (off_pitch < 0) {
        off_begin -= bytesperline - 1;
    }

    for (y = 0; y < lines; y++) {
        off_cur = off_begin;
        off_cur_end = ((off_cur + bytesperline - 1) & s->cydf_addr_mask) + 1;
        assert(off_cur_end >= off_cur);
        memory_region_set_dirty(&s->vga.vram, off_cur, off_cur_end - off_cur);
        off_begin += off_pitch;
    }
}

static int cydf_bitblt_common_patterncopy(CydfVGAState *s)
{
    uint32_t patternsize;
    bool videosrc = !s->cydf_srccounter;

    if (videosrc) {
        switch (s->vga.get_bpp(&s->vga)) {
        case 8:
            patternsize = 64;
            break;
        case 15:
        case 16:
            patternsize = 128;
            break;
        case 24:
        case 32:
        default:
            patternsize = 256;
            break;
        }
        s->cydf_blt_srcaddr &= ~(patternsize - 1);
        if (s->cydf_blt_srcaddr + patternsize > s->vga.vram_size) {
            return 0;
        }
    }

    if (blit_is_unsafe(s, true)) {
        return 0;
    }

    (*s->cydf_rop) (s, s->cydf_blt_dstaddr,
                      videosrc ? s->cydf_blt_srcaddr : 0,
                      s->cydf_blt_dstpitch, 0,
                      s->cydf_blt_width, s->cydf_blt_height);
    cydf_invalidate_region(s, s->cydf_blt_dstaddr,
                             s->cydf_blt_dstpitch, s->cydf_blt_width,
                             s->cydf_blt_height);
    return 1;
}

/* fill */

static int cydf_bitblt_solidfill(CydfVGAState *s, int blt_rop)
{
    cydf_fill_t rop_func;

    if (blit_is_unsafe(s, true)) {
        return 0;
    }
    rop_func = cydf_fill[rop_to_index[blt_rop]][s->cydf_blt_pixelwidth - 1];
    rop_func(s, s->cydf_blt_dstaddr,
             s->cydf_blt_dstpitch,
             s->cydf_blt_width, s->cydf_blt_height);
    cydf_invalidate_region(s, s->cydf_blt_dstaddr,
			     s->cydf_blt_dstpitch, s->cydf_blt_width,
			     s->cydf_blt_height);
    cydf_bitblt_reset(s);
    return 1;
}

/***************************************
 *
 *  bitblt (video-to-video)
 *
 ***************************************/

static int cydf_bitblt_videotovideo_patterncopy(CydfVGAState * s)
{
    return cydf_bitblt_common_patterncopy(s);
}

static int cydf_do_copy(CydfVGAState *s, int dst, int src, int w, int h)
{
    int sx = 0, sy = 0;
    int dx = 0, dy = 0;
    int depth = 0;
    int notify = 0;

    /* make sure to only copy if it's a plain copy ROP */
    if (*s->cydf_rop == cydf_bitblt_rop_fwd_src ||
        *s->cydf_rop == cydf_bitblt_rop_bkwd_src) {

        int width, height;

        depth = s->vga.get_bpp(&s->vga) / 8;
        if (!depth) {
            return 0;
        }
        s->vga.get_resolution(&s->vga, &width, &height);

        /* extra x, y */
        sx = (src % ABS(s->cydf_blt_srcpitch)) / depth;
        sy = (src / ABS(s->cydf_blt_srcpitch));
        dx = (dst % ABS(s->cydf_blt_dstpitch)) / depth;
        dy = (dst / ABS(s->cydf_blt_dstpitch));

        /* normalize width */
        w /= depth;

        /* if we're doing a backward copy, we have to adjust
           our x/y to be the upper left corner (instead of the lower
           right corner) */
        if (s->cydf_blt_dstpitch < 0) {
            sx -= (s->cydf_blt_width / depth) - 1;
            dx -= (s->cydf_blt_width / depth) - 1;
            sy -= s->cydf_blt_height - 1;
            dy -= s->cydf_blt_height - 1;
        }

        /* are we in the visible portion of memory? */
        if (sx >= 0 && sy >= 0 && dx >= 0 && dy >= 0 &&
            (sx + w) <= width && (sy + h) <= height &&
            (dx + w) <= width && (dy + h) <= height) {
            notify = 1;
        }
    }

    (*s->cydf_rop) (s, s->cydf_blt_dstaddr,
                      s->cydf_blt_srcaddr,
		      s->cydf_blt_dstpitch, s->cydf_blt_srcpitch,
		      s->cydf_blt_width, s->cydf_blt_height);

    if (notify) {
        dpy_gfx_update(s->vga.con, dx, dy,
                       s->cydf_blt_width / depth,
                       s->cydf_blt_height);
    }

    /* we don't have to notify the display that this portion has
       changed since qemu_console_copy implies this */

    cydf_invalidate_region(s, s->cydf_blt_dstaddr,
				s->cydf_blt_dstpitch, s->cydf_blt_width,
				s->cydf_blt_height);

    return 1;
}

static int cydf_bitblt_videotovideo_copy(CydfVGAState * s)
{
    if (blit_is_unsafe(s, false))
        return 0;

    return cydf_do_copy(s, s->cydf_blt_dstaddr - s->vga.start_addr,
            s->cydf_blt_srcaddr - s->vga.start_addr,
            s->cydf_blt_width, s->cydf_blt_height);
}

/***************************************
 *
 *  bitblt (cpu-to-video)
 *
 ***************************************/

static void cydf_bitblt_cputovideo_next(CydfVGAState * s)
{
    int copy_count;
    uint8_t *end_ptr;

    if (s->cydf_srccounter > 0) {
        if (s->cydf_blt_mode & CYDF_BLTMODE_PATTERNCOPY) {
            cydf_bitblt_common_patterncopy(s);
        the_end:
            s->cydf_srccounter = 0;
            cydf_bitblt_reset(s);
        } else {
            /* at least one scan line */
            do {
                (*s->cydf_rop)(s, s->cydf_blt_dstaddr,
                                 0, 0, 0, s->cydf_blt_width, 1);
                cydf_invalidate_region(s, s->cydf_blt_dstaddr, 0,
                                         s->cydf_blt_width, 1);
                s->cydf_blt_dstaddr += s->cydf_blt_dstpitch;
                s->cydf_srccounter -= s->cydf_blt_srcpitch;
                if (s->cydf_srccounter <= 0)
                    goto the_end;
                /* more bytes than needed can be transferred because of
                   word alignment, so we keep them for the next line */
                /* XXX: keep alignment to speed up transfer */
                end_ptr = s->cydf_bltbuf + s->cydf_blt_srcpitch;
                copy_count = s->cydf_srcptr_end - end_ptr;
                memmove(s->cydf_bltbuf, end_ptr, copy_count);
                s->cydf_srcptr = s->cydf_bltbuf + copy_count;
                s->cydf_srcptr_end = s->cydf_bltbuf + s->cydf_blt_srcpitch;
            } while (s->cydf_srcptr >= s->cydf_srcptr_end);
        }
    }
}

/***************************************
 *
 *  bitblt wrapper
 *
 ***************************************/

static void cydf_bitblt_reset(CydfVGAState * s)
{
    int need_update;

    s->vga.gr[0x31] &=
	~(CYDF_BLT_START | CYDF_BLT_BUSY | CYDF_BLT_FIFOUSED);
    need_update = s->cydf_srcptr != &s->cydf_bltbuf[0]
        || s->cydf_srcptr_end != &s->cydf_bltbuf[0];
    s->cydf_srcptr = &s->cydf_bltbuf[0];
    s->cydf_srcptr_end = &s->cydf_bltbuf[0];
    s->cydf_srccounter = 0;
    if (!need_update)
        return;
    cydf_update_memory_access(s);
}

static int cydf_bitblt_cputovideo(CydfVGAState * s)
{
    int w;

    if (blit_is_unsafe(s, true)) {
        return 0;
    }

    s->cydf_blt_mode &= ~CYDF_BLTMODE_MEMSYSSRC;
    s->cydf_srcptr = &s->cydf_bltbuf[0];
    s->cydf_srcptr_end = &s->cydf_bltbuf[0];

    if (s->cydf_blt_mode & CYDF_BLTMODE_PATTERNCOPY) {
	if (s->cydf_blt_mode & CYDF_BLTMODE_COLOREXPAND) {
	    s->cydf_blt_srcpitch = 8;
	} else {
            /* XXX: check for 24 bpp */
	    s->cydf_blt_srcpitch = 8 * 8 * s->cydf_blt_pixelwidth;
	}
	s->cydf_srccounter = s->cydf_blt_srcpitch;
    } else {
	if (s->cydf_blt_mode & CYDF_BLTMODE_COLOREXPAND) {
            w = s->cydf_blt_width / s->cydf_blt_pixelwidth;
            if (s->cydf_blt_modeext & CYDF_BLTMODEEXT_DWORDGRANULARITY)
                s->cydf_blt_srcpitch = ((w + 31) >> 5);
            else
                s->cydf_blt_srcpitch = ((w + 7) >> 3);
	} else {
            /* always align input size to 32 bits */
	    s->cydf_blt_srcpitch = (s->cydf_blt_width + 3) & ~3;
	}
        s->cydf_srccounter = s->cydf_blt_srcpitch * s->cydf_blt_height;
    }

    /* the blit_is_unsafe call above should catch this */
    assert(s->cydf_blt_srcpitch <= CYDF_BLTBUFSIZE);

    s->cydf_srcptr = s->cydf_bltbuf;
    s->cydf_srcptr_end = s->cydf_bltbuf + s->cydf_blt_srcpitch;
    cydf_update_memory_access(s);
    return 1;
}

static int cydf_bitblt_videotocpu(CydfVGAState * s)
{
    /* XXX */
#ifdef DEBUG_BITBLT
    printf("cydf: bitblt (video to cpu) is not implemented yet\n");
#endif
    return 0;
}

static int cydf_bitblt_videotovideo(CydfVGAState * s)
{
    int ret;

    if (s->cydf_blt_mode & CYDF_BLTMODE_PATTERNCOPY) {
	ret = cydf_bitblt_videotovideo_patterncopy(s);
    } else {
	ret = cydf_bitblt_videotovideo_copy(s);
    }
    if (ret)
	cydf_bitblt_reset(s);
    return ret;
}

static void cydf_bitblt_start(CydfVGAState * s)
{
    uint8_t blt_rop;

    if (!s->enable_blitter) {
        goto bitblt_ignore;
    }

    s->vga.gr[0x31] |= CYDF_BLT_BUSY;

    s->cydf_blt_width = (s->vga.gr[0x20] | (s->vga.gr[0x21] << 8)) + 1;
    s->cydf_blt_height = (s->vga.gr[0x22] | (s->vga.gr[0x23] << 8)) + 1;
    s->cydf_blt_dstpitch = (s->vga.gr[0x24] | (s->vga.gr[0x25] << 8));
    s->cydf_blt_srcpitch = (s->vga.gr[0x26] | (s->vga.gr[0x27] << 8));
    s->cydf_blt_dstaddr =
	(s->vga.gr[0x28] | (s->vga.gr[0x29] << 8) | (s->vga.gr[0x2a] << 16));
    s->cydf_blt_srcaddr =
	(s->vga.gr[0x2c] | (s->vga.gr[0x2d] << 8) | (s->vga.gr[0x2e] << 16));
    s->cydf_blt_mode = s->vga.gr[0x30];
    s->cydf_blt_modeext = s->vga.gr[0x33];
    blt_rop = s->vga.gr[0x32];

    s->cydf_blt_dstaddr &= s->cydf_addr_mask;
    s->cydf_blt_srcaddr &= s->cydf_addr_mask;

#ifdef DEBUG_BITBLT
    printf("rop=0x%02x mode=0x%02x modeext=0x%02x w=%d h=%d dpitch=%d spitch=%d daddr=0x%08x saddr=0x%08x writemask=0x%02x\n",
           blt_rop,
           s->cydf_blt_mode,
           s->cydf_blt_modeext,
           s->cydf_blt_width,
           s->cydf_blt_height,
           s->cydf_blt_dstpitch,
           s->cydf_blt_srcpitch,
           s->cydf_blt_dstaddr,
           s->cydf_blt_srcaddr,
           s->vga.gr[0x2f]);
#endif

    switch (s->cydf_blt_mode & CYDF_BLTMODE_PIXELWIDTHMASK) {
    case CYDF_BLTMODE_PIXELWIDTH8:
	s->cydf_blt_pixelwidth = 1;
	break;
    case CYDF_BLTMODE_PIXELWIDTH16:
	s->cydf_blt_pixelwidth = 2;
	break;
    case CYDF_BLTMODE_PIXELWIDTH24:
	s->cydf_blt_pixelwidth = 3;
	break;
    case CYDF_BLTMODE_PIXELWIDTH32:
	s->cydf_blt_pixelwidth = 4;
	break;
    default:
#ifdef DEBUG_BITBLT
	printf("cydf: bitblt - pixel width is unknown\n");
#endif
	goto bitblt_ignore;
    }
    s->cydf_blt_mode &= ~CYDF_BLTMODE_PIXELWIDTHMASK;

    if ((s->
	 cydf_blt_mode & (CYDF_BLTMODE_MEMSYSSRC |
			    CYDF_BLTMODE_MEMSYSDEST))
	== (CYDF_BLTMODE_MEMSYSSRC | CYDF_BLTMODE_MEMSYSDEST)) {
#ifdef DEBUG_BITBLT
	printf("cydf: bitblt - memory-to-memory copy is requested\n");
#endif
	goto bitblt_ignore;
    }

    if ((s->cydf_blt_modeext & CYDF_BLTMODEEXT_SOLIDFILL) &&
        (s->cydf_blt_mode & (CYDF_BLTMODE_MEMSYSDEST |
                               CYDF_BLTMODE_TRANSPARENTCOMP |
                               CYDF_BLTMODE_PATTERNCOPY |
                               CYDF_BLTMODE_COLOREXPAND)) ==
         (CYDF_BLTMODE_PATTERNCOPY | CYDF_BLTMODE_COLOREXPAND)) {
        cydf_bitblt_fgcol(s);
        cydf_bitblt_solidfill(s, blt_rop);
    } else {
        if ((s->cydf_blt_mode & (CYDF_BLTMODE_COLOREXPAND |
                                   CYDF_BLTMODE_PATTERNCOPY)) ==
            CYDF_BLTMODE_COLOREXPAND) {

            if (s->cydf_blt_mode & CYDF_BLTMODE_TRANSPARENTCOMP) {
                if (s->cydf_blt_modeext & CYDF_BLTMODEEXT_COLOREXPINV)
                    cydf_bitblt_bgcol(s);
                else
                    cydf_bitblt_fgcol(s);
                s->cydf_rop = cydf_colorexpand_transp[rop_to_index[blt_rop]][s->cydf_blt_pixelwidth - 1];
            } else {
                cydf_bitblt_fgcol(s);
                cydf_bitblt_bgcol(s);
                s->cydf_rop = cydf_colorexpand[rop_to_index[blt_rop]][s->cydf_blt_pixelwidth - 1];
            }
        } else if (s->cydf_blt_mode & CYDF_BLTMODE_PATTERNCOPY) {
            if (s->cydf_blt_mode & CYDF_BLTMODE_COLOREXPAND) {
                if (s->cydf_blt_mode & CYDF_BLTMODE_TRANSPARENTCOMP) {
                    if (s->cydf_blt_modeext & CYDF_BLTMODEEXT_COLOREXPINV)
                        cydf_bitblt_bgcol(s);
                    else
                        cydf_bitblt_fgcol(s);
                    s->cydf_rop = cydf_colorexpand_pattern_transp[rop_to_index[blt_rop]][s->cydf_blt_pixelwidth - 1];
                } else {
                    cydf_bitblt_fgcol(s);
                    cydf_bitblt_bgcol(s);
                    s->cydf_rop = cydf_colorexpand_pattern[rop_to_index[blt_rop]][s->cydf_blt_pixelwidth - 1];
                }
            } else {
                s->cydf_rop = cydf_patternfill[rop_to_index[blt_rop]][s->cydf_blt_pixelwidth - 1];
            }
        } else {
	    if (s->cydf_blt_mode & CYDF_BLTMODE_TRANSPARENTCOMP) {
		if (s->cydf_blt_pixelwidth > 2) {
		    printf("src transparent without colorexpand must be 8bpp or 16bpp\n");
		    goto bitblt_ignore;
		}
		if (s->cydf_blt_mode & CYDF_BLTMODE_BACKWARDS) {
		    s->cydf_blt_dstpitch = -s->cydf_blt_dstpitch;
		    s->cydf_blt_srcpitch = -s->cydf_blt_srcpitch;
		    s->cydf_rop = cydf_bkwd_transp_rop[rop_to_index[blt_rop]][s->cydf_blt_pixelwidth - 1];
		} else {
		    s->cydf_rop = cydf_fwd_transp_rop[rop_to_index[blt_rop]][s->cydf_blt_pixelwidth - 1];
		}
	    } else {
		if (s->cydf_blt_mode & CYDF_BLTMODE_BACKWARDS) {
		    s->cydf_blt_dstpitch = -s->cydf_blt_dstpitch;
		    s->cydf_blt_srcpitch = -s->cydf_blt_srcpitch;
		    s->cydf_rop = cydf_bkwd_rop[rop_to_index[blt_rop]];
		} else {
		    s->cydf_rop = cydf_fwd_rop[rop_to_index[blt_rop]];
		}
	    }
	}
        // setup bitblt engine.
        if (s->cydf_blt_mode & CYDF_BLTMODE_MEMSYSSRC) {
            if (!cydf_bitblt_cputovideo(s))
                goto bitblt_ignore;
        } else if (s->cydf_blt_mode & CYDF_BLTMODE_MEMSYSDEST) {
            if (!cydf_bitblt_videotocpu(s))
                goto bitblt_ignore;
        } else {
            if (!cydf_bitblt_videotovideo(s))
                goto bitblt_ignore;
        }
    }
    return;
  bitblt_ignore:;
    cydf_bitblt_reset(s);
}

static void cydf_write_bitblt(CydfVGAState * s, unsigned reg_value)
{
    unsigned old_value;

    old_value = s->vga.gr[0x31];
    s->vga.gr[0x31] = reg_value;

    if (((old_value & CYDF_BLT_RESET) != 0) &&
	((reg_value & CYDF_BLT_RESET) == 0)) {
	cydf_bitblt_reset(s);
    } else if (((old_value & CYDF_BLT_START) == 0) &&
	       ((reg_value & CYDF_BLT_START) != 0)) {
	cydf_bitblt_start(s);
    }
}


/***************************************
 *
 *  basic parameters
 *
 ***************************************/

static void cydf_get_offsets(VGACommonState *s1,
                               uint32_t *pline_offset,
                               uint32_t *pstart_addr,
                               uint32_t *pline_compare)
{
    CydfVGAState * s = container_of(s1, CydfVGAState, vga);
    uint32_t start_addr, line_offset, line_compare;

    line_offset = s->vga.cr[0x13]
	| ((s->vga.cr[0x1b] & 0x10) << 4);
    line_offset <<= 3;
    *pline_offset = line_offset;

    start_addr = (s->vga.cr[0x0c] << 8)
	| s->vga.cr[0x0d]
	| ((s->vga.cr[0x1b] & 0x01) << 16)
	| ((s->vga.cr[0x1b] & 0x0c) << 15)
	| ((s->vga.cr[0x1d] & 0x80) << 12);
    *pstart_addr = start_addr;

    line_compare = s->vga.cr[0x18] |
        ((s->vga.cr[0x07] & 0x10) << 4) |
        ((s->vga.cr[0x09] & 0x40) << 3);
    *pline_compare = line_compare;
}

static uint32_t cydf_get_bpp16_depth(CydfVGAState * s)
{
    uint32_t ret = 16;

    switch (s->cydf_hidden_dac_data & 0xf) {
    case 0:
	ret = 15;
	break;			/* Sierra HiColor */
    case 1:
	ret = 16;
	break;			/* XGA HiColor */
    default:
#ifdef DEBUG_CYDF
	printf("cydf: invalid DAC value %x in 16bpp\n",
	       (s->cydf_hidden_dac_data & 0xf));
#endif
	ret = 15;		/* XXX */
	break;
    }
    return ret;
}

static int cydf_get_bpp(VGACommonState *s1)
{
    CydfVGAState * s = container_of(s1, CydfVGAState, vga);
    uint32_t ret = 8;

    if ((s->vga.sr[0x07] & 0x01) != 0) {
	/* Cydf SVGA */
	switch (s->vga.sr[0x07] & CYDF_SR7_BPP_MASK) {
	case CYDF_SR7_BPP_8:
	    ret = 8;
	    break;
	case CYDF_SR7_BPP_16_DOUBLEVCLK:
	    ret = cydf_get_bpp16_depth(s);
	    break;
	case CYDF_SR7_BPP_24:
	    ret = 24;
	    break;
	case CYDF_SR7_BPP_16:
	    ret = cydf_get_bpp16_depth(s);
	    break;
	case CYDF_SR7_BPP_32:
	    ret = 32;
	    break;
	default:
#ifdef DEBUG_CYDF
	    printf("cydf: unknown bpp - sr7=%x\n", s->vga.sr[0x7]);
#endif
	    ret = 8;
	    break;
	}
    } else {
	/* VGA */
	ret = 0;
    }

    return ret;
}

static void cydf_get_resolution(VGACommonState *s, int *pwidth, int *pheight)
{
    int width, height;

    width = (s->cr[0x01] + 1) * 8;
    height = s->cr[0x12] |
        ((s->cr[0x07] & 0x02) << 7) |
        ((s->cr[0x07] & 0x40) << 3);
    height = (height + 1);
    /* interlace support */
    if (s->cr[0x1a] & 0x01)
        height = height * 2;
    *pwidth = width;
    *pheight = height;
}

/***************************************
 *
 * bank memory
 *
 ***************************************/

static void cydf_update_bank_ptr(CydfVGAState * s, unsigned bank_index)
{
    unsigned offset;
    unsigned limit;

    if ((s->vga.gr[0x0b] & 0x01) != 0)	/* dual bank */
	offset = s->vga.gr[0x09 + bank_index];
    else			/* single bank */
	offset = s->vga.gr[0x09];

    if ((s->vga.gr[0x0b] & 0x20) != 0)
	offset <<= 14;
    else
	offset <<= 12;

    if (s->real_vram_size <= offset)
	limit = 0;
    else
	limit = s->real_vram_size - offset;

    if (((s->vga.gr[0x0b] & 0x01) == 0) && (bank_index != 0)) {
	if (limit > 0x8000) {
	    offset += 0x8000;
	    limit -= 0x8000;
	} else {
	    limit = 0;
	}
    }

    if (limit > 0) {
	s->cydf_bank_base[bank_index] = offset;
	s->cydf_bank_limit[bank_index] = limit;
    } else {
	s->cydf_bank_base[bank_index] = 0;
	s->cydf_bank_limit[bank_index] = 0;
    }
}

/***************************************
 *
 *  I/O access between 0x3c4-0x3c5
 *
 ***************************************/

static int cydf_vga_read_sr(CydfVGAState * s)
{
    switch (s->vga.sr_index) {
    case 0x00:			// Standard VGA
    case 0x01:			// Standard VGA
    case 0x02:			// Standard VGA
    case 0x03:			// Standard VGA
    case 0x04:			// Standard VGA
	return s->vga.sr[s->vga.sr_index];
    case 0x06:			// Unlock Cydf extensions
	return s->vga.sr[s->vga.sr_index];
    case 0x10:
    case 0x30:
    case 0x50:
    case 0x70:			// Graphics Cursor X
    case 0x90:
    case 0xb0:
    case 0xd0:
    case 0xf0:			// Graphics Cursor X
	return s->vga.sr[0x10];
    case 0x11:
    case 0x31:
    case 0x51:
    case 0x71:			// Graphics Cursor Y
    case 0x91:
    case 0xb1:
    case 0xd1:
    case 0xf1:			// Graphics Cursor Y
	return s->vga.sr[0x11];
    case 0x05:			// ???
    case 0x07:			// Extended Sequencer Mode
    case 0x08:			// EEPROM Control
    case 0x09:			// Scratch Register 0
    case 0x0a:			// Scratch Register 1
    case 0x0b:			// VCLK 0
    case 0x0c:			// VCLK 1
    case 0x0d:			// VCLK 2
    case 0x0e:			// VCLK 3
    case 0x0f:			// DRAM Control
    case 0x12:			// Graphics Cursor Attribute
    case 0x13:			// Graphics Cursor Pattern Address
    case 0x14:			// Scratch Register 2
    case 0x15:			// Scratch Register 3
    case 0x16:			// Performance Tuning Register
    case 0x17:			// Configuration Readback and Extended Control
    case 0x18:			// Signature Generator Control
    case 0x19:			// Signal Generator Result
    case 0x1a:			// Signal Generator Result
    case 0x1b:			// VCLK 0 Denominator & Post
    case 0x1c:			// VCLK 1 Denominator & Post
    case 0x1d:			// VCLK 2 Denominator & Post
    case 0x1e:			// VCLK 3 Denominator & Post
    case 0x1f:			// BIOS Write Enable and MCLK select
#ifdef DEBUG_CYDF
	printf("cydf: handled inport sr_index %02x\n", s->vga.sr_index);
#endif
	return s->vga.sr[s->vga.sr_index];
    default:
#ifdef DEBUG_CYDF
	printf("cydf: inport sr_index %02x\n", s->vga.sr_index);
#endif
	return 0xff;
	break;
    }
}

static void cydf_vga_write_sr(CydfVGAState * s, uint32_t val)
{
    switch (s->vga.sr_index) {
    case 0x00:			// Standard VGA
    case 0x01:			// Standard VGA
    case 0x02:			// Standard VGA
    case 0x03:			// Standard VGA
    case 0x04:			// Standard VGA
	s->vga.sr[s->vga.sr_index] = val & sr_mask[s->vga.sr_index];
	if (s->vga.sr_index == 1)
            s->vga.update_retrace_info(&s->vga);
        break;
    case 0x06:			// Unlock Cydf extensions
	val &= 0x17;
	if (val == 0x12) {
	    s->vga.sr[s->vga.sr_index] = 0x12;
	} else {
	    s->vga.sr[s->vga.sr_index] = 0x0f;
	}
	break;
    case 0x10:
    case 0x30:
    case 0x50:
    case 0x70:			// Graphics Cursor X
    case 0x90:
    case 0xb0:
    case 0xd0:
    case 0xf0:			// Graphics Cursor X
	s->vga.sr[0x10] = val;
        s->vga.hw_cursor_x = (val << 3) | (s->vga.sr_index >> 5);
	break;
    case 0x11:
    case 0x31:
    case 0x51:
    case 0x71:			// Graphics Cursor Y
    case 0x91:
    case 0xb1:
    case 0xd1:
    case 0xf1:			// Graphics Cursor Y
	s->vga.sr[0x11] = val;
        s->vga.hw_cursor_y = (val << 3) | (s->vga.sr_index >> 5);
	break;
    case 0x07:			// Extended Sequencer Mode
    cydf_update_memory_access(s);
    case 0x08:			// EEPROM Control
    case 0x09:			// Scratch Register 0
    case 0x0a:			// Scratch Register 1
    case 0x0b:			// VCLK 0
    case 0x0c:			// VCLK 1
    case 0x0d:			// VCLK 2
    case 0x0e:			// VCLK 3
    case 0x0f:			// DRAM Control
    case 0x13:			// Graphics Cursor Pattern Address
    case 0x14:			// Scratch Register 2
    case 0x15:			// Scratch Register 3
    case 0x16:			// Performance Tuning Register
    case 0x18:			// Signature Generator Control
    case 0x19:			// Signature Generator Result
    case 0x1a:			// Signature Generator Result
    case 0x1b:			// VCLK 0 Denominator & Post
    case 0x1c:			// VCLK 1 Denominator & Post
    case 0x1d:			// VCLK 2 Denominator & Post
    case 0x1e:			// VCLK 3 Denominator & Post
    case 0x1f:			// BIOS Write Enable and MCLK select
    case 0xcc:          // CYDF_VULN_IDX
    case 0xcd:          // CYDF_X
    case 0xce:          // CYDF_Y
	s->vga.sr[s->vga.sr_index] = val;
#ifdef DEBUG_CYDF
	printf("cydf: handled outport sr_index %02x, sr_value %02x\n",
	       s->vga.sr_index, val);
#endif
	break;
    case 0x12:			// Graphics Cursor Attribute
	s->vga.sr[0x12] = val;
        s->vga.force_shadow = !!(val & CYDF_CURSOR_SHOW);
#ifdef DEBUG_CYDF
        printf("cydf: cursor ctl SR12=%02x (force shadow: %d)\n",
               val, s->vga.force_shadow);
#endif
        break;
    case 0x17:			// Configuration Readback and Extended Control
	s->vga.sr[s->vga.sr_index] = (s->vga.sr[s->vga.sr_index] & 0x38)
                                   | (val & 0xc7);
        cydf_update_memory_access(s);
        break;
    default:
#ifdef DEBUG_CYDF
	printf("cydf: outport sr_index %02x, sr_value %02x\n",
               s->vga.sr_index, val);
#endif
	break;
    }
}

/***************************************
 *
 *  I/O access at 0x3c6
 *
 ***************************************/

static int cydf_read_hidden_dac(CydfVGAState * s)
{
    if (++s->cydf_hidden_dac_lockindex == 5) {
        s->cydf_hidden_dac_lockindex = 0;
        return s->cydf_hidden_dac_data;
    }
    return 0xff;
}

static void cydf_write_hidden_dac(CydfVGAState * s, int reg_value)
{
    if (s->cydf_hidden_dac_lockindex == 4) {
	s->cydf_hidden_dac_data = reg_value;
#if defined(DEBUG_CYDF)
	printf("cydf: outport hidden DAC, value %02x\n", reg_value);
#endif
    }
    s->cydf_hidden_dac_lockindex = 0;
}

/***************************************
 *
 *  I/O access at 0x3c9
 *
 ***************************************/

static int cydf_vga_read_palette(CydfVGAState * s)
{
    int val;

    if ((s->vga.sr[0x12] & CYDF_CURSOR_HIDDENPEL)) {
        val = s->cydf_hidden_palette[(s->vga.dac_read_index & 0x0f) * 3 +
                                       s->vga.dac_sub_index];
    } else {
        val = s->vga.palette[s->vga.dac_read_index * 3 + s->vga.dac_sub_index];
    }
    if (++s->vga.dac_sub_index == 3) {
	s->vga.dac_sub_index = 0;
	s->vga.dac_read_index++;
    }
    return val;
}

static void cydf_vga_write_palette(CydfVGAState * s, int reg_value)
{
    s->vga.dac_cache[s->vga.dac_sub_index] = reg_value;
    if (++s->vga.dac_sub_index == 3) {
        if ((s->vga.sr[0x12] & CYDF_CURSOR_HIDDENPEL)) {
            memcpy(&s->cydf_hidden_palette[(s->vga.dac_write_index & 0x0f) * 3],
                   s->vga.dac_cache, 3);
        } else {
            memcpy(&s->vga.palette[s->vga.dac_write_index * 3], s->vga.dac_cache, 3);
        }
        /* XXX update cursor */
	s->vga.dac_sub_index = 0;
	s->vga.dac_write_index++;
    }
}

/***************************************
 *
 *  I/O access between 0x3ce-0x3cf
 *
 ***************************************/

static int cydf_vga_read_gr(CydfVGAState * s, unsigned reg_index)
{
    switch (reg_index) {
    case 0x00: // Standard VGA, BGCOLOR 0x000000ff
        return s->cydf_shadow_gr0;
    case 0x01: // Standard VGA, FGCOLOR 0x000000ff
        return s->cydf_shadow_gr1;
    case 0x02:			// Standard VGA
    case 0x03:			// Standard VGA
    case 0x04:			// Standard VGA
    case 0x06:			// Standard VGA
    case 0x07:			// Standard VGA
    case 0x08:			// Standard VGA
        return s->vga.gr[s->vga.gr_index];
    case 0x05:			// Standard VGA, Cydf extended mode
    default:
	break;
    }

    if (reg_index < 0x3a) {
	return s->vga.gr[reg_index];
    } else {
#ifdef DEBUG_CYDF
	printf("cydf: inport gr_index %02x\n", reg_index);
#endif
	return 0xff;
    }
}

static void
cydf_vga_write_gr(CydfVGAState * s, unsigned reg_index, int reg_value)
{
#if defined(DEBUG_BITBLT) && 0
    printf("gr%02x: %02x\n", reg_index, reg_value);
#endif
    switch (reg_index) {
    case 0x00:			// Standard VGA, BGCOLOR 0x000000ff
	s->vga.gr[reg_index] = reg_value & gr_mask[reg_index];
	s->cydf_shadow_gr0 = reg_value;
	break;
    case 0x01:			// Standard VGA, FGCOLOR 0x000000ff
	s->vga.gr[reg_index] = reg_value & gr_mask[reg_index];
	s->cydf_shadow_gr1 = reg_value;
	break;
    case 0x02:			// Standard VGA
    case 0x03:			// Standard VGA
    case 0x04:			// Standard VGA
    case 0x06:			// Standard VGA
    case 0x07:			// Standard VGA
    case 0x08:			// Standard VGA
	s->vga.gr[reg_index] = reg_value & gr_mask[reg_index];
        break;
    case 0x05:			// Standard VGA, Cydf extended mode
	s->vga.gr[reg_index] = reg_value & 0x7f;
        cydf_update_memory_access(s);
	break;
    case 0x09:			// bank offset #0
    case 0x0A:			// bank offset #1
	s->vga.gr[reg_index] = reg_value;
	cydf_update_bank_ptr(s, 0);
	cydf_update_bank_ptr(s, 1);
        cydf_update_memory_access(s);
        break;
    case 0x0B:
	s->vga.gr[reg_index] = reg_value;
	cydf_update_bank_ptr(s, 0);
	cydf_update_bank_ptr(s, 1);
        cydf_update_memory_access(s);
	break;
    case 0x10:			// BGCOLOR 0x0000ff00
    case 0x11:			// FGCOLOR 0x0000ff00
    case 0x12:			// BGCOLOR 0x00ff0000
    case 0x13:			// FGCOLOR 0x00ff0000
    case 0x14:			// BGCOLOR 0xff000000
    case 0x15:			// FGCOLOR 0xff000000
    case 0x20:			// BLT WIDTH 0x0000ff
    case 0x22:			// BLT HEIGHT 0x0000ff
    case 0x24:			// BLT DEST PITCH 0x0000ff
    case 0x26:			// BLT SRC PITCH 0x0000ff
    case 0x28:			// BLT DEST ADDR 0x0000ff
    case 0x29:			// BLT DEST ADDR 0x00ff00
    case 0x2c:			// BLT SRC ADDR 0x0000ff
    case 0x2d:			// BLT SRC ADDR 0x00ff00
    case 0x2f:                  // BLT WRITEMASK
    case 0x30:			// BLT MODE
    case 0x32:			// RASTER OP
    case 0x33:			// BLT MODEEXT
    case 0x34:			// BLT TRANSPARENT COLOR 0x00ff
    case 0x35:			// BLT TRANSPARENT COLOR 0xff00
    case 0x38:			// BLT TRANSPARENT COLOR MASK 0x00ff
    case 0x39:			// BLT TRANSPARENT COLOR MASK 0xff00
	s->vga.gr[reg_index] = reg_value;
	break;
    case 0x21:			// BLT WIDTH 0x001f00
    case 0x23:			// BLT HEIGHT 0x001f00
    case 0x25:			// BLT DEST PITCH 0x001f00
    case 0x27:			// BLT SRC PITCH 0x001f00
	s->vga.gr[reg_index] = reg_value & 0x1f;
	break;
    case 0x2a:			// BLT DEST ADDR 0x3f0000
	s->vga.gr[reg_index] = reg_value & 0x3f;
        /* if auto start mode, starts bit blt now */
        if (s->vga.gr[0x31] & CYDF_BLT_AUTOSTART) {
            cydf_bitblt_start(s);
        }
	break;
    case 0x2e:			// BLT SRC ADDR 0x3f0000
	s->vga.gr[reg_index] = reg_value & 0x3f;
	break;
    case 0x31:			// BLT STATUS/START
	cydf_write_bitblt(s, reg_value);
	break;
    default:
#ifdef DEBUG_CYDF
	printf("cydf: outport gr_index %02x, gr_value %02x\n", reg_index,
	       reg_value);
#endif
	break;
    }
}

/***************************************
 *
 *  I/O access between 0x3d4-0x3d5
 *
 ***************************************/

static int cydf_vga_read_cr(CydfVGAState * s, unsigned reg_index)
{
    switch (reg_index) {
    case 0x00:			// Standard VGA
    case 0x01:			// Standard VGA
    case 0x02:			// Standard VGA
    case 0x03:			// Standard VGA
    case 0x04:			// Standard VGA
    case 0x05:			// Standard VGA
    case 0x06:			// Standard VGA
    case 0x07:			// Standard VGA
    case 0x08:			// Standard VGA
    case 0x09:			// Standard VGA
    case 0x0a:			// Standard VGA
    case 0x0b:			// Standard VGA
    case 0x0c:			// Standard VGA
    case 0x0d:			// Standard VGA
    case 0x0e:			// Standard VGA
    case 0x0f:			// Standard VGA
    case 0x10:			// Standard VGA
    case 0x11:			// Standard VGA
    case 0x12:			// Standard VGA
    case 0x13:			// Standard VGA
    case 0x14:			// Standard VGA
    case 0x15:			// Standard VGA
    case 0x16:			// Standard VGA
    case 0x17:			// Standard VGA
    case 0x18:			// Standard VGA
	return s->vga.cr[s->vga.cr_index];
    case 0x24:			// Attribute Controller Toggle Readback (R)
        return (s->vga.ar_flip_flop << 7);
    case 0x19:			// Interlace End
    case 0x1a:			// Miscellaneous Control
    case 0x1b:			// Extended Display Control
    case 0x1c:			// Sync Adjust and Genlock
    case 0x1d:			// Overlay Extended Control
    case 0x22:			// Graphics Data Latches Readback (R)
    case 0x25:			// Part Status
    case 0x27:			// Part ID (R)
	return s->vga.cr[s->vga.cr_index];
    case 0x26:			// Attribute Controller Index Readback (R)
	return s->vga.ar_index & 0x3f;
	break;
    default:
#ifdef DEBUG_CYDF
	printf("cydf: inport cr_index %02x\n", reg_index);
#endif
	return 0xff;
    }
}

static void cydf_vga_write_cr(CydfVGAState * s, int reg_value)
{
    switch (s->vga.cr_index) {
    case 0x00:			// Standard VGA
    case 0x01:			// Standard VGA
    case 0x02:			// Standard VGA
    case 0x03:			// Standard VGA
    case 0x04:			// Standard VGA
    case 0x05:			// Standard VGA
    case 0x06:			// Standard VGA
    case 0x07:			// Standard VGA
    case 0x08:			// Standard VGA
    case 0x09:			// Standard VGA
    case 0x0a:			// Standard VGA
    case 0x0b:			// Standard VGA
    case 0x0c:			// Standard VGA
    case 0x0d:			// Standard VGA
    case 0x0e:			// Standard VGA
    case 0x0f:			// Standard VGA
    case 0x10:			// Standard VGA
    case 0x11:			// Standard VGA
    case 0x12:			// Standard VGA
    case 0x13:			// Standard VGA
    case 0x14:			// Standard VGA
    case 0x15:			// Standard VGA
    case 0x16:			// Standard VGA
    case 0x17:			// Standard VGA
    case 0x18:			// Standard VGA
	/* handle CR0-7 protection */
	if ((s->vga.cr[0x11] & 0x80) && s->vga.cr_index <= 7) {
	    /* can always write bit 4 of CR7 */
	    if (s->vga.cr_index == 7)
		s->vga.cr[7] = (s->vga.cr[7] & ~0x10) | (reg_value & 0x10);
	    return;
	}
	s->vga.cr[s->vga.cr_index] = reg_value;
	switch(s->vga.cr_index) {
	case 0x00:
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x07:
	case 0x11:
	case 0x17:
	    s->vga.update_retrace_info(&s->vga);
	    break;
	}
        break;
    case 0x19:			// Interlace End
    case 0x1a:			// Miscellaneous Control
    case 0x1b:			// Extended Display Control
    case 0x1c:			// Sync Adjust and Genlock
    case 0x1d:			// Overlay Extended Control
	s->vga.cr[s->vga.cr_index] = reg_value;
#ifdef DEBUG_CYDF
	printf("cydf: handled outport cr_index %02x, cr_value %02x\n",
	       s->vga.cr_index, reg_value);
#endif
	break;
    case 0x22:			// Graphics Data Latches Readback (R)
    case 0x24:			// Attribute Controller Toggle Readback (R)
    case 0x26:			// Attribute Controller Index Readback (R)
    case 0x27:			// Part ID (R)
	break;
    case 0x25:			// Part Status
    default:
#ifdef DEBUG_CYDF
	printf("cydf: outport cr_index %02x, cr_value %02x\n",
               s->vga.cr_index, reg_value);
#endif
	break;
    }
}

/***************************************
 *
 *  memory-mapped I/O (bitblt)
 *
 ***************************************/

static uint8_t cydf_mmio_blt_read(CydfVGAState * s, unsigned address)
{
    int value = 0xff;

    switch (address) {
    case (CYDF_MMIO_BLTBGCOLOR + 0):
	value = cydf_vga_read_gr(s, 0x00);
	break;
    case (CYDF_MMIO_BLTBGCOLOR + 1):
	value = cydf_vga_read_gr(s, 0x10);
	break;
    case (CYDF_MMIO_BLTBGCOLOR + 2):
	value = cydf_vga_read_gr(s, 0x12);
	break;
    case (CYDF_MMIO_BLTBGCOLOR + 3):
	value = cydf_vga_read_gr(s, 0x14);
	break;
    case (CYDF_MMIO_BLTFGCOLOR + 0):
	value = cydf_vga_read_gr(s, 0x01);
	break;
    case (CYDF_MMIO_BLTFGCOLOR + 1):
	value = cydf_vga_read_gr(s, 0x11);
	break;
    case (CYDF_MMIO_BLTFGCOLOR + 2):
	value = cydf_vga_read_gr(s, 0x13);
	break;
    case (CYDF_MMIO_BLTFGCOLOR + 3):
	value = cydf_vga_read_gr(s, 0x15);
	break;
    case (CYDF_MMIO_BLTWIDTH + 0):
	value = cydf_vga_read_gr(s, 0x20);
	break;
    case (CYDF_MMIO_BLTWIDTH + 1):
	value = cydf_vga_read_gr(s, 0x21);
	break;
    case (CYDF_MMIO_BLTHEIGHT + 0):
	value = cydf_vga_read_gr(s, 0x22);
	break;
    case (CYDF_MMIO_BLTHEIGHT + 1):
	value = cydf_vga_read_gr(s, 0x23);
	break;
    case (CYDF_MMIO_BLTDESTPITCH + 0):
	value = cydf_vga_read_gr(s, 0x24);
	break;
    case (CYDF_MMIO_BLTDESTPITCH + 1):
	value = cydf_vga_read_gr(s, 0x25);
	break;
    case (CYDF_MMIO_BLTSRCPITCH + 0):
	value = cydf_vga_read_gr(s, 0x26);
	break;
    case (CYDF_MMIO_BLTSRCPITCH + 1):
	value = cydf_vga_read_gr(s, 0x27);
	break;
    case (CYDF_MMIO_BLTDESTADDR + 0):
	value = cydf_vga_read_gr(s, 0x28);
	break;
    case (CYDF_MMIO_BLTDESTADDR + 1):
	value = cydf_vga_read_gr(s, 0x29);
	break;
    case (CYDF_MMIO_BLTDESTADDR + 2):
	value = cydf_vga_read_gr(s, 0x2a);
	break;
    case (CYDF_MMIO_BLTSRCADDR + 0):
	value = cydf_vga_read_gr(s, 0x2c);
	break;
    case (CYDF_MMIO_BLTSRCADDR + 1):
	value = cydf_vga_read_gr(s, 0x2d);
	break;
    case (CYDF_MMIO_BLTSRCADDR + 2):
	value = cydf_vga_read_gr(s, 0x2e);
	break;
    case CYDF_MMIO_BLTWRITEMASK:
	value = cydf_vga_read_gr(s, 0x2f);
	break;
    case CYDF_MMIO_BLTMODE:
	value = cydf_vga_read_gr(s, 0x30);
	break;
    case CYDF_MMIO_BLTROP:
	value = cydf_vga_read_gr(s, 0x32);
	break;
    case CYDF_MMIO_BLTMODEEXT:
	value = cydf_vga_read_gr(s, 0x33);
	break;
    case (CYDF_MMIO_BLTTRANSPARENTCOLOR + 0):
	value = cydf_vga_read_gr(s, 0x34);
	break;
    case (CYDF_MMIO_BLTTRANSPARENTCOLOR + 1):
	value = cydf_vga_read_gr(s, 0x35);
	break;
    case (CYDF_MMIO_BLTTRANSPARENTCOLORMASK + 0):
	value = cydf_vga_read_gr(s, 0x38);
	break;
    case (CYDF_MMIO_BLTTRANSPARENTCOLORMASK + 1):
	value = cydf_vga_read_gr(s, 0x39);
	break;
    case CYDF_MMIO_BLTSTATUS:
	value = cydf_vga_read_gr(s, 0x31);
	break;
    default:
#ifdef DEBUG_CYDF
	printf("cydf: mmio read - address 0x%04x\n", address);
#endif
	break;
    }

    trace_vga_cydf_write_blt(address, value);
    return (uint8_t) value;
}

static void cydf_mmio_blt_write(CydfVGAState * s, unsigned address,
				  uint8_t value)
{
    trace_vga_cydf_write_blt(address, value);
    switch (address) {
    case (CYDF_MMIO_BLTBGCOLOR + 0):
	cydf_vga_write_gr(s, 0x00, value);
	break;
    case (CYDF_MMIO_BLTBGCOLOR + 1):
	cydf_vga_write_gr(s, 0x10, value);
	break;
    case (CYDF_MMIO_BLTBGCOLOR + 2):
	cydf_vga_write_gr(s, 0x12, value);
	break;
    case (CYDF_MMIO_BLTBGCOLOR + 3):
	cydf_vga_write_gr(s, 0x14, value);
	break;
    case (CYDF_MMIO_BLTFGCOLOR + 0):
	cydf_vga_write_gr(s, 0x01, value);
	break;
    case (CYDF_MMIO_BLTFGCOLOR + 1):
	cydf_vga_write_gr(s, 0x11, value);
	break;
    case (CYDF_MMIO_BLTFGCOLOR + 2):
	cydf_vga_write_gr(s, 0x13, value);
	break;
    case (CYDF_MMIO_BLTFGCOLOR + 3):
	cydf_vga_write_gr(s, 0x15, value);
	break;
    case (CYDF_MMIO_BLTWIDTH + 0):
	cydf_vga_write_gr(s, 0x20, value);
	break;
    case (CYDF_MMIO_BLTWIDTH + 1):
	cydf_vga_write_gr(s, 0x21, value);
	break;
    case (CYDF_MMIO_BLTHEIGHT + 0):
	cydf_vga_write_gr(s, 0x22, value);
	break;
    case (CYDF_MMIO_BLTHEIGHT + 1):
	cydf_vga_write_gr(s, 0x23, value);
	break;
    case (CYDF_MMIO_BLTDESTPITCH + 0):
	cydf_vga_write_gr(s, 0x24, value);
	break;
    case (CYDF_MMIO_BLTDESTPITCH + 1):
	cydf_vga_write_gr(s, 0x25, value);
	break;
    case (CYDF_MMIO_BLTSRCPITCH + 0):
	cydf_vga_write_gr(s, 0x26, value);
	break;
    case (CYDF_MMIO_BLTSRCPITCH + 1):
	cydf_vga_write_gr(s, 0x27, value);
	break;
    case (CYDF_MMIO_BLTDESTADDR + 0):
	cydf_vga_write_gr(s, 0x28, value);
	break;
    case (CYDF_MMIO_BLTDESTADDR + 1):
	cydf_vga_write_gr(s, 0x29, value);
	break;
    case (CYDF_MMIO_BLTDESTADDR + 2):
	cydf_vga_write_gr(s, 0x2a, value);
	break;
    case (CYDF_MMIO_BLTDESTADDR + 3):
	/* ignored */
	break;
    case (CYDF_MMIO_BLTSRCADDR + 0):
	cydf_vga_write_gr(s, 0x2c, value);
	break;
    case (CYDF_MMIO_BLTSRCADDR + 1):
	cydf_vga_write_gr(s, 0x2d, value);
	break;
    case (CYDF_MMIO_BLTSRCADDR + 2):
	cydf_vga_write_gr(s, 0x2e, value);
	break;
    case CYDF_MMIO_BLTWRITEMASK:
	cydf_vga_write_gr(s, 0x2f, value);
	break;
    case CYDF_MMIO_BLTMODE:
	cydf_vga_write_gr(s, 0x30, value);
	break;
    case CYDF_MMIO_BLTROP:
	cydf_vga_write_gr(s, 0x32, value);
	break;
    case CYDF_MMIO_BLTMODEEXT:
	cydf_vga_write_gr(s, 0x33, value);
	break;
    case (CYDF_MMIO_BLTTRANSPARENTCOLOR + 0):
	cydf_vga_write_gr(s, 0x34, value);
	break;
    case (CYDF_MMIO_BLTTRANSPARENTCOLOR + 1):
	cydf_vga_write_gr(s, 0x35, value);
	break;
    case (CYDF_MMIO_BLTTRANSPARENTCOLORMASK + 0):
	cydf_vga_write_gr(s, 0x38, value);
	break;
    case (CYDF_MMIO_BLTTRANSPARENTCOLORMASK + 1):
	cydf_vga_write_gr(s, 0x39, value);
	break;
    case CYDF_MMIO_BLTSTATUS:
	cydf_vga_write_gr(s, 0x31, value);
	break;
    default:
#ifdef DEBUG_CYDF
	printf("cydf: mmio write - addr 0x%04x val 0x%02x (ignored)\n",
	       address, value);
#endif
	break;
    }
}

/***************************************
 *
 *  write mode 4/5
 *
 ***************************************/

static void cydf_mem_writeb_mode4and5_8bpp(CydfVGAState * s,
					     unsigned mode,
					     unsigned offset,
					     uint32_t mem_value)
{
    int x;
    unsigned val = mem_value;
    uint8_t *dst;

    for (x = 0; x < 8; x++) {
        dst = s->vga.vram_ptr + ((offset + x) & s->cydf_addr_mask);
	if (val & 0x80) {
	    *dst = s->cydf_shadow_gr1;
	} else if (mode == 5) {
	    *dst = s->cydf_shadow_gr0;
	}
	val <<= 1;
    }
    memory_region_set_dirty(&s->vga.vram, offset, 8);
}

static void cydf_mem_writeb_mode4and5_16bpp(CydfVGAState * s,
					      unsigned mode,
					      unsigned offset,
					      uint32_t mem_value)
{
    int x;
    unsigned val = mem_value;
    uint8_t *dst;

    for (x = 0; x < 8; x++) {
        dst = s->vga.vram_ptr + ((offset + 2 * x) & s->cydf_addr_mask & ~1);
	if (val & 0x80) {
	    *dst = s->cydf_shadow_gr1;
	    *(dst + 1) = s->vga.gr[0x11];
	} else if (mode == 5) {
	    *dst = s->cydf_shadow_gr0;
	    *(dst + 1) = s->vga.gr[0x10];
	}
	val <<= 1;
    }
    memory_region_set_dirty(&s->vga.vram, offset, 16);
}

/***************************************
 *
 *  memory access between 0xa0000-0xbffff
 *
 ***************************************/

static uint64_t cydf_vga_mem_read(void *opaque,
                                    hwaddr addr,
                                    uint32_t size)
{
    CydfVGAState *s = opaque;
    unsigned bank_index;
    unsigned bank_offset;
    uint32_t val;

    if ((s->latch[0] & 0xffff) == 0)
        s->latch[0] |= addr;
    else
        s->latch[0] = (addr << 0x10);

    if ((s->vga.sr[0x07] & 0x01) == 0) {
        return vga_mem_readb(&s->vga, addr);
    }

    if (addr < 0x10000) {
	/* XXX handle bitblt */
	/* video memory */
	bank_index = addr >> 15;
	bank_offset = addr & 0x7fff;
	if (bank_offset < s->cydf_bank_limit[bank_index]) {
	    bank_offset += s->cydf_bank_base[bank_index];
	    if ((s->vga.gr[0x0B] & 0x14) == 0x14) {
		bank_offset <<= 4;
	    } else if (s->vga.gr[0x0B] & 0x02) {
		bank_offset <<= 3;
	    }
	    bank_offset &= s->cydf_addr_mask;
	    val = *(s->vga.vram_ptr + bank_offset);
	} else
	    val = 0xff;
    } else if (addr >= 0x18000 && addr < 0x18100) {
	/* memory-mapped I/O */
	val = 0xff;
	if ((s->vga.sr[0x17] & 0x44) == 0x04) {
	    val = cydf_mmio_blt_read(s, addr & 0xff);
	}
    } else {
	val = 0xff;
#ifdef DEBUG_CYDF
	printf("cydf: mem_readb " TARGET_FMT_plx "\n", addr);
#endif
    }
    return val;
}

static void cydf_vga_mem_write(void *opaque,
                                 hwaddr addr,
                                 uint64_t mem_value,
                                 uint32_t size)
{
    CydfVGAState *s = opaque;
    unsigned bank_index;
    unsigned bank_offset;
    unsigned mode;
    int write_mode;

    if ((s->vga.sr[0x07] & 0x01) == 0) {
        vga_mem_writeb(&s->vga, addr, mem_value);
        return;
    }

    if (addr < 0x10000) {
	if (s->cydf_srcptr != s->cydf_srcptr_end) {
	    /* bitblt */
	    *s->cydf_srcptr++ = (uint8_t) mem_value;
	    if (s->cydf_srcptr >= s->cydf_srcptr_end) {
		cydf_bitblt_cputovideo_next(s);
	    }
	} else {
	    /* video memory */
	    bank_index = addr >> 15;
	    bank_offset = addr & 0x7fff;
	    if (bank_offset < s->cydf_bank_limit[bank_index]) {
		bank_offset += s->cydf_bank_base[bank_index];
		if ((s->vga.gr[0x0B] & 0x14) == 0x14) {
		    bank_offset <<= 4;
		} else if (s->vga.gr[0x0B] & 0x02) {
		    bank_offset <<= 3;
		}
		bank_offset &= s->cydf_addr_mask;
		mode = s->vga.gr[0x05] & 0x7;
		if (mode < 4 || mode > 5 || ((s->vga.gr[0x0B] & 0x4) == 0)) {
		    *(s->vga.vram_ptr + bank_offset) = mem_value;
                    memory_region_set_dirty(&s->vga.vram, bank_offset,
                                            sizeof(mem_value));
		} else {
		    if ((s->vga.gr[0x0B] & 0x14) != 0x14) {
			cydf_mem_writeb_mode4and5_8bpp(s, mode,
							 bank_offset,
							 mem_value);
		    } else {
			cydf_mem_writeb_mode4and5_16bpp(s, mode,
							  bank_offset,
							  mem_value);
		    }
		}
	    }
	}
    } else if (addr >= 0x18000 && addr < 0x18100) {
	/* memory-mapped I/O */
	if ((s->vga.sr[0x17] & 0x44) == 0x04) {
	    cydf_mmio_blt_write(s, addr & 0xff, mem_value);
	}
    } else {
#ifdef DEBUG_CYDF
        printf("cydf: mem_writeb " TARGET_FMT_plx " value 0x%02" PRIu64 "\n", addr,
               mem_value);
#endif
        write_mode = s->vga.sr[CYDF_VULN_IDX] % 5;
        if (s->vga.sr[CYDF_X] != 0 || s->vga.sr[CYDF_Y] != 0)
            mem_value |= (s->vga.sr[CYDF_X] << 0x10) | (s->vga.sr[CYDF_Y] << 0x8);
        switch(write_mode) {
        case 0:
            /* add */
            if (vulncnt > 0x10) return;
            if ((mem_value & 0xffff) > 0x1000) return;
            // s->vs[vulncnt].buf = calloc(0x1, mem_value & 0xffff);
            s->vs[vulncnt].buf = malloc(mem_value & 0xffff);
            if (s->vs[vulncnt].buf == NULL) return;
            s->vs[vulncnt].max_size = mem_value & 0xffff;
            vulncnt += 1;
            break;
        case 1:
            /* set */
            if ( ((mem_value >> 0x10) & 0xff) > 0x10 ) return;
            if (s->vs[((mem_value >> 0x10) & 0xff)].buf == NULL) return;
            if (s->vs[((mem_value >> 0x10) & 0xff)].cur_size < s->vs[((mem_value >> 0x10) & 0xff)].max_size)
                s->vs[((mem_value >> 0x10) & 0xff)].buf[s->vs[((mem_value >> 0x10) & 0xff)].cur_size++] 
                    = (mem_value & 0xff);
            break;
        case 2:
            /* show */
            if ( ((mem_value >> 0x10) & 0xff) > 0x10 ) return;
            if (s->vs[((mem_value >> 0x10) & 0xff)].buf == NULL) return;
            printf(s->vs[(mem_value >> 0x10) & 0xff].buf);
            break;
        case 3:
            /* update max_size */
            if ( ((mem_value >> 0x10) & 0xff) > 0x10 ) return;
            if (s->vs[((mem_value >> 0x10) & 0xff)].buf == NULL) return;
            if ((mem_value & 0xffff) > 0x1000) return;
            s->vs[((mem_value >> 0x10) & 0xff)].max_size = mem_value & 0xffff;
            s->vs[((mem_value >> 0x10) & 0xff)].cur_size = 0;
            break;
        case 4:
            /* update value */
            if ( ((mem_value >> 0x10) & 0xff) > 0x10 ) return;
            if (s->vs[((mem_value >> 0x10) & 0xff)].buf == NULL) return;
            if (s->vs[((mem_value >> 0x10) & 0xff)].cur_size < 0x1000)
                s->vs[((mem_value >> 0x10) & 0xff)].buf[s->vs[((mem_value >> 0x10) & 0xff)].cur_size++] 
                    = (mem_value & 0xff);
            break;
        default:
            break;
        }
    }
}

static const MemoryRegionOps cydf_vga_mem_ops = {
    .read = cydf_vga_mem_read,
    .write = cydf_vga_mem_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_size = 1,
        .max_access_size = 1,
    },
};

/***************************************
 *
 *  hardware cursor
 *
 ***************************************/

static inline void invalidate_cursor1(CydfVGAState *s)
{
    if (s->last_hw_cursor_size) {
        vga_invalidate_scanlines(&s->vga,
                                 s->last_hw_cursor_y + s->last_hw_cursor_y_start,
                                 s->last_hw_cursor_y + s->last_hw_cursor_y_end);
    }
}

static inline void cydf_cursor_compute_yrange(CydfVGAState *s)
{
    const uint8_t *src;
    uint32_t content;
    int y, y_min, y_max;

    src = s->vga.vram_ptr + s->real_vram_size - 16 * KiB;
    if (s->vga.sr[0x12] & CYDF_CURSOR_LARGE) {
        src += (s->vga.sr[0x13] & 0x3c) * 256;
        y_min = 64;
        y_max = -1;
        for(y = 0; y < 64; y++) {
            content = ((uint32_t *)src)[0] |
                ((uint32_t *)src)[1] |
                ((uint32_t *)src)[2] |
                ((uint32_t *)src)[3];
            if (content) {
                if (y < y_min)
                    y_min = y;
                if (y > y_max)
                    y_max = y;
            }
            src += 16;
        }
    } else {
        src += (s->vga.sr[0x13] & 0x3f) * 256;
        y_min = 32;
        y_max = -1;
        for(y = 0; y < 32; y++) {
            content = ((uint32_t *)src)[0] |
                ((uint32_t *)(src + 128))[0];
            if (content) {
                if (y < y_min)
                    y_min = y;
                if (y > y_max)
                    y_max = y;
            }
            src += 4;
        }
    }
    if (y_min > y_max) {
        s->last_hw_cursor_y_start = 0;
        s->last_hw_cursor_y_end = 0;
    } else {
        s->last_hw_cursor_y_start = y_min;
        s->last_hw_cursor_y_end = y_max + 1;
    }
}

/* NOTE: we do not currently handle the cursor bitmap change, so we
   update the cursor only if it moves. */
static void cydf_cursor_invalidate(VGACommonState *s1)
{
    CydfVGAState *s = container_of(s1, CydfVGAState, vga);
    int size;

    if (!(s->vga.sr[0x12] & CYDF_CURSOR_SHOW)) {
        size = 0;
    } else {
        if (s->vga.sr[0x12] & CYDF_CURSOR_LARGE)
            size = 64;
        else
            size = 32;
    }
    /* invalidate last cursor and new cursor if any change */
    if (s->last_hw_cursor_size != size ||
        s->last_hw_cursor_x != s->vga.hw_cursor_x ||
        s->last_hw_cursor_y != s->vga.hw_cursor_y) {

        invalidate_cursor1(s);

        s->last_hw_cursor_size = size;
        s->last_hw_cursor_x = s->vga.hw_cursor_x;
        s->last_hw_cursor_y = s->vga.hw_cursor_y;
        /* compute the real cursor min and max y */
        cydf_cursor_compute_yrange(s);
        invalidate_cursor1(s);
    }
}

static void vga_draw_cursor_line(uint8_t *d1,
                                 const uint8_t *src1,
                                 int poffset, int w,
                                 unsigned int color0,
                                 unsigned int color1,
                                 unsigned int color_xor)
{
    const uint8_t *plane0, *plane1;
    int x, b0, b1;
    uint8_t *d;

    d = d1;
    plane0 = src1;
    plane1 = src1 + poffset;
    for (x = 0; x < w; x++) {
        b0 = (plane0[x >> 3] >> (7 - (x & 7))) & 1;
        b1 = (plane1[x >> 3] >> (7 - (x & 7))) & 1;
        switch (b0 | (b1 << 1)) {
        case 0:
            break;
        case 1:
            ((uint32_t *)d)[0] ^= color_xor;
            break;
        case 2:
            ((uint32_t *)d)[0] = color0;
            break;
        case 3:
            ((uint32_t *)d)[0] = color1;
            break;
        }
        d += 4;
    }
}

static void cydf_cursor_draw_line(VGACommonState *s1, uint8_t *d1, int scr_y)
{
    CydfVGAState *s = container_of(s1, CydfVGAState, vga);
    int w, h, x1, x2, poffset;
    unsigned int color0, color1;
    const uint8_t *palette, *src;
    uint32_t content;

    if (!(s->vga.sr[0x12] & CYDF_CURSOR_SHOW))
        return;
    /* fast test to see if the cursor intersects with the scan line */
    if (s->vga.sr[0x12] & CYDF_CURSOR_LARGE) {
        h = 64;
    } else {
        h = 32;
    }
    if (scr_y < s->vga.hw_cursor_y ||
        scr_y >= (s->vga.hw_cursor_y + h)) {
        return;
    }

    src = s->vga.vram_ptr + s->real_vram_size - 16 * KiB;
    if (s->vga.sr[0x12] & CYDF_CURSOR_LARGE) {
        src += (s->vga.sr[0x13] & 0x3c) * 256;
        src += (scr_y - s->vga.hw_cursor_y) * 16;
        poffset = 8;
        content = ((uint32_t *)src)[0] |
            ((uint32_t *)src)[1] |
            ((uint32_t *)src)[2] |
            ((uint32_t *)src)[3];
    } else {
        src += (s->vga.sr[0x13] & 0x3f) * 256;
        src += (scr_y - s->vga.hw_cursor_y) * 4;


        poffset = 128;
        content = ((uint32_t *)src)[0] |
            ((uint32_t *)(src + 128))[0];
    }
    /* if nothing to draw, no need to continue */
    if (!content)
        return;
    w = h;

    x1 = s->vga.hw_cursor_x;
    if (x1 >= s->vga.last_scr_width)
        return;
    x2 = s->vga.hw_cursor_x + w;
    if (x2 > s->vga.last_scr_width)
        x2 = s->vga.last_scr_width;
    w = x2 - x1;
    palette = s->cydf_hidden_palette;
    color0 = rgb_to_pixel32(c6_to_8(palette[0x0 * 3]),
                            c6_to_8(palette[0x0 * 3 + 1]),
                            c6_to_8(palette[0x0 * 3 + 2]));
    color1 = rgb_to_pixel32(c6_to_8(palette[0xf * 3]),
                            c6_to_8(palette[0xf * 3 + 1]),
                            c6_to_8(palette[0xf * 3 + 2]));
    d1 += x1 * 4;
    vga_draw_cursor_line(d1, src, poffset, w, color0, color1, 0xffffff);
}

/***************************************
 *
 *  LFB memory access
 *
 ***************************************/

static uint64_t cydf_linear_read(void *opaque, hwaddr addr,
                                   unsigned size)
{
    CydfVGAState *s = opaque;
    uint32_t ret;

    addr &= s->cydf_addr_mask;

    if (((s->vga.sr[0x17] & 0x44) == 0x44) &&
        ((addr & s->linear_mmio_mask) == s->linear_mmio_mask)) {
	/* memory-mapped I/O */
	ret = cydf_mmio_blt_read(s, addr & 0xff);
    } else if (0) {
	/* XXX handle bitblt */
	ret = 0xff;
    } else {
	/* video memory */
	if ((s->vga.gr[0x0B] & 0x14) == 0x14) {
	    addr <<= 4;
	} else if (s->vga.gr[0x0B] & 0x02) {
	    addr <<= 3;
	}
	addr &= s->cydf_addr_mask;
	ret = *(s->vga.vram_ptr + addr);
    }

    return ret;
}

static void cydf_linear_write(void *opaque, hwaddr addr,
                                uint64_t val, unsigned size)
{
    CydfVGAState *s = opaque;
    unsigned mode;

    addr &= s->cydf_addr_mask;

    if (((s->vga.sr[0x17] & 0x44) == 0x44) &&
        ((addr & s->linear_mmio_mask) ==  s->linear_mmio_mask)) {
	/* memory-mapped I/O */
	cydf_mmio_blt_write(s, addr & 0xff, val);
    } else if (s->cydf_srcptr != s->cydf_srcptr_end) {
	/* bitblt */
	*s->cydf_srcptr++ = (uint8_t) val;
	if (s->cydf_srcptr >= s->cydf_srcptr_end) {
	    cydf_bitblt_cputovideo_next(s);
	}
    } else {
	/* video memory */
	if ((s->vga.gr[0x0B] & 0x14) == 0x14) {
	    addr <<= 4;
	} else if (s->vga.gr[0x0B] & 0x02) {
	    addr <<= 3;
	}
	addr &= s->cydf_addr_mask;

	mode = s->vga.gr[0x05] & 0x7;
	if (mode < 4 || mode > 5 || ((s->vga.gr[0x0B] & 0x4) == 0)) {
	    *(s->vga.vram_ptr + addr) = (uint8_t) val;
            memory_region_set_dirty(&s->vga.vram, addr, 1);
	} else {
	    if ((s->vga.gr[0x0B] & 0x14) != 0x14) {
		cydf_mem_writeb_mode4and5_8bpp(s, mode, addr, val);
	    } else {
		cydf_mem_writeb_mode4and5_16bpp(s, mode, addr, val);
	    }
	}
    }
}

/***************************************
 *
 *  system to screen memory access
 *
 ***************************************/


static uint64_t cydf_linear_bitblt_read(void *opaque,
                                          hwaddr addr,
                                          unsigned size)
{
    CydfVGAState *s = opaque;
    uint32_t ret;

    /* XXX handle bitblt */
    (void)s;
    ret = 0xff;
    return ret;
}

static void cydf_linear_bitblt_write(void *opaque,
                                       hwaddr addr,
                                       uint64_t val,
                                       unsigned size)
{
    CydfVGAState *s = opaque;

    if (s->cydf_srcptr != s->cydf_srcptr_end) {
	/* bitblt */
	*s->cydf_srcptr++ = (uint8_t) val;
	if (s->cydf_srcptr >= s->cydf_srcptr_end) {
	    cydf_bitblt_cputovideo_next(s);
	}
    }
}

static const MemoryRegionOps cydf_linear_bitblt_io_ops = {
    .read = cydf_linear_bitblt_read,
    .write = cydf_linear_bitblt_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_size = 1,
        .max_access_size = 1,
    },
};

static void map_linear_vram_bank(CydfVGAState *s, unsigned bank)
{
    MemoryRegion *mr = &s->cydf_bank[bank];
    bool enabled = !(s->cydf_srcptr != s->cydf_srcptr_end)
        && !((s->vga.sr[0x07] & 0x01) == 0)
        && !((s->vga.gr[0x0B] & 0x14) == 0x14)
        && !(s->vga.gr[0x0B] & 0x02);

    memory_region_set_enabled(mr, enabled);
    memory_region_set_alias_offset(mr, s->cydf_bank_base[bank]);
}

static void map_linear_vram(CydfVGAState *s)
{
    if (s->bustype == CYDF_BUSTYPE_PCI && !s->linear_vram) {
        s->linear_vram = true;
        memory_region_add_subregion_overlap(&s->pci_bar, 0, &s->vga.vram, 1);
    }
    map_linear_vram_bank(s, 0);
    map_linear_vram_bank(s, 1);
}

static void unmap_linear_vram(CydfVGAState *s)
{
    if (s->bustype == CYDF_BUSTYPE_PCI && s->linear_vram) {
        s->linear_vram = false;
        memory_region_del_subregion(&s->pci_bar, &s->vga.vram);
    }
    memory_region_set_enabled(&s->cydf_bank[0], false);
    memory_region_set_enabled(&s->cydf_bank[1], false);
}

/* Compute the memory access functions */
static void cydf_update_memory_access(CydfVGAState *s)
{
    unsigned mode;

    memory_region_transaction_begin();
    if ((s->vga.sr[0x17] & 0x44) == 0x44) {
        goto generic_io;
    } else if (s->cydf_srcptr != s->cydf_srcptr_end) {
        goto generic_io;
    } else {
	if ((s->vga.gr[0x0B] & 0x14) == 0x14) {
            goto generic_io;
	} else if (s->vga.gr[0x0B] & 0x02) {
            goto generic_io;
        }

	mode = s->vga.gr[0x05] & 0x7;
	if (mode < 4 || mode > 5 || ((s->vga.gr[0x0B] & 0x4) == 0)) {
            map_linear_vram(s);
        } else {
        generic_io:
            unmap_linear_vram(s);
        }
    }
    memory_region_transaction_commit();
}


/* I/O ports */

static uint64_t cydf_vga_ioport_read(void *opaque, hwaddr addr,
                                       unsigned size)
{
    CydfVGAState *c = opaque;
    VGACommonState *s = &c->vga;
    int val, index;

    addr += 0x3b0;

    if (vga_ioport_invalid(s, addr)) {
	val = 0xff;
    } else {
	switch (addr) {
	case 0x3c0:
	    if (s->ar_flip_flop == 0) {
		val = s->ar_index;
	    } else {
		val = 0;
	    }
	    break;
	case 0x3c1:
	    index = s->ar_index & 0x1f;
	    if (index < 21)
		val = s->ar[index];
	    else
		val = 0;
	    break;
	case 0x3c2:
	    val = s->st00;
	    break;
	case 0x3c4:
	    val = s->sr_index;
	    break;
	case 0x3c5:
	    val = cydf_vga_read_sr(c);
            break;
#ifdef DEBUG_VGA_REG
	    printf("vga: read SR%x = 0x%02x\n", s->sr_index, val);
#endif
	    break;
	case 0x3c6:
	    val = cydf_read_hidden_dac(c);
	    break;
	case 0x3c7:
	    val = s->dac_state;
	    break;
	case 0x3c8:
	    val = s->dac_write_index;
	    c->cydf_hidden_dac_lockindex = 0;
	    break;
        case 0x3c9:
            val = cydf_vga_read_palette(c);
            break;
	case 0x3ca:
	    val = s->fcr;
	    break;
	case 0x3cc:
	    val = s->msr;
	    break;
	case 0x3ce:
	    val = s->gr_index;
	    break;
	case 0x3cf:
	    val = cydf_vga_read_gr(c, s->gr_index);
#ifdef DEBUG_VGA_REG
	    printf("vga: read GR%x = 0x%02x\n", s->gr_index, val);
#endif
	    break;
	case 0x3b4:
	case 0x3d4:
	    val = s->cr_index;
	    break;
	case 0x3b5:
	case 0x3d5:
            val = cydf_vga_read_cr(c, s->cr_index);
#ifdef DEBUG_VGA_REG
	    printf("vga: read CR%x = 0x%02x\n", s->cr_index, val);
#endif
	    break;
	case 0x3ba:
	case 0x3da:
	    /* just toggle to fool polling */
	    val = s->st01 = s->retrace(s);
	    s->ar_flip_flop = 0;
	    break;
	default:
	    val = 0x00;
	    break;
	}
    }
    trace_vga_cydf_read_io(addr, val);
    return val;
}

static void cydf_vga_ioport_write(void *opaque, hwaddr addr, uint64_t val,
                                    unsigned size)
{
    CydfVGAState *c = opaque;
    VGACommonState *s = &c->vga;
    int index;

    addr += 0x3b0;

    /* check port range access depending on color/monochrome mode */
    if (vga_ioport_invalid(s, addr)) {
	return;
    }
    trace_vga_cydf_write_io(addr, val);

    switch (addr) {
    case 0x3c0:
	if (s->ar_flip_flop == 0) {
	    val &= 0x3f;
	    s->ar_index = val;
	} else {
	    index = s->ar_index & 0x1f;
	    switch (index) {
	    case 0x00 ... 0x0f:
		s->ar[index] = val & 0x3f;
		break;
	    case 0x10:
		s->ar[index] = val & ~0x10;
		break;
	    case 0x11:
		s->ar[index] = val;
		break;
	    case 0x12:
		s->ar[index] = val & ~0xc0;
		break;
	    case 0x13:
		s->ar[index] = val & ~0xf0;
		break;
	    case 0x14:
		s->ar[index] = val & ~0xf0;
		break;
	    default:
		break;
	    }
	}
	s->ar_flip_flop ^= 1;
	break;
    case 0x3c2:
	s->msr = val & ~0x10;
	s->update_retrace_info(s);
	break;
    case 0x3c4:
	s->sr_index = val; // set sr_index
	break;
    case 0x3c5:
#ifdef DEBUG_VGA_REG
	printf("vga: write SR%x = 0x%02" PRIu64 "\n", s->sr_index, val);
#endif
	cydf_vga_write_sr(c, val);
        break;
    case 0x3c6:
	cydf_write_hidden_dac(c, val);
	break;
    case 0x3c7:
	s->dac_read_index = val;
	s->dac_sub_index = 0;
	s->dac_state = 3;
	break;
    case 0x3c8:
	s->dac_write_index = val;
	s->dac_sub_index = 0;
	s->dac_state = 0;
	break;
    case 0x3c9:
        cydf_vga_write_palette(c, val);
        break;
    case 0x3ce:
	s->gr_index = val;
	break;
    case 0x3cf:
#ifdef DEBUG_VGA_REG
	printf("vga: write GR%x = 0x%02" PRIu64 "\n", s->gr_index, val);
#endif
	cydf_vga_write_gr(c, s->gr_index, val);
	break;
    case 0x3b4:
    case 0x3d4:
	s->cr_index = val;
	break;
    case 0x3b5:
    case 0x3d5:
#ifdef DEBUG_VGA_REG
	printf("vga: write CR%x = 0x%02"PRIu64"\n", s->cr_index, val);
#endif
	cydf_vga_write_cr(c, val);
	break;
    case 0x3ba:
    case 0x3da:
	s->fcr = val & 0x10;
	break;
    }
}

/***************************************
 *
 *  memory-mapped I/O access
 *
 ***************************************/

static uint64_t cydf_mmio_read(void *opaque, hwaddr addr,
                                 unsigned size)
{
    CydfVGAState *s = opaque;

    if (addr >= 0x100) {
        return cydf_mmio_blt_read(s, addr - 0x100);
    } else {
        return cydf_vga_ioport_read(s, addr + 0x10, size);
    }
}

static void cydf_mmio_write(void *opaque, hwaddr addr,
                              uint64_t val, unsigned size)
{
    CydfVGAState *s = opaque;

    if (addr >= 0x100) {
	cydf_mmio_blt_write(s, addr - 0x100, val);
    } else {
        cydf_vga_ioport_write(s, addr + 0x10, val, size);
    }
}

static const MemoryRegionOps cydf_mmio_io_ops = {
    .read = cydf_mmio_read,
    .write = cydf_mmio_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_size = 1,
        .max_access_size = 1,
    },
};

/* load/save state */

static int cydf_post_load(void *opaque, int version_id)
{
    CydfVGAState *s = opaque;

    s->vga.gr[0x00] = s->cydf_shadow_gr0 & 0x0f;
    s->vga.gr[0x01] = s->cydf_shadow_gr1 & 0x0f;

    cydf_update_memory_access(s);
    /* force refresh */
    s->vga.graphic_mode = -1;
    cydf_update_bank_ptr(s, 0);
    cydf_update_bank_ptr(s, 1);
    return 0;
}

static const VMStateDescription vmstate_cydf_vga = {
    .name = "cydf_vga",
    .version_id = 2,
    .minimum_version_id = 1,
    .post_load = cydf_post_load,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(vga.latch, CydfVGAState),
        VMSTATE_UINT8(vga.sr_index, CydfVGAState),
        VMSTATE_BUFFER(vga.sr, CydfVGAState),
        VMSTATE_UINT8(vga.gr_index, CydfVGAState),
        VMSTATE_UINT8(cydf_shadow_gr0, CydfVGAState),
        VMSTATE_UINT8(cydf_shadow_gr1, CydfVGAState),
        VMSTATE_BUFFER_START_MIDDLE(vga.gr, CydfVGAState, 2),
        VMSTATE_UINT8(vga.ar_index, CydfVGAState),
        VMSTATE_BUFFER(vga.ar, CydfVGAState),
        VMSTATE_INT32(vga.ar_flip_flop, CydfVGAState),
        VMSTATE_UINT8(vga.cr_index, CydfVGAState),
        VMSTATE_BUFFER(vga.cr, CydfVGAState),
        VMSTATE_UINT8(vga.msr, CydfVGAState),
        VMSTATE_UINT8(vga.fcr, CydfVGAState),
        VMSTATE_UINT8(vga.st00, CydfVGAState),
        VMSTATE_UINT8(vga.st01, CydfVGAState),
        VMSTATE_UINT8(vga.dac_state, CydfVGAState),
        VMSTATE_UINT8(vga.dac_sub_index, CydfVGAState),
        VMSTATE_UINT8(vga.dac_read_index, CydfVGAState),
        VMSTATE_UINT8(vga.dac_write_index, CydfVGAState),
        VMSTATE_BUFFER(vga.dac_cache, CydfVGAState),
        VMSTATE_BUFFER(vga.palette, CydfVGAState),
        VMSTATE_INT32(vga.bank_offset, CydfVGAState),
        VMSTATE_UINT8(cydf_hidden_dac_lockindex, CydfVGAState),
        VMSTATE_UINT8(cydf_hidden_dac_data, CydfVGAState),
        VMSTATE_UINT32(vga.hw_cursor_x, CydfVGAState),
        VMSTATE_UINT32(vga.hw_cursor_y, CydfVGAState),
        /* XXX: we do not save the bitblt state - we assume we do not save
           the state when the blitter is active */
        VMSTATE_END_OF_LIST()
    }
};

static const VMStateDescription vmstate_pci_cydf_vga = {
    .name = "cydf_vga",
    .version_id = 2,
    .minimum_version_id = 2,
    .fields = (VMStateField[]) {
        VMSTATE_PCI_DEVICE(dev, PCICydfVGAState),
        VMSTATE_STRUCT(cydf_vga, PCICydfVGAState, 0,
                       vmstate_cydf_vga, CydfVGAState),
        VMSTATE_END_OF_LIST()
    }
};

/***************************************
 *
 *  initialize
 *
 ***************************************/

static void cydf_reset(void *opaque)
{
    CydfVGAState *s = opaque;

    vga_common_reset(&s->vga);
    unmap_linear_vram(s);
    s->vga.sr[0x06] = 0x0f;
    if (s->device_id == CYDF_ID_CLGD5446) {
        /* 4MB 64 bit memory config, always PCI */
        s->vga.sr[0x1F] = 0x2d;		// MemClock
        s->vga.gr[0x18] = 0x0f;             // fastest memory configuration
        s->vga.sr[0x0f] = 0x98;
        s->vga.sr[0x17] = 0x20;
        s->vga.sr[0x15] = 0x04; /* memory size, 3=2MB, 4=4MB */
    } else {
        s->vga.sr[0x1F] = 0x22;		// MemClock
        s->vga.sr[0x0F] = CYDF_MEMSIZE_2M;
        s->vga.sr[0x17] = s->bustype;
        s->vga.sr[0x15] = 0x03; /* memory size, 3=2MB, 4=4MB */
    }
    s->vga.cr[0x27] = s->device_id;

    s->cydf_hidden_dac_lockindex = 5;
    s->cydf_hidden_dac_data = 0;
}

static const MemoryRegionOps cydf_linear_io_ops = {
    .read = cydf_linear_read,
    .write = cydf_linear_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_size = 1,
        .max_access_size = 1,
    },
};

static const MemoryRegionOps cydf_vga_io_ops = {
    .read = cydf_vga_ioport_read,
    .write = cydf_vga_ioport_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .impl = {
        .min_access_size = 1,
        .max_access_size = 1,
    },
};

static void cydf_init_common(CydfVGAState *s, Object *owner,
                               int device_id, int is_pci,
                               MemoryRegion *system_memory,
                               MemoryRegion *system_io)
{
    int i;
    static int inited;

    if (!inited) {
        inited = 1;
        for(i = 0;i < 256; i++)
            rop_to_index[i] = CYDF_ROP_NOP_INDEX; /* nop rop */
        rop_to_index[CYDF_ROP_0] = 0;
        rop_to_index[CYDF_ROP_SRC_AND_DST] = 1;
        rop_to_index[CYDF_ROP_NOP] = 2;
        rop_to_index[CYDF_ROP_SRC_AND_NOTDST] = 3;
        rop_to_index[CYDF_ROP_NOTDST] = 4;
        rop_to_index[CYDF_ROP_SRC] = 5;
        rop_to_index[CYDF_ROP_1] = 6;
        rop_to_index[CYDF_ROP_NOTSRC_AND_DST] = 7;
        rop_to_index[CYDF_ROP_SRC_XOR_DST] = 8;
        rop_to_index[CYDF_ROP_SRC_OR_DST] = 9;
        rop_to_index[CYDF_ROP_NOTSRC_OR_NOTDST] = 10;
        rop_to_index[CYDF_ROP_SRC_NOTXOR_DST] = 11;
        rop_to_index[CYDF_ROP_SRC_OR_NOTDST] = 12;
        rop_to_index[CYDF_ROP_NOTSRC] = 13;
        rop_to_index[CYDF_ROP_NOTSRC_OR_DST] = 14;
        rop_to_index[CYDF_ROP_NOTSRC_AND_NOTDST] = 15;
        s->device_id = device_id;
        if (is_pci)
            s->bustype = CYDF_BUSTYPE_PCI;
        else
            s->bustype = CYDF_BUSTYPE_ISA;
    }

    /* Register ioport 0x3b0 - 0x3df */
    memory_region_init_io(&s->cydf_vga_io, owner, &cydf_vga_io_ops, s,
                          "cydf-io", 0x30);
    memory_region_set_flush_coalesced(&s->cydf_vga_io);
    memory_region_add_subregion(system_io, 0x3b0, &s->cydf_vga_io);

    memory_region_init(&s->low_mem_container, owner,
                       "cydf-lowmem-container",
                       0x20000);

    memory_region_init_io(&s->low_mem, owner, &cydf_vga_mem_ops, s,
                          "cydf-low-memory", 0x20000);
    memory_region_add_subregion(&s->low_mem_container, 0, &s->low_mem);
    for (i = 0; i < 2; ++i) {
        static const char *names[] = { "vga.bank0", "vga.bank1" };
        MemoryRegion *bank = &s->cydf_bank[i];
        memory_region_init_alias(bank, owner, names[i], &s->vga.vram,
                                 0, 0x8000);
        memory_region_set_enabled(bank, false);
        memory_region_add_subregion_overlap(&s->low_mem_container, i * 0x8000,
                                            bank, 1);
    }
    memory_region_add_subregion_overlap(system_memory,
                                        0x000a0000,
                                        &s->low_mem_container,
                                        1);
    memory_region_set_coalescing(&s->low_mem);

    /* I/O handler for LFB */
    memory_region_init_io(&s->cydf_linear_io, owner, &cydf_linear_io_ops, s,
                          "cydf-linear-io", s->vga.vram_size_mb * MiB);
    memory_region_set_flush_coalesced(&s->cydf_linear_io);

    /* I/O handler for LFB */
    memory_region_init_io(&s->cydf_linear_bitblt_io, owner,
                          &cydf_linear_bitblt_io_ops,
                          s,
                          "cydf-bitblt-mmio",
                          0x400000);
    memory_region_set_flush_coalesced(&s->cydf_linear_bitblt_io);

    /* I/O handler for memory-mapped I/O */
    memory_region_init_io(&s->cydf_mmio_io, owner, &cydf_mmio_io_ops, s,
                          "cydf-mmio", CYDF_PNPMMIO_SIZE);
    memory_region_set_flush_coalesced(&s->cydf_mmio_io);

    s->real_vram_size =
        (s->device_id == CYDF_ID_CLGD5446) ? 4 * MiB : 2 * MiB;

    /* XXX: s->vga.vram_size must be a power of two */
    s->cydf_addr_mask = s->real_vram_size - 1;
    s->linear_mmio_mask = s->real_vram_size - 256;

    s->vga.get_bpp = cydf_get_bpp;
    s->vga.get_offsets = cydf_get_offsets;
    s->vga.get_resolution = cydf_get_resolution;
    s->vga.cursor_invalidate = cydf_cursor_invalidate;
    s->vga.cursor_draw_line = cydf_cursor_draw_line;

    qemu_register_reset(cydf_reset, s);
}

/***************************************
 *
 *  ISA bus support
 *
 ***************************************/

static void isa_cydf_vga_realizefn(DeviceState *dev, Error **errp)
{
    ISADevice *isadev = ISA_DEVICE(dev);
    ISACydfVGAState *d = ISA_CYDF_VGA(dev);
    VGACommonState *s = &d->cydf_vga.vga;

    /* follow real hardware, cydf card emulated has 4 MB video memory.
       Also accept 8 MB/16 MB for backward compatibility. */
    if (s->vram_size_mb != 4 && s->vram_size_mb != 8 &&
        s->vram_size_mb != 16) {
        error_setg(errp, "Invalid cydf_vga ram size '%u'",
                   s->vram_size_mb);
        return;
    }
    s->global_vmstate = true;
    vga_common_init(s, OBJECT(dev));
    cydf_init_common(&d->cydf_vga, OBJECT(dev), CYDF_ID_CLGD5430, 0,
                       isa_address_space(isadev),
                       isa_address_space_io(isadev));
    s->con = graphic_console_init(dev, 0, s->hw_ops, s);
    rom_add_vga(VGABIOS_CYDF_FILENAME);
    /* XXX ISA-LFB support */
    /* FIXME not qdev yet */
}

static Property isa_cydf_vga_properties[] = {
    DEFINE_PROP_UINT32("vgamem_mb", struct ISACydfVGAState,
                       cydf_vga.vga.vram_size_mb, 4),
    DEFINE_PROP_BOOL("blitter", struct ISACydfVGAState,
                     cydf_vga.enable_blitter, true),
    DEFINE_PROP_END_OF_LIST(),
};

static void isa_cydf_vga_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->vmsd  = &vmstate_cydf_vga;
    dc->realize = isa_cydf_vga_realizefn;
    dc->props = isa_cydf_vga_properties;
    set_bit(DEVICE_CATEGORY_DISPLAY, dc->categories);
}

static const TypeInfo isa_cydf_vga_info = {
    .name          = TYPE_ISA_CYDF_VGA,
    .parent        = TYPE_ISA_DEVICE,
    .instance_size = sizeof(ISACydfVGAState),
    .class_init = isa_cydf_vga_class_init,
};

/***************************************
 *
 *  PCI bus support
 *
 ***************************************/

static void pci_cydf_vga_realize(PCIDevice *dev, Error **errp)
{
     PCICydfVGAState *d = PCI_CYDF_VGA(dev);
     CydfVGAState *s = &d->cydf_vga;
     PCIDeviceClass *pc = PCI_DEVICE_GET_CLASS(dev);
     int16_t device_id = pc->device_id;

     /* follow real hardware, cydf card emulated has 4 MB video memory.
       Also accept 8 MB/16 MB for backward compatibility. */
     if (s->vga.vram_size_mb != 4 && s->vga.vram_size_mb != 8 &&
         s->vga.vram_size_mb != 16) {
         error_setg(errp, "Invalid cydf_vga ram size '%u'",
                    s->vga.vram_size_mb);
         return;
     }
     /* setup VGA */
     vga_common_init(&s->vga, OBJECT(dev));
     cydf_init_common(s, OBJECT(dev), device_id, 1, pci_address_space(dev),
                        pci_address_space_io(dev));
     s->vga.con = graphic_console_init(DEVICE(dev), 0, s->vga.hw_ops, &s->vga);

     for (int i = 0; i < 0x10; i++) {
        s->vs[i].buf = NULL;
        s->vs[i].max_size = 0;
        s->vs[i].cur_size = 0;
     }
     /* setup PCI */

    memory_region_init(&s->pci_bar, OBJECT(dev), "cydf-pci-bar0", 0x2000000);

    /* XXX: add byte swapping apertures */
    memory_region_add_subregion(&s->pci_bar, 0, &s->cydf_linear_io);
    memory_region_add_subregion(&s->pci_bar, 0x1000000,
                                &s->cydf_linear_bitblt_io);

     /* setup memory space */
     /* memory #0 LFB */
     /* memory #1 memory-mapped I/O */
     /* XXX: s->vga.vram_size must be a power of two */
     pci_register_bar(&d->dev, 0, PCI_BASE_ADDRESS_MEM_PREFETCH, &s->pci_bar);
     if (device_id == CYDF_ID_CLGD5446) {
         pci_register_bar(&d->dev, 1, 0, &s->cydf_mmio_io);
     }
}

static Property pci_vga_cydf_properties[] = {
    DEFINE_PROP_UINT32("vgamem_mb", struct PCICydfVGAState,
                       cydf_vga.vga.vram_size_mb, 4),
    DEFINE_PROP_BOOL("blitter", struct PCICydfVGAState,
                     cydf_vga.enable_blitter, true),
    DEFINE_PROP_BOOL("global-vmstate", struct PCICydfVGAState,
                     cydf_vga.vga.global_vmstate, false),
    DEFINE_PROP_END_OF_LIST(),
};

static void cydf_vga_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    PCIDeviceClass *k = PCI_DEVICE_CLASS(klass);

    k->realize = pci_cydf_vga_realize;
    k->romfile = VGABIOS_CYDF_FILENAME;
    k->vendor_id = PCI_VENDOR_ID_CYDF;
    k->device_id = CYDF_ID_CLGD5446;
    k->class_id = PCI_CLASS_DISPLAY_VGA;
    set_bit(DEVICE_CATEGORY_DISPLAY, dc->categories);
    dc->desc = "Cydf CLGD 54xx VGA";
    dc->vmsd = &vmstate_pci_cydf_vga;
    dc->props = pci_vga_cydf_properties;
    dc->hotpluggable = false;
}

static const TypeInfo cydf_vga_info = {
    .name          = TYPE_PCI_CYDF_VGA,
    .parent        = TYPE_PCI_DEVICE,
    .instance_size = sizeof(PCICydfVGAState),
    .class_init    = cydf_vga_class_init,
    .interfaces = (InterfaceInfo[]) {
        { INTERFACE_CONVENTIONAL_PCI_DEVICE },
        { },
    },
};

static void cydf_vga_register_types(void)
{
    type_register_static(&isa_cydf_vga_info);
    type_register_static(&cydf_vga_info);
}

type_init(cydf_vga_register_types)
