/*
 * \file word_bound.h
 * \brief the class to check whether a word possibly exists
 * \date Jun 2, 2010
 * \author Vernkin Chen
 */

#ifndef WORD_BOUND_H_
#define WORD_BOUND_H_

#include <vector>
#include <string>

class VTrie;

namespace cma
{

class Sentence;

/**
 * \brief the class to check whether a word possibly exists
 */
class WordBound
{
public:
    WordBound();
    ~WordBound();

    /**
     * Train the WordBound Model
     * \param iCMAModelPath the model Path of iCMA
     * \param dicts the dictionary to analysis
     * \param wbModelFile the output path of WordBound Model file
     * \return 0 for success, positive number for error counts
     */
    static int trainModel(
            const char* iCMAModelPath,
            const std::vector< std::string >& dicts,
            const char* wbModelFile
            );

    bool loadModel( const char* wbModelFile );

    void setMinScore( double score );

    double getMinScore();

    bool isPossibeWord( Sentence& sent );

private:
    double minScore_;
    VTrie* trie_;
};

}

#endif /* WORD_BOUND_H_ */
