
#include <ucontext.h> 
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>

char* register_strings[23] = {"REG_R8", "REG_R9", "REG_R10", "REG_R11", "REG_R12", "REG_R13", "REG_R14", "REG_R15", "REG_RDI", "REG_RSI", "REG_RBP", "REG_RBX", "REG_RDX", "REG_RAX", "REG_RCX", "REG_RSP", 
								"REG_RIP", "REG_EFL", "RER_CSGSFS", "REG_ERR", "REG_TRAPNO", "REG_OLDMASK", "REG_CR2"}; 

jmp_buf jump;

void handler_memory(int signo, siginfo_t* info, void* vcontext) {
    if (info->si_signo == SIGSEGV) {
        longjmp(jump, 1);
    }
}

void dump_memory(int address){
	  struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_sigaction = handler_memory;
    action.sa_flags = SA_SIGINFO;
    if (sigaction(SIGSEGV, &action, NULL) < 0) {
        perror("sigaction in dumping memory");
        exit(1);
    }
    if (setjmp(jump) == 0){
    	  printf("address - 0x%x\n", address);
    } else {
        printf("bad address - 0x%x\n", address);
        longjmp(jump, 0);
    }
}

void handler(int signo, siginfo_t* info, void* vcontext) {
	if (info->si_signo == SIGSEGV) {
  		printf("Segmentation fault. Memory location where was a fault %p \n", info->si_addr);
  		ucontext_t* context = (ucontext_t*)vcontext;
		printf("<-------REGISTER DUMP------->\n");
		for (int i = 0; i < NGREG; i++){
			printf("%s with value - 0x%x\n", register_strings[i], (unsigned int) context->uc_mcontext.gregs[i]);
		}
		char* curCharAddress = info->si_addr;
		printf("<-------MEMORY DUMP------->\n");
		for (int i = curCharAddress - 20; i < curCharAddress + 20; i++) {
			dump_memory(i);
		}
    exit(1);
	}
}

int main() {
  	struct sigaction action;
  	memset(&action, 0, sizeof(struct sigaction));
  	action.sa_flags = SA_SIGINFO;
  	action.sa_sigaction = handler;
  	if (sigaction(SIGSEGV, &action, NULL) < 0) {		
  		perror("sigaction : SIGSEGV");
  		exit(1);
  	}
  	char* c = "hello";
	  c[6] = 'f';

  	return 0;
}