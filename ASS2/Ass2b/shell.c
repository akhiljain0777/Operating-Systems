#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <pwd.h>

#define mx 500
#define BUFSIZE 100

char currD[500],in[500],path[mx],tmp[mx];



void cd_(){		// Basic chdir built in
	char* tmp=strtok(NULL," ");
	if(tmp==NULL){	// If nothing specified return to home dir
		struct passwd *pw = getpwuid(getuid());
		const char *homedir = pw->pw_dir;
		int status=chdir(homedir);
		if(status==-1)perror("chdir failed");

	}
	else{
		strcpy(path,tmp);
		int status=chdir(path);
		if(status==-1)perror("chdir failed");
	}
}

void pwd_(){	// Basic pwd built in
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
	if(tmp!=NULL){	// for ls -l
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
	else{	//for ls
		DIR *dp;
		struct dirent *sd; 
		dp=opendir(currD);
		while((sd=readdir(dp))!=NULL){
			if(strcmp(sd->d_name,".") && strcmp(sd->d_name,"..") )printf("%s\n",sd->d_name);
		}
		closedir(dp);
	}

}



void cp_(){	// copy command
	char file1[mx],file2[mx];
	char* tmp=strtok(NULL," ");
	char* tmp2=strtok(NULL," ");
	if(tmp==NULL || tmp2==NULL){	// error handling
		printf("cp requires two arguments\n");
		return;
	}
	strcpy(file1,tmp);
	strcpy(file2,tmp2);
	struct stat filestat1,filestat2;
	stat(file1,&filestat1);
	stat(file2,&filestat2);
	if(filestat1.st_mtime > filestat2.st_mtime ){	/* Only copy if file1's modified time is greater */ 
		int filedes1=open(file1,O_RDONLY);
		if(filedes1<0){
			perror("Incorrect File1 name\n");
			return;
		}
		int filedes2=open(file2,O_TRUNC|O_WRONLY,S_IRUSR | S_IWUSR);
		if(filedes2<0){
			perror("Incorrect File2 name\n");
			return;
		}
		char buff[mx];
		int bytes;
		while((bytes=read(filedes1,buff,BUFSIZE))){	 /* Reading from file 1 and writing to file 2 */
			if(bytes<0)break;
			write(filedes2,buff,bytes);
			if(bytes<BUFSIZE)break;
		}

	}
	else{
		printf("File 2 was modified later than file1\n");
	}
	return;
}

void piping(char* argv[]){	// function for piping eg a.out| b.out
	int id,fd[2],fd2[2];
	char *out1,*out2,*out3;
	pipe(fd);
	pipe(fd2);
	out1=strtok(argv[0]," |");	// name of first executable
	out2=strtok(NULL," |");		// name of second executable
	out3=strtok(NULL," |");
	if(out2==NULL){
		printf("Incorrect format\n");
		return;
	}
	id=fork();
	if(id==0){
		close(1);
		close(fd[0]);
		dup(fd[1]);	// fd[1] to std out
		char *pms[2];
		pms[0]=(char *)malloc(sizeof(char)*mx);
		strcpy(pms[0],out1);
		pms[1]=NULL;
		execvp(out1,pms);
		perror("");
		exit(0);
	}
	else{
		wait(NULL);
		if(fork()==0){
			close(0);
			close(fd[1]);
			dup(fd[0]); // fd[0] to stdin
			if(out3!=NULL){
				close(1);
				close(fd2[0]);
				dup(fd2[1]);// fd2[1] to stdout
			}
			char *pms[2];
			pms[0]=(char *)malloc(sizeof(char)*mx);
			strcpy(pms[0],out2);
			pms[1]=NULL;
			execvp(out2,pms);
			perror("");
			exit(0);
		}
		else{
			wait(NULL);
			if(out3!=NULL){
				if(fork()==0){
					close(0);
					close(fd2[1]);
					dup(fd2[0]); // fd2[0] to stdin
					char *pms[2];
					pms[0]=(char *)malloc(sizeof(char)*mx);
					strcpy(pms[0],out3);
					pms[1]=NULL;
					execvp(out3,pms);
					perror("");
					exit(0);		
				}
				else wait(NULL);
			}
		}

	}
}


void execute_(char *argv[]){ /* runs executable files */
	int id,ifd,ofd;
	int i,j,k;
	char file1[mx],file2[mx];
	int flag1=0,flag2=0;
	char temp[mx],*temp2;
	strcpy(temp,argv[0]);
	temp2=strtok(temp,"|");
	if(temp2!=NULL && strcmp(temp2,argv[0])){ // if piping present 
		piping(argv);
		fflush(stdout);
		return;
	}
	id=fork();
	if(id==0){
		i=0;
		if(argv[0][strlen(argv[0])-1]=='&')argv[0][strlen(argv[0])-1]='\0';
		while(argv[0][i]!='\0'){
			if(argv[0][i]=='<'){	// extracting file 1 name
				flag1=1;
				argv[0][i]='\0';
				i++;
				while(argv[0][i]==' ' && argv[0][i]!='\0')i++;
					j=i;
					while(argv[0][j]!=' ' && argv[0][j]!='\0' && argv[0][j]!='>')j++;
						for(k=i;k<j;k++){
							file1[k-i]=argv[0][k];
						}
						file1[j-i]='\0';
						i=j-1;
			}
			if(argv[0][i]=='>'){
				flag2=1;
				argv[0][i]='\0';
				i++;
				while(argv[0][i]==' ' && argv[0][i]!='\0')i++;
					j=i;
					while(argv[0][j]!=' ' && argv[0][j]!='\0' && argv[0][j]!='<')j++;
					for(k=i;k<j;k++){
						file2[k-i]=argv[0][k];
					}
					file2[j-i]='\0';
					i=j-1;
				}
					i++;
			}
				if(flag1==1){ //
					ifd=open(file1, O_RDONLY);
					if(ifd<0){
						perror("");
					}
					else{
						close(0);
						dup(ifd);
						close(ifd);	
					}
				}
				if(flag2==1){
					ofd=open(file2,O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
					if(ofd<0){
						perror("");
					}
					else{
						close(1);
						dup(ofd);
						close(ofd);	
					}
				}
				i=0;
				while(argv[0][i]!=' ' && argv[0][i]!='<' && argv[0][i]!='>' && argv[0][i]!='\0')i++;
				char comm[mx];
				for(j=0;j<i;j++)comm[j]=argv[0][j];
				comm[i]='\0';
				char *params[mx];
				params[0]=strtok(argv[0]," ");
				i=1;
				while((params[i++]=strtok(NULL," "))!=NULL);
				execvp(comm,params);	// for execution
				perror("execlp failed");
				exit(0);
	}
	else if(argv[0][strlen(argv[0])-1]!='&') wait(NULL);
}


void shell(){
	char *argv[2];
	while(1){
		printf("%s>",getcwd(currD,100));
		scanf (" %[^\n]",tmp); 
		argv[0]=(char *)malloc(mx*sizeof(char));
		strcpy(argv[0],tmp);
		argv[1]=NULL;
		strcpy(in,strtok(tmp," "));
		if(!strcmp(in,"cd"))cd_();
		else if(!strcmp(in,"pwd"))pwd_();
		else if(!strcmp(in,"mkdir"))mkdir_();
		else if(!strcmp(in,"rmdir"))rmdir_();
		else if(!strcmp(in,"ls"))ls_();
		else if(!strcmp(in,"cp"))cp_();
		else if(!strcmp(in,"exit"))exit(0);
		else execute_(argv);
	}
}



int main(){
	shell();
}