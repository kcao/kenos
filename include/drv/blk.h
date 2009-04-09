/* copy for linux kernel source v0.11*/

#ifndef _BLK_H
#define _BLK_H

#define NULL	0

#define DEVICE_NAME "harddisk"
#define DEVICE_INTR do_hd


#ifdef DEVICE_INTR
void (*DEVICE_INTR)(void) = NULL;
#endif

static void (DEVICE_REQUEST)(void);

#endif


