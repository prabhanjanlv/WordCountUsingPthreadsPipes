#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#define BUFFER_SIZE 250
#define READ_END	0
#define WRITE_END	1

int main(int argc, char **argv)
{
	FILE *file_handle; //Declaring a variable which is a file handler
	file_handle = fopen(argv[1], "r");	//Opens the file which is passed arguments while executing pwordcount
	
	int charcount = 0;	//Variable to store Character count of the content in input file
	int wordcount = 0;	//Variable to store word count of the content in input file
	char ch;	//Variable to read each character at once from input file
	int i=0,j;	//Variables for iterations
		

	char write_msg[BUFFER_SIZE]; //Used in pipe to send string(content of file) from parent to child process
	char read_msg[BUFFER_SIZE]; //Used in pipe to receive string(content of file) from parent to child process
	char write_msg_child[BUFFER_SIZE]; //Used in pipe to send string(word count) from child to parent process
	char read_msg_child[BUFFER_SIZE];	//Used in pipe to receive string(word count) from child to parent process

	pid_t pid;	//pid variable declaration
	
	int fd[2];	//File Descriptor to access pipe for communication between parent and child
	int fd_read[2];	//File Descriptor to access pipe for communication between child and parent

	/* create the pipe responsible for communication from parent to child */
	if (pipe(fd) == -1) 
	{
		fprintf(stderr,"Pipe failed");
		return 1;
	}
	
	/* Create another pipe for communication from child to parent */
	if (pipe(fd_read) == -1) 
	{
		fprintf(stderr,"Pipe failed");
		return 1;
	}
	
	/* now fork a child process */
	pid = fork();

	/* To Check if Creation of Fork Failed */
	if (pid < 0) 
	{
		fprintf(stderr, "Fork failed");
		return 1;
	}
	
	/* parent process */
	if (pid > 0) 
	{  
		/* close the unused end(Read End) of the pipe while writing to pipe */
		close(fd[READ_END]);		
		
		printf("Process 1 is reading file %c%s%c now ...\n",'\"',argv[1],'\"');
		
		if ( argc == 2 ) /* argc should be 2 which means there is an input file passed in arguments while executing C Program */
		{
		if ( file_handle )	/* If the input file is opened */
		  {
			while ((ch=getc(file_handle)) != EOF) /* to copy character to ch and do it until end of file */
			{
			write_msg[i] = ch;	//Move the contents of ch to array write_msg one by one as the loop runs
				i++;
			}			
		  }
		else	/* If file has not opened */
		  {
         printf("Failed to open the file\n");
          }
		}		
		else	/* If while executing the Program, arguments do not contain an input file */
		{
		/* We print argv[0] assuming it is the program name */
        printf("Please enter a file name.\nUsage: ./pwordcount <file_name>\n");
		}
		
		/* write to the pipe that will be read at child process */
		write(fd[WRITE_END], write_msg, strlen(write_msg)+1);
		
		printf("Process 1 starts sending data to Process 2 ...\n");
		
		/* close the write end of the pipe */
		close(fd[WRITE_END]);
		
		/* close the unused end of the pipe which communicates from child to parent*/
		close(fd_read[WRITE_END]);		
		/* read from the pipe which originates from child process*/
		read(fd_read[READ_END], read_msg_child, BUFFER_SIZE);
		
		/* Print the word count in parent process */
		printf("Process 1: The total number of words is %s.\n",read_msg_child);
		/*close the read end of pipe */
		close(fd_read[READ_END]);
	return(0);
	}
	/* child process */
	else 
	{ 
		/* close the unused end of the pipe that communicates from Parent to child */
		close(fd[WRITE_END]);
		/* close the unused end of the pipe that communicates from child to parent */
		close(fd_read[READ_END]);
		
		/* read from the pipe that originates from Parent process */
		read(fd[READ_END], read_msg, BUFFER_SIZE);
		
		printf("Process 2 finishes receiving data from Process 1 ...\n");
		
		printf("Process 2 is counting words now ...\n");
		
			for(j=0;j<strlen(read_msg);j++)	//To iterate through the string that is copied in parent process from input file
			{
			//Increment character count if NOT new line or space
		    if (read_msg[j] != ' ' && read_msg[j] != '\n') 
				++charcount; 
		  
			 //Increment word count if new line or space character
			if (read_msg[j] == ' ' || read_msg[j] == '\n') 
				++wordcount;
			}
			if (charcount > 0) //To check if there is just one word 
				++wordcount;			

		/* close the read end of the pipe that originates from Parent process */
		close(fd[READ_END]);
		/*Convert the wordcount integer to string for it to be made to parse in the pipe */
		sprintf(write_msg_child, "%d", wordcount);
		
		printf("Process 2 is sending the result back to Process 1 ...\n");
		
		/* write to the pipe that will be read at Parent process */
		write(fd_read[WRITE_END], write_msg_child, strlen(write_msg_child)+1);
		/* close the write end of the pipe */
		close(fd_read[WRITE_END]);
	return(0);
	}
getchar();
return(0);
}