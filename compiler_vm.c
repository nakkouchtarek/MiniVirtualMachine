#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// VIRTUAL MACHINE

#define NUM_REGS 3

unsigned regs[ NUM_REGS ];

/*
Opearations:
0 : hlt
1 : load
2 : add
3 : sub
4 : mul
5 : div
*/

/* program counter */
unsigned int PC = 0;

/* instruction fields */
int IR  = 0;
int SCL = 0;
int R0  = 0;
int R1  = 0;
int R2  = 0;

/* decode a word */
void decode( int instr )
{
	/*           NUM
	OP   REG   REG  REG
	0001 0000 0110 0100 Operation AND
	1111 0000 0000 0000 Mask 0xF000
	0001 0000 0000 0000 Result bits
	// Left shift by 12
	0000 0000 0000 0001 = 1 ( Operation is Load )
	Repeat for other registers
	*/

	IR  = (instr & 0xF000) >> 12;
  	R0  = (instr & 0x0F00) >>  8;
  	R1  = (instr & 0x00F0) >>  4;
  	R2  = (instr & 0x000F);
  	SCL = (instr & 0x00FF);
}

/* display all registers */
void showRegs()
{
  	printf( " R0   R1   R2  \n" );

  	for(int i=0; i<NUM_REGS; i++ )
    	printf( "%04d ", regs[ i ] );

  	printf( "\n" );
}

void showReg(int i)
{
   	printf( "R%d : %04d\n", i, regs[ i ] );
}

/* evaluate the last decoded instruction */
void eval()
{
  	switch( IR )
  	{
    	/* load */
    	case 1:
      	regs[ R0 ] = SCL;
      	break;

    	/* add */
    	case 2:
      	regs[ R0 ] = regs[ R1 ] + regs[ R2 ];
      	break;

      	/* sub */
    	case 3:
      	regs[ R0 ] = regs[ R1 ] - regs[ R2 ];
      	break;

      	/* mul */
    	case 4:
      	regs[ R0 ] = regs[ R1 ] * regs[ R2 ];
      	break;

      	/* div */
    	case 5:
      	regs[ R0 ] = (int)(regs[ R1 ] / regs[ R2 ]);
      	break;

      	case 6:
      	showReg(R0);
      	break;

   		/* halt */
    	case 0:
      	exit(0);
      	break;
  	}
}

// COMPILER

/*

0 : hlt
1 : load
2 : add
3 : sub
4 : mul
5 : div

r0 : 0
r1 : 1
r2 : 2

IF LOAD :
0x0F00 => REG
0x00FF => SCL
ELSE IF HLT:
0x0000
ELSE:
0x0F00 => REG
0x00F0 => REG
0x000F => REG

*/

void compile_line(char* buf)
{
	int i = 0;
    char *p = strtok (buf, " ");
    char *array[4];
    int res = 0;

    while (p != NULL)
    {
        array[i++] = p;
        p = strtok (NULL, " ");
    }

    if(!strcmp(array[0],"load"))
    {
    	res = res ^ 0x1000 ^ ((int)(array[1][1]-48)<<8) ^ atoi(array[2]);
    	decode(res);
		eval();
    }
    else if(!strcmp(array[0],"hlt"))
    {
    	decode(0);
		eval();
    }
    else if(!strcmp(array[0],"show"))
    {
    	res = res ^ 0x6000 ^ ((int)(array[1][1]-48)<<8);
    	decode(res);
		eval();
    }
    else
    {
    	if(!strcmp(array[0],"add"))
    	{
    		res = res ^ 0x2000;
    	}
    	else if(!strcmp(array[0],"sub"))
    	{
    		res = res ^ 0x3000;
    	}
    	else if(!strcmp(array[0],"mul"))
    	{
    		res = res ^ 0x4000;
    	}
    	else if(!strcmp(array[0],"div"))
    	{
    		res = res ^ 0x5000;
    	}
		else
		{
			printf("Invalid operation.\n");
			exit(0);
		}

		res = res ^ ((int)(array[1][1]-48)<<8) ^ ((int)(array[2][1]-48)<<4) ^ ((int)(array[3][1]-48));

		decode(res);
		eval();
    }
}

void handleFile(char* fileName)
{
	FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(fileName, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        compile_line(line);
    }

    fclose(fp);

    if (line)
    	free(line);
}

int main(void)
{
    handleFile("file.tsm");
    return 0;
}