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
	int i=0,n,A[mx];
	scanf("%s",f);
	fp=fopen(f,"r");
	while(fscanf(fp,"%d",&A[i])!=EOF)i++;
	n=i;
	for(i=0;i<n;i++)printf("%d ",A[i]);
	printf("\n");
	// fscanf(fp, "%s %s %s %d", str1, str2, str3, &year);
	return 0;
}
