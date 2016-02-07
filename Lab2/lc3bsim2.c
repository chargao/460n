/*
    REFER TO THE SUBMISSION INSTRUCTION FOR DETAILS

    Name 1: Charlie Gao 
    Name 2: Chase Riggins
    UTEID 1: ccg822
    UTEID 2: cmr2842
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
*/

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

  int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
  int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {                                                    
  printf("----------------LC-3b ISIM Help-----------------------\n");
  printf("go               -  run program to completion         \n");
  printf("run n            -  execute program for n instructions\n");
  printf("mdump low high   -  dump memory from low to high      \n");
  printf("rdump            -  dump the register & bus values    \n");
  printf("?                -  display this help menu            \n");
  printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  process_instruction();
  CURRENT_LATCHES = NEXT_LATCHES;
  INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
  int i;

  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating for %d cycles...\n\n", num_cycles);
  for (i = 0; i < num_cycles; i++) {
    if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
    }
    cycle();
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating...\n\n");
  while (CURRENT_LATCHES.PC != 0x0000)
    cycle();
  RUN_BIT = FALSE;
  printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
  int address; /* this is a byte address */

  printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
  int k; 

  printf("\nCurrent register/bus values :\n");
  printf("-------------------------------------\n");
  printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
  printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  printf("Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
  fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  fprintf(dumpsim_file, "Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
  char buffer[20];
  int start, stop, cycles;

  printf("LC-3b-SIM> ");

  scanf("%s", buffer);
  printf("\n");

  switch(buffer[0]) {
  case 'G':
  case 'g':
    go();
    break;

  case 'M':
  case 'm':
    scanf("%i %i", &start, &stop);
    mdump(dumpsim_file, start, stop);
    break;

  case '?':
    help();
    break;
  case 'Q':
  case 'q':
    printf("Bye.\n");
    exit(0);

  case 'R':
  case 'r':
    if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
    else {
	    scanf("%d", &cycles);
	    run(cycles);
    }
    break;

  default:
    printf("Invalid Command\n");
    break;
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
  int i;

  for (i=0; i < WORDS_IN_MEM; i++) {
    MEMORY[i][0] = 0;
    MEMORY[i][1] = 0;
  }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {                   
  FILE * prog;
  int ii, word, program_base;

  /* Open program file. */
  prog = fopen(program_filename, "r");
  if (prog == NULL) {
    printf("Error: Can't open program file %s\n", program_filename);
    exit(-1);
  }

  /* Read in the program. */
  if (fscanf(prog, "%x\n", &word) != EOF)
    program_base = word >> 1;
  else {
    printf("Error: Program file is empty\n");
    exit(-1);
  }

  ii = 0;
  while (fscanf(prog, "%x\n", &word) != EOF) {
    /* Make sure it fits. */
    if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
             program_filename, ii);
	    exit(-1);
    }

    /* Write the word to memory array. */
    MEMORY[program_base + ii][0] = word & 0x00FF;
    MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
    ii++;
  }

  if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

  printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */ 
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) { 
  int i;

  init_memory();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(program_filename);
    while(*program_filename++ != '\0');
  }
  CURRENT_LATCHES.Z = 1;  
  NEXT_LATCHES = CURRENT_LATCHES;
    
  RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
  FILE * dumpsim_file;

  /* Error Checking */
  if (argc < 2) {
    printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
           argv[0]);
    exit(1);
  }

  printf("LC-3b Simulator\n\n");

  initialize(argv[1], argc - 1);

  if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
    printf("Error: Can't open dumpsim file\n");
    exit(-1);
  }

  while (1)
    get_command(dumpsim_file);
    
}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/


