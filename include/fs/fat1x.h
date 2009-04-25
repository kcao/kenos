/***************************************************************************
 *   Copyright (C) 2009 by Cao, Chen                                       *
 *   ken.ccao@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef	_KENOS_FAT1X_H_
#define	_KENOS_FAT1X_H_

#define CLUS_SIZE		512
/* root directory begin sector */
/* 0x2600 / 0x200 = 0x13 */
#define FAT_ROOT_BEG		19
#define FAT_ROOT_ENT_CNT	224
/* data area start sector */
/* 0x4200 / 0x200 = 0x21 */
#define FAT_DATA_BEG		33
#define FAT_TABENT_OFF		2

typedef int		iFILE;


#define	data_clus_offset(ent_nr) \
	(FAT_DATA_BEG + (ent_nr) - FAT_TABENT_OFF)

#endif /* _KENOS_FAT1X_H_ */


