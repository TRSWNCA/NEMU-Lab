#include "cpu/exec/template-start.h"

#define instr test

static void do_execute() {
  cpu.eflags.CF = 0;
  cpu.eflags.OF = 0;
  DATA_TYPE result = op_dest -> val & op_src -> val;
  int len = (DATA_BYTE << 3) - 1;
  cpu.eflags.SF = result >> len;
  cpu.eflags.ZF = !result;
  int i, flag = 1;
  for (i = 0; i < 8; i++) {
    if (result & (1 << i)) flag = flag ^ 1;
  }
  cpu.eflags.PF = flag;
  print_asm_template1();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
#include "cpu/exec/template-end.h"