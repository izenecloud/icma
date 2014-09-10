/**
 * \file MemAllocator.h
 * \brief 
 * \date Aug 9, 2010
 * \author Vernkin Chen
 */

#ifndef MEMALLOCATOR_H_
#define MEMALLOCATOR_H_
#include <cstdlib>
#include <cstdio>

namespace cma
{

template< typename T >
class NewAllocator
{
public:
    static T* createArray( size_t size )
    {
        return new T[ size ];
    }

    static void deleteArray( T* p )
    {
        delete[] p;
    }

    static T* resize(
            T* p,
            size_t& startIdx,
            size_t origUsedSize,
            size_t newSize
            )
    {
        T* ret = createArray( newSize );
        for( size_t i = startIdx; i < origUsedSize; ++i )
        {
            ret[ i - startIdx ] = p[ i ];
        }
        deleteArray( p );
        startIdx = 0;
        return ret;
    }
};


template< typename T >
class MAllocAllocator
{
public:


    static T* createArray( size_t size )
    {
        return (T*)malloc( size * sizeof(T) );
    }

    static void deleteArray( T* p )
    {
        free( p );
    }

    static T* resize(
            T* p,
            size_t& startIdx,
            size_t origUsedSize,
            size_t newSize
            )
    {
        return (T*)realloc( p, newSize * sizeof(T) );
    }

};

}

#endif /* MEMALLOCATOR_H_ */
