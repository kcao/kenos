/* copied from linux v0.11 */
#ifndef _CONFIG_H
#define _CONFIG_H

/*
 * The HD_TYPE macro should look like this:
 *
 * #define HD_TYPE { head, sect, cyl, wpcom, lzone, ctl}
 *
 * In case of two harddisks, the info should be sepatated by
 * commas:
 *
 * #define HD_TYPE { h,s,c,wpcom,lz,ctl },{ h,s,c,wpcom,lz,ctl }
 */

/*
   This is an example, two drives, first is type 2, second is type 3:
  
  #define HD_TYPE { 4,17,615,300,615,8 }, { 6,17,615,300,615,0 }
  
   NOTE: ctl is 0 for all drives with heads<=8, and ctl=8 for drives
   with more than 8 heads.
  
   If you want the BIOS to tell what kind of drive you have, just
   leave HD_TYPE undefined. This is the normal thing to do.
*/

/* kenos hd_type */
/* 32MB */
/* 
#define HD_TYPE { head, spt, cyl, wpcom, lzone, ctl} 
*/

#define HD_TYPE { 16, 63, 65, 300, 65, 8 }

/*
physical:
---
Disk /dev/sda: 80.0 GB, 80026361856 bytes
255 heads, 63 sectors/track, 9729 cylinders
Units = cylinders of 16065 * 512 = 8225280 bytes
Disk identifier: 0x25a54f67

vb:
---
Disk /dev/hda: 33 MB, 33554432 bytes
16 heads, 63 sectors/track, 65 cylinders
Units = cylinders of 1008 * 512 = 516096 bytes
Disk identifier: 0x52f3f77c

bx:
---
Disk /dev/hda:  MB,  bytes
 heads, 63 sectors/track, 65 cylinders
Units = cylinders of  * 512 =  bytes
Disk identifier: 0x

*/

#endif


