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
    bool lineStart = true;
    while(from.get(ch)) {
        if(ch == '\r')
            continue;

        // ignore the first word in each line
        if(lineStart) {
            while(ch != ' ' && from.get(ch)) ;

            while(ch == ' ' && from.get(ch)) ;

            lineStart = false;
        }

	if(ch == '[')
	    continue;
	if(ch == ']') {
            // ignore "]..."
	    while(ch != ' ' && from.get(ch)) ;
	}

	to.put(ch);

        if(ch == '\n')
            lineStart = true;
    }
}

} // namespace cma
