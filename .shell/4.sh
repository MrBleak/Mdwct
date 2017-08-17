#!/bin/bash

read -p "请输入一个数字：" n
n2=`echo $n|grep -E '[^0-9]'|wc -l`

if [ $n2 -eq 1 ]
then
	echo "你输入的不是数字"
	exit
fi

ni=$[$n%2]

if [ $ni -eq 0 ]
then
	echo "你输入的是偶数"
else
	echo "你输入的是奇数"
fi
