#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/types.h>

#define BUFSIZE 100

int main(int argc, char* argv[]){
	if(argc!=3){
		printf("Wrong Format.\nPlease try again!!\n");
		return 0;
	}
	int fd[2],id,filedes1,filedes2,ret[2];
	char buff[200];
	pipe(fd);
	pipe(ret);
	filedes1=open(argv[1],O_RDONLY);
	if(filedes1<0){
		perror("Incorrect File1 name\n");
		return 0;
	}
	filedes2=open(argv[2],O_WRONLY | O_CREAT,S_IRUSR | S_IWUSR);
	if(filedes2<0){
		perror("Incorrect File2 name\n");
		return 0;
	}	
	id=fork();
	int tmp;
	char a[10];
	if(id!=0){
		close(fd[0]);
		while(tmp=read(filedes1,buff,BUFSIZE)){
			write(fd[1],buff,tmp);
			printf("Parent has written to the pipe ... waiting for Acknowledgement \n");
			read(ret[0],a,2);
			if(a[0]=='-' && a[1]=='1')exit(-1);
			if(a[0]=='0' && tmp<100)exit(1);
		}

	}
	else{
		close(fd[1]);
		while(tmp=read(fd[0],buff,BUFSIZE)){
			write(filedes2,buff,tmp);
			printf("Child has appended to the file\n");
			if(tmp<0){
				write(ret[1],"-1",2);
				perror("Error in copying\n");
				exit(-1);	
			}
			else if(tmp<100){
				write(ret[1],"0",2);
				printf("File copied successfully\n");
				exit(1);
			}
			else write(ret[1],"0",2);
		}
	}

	return 0;
} 