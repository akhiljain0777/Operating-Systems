#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <sys/wait.h>
#include <sys/sem.h>
#include <string.h>
#include <signal.h>


#define P(s) semop(s, &wait_op, 1) 
#define V(s) semop(s, &signal_op, 1)  
struct sembuf wait_op,signal_op;

struct record{
	char first[25],last[25];
	int roll;
	float cgpa;
};

void update(char *filename,struct record *records,int size){
	int i;
	FILE *file=fopen(filename,"w");
	for(i=0;i<size;i++){
		fprintf(file, "%s %s %d %0.2f\n",records[i].first,records[i].last,records[i].roll,records[i].cgpa );
	}
	fclose(file);
}

int shmid,shmid2,i=0,shmid3;
int semid1,*flag;
struct record *records;
key_t key=1101,key2=1100,key3=3700;	

void my_handler(){
	shmdt(flag);
	shmdt(records);
	semctl(semid1, 0, IPC_RMID, 0);
	shmctl(shmid, IPC_RMID, 0);
	shmctl(shmid2, IPC_RMID, 0);
	shmctl(shmid3, IPC_RMID, 0);
	exit(0);
}


int main(int argc,char *argv[]){
	if(argc!=2){
		printf("Invalid input\n" );
		return 0;
	}
	signal(SIGINT, my_handler); // ctrl+c will detach and delete shared memory
	
	shmid=shmget(key,110*sizeof(struct record),0777|IPC_CREAT);
	records=(struct record *)shmat(shmid,NULL,0);
	shmid2=shmget(key2,2*sizeof(int),0777|IPC_CREAT);
	flag=(int *)shmat(shmid2,NULL,0);
	semid1 = semget(key3, 1, 0777|IPC_CREAT);
	semctl(semid1,0,SETVAL,1);
	FILE *file=fopen(argv[1],"r");
	if(file==NULL){printf("File can not opened");return 0;}
	signal_op.sem_num=wait_op.sem_num=0;
	wait_op.sem_op=-1;
	wait_op.sem_flg=signal_op.sem_num=0;
	signal_op.sem_op=1;
	i=0;
	while(fscanf(file,"%s %s %d %f",records[i].first,records[i].last,&records[i].roll,&records[i].cgpa)!=EOF)i++;
	flag[1]=i;
	flag[0]=0;
	//TODO
	shmid3 = shmget(key3,sizeof(int),0777|IPC_CREAT);
	while(1){
		sleep(5);
		P(semid1); // critical section
		if(flag[0]){update(argv[1],records,flag[1]);}
		flag[0]=0;
		V(semid1);
	}
	return 0;
}
