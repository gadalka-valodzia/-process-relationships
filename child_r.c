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

void signal_handler(int signo)
{ 
    if(signo==SIGUSR2)
    {
        end=true;
    }
}

union semun             
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
}arg;

struct sembuf sbuf;
int main()
{
    int msqid;
    int semid;
    char* text;
    text=(char*)malloc(22*sizeof(char));  
    key_t gtk;                                              //create key to create a message
    if(signal(SIGUSR2,signal_handler)==SIG_ERR) signal(SIGUSR2,SIG_IGN);    //for signal
    if((gtk=ftok("/tmp",'a'))==(key_t)-1)   //get key
    {
        perror("IPC error: ftok");
        exit(1);
    }
    semid=semget(gtk,0,0);  //give id for semaphore arrays
    msqid=msgget(gtk,0);    //give id for message queues
    while(1)
    {
        sleep(1);
        down(semid,0);
        down(semid,1);
        int size=msgrcv(msqid,&mess,22,0,0);            //receve messege
        if(size==-1)printf("fail to receve message");
        else printf("\n\nmessage receve correctly with size=%d\n",size);
        printf("receved text :  ");
        for(int i=0;i<22;i++)
            printf("%c",mess.mes[i]);
        printf("\n------------------------------\n");
        up(semid,1);
        up(semid,2);
        if(end==true)
        {
            msgctl(msqid,IPC_RMID,NULL);              //delete masseges
            break;
        }
    }
return 0;
}
void down(int semid,int sems_num)                   //for dicrement semaphore
{
    sbuf.sem_num=sems_num;
    sbuf.sem_op=-1;
    sbuf.sem_flg=0;
    semop(semid,&sbuf,1);
}
void up(int semid,int sems_num)                     //for increment semaphore
{
    sbuf.sem_num=sems_num;
    sbuf.sem_op=1;
    sbuf.sem_flg=0;
    semop(semid,&sbuf,1);
}
