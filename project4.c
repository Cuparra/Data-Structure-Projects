#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OR 			||
#define AND 		&&
#define GAP			-1
#define NOTFOUND    -1
#define WRITEFILE   "result.txt"

/*	Name:  Tiago Trocoli	
	email: tiago1trocoli@gmail.com
	
	Description: The program finds the best protein sequence alignment using dynamic programming.
*/


/*
	How to use/How it works:

	1) Write a name of a file, including '.fasta', that is in the same folder as main.c.
	2) Define the penality (gap).
	3) The program will find the optimal score and all optimal alignments and write them to a file called result.txt.
*/


enum option{MATCH = -1, DELETE = 0, INSERT = 1};

/* A function that maps the protein abbreveations into a index of the weight table (see below). */
int map(char ch){

	switch(ch){
		case 'A': return 0;
		case 'R': return 1;
		case 'N': return 2;
		case 'D': return 3;
		case 'C': return 4;
		case 'Q': return 5;
		case 'E': return 6;
		case 'G': return 7;
		case 'H': return 8;
		case 'I': return 9;
		case 'L': return 10;
		case 'K': return 11;
		case 'M': return 12;
		case 'F': return 13;
		case 'P': return 14;
		case 'S': return 15;
		case 'T': return 16;
		case 'W': return 17;
		case 'Y': return 18;
		case 'V': return 19;
		default : return NOTFOUND;
	}
}

/* The BLOSUM62 score table*/
int weight[20][20] = {	{4,-1,-2,-2,0,-1,-1,0,-2,-1,-1,-1,-1,-2,-1,1,0,-3,-2,0},
						{-1,5,0,-2,-3,1,0,-2,0,-3,-2,2,-1,-3,-2,-1,-1,-3,-2,-3},
						{-2,0,6,1,-3,0,0,0,1,-3,-3,0,-2,-3,-2,1,0,-4,-2,-3},
						{-2,-2,1,6,-3,0,2,-1,-1,-3,-4,-1,-3,-3,-1,0,-1,-4,-3,-3},
						{0,-3,-3,-3,9,-3,-4,-3,-3,-1,-1,-3,-1,-2,-3,-1,-1,-2,-2,-1},
						{-1,1,0,0,-3,5,2,-2,0,-3,-2,1,0,-3,-1,0,-1,-2,-1,-2},
						{-1,0,0,2,-4,2,5,-2,0,-3,-3,1,-2,-3,-1,0,-1,-3,-2,-2},
						{0,-2,0,-1,-3,-2,-2,6,-2,-4,-4,-2,-3,-3,-2,0,-2,-2,-3,-3},
						{-2,0,1,-1,-3,0,0,-2,8,-3,-3,-1,-2,-1,-2,-1,-2,-2,2,-3},
						{-1,-3,-3,-3,-1,-3,-3,-4,-3,4,2,-3,1,0,-3,-2,-1,-3,-1,3},
						{-1,-2,-3,-4,-1,-2,-3,-4,-3,2,4,-2,2,0,-3,-2,-1,-2,-1,1},
						{-1,2,0,-1,-3,1,1,-2,-1,-3,-2,5,-1,-3,-1,0,-1,-3,-2,-2},
						{-1,-1,-2,-3,-1,0,-2,-3,-2,1,2,-1,5,0,-2,-1,-1,-1,-1,1},
						{-2,-3,-3,-3,-2,-3,-3,-3,-1,0,0,-3,0,6,-4,-2,-2,1,3,-1},
						{-1,-2,-2,-1,-3,-1,-1,-2,-2,-3,-3,-1,-2,-4,7,-1,-1,-4,-3,-2},
						{1,-1,1,0,-1,0,0,0,-1,-2,-2,0,-1,-2,-1,4,1,-3,-2,-2},
						{0,-1,0,-1,-1,-1,-1,-2,-2,-1,-1,-1,-1,-2,-1,1,5,-2,-2,0},
						{-3,-3,-4,-4,-2,-2,-3,-2,-2,-3,-2,-3,-1,1,-4,-3,-2,11,2,-3},
						{-2,-2,-2,-3,-2,-1,-2,-3,2,-1,-1,-2,-1,3,-3,-2,-2,2,7,-1},
						{0,-3,-3,-3,-1,-2,-2,-3,-3,3,1,-2,1,-1,-2,-2,0,-3,-1,4}};



