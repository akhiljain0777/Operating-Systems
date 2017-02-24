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

void search(struct record *records,int size){
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


void update(struct record *records,int size,int semid1){
	P(semid1);
	int rollno,i;
	printf("Enter roll no\n");
	scanf("%d",&rollno);
	for(i=0;i<size;i++){
		if(records[i].roll==rollno){
			printf("Old CGPA=%.2f\nEnter new CGPA\n",records[i].cgpa);
			scanf("%f",&records[i].cgpa);
			printf("CGPA updated successfully\n");
			break;
		}
	}
	if(i==size)printf("Invalid roll no\n");
	V(semid1);
}

int main(){
	int shmid,i,shmid2,semid1,*flag;
	struct record *records;
	key_t key=1101,key2=1100,key3=1102;
	shmid=shmget(key,110*sizeof(struct record),0777|IPC_CREAT);
	records=(struct record *)shmat(shmid,NULL,0);
	shmid2=shmget(key2,2*sizeof(int),0777|IPC_CREAT);
	flag=(int *)shmat(shmid2,NULL,0);
	semid1 = semget(key3, 1, 0777|IPC_CREAT);
	while(1){
		printf("1.Search for a student record\n");
		printf("2.Update CGPA of a student\n");
		printf("3.Exit\n");
		int n;
		scanf("%d",&n);
		if(n==1)search(records,flag[1]);
		else if(n==2){update(records,flag[1],semid1);flag[0]=1;}
		else if(n==3)break;
		else printf("Invalid input\nPlease try again\n");
	}
	return 0;
}