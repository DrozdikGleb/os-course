#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

vector<string> parseLine(const string str){
	stringstream ss;
	ss << str;
	string temp;
	vector<string> tokens;
	while (ss >> temp) {
		tokens.push_back(temp);
	}
	return tokens;
}

char** convertToChar(const vector<string> tokens){
	char** args = new char*[tokens.size() + 1];
	for (size_t i = 0; i < tokens.size(); ++i) {
		args[i] = new char[tokens[i].size() + 1];
        strcpy(args[i], tokens[i].c_str());
    }
    args[tokens.size()] = NULL;
    return args;
}

bool launch_process(char** args) {
    if(strcmp("exit", args[0]) == 0) {
        return false;
    }
    pid_t pid = fork();
    int status = 0;
    if(pid == 0){
    	if(execvp(args[0], args) == -1) {
    		perror("execve failed ");
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
    printf("%s %d\n", "exit code:", status);
    return true;
}

void freeMemory(char** array, size_t n){
    for(size_t i = 0; i < n; ++i) {
        delete[] array[i];
    }
    delete[] array;
}

int main(int argc, char const *argv[]) {	
	string line;
	char** tokens;
	bool status = true;
	while(status) {
		printf("$ ");
		getline(cin, line);
		if (line.size() == 0){
			continue;
		}
		vector<string> strings = parseLine(line);
		tokens = convertToChar(strings);
		status = launch_process(tokens);
		freeMemory(tokens, strings.size());
	}
	return 0;
}
