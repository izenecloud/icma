/**
 * \file DCTrie.h
 * \brief 
 * \date Jul 26, 2010
 * \author Vernkin Chen
 */

#ifndef DCTRIE_H_
#define DCTRIE_H_

namespace cma
{
extern int DCTRIE_CODE[ 256 ];

class DCTrie
{
public:
    DCTrie();

    void insert( const char* str, int data );

    inline int search( const char* str )
    {
        if( *str == 0 )
            return 0;
        unsigned const char* ustr = (unsigned const char*)str;
        return data_[ DCTRIE_CODE[ *ustr ] ][ DCTRIE_CODE[ *(ustr + 1) ] ];
    }

private:
    int data_[ 27 ][ 27 ];
};

}

#endif /* DCTRIE_H_ */
