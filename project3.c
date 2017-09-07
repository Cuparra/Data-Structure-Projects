#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#define sizeASCII   128
#define AND         &&
#define TRUE        1
#define FALSE       0
#define MAXBITS     64
#define LEFT        1
#define RIGHT       0


/*	Nome: Tiago Trocoli	
    Email: tiago1trocoli@gmail.com
    
    Description: Program to compress ASCII text files using huffman code into huff files.
    Exemple: dictionary.txt
*/


/* How the program works:

    1) Enter the file name, including .txt. (ex: dicionario.txt)
    2) Wait few seconds and program will make a compressed file of the type .huff (ex: dicionario.huff)
    3) The program also will uncompress the compressed file and make a new file ending with (copied).txt. (ex: dicionario(copied).txt)

    Note1: The program was tested in Ubuntu 16.04.
    Nate2: dicionario.txt takes up 2.7MB, when compressed takes up 1.5MB.
    Note3: The algorithm is a bit difficult to understand, especially because it deals with bits using bitwise operators...
*/

/*
    The header of the file's type .huff:
    1) It is made up of chunks of 64 bits (uint64_t)
    2) The first chunk is the number of letters of the original file.
    3) The next 128 chunks are the frequencies of each ascii character.

    Explanation:    The number of letters serves as a stopping criteria in the uncompressing stage.
                    The frequencies serves to build huffman tree.

*/


/***********************************************************************************************************************/
/*Priority queue data structure*/

struct Node{

    char data;          /* ascii character.*/
    uint64_t freq;      /* its frequency.*/
    struct Node *left;  /* left and right Node is used to build the huffman tree*/
    struct Node *right;
};

struct Tree{

    int size;               /* the tree's actual size*/
    int capacity;           /* its capacity */
    struct Node **array;    /* this array starts at index 1.*/
};

typedef struct Tree Tree;
typedef struct Node Node;

/* A utility function to know if size if one.*/
int isSizeOne(Tree *T){

    return (T->size == 1);
}

/* A utility function to know if the node is a leaf.*/
int isLeaf(Node *l){

    return ( (l->right == NULL) AND (l->left == NULL) );
}

/* It inicializes the Tree struct*/
Tree *inicializeTree(int capacity){

    Tree *T     = (Tree*)malloc(sizeof(Tree));
    T->array    = (Node**)malloc((capacity+1)*sizeof(Node*));
    T->capacity = capacity+1;
    T->size     = 0;

    return T;
}

/* A utility to swap nodes.*/
void swapNode(Node **l1, Node **l2){

    Node *temp  = *l1;
    *l1         = *l2;
    *l2         = temp;
}

/* A utility to make nodes.*/
Node *newNode(uint64_t freq, char data){

    Node *L     = (Node*)malloc(sizeof(Node));
    L->left     = NULL;
    L->right    = NULL;
    L->data     = data;
    L->freq     = freq;

    return L;
}

/* A heap function to upgrade the node.*/
void goUP(Tree *T, int i){

    int j;
    int stop    = FALSE;
    Node **arr  = T->array;

    for(j = i/2; j>=1 AND !stop ;j=j/2){

        if(arr[i]->freq < arr[j]->freq){
            swapNode(&arr[i],&arr[j]);
        }else
            stop = 1;

        i = j;
    }
}

/* A heap function to downgrade the node.*/
void goDown(Tree *T, int i){

    int j;
    int stop    = FALSE;
    int size    = T->size;
    Node **arr  = T->array;

    for(j = 2*i; j <= size AND !stop ; j = 2*j){

        if( j < size AND arr[j+1]->freq < arr[j]->freq)
            j = j + 1;

        if(arr[i]->freq > arr[j]->freq){
            swapNode(&arr[i],&arr[j]);
        }else
            stop = TRUE;

        i = j;
    }
}

/* A heap function that removes nodes. */
Node *removeNode(Tree *T){

    Node **arr = T->array;

    swapNode(&arr[1], &arr[T->size]);
    --T->size;
    goDown(T,1);

    return arr[T->size+1];
}

/* A heap function that inserts nodes. */
void insertNode(Tree *T, Node *newNode){

    int size = T->size;
    T->array[size+1] = newNode;
    ++T->size;
    goUP(T,T->size);

}

/* A heap function that buils Tree structs. */
Tree *buildTree(uint64_t *freq, int capacity){

    int i,j=1;
    Tree *T     = inicializeTree(capacity);

    for(i=1; i<=capacity; i++){

        if(freq[i] > 0)
            T->array[j++] = newNode(freq[i],i);
    }

    T->size = j-1;

    for(i = j/2; i>=1 ; --i)
        goDown(T,i);


    return T;
}

/***********************************************************************************************************************/
/* Table data structure*/

