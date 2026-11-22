/*
** nester - NES emulator
** Copyright (C) 2000  Darren Ranalli
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful, 
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
** Library General Public License for more details.  To obtain a 
** copy of the GNU Library General Public License, write to the Free 
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
*/

/* 
** NesterDC by Ken Friece
** This routine was so different than the original, that I included 
** diff output at the bottom of the routine. The orginal is also called
** nes_rom.cpp.original
*/

#include "nester.h"
#include "types.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "nes_rom.h"
#include "nes.h"
#include "debug.h"
#ifdef __DREAMCAST__
#include <core/dc_utils.h>
#else
#include "pc_utils.h"
#endif

bool8 NES_ROM::initialize(const char *filename)
{
	unif_mapper=0;
	mapper = 0;
	crc = 0;
	fds = 0;
	image_type = invalid_image;
	trainer    = NULL;
	ROM_banks  = NULL;
	VROM_banks = NULL;
	*rom_name = '\0';
	*rom_path = '\0';
	isNSF = FALSE;
	
	int name_len = strlen(filename);
	if (stricmp(".zip", filename + (name_len - 4)) == 0)
		return load_rom_zip(filename);
	else
		return load_rom(filename);
}


NES_ROM::~NES_ROM()
{
	if (trainer)
		free (trainer);
	if (ROM_banks)
		free (ROM_banks);
	if (VROM_banks)
		free (VROM_banks);
}


uint8 NES_ROM::normalize_bank_num (uint8 bank_num)
{
	uint8 rv = 0x1;
	if (bank_num == 0)
		return 0;
	while (bank_num > rv)
		rv <<= 1;
	return rv;
}


