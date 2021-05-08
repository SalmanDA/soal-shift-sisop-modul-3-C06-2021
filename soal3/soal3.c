#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <sys/stat.h>
#include <ctype.h>
#include<dirent.h>
#include<unistd.h>

char destination[1001];

void* mover(char* argc){
	char* source=(char*) argc;
	
	//moving the files
	char item[1001];
	strcpy(item,source);
	
	char *new, *old;
	char a='/', b='.';
	old = strrchr(item,a);
	new = strrchr(item,b);
	char extension[100];
	
	if(old){
		//check if file exist
		if(checkFile(item)){
		checkExtension(old,extension);
		}
		else return 0;
	}
	//make new directory for each file type
	mkdir(extension,0777);
	
	char path[101];
	strcpy(path,(char*) argc);
	
	char newDir[101];
	getcwd(newDir,sizeof(newDir));
	strcat(newDir,"/"); strcat(newDir,"extension");strcat(newDir,"/");strcat(newDir,old+1);
	printf("%s\n%s\n",path,newDir);
	rename(path,newDir);
	return(void *) 1;//to move the soure
	pthread_exit(0);
	
}

int main(int argc, char* argv[]){
	
	char source[1001];
	getcwd(destinaiton,sizeof(destination));
	strcpy(source,destionation);
	

	pthread_t tid[argc-2];
	if(strcmp(argv[1],"-f")==0){
		int x=0;
		for(int i=2;i<argc;i++){
		pthread_create(&tid[x],NULL,mover,(void*)argv[i]);
		x++;
		}
		for(int i=0;i<x;i++){
		void *ptr;
		pthread_join(tid[i],&ptr);
		
		if(((int)ptr)==1){
			printf("File %d : Berhasil dikategorikan \n", i+1);
		}
		else{
			printf("File %d : Sad, gagal :( \n",i+1);
		}
		}
		return 0;
		
	}
	else if(strcmp(argv[1],"-d")==0){
	
	}
	else if(strcmp(argv[1],"*")==0){

	}
	
	
}

