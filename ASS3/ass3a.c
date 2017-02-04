#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <sys/wait.h>
#include <sys/sem.h>

#define P(s) semop(s, &wait_op, 1) 
#define V(s) semop(s, &signal_op, 1)  

struct sembuf wait_op,signal_op;
int semid1, semid2,semid3,semid4;


int m,n;
	
void producer(int *buff){
	int *in,*out,*SUM;
	in=&buff[20];
	out=&buff[21];
	SUM=&buff[22];
	int i;
	for(i=1;i<=50;i++){
		P(semid3);
		P(semid2);
		buff[buff[20]]=i;
		buff[24]+=i;
		buff[20]=(buff[20]+1)%20;
		V(semid2);
		printf("Producer (%d) writes %d\n",getpid(),i);
		V(semid4);
	}
	return;
}

void consumer(int *buff){
	while(buff[22]!=m*25*51){
		P(semid1);
		P(semid4);
		if(buff[22]!=m*25*51)buff[22]=buff[22]+buff[buff[21]];
		printf("Consumer (%d) reads %d SUM= %d in=%d  out=%d\n",getpid(),buff[buff[21]],buff[22],buff[20],buff[21]);
		buff[21]=(buff[21]+1)%20;
		V(semid1);
		V(semid3);
		if(buff[22]>=m*25*51)break;
	}
	exit(1);
}

int main(){
	int shmid,id;
	int *SUM,*buff,*in,*out;
	scanf("%d %d",&m,&n);
	shmid=shmget(IPC_PRIVATE,30*sizeof(int),0777|IPC_CREAT);
	buff=(int *)shmat(shmid,NULL,0);
	buff[20]=0;
	buff[21]=0;
	buff[22]=0;
	signal_op.sem_num=wait_op.sem_num=0;
	wait_op.sem_op=-1;
	wait_op.sem_flg=signal_op.sem_num=0;
	signal_op.sem_op=1;
	semid1 = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
	semid2 = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
	semid3 = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
	semid4 = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
	semctl(semid1,0,SETVAL,1);
	semctl(semid2,0,SETVAL,1);
	semctl(semid3,0,SETVAL,20);
	semctl(semid4,0,SETVAL,0);

	id=fork();
	if(id==0){
		buff[23]=0;
		buff[24]=0;
		int id2;
		int k=m;
		while(k--){
			id2=fork();
			if(id2==0){
				producer(buff);
				exit(1);
			}

		}
		while(m--)wait(NULL);
	}
	else{
		int id3;
		int k=n;
		while(k--){
			id3=fork();
			if(id3==0){
				consumer(buff);
				exit(1);
			}
		}
		wait(NULL);
		while(--n){V(semid4);wait(NULL);}
		
		
	}
	
	return 0;
}