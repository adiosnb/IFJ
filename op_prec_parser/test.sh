#!/bin/bash

i=100
while [ $i != 0 ] 
do
    generate_expr/generate > input_test.txt
    ./parser > out.txt 2>/dev/null
    echo $i
    i=$(( $i-1 ))
done
