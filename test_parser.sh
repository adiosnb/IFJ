#!/bin/bash

#colored strings
pass="\033[1;32mPASS\033[1;39m"
error="\033[1;31mFAIL\033[1;39m"

#backup makefile
if [ "`ls Makefile`" != "" ]
then
    mv Makefile  tmp_make_file
fi

#variables for statistics
error_num=0
pass_num=0

#init
cmake CMakeLists.txt
make parser

#start with error tests
cd tests/error_test
echo
echo -e "\033[1;39m"

echo "Tests for error code"
echo
for i in `ls`
do
    echo
    echo "Expected error:" `cat $i | grep //`
    expected_ret=`cat $i | grep // | cut -f2 -d'#'`
    ./../../parser $i > /dev/null
    ret=$?
    if [ $ret -eq 0 ]
        then
        echo -e "TEST ERROR: $i: $error || return code : $ret"
        error_num=$(( $error_num + 1 ))
    else
        if [ $expected_ret -eq $ret ]
        then
            echo -e "TEST ERROR: $i: $pass  || return code : $ret"
            pass_num=$(( $pass_num + 1 ))
        else
            echo -e "TEST ERROR: $i: $error || return code : $ret"
            error_num=$(( $error_num + 1 ))
        fi
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
        pass_num=$(( $pass_num + 1 ))
    else
        echo -e "TEST GOOD: $i: $error || return code : $ret"
        error_num=$(( $error_num + 1 ))
    fi
done

cd ../..


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
echo
echo
echo "Num of errors : $error_num"
echo "Num of passes : $pass_num"