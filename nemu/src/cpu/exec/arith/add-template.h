#include "cpu/exec/template-start.h"

#define instr add

void do_execute() {
  DATA_TYPE result = op_dest -> val + op_src -> val;
  OPERAND_W(op_dest, result);
  update_eflags_pf_zf_sf(result);
	cpu.eflags.CF = result < op_dest->val;
	cpu.eflags.OF = MSB(~(op_dest->val ^ op_src->val) & (op_dest->val ^ result));
  print_asm_template2();
  /*
  int len = (DATA_BYTE << 3) - 1, s1, s2;
  cpu.eflags.CF = (res < op_dest -> val);
  cpu.eflags.SF = res >> len;
  s1 = op_dest -> val >> len;
  s2 = op_src -> val >> len;
  cpu.eflags.OF = (s1 == s2 && s1 != cpu.eflags.SF);
  cpu.eflags.ZF = !res;
  OPERAND_W(op_dest, res);
  res ^= res >> 4;
  res ^= res >> 2;
  res ^= res >> 1;
  cpu.eflags.PF = !(res & 1);
  print_asm_template2();
  */
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif

#include "cpu/exec/template-end.h"