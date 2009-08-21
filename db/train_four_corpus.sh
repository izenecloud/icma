#!/bin/bash

#define variables
TRAIN_EXE="../bin/cmactrainer"
CORPUS=(as cityu msr pku)
CORPUS_LEN=${#CORPUS[*]}

i=0
while [ $i -lt $CORPUS_LEN ]; do
	corpora=${CORPUS[$i]}
	echo "To Train corpora ${corpora}"
	pathBegin=${corpora}/gb18030/${corpora}
	destFile=${pathBegin}_training.gb18030
	#convert to gb18030
	iconv -t gb18030 -f utf8 ${pathBegin}_training.utf8 -o  ${destFile}
	echo "Begin to train ${destFile}"
	${TRAIN_EXE} ${destFile} ${pathBegin}c gb18030 / --no-train-pos
	rm ${destFile}

	let ++i;
done

echo "All Training done!"
