/*! \mainpage Document of the Chinese Morphological Analyzer(Chen)
 *
 * <b>CMA</b> (Chinese Morphological Analyzer) is a platform-independent C++ library for Chinese 
 * word segmentation and POS tagging. And Chinese Morphological Analyzer(Chen) is using Maxent Model and Character-baed
 * Segmentation to perform segmentation and POS tagging.
 *
 * \section compilefile Compile the file
 *
 * Two approaches can be used to compile. For the final user, the second approach is recommended.
 *
 * 1. Use the cmake directly.
 *
 * <ol>
 *   <li> Using shell, go to the project root directory.
 *   <li> Type &quot;.cd build &quot;.
 *   <li> Under linux, type &quot;cmake ../source&quot;; Under windows, run in the msys, type &quot;cmake -G &quot;Unix Makefiles&quot; ../source &quot;.
 *   <li> Finally Type  &quot;.make &quot;. to compile all the source.
 * </ol>
 *
 * 2. Use a script build/build.sh to compile. The usage is:<BR>
 *   <B>./build.sh [ release | debug | profile | clean ] [ linux | win32 ]</B><BR>
 *
 * The parameters contains four building types (default is <B>release</B>) and two platforms
 * (default is <B>linux</B>). For the building type, <B>debug</B> is compiled with debug information;
 * <B>release</B> is for release purpose with optimized code; <B>profile</B> will add -pg which
 * used by gprof; and <B>clean</B> will clean all temporal files. For the platform, <B>linux</B>
 * represents linux platform while <B>win32</B> represents windows platform.
 *
 *
 * If the external program uses the library, simply add all the header files
 * in the $include$ directory under the project root directory, and add the
 * lib/libcmac.a into the library for static linking or lib/libcmac.so for dynamic linking.
 *
 * \section runtrainer Run the Trainer
 *
 * The dataset have to be trained by the Trainer. The Trainer is a executable 
 * file with name camctrainer under directory bin.
 *
 * The SYNOPSIS for the trainer is: <BR>
 * ./cmactrainer pocXmlFile mateFile modelPath [encoding] [posDelimiter] [pos | poc | pos+poc] [large-corpus] <BR>
 *
 * <BR>The Description for the parameters:
 * <ul>
 *    <li> pocXmlFile is the path of Segmentation XML file in the associated
 * encoding. One sample is db/icwb/utf8/poc.xml (With utf8 encoding). </li>
 *    <li> mateFile  is the material file, the words separated by spaces and
 * maybe has POS information. For example, mateFile with POS information:
 * word1/pos1 word2/pos2...; mateFile without POS information: word1 word2....</li>
 *    <li> modelPath is the directory to hold all the output files (include
 * trained model files and dictionaries).</li>
 *    <li> encoding is the encoding of the mateFile, Default value is gb18030.
 * Support utf8, gb2312, gb18030 and big5 now. Recommend to use utf8/gb18030
 * to support both Simplified and Traditional Chinese.</li>
 *    <li> posDelimiter is the delimiter between the word and the pos tag.
 * Default value is '/'.Like '/' and '_'. If the taining model is poc(the
 * next parameter) and the mateFile doesn't contains POS information,
 * posDemiliter can be any single character (recommend to ues '/').</li>
 *    <li>pos | poc | pos+poc is the training mode. Default value is pos+poc.
 * poc only trains segmentation model, pos only trains POS tagging model,
 * and pos+poc trains both of them.</li>
 *    <li>large-corpus indicates whether the mateFile is large ( > 20MB).
 * Default value in none (don't add parameter large-corpus). some parameters
 * are adjusted for large mateFile.</li>
 * </ul>
 *
 * Take  &quot;/dir1/dir2 &quot; as the cateFile, after the training. The
 * following files are created (All under directory /dir1/dir2):
 * <ol>
 *  <li> <b>pos.model</b> is the POS statistical model file. Exists when
 * training mode is pos or pos+poc.</li>
 *  <li> <b>pos.pos</b> is the all the POS gained from the training dataset.
 * Exists when training mode is pos or pos+poc.</li>
 *  <li> <b>sys.dic</b> is the dictionary (include words and POS tags) gained from the
 * training dataset. This file is plain text and should be loaded as user
 * dictionary. To convert it to the system dictionary, use
 * Knowledge::encodeSystemDict(const char* txtFileName, const char* binFileName),
 * then the binFileName can be loaded as the system dictioanry. Exists when
 * training mode is pos or pos+poc.</li>
 *  <li> <b>poc.model</b> is the POC statistical model file. Exists when
 * training mode is poc or pos+poc.</li>
 * </ol>
 *
 * All the files are required to run the program.<BR>
 *
 * <b>Notice:</b> Configuration files ( cma.config, poc.xml and pos.config) are not
 * created automatically. You can copy these files with same names under
 * db/icwb/utf8 (If the encoding of mateFile isn't utf8, require to change
 *  the encoding of copied files).
 *
 * \section rundemo Run the Demo
 *
 * After the training, you can run the demo to segment the file, The Demo is a 
 * executable file with name camcsegger under directory bin.
 *
 * The SYNOPSIS for the trainer is: <BR>
 * ./cmacsegger modelPath inFile outFile [encoding] [posDelimiter] <BR>
 *
 * <BR>The Description for the parameters:
 * <ul>
 *    <li> modelPath is the directory contains all the tained models, dictionaries
 *    and configuration. </li>
 *    <li> inFile the input file.</li>
 *    <li> outFile the output file.</li>
 *    <li> encoding is the encoding of the mateFile, and gb2312 is the default
 * encoding. Support utf8, gb18030, gb2312 and big5.</li>
 *    <li> posDelimiter is the delimiter between the word and the pos tag, like
 * '/' and '_' and default is '/'.</li>
 * </ul>
 *
 * The result with pos tagging can be found in the outFile.
 
 \section useapi Use the API
 
 First of the all, add the lib/libcmac.a into the library path.
 
<i>Step 1: Include the header files in directory "include"</i>
\code
#include "cma_factory.h"
#include "analyzer.h"
#include "knowledge.h"
#include "sentence.h"
\endcode

<i>Step 2: Use the library name space</i>
\code
using namespace cma;
\endcode

<i>Step 3: Call the interface and handle the result</i>

\code
// create instances
CMA_Factory* factory = CMA_Factory::instance();
Analyzer* analyzer = factory->createAnalyzer();
Knowledge* knowledge = factory->createKnowledge();

//It is suggested to set encoding after crate the Knowledge. Another supported encode type is big5.
knowledge->setEncodeType(Knowledge::ENCODE_TYPE_GB2312);

// Load all the model by specific modelPath and encoding
knowledge->loadModel( "gb18030", "..." ).

// Load User Dictionaries and Stop Words if neccessary.
knowledge->loadUserDict("...");
knowledge->loadStopWordDict("...");
 
// (optional) if POS tagging is not needed, call the function below to turn off the analysis and 
// output for POS tagging, so that large execution time could be saved when execute 
// Analyzer::runWithSentence(), Analyzer::runWithString(), Analyzer::runWithStream().
analyzer->setOption(Analyzer::OPTION_TYPE_POS_TAGGING, 0);

// (optional) set the number of N-best results,
// if this function is not called, one-best analysis is performed defaultly on Analyzer::runWithSentence().
analyzer->setOption(Analyzer::OPTION_TYPE_NBEST, 5);

// set knowledge
analyzer->setKnowledge(knowledge);

// 1. analyze a paragraph
const char* result = analyzer->runWithString("...");
...

// 2. analyze a file
analyzer->runWithStream("...", "...");

// 3. split paragraphs into sentences
string line;
vector<Sentence> sentVec;
while(getline(cin, line)) // get paragraph string from standard input
{
    sentVec.clear(); // remove previous sentences
    analyzer->splitSentence(line.c_str(), sentVec);
    for(size_t i=0; i<sentVec.size(); ++i)
    {
        analyzer->runWithSentence(sentVec[i]); // analyze each sentence
        ...
    }
}

// destroy instances
delete knowledge;
delete analyzer;
\endcode

 
 */
 
 /** 
 * \brief the CMD tool for the Segger
 * \file SeggerCMD.h
 * \author vernkin
 */

#ifndef _SEGGERCMD_H
#define	_SEGGERCMD_H

#include <iostream>

using namespace std;

#include "icma/icma.h"

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

