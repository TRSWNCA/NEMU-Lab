#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args) {
  cpu_exec(args != NULL ? atoi(args) : 1);
  return 0;
}

static int cmd_x(char *args) {
  char *arg = strtok(args, " ");
  if (arg == NULL) return puts("Wrong args!!"), 1;
  int n = atoi(arg);
  arg = strtok(NULL, " "); // second arg
  if (arg == NULL) return puts("Wrong args!!"), 1;
  bool success = false; int i;
  swaddr_t startadd = expr(arg, &success);
  if (!success) return puts("Error expr!"), 1;
  for (i = 0; i < n; ++ i) {
    uint32_t ls = swaddr_read(startadd + i * 4, 4);
    if (i % 4 == 0) printf("0x%08x: ", startadd + i * 4);
    printf("0x%08x%c", ls, " \n"[i % 4 == 3]);
  }
  putchar(10);
  return 0;
}

static int cmd_info(char *args) {
  if (args == NULL) return puts("Wrong args!!"), 1;
  if (args[0] == 'r') {
    // printf("CPU_EIP: %d\n", cpu.eip);
    int i;
    for (i = 0; i < 8; ++ i) {
      //printf("%s: %08x\t%s: %04x\t%s: %02x\n", regsl[i], reg_l(i), regsw[i], reg_w(i), regsb[i], reg_b(i));
      printf("%s\t0x%08x\t%d\n", regsl[i], reg_l(i), reg_l(i));
    }
  } else if (args[0] == 'w') {
    info_wp();
  } else {
    return puts("Wrong args!!!"), 1;
  }
  return 0;
}

static int cmd_w(char *args) {
  if (args == NULL) return puts("Wrong args!!"), 1;
  WP * f; bool suc;
  f = new_wp();
  f -> val = expr(args, &suc);
  if (!suc) {
    return puts("Wrong args!!"), 1;
  }
  strcpy(f -> expr, args);
  printf("Watchpoint created: Watchpoint %d: %s = 0x%x\n", f -> NO, f -> expr, f -> val);
  return 0;
}

static int cmd_d(char * args) {
  if (args == NULL) return puts("Wrong args!!"), 1;
  delete_wp(atoi(args));
  return 0;
}

static int cmd_p(char * args) {
  if (args == NULL) return puts("Wrong args!!"), 1;
  bool success = false;
  uint32_t ans = expr(args, &success);
  if (!success) return puts("Error expr!"), 1;
  printf("%s = %d\n", args, ans);
  return 0;
}

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Steps through N instructions and then pauses, When N is not given, the default is 1.", cmd_si },
  { "x", "Read from the address", cmd_x },
  { "info", "Print all registers", cmd_info },
  { "w", "Add watch point", cmd_w },
  { "d", "Delete watch point", cmd_d },
  { "p", "Evaluate an expression on the current thread", cmd_p }
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if(arg == NULL) {
    /* no argument given */
    for(i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for(i = 0; i < NR_CMD; i ++) {
      if(strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop() {
  while(1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if(cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if(args >= str_end) {
      args = NULL;
    }

#ifdef HAS_DEVICE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for(i = 0; i < NR_CMD; i ++) {
      if(strcmp(cmd, cmd_table[i].name) == 0) {
        if(cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
