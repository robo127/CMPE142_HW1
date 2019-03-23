#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<fcntl.h>
#include<errno.h>
#include<sys/types.h> 
#include<sys/wait.h> 
#include<readline/readline.h> 
#include<readline/history.h> 
  
#define MAXCOM 1000 // max number of letters to be supported 
#define MAXLIST 100 // max number of commands to be supported 
  
 //use gcc -o wish wish.c -lreadline at end of compile line
// Clearing the shell using escape sequences 
#define clear() printf("\033[H\033[J") 

// Greeting shell during startup 
void init_shell() 
{ 
    clear(); 
    printf("Robin's Shell\n");
    sleep(1); 
    clear(); 
} 
  
// Function to take input 
int takeInput(char* str) 
{ 
	char* buf; 
	ssize_t bufsize = 0;
	buf = readline("\n>>> "); 
	if (strlen(buf) != 0) { 
		strcpy(str, buf); 
		return 0; 
	} else { 
		return 1; 
	} 
} 

// Function to print Current Directory. 
void printDir() 
{ 
    char cwd[1024]; 
    getcwd(cwd, sizeof(cwd)); 
    printf("\nDir: %s", cwd); 
} 


  
// Function to execute builtin commands 
int commandLineHandler(char** parsed) 
{ 
	int commandAmount = 3;
	int i;
	int builtinCommand = 0; 
	char* ListOfBuiltinCmds[commandAmount]; 
	char* username; 
	if(access("/bin/ls", X_OK) == -1 && access("/usr/bin/ls", X_OK) == -1)
   	{
		char error_message[30] = "An error has occurred\n";
    		write(STDERR_FILENO, error_message, strlen(error_message));
    	}
    	ListOfBuiltinCmds[0] = "exit"; 
    	ListOfBuiltinCmds[1] = "cd";
	ListOfBuiltinCmds[2] = "path";
  
	for (i = 0; i < commandAmount; i++) { 
		if (strcmp(parsed[0], ListOfBuiltinCmds[i]) == 0) 
		{ 
			builtinCommand = i + 1; 
           		break; 
        	} 
	} 
  
	switch (builtinCommand) { 
	case 1: 
		if(parsed[1] != NULL)
		{
			char error_message[30] = "An error has occurred\n";    				write(STDERR_FILENO, error_message, strlen(error_message));		
		}
		else
		{
      			printf("\nExiting\n"); 
    		    	exit(0); 
		}
    	case 2: 
		if (parsed[1] == NULL || parsed[2] != NULL)
		{
			char error_message[30] = "An error has occurred\n";
  			write(STDERR_FILENO, error_message, strlen(error_message));
		}
		else
		{
      		  	chdir(parsed[1]); 
		}
		return 1;
	case 3:	
		;
		int paths = 0;
		int clearance;
		char *pathway = "PATH=";
		char *space = ":";
		char *new;
		while(parsed[paths+1] != NULL)
		{
			if(parsed[paths+1] != NULL)
			{
				paths++;
			}
			
		}
		if(paths > 0)
		{
			new = malloc (sizeof(char)*MAXCOM);
			strcat(new,pathway);
			for(i = 0; i<paths; i++)
			{
				strcat(new, parsed[i+1]);
				if(parsed[i+2] == NULL)
				{
					break;
				}
				else
				{				
					strcat(new, space); 
				}
			}
			clearance = putenv(new);
		}
		else
		{
			clearance = putenv(pathway);
		}
		if(clearance != 0)
		{
			char error_message[30] = "An error has occurred\n";
  			write(STDERR_FILENO, error_message, strlen(error_message));
		}
	printf("test\n");
	const char* s = getenv("PATH");
	printf("PATH :%s\n",(s!=NULL)? s : "getenv returned NULL");
	printf("end test\n");
	        return 1; 
    	default: 
    		break; 
    	} 
    	return 0; 
} 
  
// function for parsing command words 
void seperateSpace(char* str, char** parsed) 
{ 
    int i; 
    for (i = 0; i < MAXLIST; i++) { 
        parsed[i] = strsep(&str, " "); 
        if (parsed[i] == NULL) 
            break; 
        if (strlen(parsed[i]) == 0) 
            i--; 
    } 
} 
  
int processString(char* str, char** parsed) 
{ 
    seperateSpace(str, parsed); 
    if (commandLineHandler(parsed)) 
        return 0; 
    else
        return 1 ; 
} 

