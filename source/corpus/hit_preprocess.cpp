/** \file hit_preprocess.cpp
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 24, 2009
 */

#include "hit_preprocess.h"

#include <fstream>
#include <stdexcept>
#include <cassert>

using namespace std;

namespace cma
{

void HIT_Preprocess::format(const char* rawFile, const char* resultFile)
{
    assert(rawFile && resultFile);

    // open files
    ifstream from(rawFile);
    if(!from) {
	throw runtime_error(FILE_OPEN_ERROR + rawFile);
    }

    ofstream to(resultFile);
    if(!to) {
	throw runtime_error(FILE_OPEN_ERROR + resultFile);
    }

    string line;
    bool brace_start = false;

    // the 1st line is assumed as raw sentence like "我们即将动身。",
    // which is ignored.
    while(getline(from, line)) {
	// the 2nd line is assumed as segmented sentence like "[1]我们/r [2]即将/d [3]动身/v [4]。/wp [5]<EOS>/<EOS>",
	// this sentence is printed without the sequence tags and EOS tags,
	// so the output is like "我们/r 即将/d 动身/v 。/wp".
	getline(from, line);

	brace_start = false;
	for(unsigned int i=0; i<line.length(); ++i) {
	    // check sequence tag
	    if(line[i] == '[' || line[i] == ']') {
		    brace_start = !brace_start;
		    continue;
	    }

	    // check EOS tag
	    if(line[i] == '<')
		break;

	    if(brace_start == false)
		to.put(line[i]);
	}
	to.put('\n');

	// the 3rd line is assumed as word-relations like "[3]动身_[2]即将  [3]动身_[1]我们  [5]<EOS>_[3]动身",
	// which is ignored.
	getline(from, line);
    }
}

} // namespace cma
