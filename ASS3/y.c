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

struct record{
	char first[25],last[25];
	int roll;
	float cgpa;
};


int main(){
	int shmid,i;
	struct record *records;
	key_t key=1098;
	shmid=shmget(key,110*sizeof(struct record),0777|IPC_CREAT);
	records=(struct record *)shmat(shmid,NULL,0);
	printf("%s  %s %d %f\n",records[0].first,records[0].last,records[0].roll,records[0].cgpa);
	return 0;
}