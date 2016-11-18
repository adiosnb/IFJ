#!/bin/bash

test_file="input_test.txt"
output="out"
#cleaning and compiling
make clean
make
clear

#backup original test file
cp $test_file test_file_backup.txt

rm $test_file

echo "TEST 1"
echo "i*i" > $test_file
echo "Expected
i i *"
./parser

echo "TEST 2"
echo "i+i+i*i" > $test_file
echo "Expected
i i + i i * +"
./parser



echo "TEST 3"
echo "i*i*i-i+i/i" > $test_file
echo "Expected
i i * i * i - i i / +"
./parser


echo "TEST 4"
echo "(((i+i)/i+i)*(i+i/i)*i)/i" > $test_file
echo "Expected
i i + i / i + i i i / + * i * i /"
./parser


#recovery of test file
rm $test_file
mv test_file_backup.txt $test_file

make clean