bool8 NES_ROM::load_rom (const char *filename)
{
	FILE *fp = NULL;
	int filesize = 0;
	isNSF = FALSE;

	set_path_info (filename);

	fp = fopen (filename, "rb");
	if (!fp)
	{
		REPORT_BAD_STUFF
		goto error;
	}
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	if (fread ((void*)&header, sizeof(NES_header), 1, fp) != 1) 
	{
		REPORT_BAD_STUFF
		goto error;
	}

	/* patch for 260-in-1(#235) */
	//header.num_16k_rom_banks = (!header.dummy) ? 256 : header.dummy;

	/* set num_banks to 2^x */
	//header.num_16k_rom_banks = normalize_bank_num(header.num_16k_rom_banks);
	//header.num_8k_vrom_banks = normalize_bank_num(header.num_8k_vrom_banks);

	// NSF file? If so, special load from most recent NNNesterJ (SCHERZO)
	if(!strncmp((const char*)header.id, "NES", 3) && (header.ctrl_z == 'M'))
	{
		image_type = ines_image;
		ROM_banks = (uint8*) malloc(0x40000);

		VROM_banks = (uint8*) malloc(0);

		if (fread(ROM_banks + 0x10, filesize - sizeof(NES_header), 1, fp) != 1)
		{
			REPORT_BAD_STUFF
			goto error;
		}

		*((int*) ROM_banks) = filesize;
		ROM_banks[0x4] = header.num_16k_rom_banks;
		ROM_banks[0x5] = header.num_8k_vrom_banks;
		ROM_banks[0x6] = header.flags_1;
		ROM_banks[0x7] = header.flags_2;
		ROM_banks[0x8] = header.reserved[0];
		ROM_banks[0x9] = header.reserved[1];
		ROM_banks[0xA] = header.reserved[2];
		ROM_banks[0xB] = header.reserved[3];
		ROM_banks[0xC] = header.reserved[4];
		ROM_banks[0xD] = header.reserved[5];
		ROM_banks[0xE] = header.reserved[6];
		ROM_banks[0xF] = header.reserved[7];

		header.id[0] = 'N';
		header.id[1] = 'E';
		header.id[2] = 'S';
		header.ctrl_z = 0x1A;
		header.num_16k_rom_banks = 1;
		header.num_8k_vrom_banks = 0;
		header.flags_1 = 0xC0;
		header.flags_2 = 0x00;
		header.reserved[0] = header.reserved[1] = header.reserved[2] = header.reserved[3] = 0;
		header.reserved[4] = header.reserved[5] = header.reserved[6] = header.reserved[7] = 0;
		
		mapper = 12;
		
		// Get NSF Header so that player can know things, eh
		fseek(fp, 0, SEEK_SET);
		fread(&nsf_header, sizeof(nsf_header), 1, fp);
		
		isNSF = TRUE;
	}
	else if(!strncmp((char*)header.id, "UNIF", 4))
	{		// UNIF
		uint8* p = new uint8[filesize - 0x10];
		fread(p, 1, filesize - 0x10, fp);
	
		int32 promcn=0, cromcn=0, promtsize=0, cromtsize=0, unif_pos=0x10;
		uint8 *prommp[16], *crommp[16];
		uint32 prommsize[16], crommsize[16], promcsize[16], cromcsize[16];
		const char* mapperstr[4] = { "BMC-NovelDiamond9999999" ,"BTL-MARIO1-MALEE" , "BMC-Supervision16in1","BMC-Super24in1" };
		ROM_banks = VROM_banks = NULL;
		header.num_16k_rom_banks = header.num_8k_vrom_banks = 0;
		header.flags_1 = header.flags_2 = 0;
		unif_psize_16k = unif_csize_8k =0;
		memset(prommp, 0, sizeof(prommp));
		memset(crommp, 0, sizeof(crommp));

		image_type = unif_image;
		unif_mapper = 0xFF;
		while(unif_pos < (filesize-0x10))
		{
			uint32 ch = *((uint32*) &p[unif_pos]);
			uint32 chunksize = *((uint32*) &p[unif_pos+4]);
			if(ch == 0x5250414D)
			{						//MAPR
				if(chunksize >= 23 && !memcmp(&p[unif_pos+8], mapperstr[0], 23))
					unif_mapper = 1;
				else if(chunksize >= 16 && !memcmp(&p[unif_pos+8], mapperstr[1], 16))
					unif_mapper = 2;
				else if(chunksize >= 20 && !memcmp(&p[unif_pos+8], mapperstr[2], 20))
					unif_mapper = 3;
				else if(chunksize >= 14 && !memcmp(&p[unif_pos+8], mapperstr[3], 14))
					unif_mapper = 4;
			}
			else if(ch==0x5252494D)
			{				//MIRR
			}
			else if(ch==0x52544142)
			{				//BATR
			}
			else if((ch&0x00ffffff)==0x00475250)
			{	//PRG?
				uint32 msize, pagen;
				pagen = (ch>>24)-'0';
				if(chunksize<0x4000)
				{
					msize=0x4000;
				}
				else
				{
					msize=chunksize&0xFFFFC000;
					if(chunksize&0x3FFF)
						msize+=0x4000;
				}
				unif_psize_16k += (uint8) (msize/0x4000);
				prommp[pagen] = &p[unif_pos+8];
				promcsize[pagen] = chunksize;
				prommsize[pagen] = msize;
				promtsize += msize;
			}
			else if((ch&0x00ffffff)==0x00524843)
			{	//CHR?
				uint32 msize, pagen;
				pagen = (ch>>24)-'0';
				if(chunksize<0x2000)
				{
					msize=0x2000;
				}
				else
				{
					msize=chunksize&0xFFFFE000;
					if(chunksize&0x1FFF)
						msize+=0x2000;
				}
				unif_csize_8k += (uint8) (msize/0x2000);
				crommp[pagen] = &p[unif_pos+8];
				cromcsize[pagen] = chunksize;
				crommsize[pagen] = msize;
				cromtsize += msize;
			}
			unif_pos += (chunksize + 8);
		}
		{
			uint32 i, tsize;
			ROM_banks = (uint8*)malloc(promtsize);
			VROM_banks = (uint8*)malloc(cromtsize);

			for(tsize=0, i=0; prommp[i]&&i<16; i++)
			{
				memcpy(&ROM_banks[tsize], prommp[i], promcsize[i]);
				tsize+=prommsize[i];
			}
			for(tsize=0, i=0; crommp[i]&&i<16; i++)
			{
				memcpy(&VROM_banks[tsize], crommp[i], cromcsize[i]);
				tsize+=crommsize[i];
			}
		}
		delete[] p;
	}
	else if(!memcmp(header.id, "NES", 3) && (header.ctrl_z == 0x1A))
		image_type = load_nes_rom(fp);
	else if(!memcmp(header.id, "FDS", 3) && (header.ctrl_z == 0x1A))
		image_type = load_fds_rom(fp);
	else
	{
		REPORT_BAD_STUFF
		goto error;
	}

	fclose (fp);

	if (image_type == invalid_image)
	{
		REPORT_BAD_STUFF
		goto error;
	}

	if (!isNSF)
		correct_rom_image();

	return true;

error:
	if (fp)
		fclose(fp);
	image_type = invalid_image;

	return false;
}

