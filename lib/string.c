void
memcpy(void *dst, void *src, unsigned int size)
{
	__asm__ volatile ("\n"
			  "cld\n"
			  "rep\n" "movsb\n"::"S" (src), "D"(dst), "c"(size)
			  :);
}

void
memset(void *dst, unsigned int c, unsigned int size)
{
	__asm__ volatile ("\n"
			  "shrl $2, %%ecx\n"
			  "cld\n"
			  "rep\n" "stosl\n"::"D" (dst), "a"(c), "c"(size)
			  :);
}

// 0:equal
// 1:nequal
int
memcmp(void *dst, void *src, unsigned int size)
{
	unsigned int result;
	__asm__ volatile ("\n"
			  "shrl $2, %%ecx\n"
			  "xor %%eax, %%eax\n"
			  "cld\n"
			  "rep\n"
			  "cmpsl\n"
			  "je 1f\n" "incl %%eax\n" "1:\n":"=a" (result)
			  :"S"(src), "D"(dst), "c"(size)
			  :);
	return result;
}

unsigned int
strlen(char *s)
{
	unsigned int result;
	__asm__ volatile ("\n"
			  "movl $0xffffffff, %%ecx\n"
			  "movb $0, %%al\n"
			  "cld\n"
			  "repne\n"
			  "scasb\n"
			  "jne notfound\n"
			  "subl $0xffffffff, %%ecx\n"
			  "neg %%ecx\n"
			  "dec %%ecx\n"
			  "jmp found\n"
			  "notfound:\n"
			  "xorl %%ecx, %%ecx\n" "found:\n":"=c" (result)
			  :"S"(s)
			  :);
	return result;
}

char *
strcpy(char *dst, const char *src)
{
	char c;
	char *cpy = dst;
	while (c = *src != '\0') {
		*dst = c;
		src++;
		dst++;
	}
	*dst = c;
	return cpy;
}

char *
strncpy(char *dst, const char *src, unsigned int size)
{
	for (int i = 0; i < size; i++) {
		dst[i] = src[i];
	}
	return dst;
}

char *
strcat(char *dst, const char *src)
{
	char *cpy = dst;
	char c;
	while (c = *dst != '\0') {
		dst++;
	}
	strcpy(dst, src);
	return cpy;
}

char *
strncat(char *dst, const char *src, unsigned int size)
{
	char *cpy = dst;
	char c;
	while (c = *dst != '\0') {
		dst++;
	}
	strncpy(dst, src, size);
	return cpy;
}

int
strcmp(const char *dst, const char *src)
{
	char cs = '\0';
	char cd = '\0';
	while (cs == cd) {
		cs = *src;
		cd = *dst;
		if (cs != cd) {
			return 1;
		}
		if (cs == '\0') {
			return 0;
		}
		src++;
		dst++;
	}
}

int
strncmp(const char *dst, const char *src, unsigned int size)
{
	for (int i = 0; i < size; i++) {
		if (dst[i] != src[i]) {
			return 1;
		}
	}
	return 0;
}
