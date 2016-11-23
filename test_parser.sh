#!/bin/bash

#backup makefile

if [ "`ls Makefile`" != "" ]
then
    mv Makefile  tmp_make_file
fi


#init
cmake CMakeLists.txt
make parser

#start with error tests
cd tests/error_test
pwd
echo
echo

echo "Tests for error code"
for i in `ls`
do
    ./../../parser $i > /dev/null
    ret=$?
    if [ $ret -eq 0 ]
        then
        echo "TEST ERROR: $i: FAIL"
    else
            echo "TEST ERROR: $i: PASS"
    fi
done

cd ../..
echo

cd tests/good_test
pwd
echo "Tests for good code"
for i in `ls`
do
    ./../../parser $i > /dev/null
    ret=$?
    if [ $ret -eq 0 ]
        then
        echo "TEST GOOD: $i: PASS"
    else
        echo "TEST GOOD: $i: FAIL"
    fi
done

cd ../..
pwd


echo
echo
echo

#destroy
make clean
echo "rm -r CMakeFiles"
rm -r CMakeFiles/
rm -v  cmake_install.cmake CMakeCache.txt Makefile

if [ "`ls tmp_make_file`" != "" ]
then
    mv tmp_make_file Makefile
fi