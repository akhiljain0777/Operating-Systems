#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define MX 100


int main(){
	int id;
	id=fork();
	if(id==0){
		char *argv[2];
		argv[0]=(char *)malloc(MX*sizeof(char));
		strcpy(argv[0],"./shell");
		argv[1]=(char *)NULL;
		execvp(argv[0],argv);
		perror("execvp not working");
	}
	else wait(NULL);

	return 0;
}