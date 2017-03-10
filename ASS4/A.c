#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <sys/wait.h>
#include <sys/sem.h>

#define BUF_SIZE 10
#define P(s) semop(s, &wait_op, 1) 
#define V(s) semop(s, &signal_op, 1)  

int *buff,semid[4];
struct sembuf wait_op,signal_op;
int shmid1,shmid2,shmid3;


void random_sleep(){
	int tme;
	tme=rand()%3;
	sleep(tme);
}

int random_number(){
	return (rand()%11 - 5);
}


void producer(int n){
	P(semid[2]);		// check if buffer is full
	P(semid[1]);		// producer critical section
	buff[buff[10]]=n;
	buff[10]=(buff[10]+1)%BUF_SIZE;
	V(semid[1]);		
	printf("A writes %d\n",n);
	V(semid[3]); // consumer can read the data
}

void my_handler(){
	shmdt(buff);
	exit(0);
}



int main(){
	srand((unsigned)time(NULL)); 
	int n;
	
	signal(SIGINT, my_handler); // ctrl+c will detach and delete shared memory


	shmid3 = shmget(ftok("A.c",12) , sizeof(int) , 0777);
	while(shmid3 < 0){
		printf("Waiting for B to start\n");
		shmid3 = shmget(ftok("A.c",12) , sizeof(int) , 0777);
		sleep(2);
	}

	shmid1=shmget(ftok("A.c",11),15*sizeof(int),0777|IPC_CREAT);
	buff=(int *)shmat(shmid1,NULL,0);
	

	semid[0] = semget(ftok("A.c",7), 1, 0777);
	semid[1] = semget(ftok("A.c",8), 1, 0777);
	semid[2] = semget(ftok("A.c",9), 1, 0777);
	semid[3]= semget(ftok("A.c",10), 1, 0777);

	
	signal_op.sem_num=wait_op.sem_num=0;
	wait_op.sem_op=-1;
	wait_op.sem_flg=signal_op.sem_flg=0;
	signal_op.sem_op=1;
	

	while(1){
		shmid3 = shmget(ftok("A.c",12) , sizeof(int) , 0777);
		if(shmid3<0)exit(0);
		random_sleep();
		n=random_number();
		producer(n);

	}

	return 0;
}