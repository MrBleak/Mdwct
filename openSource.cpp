/*************************************************************************
	> File Name: openSource.cpp 
	> Author: 
	> Mail: 
	> Created Time: 2017年08月17日 星期四 18时58分01秒
 ************************************************************************/

#include "openSource.h"

int openSource()
{
    FILE* fp = NULL;
    if (NULL == (fp = fopen("./SourcePath", "r")))
    {
        printf("open SourcePath failed\n");
        return -1;
    }
    char c;
    char fname[1000] = {0};
    char* q = NULL;
    while(!feof(p))
    {
        memset(fname, 0, 1000);
        q = &fname[0];
        while('\n' != (c = fgetc(p)))
        {
            if (EOF == c)
                break;
            *q = c;
            q++;
        }
        if (EOF == c)
            break;
        FILE* fp = NULL;
        
        if(NULL == (fp = fopen(fname, "r")))
        {
            printf("open %s failed\n", fname);
        }
        else
        {
            // 添加更改文件的實現函數:addZlog(FILE** fpS);
            fclose(fp);
        }
    }
    return 0;
}