/* Table is an array that maps each ascii caracter to its huffman code.*/
struct TABLE{
    uint64_t code;  /* the huffman code of the ascii character.*/
    int size;       /* the size of its code.*/
};
typedef struct TABLE Table;

/* get the element of the array according to the ascii character (idx) */
Table *getTable(Table *table, int idx){

    if(idx >= 0 AND idx < sizeASCII)
        return &table[idx];
    else
        return NULL;
}

/* it buils the Table array by walking the huffman tree in pre-order.*/
void buildTable(Node *l, Table *table, uint64_t code,  int size){

    int idx = l->data;

    /* for each left step*/
    if(l->left != NULL){

        /* add 0 on the code.*/
        code <<= 1;
        buildTable(l->left,table, code, size+1);
        code >>= 1;
    }

    /* for each right step*/
    if(l->right != NULL){

        /* add 1 on the code.*/
        code <<= 1;
        code |= (uint64_t) 1;
        buildTable(l->right,table, code, size+1);
        code >>= 1;
    }

    /* if it is a leaf...*/
    if(isLeaf(l) == TRUE){

        /* insert the huffman code and its size in element of array.*/
        table[idx].code = code;
        table[idx].size = size;

    }

}

/* inicialize the array of tables.*/
Table *inicializeTable(Node *root){

    Table *table = (Table*)malloc(sizeASCII*sizeof(Table));
    buildTable(root,table,0,0);

    return table;
}

/***********************************************************************************************************************/
/* Huffman Tree data structure */

/* Phase 1.2 / 2.2: Build the huffman tree.*/
Tree *buildHuffmanTree(int capacity, uint64_t *freq){

    /* construct the priority queue...*/
    Tree *T = buildTree(freq, capacity);

    /* while the there're more than one element in the priority queue...*/
    while( isSizeOne(T) == FALSE ){

        /* remove its two nodes...*/
        Node *l1 = removeNode(T);
        Node *l2 = removeNode(T);

        /* create a node, in which, its frequency is the sum of the two removes nodes...*/
        Node *l3   = newNode(l1->freq + l2->freq, '*');
        l3->left   = l1; /* put the removed nodes as its children.*/
        l3->right  = l2;

        /* insert this new node in the priority queue*/
        insertNode(T,l3);
    }

    return T;
}

/* Add the first n bits of y on x, from left to right.*/
uint64_t addBits(uint64_t x, uint64_t y, int n){

    return (x << n) | y;
}

/* Copy the n fist bits of x... */
uint64_t copyBits(uint64_t x, int n, int size, int flag){

    /*from left to right...*/
    if(flag == LEFT)
        return ( (0xFFFFFFFFFFFFFFFF >> (MAXBITS - size)) & x) >> (size - n);

    /* or from right to left.*/
    return ( x << (MAXBITS - n) ) >> (MAXBITS - n);
}

/* Phase 1.4: Compress the file*/
void compress(Tree *T, FILE *fileR, FILE *fileW){

    uint64_t buff   = 0;            /* chunk of 64 bits to be store in the compressed file*/
    int buffsize    = MAXBITS;      /* variable that contains the actual size of the buff.*/

    /* Create a array that maps each ascii character to its huffman code.*/
    Table *table    = inicializeTable(T->array[1]);

    /* get the element of this map by reading one character in the file.*/
    Table *elem     = getTable(table,fgetc(fileR));
    uint64_t code   = elem->code;
    int size        = elem->size;

    do{

        /* if the actual size of the buff is greater then character's code, do...*/
        if(buffsize >= size){

            /* decrease the buff*/
            buffsize    -= size;
            /* add this code on the buff*/
            buff        = addBits(buff,code,size);

            /* read another character, and if it's not EOF do...*/
            if( ( elem = getTable(table,fgetc(fileR)) ) != NULL ){
                code    = elem->code; /* get its code*/
                size    = elem->size; /* get its code's size*/
            }

        }else{ /* otherwise do... */

            /* take the first buffsize character's code, from left to right*/
            uint64_t temp   = copyBits(code,buffsize,size, LEFT);
            /* add it on buff.*/
            buff            = addBits(buff,temp,buffsize);
            /* reduce the size of character's code.*/
            code            = copyBits(code,size - buffsize, size, RIGHT);
            size            -= buffsize;
            buffsize        = 0;

        }

        /* if buff reaches its limits (64 bits), do...*/
        if(buffsize == 0){
            fwrite(&buff,sizeof(uint64_t),1,fileW); /* write it in the compressed file.*/
            buffsize = MAXBITS;                     /* restore its size.*/
            buff = 0;
        }

    /* while character is not EOF.*/
    }while(elem != NULL);

    /* the remaining bits are zeros.*/
    if(buffsize > 0){

        buff <<= buffsize;
        fwrite(&buff,sizeof(uint64_t),1,fileW);
    }
}

