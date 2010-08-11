/**
 * \file VGenericArray.h
 * \brief 
 * \date Aug 2, 2010
 * \author Vernkin Chen
 */

#ifndef VGENERICARRAY_H_
#define VGENERICARRAY_H_

#include <icma/util/MemAllocator.h>

namespace cma
{

template< typename T, class MemAllocator >
class GenericArray
{
public:
    GenericArray()
    {
        init();
    }

    GenericArray( size_t size )
    {
        init();
        reserve( size );
    }

    ~GenericArray()
    {
        MemAllocator::deleteArray( data_ );
    }

    inline size_t size() const
    {
        return endOffset_ - startOffset_;
    }

    inline bool empty() const
    {
        return endOffset_ == startOffset_;
    }

    inline void clear()
    {
        startOffset_ = 0;
        endOffset_ = 0;
    }

    inline size_t capacity() const
    {
        return size_;
    }

    inline size_t usedLen() const
    {
        return endOffset_;
    }

    inline size_t freeLen() const
    {
        return size_ - endOffset_;
    }

    void push_back( const T& t )
    {
        if( endOffset_ == size_ )
        {
            size_t newSize = (size_t)( size_ * 1.5 + 1 );
            reserve( newSize );
        }
        data_[ endOffset_ ] = t;
        ++endOffset_;
    }


    void removeHead()
    {
        if( startOffset_ == endOffset_ )
            return;
        ++startOffset_;
    }

    inline T& operator[]( int idx )
    {
        return data_[ startOffset_ + idx ];
    }

    inline T& operator[]( int idx ) const
    {
        return data_[ startOffset_ + idx ];
    }

    void swap( GenericArray< T, MemAllocator >& other )
    {
        T* tmpData = other.data_;
        other.data_ = data_;
        data_ = tmpData;

        size_t tmpSize = other.startOffset_;
        other.startOffset_ = startOffset_;
        startOffset_ = tmpSize;

        tmpSize = other.endOffset_;
        other.endOffset_ = endOffset_;
        endOffset_ = tmpSize;

        tmpSize = other.size_;
        other.size_ = size_;
        size_ = tmpSize;
    }

    void reserve( size_t size )
    {
        if( size <= size_ )
            return;
        if( size_ == 0 )
        {
            size_ = size;
            data_ = MemAllocator::createArray( size_ );
            startOffset_ = endOffset_ = 0;
            return;
        }

        size_t usedSize = endOffset_ - startOffset_;
        // startOffset_ with be set in the resize()
        data_ = MemAllocator::resize( data_, startOffset_, endOffset_, size );
        endOffset_ = startOffset_ + usedSize;
        size_ = size;
/*
        T* tmp = new T[ size ];
        size_t usedSize = endOffset_ - startOffset_;
        //memcpy( tmp, data_ + startOffset_, usedSize * sizeof( T ) );
        for( size_t i = 0; i < usedSize; ++i )
            tmp[ i ] = this->operator []( i );

        delete[] data_;
        data_ = tmp;
        startOffset_ = 0;
        endOffset_ = usedSize;
        size_ = size;
        */
    }

private:
    void init()
    {
        data_ = NULL;
        size_ = 0;
        startOffset_ = 0;
        endOffset_ = 0;
    }

public:
    T* data_;
    size_t startOffset_;
    size_t endOffset_;
    size_t size_;

};

/**
 * Normal class Generic Array
 */
template< typename T >
class VGenericArray : public GenericArray< T, NewAllocator<T> >
{
};

/**
 * Primitive Type Generic Array
 */
template< typename T>
class PGenericArray : public GenericArray< T, MAllocAllocator<T> >
{
};


}

#endif /* VGENERICARRAY_H_ */
