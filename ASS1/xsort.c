#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#define mx 100

int main(int argc,char *argv[]){
	char *parameters[3];
	parameters[0]=(char *)malloc(100*sizeof(char));
	parameters[1]=(char *)malloc(100*sizeof(char));
	strcpy(parameters[0],"./sort1");
	strcpy(parameters[1],argv[1]);
	parameters[2]=NULL;
	execlp("/usr/bin/xterm","/usr/bin/xterm","-hold","-e",parameters[0],parameters[1],(char*)NULL);
	perror("exec failed");
	return 0;
}