bool8 NES_ROM::load_rom_zip (const char *filename)
{
	int filesize = 0;
	set_path_info (filename);

	unzFile fp;
	bool8 bOpen = false;
	fp = unzOpen(filename);
	if (!fp)
		goto error;
	else
		bOpen = true;

	char inner_name[0x100];
	get_largest_rom_name_in_zip_fp(fp, inner_name);

	unzLocateFile(fp, inner_name, 1);
	unzOpenCurrentFile(fp);
	
	unz_file_info fileInfo;
	unzGetCurrentFileInfo(fp, &fileInfo, NULL, 0, NULL, 0, NULL, 0);
	
	filesize = fileInfo.uncompressed_size;
	
	if (unzReadCurrentFile(fp, (void*) &header, sizeof(NES_header)) != sizeof(NES_header))
		goto error;

	/* patch for 260-in-1(#235) */
	//header.num_16k_rom_banks = (!header.dummy) ? 256 : header.dummy;

	/* set num_banks to 2^x */
	//header.num_16k_rom_banks = normalize_bank_num(header.num_16k_rom_banks);
	//header.num_8k_vrom_banks = normalize_bank_num(header.num_8k_vrom_banks);

	// NSF file? If so, special load from most recent NNNesterJ (SCHERZO)
	if(!strncmp((const char*)header.id, "NES", 3) && (header.ctrl_z == 'M'))
	{
		image_type = ines_image;
		ROM_banks = (uint8*) malloc(0x40000);

		VROM_banks = (uint8*) malloc(0);
		
		int32 read_count = filesize - sizeof(NES_header);
		if (unzReadCurrentFile(fp, ROM_banks + 0x10, read_count) != read_count)
		{
			REPORT_BAD_STUFF
			goto error;
		}

		*((int*) ROM_banks) = filesize;
		ROM_banks[0x4] = header.num_16k_rom_banks;
		ROM_banks[0x5] = header.num_8k_vrom_banks;
		ROM_banks[0x6] = header.flags_1;
		ROM_banks[0x7] = header.flags_2;
		ROM_banks[0x8] = header.reserved[0];
		ROM_banks[0x9] = header.reserved[1];
		ROM_banks[0xA] = header.reserved[2];
		ROM_banks[0xB] = header.reserved[3];
		ROM_banks[0xC] = header.reserved[4];
		ROM_banks[0xD] = header.reserved[5];
		ROM_banks[0xE] = header.reserved[6];
		ROM_banks[0xF] = header.reserved[7];

		header.id[0] = 'N';
		header.id[1] = 'E';
		header.id[2] = 'S';
		header.ctrl_z = 0x1A;
		header.num_16k_rom_banks = 1;
		header.num_8k_vrom_banks = 0;
		header.flags_1 = 0xC0;
		header.flags_2 = 0x00;
		header.reserved[0] = header.reserved[1] = header.reserved[2] = header.reserved[3] = 0;
		header.reserved[4] = header.reserved[5] = header.reserved[6] = header.reserved[7] = 0;

		mapper = 12;

		// Get NSF Header so that player can know things, eh
		unzCloseCurrentFile(fp);
		unzClose(fp);
		fp = unzOpen(filename);
		unzLocateFile(fp, inner_name, 1);
		unzOpenCurrentFile(fp);
		if (unzReadCurrentFile(fp, (void*) &nsf_header, sizeof(nsf_header)) != sizeof(nsf_header))
			goto error;
		//fseek(fp, 0, SEEK_SET);
		//fread(&nsf_header, sizeof(nsf_header), 1, fp);

		isNSF = TRUE;
	}
	else if(!memcmp(header.id, "NES", 3) && (header.ctrl_z == 0x1A))
		image_type = load_nes_rom_zip(fp);
	else if(!memcmp(header.id, "FDS", 3) && (header.ctrl_z == 0x1A))
		image_type = load_fds_rom_zip(fp);
	else
		goto error;

	unzCloseCurrentFile(fp);
	unzClose(fp);
	bOpen = false;

	if (image_type == invalid_image)
	{
		REPORT_BAD_STUFF
		goto error;
	}

	if (!isNSF)
		correct_rom_image ();

	return true;

error:
	if (bOpen)
	{
		unzCloseCurrentFile(fp);
		unzClose(fp);
	}
	image_type = invalid_image;

	return false;
}

