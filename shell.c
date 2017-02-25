#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "struct.c"
#include "fetchcmd.c"
#include "execmd.c"





int main(){

	printf("================= Ethan's Shell Project =================\n");

	while (1){
		struct Command *command = malloc(sizeof(struct Command));
		char result[200];
		char* user = getlogin();
		char* path = getcwd(result, sizeof(result));
		if(user == NULL){
			user = "";
		}
		printf("%s: shell %s$ ",result , user);
		char line[200];
		fgets(line, sizeof(line), stdin);
		line[strlen(line)-1] = '\0';
		
		if (!strcmp(line, "exit")){
			printf("====================== Exit Shell =======================\n");
			return 0;
		}

		if (!strcmp(line, "\0")){
			continue;
		}

// fetch command
		ReadCommand(line, command);	
		ReadRedirectsAndBackground(command);
		for (int i = 0; i < command -> num_sub_commands; i++){
			ReadArgs(command->sub_commands[i].line, command->sub_commands[i].argv, MAX_ARGS);
		}

// execute command
		
		//printer as expmle
		//PrintCommand(command);

		ExecuteCmd(command);
		free(command);

		//printf("End ExecuteCmd\n" );
	} //while end
}


