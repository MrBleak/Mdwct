#!bin/bash
for file in ./*
do
    if test -f $file
    then
        echo $file ���ļ�
    else
        echo $file ��Ŀ¼
    fi
done