/**************************************************************************************/
/* Stack data structure*/

int *stack;
int last    = -1;  /* Index to the last element of the stack.*/
int capacity = 0;  /* Maximum size of the stack.*/

void createStack(int size){

    capacity = size;
    stack    = (int*)malloc(capacity*sizeof(int));
}

int removeStack(){

    return stack[last--];
}

void insertStack(int data){

    stack[++last] = data;
}

int getAtualSize(){

    return last;
}

int StackisEmpty(){

	return (last == -1) ? 0 : 1;
}

int seeDataStack(int index){

    return stack[index];
}

/*****************************************************************************************************************/
/* Sequence alignment Data Structure */

int **M 	        = NULL;	/* Table used in dynamic program (Part 3).*/
char *s1 	        = NULL;	/* The first protein sequence.*/
char *s2 	        = NULL; /* The second protein sequence.*/
char *str1 	        = NULL;	/* Variable that stores the optimal sequences of s1.*/
char *str2	        = NULL;	/* Variable that stores the optimal sequences of s2.*/
size_t n1 	        = 0;	/* Size of s1 (without '\0').*/
size_t n2 	        = 0;	/* Size of s2 (without '\0').*/
int gap;					/* The gap penalty.*/
int optimalScore;			/* The optimal score.*/
FILE *ptr;					/* Pointer to a file to be written (result.txt).*/


/* Part 4.3: It creates the lines of mismatches with "*" and of matches with "|".*/
void writeFile(size_t size){

	int i;
	size_t n        = size - 1;

	/* Line of mismatches and of matches.*/
	char *buff1 = (char*)malloc((size+1)*sizeof(char));
	char *buff2 = (char*)malloc((size+1)*sizeof(char));

	/* For each character of the two sequences, do...*/
	for(i=0;i<n;i++){

        char v1 = str1[i];
        char v2 = str2[i];

        /* If it's a match, do...*/
        if(v1 == v2){

        	buff1[i] = ' ';
        	buff2[i] = '|';

        /* If it's not a mismatch nor match, do...*/
        }else if(v1 == '-' OR v2 == '-'){

            buff1[i] = ' ';
        	buff2[i] = ' ';

        /* If it's a mismatch, do...*/
        }else{

        	buff1[i] = '*';
        	buff2[i] = ' ';
        }
    }

    buff1[i]    = '\0';
    buff2[i]    = '\0';

    /* Write the lines and optimal sequences to result.txt.*/
    fprintf(ptr,"%s\n%s\n%s\n%s\n\n", buff1, str1, buff2, str2);

    free(buff1);
    free(buff2);
}

/* Part 4.2: It creates the optminal sequences by using the stack stored in Part 4.1*/
void printAlignment(){

	int i,j;
	int k = 0, l = 0;
	int size = getAtualSize();

	/* From the end to the initial of the stack, do...*/
	for(i=size; i>-1 ; i--){

		j 			= size - i;
		int value 	= seeDataStack(i);

		/* if the operation is match, do...*/
		if(value == MATCH){

			str1[j] = s1[k++];
			str2[j] = s2[l++];

		/* if the operation is delete, do...*/
		}else if(value == DELETE){

			str1[j] = s1[k++];
			str2[j] = '-';

		/* if the operation is insert, do...*/
		}else{

			str1[j] = '-';
			str2[j] = s2[l++];

		}
	}

	str1[size+1] = '\0';
	str2[size+1] = '\0';

	writeFile(size+2);
}

/* Part 4.1:
	This function is a modification of Algorithm Align found in the book Introduction to Computational Molecular
	Biology, Setubal and Meidanis, pag. 53.
	It starts from (n1,n2) and finds all optimal path to (0,0) walking in reverse direction of Part 3.
 	During the walk, for each step it stores the operation (delete, match, insert) on the stack.*/
