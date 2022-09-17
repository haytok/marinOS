#include "elf.h"
#include "lib.h"
#include "defines.h"

struct elf_header {
	// 2 byte
	struct {
		unsigned char magic[4];
		unsigned char class;
		unsigned char format;
		unsigned char version;
		// こっからはあってもなくてもいい領域なはず
		unsigned char abi;
		unsigned char abi_version;
		unsigned char reserve[7];
	} id;
	short type;
	short arch;
	long version;
	long entry_point;
	long program_header_offset; // セグメントの情報を取得するのに必要
	long section_header_offset;
	long flags;
	short header_size;
	short program_header_size; // セグメントの情報を取得するのに必要
	short program_header_num; // セグメントの情報を取得するのに必要
	short section_header_size;
	short section_header_num;
	short section_header_index;
};

struct elf_program_header {
	long type;
	long offset;
	long virtual_addr;
	long physical_addr;
	long file_size;
	long memory_size;
	long flags;
	long align;
};

int elf_check(struct elf_header *header)
{
	if (memcmp(header->id.magic,
		   "\x7f"
		   "ELF",
		   4)) {
		return -1;
	}

	if (header->id.class != 1)
		return -1;
	// ビッグエンディアンかをチェック
	if (header->id.format != 2)
		return -1;
	if (header->id.version != 1)
		return -1;
	if (header->type != 2)
		return -1;
	if (header->version != 1)
		return -1;

	// Hitachi H8/300 or H8/300H
	if ((header->arch != 46) && (header->arch != 47))
		return -1;

	return 0;
}

static int elf_load_program(struct elf_header *header)
{
	// long にするとエラーになった ... :(
	int i;
	struct elf_program_header *phdr;

	for (i = 0; i < header->program_header_num; i++) {
		// header までのアドレスと header からのアドレスを考慮しないといけない。
		// header は struct elf_header 型のポインタなので、一旦 1 byte 用のプリミティブな型に変換する必要がある。
		phdr = (struct elf_program_header
				*)((char *)header +
				   header->program_header_offset +
				   header->program_header_size * i);

		if (phdr->type != 1) {
			continue;
		}

		putxval(phdr->offset, 6);
		puts("  ");
		putxval(phdr->virtual_addr, 8);
		puts("  ");
		putxval(phdr->physical_addr, 8);
		puts("  ");
		putxval(phdr->file_size, 5);
		puts("  ");
		putxval(phdr->memory_size, 5);
		puts("  ");
		putxval(phdr->flags, 2);
		puts("  ");
		putxval(phdr->align, 2);
		puts("\n");
	}

	return 0;
}

int elf_load(char *buf)
{
	struct elf_header *header = (struct elf_header *)buf;

	if (elf_check(header) < 0) {
		return -1;
	}

	if (elf_load_program(header) < 0) {
		return -1;
	}

	return 0;
}
