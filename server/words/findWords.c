#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "trie.c"

char lmatrix[5][5];

void findWordsUtil(NODE *f,char a[5][5],int visited[5][5],int i,int j,char str[30]){
    visited[i][j]=1;
    char x[2] = {(char)a[i][j],'\0'};
    strcat(str,x);
    printf("%s %s %d\n",str,x,strlen(str));
    if(find(f,str)){
        printf("%s\n",str);
    }
    int row,col;
    for(row=i-1;row<=i+1&&row<5;row++){
        for(col=j-1;col<=j+1&&col<5;col++){
            if(row>=0&&col>=0&&!visited[row][col]){
                findWordsUtil(f,a,visited,row,col,str);
            }
        }
    }
    str[strlen(str)-1] = '\0';
    visited[i][j]=0;
}

void findWords(NODE *f,char a[5][5]){
    int visited[5][5]={0};
    char str[30] = "\0";
    int i,j;
    for(i=0;i<5;i++){
        for(j=0;j<5;j++){
            findWordsUtil(f,a,visited,i,j,str);
        }
    }
}

// void generate_matrix()
// {
//     strcpy(lmatrix[0],"RSCLS");
//     strcpy(lmatrix[1],"DEIAE");
//     strcpy(lmatrix[2],"GNTRP");
//     strcpy(lmatrix[3],"IAESO");
//     strcpy(lmatrix[4],"LMIDC");

//     for(int i = 0 ; i<5 ; i++)
//     {
//         for(int j = 0 ; j<5 ; j++)
//         {
//             printf("%c ",lmatrix[i][j]);
//         }
//         printf("\n");
//     }

// }

// int main(){
//     NODE* f = giveNode('`');
//     NODE* b = giveNode('{');
//     fileops(f,b);

//     generate_matrix();

 
//     findWords(f,lmatrix);
// }