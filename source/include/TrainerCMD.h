/* 
 * File:   TrainerCMD.h
 * Author: vernkin
 *
 * Created on April 29, 2009, 3:00 PM
 */

#ifndef _TRAINERCMD_H
#define	_TRAINERCMD_H

#include "cma_ctype.h"

#include "CMAPOCTagger.h"
#include "CMAPOSTagger.h"
#include <stdlib.h>
using namespace cma;

void beginTrain(string pMateFile, string cateFile, string enc, string posDelimiter);

void printTainerUsage();

int tainerEntry(int argc, char** argv);

#endif	/* _TRAINERCMD_H */

