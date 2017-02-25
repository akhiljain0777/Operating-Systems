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

struct record{
	char first[25],last[25];
	int roll;
	float cgpa;
};

void search(struct record *records,int size){ // getting a record
	int rollno,i;
	printf("Enter roll no\n");
	scanf("%d",&rollno);
	for(i=0;i<size;i++){
		if(records[i].roll==rollno){
			printf("%s %s %d %.2f\n",records[i].first,records[i].last,records[i].roll,records[i].cgpa);
			break;
		}
	}
	if(i==size)printf("Invalid roll no\n");
}


void update(struct record *records,int size,int semid1,int *flag){
	// updating gpa for record
	int rollno,i;
	printf("Enter roll no\n");
	scanf("%d",&rollno);
	for(i=0;i<size;i++){
		if(records[i].roll==rollno){
			printf("Old CGPA=%.2f\nEnter new CGPA\n",records[i].cgpa);
			float tmp;
			scanf("%f",&tmp);
			P(semid1);    // critical section
			records[i].cgpa=tmp;
			*flag=1;
			V(semid1);
			printf("CGPA updated successfully\n");
			break;
		}
	}
	if(i==size)printf("Invalid roll no\n");

			
}
int *flag;
struct record *records;

void my_handler(){
	shmdt(flag);
	shmdt(records);
	exit(0);
}

int main(){
	int shmid,i,shmid2,semid1,shmid3;
	key_t key=1101,key2=1100,key3=3700;
	signal(SIGINT,my_handler); // ctrl+c will first detach shared memory
	shmid3 = shmget(key3 , sizeof(int) , 0666);
	while(shmid3 < 0){
		printf("Waiting for x to start\n");
		shmid3 = shmget(key3 , sizeof(int) , 0666);
		sleep(2);
	}

	shmid=shmget(key,110*sizeof(struct record),0777|IPC_CREAT);
	records=(struct record *)shmat(shmid,NULL,0);
	shmid2=shmget(key2,2*sizeof(int),0777|IPC_CREAT);
	flag=(int *)shmat(shmid2,NULL,0);
	semid1 = semget(key3, 1, 0777|IPC_CREAT);
	signal_op.sem_num=wait_op.sem_num=0;
	wait_op.sem_op=-1;
	wait_op.sem_flg=signal_op.sem_num=0;
	signal_op.sem_op=1;
	

	while(1){
		printf("1.Search for a student record\n");
		printf("2.Update CGPA of a student\n");
		printf("3.Exit\n");
		int n;
		scanf("%d",&n);
		if(n==1)search(records,flag[1]);
		else if(n==2){update(records,flag[1],semid1,&flag[0]);}
		else if(n==3)break;
		else printf("Invalid input\nPlease try again\n");
	}
	shmdt(flag);
	shmdt(records);

	return 0;
}