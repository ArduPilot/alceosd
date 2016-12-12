/*
    AlceOSD - Graphical OSD
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

#include "alce-osd.h"


/* flash operation functions */
static void erase_page0(u32union addr)
{
    NVMADRU = addr.w[1];
    NVMADR = addr.w[0];
    NVMCON = 0x4003;

    __builtin_write_NVM();
    while (NVMCONbits.WR == 1) {}
}

int erase_page(unsigned long erase_address)
{
    u32union addr;
    addr.l = erase_address & 0xfff800;

    erase_page0(addr);
    if (hw_rev >= 0x03) {
        addr.l += 0x400;
        erase_page0(addr);
    }
    
    return NVMCONbits.WRERR;
}

static int write_dword(unsigned long addr, unsigned long data0, unsigned long data1)
{
    u32union wr_addr;
    u32union wr_data0, wr_data1;
    unsigned int tmp;

    wr_addr.l = addr;
    wr_data0.l = data0;
    wr_data1.l = data1;


    NVMCON = 0x4001;
    NVMADRU = wr_addr.w[1];
    NVMADR = wr_addr.w[0];

    tmp = TBLPAG;

    TBLPAG = 0xFA;
    __builtin_tblwtl(0, wr_data0.w[0]);
    __builtin_tblwth(1, wr_data0.w[1]);
    __builtin_tblwtl(2, wr_data1.w[0]);
    __builtin_tblwth(3, wr_data1.w[1]);

    //INTCON2bits.GIE = 0;
    __builtin_write_NVM();
    while(NVMCONbits.WR == 1);

    TBLPAG = tmp;

    return NVMCONbits.WRERR;
}

void inline write_word(unsigned long addr, unsigned long data)
{
    if ((addr % 4) == 0)
        write_dword(addr, data, 0xffffff);
    else
        write_dword(addr, 0xffffff, data);
}

void read_flash(unsigned long addr, unsigned int size, unsigned char *buf)
{
    unsigned int tmp;
    unsigned int data;
    unsigned int i;
    unsigned int instr = size / 3;
    unsigned int left = size % 3;

    tmp = TBLPAG;
    TBLPAG = addr >> 16;
    for (i = 0; i < instr; i++, addr += 2) {
        data = __builtin_tblrdl(addr & 0xFFFF);
        *buf++ = data & 0xFF;
        *buf++ = data >> 8;
        data = __builtin_tblrdh(addr & 0xFFFF);
        //printf("read1.2 %x %x\n", (unsigned int) data >> 16, (unsigned int) data);
        *buf++ = data & 0xFF;
    }
    if (left >= 1) {
        data = __builtin_tblrdl(addr & 0xFFFF);
        *buf++ = data & 0xFF;
        if(left >= 2)
            *buf = data >> 8;
    }
    TBLPAG = tmp;
}
