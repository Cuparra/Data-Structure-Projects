#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>


#define UPPER_LETER(l)  (int) l < 91
#define OCCUPIED            1
#define EMPTY               0
#define MOD                 %

/*	Nome: Tiago Trocoli
    Email: tiago1trocoli@gmail.com
*/


/* In my computer, this program takes +/- 27s to find the solution.*/

/**********************************************************************************************************************************************/

/* There are no words with more than 24 + '\0' letters.*/
typedef struct{
    char string[25];
}Word;

Word *buffer = NULL;
int size = 1;

/* Read the file and copy the words into buffer vector.*/
void readFile(char *fileName){

    FILE* fp;

    fp = fopen(fileName, "r");

    buffer = (Word*)malloc(size*sizeof(Word));

    while( fscanf(fp,"%s",buffer[size-1].string) != EOF ){

        buffer = (Word*)realloc(buffer,(size+1)*sizeof(Word));
        ++size;
    }

    --size;
    fclose(fp);
}

/**********************************************************************************************************************************************/
/*
    In other to check if two words are anagrams we use this theorem below:
    Fundamental Theorem of Arithmetic (FTA): "every integer greater than 1 either is prime itself or is the product of prime numbers and
    that this product is unique, up to the order of the factors." (Wikipedia)
*/

/*  To use this fact, fist we create a list that maps each letter into a prime number.
    So, a/A -> 2, b/B -> 3, ..., z/Z -> 101.*/
int prime[26] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101};


/*  Second, we create a function that receives a word as an input and outputs a number which is the multiplication of prime numbers corresponding to
    each of input's letter. Example:

    map(Ana) = 2 * 43 * 2 = 172
    map(naa) = 43 * 2 * 2 = 172

    By the FTA, we conclude that for each input the output is unique and X and Y are anagrams if and only if map(X) = map(Y).*/

uint64_t map(char *word){

    int i;
    uint64_t uniqueNum = 1;

    for(i=0; word[i] != '\0';i++){

        char c = word[i];

        if ( UPPER_LETER(c) )
            uniqueNum *= (prime[c - 65]);
        else
            uniqueNum *= (prime[c - 97]);
    }

    return uniqueNum;
}

/**********************************************************************************************************************************************/
/*Hash Table functions and data structure*/

/*Every element of the Hash Table is a Slot and it stores the set of anagrams.*/
typedef struct{
    uint64_t uniqueNum;     /* map(word) = uniqueNum.*/
    int setOfAnagrams[20]; /* It stores the positions of words corresponding to the vector buffer (not the words!).*/
    int sizeSet;            /* The number of anagrams in this slot (max = 20).*/
    int isEmpty;
}Slot;

typedef struct{
    int size;
    Slot *slot;
}Hash;

Hash *H = NULL;
int greaterSet = 0;         /* The size of the greatest set of anagrams.*/

/* As you may note, my program consumes quite a few memory. For 235886 words in the dictionary.txt, my hash table uses +/- 39 megabytes. */
/* because for the sake of efficiency, I prefer to allocate setOfAnagrams statically instead of dynamically as the hash is populated.*/

void createHash(int size){

    H        = (Hash*)malloc(sizeof(Hash));
    H->size  = (int) 1.7*size + 1;
    H->slot  = (Slot*)calloc(H->size, sizeof(Slot));

}


/* Insert each word of the buffer vector in the Hash Table... */
void insertHash(char *str, int position){

    uint64_t uniqueNum          = map(str);
    Slot *slot                  = H->slot;
    int h                       = 0;
    int stop                    = 0;
    int i                       = 0;

    uint32_t key;

    while(!stop){

        key = (uniqueNum + i + i*i) MOD H->size;    /* hash function. */

        /* If this slot is empty...*/
        if(slot[key].isEmpty == EMPTY){

            slot[key].setOfAnagrams[slot->sizeSet]  = position;  /* store the position of this word corresponding to buffer vector.*/
            slot[key].uniqueNum                     = uniqueNum; /* store its unique number.*/
            slot[key].isEmpty                       = OCCUPIED;
            stop                                    = 1;
            ++slot[key].sizeSet;                                 /* increase the size of the set.*/

        /* If this slot is not empty but this word belongs to the slot's set of anagrams...*/
        }else if(slot[key].uniqueNum == uniqueNum){

            slot[key].setOfAnagrams[slot[key].sizeSet] = position;
            stop                                       = 1;
            ++slot[key].sizeSet;

        }

        /* Otherwise, find another slot.*/
        ++h;
        ++i;
    }

    /* Update size of the greatest set of anagrams.*/
    if(greaterSet < slot[key].sizeSet)
        greaterSet = slot[key].sizeSet;

}

/* Populate the Hash Table with words.*/
void populateHash(){

    int i;

    for(i=0;i<size;i++)
        insertHash(buffer[i].string, i);
}

/* Print the greatest set of anagram.*/
void printGreatestSet(){

    int i,j;
    int cont    = 1;
    Slot *slot  = H->slot;

    for(i=0;i<H->size;i++){

        if(slot[i].sizeSet == greaterSet){

            printf("\n\n%d - Set of anagrams :\n", cont++);

            for(j=0;j<greaterSet;j++){

                int position = slot[i].setOfAnagrams[j];
                printf("%d: %s\n", j+1, buffer[position].string);
            }
        }
    }

}

char nameFile[30];

int main(){

    printf("Write the name of the file: ");

    scanf("%s",nameFile);

    readFile(nameFile);

    createHash(size);

    populateHash();

    printGreatestSet();

    return 0;
}
