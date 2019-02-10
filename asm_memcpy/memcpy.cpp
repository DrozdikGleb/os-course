#include <cstdio>
#include <cstring>
#include <iostream>

using namespace std;

void *copy_1(void* dest, const void* src, size_t i){
	/*char * r = (char *)dest + i;
	*r = *((char *)src + i);*/
	asm("mov (%0), %%al \n\t"
			"mov %%al, (%1) \n\t"
			:
			:"r"((const char*)src + i), "r"((char*)dest + i)
			:"al", "memory"
			);
}

void *memcpy_1(void* dest, const void* src, int n){
	for (size_t i = 0; i < n; ++i){
		copy_1(dest, src, i);
	}
}

void* memcpy_8(void* dest, const void* src, int n){
	int k = 1;
	while (n - 8 * k > 0) {
		asm("mov (%0), %%rax \n\t"
			"mov %%rax, (%1) \n\t"
			:
			:"r"((const char*)src + 8 * (k - 1)), "r"((char*)dest + 8 * (k - 1))
			:"rax", "memory"
			);
		k++;
	}
	for (size_t i = 8 * (k - 1); i < n; ++i) {
		copy_1(dest, src, i);
	}
}

void* memcpy_16(void* dest, const void* src, int n) {
	int k = 1;
	//xmm0 128 bit register , movdqu - mov for xmm0
	while (n - 16 * k > 0) {
		asm("movdqu (%0), %%xmm0 \n\t"
			"movdqu %%xmm0, (%1) \n\t"
			:
			:"r"((const char*)src + 16 * (k - 1)), "r"((char*)dest + 16 * (k - 1))
			:"xmm0", "memory"
			);
		k++;
	}
	for (size_t i = 16 * (k - 1); i < n; ++i) {
		copy_1(dest, src, i);
	}
}

void* memcpy_16_align(void* dest, const void* src, int n){
    size_t cur = 0;

    
    while((size_t)((char *)src + cur) % 16 != 0 && cur < n) {
        copy_1(dest, src, cur);
        cur++;
    }
    
    
    int k = 1;
	while (n - cur > 16) {
		asm("movdqa (%0), %%xmm0 \n\t"
			"movdqu %%xmm0, (%1) \n\t"
			:
			:"r"((const char*)src + 16 * (k - 1)), "r"((char*)dest + 16 * (k - 1))
			:"xmm0", "memory"
			);
		k++;
		cur += 16;
	}
	for (int i = cur; i < n; ++i) {
		copy_1(dest, src, i);
	}
}

int main(){
	char src[] = "Hello, world";
	//test by 1 byte
	char dest[12];
	memcpy_1(dest, src, 12);
	printf("%s\n", dest);
	//by 8 bytes
	char src8[] = "Hello, world; Goodbye, world";
	char dest8[29];
	memcpy_8(dest8, src8, 29);
	printf("%s\n", dest8);
	//by 16 bytes
	char src16[] = "Hello, world; Goodbye, worldHello, world; Goodbye, world fjfdjkjsfdkjghdsfjghdflkjghdflkjghsldfkjghsdlfkjhgsdlkfjhgldsfkjhgdlskfjhgldskfjhgsldkfjg";
	char dest16[57];
	//memcpy_16(dest16, src16 + 2, 55);
	//printf("%s\n", dest16);

	char dest16_align[100];
	memset(dest16_align, '-', 70);
	memcpy_16_align(dest16_align  + 2, src16, 40);
	printf("%s\n", dest16_align);
}