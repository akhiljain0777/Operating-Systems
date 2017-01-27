#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/stat.h>
#include <dirent.h>
#define mx 100

char currD[500],in[500],path[mx];


void shell(){
	printf("%s>",getcwd(currD,100));
	while(1){
		scanf("%s",in);
		if(!strcmp(in,"cd")){
			scanf("%s",path);
			int status=chdir(path);
			if(status==-1)perror("chdir failed");

		}
		else if(!strcmp(in,"pwd")){
			printf("%s\n",getcwd(currD,100));
		}
		else if(!strcmp(in,"mkdir")){
			scanf("%s",path);
			int status=mkdir(path,S_IRWXU | S_IRWXG | S_IRWXO);
			if(status==-1)perror("mkdir failed");
		}
		else if(!strcmp(in,"rmdir")){
			scanf("%s",path);
			int status=rmdir(path);
			if(status==-1)perror("rmdir failed");
		}
		else if(!strcmp(in,"ls")){
			DIR *dp;
			struct dirent *sd; 
			dp=opendir(currD);
			while((sd=readdir(dp))!=NULL){
				if(strcmp(sd->d_name,".") && strcmp(sd->d_name,"..") )printf("%s\n",sd->d_name);
			}
			closedir(dp);
		}
		else if(!strcmp(in,"cp")){

		}
		else if(!strcmp(in,"exit")){
			exit(0);
		}

		printf("%s>",getcwd(currD,100));	
	}
}



int main(){
	shell();
}