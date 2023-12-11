#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char* argv[] ){
	char command_string[MAX_SIZE];
	char* command[MAX_N_ARGS];
	int pid,status;
	char* buf; //storage buffer
	struct timespec t1,t2; //clock_gettime structure to store time
	write(STDOUT_FILENO,START,strlen(START)); //writing first prompt
	while(strcmp(command_string,EXIT_CODE)!=0){
		read(STDIN_FILENO,command_string,MAX_SIZE);	    	//reading command
		command_string[strlen(command_string)-1]='\0';  	//We need to remove the last character
		if(clock_gettime(CLOCK_MONOTONIC,&t1) == -1){perror("clock_gettime");exit(EXIT_FAILURE);}
		pid=fork();					
		if(pid !=0){
			wait(&status);
			if(clock_gettime(CLOCK_MONOTONIC,&t2) == -1){perror("clock_gettime");exit(EXIT_FAILURE);}
			if(WIFEXITED(status)){
				//Writing prompt with exit information & time delay
				sprintf(buf,"[exit:%d|%ldms]",WIFEXITED(status),(t2.tv_nsec-t1.tv_nsec)/1000000);
				write(STDOUT_FILENO,PROMPT_BEGINNING,strlen(PROMPT_BEGINNING));
				write(STDOUT_FILENO,buf,strlen(buf));
				write(STDOUT_FILENO,PROMPT_END,strlen(PROMPT_END));
			} else if (WIFSIGNALED(status)){
				//Writing prompt with signal information & time delay
				sprintf(buf,"[exit:%d|%ldms]",WTERMSIG(status),(t2.tv_nsec-t1.tv_nsec)/1000000);
				write(STDOUT_FILENO,PROMPT_BEGINNING,strlen(PROMPT_BEGINNING));
				write(STDOUT_FILENO,buf,strlen(buf));
				write(STDOUT_FILENO,PROMPT_END,strlen(PROMPT_END));
			}
			if(strcmp(command_string,EXIT_CODE)==0){
				write(STDOUT_FILENO,EXIT_MESSAGE,strlen(EXIT_MESSAGE)); //Exit message in case of exit
				return EXIT_SUCCESS;
			}
		}
		else if(pid==0){
			parse_command(command_string,command);
			execlp(command[0],command);	//executing command (no arguments)
			return EXIT_SUCCESS;
		}
	}
}

void parse_command(char* command_string,char** command){
	char *delim=" ";
	unsigned int count=0;
	//command + options + NULL = MAX_N_ARGS+2 elements
	char* tokens[MAX_N_ARGS+2];
	char* token=strtok(command_string,delim);
	tokens[0]=token;
	count++;
	while(token != NULL && count<MAX_N_ARGS+1){
		token=strtok(NULL,delim);
		tokens[count]=token;
		count++;
	}
	tokens[count]=(char*) NULL;
	execvp(tokens[0],tokens);
	command=tokens;
}
