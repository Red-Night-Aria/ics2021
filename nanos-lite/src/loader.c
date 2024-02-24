#include <proc.h>
#include <elf.h>
#include "fs.h"

#undef __LP64__

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr hdr;
  // Log("Try to open: %s", filename);
  int proc_fd = fs_open(filename, 0, 0);
  // Log("fd: %d", proc_fd);
  
  fs_read(proc_fd, &hdr, sizeof(Elf_Ehdr));
  
  assert(strncmp((char*)hdr.e_ident, ELFMAG, SELFMAG) == 0);
  #ifndef EXPECT_ELF_TYPE
    #error EXPECT_ELF_TYPE has not be defined for current ISA
  #endif
  assert(hdr.e_machine == EXPECT_ELF_TYPE);
  for (int i = 0; i < hdr.e_phnum; ++i) {
    size_t ph_offset = hdr.e_phoff + i * hdr.e_phentsize; 
    Elf_Phdr phdr;
    fs_lseek(proc_fd, ph_offset, SEEK_SET);
    fs_read(proc_fd, &phdr, sizeof(Elf_Phdr));
    // ramdisk_read(&phdr, ph_offset, sizeof(Elf_Phdr));
    if (phdr.p_type != PT_LOAD) {
      continue;
    }
    // ramdisk_read((void*)phdr.p_vaddr, phdr.p_offset, phdr.p_filesz);
    fs_lseek(proc_fd, phdr.p_offset, SEEK_SET);
    fs_read(proc_fd, (void*)phdr.p_vaddr, phdr.p_filesz);
    memset((void*)(phdr.p_vaddr + phdr.p_filesz), 0, phdr.p_memsz - phdr.p_filesz);
  }
  return hdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  assert(entry != 0);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

