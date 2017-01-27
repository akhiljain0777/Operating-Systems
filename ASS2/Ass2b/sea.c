#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define mx 1000

int main(int argc,char *argv[]){
	char f[mx];
	FILE *fp;
	int i=0,n,A[mx],id,tmp,status1=0,status2=0,start,finish,k;
	fp=fopen(argv[1],"r");
	while(fscanf(fp,"%d",&A[i])!=EOF)i++; // Scanning integers from the file
	n=i; // n stores number of elements 
	fclose(fp);
	for(i=0;i<n;i++)printf("%d ",A[i]);
	printf("\n");		
	while(1){
		status1=status2=0;
		printf("\nEnter k\n");
		scanf("%d",&k);
		if(k<=0)return 0; // For k<=0 terminate the program
		tmp=n;
		start=0;   // start refers to the starting index of a partition for a child 
		finish=n; // (Finish-1) refers to the index of last element of a partition for a child
		int flag;
		while(n>10){
			flag=0;
			id=fork();
			if(id==0){ /* First Child */
				if(status2 || status1)exit(1); /* If already found*/
				finish-=(n-n/2);  /* Updating finish for 1st child */
				n=n/2; /* 1st partition*/
			}
			else{
				wait(&status1); /* Wait for first child to finish*/
				id=fork();
				if(id==0){  /* Second child*/
					if(status1 || status2)exit(1);/* If already found*/
					start+=n/2;/* Updating 2nd for 1st child */
					n-=n/2; /* 2nd partition*/
				}
				else {wait(&status2);flag=1;}/*if flag=1 then both childs are finished*/
			}
			if(tmp==n )break;/* If root process then break*/
			if(status1 || status2)exit(1);/*If element is found*/
			if(flag)exit(0); /*Element not found in both partitions */
		}
		if(!status1 && !status2 && n<=10){
			for(i=start;i<finish;i++){
				if(A[i]==k){
					if(tmp!=n){exit(1);} 
					status1=status2=1; /* If root process then don't exit*/
				}
			}
			if(tmp!=n)exit(0);
		}
		if(tmp==n){
			if(status1 || status2)printf("\nFound\n");
			else printf("\nNot Found\n");
		}
	}
	return 0;
}
