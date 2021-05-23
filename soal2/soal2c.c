#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

int pid;
int fd1[2];
int fd2[2];

void main() {
	pid_t child_id;

  	if (pipe(fd1)==-1) 
	{ 
		exit(1); 
	} 

  	child_id = fork();
  	if (child_id < 0)
    	{
        	exit(EXIT_FAILURE);
    	} else 
	if (child_id == 0) {
  		dup2(fd1[1], 1);
 
  		close(fd1[0]);
  		close(fd1[1]);
  		
  		char *arg[] = {"ps", "aux", NULL};
  		execv("/bin/ps", arg);
  		
  		_exit(1);
  	} else {
  		if (pipe(fd2) == -1) {
			exit(1); 
  		}
	
		child_id = fork();
  		if (child_id < 0)
   	 	{
        		exit(EXIT_FAILURE);
    		} else 
		if (child_id == 0) {
  			dup2(fd1[0], 0);
		  	dup2(fd2[1], 1);

		  	close(fd1[0]);
		  	close(fd1[1]);
		  	close(fd2[0]);
		  	close(fd2[1]);

		  	char *arg[] = {"sort", "-nrk", "3.3",NULL};
		  	execv("/bin/sort", arg);

		  	_exit(1);
  		} else {
  			close(fd1[0]);
  			close(fd1[1]);

		  	dup2(fd2[0], 0);

		  	close(fd2[0]);
		  	close(fd2[1]);

		  	char *arg[] = {"head","-5",NULL}; 
		  	execv("/bin/head",arg);
		  
		  	_exit(1);	
		}
	}
}
