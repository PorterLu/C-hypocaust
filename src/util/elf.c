#include "elf.h"
#include "page.h"
#include "print.h"
#include "string.h"

#define IS_ELF(hdr) \
  ((hdr)->e_ident[0] == 0x7f && (hdr)->e_ident[1] == 'E' && \
   (hdr)->e_ident[2] == 'L'  && (hdr)->e_ident[3] == 'F')

Elf64_Ehdr *elf = NULL;
char *strtab = NULL;
Elf64_Sym *symtab = NULL;
int nr_symtab_entry;

bool init_elf(const char* buf) {
  elf = (void*) buf;
  if(!IS_ELF(elf))
    panic("ELF error");
  
  printf("Guest ELF file detect\n");

  buf = (uint8_t*)elf + elf->e_shoff;
  uint64_t sh_size = elf->e_shentsize * elf->e_shnum;
  Elf64_Shdr *sh = buf;

  buf = (uint8_t*)elf + sh[elf->e_shstrndx].sh_offset;
  char *shstrtab = buf;

  for(int i = 0; i < elf->e_shnum; i++) {
    if(sh[i].sh_type == SHT_SYMTAB && strcmp(shstrtab + sh[i].sh_name, ".symtab") == 0) {
      symtab = page_alloc(PGROUNDUP(sh[i].sh_size)/PGSIZE);
      memcpy(symtab, (uint8_t*)elf + sh[i].sh_offset, sh[i].sh_size);
      nr_symtab_entry = sh[i].sh_size/sizeof(symtab[0]);
    } else if(sh[i].sh_type == SHT_STRTAB && strcmp(shstrtab + sh[i].sh_name, ".strtab") == 0) {
      strtab = page_alloc(PGROUNDUP(sh[i].sh_size)/PGSIZE);
      memcpy(strtab, (uint8_t*)elf + sh[i].sh_offset, sh[i].sh_size);
    }
  }
}

