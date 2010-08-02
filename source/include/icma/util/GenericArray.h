/**
 * \file GenericArray.h
 * \brief 
 * \date Aug 2, 2010
 * \author Vernkin Chen
 */

#ifndef GENERICARRAY_H_
#define GENERICARRAY_H_

namespace cma
{

template<class T>
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
        delete data_;
    }

    inline size_t size()
    {
        return endOffset_ - startOffset_;
    }

    inline bool empty()
    {
        return endOffset_ == startOffset_;
    }

    void clear()
    {
        startOffset_ = 0;
        endOffset_ = 0;
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

    void swap( GenericArray<T>& other )
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


private:
    void init()
    {
        data_ = NULL;
        size_ = 0;
        startOffset_ = 0;
        endOffset_ = 0;
    }

    void reserve( size_t size )
    {
        if( size <= size_ )
            return;
        if( size_ == 0 )
        {
            size_ = size;
            data_ = new T[ size_ ];
            startOffset_ = endOffset_ = 0;
            return;
        }

        T* tmp = new T[ size ];
        size_t usedSize = endOffset_ - startOffset_;
        memcpy( tmp, data_ + startOffset_, usedSize * sizeof( T ) );
        delete data_;
        data_ = tmp;
        startOffset_ = 0;
        endOffset_ = usedSize;
        size_ = size;
    }

private:
    T* data_;
    size_t startOffset_;
    size_t endOffset_;
    size_t size_;
};

}

#endif /* GENERICARRAY_H_ */