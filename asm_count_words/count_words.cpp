
#include <iostream>
#include <stdio.h>
#include <emmintrin.h>

using namespace std;

const __m128i SPACES_MASK = _mm_set1_epi8((char) 32);
const __m128i ONE_MASK = _mm_set1_epi8((char) 1);

int naive_impl(string str, int size) {
    if (size == 0) {
        return 0;
    }
    int i = 0;
	int count = 0;
	if (str[i] != ' '){
        count++;    
    }
    i++;
	while (i < size) {
		if (str[i] != ' ' && str[i - 1] == ' '){
			count++;
		}
        i++;
	}
	return count;
}

int fast_count(const char *string, int size) {
    int i = 0;
    int ans = 0;

    while((size_t)(string + i) % 16 != 0 && i < size) {
        i++;
    }

    ans += naive_impl(string, i + 1);

    while (size > 16) {
        __m128i stringLeftShift = _mm_and_si128(_mm_cmpgt_epi8(_mm_loadu_si128((__m128i *) (string + i - 1)), SPACES_MASK),
                                     ONE_MASK);
        __m128i actualString = _mm_and_si128(_mm_cmpgt_epi8(_mm_loadu_si128((__m128i *) (string + i)), SPACES_MASK),
                                      ONE_MASK);
        __m128i stringXor = _mm_xor_si128(stringLeftShift, actualString);
        int a = __builtin_popcountll((((unsigned long long *) (&stringXor))[0])) + 
                __builtin_popcountll((((unsigned long long *) (&stringXor))[1]));
        ans += a;
        i += 16;
        size -= 16;
    }
    ans /= 2;
    if(string[i-1]!=' ' && string[i]==' '){
        ans++;
    }
    ans += naive_impl(string + i, size);
    return ans;
}

int main(){
    char src[239];
    int n = 238;

    char src1[11] = "aa aa aa";
    int n1 = 10;

    for (int i = 0; i < 238; ++i) {
        if (i % 2 == 0){
            src[i]=' ';
        } else {
            src[i] = '1';
        }
    }
    std::string test = "                         01234567890abcdef  s s s                       ab    ";
    int a = naive_impl(src, n);
    int b = fast_count(src, n);
    printf("%d\n", b);

	printf("%d\n", a);
}