/*
    Name 1: Charlie Gao 
    Name 2: Tabish Chasmawala 
    UTEID 1: ccg822
    UTEID 2: tc22364 
*/

#include <stdio.h>    /* standard input/output library */
#include <stdlib.h>    /* Standard C Library */
#include <string.h>    /* String operations library */
#include <ctype.h>    /* Library for useful character operations */
#include <limits.h>    /* Library for definitions of common variable type characteristics */
#define numOpCodes 31
#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255
#define MAX_LINE_LENGTH 255

/*Array of valid opcodes*/
char* codes[numOpCodes] = { "add",
                            "and",
                            "br",
                            "brp",
                            "brz",
                            "brzp",
                            "brn",
                            "brnp",
                            "brnz",
                            "brnzp",
                            "halt",
                            "jmp",
                            "jsr",
                            "jsrr",
                            "ldb",
                            "ldw",
                            "lea",
                            "nop",
                            "not",
                            "ret",
                            "lshf",
                            "rshfl",
                            "rshfa",
                            "rti",
                            "stb",
                            "stw",
                            "trap",
                            "xor",
                            ".orig",
                            ".end",
                            ".fill"
};

typedef struct {
    int address;
    char label[MAX_LABEL_LEN + 1];  /* Question for the reader: Why do we need to add 1? */
} TableEntry;
TableEntry symbolTable[MAX_SYMBOLS];
int sym_table_size = 0; /*Current size of symbol table*/
int baseline; /*.orig baseline*/
int* cond;

enum{
    DONE, OK, EMPTY_LINE
};

/*Adds label and value to symbol table*/
void addSymbolTable(char* label, int value){
    symbolTable[sym_table_size].address=value;
    strcpy(symbolTable[sym_table_size].label,label);
    sym_table_size += 1;
}

/*Returns offset from start of program if label is found*/
int searchSymbolTable(char* label){
    int i;
    for (i = 0; i < sym_table_size; i++){
        printf("comparing intable %s to %s\n", symbolTable[i].label,label);
        if (strcmp(symbolTable[i].label, label) == 0) {return symbolTable[i].address;} /*found*/
    }
    return -1; /*not found*/
}

FILE* infile =  NULL;
FILE* outfile = NULL;

