/** \file test_pfr_preprocess.cpp
 * Test to remove phrase tags and first word in each line in the corpus "PFR_199801_人民日报标注语料".
 * An example of input and output is like below:
 * Input from raw file:
 *	19980101-01-001-006/m  [中央/n 人民/n 广播/vn 电台/n]nt 
 * Output to result file:
 *	中央/n  人民/n  广播/vn  电台/n
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 24, 2009
 */

#include "pfr_preprocess.h"

#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace cma;

int main(int argc, char* argv[])
{
    if(argc < 3) {
    	cerr << "usage: ./a raw result" << endl;
    	exit(1);
    }

    PFR_Preprocess preprocessor;

    preprocessor.format(argv[1], argv[2]);

    return 0;
}
