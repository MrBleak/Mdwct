#include <stdio.h>
#include <stdlib.h>

int main()
{
	FILE* pAdd = fopen("zhangrenhe_text.txt", "w");
	FILE* pfile = fopen("./main.cpp", "r");
	if((!pfile) || (!pAdd))
	{
		printf("Error:Open fial\n");
	}
	int c;
	char str[]="//my dream will come true.\r\n";
	fputs(str, pAdd);
	while((c=fgetc(pfile))!=EOF)
	{
	//	printf("%c", c);
		fputc(c, pAdd);
	}
	fclose(pfile);
	fclose(pAdd);
    system("./rm.sh");
	return 0;
}