NES_ROM::rom_image_type NES_ROM::load_nes_rom (FILE *fp)
{
	size_t nread;

	ROM_banks = (uint8*)malloc(header.num_16k_rom_banks * (16*1024));
	if (!ROM_banks)
		return invalid_image;

	VROM_banks = (uint8*)malloc(header.num_8k_vrom_banks * (8*1024));
	if (!VROM_banks)
		return invalid_image;

	if (has_trainer()) 
	{
		trainer = (uint8*)malloc(TRAINER_LEN);
		if(!trainer)
			return invalid_image;

		if (fread (trainer, TRAINER_LEN, 1, fp) != 1) 
			return invalid_image;
	}

	nread = 16 * 1024 * header.num_16k_rom_banks; 
	if (nread)
	{
		if (fread(ROM_banks, nread, 1, fp) != 1) 
			return invalid_image;
	}

	nread = 8 * 1024 * header.num_8k_vrom_banks;
	if (nread)
	{
		if (fread(VROM_banks, nread, 1, fp) != 1) 
			return invalid_image;
	}

	crc = calc_crc();

	return ines_image;
}

NES_ROM::rom_image_type NES_ROM::load_nes_rom_zip (unzFile fp)
{
	size_t nread;
	int32 read_count = TRAINER_LEN;

	if (header.num_16k_rom_banks == 0)
		ROM_banks = (uint8*) NULL;
	else
	{
		ROM_banks = (uint8*) malloc(header.num_16k_rom_banks * (16*1024));
		if (!ROM_banks)
			return invalid_image;
	}

	if (header.num_8k_vrom_banks == 0)
		VROM_banks = (uint8*) NULL;
	else
	{
		VROM_banks = (uint8*) malloc(header.num_8k_vrom_banks * (8*1024));
		if (!VROM_banks)
			return invalid_image;
	}

	if (has_trainer()) 
	{
		trainer = (uint8*) malloc(TRAINER_LEN);
		if(!trainer)
			return invalid_image;

		if (unzReadCurrentFile(fp, trainer, TRAINER_LEN) != TRAINER_LEN)
			return invalid_image;
	}

	read_count = nread = 16 * 1024 * header.num_16k_rom_banks; 
	if (nread)
	{
		if (unzReadCurrentFile(fp, ROM_banks, read_count) != read_count)
			return invalid_image;
	}

	read_count = nread = 8 * 1024 * header.num_8k_vrom_banks;
	if (nread)
	{
		if (unzReadCurrentFile(fp, VROM_banks, read_count) != read_count)
			return invalid_image;
	}

	crc = calc_crc();

	return ines_image;
}


NES_ROM::rom_image_type NES_ROM::load_fds_rom(FILE *fp)
{
	int i;
	uint8 disk_num = header.num_16k_rom_banks;

	header.id[0] = 'N';
	header.id[1] = 'E';
	header.id[2] = 'S';
	header.num_16k_rom_banks *= 4;
	header.num_8k_vrom_banks = 0;
	header.flags_1 = 0x40;
	header.flags_2 = 0x10;
	memset(header.reserved, 0, 8);

	ROM_banks = (uint8*)malloc(16 + 65500 * disk_num);
	if(!ROM_banks)
		return invalid_image;

	VROM_banks = (uint8*)malloc (1); /* dummy */
	if(!VROM_banks)
		return invalid_image;

	if (fread (ROM_banks + 16, 65500 * disk_num, 1, fp) != 1)
		return invalid_image;

	ROM_banks[0] = 'F';
	ROM_banks[1] = 'D';
	ROM_banks[2] = 'S';
	ROM_banks[3] = 0x1A;
	ROM_banks[4] = disk_num;

	mapper = 20;
	fds = (ROM_banks[0x1f] << 24) | (ROM_banks[0x20] << 16) |
	(ROM_banks[0x21] <<  8) | (ROM_banks[0x22] <<  0);

	for(i = 0; i < disk_num; i++)
	{
		int file_num = 0;
		uint32 pt = 16 + (65500 * i) + 0x3a;
		while(ROM_banks[pt] == 0x03)
		{
			pt += 0x0d;
			pt += ROM_banks[pt] + ROM_banks[pt + 1] * 256 + 4;
			file_num++;
		}
		ROM_banks[16 + (65500 * i) + 0x39] = file_num;
	}

	return fds_image;
}

