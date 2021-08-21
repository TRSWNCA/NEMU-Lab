#include "prefix/prefix.h"

#include "data-mov/mov.h"
#include "data-mov/xchg.h"

#include "arith/dec.h"
#include "arith/inc.h"
#include "arith/neg.h"
#include "arith/imul.h"
#include "arith/mul.h"
#include "arith/idiv.h"
#include "arith/div.h"

#include "logic/and.h"
#include "logic/or.h"
#include "logic/not.h"
#include "logic/xor.h"
#include "logic/sar.h"
#include "logic/shl.h"
#include "logic/shr.h"
#include "logic/shrd.h"

#include "string/rep.h"

#include "misc/misc.h"

#include "special/special.h"

/*---------------------------------------*/
#include "control/jmp.h"
#include "data-mov/leave.h"
#include "data-mov/movext.h"
#include "logic/setcc.h"
#include "arith/sub.h"
#include "arith/add.h"
#include "arith/adc.h"
#include "arith/sbb.h"
#include "string/lods.h"
#include "string/scas.h"
#include "string/stos.h"
#include "string/movs.h"
