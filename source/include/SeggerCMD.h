/*! \mainpage Document of the Chinese Morphological Analyzer(Chen)
 *
 * Chinese Morphological Analyzer(Chen) is using Maxent Model and Character-baed
 * Segmentation to perform segmentation and pos tagging.
 *
 * \section compilefile Compile the file
 *
 * <ol>
 *   <li> Using shell, go to the project root directory.
 *   <li> Type &quot;.mkdir build &quot;.
 *   <li> Type &quot;.cd build &quot;.
 *   <li> Type &quot;.cmake ../source &quot;.
 *   <li> Finally Type  &quot;.make &quot;. to compile all the source.
 * </ol>
 *
 * If the external program uses the library, simply add all the header files
 * in the $include$ directory under the project root directory, and add the
 * build/cmac/libcmac.a into the library path.
 *
 * \section runtrainer Run the Trainer
 *
 * The dataset have to be trained by the Trainer. The Trainer is a executable 
 * file with name camctrainer under build/cmac.
 *
 * The SYNOPSIS for the trainer is: <BR>
 * ./cmactrainer mateFile cateFile [encoding] [posDelimiter] <BR>
 *
 * <BR>The Description for the parameters:
 * <ul>
 *    <li> mateFile is the material file, it should be in the form word1/pos1 
 * word2/pos2 word3/pos3 ...</li>
 *    <li> cateFile is the category file, there are several files are created
 * after the training, and with cateFile as the prefix, prefix should contains
 * both path and name, such /dir1/dir2/n1.</li>
 *    <li> encoding is the encoding of the mateFile, and gb2312 is the default
 * encoding. Only support gb2312 and big5 now.</li>
 *    <li> posDelimiter is the delimiter between the word and the pos tag, like
 * '/' and '_' and default is '/'.</li>
 * </ul>
 *
 * Take  &quot;/dir1/dir2/cate &quot; as the cateFile, after the training. The
 * following files are created (All under directory /dir1/dir2):
 * <ol>
 *  <li> cate.model is the POS statistical model file.</li>
 *  <li> cate.pos is the all the POS gained from the training dataset.</li>
 *  <li> cate.dic is the dictionary (include words and POS tags) gained from the
 * training dataset. This file is plain text and should be loaded as user
 * dictionary. To convert it to the system dictionary, use
 * Knowledge::encodeSystemDict(const char* txtFileName, const char* binFileName),
 * then the binFileName can be loaded as the system dictioanry.</li>
 *  <li>cate-poc.model is the POC statistical model file.</li>
 * </ol>
 *
 * All the files are required to run the program.<BR>
 *
 *
 * \section rundemo Run the Demo
 *
 * After the training, you can run the demo to segment the file, The Demo is a 
 * executable file with name camcsegger under build/cmac.
 *
 * The SYNOPSIS for the trainer is: <BR>
 * ./cmacsegger cateFile inFile outFile [encoding] [posDelimiter] <BR>
 *
 * <BR>The Description for the parameters:
 * <ul>
 *    <li> cateFile is the category file, there are several files are created
 * after the training, and with cateFile as the prefix, prefix should contains
 * both path and name, such /dir1/dir2/n1.</li>
 *    <li>inFile the input file.</li>
 *    <li>outFile the output file.</li>
 *    <li> encoding is the encoding of the mateFile, and gb2312 is the default
 * encoding. Only support gb2312 and big5 now.</li>
 *    <li> posDelimiter is the delimiter between the word and the pos tag, like
 * '/' and '_' and default is '/'.</li>
 * </ul>
 *
 * The result with pos tagging can be found in the outFile.
 */
 
 /*
 * \brief the CMD tool for the Segger
 * \file SeggerCMD.h
 * \author vernkin
 */

#ifndef _SEGGERCMD_H
#define	_SEGGERCMD_H

#include <iostream>

using namespace std;

#include "cma_factory.h"
#include "analyzer.h"
#include "knowledge.h"
#include "sentence.h"
#include "cma_ctype.h"

using namespace cma;

#include "VTimer.h"

/**
 * To segment the file
 */
void beginSeg(string mate, string inFile, string outFile, string enc,
        string posDelimiter);

int seggerEntry(int argc, char** argv);

void printSeggerUsage();

#endif	/* _SEGGERCMD_H */

