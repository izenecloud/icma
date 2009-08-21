#!/bin/bash

#define variables
CORPUS=(as cityu msr pku)
CORPUS_LEN=${#CORPUS[*]}
destBegin="icwb/gb18030/icwbc"
i=0
while [ $i -lt $CORPUS_LEN ]; do
	corpora=${CORPUS[$i]}
	echo "To update dictionary in corpora ${corpora}"
	srcBegin=${corpora}/gb18030/${corpora}c

	cp ${destBegin}.black ${srcBegin}.black
	cp ${destBegin}.dic ${srcBegin}.dic
	cp ${destBegin}.dic.1 ${srcBegin}.dic.1
	
	let ++i;
done

echo "Training done!"
