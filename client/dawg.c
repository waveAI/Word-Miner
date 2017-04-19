#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define CHARS 28

typedef struct node{
    char c;
    struct node* front[CHARS];
    struct node* back[CHARS];
}NODE;

NODE* giveNode(char c)
{
    NODE* n = (NODE*)malloc(sizeof(NODE));
    n->c = c;
    for(int i = 0 ; i<CHARS ; i++)
    {
        n->front[i] = NULL;
        n->back[i] = NULL;
    }

    return n;
}

NODE* FRONT;
NODE* BACK;


void addWord(NODE* F,NODE* B,char* s)
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

    b = B;
    for(j = len-1 ; j>i ; j--)
    {
        int d = s[j]-'`';

        if(b->back[d]==NULL)
            break;
        else
            b = b->back[d];
    }

    NODE* p = a;
    NODE* q;

    for(int k = i ; k<=j ; k++)
    {
        int d = s[k]-'`';
        int e = p->c - '`';

        q = giveNode(s[k]);
        p->front[d] = q;
        q->back[e] = p;

        p = q;
    }

    int d = (j==len-1)?27:s[j+1]-'`';
    int e = p->c - '`';

    p->front[d] = b;
    b->back[e] = p;
    
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

    printf("%s %d\n",s,len);

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

    int d = '{' - '`';
    if(a->front[d]!=NULL && a->front[d]->c=='{')
        return 1;

    return 0;
}

void fileops(NODE* f,NODE* b)
{
   FILE * fp = fopen ("wrds.txt", "r+");

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
            addWord(f,b,word);
        }

        if(feof(fp))
        {
            break;
        }
   }

   return ;
}



int main()
{
    NODE* f = giveNode('`');
    NODE* b = giveNode('{');

    fileops(f,b);
    
    char s[30];

    strcpy(s,"cod");

    if(find(f,s))
        printf("YES!!\n");
    else
        printf("NO\n");
    // if(find(f,b,"painter"))
    //     printf("YES!!\n");
    // else
    //     printf("NO\n");
    // if(find(f,b,"paint"))
    //     printf("YES!!\n");
    // else
    //     printf("NO\n");
    // if(find(f,b,"taint"))
    //     printf("YES!!\n");
    // else
    //     printf("NO\n");
    // if(find(f,b,"ghoul"))
    //     printf("YES!!\n");
    // else
    //     printf("NO\n");
    // if(find(f,b,"komal"))
    //     printf("YES!!\n");
    // else
    //     printf("NO\n");
    // if(find(f,b,"ghool"))
    //     printf("YES!!\n");
    // else
    //     printf("NO\n");
    // if(find(f,b,"merchandised"))
    //     printf("YES!!\n");
    // else
    //     printf("NO\n");

}
