#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "trie.c"

#define WAIT 55
#define NUM_MAT 9
#define TOP 1

char IP[15];
int PORT;

NODE* f ;
NODE* f1 ;

char lmatrix[5][5];

pthread_mutex_t matrix;
pthread_mutex_t dic;
pthread_mutex_t wordst;
pthread_mutex_t nodet;

pthread_mutex_t client_matrixt;
pthread_mutex_t client_scoret;
pthread_mutex_t scoreboardt;

int g = 0;

typedef struct scores{
    int j;
    int score;
    int cfd;
    int level;
    char name[20];
}SCORE;

typedef struct message{
    SCORE scoreboard[30];
    char words[500];
}MSG;

SCORE client_matrix[30];
SCORE client_score[30];

SCORE lboard[30];

MSG msg;


char s[30] = "./words/mat00.txt";

void generate_matrix()
{

    int h = s[12]-'0';

    if(g==NUM_MAT)
    {
        g = 0;
        s[11] = '0';
        s[12] = '1';
    }   
    else if(h==9)
    {
        s[12] = '0';
        s[11] += 1;
    }
    else
    {
        s[12] += 1;
    }

    printf("%s\n",s);

    FILE* fp = fopen(s,"r");

    char line[30];
    char word[20];
    for(int i = 0 ; i<5 ; i++)
    {   
        fgets(line,sizeof(line),fp);
        sscanf(line,"%s",word);
        strcpy(lmatrix[i],word);
        printf("%s\n",lmatrix[i]);
    }

    while(fgets(line,sizeof(line),fp)!=NULL)
    {
        sscanf(line,"%s",word);

        pthread_mutex_lock(&nodet);
            addWord(f,word);
        pthread_mutex_unlock(&nodet);
        // printf("%s\n",word);
    }
    
    g++;
}

int compare(const void * a, const void * b)
{
    SCORE p = *(SCORE*)a;
    SCORE q = *(SCORE*)b;

    return  (q.level==p.level)?(q.score)-(p.score):q.level-p.level;
}

void *MatrixGen(void *args)
{
    while(1)
    {
        pthread_mutex_lock(&matrix);

        sleep(WAIT);

        f = giveNode('`');
        f1 = giveNode('`');

        pthread_mutex_lock(&wordst);
            strcpy(msg.words,"WORDS\n");
        pthread_mutex_unlock(&wordst);
        
        generate_matrix();
        //printf("greetings from the lanisters!\n");

        for(int i = 0 ; i<30 ; i++)
            lboard[i] = msg.scoreboard[i];

        qsort(lboard,30,sizeof(SCORE),compare);

        int lev = lboard[0].level;
        int rank = 1;

        for(int i = 0 ; i<30 ; i++)
        {
            // printf("%d %d\n",lboard[i].level,lboard[i].score);
            if(lboard[i].score!=-1)
            {
                int j = lboard[i].j;

                if(msg.scoreboard[j].level!=lev)
                {
                    lev = msg.scoreboard[j].level;
                    rank = 1;
                }

                if(rank<=TOP && msg.scoreboard[j].score>0)
                {
                    if(lev==3)
                        msg.scoreboard[j].level = 1;
                    else
                        msg.scoreboard[j].level++;
                }
            }

            rank++;
        }

        for(int i = 0 ; i<30 ; i++)
            if(msg.scoreboard[i].score>0)
                msg.scoreboard[i].score = 0;

        // sleep(WAIT);

        pthread_mutex_unlock(&matrix);

        sleep(5);        

    }
}

