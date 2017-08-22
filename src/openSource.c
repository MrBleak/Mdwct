/*************************************************************************
	> File Name: openSource.cpp
	> Author:
	> Mail:
	> Created Time: 2017年08月17日 星期四 18时58分01秒
 ************************************************************************/

#include "openSource.h"
#include"linkStack.h"

int openSource()
{
    FILE* p = NULL;
    if (NULL == (p = fopen("./SourcePathbak", "r")))
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
            printf("Error:open %s failed\n", fname);
        }
        else
        {
            // 添加更改文件的實現函數:addZlog(FILE** fpS);
            char cMacro[] = "#include \"stdio.h\"\n#define Function_entry_location_zrh printf(\"FILE:%s:FUNC:%s:LINE:%d\\n\", __FILE__, __FUNCTION__, __LINE__);\n";
            FILE* fpTmp = NULL;
            if (NULL == (fpTmp = fopen("Temp.zrh", "w")))
            {
                printf("Error:open Temp.zrh failed\n");
                return -1;
            }

            if (EOF == fputs(cMacro, fpTmp))
            {
                printf("Error:MACRO failed!\n");
            }

            char d = '\n';
            DataType e;
            int isMacro = 0;
            LinkStack pTopSt = NULL;
            char addlog[] = "\n\tFunction_entry_location_zrh\n";
            InitStack( &pTopSt );
            while(EOF != (c = fgetc(fp)))
            {
                if (EOF == fputc(c, fpTmp))
                {
                    printf("Error:put c to fpTmp failed\n");
                }
                if (('\n' == d) && '#' == c)
                {
                    isMacro = 1;
                }
                d = c;
                if ((1 == isMacro) && ('\n' == c))
                {
                    isMacro = 0;
                }

                if (0 == isMacro)
                {
                    switch (c)
                    {
                        case '(' :
                            {
                                if(2 < StackLength(pTopSt))
                                {
                                    PushStack(pTopSt, c);
                                }
                                else if (2 == StackLength(pTopSt))
                                {
                                    GetTop(pTopSt, &e);
                                    if ('(' == e)
                                    {
                                        PushStack(pTopSt, c);
                                    }
                                    else if(')' == e)
                                    {
                                        PopStack(pTopSt, &e);
                                    }
                                }
                                else
                                {
                                    LinkStack pTRA = pTopSt->next;

                                    while (pTRA)
                                    {
                                        if ('(' != pTRA->data)
                                        {
                                            break;
                                        }
                                        pTRA = pTRA->next;
                                    }
                                    if (!pTRA)
                                    {
                                        PushStack(pTopSt, c);
                                    }
                                }
                            }
                            break;
                        case ')' :
                            {
                                GetTop(pTopSt, &e);
                                if(1 == StackLength(pTopSt))
                                {
                                    PushStack(pTopSt, c);
                                }
                                else if(1 < StackLength(pTopSt))
                                {
                                    if ('(' == e)
                                    {
                                        PopStack(pTopSt, &e);
                                    }
                                }
                            }
                            break;
                        case '{' :
                            {
                                if(2 == StackLength(pTopSt))
                                {
                                    PushStack(pTopSt, c);

                                    if (EOF == fputs(addlog, fpTmp))
                                    {
                                        printf("Error:add log failed!\n");
                                    }
                                }
                                else if(2 < StackLength(pTopSt))
                                {
                                    PushStack(pTopSt, c);
                                }
                            }
                            break;
                        case '}' :
                            {
                                GetTop(pTopSt, &e);
                                if('{' == e)
                                {
                                    PopStack(pTopSt, &e);
                                    if(2 == StackLength(pTopSt))
                                    {
                                        PopStack(pTopSt, &e);
                                        PopStack(pTopSt, &e);
                                    }
                                }
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
            fclose(fpTmp);
            fpTmp = NULL;
            fclose(fp);
            fp = NULL;
            DestoryStack(pTopSt);
#if 0
            printf("%s\n", fname);
            if(-1 == remove(fname))
            {
                printf("delete file failed\n");
            }
#endif
            if(-1 == rename("./Temp.zrh", fname))
            {
                printf("Error:con not move file!\n");
            }
        }
    }
    fclose(p);
    p = NULL;
    return 0;
}
