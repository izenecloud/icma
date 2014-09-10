/** \file test_hit_preprocess.cpp
 * Test to process the corpus "HIT_哈工大".
 * That is, extract the segmented sentences, and remove the sequence tags and EOS tags.
 * An example of input and output is like below:
 * Input from raw file:
 *    我们即将动身。
 *    [1]我们/r [2]即将/d [3]动身/v [4]。/wp [5]<EOS>/<EOS>
 *    [3]动身_[2]即将  [3]动身_[1]我们  [5]<EOS>_[3]动身
 * Output to result file:
 *    我们/r 即将/d 动身/v 。/wp 
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 24, 2009
 */

#include "hit_preprocess.h"

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

    HIT_Preprocess preprocessor;

    preprocessor.format(argv[1], argv[2]);

    return 0;
}
