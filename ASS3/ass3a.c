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
		buff[*in]=i;
		*in=(*in+1)%20;
		printf("Producer (%d) writes %d\n",getpid(),i);
		V(semid2);
		V(semid1);
		V(semid4);
	}

}

void consumer(int *buff){
	int *in,*out,*SUM;
	in=&buff[20];
	out=&buff[21];
	SUM=&buff[22];
	while(1){
		P(semid4);
		P(semid1);
		int tmp=buff[*out];
		*out=(*out+1)%20;
		printf("Consumer (%d) reads %d\n",getpid(),tmp);
		*SUM=*SUM+tmp;
		V(semid1);
		V(semid3);
		//if(SUM==m*25*51)break;
	}
}

int main(){
	int shmid,id;
	int *SUM,*buff,*in,*out;
	scanf("%d %d",&m,&n);
	shmid=shmget(IPC_PRIVATE,30*sizeof(int),0777|IPC_CREAT);
	buff=(int *)shmat(shmid,NULL,0);
	in=&buff[20];
	out=&buff[21];
	SUM=&buff[22];
	signal_op.sem_num=0;
	wait_op.sem_op=-1;
	wait_op.sem_flg=signal_op.sem_num=0;
	signal_op.sem_op=1;
	semid1 = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
	semid2 = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
	semctl(semid1,0,SETVAL,0);
	semctl(semid2,0,SETVAL,1);
	semctl(semid3,0,SETVAL,20);
	semctl(semid4,0,SETVAL,0);

	id=fork();
	if(id==0){
		int id;
		buff=(int *)shmat(shmid,NULL,0);
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
		buff=(int *)shmat(shmid,NULL,0);
		while(n--){
			id=fork();
			if(id==0){
				consumer(buff);
				exit(1);
			}
		}
		semctl(semid1, 0, IPC_RMID, 0);
		semctl(semid2, 0, IPC_RMID, 0);
		semctl(semid3, 0, IPC_RMID, 0);
		semctl(semid4, 0, IPC_RMID, 0);	
		shmctl(shmid, IPC_RMID, 0);
	}

	return 0;
}