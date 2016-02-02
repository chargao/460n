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

typedef struct{
    int loc; /*address*/
    char* label;
}SymbolEntry;

#define MAX_LINE_LENGTH 255
#define MAX_SYMBOL_COUNT 255

char* sym_table_labels[500]; //Names of labels
int sym_table_values[500]; //Value is offset from PC
int sym_table_size = 0; //Current size of symbol table

int pc_offset = 0;

enum{
    DONE, OK, EMPTY_LINE
};

//Adds label and value to symbol table
void addSymbolTable(char* label, int value)
{
    sym_table_labels[sym_table_size] = label;
    sym_table_values[sym_table_size] = value;

    sym_table_size += 1;
}

//Returns offset from start of program if label is found
int searchSymbolTable(char* label)
{
    for (int i = 0; i < sym_table_size; i++)
    {
    if (strcmp(sym_table_labels[i], label) == 0) //Label has been found
        return sym_table_values[i];
    }
    return -1;
}

FILE* infile =  NULL;
FILE* outfile = NULL;

int main(int argc, char* argv[]) {

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

    /* 
    *
    *
    *Do stuff with files 
    *
    *
    */

    char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;

    int lRet;

    FILE * lInfile;

    lInfile = fopen( "data.in", "r" );    /* open the input file */

    do{
        lRet = readAndParse( lInfile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
        if( lRet != DONE && lRet != EMPTY_LINE )
        {
    /*...*/
        }
    } while( lRet != DONE );


    fclose(infile);
    fclose(outfile);
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
    for(k=0;k < t_length;k++){
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

/*Array of valid opcodes*/
#define numOpCodes 31
char* codes[numOpCodes] = {"ADD","AND","BR","BRN","BRZ","BRP","BRNZ","BRNP","BRZP","BRNZP","HALT","JMP","JSR","JSRR","LDB","LDW","LEA","NOP","NOT","RET","LSHF","RSHFL","RSHFA","RTI","STB","STW","TRAP","XOR",".ORIG",".FILL",".END"};

/*Returns 0 if not an opcode; 1 otherwise */
int isOpcode(char * code){
    int x;
    for(x=0;x<numCodes;x++){
        if(strcmp(code, codes[x])==0){return 1;} /*YES OPCODE*/
    }
    return -1; /*NOT OPCODE*/
}

/* parsing opcodes
* returns the integer value of the opcode binary value (e.g. ADD returns 1)
* Special Cases: 
* .FILL=16, .END=17,invalid=18
* NZP will be set separately, passed by pointer
* 
*/
int parseOpcodes(char* op, int* nzp){
    *nzp=0;

    if     (strcmp(op,"ADD")==0)  {return 1;}
    else if(strcmp(op,"AND")==0)  {return 5;}
    else if(strcmp(op,"NOP")==0)  {*nzp=0;return 0;}
    else if(strcmp(op,"BRP")==0)  {*nzp=1;return 0;}
    else if(strcmp(op,"BRZ")==0)  {*nzp=2;return 0;}
    else if(strcmp(op,"BRZP")==0) {*nzp=3;return 0;}
    else if(strcmp(op,"BRN")==0)  {*nzp=4;return 0;}
    else if(strcmp(op,"BRNP")==0) {*nzp=5;return 0;}
    else if(strcmp(op,"BRNZ")==0) {*nzp=6;return 0;}
    else if(strcmp(op,"BR")==0)   {*nzp=7;return 0;}
    else if(strcmp(op,"BRNZP")==0){*nzp=7;return 0;}
    /*else if(strcmp(op,"HALT")==0) {return 15;}*/
    /*etc, currently working*/
}
