/*
 * Firmware Image Builder
 * Copyright 2008-2015 Marvell Technology Group Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include "elf.h"
#include "wm_utils.h"
#include <firmware_structure.h>

#if defined(AXF_BUILT_BY_IAR)
	static const char *build_string = "AXF_BUILT_BY_IAR";
#elif defined(AXF_BUILT_BY_ARM_GCC)
	static const char *build_string = "AXF_BUILT_BY_ARM_GCC";
#endif /* AXF_BUILT_BY_IAR */

/*
 * Exits with error messages
 */
static void die_usage(const char *argv0)
{
	printf("build_string: %s\n", build_string);
	fprintf(stderr, "Usage: %s <input_file.axf> <output_file.bin>\n",
								argv0);
	exit(1);
}

static void die_perror(const char *msg)
{
	perror(msg);
	exit(1);
}

static void die(const char *msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

/*
 * Endian Conversions
 */
static uint16_t le16_to_h(uint16_t *x)
{
	uint8_t *b = (uint8_t *)x;
	return b[0] | (b[1] << 8);
}

static uint16_t be16_to_h(uint16_t *x)
{
	uint8_t *b = (uint8_t *)x;
	return (b[0] << 8) | b[1];
}

static uint32_t le32_to_h(uint32_t *x)
{
	uint8_t *b = (uint8_t *)x;
	return b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24);
}

