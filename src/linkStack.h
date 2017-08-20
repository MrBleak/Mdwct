/*************************************************************************
	> File Name: linkStack.h
	> Author: 
	> Mail: 
	> Created Time: 2017年08月18日 星期五 20时22分43秒
 ************************************************************************/

#ifndef _LINKSTACK_H
#define _LINKSTACK_H

typedef char DataType;
typedef struct node{
    DataType data;
    struct node *next;        
}LStackNode,*LinkStack;

//新建栈
void InitStack(LinkStack *top);
// 判断栈是否为空
int StackEmpty(LinkStack top);
// 将元素入栈：需要为入栈的结点分配内存
int PushStack(LinkStack top,DataType e);
// 将元素出栈：需要释放结点空间
int PopStack(LinkStack top,DataType *e);
// 取栈顶元素：需要对边界做判断（栈是否为空）
int GetTop(LinkStack top,DataType *e);
// 获取栈长度：因为需要头结点，依次寻找下一个结点，所以时间复杂度为：O(n)
int StackLength(LinkStack top);
// 销毁链栈：需要释放动态分配的结点空间
void DestoryStack(LinkStack top);

#endif
