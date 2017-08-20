/*************************************************************************
	> File Name: Mdwct.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月17日 星期四 23时37分32秒
 ************************************************************************/

#include"Mdwct.h"
#include"openSource.h"

int main()
{
    printf("Mdwct.main start\n");
    
    system("./getSource_bak.sh");

    if(-1 == openSource())
    {
        printf("Error:openSource is failed!\n");
    }
    return 0;
}
