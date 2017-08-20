/*************************************************************************
	> File Name: linkStack.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月18日 星期五 20时22分05秒
 ************************************************************************/

#include "linkStack.h"
#include <stdlib.h>
#include <stdio.h>

void InitStack(LinkStack *top){
     // 为头结点分配存储单元
     if((*top=(LinkStack)malloc(sizeof(LStackNode)))==NULL){
         exit(-1);  
     }
     // 将头结点的指针域置为空
     (*top)->next = NULL;
}
// 判断栈是否为空
int StackEmpty(LinkStack top){
    if(top->next == NULL){
         return 1;
    }else{
         return 0;      
    }
}
// 将元素入栈：需要为入栈的结点分配内存
int PushStack(LinkStack top,DataType e){
    LStackNode *p;
    if((p=(LStackNode*)malloc(sizeof(LStackNode)))==NULL){
         printf("内存分配失败\n");
         return 0;
    }
    p->data = e;
    p->next = top->next;
    top->next = p;
    return 1;
}
// 将元素出栈：需要释放结点空间
int PopStack(LinkStack top,DataType *e){
    LStackNode *p;
    p = top->next;
    if(!p){
         printf("栈已空\n");
         return 0; 
    }
    top->next = p->next;
    *e = p->data;
    free(p);
    return 1;
}
// 取栈顶元素：需要对边界做判断（栈是否为空）
int GetTop(LinkStack top,DataType *e){
    LStackNode *p;
    p = top->next;
    if(!p){
         printf("栈已空\n");
         return 0; 
    }
    *e = p->data;
    return 1;
}
// 获取栈长度：因为需要头结点，依次寻找下一个结点，所以时间复杂度为：O(n)
int StackLength(LinkStack top){
    int i=0;
    LStackNode *p;
    p = top;
    while(p->next){
         p = p->next;             
         i++;
    }
    return i;
}
// 销毁链栈：需要释放动态分配的结点空间
void DestoryStack(LinkStack top){
     LStackNode *p,*q;
     p = top;
     while(!p){
          q = p;
          p = p->next;
          free(q);
     }
}
