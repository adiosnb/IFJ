#!/bin/bash

TESTA=$1
TESTB=$2

if [ $# -lt 2 ] 
then
	echo "SYNTAX: compare.sh FILE_A FILE_B"
	exit
fi

FILEA=`mktemp`
FILEB=`mktemp`

../scanner $TESTA > $FILEA
../scanner $TESTB > $FILEB

diff $FILEA $FILEB

 
