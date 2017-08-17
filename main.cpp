#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    // system("./getSource.sh");
	FILE* fpSP = fopen("./SourcePath", "r");
	if(!fpSP)
	{
		printf("Error:Open failed\n");
        return -1;
	}
    char cFName[1000] = {0};  
    int len = 0;

    while(!feof(fpSP))  
    {  
        memset(cFName, 0, sizeof(cFName));  

        FILE* fpS = fopen(cFName, "r");
    	if(!fpS)
    	{
	    	printf("Error:Open failed\n");
            return -1;
	    }
        FILE* fpTmp = fopen("Temp.zrh", "w");
	    int c;
	    char str[]="//my dream will come true.\r\n";
	    fputs(str, fpTmp);
	    while((c=fgetc(fpS))!=EOF)
	    {
	    //	printf("%c", c);
		    fputc(c, fpTmp);
	    }
	    fclose(fpS);
	    fclose(fpTmp);
    }
    /*
    FILE* fpS = fopen();
    */
	fclose(fpSP);
    system("./rm.sh");
    
	return 0;
}
