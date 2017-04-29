#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "trie.c"


int wrd = 0;

FILE* fp;

void generate_matrix(char mat[5][5])
{
	strcpy(mat[0],"BTOWT");
	strcpy(mat[1],"OOLAU");
	strcpy(mat[2],"TLSRM");
	strcpy(mat[3],"QAEHE");
	strcpy(mat[4],"CERWD");

    for(int i = 0 ; i<5 ; i++)
    {
        for(int j = 0 ; j<5 ; j++)
        {
            mat[i][j] = tolower(mat[i][j]);
            fprintf(fp,"%c",mat[i][j]);
            printf("%c ",mat[i][j]);
        }
        fprintf(fp,"\n");
        printf("\n");
    }
}

void dfs(NODE* f,int u,int v,NODE* p,char s[30],NODE* q,char mat[5][5],int visited[5][5])
{

    if(u<0 || v<0 || u>5 || v>5)
        return;

    visited[u][v] = 1;

    if(p->isword)
    {
        addWord(q,s);
        fprintf(fp,"%s\n",s);
        // printf("%s\n",s);
    }

    for(int i = u-1 ; i<=u+1 ; i++)
    {
        for(int j = v-1 ; j<=v+1 ; j++)
        {
            if(i>=0 && j>=0 && i<5 && j<5)
            {
                if(!visited[i][j])
                {                
                    char s1[30];
                    strcpy(s1,s);
                    
                    int d = mat[i][j] - '`';
                    if(p->front[d]!=NULL)
                    {
                        char s2[2] = {mat[i][j],'\0'};
                        strcat(s1,s2);
                        dfs(f,i,j,p->front[d],s1,q,mat,visited);
                    }
                }
            }
        }
    }

    visited[u][v] = 0;

}

void findWords(NODE* f,NODE* q,char mat[5][5])
{

    f = giveNode('`');

    fileops(f);

    int visited[5][5] = {0};

    for(int i = 0 ; i<5 ; i++)
        for(int j = 0 ; j<5 ; j++)
            visited[i][j] = 0;

    for(int i = 0 ; i<5 ; i++)
    {
        for(int j = 0 ; j<5 ; j++)
        {
            int d = mat[i][j] - '`';
            char s[30] = {mat[i][j],'\0'};
            if(f->front[d]!=NULL)
            {
                // printf("%d %d\n",i,j);
                dfs(f,i,j,f->front[d],s,q,mat,visited);
            }
        }
    }

}

int main()
{
    char mat[5][5];
    

    NODE* q = giveNode('`');
    NODE* f;

    fp = fopen("mat02.txt","w");
    
    generate_matrix(mat);
    findWords(f,q,mat);

    char s[30];



}
