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
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/msg.h>
extern char **environ;

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
}arg;

int main()
{   
    char option;
    int msqid;
    int qtk;
    int semset_id;
    int status,flag=1,o=0,ret;
    pid_t numpid[99];
    pid_t pid;
    key_t gtk;
    
    if((gtk=ftok("/tmp",'a'))==(key_t)-1)          //give key
    {
        perror("IPC error: ftok");
        exit(1);
    }
    
    while(1){ 
        option=getchar();
        if(option=='+')
            {
                qtk=semget(gtk,3,IPC_CREAT|0666);              
                msqid=msgget(gtk,(IPC_CREAT|0666));            
                
                arg.val=0;//for items(for  receve)
                int q=semctl(qtk,0,SETVAL,arg);
                    
                arg.val=1;//for mutex(for send)
                q=semctl(qtk,1,SETVAL,arg);
                    
                arg.val=2;//for free_space
                q=semctl(qtk,2,SETVAL,arg);
                for(int i=1;i<3;i++)
                {
                    printf("<================================PROCESS № %d===========================>\n",o+1);
                    pid=fork();
                    if(pid>1) 
                    {
                        numpid[o]=pid;
                        printf("child pid =%d\n\n",numpid[o]);   
                        o=o+1;
                        flag=0;
                    }
                    if(pid==-1)
                    perror("fork");
                    if(!pid && (i%2==0))
                    {   
                        ret=execve("/home/vova/laba5/child",NULL,NULL);
                        if(ret==-1)
                        {
                            perror("execv");
                        }
                    } 
                    else if(!pid && (i%2==1))
                    {
                        ret=execve("/home/vova/laba5/child1",NULL,NULL);
                            if(ret==-1)
                            {
                                perror("execv");
                            }  
                    }
                    
                }
            }  
        else if(option=='-' && flag==0)
            {
                for(int i=0;i<2;i++)
                {
                     if(o==2)
                    {
                        int rmid=semctl(qtk,0,IPC_RMID,0);
                        for(int i=0;i<o;i++)
                        {
                            ret=kill(numpid[i],SIGUSR2);
                            printf("%d\n",numpid[i]);
                        }
                        if(ret!=0)  
                        {
                            printf("WARNING\n"); 
                        }
                    }
                    o=o-1;
                    printf("child pid delete=%d\n\nu have  %d  CHILD_PROCESS\n\n",numpid[o],o);
                    ret=kill(numpid[o],SIGKILL);
                    waitpid(numpid[o], &status, 0);
                    if(!ret)
                        perror("kill");
                    if(o==0)
                    {
                        printf("delete all child process");
                        flag=1;
                    }
                }
            }
        else if(option=='e'&&flag==1)
            {
                return 0;
            }
        }

        exit(EXIT_SUCCESS);
        return 0;
}
