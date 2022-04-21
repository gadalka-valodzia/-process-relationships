#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
#include<string.h>
#include <signal.h>
#include <stdbool.h>
#include<time.h>
#include<sys/msg.h>
#include<sys/ipc.h>
#include<sys/sem.h>
extern char **environ;
void down(int semid,int sem_num);
void up(int semid,int sem_num);
bool end=false;
struct message
{
    char msg_type;
    char mes[22];
} mess;
union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
}arg;

void signal_handler(int signo)
{ 
    if(signo==SIGUSR2)
    {
        end=true;
    }
}
struct sembuf sbuf;

int main()
{
    srand(time(NULL));
    int semid;
    int msqid;
    key_t gtk;   
    char* text;
    text=(char*)malloc(22*sizeof(char));
    if(signal(SIGUSR2,signal_handler)==SIG_ERR) signal(SIGUSR2,SIG_IGN);//create key to create a message
    if((gtk=ftok("/tmp",'a'))==(key_t)-1)
    {
        perror("IPC error: ftok");
        exit(1);
    }
    msqid=msgget(gtk,0);         //create pack of messeges
    semid=semget(gtk,0,0);
   
    
    while(1)
    {
        sleep(1);
       down(semid,2);
       down(semid,1); 
       printf("send: ");
        for(int i=0;i<22;i++)
        {
        mess.mes[i]=rand()%('z'-'a')+'a';
        printf("%c",mess.mes[i]);
        }
        int error=msgsnd(msqid, &mess,22,0);
        if(error==0)printf("\n%d process  send message correctly",getpid());
        printf("\n");
       sleep(2);
       up(semid,1);
       up(semid,0);
        
        if(end==true)
        {
            msgctl(msqid,IPC_RMID,NULL);              //delete masseges
            break;
        }
    }
return 0;
}
void down(int semid,int sems_num)
{
    sbuf.sem_num=sems_num;
    sbuf.sem_op=-1;
    sbuf.sem_flg=0;
    semop(semid,&sbuf,1);
}
void up(int semid,int sems_num)
{
    sbuf.sem_num=sems_num;
    sbuf.sem_op=1;
    sbuf.sem_flg=0;
    semop(semid,&sbuf,1);
}
