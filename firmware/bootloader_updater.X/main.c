 /*
    AlceOSD - Bootloader self-updater
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

#include <xc.h>
#include <string.h>
#include <p33Exxxx.h>
#include "bootloader.h"


#define BOOT_FLASH_START_ADDR           (0x000800)
#define BOOT_FLASH_END_ADDR             (0x000fff)


/* board config */
#define TRIS_LED                        (TRISAbits.TRISA10)
#define LED                             (LATAbits.LATA10)

#define VERSION 0xfffffffd


typedef union {
    unsigned long l;
    unsigned int  w[2];
    unsigned char b[4];
} u32union;




int erase_page(unsigned long erase_address)
{
    u32union addr;
    addr.l = erase_address & 0xfff800;

    NVMADRU = addr.w[1];
    NVMADR = addr.w[0];
    NVMCON = 0x4003;

    __builtin_write_NVM();
    while (NVMCONbits.WR == 1);

    return NVMCONbits.WRERR;
}

int write_dword(unsigned long addr, unsigned long data0, unsigned long data1)
{
    u32union wr_addr;
    u32union wr_data0, wr_data1;

    wr_addr.l = addr;
    wr_data0.l = data0;
    wr_data1.l = data1;

    NVMCON = 0x4001;
    NVMADRU = wr_addr.w[1];
    NVMADR = wr_addr.w[0];

    TBLPAG = 0xFA;
#if 1
    __builtin_tblwtl(0, wr_data0.w[0]);
    __builtin_tblwth(1, wr_data0.w[1]);
    __builtin_tblwtl(2, wr_data1.w[0]);
    __builtin_tblwth(3, wr_data1.w[1]);
#endif
    __builtin_write_NVM();
    while(NVMCONbits.WR == 1);

    return NVMCONbits.WRERR;
}


unsigned long read_word(unsigned long addr)
{
    unsigned int tmp, r;
    unsigned long data;

    tmp = TBLPAG;
    TBLPAG = addr >> 16;
    r = __builtin_tblrdl(addr & 0xFFFF);
    data = (unsigned long) r;
    r = __builtin_tblrdh(addr & 0xFFFF) | 0xFF00;
    data |= ((unsigned long) r) << 16;
    TBLPAG = tmp;
    return data;
}


int main(void)
 {
    unsigned int i, size;
    unsigned long w0, w1;
    volatile long delay;

    TRIS_LED = 0;
    LED = 0;

    w0 = read_word(0xffe);
    if (w0 == VERSION) {
        for(;;) {
            LED =  ~LED;
            for (delay = 0; delay < 100000; delay++);
        }
    }

    erase_page(0x800);

    size = sizeof(bootloader_data) >> 1;
    i = 0;
    while (size != 0) {

        w0 = bootloader_data[i*4] | ((unsigned long)bootloader_data[i*4+1] << 16);
        size -= 2;
        if (size > 1) {
            w1 = bootloader_data[i*4+2] | ((unsigned long)bootloader_data[i*4+3] << 16);
            size -= 2;
        } else {
            w1 = 0xffffffff;
            size = 0;
        }
        write_dword(BOOT_FLASH_START_ADDR + i*4, w0, w1);
        i += 1;
    }

    write_dword(BOOT_FLASH_END_ADDR - 3, 0xffffffff, VERSION);
    LED = 1;

    for (;;);
    return 0;
}