int main(int argc, char* argv[]) {
    /*some initializations*/
    cond=malloc(sizeof(int));

/* open the source file */
    infile = fopen(argv[1], "r");
    outfile = fopen(argv[2], "w");

    if (!infile) {
        printf("Error: Cannot open file %s\n", argv[1]);
        exit(4);
    }
    if (!outfile) {
        printf("Error: Cannot open file %s\n", argv[2]);
        exit(4);
    }

    char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;

    int lRet;

    /*making symbol table*/
    do{
        int temp;
        lRet = readAndParse( infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
        if( lRet != DONE && lRet != EMPTY_LINE )
        {
            
            /*Populate Symbol Table*/
            if(strcmp(lOpcode,".orig")==0){baseline=toNum(lArg1);}
            else if(strcmp(lLabel,"")!=0){
                printf("line %s\n", lLine);
                addSymbolTable(lLabel,temp+baseline);
                
            }

        }
        temp+=2;
    } while( lRet != DONE );

    /*debug symbol table*/
    int temp;
    for(temp=0;temp<sym_table_size;temp++){
        printf("%s, %.04x\n",symbolTable[temp].label,symbolTable[temp].address);
    }

    rewind(infile);

    do{
        lRet = readAndParse( infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
        char* op = lOpcode;
        if( lRet != DONE && lRet != EMPTY_LINE )
        {
            int ins,dr,sr1,sr2; /*ins(truction)*/
            /*Parse instructions*/
            /*1. Parse Opcode and create instruction*/
            /*2. For each opcode, check the formatting of arguments*/
            /*3.Throughout, append values to instruction*/
            /*4. print to file*/

            if (strcmp(op,"add")==0){
                ins=0x1000;
                dr=regParse(lArg1);
                sr1 = regParse(lArg2);
                if(regParse(lArg2)==-1){sr2 = toNum(lArg2)+0x20;}
                else{sr2 = regParse(lArg2);}
                ins=ins+(dr<<9)+(sr1<<6)+sr2;
            }         
            else if (strcmp(op,"and")==0){
                ins=0x5000;
                dr=regParse(lArg1);
                sr1 = regParse(lArg2);
                if(regParse(lArg2)==-1){sr2 = toNum(lArg2)+0x20;}
                else{sr2 = regParse(lArg2);}
                ins=ins+(dr<<9)+(sr1<<6)+sr2;
            }
            /*various branches*/
            else if(strcmp(op,"nop")==0)   {
                ins=0x0000;
                dr=searchSymbolTable(lArg1);
                if(dr==-1){printf("undefined label %s\n",dr);/*exit(1);*/}
                else{ins+=((dr-baseline)<<1);}
            }
            else if (strcmp(op,"brp")==0)  {
                ins=0x0000+(0x1<<9);
                dr=searchSymbolTable(lArg1);
                if(dr==-1){printf("undefined label %s\n",dr);/*exit(1);*/}
                else{ins+=((dr-baseline)<<1);}
            }
            else if (strcmp(op,"brz")==0)  {
                ins=0x0000+(0x2<<9);
                dr=searchSymbolTable(lArg1);
                if(dr==-1){printf("undefined label %s\n",dr);/*exit(1);*/}
                else{ins+=((dr-baseline)<<1);}
            }
            else if (strcmp(op,"brzp")==0) {
                ins=0x0000+(0x3<<9);
                dr=searchSymbolTable(lArg1);
                if(dr==-1){printf("undefined label %s\n",dr);/*exit(1);*/}
                else{ins+=((dr-baseline)<<1);}
            }
            else if (strcmp(op,"brn")==0)  {
                ins=0x0000+(0x4<<9);
                dr=searchSymbolTable(lArg1);
                if(dr==-1){printf("undefined label %s\n",dr);/*exit(1);*/}
                else{ins+=((dr-baseline)<<1);}
            }
            else if (strcmp(op,"brnp")==0) {
                ins=0x0000+(0x5<<9);
                dr=searchSymbolTable(lArg1);
                if(dr==-1){printf("undefined label %s\n",dr);/*exit(1);*/}
                else{ins+=((dr-baseline)<<1);}
            }
            else if (strcmp(op,"brnz")==0) {
                ins=0x0000+(0x6<<9);
                dr=searchSymbolTable(lArg1);
                if(dr==-1){printf("undefined label %s\n",dr);/*exit(1);*/}
                else{ins+=((dr-baseline)<<1);}
            }
            else if (strcmp(op,"br")==0)   {
                ins=0x0000+(0x7<<9);
                dr=searchSymbolTable(lArg1);
                if(dr==-1){printf("undefined label %s\n",dr);/*exit(1);*/}
                else{ins+=((dr-baseline)<<1);}
            }
            else if (strcmp(op,"brnzp")==0){
                ins=0x0000+(0x7<<9);
                dr=searchSymbolTable(lArg1);
                if(dr==-1){printf("undefined label %s\n",dr);/*exit(1);*/}
                else{ins+=((dr-baseline)<<1);}
            } 

            else if(strcmp(op,"jmp")==0)  {ins=0xC000+(regParse(lArg1)<<6);} 
            else if(strcmp(op,"ret")==0)  {ins=0xC000+(regParse(lArg1)<<6);}
            else if(strcmp(op,"jsr")==0)  {ins=0x4000+0x800+toNum(lArg1);}
            else if(strcmp(op,"jsrr")==0) {ins=0x4000+toNum(lArg1);}

            else if(strcmp(op,"ldb")==0)  {
                ins=0x2000;
                dr=regParse(lArg1);
                sr1=regParse(lArg2);
                sr2=toNum(lArg3);
                ins=ins+(dr<<9)+(sr1<<6)+sr2;
            }
            else if(strcmp(op,"ldw")==0)  {
                ins=0x6000;
                dr=regParse(lArg1);
                sr1=regParse(lArg2);
                sr2=toNum(lArg3);
                ins=ins+(dr<<9)+(sr1<<6)+sr2;
            }
            else if(strcmp(op,"lea")==0)  {
                /*search sym table, and use that addr - baseline = offset*/
                ins=0xE000;
                dr=regParse(lArg1);
                sr1=searchSymbolTable(lArg2);
                ins+=((dr<<9)+((sr1-baseline)<<1));
                /*if(sr1==-1){printf("undefined label %s\n",sr1);exit(1);}
                else{
                    ins+=((dr<<9)+(sr1<<1);
                }*/
            }
            else if(strcmp(op,"rti")==0)  {ins=0x8000;}
            else if(strcmp(op,"lshf")==0) {*cond=0; ins=0xD000;} /*left shift*/
            else if(strcmp(op,"rshfl")==0){*cond=1; ins=0xD000;} /*right logical shift*/
            else if(strcmp(op,"rshfa")==0){*cond=3; ins=0xD000;} /*right arithmetic shift*/
            else if(strcmp(op,"stb")==0)  {
                ins=0x3000;
                dr=regParse(lArg1);
                sr1=regParse(lArg2);
                sr2=toNum(lArg3);
                ins=ins+(dr<<9)+(sr1<<6)+sr2;
            }
            else if(strcmp(op,"stw")==0)  {
                ins=0x7000;
                dr=regParse(lArg1);
                sr1=regParse(lArg2);
                sr2=toNum(lArg3);
                ins=ins+(dr<<9)+(sr1<<6)+sr2;
            }
            else if(strcmp(op,"trap")==0) {ins=0xF000+toNum(lArg1);} /*various trap vects*/
            else if(strcmp(op,"halt")==0) {ins=0xF000+toNum(lArg1);} /*aka x25*/
            else if(strcmp(op,"xor")==0)  {
                ins=0x9000;
                dr=regParse(lArg1);
                sr1 = regParse(lArg2);
                if(regParse(lArg2)==-1){sr2 = toNum(lArg2)+0x20;}
                else{sr2 = regParse(lArg2);}
                ins=ins+(dr<<9)+(sr1<<6)+sr2;
            } 
            else if(strcmp(op,"not")==0)  {
                ins=0x9000;
                dr=regParse(lArg1);
                sr1 = regParse(lArg2);
                if(regParse(lArg2)==-1){sr2 = toNum(lArg2)+0x20;}
                else{sr2 = regParse(lArg2);}
                ins=ins+(dr<<9)+(sr1<<6)+sr2;
            }
            /*BELOW ARE PSEUDO-OPS*/
            else if(strcmp(op,".orig")==0) {ins=toNum(lArg1);}
            else if(strcmp(op,".fill")==0) {ins=toNum(lArg1);}
            else if(strcmp(op,".end")==0)  {;}
            else {
                printf("invalid opcode %s\n",op);
                exit(2);
            }

            fprintf( outfile, "0x%.4X\n",ins);    /* where lInstr is declared as an int */


        }
    } while( lRet != DONE );

    fclose(infile);
    fclose(outfile);
    free(cond);
}

int regParse(char* r){
    if(strcmp(r,"r0")==0){return 0x0;}
    else if(strcmp(r,"r1")==0){return 0x1;}
    else if(strcmp(r,"r2")==0){return 0x2;}
    else if(strcmp(r,"r3")==0){return 0x3;}
    else if(strcmp(r,"r4")==0){return 0x4;}
    else if(strcmp(r,"r5")==0){return 0x5;}
    else if(strcmp(r,"r6")==0){return 0x6;}
    else if(strcmp(r,"r7")==0){return 0x7;}
    else {return -1;}
}


int toNum( char * pStr ){
    char * t_ptr;
    char * orig_pStr;
    int t_length,k;
    int lNum, lNeg = 0;
    long int lNumLong;

    orig_pStr = pStr;
    if( *pStr == '#' ){                /* decimal */  
        pStr++;
        if( *pStr == '-' ){            /* dec is negative */
            lNeg = 1;
            pStr++;
        }

        t_ptr = pStr;
        t_length = strlen(t_ptr);
        for(k=0;k < t_length-1;k++){
            if (!isdigit(*t_ptr)){
                printf("Error: invalid decimal operand, %s\n",orig_pStr);
                exit(4);
            }
            t_ptr++;
        }
        lNum = atoi(pStr);
        if (lNeg){lNum = -lNum;}

        return lNum;
    }
    else if( *pStr == 'x' ){    /* hex */
        pStr++;
        if( *pStr == '-' ){            /* hex is negative */
            lNeg = 1;
            pStr++;
        }
        t_ptr = pStr;
        t_length = strlen(t_ptr);
        for(k=0;k < t_length;k++){
            if (!isxdigit(*t_ptr)){
                printf("Error: invalid hex operand, %s\n",orig_pStr);
                exit(4);
            }
            t_ptr++;
        }
        lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
        lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
        if( lNeg ){lNum = -lNum;}
            return lNum;
    }
    else{
        printf( "Error: invalid operand, %s\n", orig_pStr);
        exit(4); 
    /*This has been changed from error code 3 to error code 4, see clarification 12 */
    }
}

int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char
    ** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4){
    char * lRet, * lPtr;
    int i;
    if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
        return( DONE );
    for( i = 0; i < strlen( pLine ); i++ )
        pLine[i] = tolower( pLine[i] );

    /* convert entire line to lowercase */
    *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

    /* ignore the comments */
    lPtr = pLine;

    while( *lPtr != ';' && *lPtr != '\0' &&
        *lPtr != '\n' ) 
    lPtr++;

    *lPtr = '\0';
    if( !(lPtr = strtok( pLine, "\t\n ," ) ) ) 
        return( EMPTY_LINE );

    if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
    {
        *pLabel = lPtr;
        if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
    }

    *pOpcode = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg1 = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg2 = lPtr;
    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg3 = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg4 = lPtr;

    return( OK );
}
/* Note: MAX_LINE_LENGTH, OK, EMPTY_LINE, and DONE are defined values */


/*Returns -1 if not an opcode; 1 otherwise */
int isOpcode(char * code){
    int x;
    for(x=0;x<numOpCodes;x++){
        if(strcmp(code, codes[x])==0){return 1;} /*YES OPCODE*/
    }
    return -1; /*NOT OPCODE*/
}

/* parsing opcodes
* Returns the integer value of the opcode binary value (e.g. ADD returns 1)
* Special Cases: 
* .ORIG=16, .FILL=17, .END=18, invalid=19
* Condition codes (NZP, trap vectors) will be set separately, passed by pointer.
* Condition codes will also be repurposed for other instructions with shared opcodes.
* These are in the order presented on pg 6 of LC-3b_ISA.pdf for future reference.
*/