void *ClientThread(void *args)
{
    int j = (int)args;

    int clientfd = msg.scoreboard[j].cfd;

    int rec;

    char str[20];

    if((rec=recv(clientfd,str,20,0)) > 0)
    {
        printf("client's name : %s\n",str);
        strcpy(msg.scoreboard[j].name,str);
    }

    for(int i = 0 ; i<20 ; i++)
        str[i] = '\0';

    while(1)
    {
        // printf("%f %f %f\n",total,clock(),t1);
        
        if((rec=recv(clientfd,str,20,0)) > 0)
        {
            printf("nreading from client:%s\n",str);
            pthread_mutex_lock(&nodet);
            if(find(f1,str))
            {
                strcpy(str,"D");
            }
            else
            {
                if(find(f,str))
                {
                    
                    addWord(f1,str);
                    
                    pthread_mutex_lock(&scoreboardt);
                        msg.scoreboard[j].score += strlen(str);
                    pthread_mutex_unlock(&scoreboardt);

                    pthread_mutex_lock(&wordst);
                        strcat(msg.words,str);
                        strcat(msg.words,"\n");
                    pthread_mutex_unlock(&wordst);
                    
                    strcpy(str,"Y");
                }
                else
                    strcpy(str,"N");
            }
            pthread_mutex_unlock(&nodet);

            send(clientfd,str,20,MSG_WAITALL);

            for(int i = 0 ; i<20 ; i++)
                str[i] = '\0';
        }
        else if((rec=recv(clientfd,str,20,0))==0 )
        {
            printf("client %d %s has disconnected\n",j,msg.scoreboard[j].name);

            pthread_mutex_lock(&scoreboardt);
                msg.scoreboard[j].score = -1;
                msg.scoreboard[j].level = 1;
            pthread_mutex_unlock(&scoreboardt);

            break;
        }

    }

    printf("closed\n");

    close(clientfd);

    pthread_exit(0);
}

void *MatrixSend(void* args)
{
    int j = (int)args;
    int clientfd = client_matrix[j].cfd;

    while(1)
    {
        pthread_mutex_lock(&matrix);
        pthread_mutex_unlock(&matrix);
        // findWords(f,lmatrix);



        if(send(clientfd,lmatrix,25,MSG_WAITALL)==-1)
        {
            printf("matrix connection gone bad\n");

            pthread_mutex_lock(&client_matrixt);
                client_matrix[j].score = -1;
            pthread_mutex_unlock(&client_matrixt);

            break;
        }

        sleep(10);
    }

    close(clientfd);

    pthread_exit(0);
}

