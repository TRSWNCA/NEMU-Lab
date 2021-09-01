#include "cpu/exec/template-start.h"

#define instr ret

make_helper(concat(ret_n_, SUFFIX)) {
  cpu.eip = MEM_R(reg_l(R_ESP));// - 1;
  reg_l(R_ESP) += DATA_BYTE;
  print_asm("ret");
  return 1;
}

make_helper(concat(ret_i_, SUFFIX)) {
  uint32_t num = instr_fetch(eip + 1, 2);
  cpu.eip = MEM_R(reg_l(R_ESP));
  reg_l(R_ESP) += DATA_BYTE;
  reg_l(R_ESP) += num;
  print_asm("ret 0x%x", num);
  return 1;
}

#include "cpu/exec/template-end.h"