void execArgs(char** parsed) 
{ 

	//ls & ls > c.txt & echo lol
	int holderForNextCMD = 0;
	
	int originalSize = sizeof(parsed)/sizeof(parsed[0]);

	int numberOfCMD = 1;
	for(int i = 0; i < originalSize ; i++)
	{
		if(parsed[i+1] != NULL)
		{			
			if(!strcmp(parsed[i+1], "&"))
			{
				numberOfCMD++;
			}
		}
	}	
	printf("%i commands\n",numberOfCMD);
		int newSize = 0;
	//Loop in case of parallel processing
	for(int i = 0; i < numberOfCMD; i++)
	{
		printf("%i round", i);
		if(i+1==numberOfCMD)
		{
			printf("dfsfdsfround\n");
		}
		newSize = 0;
		//get next size of string of CMD ARG
		for(int k = holderForNextCMD; k < originalSize; k++)
		{
			newSize++;
			if(parsed[k+1] == NULL)
			{
				break;
			}
			else
			{
				if(!strcmp(parsed[k+1], "&"))
				{
					break;
				}
			}
		} 
		printf("next round %i\n", i);
		char** temp = calloc(newSize,sizeof(char*));
		printf("next2 round %i\n", i);
		//set cmd for string to exec
		temp[0]=parsed[holderForNextCMD];
		int placeholder = holderForNextCMD;
		for(int t = 1; t < newSize; t++)
		{
			for(int z = holderForNextCMD; z < originalSize; z++)
			{
				if(!strcmp(parsed[z+1], ">"))
				{
					placeholder++;
				}
				else
				{
					break;
				}
			}
			temp[t] = parsed[placeholder];
		}
		printf("next3\n");
		//printf("%li dab dab yea\n", newSize/sizeof(int));

		//get new position of commands, update holderForNextCMD
		for(int j = holderForNextCMD; j < originalSize; j++)
		{
			if(parsed[j+1] == NULL)
			{
				holderForNextCMD = j;
				break;
			}
			if(!strcmp(parsed[j+1], ">") || !strcmp(parsed[j+1], "&"))
			{
				holderForNextCMD = j;
				break;
			}

		}
		printf("next4 round \n");
		printf("%i\n",holderForNextCMD);
		pid_t pid = fork();
		if (pid == -1) 
		{ 
			char error_message[30] = "An error has occurred\n";
  			write(STDERR_FILENO, error_message, strlen(error_message));
        		return; 
		} 
		else if (pid == 0) 
		{ 
			int place1 = 1;
			int place2 = 2;
			if(parsed[holderForNextCMD+1] == NULL)
			{
				
				if (execvp(temp[0], temp) < 0) 
				{ 
       	     				char error_message[30] = "An error has occurred\n";
  					write(STDERR_FILENO, error_message, strlen(error_message));
				}
			}
			else if(!strcmp(parsed[holderForNextCMD+1], ">"))
			{
				place2 = open(parsed[holderForNextCMD+2], O_WRONLY|O_CREAT|O_TRUNC, 0666);
				dup2(place2, STDOUT_FILENO);
				if(parsed[holderForNextCMD+2]==NULL || !strcmp(parsed[holderForNextCMD+2], ">"))
				{
					char error_message[30] = "An error has occurred\n";
  					write(STDERR_FILENO, error_message, strlen(error_message));
				}
				else
				{
					if(execvp(temp[0], temp)<0)
					{
					char error_message[30] = "An error has occurred\n";
  					write(STDERR_FILENO, error_message, strlen(error_message));
					}
				}
				close(place2);
			}
			else
			{
				if(execvp(temp[0], temp)<0)
				{
					char error_message[30] = "An error has occurred\n";
  					write(STDERR_FILENO, error_message, strlen(error_message));
				}
			}
        		exit(0); 
		} 
		else 
		{ 
        		wait(NULL);  

		}
	}
	        		return; 
	
		
} 
  
int main() 
{ 
	char inputString[MAXCOM]; 
	char *parsedArgs[MAXLIST];
	setbuf(stdout, NULL);
	int execFlag = 0;
	int start = 0;
	//set inital PAth to /bin as instructed
	static char *var = "PATH=/bin";
	//start = putenv(var);
        init_shell(); 
	printf("test\n");
	const char* s = getenv("PATH");
	printf("PATH :%s\n",(s!=NULL)? s : "getenv returned NULL");
	printf("end test\n");
  	if(start == 0)
	{
		
		while (1) 
		{
			// print shell line 
			printDir(); 
			// take input 
			if(takeInput(inputString))
			{
				continue;
			}
			// process 
			execFlag = processString(inputString, parsedArgs); 
			// execflag returns zero if there is no command 
			// or it is a builtin command, 
			// 1 if it is a simple command 
  
       			  // execute 
			if (execFlag == 1)
			{
				execArgs(parsedArgs); 
			}
		} 
		return 0; 
	}

	else
	{
		return 0;
	}
	

	
} 