static uint32_t be32_to_h(uint32_t *x)
{
	uint8_t *b = (uint8_t *)x;
	return (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
}

static void h_to_le32(uint32_t v, uint32_t *x)
{
	uint8_t *b = (uint8_t *)x;
	b[0] = v;
	b[1] = (v >> 8);
	b[2] = (v >> 16);
	b[3] = (v >> 24);
}

static void h_to_be32(uint32_t v, uint32_t *x)
{
	uint8_t *b = (uint8_t *)x;
	b[3] = v;
	b[2] = (v >> 8);
	b[1] = (v >> 16);
	b[0] = (v >> 24);
}

/* convert ELF header field to host endianness */
#define get_u16(endian, x) \
	((endian) == ELFDATA2LSB ? le16_to_h(&(x)) : be16_to_h(&(x)))
#define get_u32(endian, x) \
	((endian) == ELFDATA2LSB ? le32_to_h(&(x)) : be32_to_h(&(x)))

/* convert back to target endianness */
#define put_u32(endian, v, x) \
	do { \
		if ((endian) == ELFDATA2LSB) \
			h_to_le32(v, &(x)); \
		else \
			h_to_be32(v, &(x)); \
	} while (0)

#define MAGIC_STR    "MRVL"

static void write_img_hdr(uint8_t endian, uint16_t elf_seg_cnt,
				uint32_t entry, FILE *out)
{
	struct img_hdr ihdr;
	memcpy(&ihdr.magic_str, MAGIC_STR, 4);
	put_u32(endian, FW_MAGIC_SIG, *(uint32_t *)&ihdr.magic_sig);
	put_u32(endian, time(NULL), *(uint32_t *)&ihdr.time);
	put_u32(endian, elf_seg_cnt, ihdr.seg_cnt);
	put_u32(endian, entry, ihdr.entry);
	if (fwrite(&ihdr, sizeof(struct img_hdr), 1, out) != 1)
		die_perror("unable to write output file");
}

static void update_header(uint8_t endian, struct seg_hdr *cur_sh,
						struct seg_hdr *sh)
{
	static int idx;

	put_u32(endian, FW_BLK_LOADABLE_SEGMENT, sh[idx].type);
	put_u32(endian, cur_sh->offset, sh[idx].offset);
	put_u32(endian, cur_sh->len, sh[idx].len);
	put_u32(endian, cur_sh->laddr, sh[idx].laddr);
	put_u32(endian, cur_sh->crc, sh[idx].crc);
	idx++;
}

static void write_data(uint8_t endian, uint16_t elf_seg_cnt,
			Elf32_Phdr *elf_sh, struct seg_hdr *sh,
			FILE *in, FILE *out)
{
	int i, pad;
	struct seg_hdr *cur_sh;

	cur_sh = malloc(sizeof(struct seg_hdr));
	if (!cur_sh)
		die("out of memory");

#if defined(AXF_BUILT_BY_IAR)
	/*
	Grouping all the sections whoose load address is
	marked as flash memory (address: '0x1f******') are
	being placed at start of binary file.
	*/
	for (i = 0; i < elf_seg_cnt; i++) {
		uint8_t *data;
		uint32_t seg_offs = get_u32(endian, elf_sh[i].p_offset);

		cur_sh->laddr = get_u32(endian, elf_sh[i].p_vaddr);
		cur_sh->len = get_u32(endian, elf_sh[i].p_filesz);
		cur_sh->type = get_u32(endian, elf_sh[i].p_type);
		cur_sh->offset = ftell(out);

		/* keep only loadable segments */
		if (cur_sh->type != PT_LOAD || cur_sh->len == 0)
			continue;

		/*
		Skip the sections whose load address is not in
		flash memory.
		*/
		if (!(cur_sh->laddr & 0x1f000000))
			continue;

		if (fseek(in, seg_offs, SEEK_SET) != 0)
			die_perror("cannot seek to ELF segment");

		/* pad and round segment size up to next 32-bit boundary */
		pad = ((cur_sh->len + 3) & ~3) - cur_sh->len;

		data = malloc(cur_sh->len + pad);
		if (!data)
			die("out of memory");
		if (fread(data, cur_sh->len, 1, in) != 1)
			die_perror("cannot read ELF segment");

		memset(data + cur_sh->len, 0xff, pad);
		cur_sh->len += pad;

		if (fwrite(data, cur_sh->len, 1, out) != 1)
			die_perror("unable to write output file");

		cur_sh->crc = soft_crc32(data, cur_sh->len, 0);
		free(data);
		update_header(endian, cur_sh, sh);
	}

	for (i = 0; i < elf_seg_cnt; i++) {
		uint8_t *data;
		uint32_t seg_offs = get_u32(endian, elf_sh[i].p_offset);

		cur_sh->laddr = get_u32(endian, elf_sh[i].p_vaddr);
		cur_sh->len = get_u32(endian, elf_sh[i].p_filesz);
		cur_sh->type = get_u32(endian, elf_sh[i].p_type);
		cur_sh->offset = ftell(out);

		/* keep only loadable segments */
		if (cur_sh->type != PT_LOAD || cur_sh->len == 0)
			continue;

		/*
		Skip the sections whose load address is in flash memory,
		as they are already handled in the above loop.
		*/
		if (cur_sh->laddr & 0x1f000000)
			continue;

		if (fseek(in, seg_offs, SEEK_SET) != 0)
			die_perror("cannot seek to ELF segment");

		/* pad and round segment size up to next 32-bit boundary */
		pad = ((cur_sh->len + 3) & ~3) - cur_sh->len;

		data = malloc(cur_sh->len + pad);
		if (!data)
			die("out of memory");
		if (fread(data, cur_sh->len, 1, in) != 1)
			die_perror("cannot read ELF segment");

		memset(data + cur_sh->len, 0xff, pad);
		cur_sh->len += pad;

		if (fwrite(data, cur_sh->len, 1, out) != 1)
			die_perror("unable to write output file");

		cur_sh->crc = soft_crc32(data, cur_sh->len, 0);
		free(data);
		update_header(endian, cur_sh, sh);
	}
#elif defined(AXF_BUILT_BY_ARM_GCC)
	for (i = 0; i < elf_seg_cnt; i++) {
		uint8_t *data;
		uint32_t seg_offs = get_u32(endian, elf_sh[i].p_offset);

		cur_sh->laddr = get_u32(endian, elf_sh[i].p_vaddr);
		cur_sh->len = get_u32(endian, elf_sh[i].p_filesz);
		cur_sh->type = get_u32(endian, elf_sh[i].p_type);
		cur_sh->offset = ftell(out);

		/* keep only loadable segments */
		if (cur_sh->type != PT_LOAD || cur_sh->len == 0)
			continue;

		if (fseek(in, seg_offs, SEEK_SET) != 0)
			die_perror("cannot seek to ELF segment");

		/* pad and round segment size up to next 32-bit boundary */
		pad = ((cur_sh->len + 3) & ~3) - cur_sh->len;

		data = malloc(cur_sh->len + pad);
		if (!data)
			die("out of memory");
		if (fread(data, cur_sh->len, 1, in) != 1)
			die_perror("cannot read ELF segment");

		memset(data + cur_sh->len, 0xff, pad);
		cur_sh->len += pad;

		if (fwrite(data, cur_sh->len, 1, out) != 1)
			die_perror("unable to write output file");

		cur_sh->crc = soft_crc32(data, cur_sh->len, 0);
		free(data);
		update_header(endian, cur_sh, sh);
	}
#else
	#error "You have passed incorrect option"
#endif /* AXF_BUILT_BY_IAR */
	free(cur_sh);
}

static void write_seg_hdr(uint8_t endian, uint16_t seg_cnt, struct seg_hdr *sh,
								FILE *out)
{
	if (fwrite(sh, sizeof(struct seg_hdr) * seg_cnt, 1, out) != 1)
		die_perror("unable to write output file");
}

static void elf_to_firmware_image(FILE *in, FILE *out)
{
	Elf32_Ehdr *elf_hdr;
	Elf32_Phdr *elf_sh;
	struct seg_hdr *sh;
	uint16_t elf_seg_cnt;
	uint16_t seg_cnt = 0;
	uint8_t endian;
	int i;

	elf_hdr = malloc(sizeof(Elf32_Ehdr));
	if (!elf_hdr)
		die("out of memory");

	if (fread(elf_hdr, sizeof(Elf32_Ehdr), 1, in) != 1)
		die_perror("cannot read ELF file header");

	if (strncmp((char *)elf_hdr->e_ident, ELFMAG, SELFMAG) != 0)
		die("invalid ELF file: bad magic number");
	if (elf_hdr->e_ident[EI_CLASS] != ELFCLASS32)
		die("invalid ELF file: only 32bits files are supported");
	endian = elf_hdr->e_ident[EI_DATA];
	if (endian != ELFDATA2LSB && endian != ELFDATA2MSB)
		die("invalid ELF file: unknown endianess setting");

	elf_seg_cnt = get_u16(endian, elf_hdr->e_phnum);
	if (elf_seg_cnt == 0)
		die("invalid ELF file: no program headers");
	if (fseek(in, get_u32(endian, elf_hdr->e_phoff), SEEK_SET) != 0)
		die_perror("cannot seek to ELF program header table");
	elf_sh = malloc(elf_seg_cnt * sizeof(Elf32_Phdr));
	if (!elf_sh)
		die("out of memory");
	if (fread(elf_sh, elf_seg_cnt * sizeof(Elf32_Phdr), 1, in) != 1)
		die_perror("cannot read ELF segment headers");

	/* Count the loadable segments */
	for (i = 0; i < elf_seg_cnt; i++)
		if (!(get_u32(endian, elf_sh[i].p_type) != PT_LOAD ||
				get_u32(endian, elf_sh[i].p_filesz) == 0))
			seg_cnt++;
	if (seg_cnt == 0)
		die("invalid ELF file: no loadable segments");
	/* Write image header */
	write_img_hdr(endian, seg_cnt, elf_hdr->e_entry, out);

	if (seg_cnt > SEG_CNT) {
		fprintf(stderr, "segments %d exceed maximum limit of %d",
							seg_cnt, SEG_CNT);
		exit(1);
	}

	sh = malloc(SEG_CNT * sizeof(struct seg_hdr));
	if (!sh)
		die("out of memory");
	memset(sh, 0xff, SEG_CNT * sizeof(struct seg_hdr));

	/* Write data */
	if (fseek(out, sizeof(struct img_hdr) +
		(SEG_CNT * sizeof(struct seg_hdr)), SEEK_SET) != 0)
		die_perror("cannot seek in output file");
	write_data(endian, elf_seg_cnt, elf_sh, sh, in, out);

	/* Write segment headers */
	if (fseek(out, sizeof(struct img_hdr), SEEK_SET) != 0)
		die_perror("cannot seek in output file");
	write_seg_hdr(endian, SEG_CNT, sh, out);

	free(elf_hdr);
	free(elf_sh);
	free(sh);
}

int main(int argc, char *argv[])
{
	FILE *in, *out;

	if (argc != 3)
		die_usage(argv[0]);

	in = fopen(argv[1], "rb");
	if (!in)
		die_perror(argv[1]);
	out = fopen(argv[2], "wb");
	if (!out)
		die_perror(argv[2]);

	elf_to_firmware_image(in, out);

	fclose(in);
	if (fclose(out) != 0)
		die_perror(argv[3]);
	return 0;
}
