#include <stdio.h>
#include <stdlib.h>

#define INFINITE 100000000
#define TRUE    1
#define FALSE   0
#define EMPTY   -1
#define AND     &&

/* Nome:  Tiago Trocoli  
   Email: tiago1trocoli@gmail.com
   
   Description: Dynamic program to find all all shortest path beetwen to cells of a given matrix.
*/


/*It was assumed that Thomas only walks left, right, down and up.*/

/* Maze data structure*/

int s1, s2; /* Coordinates of the exit. */
int t1, t2; /* Coordinates of Thomas. */
int n, m;    /* Number of rows (n) and columns (m). */

/* The maze data structure is a vector of Vertex, in which each of its grid corresponds to a Vertex.*/
typedef struct{
    int dist;    /* The shortest distance from the exit.*/
    int adj[4];  /* The set of adjancent grids, maximum 4 (left,right,up,down).*/
    int prev[4]; /* The set of predecessors. It is used to find the shortest paths.*/
}Vertex;

Vertex *V;

/*An one-to-one function that maps the coordinates of the grid to its location on the vector Vertex.*/
/*F(row,column) = m*row + column, where m is the number of columns.*/
int function(int x,int y){

   return  m*x + y;
}

/* getX(F(row,column)) = row */
int getX(int value){

    return (int) value / m;
}

/* getX(F(row,column)) = column */
int getY(int value){

    return value % m;
}

void initializeVertex(){

    int i,j;
    V = (Vertex*)malloc(m*n*sizeof(Vertex));

    for(i = 0; i < n*m; i++){

        V[i].dist  = INFINITE;
        for(j=0; j < 4 ;j++){

            V[i].prev[j]    = EMPTY;
            V[i].adj[j]     = EMPTY;
        }
    }
}

/*Put (x2,y2) in the set of adjacent vertex of (x1,y1)*/
void addAdj(int x1, int y1, int x2, int y2){

    int i = -1;

    int v1 = function(x1,y1);
    int v2 = function(x2,y2);

    while(V[v1].adj[++i] != EMPTY);
    V[v1].adj[i] = v2;
}

/*Put v1 in the set of predecessors of v2*/
void addPredecessor(int v1, int v2){

    int i = -1;

    while(V[v1].prev[++i] != EMPTY);
    V[v1].prev[i] = v2;
}

int verifyBoundary(int x, int y){

    return  (x > -1 AND x < n) AND (y > -1 AND y < m);

}

/**************************************************************************************/
/* Queue data structure*/

int *queue;
int top         = -1;
int front       = 0;
int queueSize   = 0;

void createQueue(int size){

    queue = (int*)malloc(size*sizeof(int));
}

void enqueue(int data){

    queue[++top] = data;
    ++queueSize;
}

int dequeue(){

    --queueSize;
    return queue[front++];
}

int isQueueEmpty(){

    return queueSize == 0;
}

/**************************************************************************************/
/* Stack data structure*/

int *stack;
int last    = -1;  /*Index to the last element of the stack.*/
int maxSize = 0;

void createStack(int size){

    maxSize = size;
    stack   = (int*)malloc(size*sizeof(int));
}

int removeStack(){

    return stack[last--];
}

int insertStack(int data){

    stack[++last] = data;
}

int getAtualSize(){

    return last;
}

int seeDataStack(int index){

    return stack[index];
}

/**************************************************************************************/
/*Breadth First Search*/

/*I use BFS to find all shortest distance from the exit to all grid in order to backtrack the paths in the right order.*/
/*Require all vertex's distance = INFINITY.*/

void modifiedBFS(){

    int i;
    int Exit = function(s1,s2);

    V[Exit].dist = 0;
    enqueue(Exit);

    while(!isQueueEmpty()){

        int v1   = dequeue();

        /*For each edge v1 - v2*/
        for( i = 0 ; (V[v1].adj[i] != EMPTY) ; i++) {

            int v2 = V[v1].adj[i];

            /*If v2.dist = INFINITY, it means v2 ins't discovered yet.*/
            if(V[v1].dist + 1 < V[v2].dist){

                enqueue(v2);
                V[v2].dist = V[v1].dist + 1;
                addPredecessor(v2,v1);

            /*If v2 was already discovered.*/
            }else if(V[v1].dist + 1 == V[v2].dist)
                addPredecessor(v2,v1);
        }
    }

}


/**************************************************************************************/

int pathExist = FALSE;

void printPath(){

    int i = -1;

    while(++i <= getAtualSize()){

        int v = seeDataStack(i);
        printf("(%d,%d) ", getX(v), getY(v));
    }

    printf("\n");
    pathExist = TRUE;
}

/*Recursive function that uses the set of predecessors to walk on the path.*/
int printAllPaths(int v){

    int i;

    insertStack(v);

    for(i=0; (V[v].prev[i] != EMPTY) ; i++)
        printAllPaths(V[v].prev[i]);

    /*Whenever a function reach its destination, it prints a path.*/
    if(s1 == getX(v) AND s2 == getY(v))
        printPath();

    removeStack();
}

/**************************************************************************************/

void readFile(char *fileName){

    FILE* fp;
    int i, j;
    int maze[100][100];

    fp = fopen(fileName, "r");

    /* Read the coordinates of Thomas. */
    fscanf(fp,"%d %d\n", &t1,&t2);

    /* Read the coordinates of the exit. */
    fscanf(fp,"%d %d\n", &s1,&s2);

    /* Read the number of rows (n) and columns (m). */
    fscanf(fp,"%d %d\n", &n,&m);

    /* Read the maze. */
    for (i=0; i < n; i++){
        for(j=0; j < m; j++)
            fscanf(fp,"%d", &maze[i][j]);
     }

    initializeVertex();

    for (i=0; i < n; i++){
        for(j=0; j < m; j++){
            if(maze[i][j] == 0){
                if(maze[i+1][j] == 0 AND verifyBoundary(i+1,j))
                    addAdj(i,j,i+1,j);
                if(maze[i-1][j] == 0 AND verifyBoundary(i-1,j))
                    addAdj(i,j,i-1,j);
                if(maze[i][j+1] == 0 AND verifyBoundary(i,j+1))
                    addAdj(i,j,i,j+1);
                if(maze[i][j-1] == 0 AND verifyBoundary(i,j-1))
                    addAdj(i,j,i,j-1);
            }
        }
    }
}

int main(){

    readFile("maze.txt");

    createQueue(m*n);

    modifiedBFS();

    createStack(m*n);

    printAllPaths(function(t1,t2));

    if(pathExist == FALSE)
        printf("Nao existe caminho entre Thomas e a saida.\n");

    return 0;
}
