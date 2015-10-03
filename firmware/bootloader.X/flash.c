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

#include "bootloader.h"

static unsigned long erased_page[TOTAL_PAGES];

void init_flash(void)
{
    unsigned int i;
    for (i = 0; i < TOTAL_PAGES; i++)
        erased_page[i] = 0;
}

int erase_page(unsigned long erase_address)
{
    u32union addr;
    addr.l = erase_address & 0xfff800;

    NVMADRU = addr.w[1];
    NVMADR = addr.w[0];
    NVMCON = 0x4003;

    INTCON2bits.GIE = 0;
    __builtin_write_NVM();
    while (NVMCONbits.WR == 1) {}
    INTCON2bits.GIE = 1;

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
    __builtin_tblwtl(0, wr_data0.w[0]);
    __builtin_tblwth(1, wr_data0.w[1]);
    __builtin_tblwtl(2, wr_data1.w[0]);
    __builtin_tblwth(3, wr_data1.w[1]);

    INTCON2bits.GIE = 0;
    __builtin_write_NVM();
    while(NVMCONbits.WR == 1);
    INTCON2bits.GIE = 1;

    return NVMCONbits.WRERR;
}

int erase_addr(unsigned long addr)
{
    unsigned long page = (addr >> 11);
    int ret = 0;

    if (erased_page[page])
        return ret;

    put_char('e');
    ret = erase_page(addr);
    if (ret)
        return ret;
    erased_page[page] = 1;
    if (page == 0)
        ret = write_dword(0, 0x040800, 0x000000);
    return ret;
}

void goto_usercode(void)
{
    void (*fptr)(void);
    LED = 1;
    fptr = (void (*)(void)) USER_APP_RESET_ADDRESS;
    fptr();
}
