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

#include "utils.h"

/* compare the first 11 bytes of buf again name.
 *
 * buf[0:10] contains the name of the file in fat12/16.
 */
int cmpfname(char *name, char *buf)
{
	int i = 0, j = 0;
	/* compare the file name */
	for (i = 0, j = 0; i < 8; i++, j++) {
		if (name[i] == '.') {
			i++;
			break;
		}
		if (name[i] != buf[j]) {
			return -1;
		}
	}
	/* j <= 8.
	 * check the rest of the name in buf.
	 */
	for (; j < 8; j++) {
		if (buf[j] != ' ') {
			return -1;
		}
	}
	/* compare the extent of file name, j == 8 */
	for (; j < 11; j++, i++) {
		if (name[i] == '\0') {
			break;
		}
		if (name[i] != buf[j]) {
			return -1;
		}
	}
	/* j <= 11.
	 * check the rest of the ext name.
	 */
	for (; j < 11; j++) {
		if (buf[j] != ' ') {
			return -1;
		}
	}

	return 0;
}

