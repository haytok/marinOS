#include <stdio.h>
#include <string.h>
#include <assert.h>

#define PASSED_TEST_MESSAGE(...) printf("Passed Test for %s !!!\n", __func__);

void *original_memset(void *s, int c, long len)
{
	char *head;
	for (head = s; len > 0; len--) {
		*(head++) = c;
	}
	return head;
}

void *original_memcpy(void *dest, const void *src, long n)
{
	char *d = dest;
	const char *s = src;
	for (; n > 0; n--) {
		*(d++) = *(s++);
	}
	return dest;
}

int original_memcmp(const void *s1, const void *s2, long n)
{
	// NULL チェックしても良さそう。
	if (n == 0) {
		return 0;
	}

	// char 専
	const char *t1 = s1;
	const char *t2 = s2;
	for (; n > 0; n--) {
		if (*t1 != *t2) {
			return (*t1 > *t2) ? 1 : -1;
		}
		t1++;
		t2++;
	}
	return 0;
}

int original_strlen(const char *s)
{
	const char *head = s;
	int len = 0;
	while (*head++)
		len++;
	return len;
}

char *original_strcpy(char *dest, const char *src)
{
	char *d = dest;
	for (;; dest++, src++) {
		*dest = *src;
		if (!*src)
			break;
	}
	return d;
}

int original_strcmp(const char *s1, const char *s2)
{
	while (*s1 || *s2) {
		if (*s1 != *s2) {
			return (*s1 > *s2) ? 1 : -1;
		}
		*s1++;
		*s2++;
	}
	return 0;
}

int original_strncmp(const char *s1, const char *s2, int n)
{
	while ((*s1 || *s2) && n > 0) {
		if (*s1 != *s2) {
			return (*s1 > *s2) ? 1 : -1;
		}
		*s1++;
		*s2++;
		n--;
	}
	return 0;
}

void test_memset()
{
	char str_0[] = "0123456789";
	memset(str_0 + 2, '*', 5);
	assert(strcmp(str_0, "01*****789") == 0);

	char str_1[] = "0123456789";
	original_memset(str_1 + 2, '*', 5);
	assert(strcmp(str_1, "01*****789") == 0);

	PASSED_TEST_MESSAGE();
}

void test_memcpy()
{
	struct test_memcpy_case_type {
		char dest[255];
		const void *src;
		long n;
		char expected[255];
		void *(*func)(void *dest, const void *src, long n);
	} test_cases[] = {
		// original_memcpy に合わせて作成した関数ポインタの型の変数に memcpy を引数に渡しているので、Warning が出るが、一旦放置しておく。
		// ライブラリ関数 memcpy のテスト
		{ "1111111111", (const char *)"ab\0cde", 5, "ab", memcpy },
		{ "1111111111", (const char *)"abcde", 5, "abcde11111",
		  memcpy },
		{ "1111111111", (const char *){ "abcde" }, 5, "abcde11111",
		  memcpy },
		// 自作の memcpy のテスト
		{ "1111111111", (const char *)"ab\0cde", 5, "ab",
		  original_memcpy },
		{ "1111111111", (const char *)"abcde", 5, "abcde11111",
		  original_memcpy },
		{ "1111111111", (const char *){ "abcde" }, 5, "abcde11111",
		  original_memcpy }
	};

	for (size_t i = 0;
	     i < sizeof(test_cases) / sizeof(struct test_memcpy_case_type);
	     i++) {
		test_cases[i].func(test_cases[i].dest, test_cases[i].src,
				   test_cases[i].n);
		assert(strcmp(test_cases[i].dest, test_cases[i].expected) == 0);
	}

	PASSED_TEST_MESSAGE();
}

void test_memcmp()
{
	struct test_memcmp_case_type {
		const void *s1;
		const void *s2;
		int n;
		int expected;
		int (*func)(const void *s1, const void *s2, long n);
	} test_memcmp_cases[] = {
		// ライブラリ関数 memcpy のテスト
		{ (const char *)"abcde", (const char *)"abcde", 2, 0, memcmp },
		{ (const char *)"abcde", (const char *)"accde", 2, -1, memcmp },
		{ (const char *)"abcde", (const char *)"aacde", 2, 1, memcmp },
		// 自作の memcpy のテスト
		{ (const char *)"abcde", (const char *)"abcde", 2, 0,
		  original_memcmp },
		{ (const char *)"abcde", (const char *)"accde", 2, -1,
		  original_memcmp },
		{ (const char *)"abcde", (const char *)"aacde", 2, 1,
		  original_memcmp }
		// 以下のテストを実行した後に上のテストを実行すると、コアダンプする。
		// これは、Table ドリブンテストを実施すると、構造体が確保するメモリ空間の更新がうまくできずバグるため？？？
		// { (const char *)"\0abc\0de", (const char *)"\0abc\0de", 7, 0,
		//   memcmp },
		// { (const char *)"\0abc\0de", (const char *)"\0abcdef", 3, 0,
		//   memcmp },
		// { (const char *)"\0abc\0de", (const char *)"\0abc", 3, 0,
		//   memcmp },
	};
	for (size_t i = 0; i < sizeof(test_memcmp_cases) /
				       sizeof(struct test_memcmp_case_type);
	     i++) {
		struct test_memcmp_case_type obj = test_memcmp_cases[i];
		assert(obj.func(obj.s1, obj.s2, obj.n) == obj.expected);
	}

	PASSED_TEST_MESSAGE();
}

