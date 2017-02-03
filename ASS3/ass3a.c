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
	int i;
	for(i=1;i<=50;i++){
		P(semid3);
		buff[buff[20]]=i;
		buff[20]=(buff[20]+1)%20;
		printf("Producer (%d) writes %d\n",getpid(),i);
		//V(semid1);
		V(semid4);
		//V(semid2);
		
	}
	return;
}

void consumer(int *buff){
	while(buff[22]!=m*25*51){
		P(semid1);
		P(semid4);
		buff[22]=buff[22]+buff[buff[21]];
		printf("Consumer (%d) reads %d SUM= %d in=%d  out=%d\n",getpid(),buff[buff[21]],buff[22],buff[20],buff[21]);
		buff[21]=(buff[21]+1)%20;
		V(semid3);
		V(semid1);
		if(buff[22]==m*25*51)exit(1);
	}
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
		int id;
		//buff=(int *)shmat(shmid,NULL,0);
		while(m--){
			id=fork();
			if(id==0){
				producer(buff);
				exit(1);
			}

		}
	}
	else{
		int id;
		//buff=(int *)shmat(shmid,NULL,0);
		while(n--){
			id=fork();
			if(id==0){
				consumer(buff);
				exit(1);
			}
		}
	
		wait(NULL);
		//semctl(semid1, 0, IPC_RMID, 0);
		//semctl(semid2, 0, IPC_RMID, 0);
		//semctl(semid3, 0, IPC_RMID, 0);
		//semctl(semid4, 0, IPC_RMID, 0);	
		//shmctl(shmid, IPC_RMID, 0);
	}
	wait(NULL);
	if(id)printf("Sum=%d\n",buff[22]);
	return 0;
}