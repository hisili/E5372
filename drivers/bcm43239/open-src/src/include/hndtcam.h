/*
 * HND SOCRAM TCAM software interface.
 *
 * Copyright (C) 1999-2011, Broadcom Corporation
 * 
 *         Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed to you
 * under the terms of the GNU General Public License version 2 (the "GPL"),
 * available at http://www.broadcom.com/licenses/GPLv2.php, with the
 * following added to such license:
 * 
 *      As a special exception, the copyright holders of this software give you
 * permission to link this software with independent modules, and to copy and
 * distribute the resulting executable under terms of your choice, provided that
 * you also meet, for each linked independent module, the terms and conditions of
 * the license of that module.  An independent module is a module which is not
 * derived from this software.  The special exception does not apply to any
 * modifications of the software.
 * 
 *      Notwithstanding the above, under no circumstances may you combine this
 * software in any way with any other Broadcom software provided under a license
 * other than the GPL, without Broadcom's express prior written consent.
 *
 * $Id: hndtcam.h 277737 2011-08-16 17:54:59Z $
 */
#ifndef _hndtcam_h_
#define _hndtcam_h_

/*
 * 0 - 1
 * 1 - 2 Consecutive locations are patched
 * 2 - 4 Consecutive locations are patched
 * 3 - 8 Consecutive locations are patched
 * 4 - 16 Consecutive locations are patched
 * Define default to patch 2 locations
 */

#ifdef  PATCHCOUNT
#define SRPC_PATCHCOUNT PATCHCOUNT
#else
#define PATCHCOUNT 0
#define SRPC_PATCHCOUNT PATCHCOUNT
#endif

#ifdef BCM_BOOTLOADER
/* Patch count is hardcoded to 0 for boot loaders. */
#undef SRPC_PATCHCOUNT
#define SRPC_PATCHCOUNT	0
#endif

/* N Consecutive location to patch */
#define SRPC_PATCHNLOC (1 << (SRPC_PATCHCOUNT))

#define PATCHHDR(_p)		__attribute__ ((__section__ (".patchhdr."#_p))) _p
#define PATCHENTRY(_p)		__attribute__ ((__section__ (".patchentry."#_p))) _p

typedef struct {
	uint32	data[SRPC_PATCHNLOC];
} patch_entry_t;

typedef struct {
	void		*addr;		/* patch address */
	uint32		len;		/* bytes to patch in entry */
	patch_entry_t	*entry;		/* patch entry data */
} patch_hdr_t;

/* patch values and address structure */
typedef struct patchaddrvalue {
	uint32	addr;
	uint32	value;
} patchaddrvalue_t;

extern void *socram_regs;
extern uint32 socram_rev;

extern void hnd_patch_init(void *srp);
extern void hnd_tcam_write(void *srp, uint16 index, uint32 data);
extern void hnd_tcam_read(void *srp, uint16 index, uint32 *content);
void * hnd_tcam_init(void *srp, int no_addrs);
extern void hnd_tcam_disablepatch(void *srp);
extern void hnd_tcam_enablepatch(void *srp);
extern void hnd_tcam_load(void *srp, const  patchaddrvalue_t *patchtbl);
extern void hnd_tcam_load_default(void);
extern void hnd_tcam_reclaim(void);

#endif /* _hndtcam_h_ */
