/** \file TrainerCMD.h
 *
 * \brief Command tool for the dataset training
 *
 * 
 * \author vernkin
 */

#ifndef _TRAINERCMD_H
#define	_TRAINERCMD_H

#include "CMAPOCTagger.h"
#include "CMAPOSTagger.h"

#include <stdlib.h>

using namespace cma;

/**
 * This function carry out the training
 *
 * \param pMateFile the meterial file
 * \param cateFile the category name
 * \param enc the encoding, only support gb2312 and big5
 * \param posDelimiter the pos delimiter
 */
void beginTrain(string pMateFile, string cateFile, string enc, string posDelimiter);

/**
 * Print the help information of the Trainer
 */
void printTainerUsage();

/**
 * The main function simply invoke this function
 *
 * \param argc the counter of the arguments
 * \param argv the content of the arguments
 * \return the exit value
 */
int tainerEntry(int argc, char** argv);

#endif	/* _TRAINERCMD_H */

