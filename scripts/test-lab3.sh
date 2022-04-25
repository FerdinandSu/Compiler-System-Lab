#!/bin/bash  

mkdir out

for i in {1..4}  
do  
echo -e "\n\033[34m<$i.c>\033[0m\n"
./parser ./examples/lab3/$i.c ./out/$i.ir
done