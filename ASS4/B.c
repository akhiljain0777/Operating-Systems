#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <sys/wait.h>
#include <sys/sem.h>

#define BUFF_SIZE 10
#define MAX_Threads 10
#define UNBLOCK_Threads 5
#define NUM_THREADS 100000

#define P(s) semop(s, &wait_op, 1) 
#define V(s) semop(s, &signal_op, 1)  

pthread_t tid[NUM_THREADS];

int flag,tmp;
struct sembuf wait_op,signal_op;
int *buff,semid[4];
int shmid1,id,shmid2,shmid3;
int Ticket=100;
int global_cnt=0;
pthread_mutex_t mutex; 
pthread_cond_t cond;

struct val{
	int x;
};


void random_sleep(){
	int tme;
	tme=rand()%3;
	sleep(tme);
}

int min(int x,int y){
	return x>y ? x:y;
}


int consumer(){
	P(semid[0]);		// consumer critical section
	P(semid[3]);		//if buffer is empty 		
	int tmp=buff[buff[11]];
	buff[11]=(buff[11]+1)%BUFF_SIZE;	
	V(semid[0]);
	V(semid[2]);
	printf("B reads %d\n",tmp);
	return tmp;
}

void my_handler(){ // Signal handler 
	shmctl(shmid3, IPC_RMID, 0);
	shmdt(buff);
	semctl(semid[0], 0, IPC_RMID, 0);
	semctl(semid[1], 0, IPC_RMID, 0);
	semctl(semid[2], 0, IPC_RMID, 0);
	semctl(semid[3], 0, IPC_RMID, 0);
	shmctl(shmid1, IPC_RMID, 0);
	shmctl(shmid2, IPC_RMID, 0);
	exit(0);
}


void* book_ticket(void* p){
	int x=*((int *)p);
	pthread_mutex_lock(&mutex);
	/* Block threads which no. of threads started and not finished
	are greater than max threads allowed */
	if(global_cnt>=MAX_Threads){ 
		flag=1;
		while(global_cnt<=UNBLOCK_Threads){
			pthread_cond_wait(&cond, &mutex);
		}
	}
	global_cnt++; // For no. of threads started and not finished
	if(Ticket-x>=0){
		Ticket=min(Ticket-x,100);
		printf("Request fulfilled for x=%d\n",x);
		pthread_mutex_unlock(&mutex);
		tmp=1;
		random_sleep();
		global_cnt--;
		return ((void *)(&tmp)); //return 1
	}
	else{
		printf("Request cannot be fulfilled for x=%d\n",x);
		pthread_mutex_unlock(&mutex);
		random_sleep();
		tmp=0;
		global_cnt--;
		return ((void *)(&tmp)); //return 0
	}
}




int main(){
	signal(SIGINT, my_handler); // ctrl+c will detach and delete shared memory
	struct val params[100000];
	shmid1=shmget(ftok("A.c",11),15*sizeof(int),0777|IPC_CREAT);
	buff=(int *)shmat(shmid1,NULL,0);
	buff[10]=0;  //in 
	buff[11]=0;   // out 
	
	signal_op.sem_num=wait_op.sem_num=0;
	wait_op.sem_op=-1;
	wait_op.sem_flg=signal_op.sem_flg=0;
	signal_op.sem_op=1;
	
	semid[0] = semget(ftok("A.c",7), 1, 0777|IPC_CREAT);
	semid[1] = semget(ftok("A.c",8), 1, 0777|IPC_CREAT);
	semid[2] = semget(ftok("A.c",9), 1, 0777|IPC_CREAT);
	semid[3]= semget(ftok("A.c",10), 1, 0777|IPC_CREAT);
	
	semctl(semid[0],0,SETVAL,1);
	semctl(semid[1],0,SETVAL,1);
	semctl(semid[2],0,SETVAL,BUFF_SIZE);
	semctl(semid[3],0,SETVAL,0);

	
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	/*A should start after B*/
	shmid3 = shmget(ftok("A.c",12) , sizeof(int) , 0777 | IPC_CREAT);
	
	int i=0;
	while(1){
		params[i].x=consumer(); //Get the value requested
		pthread_create(&tid[i], NULL, book_ticket, (void *) &params[i]); /*Creating the thread*/
		pthread_mutex_lock(&mutex);
		if(flag && global_cnt<=UNBLOCK_Threads){  // Unblock the process blocked due to excessive server usage 
			flag=0;
			pthread_cond_broadcast(&cond);
		}
		pthread_mutex_unlock(&mutex);
		i++;
	}


	return 0;
}