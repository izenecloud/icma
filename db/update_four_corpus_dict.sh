#!/bin/bash

#define variables
CORPUS=(as cityu msr pku)
CORPUS_LEN=${#CORPUS[*]}
destBegin="icwb/gb18030/"
i=0
while [ $i -lt $CORPUS_LEN ]; do
	corpora=${CORPUS[$i]}
	echo "To update dictionary in corpora ${corpora}"
	srcBegin=${corpora}/

	#cp ${destBegin}.black ${srcBegin}.black
	cp ${destBegin}sys.dic ${srcBegin}sys.dic
	
	let ++i;
done

echo "Training done!"
