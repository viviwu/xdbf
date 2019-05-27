#!/usr/bin/env bash 

set -euxo pipefail          #set -eux   set -o pipefail

echo 'hello bash!'

function func_expr(){
    local var1=99
    echo 'var1=' $var1
    unset var1      #delete var1

    a=10
    b=20
    #val=a+b 原生bash不支持简单的数学运算，但是可以通过其他命令来实现，例如 awk 和 expr，expr 最常用。
    val=`expr $a + $b`      #expr 是一款表达式计算工具，使用它能完成表达式的求值操作。
    echo "a + b : $val"
    unset a b var   #空格后一起删除 
}
func_expr    #调用function


function  shell_array(){
    my_array=(A B "C" D) 
    my_array[1]=2 
    echo "第二个元素为: ${my_array[1]}" 

    echo "数组的元素为: ${my_array[*]}"
    echo "数组的元素为: ${my_array[@]}"

    echo "数组元素个数为: ${#my_array[*]}"
    echo "数组元素个数为: ${#my_array[@]}"
}
shell_array

function func_cmd(){
    touch log.txt   #creat a file named log.txt 
    read name 
    echo -e "$name It is a test \c"     # -e 开启转义 \c 不换行
    echo -e "OK! \n" > log.txt          # 显示结果定向至文件
}
func_cmd