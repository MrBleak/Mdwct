#include <stdio.h>
#include <string.h>

void print(char* data)
{
  printf("data:%s\n", data);
  printf("size:%d\n", strlen(data) + 1);
}

int main(int argc, char *argv[])
{
	if (argc == 2)
	{
		// double a = (double)argv[1];
		printf("argv[1]=%d\n", *argv[1]);
	}
	char c = '1';
	char d = '0';
  char str[11] = "zhangrenhe";
	
	printf("c=%f\td=%f\n", (double)c, (double)d);
  print(str);
  return 0;
}
