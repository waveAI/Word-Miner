#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define CHARS 28

typedef struct node{
    char c;
    int isword;
    struct node* front[CHARS];
}NODE;

NODE* giveNode(char c)
{
    NODE* n = (NODE*)malloc(sizeof(NODE));
    n->c = c;
    n->isword = 0;
    for(int i = 0 ; i<CHARS ; i++)
    {
        n->front[i] = NULL;
    }

    return n;
}

NODE* FRONT;


void addWord(NODE* F,char* s)
{
    int len = strlen(s);

    NODE* a;
    NODE* b;
    int i , j;
    
    a = F;
    for(i = 0 ; i<len ; i++)
    {
        int d = s[i]-'`'; 

        if(a->front[d]==NULL)
            break;
        else
            a = a->front[d];
    }


    NODE* p = a;
    NODE* q;

    for(int k = i ; k<len ; k++)
    {
        int d = s[k]-'`';

        q = giveNode(s[k]);
        p->front[d] = q;

        p = q;
    }

    p->isword = 1;
    
    return;
}

int find(NODE* F,char *s)
{
    int len = strlen(s);
    NODE* a = F;
    int k = 1;

    for(int i = 0 ; i<len ; i++)
    {
        if(!isalpha(s[i]))
        {
            k = 0;
            break;
        }     

        s[i] = tolower(s[i]);
    }

    // printf("%s %d\n",s,len);

    if(!k)
        return 0;

    for(int i = 0 ; i<len ; i++)
    {
        int d = s[i]-'`';

        if(a->front[d]==NULL)
            return 0;
        
        a = a->front[d];
        // printf("%c\n",a->c);
    }

    if(a->isword==1)
        return 1;

    return 0;
}

void fileops(NODE* f)
{
   FILE * fp = fopen ("words.txt", "r+");

   char word[20];
   char s2[20];
   char s3[20];
   int count;

   char line[50];

   int g = 0;
   while(fgets(line,sizeof(line),fp)!=NULL)
   {
        sscanf(line,"%s",word);

        int k = 1;
        int l = strlen(word);
        g++;
        if(l<=3)
            continue;

        for(int i = 0 ; i<l ; i++)
        {
            if(!isalpha(word[i]))
            {
                k = 0;
                break;
            }
            word[i] = tolower(word[i]);
        }

        if(k && l>2)
        {
            addWord(f,word);
        }

        if(feof(fp))
        {
            break;
        }
   }

   return ;
}



// int main()
// {
//     NODE* f = giveNode('`');

//     fileops(f);
    
//     char s[30];

//     strcpy(s,"codia");

//     if(find(f,s))
//         printf("YES!!\n");
//     else
//         printf("NO\n");

// }
