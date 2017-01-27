#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <time.h>
#define mx 500

char currD[500],in[500],path[mx],tmp[mx];


void cd_(){
	char* tmp=strtok(NULL," ");
	if(tmp==NULL){
		printf("Name of Directory missing\n");
	}
	else{
		strcpy(path,tmp);
		int status=chdir(path);
		if(status==-1)perror("chdir failed");
	}
}

void pwd_(){
	printf("%s\n",getcwd(currD,100));
}

void mkdir_(){
	char* tmp=strtok(NULL," ");
	if(tmp==NULL){
		printf("Name of Directory missing\n");
	}
	else{
		strcpy(path,tmp);
		int status=mkdir(path,S_IRWXU | S_IRWXG | S_IRWXO);
		if(status==-1)perror("mkdir failed");
	}

}

void rmdir_(){
	char* tmp=strtok(NULL," ");
	if(tmp==NULL){
		printf("Name of Directory missing\n");
	}
	else{
		strcpy(path,tmp);
		int status=rmdir(path);
		if(status==-1)perror("rmdir failed");
	}	

}

void ls_(){
	char* tmp=strtok(NULL," ");
	if(tmp!=NULL){
		DIR *dp;
		struct dirent *sd; 
		dp=opendir(currD);
		while((sd=readdir(dp))!=NULL){
			if(!strcmp(sd->d_name,".") || !strcmp(sd->d_name,"..") )continue;
			struct stat fileStat;
			stat(sd->d_name,&fileStat);
			printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    		printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
    		printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
    		printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
    		printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
    		printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
    		printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
    		printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
    		printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
   			printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
   			printf("  %5lu",fileStat.st_nlink);
   			printf("  %5lu",fileStat.st_size);
   			char* tym=ctime(&fileStat.st_mtime);
   			tym[strlen(tym)-1]='\0';
   			printf("  %s",tym);
			printf("  %s\n",sd->d_name);
		}
		closedir(dp);

		
	}
	else{
		DIR *dp;
		struct dirent *sd; 
		dp=opendir(currD);
		while((sd=readdir(dp))!=NULL){
			if(strcmp(sd->d_name,".") && strcmp(sd->d_name,"..") )printf("%s\n",sd->d_name);
		}
		closedir(dp);
	}

}

void cp_(){
	char file1[mx],file2[mx];
	char* tmp=strtok(NULL," ");
	char* tmp2=strtok(NULL," ");
}

void shell(){
	while(1){
		printf("%s>",getcwd(currD,100));
		scanf (" %[^\n]",tmp); 
		strcpy(in,strtok(tmp," "));
		if(!strcmp(in,"cd"))cd_();
		else if(!strcmp(in,"pwd"))pwd_();
		else if(!strcmp(in,"mkdir"))mkdir_();
		else if(!strcmp(in,"rmdir"))rmdir_();
		else if(!strcmp(in,"ls"))ls_();
		else if(!strcmp(in,"cp"))cp_();
		else if(!strcmp(in,"exit"))exit(0);

	}
}



int main(){
	shell();
}