void test_strlen()
{
	// 本家 の strlen のテスト
	const char *s_0 = "hoge";
	assert(strlen(s_0) == 4);

	const char *s_1 = "";
	assert(strlen(s_1) == 0);

	const char s_2[] = "hoge";
	assert(strlen(s_2) == 4);

	assert(strlen("hello world") == 11);

	// 自作の strlen のテスト
	const char *s_3 = "hoge";
	assert(original_strlen(s_3) == 4);

	const char *s_4 = "";
	assert(original_strlen(s_4) == 0);

	const char s_5[] = "hoge";
	assert(original_strlen(s_5) == 4);

	assert(original_strlen("hello world") == 11);

	PASSED_TEST_MESSAGE();
}

void test_strcpy()
{
	// 本家の strcpy のテスト
	char str_1[] = "ABCDEF";
	char str_2[] = "123";
	char *str_3 = "abcd";

	// ヌル文字 \0 までコピーされるのが注意
	// 文字列の比較は == ではなく strcmp 関数等を使用する必要がある。
	strcpy(str_1, str_2);
	assert(strcmp(str_1, "123") == 0);

	strcpy(str_1, str_3);
	assert(strcmp(str_1, "abcd") == 0);

	strcpy(str_1, "xyz");
	assert(strcmp(str_1, "xyz") == 0);

	// 自作の strcpy のテスト
	char str_4[] = "ABCDEF";
	char str_5[] = "123";
	char *str_6 = "abcd";

	original_strcpy(str_4, str_5);
	assert(strcmp(str_4, "123") == 0);

	original_strcpy(str_4, str_6);
	assert(strcmp(str_4, "abcd") == 0);

	original_strcpy(str_4, "xyz");
	assert(strcmp(str_4, "xyz") == 0);

	PASSED_TEST_MESSAGE();
}

void test_strcmp()
{
	char str_0[] = "ABC";
	char str_1[] = "ABC";
	char str_2[] = "ABD";
	char str_3[] = "B";
	char str_4[] = "AAAA";

	assert(strcmp(str_0, str_1) == 0);
	assert(strcmp(str_0, str_2) == -1);
	assert(strcmp(str_0, str_3) == -1);
	assert(strcmp(str_0, str_4) == 1);

	assert(original_strcmp(str_0, str_1) == 0);
	assert(original_strcmp(str_0, str_2) == -1);
	assert(original_strcmp(str_0, str_3) == -1);
	assert(original_strcmp(str_0, str_4) == 1);

	PASSED_TEST_MESSAGE();
}

void test_strncmp()
{
	char str[] = "ABC";

	assert(strncmp(str, "ABD", 2) == 0);
	assert(strncmp(str, "ABC", 2) == 0);
	assert(strncmp(str, "AAA", 2) == 1);
	assert(strncmp(str, "ABCD", 2) == 0);
	assert(strncmp(str, "AB", 2) == 0);
	assert(strncmp(str, "B", 2) == -1);
	// このパターンだと溢れた文字の文字コードが帰ってくる。以下の例だと B の文字コード 66 が返る。
	assert(strncmp(str, "A", 2) == 66);

	assert(original_strncmp(str, "ABD", 2) == 0);
	assert(original_strncmp(str, "ABC", 2) == 0);
	assert(original_strncmp(str, "AAA", 2) == 1);
	assert(original_strncmp(str, "ABCD", 2) == 0);
	assert(original_strncmp(str, "AB", 2) == 0);
	assert(original_strncmp(str, "B", 2) == -1);
	// glibc の関数と違って、自作の strncmp は 0, 1, -1 飲み返す。
	assert(original_strncmp(str, "A", 2) == 1);

	PASSED_TEST_MESSAGE();
}

void test()
{
	test_memset();
	test_memcpy();
	test_memcmp();
	test_strlen();
	test_strcpy();
	test_strcmp();
	test_strncmp();
}

int main(void)
{
	puts("Start Test !!!");
	test();
	puts("Passed Test !!!");

	return 0;
}