NES_ROM::rom_image_type NES_ROM::load_fds_rom_zip (unzFile fp)
{
	int i;
	uint8 disk_num = header.num_16k_rom_banks;
	int32 read_count;

	header.id[0] = 'N';
	header.id[1] = 'E';
	header.id[2] = 'S';
	header.num_16k_rom_banks *= 4;
	header.num_8k_vrom_banks = 0;
	header.flags_1 = 0x40;
	header.flags_2 = 0x10;
	memset(header.reserved, 0, 8);

	ROM_banks = (uint8*) malloc(16 + 65500 * disk_num);
	if(!ROM_banks)
		return invalid_image;

	VROM_banks = (uint8*) malloc (1); /* dummy */
	if(!VROM_banks)
		return invalid_image;

	read_count = 65500 * disk_num;
	//if (GpFileRead(fp, ROM_banks + 16, read_count, &read_count) != SM_OK)
	if (unzReadCurrentFile(fp, ROM_banks + 16, read_count) != read_count)
		return invalid_image;

	ROM_banks[0] = 'F';
	ROM_banks[1] = 'D';
	ROM_banks[2] = 'S';
	ROM_banks[3] = 0x1A;
	ROM_banks[4] = disk_num;

	mapper = 20;
	fds = (ROM_banks[0x1f] << 24) | (ROM_banks[0x20] << 16) |
	(ROM_banks[0x21] <<  8) | (ROM_banks[0x22] <<  0);

	for(i = 0; i < disk_num; i++)
	{
		int file_num = 0;
		uint32 pt = 16 + (65500 * i) + 0x3a;
		while(ROM_banks[pt] == 0x03)
		{
			pt += 0x0d;
			pt += ROM_banks[pt] + ROM_banks[pt + 1] * 256 + 4;
			file_num++;
		}
		ROM_banks[16 + (65500 * i) + 0x39] = file_num;
	}

	return fds_image;
}

void NES_ROM::correct_rom_image ()
{
	bool8 header_valid = true;
	uint32 i;

	mapper = (header.flags_1 >> 4);
	monitor_type = monitor_ntsc;

#include "nes_rom_correct.cpp"
	
	// if there is anything in the reserved bytes,
	// don't trust the high nybble of the mapper number
	for(i = 0; i < sizeof(header.reserved); ++i) 
	{
		if(header.reserved[i] != 0x00) 
		{
			header_valid = false;
			break;
		}
	}
	
	dbgio_printf("header_valid = %d mapper = %d header.flags_2 = %X\n", header_valid, mapper, header.flags_2);
	
	if (header_valid) 
		mapper |= (header.flags_2 & 0xF0);
	else
		LOG("Invalid NES header ($8-$F)");
	
	if (crc32() == 0x95CA9EC7) // Castlevania 3
		mapper = 5;
	
	dbgio_printf("correct_rom_image mapper = %d\n", mapper);
}


uint32 NES_ROM::calc_crc ()
{
	uint32 i, j;
	uint32 c, crctable[256];
	uint32 nes_crc = 0;

	for(i = 0; i < 256; i++)
	{
		c = i;
		for (j = 0; j < 8; j++)
		{
			if (c & 1)
				c = (c >> 1) ^ 0xedb88320;
			else
				c >>= 1;
		}
		crctable[i] = c;
	}

	for(i = 0; i < header.num_16k_rom_banks; i++)
	{
		c = ~nes_crc;
		for(j = 0; j < 0x4000; j++)
			c = crctable[(c ^ ROM_banks[i*0x4000+j]) & 0xff] ^ (c >> 8);
		nes_crc = ~c;
	}

	return nes_crc;
}


const char* NES_ROM::GetRomCrc() 
{
	static char rom_crc[12];
	if (fds)
		sprintf(rom_crc, "F%x", fds);
	else
		sprintf(rom_crc, "%x", crc);
	return rom_crc;
}


void NES_ROM::set_path_info(const char* fn)
{
	int basename = strlen(fn);
	const char *p;
	char *q;
	const char *r;

	while (basename)
	{
		if((fn[basename] == '\\') || (fn[basename] == '/'))
		{
			++basename;
			break;
		}
		--basename;
	}

	p = fn + basename;
	q = rom_name;
	while (*p)
		*q++ = *p++;
	*q = '\0';

	p = fn;
	q = rom_path;
	r = fn + basename;
	while (p != r)
		*q++ = *p++;
	*q = '\0';
}


