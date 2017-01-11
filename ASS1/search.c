#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define mx 100

int main(){
	char f[mx];
	FILE *fp;
	int i=0,n,A[mx],id,tmp,status1=0,status2=0,start,finish,k;
	printf("Enter a file name\n");
	scanf("%s",f);
	fp=fopen(f,"r");
	while(fscanf(fp,"%d",&A[i])!=EOF)i++;
	n=i;
	printf("\nEnter k\n");
	scanf("%d",&k);
	fclose(fp);
	tmp=n;
	for(i=0;i<n;i++)printf("%d ",A[i]);
	printf("\n");
	start=0;
	finish=n;
	int flag;
	while(n>10){
		flag=0;
		id=fork();
		if(id==0){
			if(status2 || status1)exit(1);
			finish-=(n-n/2);
			n=n/2;
		}
		else{
			wait(&status1);
			id=fork();
			if(id==0){
				if(status1 || status2)exit(1);
				start+=n/2;
				n-=n/2;
			}
			else {wait(&status2);flag=1;}
		}
		if(tmp==n )break;
		if(status1 || status2)exit(1);
		if(flag)exit(0);
	//	printf("n=%d start=%d finish=%d pid=%d\n",n,start,finish,getpid()); 
	}
	if(!status1 && !status2 && n<=10){
		for(i=start;i<finish;i++){
			if(A[i]==k){
				if(tmp!=n){exit(1);}
				status1=status2=1;
			}
		}
		if(tmp!=n)exit(0);
	}
	if(tmp==n){
		if(status1 || status2)printf("Found\n");
		else printf("Not Found\n");
	}
	return 0;
}
