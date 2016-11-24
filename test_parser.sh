#!/bin/bash

#colored strings
pass="\033[1;32mPASS\033[1;39m"
error="\033[1;31mFAIL\033[1;39m"

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
echo
for i in `ls`
do
    echo
    echo "Expected error:" `cat $i | grep //`
    ./../../parser $i > /dev/null
    ret=$?
    if [ $ret -eq 0 ]
        then
        echo -e "TEST ERROR: $i: $error || return code : $ret"
    else
        echo -e "TEST ERROR: $i: $pass  || return code : $ret"
    fi
done

cd ../..
echo

cd tests/good_test
pwd
echo
echo
echo "Tests for good code"
for i in `ls`
do
    echo
    ./../../parser $i > /dev/null
    ret=$?
    if [ $ret -eq 0 ]
        then
        echo -e "TEST GOOD: $i: $pass  || return code : $ret"
    else
        echo -e "TEST GOOD: $i: $error || return code : $ret"
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