#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#define mx 1000

void swap(int *x,int *y){
	int z=*x;
	*x=*y;
	*y=z;
}

int main(int argc,char*argv[]){
	FILE *fp;
	int A[mx],i=0,j,n;
	fp=fopen(argv[1],"r");
	while(fscanf(fp,"%d",&A[i])!=EOF)i++; // Scanning integers from the file
	n=i; // n stores number of elements 
	for(i=n-1;i>=0;i--){ /*Bubble sort*/
		for(j=1;j<=i;j++){
			if(A[j]<A[j-1])swap(&A[j-1],&A[j]);
		}
	}
	for(i=0;i<n;i++)printf("%d ",A[i]);
	printf("\n");
	fclose(fp);
	return 0;	
}