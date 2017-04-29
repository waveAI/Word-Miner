#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <string.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include <ctype.h>

#include "trie.c"

#define WAIT 10

char IP[15];
int PORT;

NODE* f ;

int client_fd;
int score_fd;
int matrix_fd;

GtkWidget *g_entry1;
GtkWidget *g_displayLabel;
GtkWidget *g_displayButton;

GtkWidget* g_label[5][5];

GtkWidget* g_score[11];
GtkWidget* g_name[11];
GtkWidget* g_label2;

GtkWidget* g_ur_score;
GtkWidget* g_ur_name;

GtkWidget* g_words;

pthread_mutex_t levt;

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

int sh = 0;

void *matrixThread(void* args)
{
    struct sockaddr_in sock_var;
    int clientFileDiscriptor=socket(AF_INET,SOCK_STREAM,0);
    matrix_fd = clientFileDiscriptor;
    char str_clnt[20],str_ser[20];
    
    int sen , rec , scn;

    sock_var.sin_addr.s_addr=inet_addr(IP);
    sock_var.sin_port=PORT+1;
    sock_var.sin_family=AF_INET;

    char lmatrix[5][5];

    if(connect(clientFileDiscriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
    {

        while(1)
        {

            if((rec=recv(clientFileDiscriptor,lmatrix,25,MSG_WAITALL))>0)
            {

                if(!sh)
                    sh = 1;

                gtk_label_set_text(GTK_LABEL(g_words),"WORDS\n");
                for(int i = 0 ; i<5 ; i++)
                {
                    for(int j = 0 ;  j<5 ; j++)
                    {
                        char s[2] = {toupper(lmatrix[i][j])};
                        // printf("%s ",s);
                        gtk_label_set_text(GTK_LABEL(g_label[i][j]),s);                      
                    }
                }

                f = giveNode('`');
                // strcpy(show,"YOUR WORDS\n");
                // gtk_label_set_text(GTK_LABEL(g_words), show);
            }
        }


        close(clientFileDiscriptor);
    }
    else
    {
        printf("matrix socket creation failed\n");
    }  

    pthread_exit(0);
}

int compare(const void * a, const void * b)
{
    SCORE p = *(SCORE*)a;
    SCORE q = *(SCORE*)b;

    return (q.score)  - (p.score);
}

char cname[20];
int h = 0;
int lev = 1;

void *scoreThread(void* args)
{
    struct sockaddr_in sock_var;
    int clientFileDiscriptor=socket(AF_INET,SOCK_STREAM,0);
    score_fd = clientFileDiscriptor;

    int sen , rec = 1 , scn;

    sock_var.sin_addr.s_addr=inet_addr(IP);
    sock_var.sin_port=PORT+2;
    sock_var.sin_family=AF_INET;


    int gtk_ptr = 1;
    char scr[10];

    if(connect(clientFileDiscriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
    {
        MSG msg;

        while(1)
        {
            if(rec==0)
            {
                gtk_label_set_text(GTK_LABEL(g_label2),"Server Error");
                sleep(5);
                
                close(client_fd);
                close(score_fd);    
                close(matrix_fd);
                
                gtk_main_quit();
            }
            if((rec=recv(clientFileDiscriptor,&msg,sizeof(msg),MSG_DONTWAIT))>0)
            {
                gtk_label_set_text(GTK_LABEL(g_words),msg.words);
                qsort(msg.scoreboard,30,sizeof(SCORE),compare);

                int j = 0;
                for(int i = 0 ; i<30 ; i++)
                {
                    if(!strcmp(msg.scoreboard[i].name,cname))
                    {
                        j = i;
                        break;
                    }
                }

                int l = msg.scoreboard[j].level;
                
                if(lev!=l)
                {
                    pthread_mutex_lock(&levt);
                    lev = l;
                    pthread_mutex_unlock(&levt);
                }
                
                char scr[10];
                sprintf(scr,"%d",msg.scoreboard[j].score);
                gtk_label_set_text(GTK_LABEL(g_ur_score), scr);

                if(l==3)
                    h = 1;
                
                if(h==1 && l==1)
                {
                    h = 0;
                    gtk_label_set_text(GTK_LABEL(g_label2),"YOU WIN!!!");  
                }
                else
                {
                    char lev[10] = "LEVEL 0";
                    lev[6] += l;
                    gtk_label_set_text(GTK_LABEL(g_label2),lev);
                }

                for(int i = 0 ; i<10 ; i++)
                {
                    if(msg.scoreboard[i].level==l)
                    {
                        if(msg.scoreboard[i].score >= 0)
                        {
                            gtk_label_set_text(GTK_LABEL(g_name[gtk_ptr]),msg.scoreboard[i].name); 
                            sprintf(scr,"%d",msg.scoreboard[i].score);
                            gtk_label_set_text(GTK_LABEL(g_score[gtk_ptr++]),scr); 
                        }
                    }
                        // printf("%s %d\n",msg.scoreboard[i].name,msg.scoreboard[i].score);
                }

                while(gtk_ptr<10)
                {
                    gtk_label_set_text(GTK_LABEL(g_name[gtk_ptr]),"");
                    gtk_label_set_text(GTK_LABEL(g_score[gtk_ptr++]),"");  
                }

                gtk_ptr = 1;
            }

            
        }


        close(clientFileDiscriptor);
    }
    else
    {
        printf("score socket creation failed\n");
    }  

    pthread_exit(0);
}

int cfd ;

int my_score = 0;


void on_button1_clicked()
{
    char* input;
    char str_ser[20];
    int sen , rec;
    // gtk_label_set_text(GTK_LABEL(g_displayLabel), "Hello, world!");

    if(!sh)
    {
        gtk_label_set_text(GTK_LABEL(g_label2),"Wait for next game");
        gtk_entry_set_text(g_entry1,"");
        return ;        
    }

    input = gtk_entry_get_text(g_entry1);
    pthread_mutex_lock(&levt);
    if(strlen(input)<lev+2)
    {
        char len[20] = "Min word length : 2";
        len[18] += lev;
        gtk_label_set_text(GTK_LABEL(g_label2),len);
        gtk_entry_set_text(g_entry1,"");
        pthread_mutex_unlock(&levt);
        return;
    }
    pthread_mutex_unlock(&levt);

    printf("%s\n",input);


    if(find(f,input))
    {
        gtk_label_set_text(GTK_LABEL(g_label2), "Aldready Entered");
        gtk_entry_set_text(g_entry1,"");
        return ;
    }

    addWord(f,input);

    if((sen=send(cfd,input,strlen(input),MSG_WAITALL))==strlen(input))
    {
        printf("you entered: %s\n",input);
        gtk_entry_set_text(g_entry1,"");
        if((rec=recv(cfd,str_ser,20,0))>0)
        {
            printf("from server : %s\n",str_ser);

            if(!strcmp(str_ser,"Y"))
            {
                gtk_label_set_text(GTK_LABEL(g_label2), "Found :)");

                // strcat(show,input);
                // strcat(show,"\n");
                // printf("show %s\n",show);
                // gtk_label_set_text(GTK_LABEL(g_words), show);
            }
            else if(!strcmp(str_ser,"D"))
            {
                gtk_label_set_text(GTK_LABEL(g_label2), "Someone is fast :|");
            }
            else
            {
                gtk_label_set_text(GTK_LABEL(g_label2), "Not Found :(");
            }
        }
    }

    // gtk_label_set_text(GTK_LABEL(g_displayLabel), input);
}

void on_game_window_destroy()
{
    // printf("%d %d yo\n",client_fd,score_fd);
    close(client_fd);
    close(score_fd);
    close(matrix_fd);
    // printf("mama %d\n",g);
    gtk_main_quit();
}

int main(int *argc,char* argv[])
{

    printf("Enter Server IP address and PORT no.\n");

    scanf("%s%d",IP,&PORT);

    f = giveNode('`');

    pthread_mutex_init(&levt,NULL);

    GtkBuilder      *builder; 
    GtkWidget       *window;
 
    gtk_init(&argc, &argv);
 

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "glade/game_window.glade", NULL);
    
    window = GTK_WIDGET(gtk_builder_get_object(builder, "game_window"));
    
    
    // get pointers to the two labels
    // g_displayLabel = GTK_WIDGET(gtk_builder_get_object(builder, "displayLabel"));
    g_entry1 = GTK_WIDGET(gtk_builder_get_object(builder, "entry1"));  

    int k = 0;
    char str[10] = "label-00";

    for(int i = 0 ; i<5 ; i++)
        for(int j = 0 ; j<5 ; j++)
        {
            str[6] = (char)('0'+i);
            str[7] = (char)('0'+j);
            // printf("%s\n",str);
            g_label[i][j] = GTK_WIDGET(gtk_builder_get_object(builder, str));
        }
    
    char str1[10] = "score_0";
    char str2[10] = "name_0";

    for(int i = 1 ; i<10 ; i++)
    {
        str1[6] = (char)('0'+i);
        str2[5] = (char)('0'+i);
        g_score[i] = GTK_WIDGET(gtk_builder_get_object(builder, str1));
        g_name[i] = GTK_WIDGET(gtk_builder_get_object(builder, str2));
    }

        g_score[10] = GTK_WIDGET(gtk_builder_get_object(builder, "score_10"));
        g_name[10] = GTK_WIDGET(gtk_builder_get_object(builder, "name_10"));

        g_ur_name = GTK_WIDGET(gtk_builder_get_object(builder, "ur_name"));
        g_ur_score = GTK_WIDGET(gtk_builder_get_object(builder, "ur_score"));
        g_label2 = GTK_WIDGET(gtk_builder_get_object(builder, "label2"));

        g_words = GTK_WIDGET(gtk_builder_get_object(builder, "words"));

        g_signal_connect(g_entry1, "activate", G_CALLBACK(on_button1_clicked), NULL);

    struct sockaddr_in sock_var;
    int clientFileDiscriptor=socket(AF_INET,SOCK_STREAM,0);
    cfd = clientFileDiscriptor;
    client_fd = clientFileDiscriptor;

    char str_clnt[20],str_ser[20];
    
    int sen , rec , scn;

    sock_var.sin_addr.s_addr=inet_addr(IP);
    sock_var.sin_port=PORT;
    sock_var.sin_family=AF_INET;
    
    char lmatrix[5][5];

    if(connect(clientFileDiscriptor,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
    {


        printf("Connected to server %d\n",clientFileDiscriptor);
        
        pthread_t matrix_th;
        pthread_t score_th;

        pthread_create(&matrix_th,NULL,matrixThread,(void *)1);
        pthread_create(&score_th,NULL,scoreThread,(void *)1);

        printf("Enter you name: \n");
        scanf("%s",str_clnt);
        strcpy(cname,str_clnt);

        gtk_label_set_text(GTK_LABEL(g_ur_name), str_clnt);

        sen=send(clientFileDiscriptor,str_clnt,strlen(str_clnt),MSG_WAITALL);

        // GtkCssProvider *cssProvider = gtk_css_provider_new ();
        // gtk_css_provider_load_from_path(cssProvider,"glade/style.css",NULL);
        // gtk_style_context_add_provider(gtk_widget_get_style_context(window),cssProvider,GTK_STYLE_PROVIDER_PRIORITY_USER);

        gtk_builder_connect_signals(builder, NULL);

        g_object_unref(builder);
    
        gtk_widget_show(window);                
        gtk_main();

        
        // while(1)
        // {
        //     // printf("Enter your word:\n");
        //     // scanf("%s",str_clnt);

        //     // if((sen=send(clientFileDiscriptor,str_clnt,strlen(str_clnt),MSG_WAITALL))==strlen(str_clnt))
        //     // {
        //     //     printf("you entered: %s\n",str_clnt);
        //     //     if((rec=recv(clientFileDiscriptor,str_ser,20,0))>0)
        //     //     {
        //     //         printf("from server : %s\n",str_ser);
        //     //     }
        //     // }
    
        // }

        // pthread_join(matrix_th,NULL);
        close(clientFileDiscriptor);
    }
    else
    {
        printf("socket creation failed\n");
    }
    return 0;
}