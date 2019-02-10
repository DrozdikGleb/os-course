#include <stdio.h>
#include <dlfcn.h>

extern int sum();
extern int sub();

int main()
{
	int sum_answer = sum(4, 5);
	int sub_answer = sub(4, 5);

	void* library_handler = dlopen("libdyn2.so", RTLD_LAZY);
	if (!library_handler){
		fprintf(stderr, "%s\n", dlerror());
		return 1;
	}
	int (*func)(int, int);
	func = dlsym(library_handler, "mult");
	int mult_answer = func(4, 5);
	printf("%d\n", sum_answer);
	printf("%d\n", sub_answer);
	printf("%d\n", mult_answer);

	dlclose(library_handler);
	return 0;
}