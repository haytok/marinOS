#include <stdio.h>
#include <string.h>
#include <assert.h>

#define PASSED_TEST_MESSAGE(...) printf("Passed Test for %s !!!\n", __func__);

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

	PASSED_TEST_MESSAGE();
}

int main(void)
{
	puts("Start Test !!!");
	test_strlen();
	test_strcpy();
	test_strcmp();
	puts("Passed Test !!!");

	return 0;
}
