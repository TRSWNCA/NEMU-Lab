#include "cpu/exec/template-start.h"

#define instr add

void do_execute() {
  DATA_TYPE res = op_dest -> val + op_src -> val;
  OPERAND_W(op_dest, res);
  update_eflags_pf_zf_sf(res);
  int len = (DATA_BYTE << 3) - 1, s1, s2;
  cpu.eflags.CF = (res < op_dest -> val);
  s1 = op_dest -> val >> len;
  s2 = op_src -> val >> len;
  cpu.eflags.OF = (s1 == s2 && s1 != cpu.eflags.SF);
  print_asm_template2();
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"