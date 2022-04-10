#!/bin/bash  
  
for i in {1..21}  
do  
echo -e "\n\033[34m<$i.c>\033[0m\n"
./parser ./examples/lab2/$i.c
done