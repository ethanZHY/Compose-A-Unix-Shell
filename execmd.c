#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

void ExecuteCmd(struct Command *command){

	// back ground check
	int background = command -> background;
	int num_sub = command -> num_sub_commands;
	int num_pipes = num_sub - 1;

	// create N-1 pipes
	int fds[num_pipes][2];
	for (int i = 0; i < num_pipes; i++){
		int err = pipe(fds[i]);
		if (err == -1){
			perror("pipe err");
			exit(1);
		}
	}

// optional feature
	// if chdir detected
	if (strcmp(command -> sub_commands[0].argv[0], "cd") == 0){
		char *directory = command->sub_commands[0].argv[1];
		int dir = chdir (directory);
		if (dir < 0){
			fprintf(stderr, "%s: No such file or directory\n", command->sub_commands[0].argv[1]);
		}
		return;
	}

	
	int child[num_sub];
	int waitchild[num_sub];
	for (int k = 0; k < num_sub; k++){
			waitchild[k] = 0;
		}
	int parent = 0;
	// fork() N child process
	for (int i = 0; i < num_sub; i++){

		for (int k = 0; k < num_sub; k++){
			child[k] = 0;
		}
		child[i] = fork();
		waitchild[i] = child[i];
		if (child[i] < 0){
			perror("fork err");
			exit(1);
		}		
		else if (child[i] == 0){
			child[i] = getpid();
			break;
		}
		else{
			if (i == num_sub - 1){
				parent = 1;
				for (int k = 0; k < num_sub; k++){
					child[k] = 0;
				}
				for (int j = 0; j < num_pipes; j++){
					close(fds[j][0]);
					close(fds[j][1]);
				}
				if (background){
					printf("[%d]\n", waitchild[num_sub - 1]);
				} 
			}
		}
	} // fork() end
	
	for (int i = 0; i < num_sub; i++){
		if (child[i] > 0){

			// close pipes not being used
			//2 or more subcommand
			if (num_sub > 1){
				// first cmd
				if (i == 0){
					close(fds[0][0]);
					close(1);					
					dup(fds[0][1]);
					//fprintf(stderr,"fc close fds[%d][0]\n", i);
					for (int t = 0; t < num_pipes; t++){
						if (t != i){
							close(fds[t][0]);
							close(fds[t][1]);
						}
					}	
				}
				// last cmd
				else if (i == num_sub - 1){
					close(fds[i - 1][1]);
					close(0);
					dup(fds[i - 1][0]);
					//printf("lc close fds[%d][1]\n", i - 1);
					for (int t = 0; t < num_pipes; t++){
						if (t != i - 1){
							close(fds[t][0]);
							close(fds[t][1]);
						}
					}
				// middle cmd
				}else{
					close(0);
					dup(fds[i - 1][0]);	
					close(fds[i - 1][1]);
					printf("bt1 close fds[%d][1]\n", i - 1);

					close(1);
					dup(fds[i][1]);
					close(fds[i][0]);
					fprintf(stderr,"bt2 close fds[%d][0]\n", i);

					for (int t = 0; t < num_pipes; t++){
						if (t != i && t != i - 1){
							close(fds[t][0]);
							close(fds[t][1]);
						}
					}
				}			
			}
			// only 1 subcommand
			if (num_sub == 1){
				close(fds[0][0]);
				close(fds[0][1]);
			}

			// if input redirect
			if (command -> stdin_redirect != NULL){
				close(0);
				if(open(command -> stdin_redirect, O_RDONLY) == -1){
					fprintf(stderr,"%s: File not found.\n", command -> stdin_redirect);
					exit(1);
				}
			}

			// if output redirect
			if (command -> stdout_redirect != NULL){
				close(1);
				if(open(command -> stdout_redirect, O_WRONLY | O_CREAT, 0660) == -1){
					fprintf(stderr,"%s: Can not open file.\n", command -> stdin_redirect);
					exit(1);
				}
			}				

			// execute cmd
			int result = execvp(command -> sub_commands[i].argv[0], command -> sub_commands[i].argv);
			if(result < 0 ){
				fprintf(stderr,"%s: Command not found\n", command -> sub_commands[i].argv[0]);
				exit(1);		
			}	
		} // if end

		if (parent == 1){
			waitpid(waitchild[i],NULL,0);
		}
	} // for end

} // ExecuteCmd end


