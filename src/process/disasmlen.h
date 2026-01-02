#pragma once
#ifndef HACKER_DISASM_LEN_H
#define HACKER_DISASM_LEN_H
#include <stdint.h>

#ifndef UUW_DEFAULT_PARAM
# ifndef __cplusplus
#  define UUW_DEFAULT_PARAM(x)
# else
#  define UUW_DEFAULT_PARAM(x) =x
# endif
#endif

#ifndef extc
# ifndef __cplusplus
#  define extc
# else
#  define extc extern "C"
# endif
#endif

enum disasmlen_flags_t {
	ILF_NONE = 0,
	ILF_PLENMASK = 0x7,  // prefix len
	ILF_MODRM_SHIFT = 4,  // modrm len
	ILF_MODRM_MASK = 0x7 << 4,
	// 0x8,0x80 reserved.
	ILF_2BYTE = 0x100, // instruction code is 2bytes. (code only, not including reg/imm/other)
	ILF_PC_RELA = 0x200, // pc relative mod/rm.
	ILF_INVALID = 0x8000,
};
#if defined(_M_AMD64) || defined(__x86_64__)
#  define get_instruction_length get_instruction_length_amd64
#else
#  define get_instruction_length get_instruction_length_ix86
#endif

// flags: recv the ILF_xxx vals.
extc int get_instruction_length_ix86(const uint8_t* ptr, unsigned int* flags UUW_DEFAULT_PARAM(0));
extc int get_instruction_length_amd64(const uint8_t* ptr, unsigned int* flags UUW_DEFAULT_PARAM(0));
extc uint8_t* function_start(void* p);
#endif
