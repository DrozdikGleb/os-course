#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

// greater - 1 for more than size, 0 for equal size, -1 otherwise.
struct Arguments {
	char *name;
	char *path;
	int inum;
	off_t size;
	int greater;
	nlink_t nlinks;
};

struct Arguments arguments = {NULL, NULL, -1, -1, -2, -1};

void parse_args(char* line[], int argc) {
	for (int i = 2; i < argc - 1; i+=2){
		char *curArg = line[i + 1];
		if (strcmp(line[i], "-inum") == 0){
			arguments.inum = atoi(curArg);
		} else if (strcmp(line[i], "-name") == 0) {
			arguments.name = curArg;
		} else if (strcmp(line[i], "-nlinks") == 0) {
			arguments.nlinks = atoi(curArg);
		} else if (strcmp(line[i], "-exec") == 0) {
			arguments.path = curArg;
		} else if (strcmp(line[i], "-size") == 0) {
			switch (curArg[0]){
				case '-': 
					arguments.size = atoi(&curArg[1]);
					arguments.greater = -1;
				break;
				case '+':
					arguments.size = atoi(&curArg[1]);
					arguments.greater = 1;
				break;
				case '=':
					arguments.size = atoi(&curArg[1]);
					arguments.greater = 0;
				break;
				default:
					perror("need = or - or + before size");
					exit(-1);
				}
		} else {
			perror("unexpected argument");
			exit(-1);
		}
	}
}

void doExecve(char** parameters){
	pid_t pid = fork();
	int status = 0;
	if(pid == 0){
    	if (execve(parameters[0], parameters, NULL) == -1){
			perror("execve failed");
		}
   		exit(EXIT_FAILURE);
   	} else {
   		if (pid > 0) {
   			do{
   			int error = wait(&status);
   		    	if(error == -1){
   		    	perror("When we wait child error was occured.");
   		    	exit(EXIT_FAILURE);
   		    }
   			} while(!WIFEXITED(status) && !WIFSIGNALED(status));
   		} else {
   			perror("fork failed");
   		}
   	}
}

void workWithFile(char * path, struct dirent *d, struct stat sb){
	if ((arguments.inum != -1 && arguments.inum != sb.st_ino) || (arguments.nlinks != -1 && arguments.nlinks != sb.st_nlink)){
		return;
	}
	if ((arguments.name != NULL && strcmp(arguments.name , d->d_name) != 0)){
		return;
	}
	if(arguments.size != -1){
		switch (arguments.greater){
			case 1 : if (sb.st_size <= arguments.size) return;
				break;
			case -1 : if (sb.st_size >= arguments.size) return;
				break;
			case 0 : if(sb.st_size != arguments.size) return;
				break;
		}
	}
	if(arguments.path != NULL){
		char* parameters[] = {arguments.path, path, NULL};
		doExecve(parameters);
	}else{
		printf("%s \n", path); 
	}
}

void walkThroughDirectories(char *dir){
	DIR *curDir;
	struct dirent *d;
	char b[512];
	curDir = opendir(dir);
	struct stat sb;
	if(curDir != NULL){
		while((d = readdir(curDir)) != NULL){
			if(strcmp(d->d_name, ".") == 0 || strcmp(d->d_name,"..") == 0){
				continue;
			}
			char curPath[] = "";
			strcat(curPath, dir);
			strcat(curPath, "/");
			strcat(curPath, d->d_name);
			switch(d->d_type) {
				case DT_REG :
					if (stat(curPath, &sb) < 0){
						perror("something with stat");
						return;
					}
					workWithFile(curPath, d, sb);
					break;
				case DT_DIR :
					walkThroughDirectories(curPath);
					break;
			}
		}
		closedir(curDir);
	}
}


int main(int argc, char *argv[])
{
	struct dirent *d = NULL;
	struct stat fileStat;
	DIR *pDir = NULL;
	if (argc < 2){
		printf("%s\n", "not enough arguments");
		return 1;
	}
	parse_args(argv, argc);
	walkThroughDirectories(argv[1]);
	return 0;
}