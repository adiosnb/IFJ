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

echo "Basic tests for op parser function"

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

echo "TEST 5"
echo "(i*i/i+1)/i+(i/i/(i+i*i))/(i+i+i*i*i+i)" > $test_file
echo "Expected
i i * i / 1 + i / i i / i i i * + / i i + i i * i * + i + / +"
./parser

echo "Test for correct finding error"

echo "TEST 6"
echo "i i - i" > $test_file
echo "Expected missing operator"
./parser

echo "TEST 7"
echo "i + (i - i" > $test_file
echo "Expected missing parenthesis"
./parser

echo "TEST 8"
echo "i + (i - i))" > $test_file
echo "Expected too many parethesis"
./parser

echo "TEST 9"
echo "(i*i/i+1)/i+(i/i/(i+i*i))/(i+i+i i*i+i)" > $test_file
echo "Expected missing operator"
./parser

echo
echo "TEST 10"
echo "(i*i/i+1)/i+(i/i/((i+i*i))/(i+i+i/i*i+i)" > $test_file
echo "Expected missing right parenthesis"
./parser


echo
#cleaning up after tests
rm $test_file
mv test_file_backup.txt $test_file

make clean