void findAllPaths(int i, int j){

	/* Whenever reach (0,0), go to Part 4.2.*/
	if(i == 0 AND j == 0)
		printAlignment();

	if(i > 0 AND (M[i][j] == M[i-1][j] + gap) ){

		insertStack(DELETE);
		findAllPaths(i-1,j);
	}

	if(i > 0 AND j > 0 AND (M[i][j] == M[i-1][j-1] + weight[map(s1[i-1])][map(s2[j-1])]) ){

		insertStack(MATCH);
		findAllPaths(i-1,j-1);
	}

	if(j > 0 AND (M[i][j] == M[i][j-1] + gap) ){

		insertStack(INSERT);
		findAllPaths(i,j-1);
	}

	removeStack();
}

/* Part 4.0: Function that find all optimal alignments and write them to result.txt.*/
void printAllAlignment(){

	createStack(10*(n1+n2));

	ptr 	= fopen(WRITEFILE, "w");

	str1 = (char*)calloc(n1+n2+2, sizeof(char));
	str2 = (char*)calloc(n1+n2+2, sizeof(char));

	fprintf(ptr,"Optimal score: %d\n\n", optimalScore);

	findAllPaths(n1, n2);

	fclose(ptr);
}

/* Utility function that returns the largest number between a,b and c.*/
int max(int a, int b, int c){

	int temp;

	if(a > b)
		temp = a;
	else
		temp = b;

	return (temp > c ? temp : c);

}

/* 	Part 3: Dynamic program that returns the optmial score.
 	It's the same algorithm found in the book Introduction to Computational Molecular
	Biology, Setubal and Meidanis, pag. 52.*/
int alignment(){

	size_t i,j;

	for(i = 0;i<=n1;i++)
		M[i][0] = i*gap;

	for(j=0;j<=n2;j++)
		M[0][j] = j*gap;

	for(i=1;i<=n1;i++){

		for(j=1;j<=n2;j++){

			int k = map(s1[i-1]);
			int l = map(s2[j-1]);

			M[i][j] = max(M[i][j-1] + gap, M[i-1][j-1] + weight[k][l], M[i-1][j] + gap);

		}
	}

	return M[n1][n2];
}

/* Part 2: Allocate memory to the table that is used in dynamic program.*/
int **constructMatriz(){

	int i;
	int **Matrix;
	int size1 = n1 + 1;
	int size2 = n2 + 1;

	Matrix = (int**)malloc(size1*sizeof(int*));

	for(i=0;i<size1;i++)
		Matrix[i] = (int*)malloc(size2*sizeof(int));

	return Matrix;
}

/*****************************************************************************************************************/
/* Functions to read fasta files*/


/* Part 1.2: Function that stores the protein sequence found in .fasta file.*/
char *getSequence(FILE *pFIle){

	int ch;
	int counter 	= 0;
	char *sequence 	= (char*)malloc(80*sizeof(char));

	do{
		/* For each character read in .fasta file do...*/
		ch = fgetc(pFIle);

		/* If the sequence's size exceeds its limit, increase by 80.*/
		if((counter%80) == 0)
			sequence = (char*)realloc(sequence, (1+ counter/80)*80);

		/* Store character as long as it's not '\n'.*/
		if(ch != '\n')
			sequence[counter++] = ch;

	}while(ch != '>' AND ch != EOF);

	sequence[counter-1] = '\0';

	return sequence;
}


/* Part 1.1: read the header.*/
void readHeader(FILE *pFIle){

    while(fgetc(pFIle) != '>');
	while(fgetc(pFIle) != '\n');
}


/* Part 1.0: function that reads the .fasta file. */
void readFile(char *nameFile){

	FILE *pFIle	= fopen(nameFile, "r");

	readHeader(pFIle);
	s1  = getSequence(pFIle);
	n1  = strlen(s1);

    fseek( pFIle, -1, SEEK_CUR );

	readHeader(pFIle);
	s2	= getSequence(pFIle);
	n2  = strlen(s2);
}


int main(){

	char *nameFile 	= (char*)calloc(30, sizeof(char));

	printf("Write the file's name: ");
	scanf("%s",nameFile);

	printf("Define the gap: ");
	scanf("%d", &gap);

	readFile(nameFile);

	M = constructMatriz();

	optimalScore = alignment();

	printAllAlignment();

    return 0;
}
