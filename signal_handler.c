#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

// CS481 Assignment 2
//
// Adam Spanswick
//
//This program starts a child process using fork() that uses execvp
//to call the yes command. There are two signals handlers one for
//Ctrl-z and one for Ctrl-c. When the Ctrl-z signal is encountered
//the parent process either pauses the child process if it is running
//or resume's the child process if it is paused. When the Ctrl-c signal
//is encountered the parent process kills the child process and the exits 
//itself.

pid_t child_pid; //Process ID of the child.
int paused = 0; //Default not paused. 
int stat; //Status of child process.

//Ctrl-c Signal Handler:
//   Kills the child process and exits the main process.
void handle_sigint1(int sig_int) {
	printf("\nTerminating program Ctrl-c caught.\n");
	kill(child_pid, sig_int);

	if(WIFEXITED(stat)) {
		printf("Exit Status of child: %d\n", WEXITSTATUS(stat));
	}

	exit(0);
}

//Ctrl-z Signal Handler:
//   Toggles the child yes to stop the child process if it is
//   running, or resume the child process if it is stopped. Keeps
//   the main program running.
void handle_sigint2(int sig_tstp) {
	printf("\nCtrl-z caught.\n");

	if(!paused) {
		paused = 1;
		printf("Pausing the child process: %d\n", child_pid);
		kill(child_pid, sig_tstp);
	} else if (paused) {
		paused = 0;
		printf("Resuming the child process: %d\n", child_pid);
		kill(child_pid, SIGCONT);
	}
}

int main (int argc, char **argv) {
	child_pid = fork();
	int parent_pid = getpid();
	char *const args[] = {"yes", "Running yes command.", NULL};
	char *const env[] = {NULL};

	if(child_pid == 0){
		//Fork Successful.
		printf("About to run yes.\n");
		execvp("yes", args);
	} else if (child_pid < 0) {
		//Fork failed.
		printf("fork() failed.\n");
		exit(0);
	} else {
		//Parent process code, fork successful.
		signal(SIGINT, handle_sigint1);
		signal(SIGTSTP, handle_sigint2);
		wait(&stat);
	}

	while(1){}

		return 0;
}