int dr, sr1, sr2; /*if an instruction uses an immediate value, reuse sr2 as immediate*/
void setcc(void);
void process_instruction() {
  int currentPc = CURRENT_LATCHES.PC/2;
  int instruction = ((MEMORY[currentPc][1] << 8) + MEMORY[currentPc][0]);

  int loc, data; /*used for any memory-related instructions*/
  int sign, shftnum; /*used specifically for RSHFA*/
  int n, z, p; /* local n z p bits */
  int offset9; /* PCoffset9 */
  int offset11;
  int offset6;
  int memLoc;

  switch (instruction & 0xF000) { /*decode opcode*/
    case 0x1000: /*ADD*/
      if ((instruction & 0x0020) == 0) { /*two sr*/
        dr = (instruction & 0x0E00); /*instruction[11:9]*/
        dr = dr >> 9;
        sr1 = (instruction & 0x01C0); /*instruction[8:6]*/
        sr1 = sr1 >> 6;
        sr2 = (instruction & 0x0007); /*instruction[2:0] | no shift necessary */
        /*execute*/
        NEXT_LATCHES.REGS[dr] = CURRENT_LATCHES.REGS[sr1] + CURRENT_LATCHES.REGS[sr2];
        setcc();
      }
      else { /*one sr*/
        dr = (instruction & 0x0E00); /*instruction[11:9]*/
        dr = dr >> 9;
        sr1 = (instruction & 0x01C0); /*instruction[8:6]*/
        sr1 = sr1 >> 6;
        sr2 = (instruction & 0x001F); /* imm5[4:0] */
        /*execute*/
        NEXT_LATCHES.REGS[dr] = CURRENT_LATCHES.REGS[sr1] + sr2; /* sr2 = imm5 */
        setcc();                                                                                                          /* DONT FORGET ABOUT PC */
      }
          break;
    
    case 0x5000: /* AND */
      if ((instruction & 0x0020) == 0) {  /* two sr */
        dr = (instruction & 0x0E00); /*instruction[11:9]*/
        dr = dr >> 9;
        sr1 = (instruction & 0x01C0); /*instruction[8:6]*/
        sr1 = sr1 >> 9;
        sr2 = (instruction & 0x0007); /*instruction[2:0] | no shift necessary */
        /* execute */
        NEXT_LATCHES.REGS[dr] = CURRENT_LATCHES.REGS[sr1] & CURRENT_LATCHES.REGS[sr2];
        setcc();
      }
      else { /* one sr1 */
        dr = (instruction & 0x0E00); /*instruction[11:9]*/
        dr = dr >> 9;
        sr1 = (instruction & 0x01C0); /*instruction[8:6]*/
        sr1 = sr1 >> 9;
        sr2 = instruction & 0x001F; /* imm5[4:0] */
        /*execute*/
        NEXT_LATCHES.REGS[dr] = CURRENT_LATCHES.REGS[sr1] & sr2; /* sr2 is actually imm5 */
        setcc();
      }
          break;
    
    case 0x0000: /* BRANCH */
        n = instruction & 0x0800;
          z = instruction & 0x0400;
          p = instruction & 0x0200;
          /*BUG IF BRANCH IS FIRST INSTRUCTION */


          if ((n && CURRENT_LATCHES.N) || (z && CURRENT_LATCHES.Z) || (p && CURRENT_LATCHES.P)) {
            offset9 = instruction & 0x01FF;
            if (instruction & 0x0100) {     /* PCoffset9 is negative, must sign extend */
              offset9 = offset9 | 0xFFFFFE00;
              offset9 = offset9 << 1; /* since neg, 0x0200 must == 1 */
              NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2 + offset9;
            }
            else { /* PCoffset9 is positive */
              offset9 << 1;
              NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2 + offset9;
            }
          }
          break;
    
    case 0x4000: /* JSR and JSRR */
      NEXT_LATCHES.REGS[7] = CURRENT_LATCHES.PC + 2;
      if(instruction & 0x0800) { /*JSR case */
        if(instruction & 0x0400) {/* negative PCoffset11 */
          offset11 = instruction & 0x7FF;
          offset11 = offset11 | 0xFFFFF000;
          offset11 = offset11 << 1;
          NEXT_LATCHES.PC =CURRENT_LATCHES.PC + 2 + offset11;
        }
        else { /* positive PCoffset */
          offset11 = offset11 << 1;
          NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2 + offset11;
        }

      }
      else { /* JSRR case */
        sr1 = instruction & 0x01C0; /* BaseR bits [8:6] */
        sr1 = sr1 >> 6;
        NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[sr1];

      }
          break;
    
    case 0x2000: /* LDB */
      if(instruction & 0x0020) { /* base offset negative */
        sr1 = instruction & 0x01C0;
        sr1 >> 6;
        offset6 = instruction & 0x003F;
        offset6 = offset6 | 0xFFFFFFC0;
        memLoc = offset6 + CURRENT_LATCHES.REGS[sr1];
        dr = 0x0E00;
        dr = dr >> 9;
        memLoc = memLoc / 2;
        NEXT_LATCHES.REGS[dr] = MEMORY[memLoc][0]; /* come back and double check this */
        setcc();
      }
      else { /* base offset positve */
        sr1 = instruction & 0x01C0; /* BaseR */
        sr1 >> 6;
        offset6 = offset6 & 0x003F; /*boffset6*/
        memLoc = CURRENT_LATCHES.REGS[sr1] + offset6;
        dr = instruction & 0x0E00;
        dr = dr >> 9;
        memLoc = memLoc /2;
        NEXT_LATCHES.REGS[dr] = MEMORY[memLoc][0]; /* come back and double check this */
        /* need to account for sign extension */
        setcc();
      }
          break;
    
    case 0x6000: /* LDW */
      if(instruction & 0x0020) { /* base offset negative */
        sr1 = instruction & 0x01C0;
        sr1 >> 6;
        offset6 = instruction & 0x003F;
        offset6 = offset6 | 0xFFFFFFC0; /*negative sign extend*/
        offset6 << 1;
        memLoc = offset6 + CURRENT_LATCHES.REGS[sr1];
        dr = 0x0E00;
        dr = dr >> 9;
        memLoc = memLoc/2;
        NEXT_LATCHES.REGS[dr] = MEMORY[memLoc][0]; /* come back and double check this */
        setcc();
      }
      else { /* base offset positve */
        sr1 = instruction & 0x01C0; /* BaseR */
        sr1 >> 6;
        offset6 = offset6 & 0x003F; /*boffset6*/
        offset6 << 1;
        memLoc = CURRENT_LATCHES.REGS[sr1] + offset6;
        dr = instruction & 0x0E00;
        dr = dr >> 9;
        memLoc = memLoc/2;
        NEXT_LATCHES.REGS[dr] = MEMORY[memLoc][0]; /* come back and double check this */
        /* need to account for sign extension */
        setcc();
      }
      break;
    
    case 0xE000:
      dr = instruction & 0x0E00;
      dr >> 9;
      offset9 = instruction & 0x01FF;
      if (instruction & 0x0100) {     /* PCoffset9 is negative, must sign extend */
        offset9 = offset9 | 0xFFFFFE00;
        offset9 = offset9 << 1; /* since neg, 0x0200 must == 1 */
        NEXT_LATCHES.REGS[dr] = CURRENT_LATCHES.PC + 2 + offset9;
      }
      else { /* PCoffset9 is positive */
        offset9 << 1;
        NEXT_LATCHES.REGS[dr] = CURRENT_LATCHES.PC + 2 + offset9;
      }
      break;
    
    case 0xC000: /*JMP and RET*/
      sr1 = (instruction & 0x01C0) >> 6; /*instruction[8:6]*/
      sr1/=2; /*note memory is word addressable*/
      /*execute*/
          NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[sr1];
      break;
    
    case 0xD000: /*LSHF, RSHFL, RSHFA*/
      dr = (instruction & 0x0E00) >> 9; /*instruction[11:9]*/
      sr1 = (instruction & 0x01C0) >> 6; /*instruction[8:6]*/
      sr2 = (instruction & 0x000F);    /*instruction[3:0], repurposed as amt(4)*/
      /*finish decode & execute*/
      switch (instruction & 0x0030) {
        case 0x0000: /*LSHF*/
          NEXT_LATCHES.REGS[dr] = Low16bits(CURRENT_LATCHES.REGS[sr1] << sr2);
          break;
        case 0x0010: /*RSHFL*/
          NEXT_LATCHES.REGS[dr] = Low16bits(CURRENT_LATCHES.REGS[sr1] >> sr2); /*this may or may not work as expected*/
          break;
        case 0x0030: /*RSHFA*/
          sign = (CURRENT_LATCHES.REGS[sr1] & 0x8000);
          for (shftnum = 0; shftnum < sr2; shftnum++) {
            NEXT_LATCHES.REGS[dr] = Low16bits(CURRENT_LATCHES.REGS[sr1] >> 1);
            NEXT_LATCHES.REGS[dr] &= sign;
          }
          break;
      }
      setcc();
      break;

    case 0x3000: /*STB*/
      sr1 = (instruction & 0x0E00) >> 9; /*instruction[11:9]*/ /*note this is SR*/
      dr = (instruction & 0x01C0) >> 6; /*instruction[8:6]*/  /*note this is baseR*/
      sr2 = (instruction & 0x003F);    /*instruction[5:0]*/  /*note this is offset(6)*/
      if(instruction & 0x0020) { sr2 = sr2 | 0xFFFFFFC0; }/* base offset negative,  negative sign extend*/

      /*execute*/
      data = (CURRENT_LATCHES.REGS[sr1] & 0x00FF);
      loc = (CURRENT_LATCHES.REGS[dr] + sr2)/2; /*sExt(offset) + BaseR*/ /*note memory is word addressable*/
      MEMORY[loc][0] = data & 0xFF; /*lower byte*/
      break;

    case 0x7000: /*STw*/ /*We are making the assumption that the offset is properly aligned*/
      sr1 = (instruction & 0x0E00) >> 9; /*instruction[11:9]*/ /*note this is SR*/
      dr = (instruction & 0x01C0) >> 6;  /*instruction[8:6]*/  /*note this is baseR*/
      sr2 = (instruction & 0x003F);      /*instruction[5:0]*/  /*note this is offset(6)*/
      if(instruction & 0x0020) { sr2 = sr2 | 0xFFFFFFC0; }/* base offset negative,  negative sign extend*/
      /*execute*/
      data = Low16bits(CURRENT_LATCHES.REGS[sr1]);
      loc = (CURRENT_LATCHES.REGS[dr] + sr2)/2; /*sExt(offset) + BaseR*/ /*note memory is word addressable*/
      MEMORY[loc][0] = data & 0x00FF; /*lower byte*/
      MEMORY[loc][1] = (data & 0xFF00) >> 8; /*upper byte*/
      break;

    case 0xF000: /*TRAP*/
      sr2 = (instruction & 0x00FF); /*note this is the trap vector*/
      /*execute*/
          NEXT_LATCHES.REGS[7] = CURRENT_LATCHES.PC;
      loc = Low16bits(sr2) << 1;
          NEXT_LATCHES.PC = (MEMORY[loc][1] << 8) + MEMORY[loc][0];
      break;

    case 0x9000: /*XOR*/
      if ((instruction & 0x0020) == 0) { /*two SR*/
        dr = (instruction & 0x0E00) >> 9; /*instruction[11:9]*/
        sr1 = (instruction & 0x01C0) >> 6; /*instruction[8:6]*/
        sr2 = (instruction & 0x0007);    /*instruction[2:0]*/
        /*execute*/
        NEXT_LATCHES.REGS[dr] = Low16bits(CURRENT_LATCHES.REGS[sr1] ^ CURRENT_LATCHES.REGS[sr2]);
        setcc();
      }
      else { /*one SR*/
        dr = (instruction & 0x0E00) >> 9; /*instruction[11:9]*/
        sr1 = (instruction & 0x01C0) >> 6; /*instruction[8:6]*/
        sr2 = (instruction & 0x001F);    /*imm5*/
        /*execute*/
        NEXT_LATCHES.REGS[dr] = Low16bits(CURRENT_LATCHES.REGS[sr1] ^ sr2);
        setcc();
      }
    break;

    default: /*cannot decode opcode, shold throw invalid opcode*/
      /*we are making the assumption that all opcodes are valid*/
      break;
  }
  NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
}

void setcc(){
  if(CURRENT_LATCHES.REGS[dr] > 0){CURRENT_LATCHES.N = 1;} /*setcc*/
  else if(CURRENT_LATCHES.REGS[dr]==0){CURRENT_LATCHES.Z = 1;}
  else{CURRENT_LATCHES.P = 1;}
}






/*
    case 0x9000: NOT
      dr  = (instruction & 0x0E00)>>9; instruction[11:9]
      sr1 = (instruction & 0x01C0)>>6; instruction[8:6]
      execute
      CURRENT_LATCHES.REGS[dr] = ~(CURRENT_LATCHES.REGS[sr1]);
      setcc();
      break;
*/