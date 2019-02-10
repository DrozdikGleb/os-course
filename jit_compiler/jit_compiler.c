#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>

const size_t SIZE = 1024;

typedef int (*func)(int);

void* alloc_memory(size_t size){
	void* ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (ptr == (void*)-1) {
		perror("mmap");
		return NULL;
	}
	return ptr;
}

int add_security(void* ptr, size_t size){
	if (mprotect(ptr, size, PROT_READ | PROT_EXEC) == -1){
		perror("mprotect");
		return -1;
	}
	return 0;
}

void put_function(char* ptr){
	char code[] = {
		0x55,
		0x48, 0x89, 0xe5,
		0x89, 0x7d, 0xfc,
		0x8b, 0x45, 0xfc,
		0x83, 0xc0, 0x05,
		0x5d,
		0xc3
	};
	memcpy(ptr, code, sizeof(code));
}
int modificate(char* ptr, int num){
	char* new_ptr = ptr + 12;
	if (mprotect(ptr, SIZE, PROT_READ | PROT_WRITE) == -1){
		perror("mprotect");
		return -1;
	}
	memcpy(new_ptr, &num, 1);
	if (mprotect(ptr, SIZE, PROT_READ | PROT_EXEC) == -1){
		perror("mprotect");
		return -1;
	}
}

int main(int argc, char const *argv[])
{
	int number;
	scanf("%d", &number);
	void* pointer = alloc_memory(SIZE);
	put_function(pointer);
	add_security(pointer, SIZE);
	modificate(pointer, number);
	func f = pointer;
	int result = f(3);
	printf("%d\n", result);
	munmap(pointer, SIZE);
	return 0;
}