void *matrixThread(void* args)
{
    struct sockaddr_in sock_var;
    int serverfd=socket(AF_INET,SOCK_STREAM,0);
    int clientfd;
    int cfd;

    pthread_t t[100];

    // generate_matrix();

    sock_var.sin_addr.s_addr = inet_addr(IP);
    sock_var.sin_port = PORT+1;
    sock_var.sin_family = AF_INET;

    int k = 0;

    if(bind(serverfd,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
    {
        printf("matrix socket has been created\n");
        listen(serverfd,0); 

        for(int i = 0 ; i<100 ; i++)
        {
                clientfd = accept(serverfd,NULL,NULL);
                printf("nConnected to matrix client %d\n",clientfd);
                
                for(int j = 0 ; j<30 ; j++)
                {
                    if(client_matrix[j].score==-1)
                    {
                        pthread_mutex_lock(&client_matrixt);

                        k = j;
                        client_matrix[j].score = 0;
                        client_matrix[j].cfd = clientfd;

                        pthread_mutex_unlock(&client_matrixt);
                        break;
                    }
                }                 
                
                pthread_create(&t[k],NULL,MatrixSend,(void *)k);
            
        }
        close(serverfd);
    }
    else
    {
        printf("matrix socket creation failed\n");
    }    

    pthread_exit(0);
}

void *scoreSend(void* args)
{
    int j = (int)args;
    int clientfd = client_score[j].cfd;

    while(1)
    {
        // findWords(f,lmatrix);
        if(send(clientfd,&msg,sizeof(msg),MSG_WAITALL)==-1)
        {
            printf("score connection gone bad\n");

            pthread_mutex_lock(&client_scoret);
            client_score[j].score = -1;
            pthread_mutex_unlock(&client_scoret);

            break;
        }

        sleep(1);

    }

    close(clientfd);

    pthread_exit(0);
}

void *scoreThread(void* args)
{
    struct sockaddr_in sock_var;
    int serverfd=socket(AF_INET,SOCK_STREAM,0);
    int clientfd;
    int cfd;

    pthread_t t[100];


    sock_var.sin_addr.s_addr = inet_addr(IP);
    sock_var.sin_port = PORT+2;
    sock_var.sin_family = AF_INET;

    int k = 0;

    strcpy(msg.words,"WORDS\n");

    if(bind(serverfd,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
    {
        printf("score socket has been created\n");
        listen(serverfd,0); 

        for(int i = 0 ; i<100 ; i++)
        {
                clientfd = accept(serverfd,NULL,NULL);
                printf("nConnected to score client %d\n",clientfd);
                
                for(int j = 0 ; j<30 ; j++)
                {
                    if(client_score[j].score==-1)
                    {
                        pthread_mutex_lock(&client_scoret);

                        k = j;
                        client_score[j].score = 0;
                        client_score[j].cfd = clientfd;

                        pthread_mutex_unlock(&client_scoret);
                        break;
                    }
                }                
                
                pthread_create(&t[k],NULL,scoreSend,(void *)k);
            
        }
        close(serverfd);
    }
    else
    {
        printf("score socket creation failed\n");
    }    

    pthread_exit(0);
}

int main()
{
    printf("Enter IP address of server and PORT:\n");

    scanf("%s %d",IP,&PORT);

    struct sockaddr_in sock_var;
    int serverfd=socket(AF_INET,SOCK_STREAM,0);
    int clientfd;
    int cfd;

    pthread_t t[100];
    pthread_t matrix_th;
    pthread_t score_th;
    pthread_t matgen;

    // generate_matrix();

    sock_var.sin_addr.s_addr = inet_addr(IP);
    sock_var.sin_port = PORT;
    sock_var.sin_family = AF_INET;

    struct sockaddr_in socket2;
    int serverSocket2 = socket(AF_INET,SOCK_STREAM,0);
    int cfd2;


    pthread_mutex_init(&matrix,NULL);
    pthread_mutex_init(&dic,NULL);

    pthread_mutex_init(&client_matrixt,NULL);
    pthread_mutex_init(&client_scoret,NULL);
    pthread_mutex_init(&scoreboardt,NULL);
    pthread_mutex_init(&wordst,NULL);
    pthread_mutex_init(&nodet,NULL);

    // fileops(f);

    for(int i = 0 ; i<30 ; i++)
    {
        msg.scoreboard[i].score = -1;
        msg.scoreboard[i].j = -1;
        msg.scoreboard[i].level = 1;

        client_matrix[i].score = -1;
        client_score[i].score = -1;
    }

    pthread_create(&matrix_th,NULL,matrixThread,(void *)1);
    pthread_create(&score_th,NULL,scoreThread,(void *)1);
    pthread_create(&matgen,NULL,MatrixGen,(void *)1);

    int k = 0;

    if(bind(serverfd,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
    {
        printf("nsocket has been created\n");
        listen(serverfd,0); 

        for(int i = 0 ; i<100 ; i++)
        {
                clientfd = accept(serverfd,NULL,NULL);
                printf("nConnected to client %d\n",clientfd);

                
                for(int j = 0 ; j<30 ; j++)
                {
                    if(msg.scoreboard[j].score==-1)
                    {
                        pthread_mutex_lock(&scoreboardt);
                        
                        k = j;
                        msg.scoreboard[j].j = j;
                        msg.scoreboard[j].score = 0;
                        msg.scoreboard[j].cfd = clientfd;
                        
                        pthread_mutex_unlock(&scoreboardt);
                        break;
                    }
                }
                printf("k ---------- %d\n",k);
                pthread_create(&t[k],NULL,ClientThread,(void *)k);
            
        }
        close(serverfd);
    }
    else
    {
        printf("nsocket creation failed\n");
    }

    pthread_join(matrix_th,NULL);

    return 0;
}
