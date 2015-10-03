 /*
    AlceOSD - UART bootloader
    Copyright (C) 2015  Luis Alves

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FLASH_H
#define	FLASH_H

#define BOOT_FLASH_START_ADDR           (0x000800)
#define BOOT_FLASH_END_ADDR             (0x000fff)
#define DEV_CONFIG_REG_BASE_ADDRESS 	(0x0557EC)
//#define DEV_CONFIG_REG_END_ADDRESS   	(0x0557FE)
#define USER_APP_RESET_ADDRESS          (0x004000)

#define PAGE_SIZE                       (1024)
#define TOTAL_PAGES                     (170)


typedef union {
    unsigned long l;
    unsigned int  w[2];
    unsigned char b[4];
} u32union;

void init_flash(void);
int erase_page(unsigned long erase_address);
int write_dword(unsigned long addr, unsigned long data0, unsigned long data1);
int erase_addr(unsigned long addr);
void goto_usercode(void);

#endif	/* FLASH_H */