/* Phase 1.3: make the header of the compressed file.*/
void makeHeader(uint64_t *freq, FILE *file){

    int i;
    uint64_t allFreq = 0;

    for(i=0;i<sizeASCII;i++)
        allFreq += freq[i];

    /* write the number of letter compressed(= sum of its frequency)*/
    fwrite(&allFreq,sizeof(uint64_t),1,file);
    /* write the array of its frequency.*/
    fwrite(freq,sizeASCII*sizeof(uint64_t),1,file);
}

/* Phase 2.1: read the header.*/
uint64_t *readHeader(FILE *file, uint64_t *allFreq){

    uint64_t *freq = (uint64_t*)calloc(sizeASCII,sizeof(uint64_t));

    fread(allFreq,sizeof(uint64_t),1,file);
    fread(freq,sizeASCII*sizeof(uint64_t),1,file);

    return freq;
}

/* Phase 1.1: Read the file and calculate the frequency of each ASCII character.*/
uint64_t *readFile(FILE *fileR){

    uint64_t *freq = (uint64_t*)calloc(sizeASCII,sizeof(uint64_t));
    int ch;

    /* Calculate the frequencies. */
    while( (ch = fgetc(fileR)) != EOF  )
            ++freq[ch];

    return freq;
}


/* Phase 1: encode the file*/
char *encode(char *nameR){

    char *nameW = (char*)malloc(30*sizeof(char));

    strncpy( nameW,nameR,strlen(nameR)-4  );
    nameW[strlen(nameR)-4] = '\0';
    strcat(nameW,".huff");
    nameW[strlen(nameR)+1] = '\0';

    FILE *fileR = fopen(nameR, "r");
    FILE *fileW = fopen(nameW, "w");

    /* Phase 1.1: Get the frequecies of all ascii characteres.*/
    printf("\nReading the %s...\n", nameR);
    uint64_t *freq = readFile(fileR);

    /* Phase 1.2: Build the huffman tree.*/
    printf("Buiding Huffman Tree.\n");
    Tree *T = buildHuffmanTree(sizeASCII,freq);

    printf("Compressing...");
    rewind(fileR);
    /* Phase 1.3: make the header*/
    makeHeader(freq,fileW);
    /* Phase 1.4: Compress the file*/
    compress(T,fileR,fileW);

    printf(" completed.\n");
    fclose(fileR);
    fclose(fileW);

    printf("The compressed file %s was created.\n", nameW);
    return nameW;
}

/* Phase 2.3: uncompress the file.*/
void uncompress(FILE *fileR, FILE *fileW, Node *root, uint64_t allFreq){

    uint64_t chunk;
    Node *p = root; /* begin in the root of huffman tree.*/

    do{

        int cicles = MAXBITS;
        fread(&chunk, sizeof(uint64_t),1,fileR);

        /* while all 64 bits of the chunk and all letters are not read, do...*/
        while( (cicles--) && (allFreq != 0) ){

            /* read one bit of the chunk.*/
            uint64_t bit = (chunk >> (MAXBITS - 1) ) & 1;
            chunk <<= 1;

            /* if it's one, go to the right of the huffman tree.*/
            if(bit == 1)
                p = p->right;
            else /* otherwise...*/
                p = p->left;

            /* if it's a leaf, write the character in the uncompressed file and etc..*/
            if(isLeaf(p) == TRUE){
                fwrite(&p->data, sizeof(char),1,fileW);
                p = root;
                --allFreq;
            }

        }

    }while(allFreq);

}

/* Phase 2: encode the file*/
void decode(char *nameFile){

    uint64_t allFreq;
    char newFile[30];

    strncpy( newFile,nameFile,strlen(nameFile)-5  );
    newFile[strlen(nameFile)-5] = '\0';
    strcat( newFile,"(copied).txt");
    newFile[strlen(nameFile)+7] = '\0';

    FILE *fileR         = fopen(nameFile, "r");
    FILE *fileW         = fopen(newFile,"w");

    /* Phase 2.1: read the header.*/
    uint64_t *freq = readHeader(fileR,&allFreq);

    printf("\n\nUncompressing the file %s...", nameFile);

    /* Phase 2.2: build the huffman tree.*/
    Tree *T = buildHuffmanTree(sizeASCII,freq);
    /* Phase 2.3: uncompress the file.*/
    uncompress(fileR,fileW,T->array[1],allFreq);

    printf("\nThe uncompressed file, %s, was created.\n", newFile);

    fclose(fileW);
    fclose(fileR);
}

/***********************************************************************************************************************/

int main(){

    char nameFile[30];

    printf("Write the name of the txt file (ex: test.txt) : ");
    scanf("%s",nameFile);

    char *compressedFile = encode(nameFile);

    decode(compressedFile);

    return 0;
}
