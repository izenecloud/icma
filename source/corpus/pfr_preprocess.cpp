/** \file pfr_preprocess.cpp
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 24, 2009
 */

#include "pfr_preprocess.h"

#include <fstream>
#include <stdexcept>
#include <cassert>

using namespace std;

namespace cma
{

void PFR_Preprocess::format(const char* rawFile, const char* resultFile)
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

    // remove the phrase tags "[...]..."
    char ch;
    while(from.get(ch)) {
	if(ch == '[')
	    continue;
	if(ch == ']') {
	    while(ch != ' ') {
		from.get(ch);
	    }
	}
	to.put(ch);
    }
}

} // namespace cma
