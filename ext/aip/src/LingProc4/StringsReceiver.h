#ifndef _STRINGSRECEIVER_H_
#define _STRINGSRECEIVER_H_

#include <_include/_inttype.h>

#include <lib/aptl/avector.h>

class StringsReceiver
{
public:
    StringsReceiver() : errorOccurred( false ) { }

public:
    virtual char *requestBuffer( size_t approximateLength ) = 0;
    virtual void commitString( size_t actualLength ) = 0;

    virtual bool hasErrorOccurred() const { return errorOccurred; }

    virtual ~StringsReceiver() {}

protected:
    bool errorOccurred;
};

template< int bufferSize >
class BufferReceiver : public StringsReceiver
{
public:
    BufferReceiver() : StringsReceiver(), length( 0 ) { }

public:
    virtual char *requestBuffer( size_t approximateLength )
    {
        if ( approximateLength > bufferSize )
        {
            errorOccurred = true;
            return 0;
        }

        return buffer;
    }

    virtual void commitString( size_t actualLength )
    {
        if ( actualLength < bufferSize )
            buffer[ actualLength ] = '\0';

        length = actualLength;
    }

    const char *getBuffer() const { return buffer; }
    size_t      size()      const { return length; }

protected:
    size_t length;
    char   buffer[ bufferSize ];
};

class MultipleStringsReceiver : public StringsReceiver
{
public:
    MultipleStringsReceiver() : StringsReceiver(), buffer(), markup(), threshold( 0 ) { }

public:
    virtual char *requestBuffer( size_t approximateLength )
    {
        buffer.resize( buffer.size() + approximateLength );
        if ( buffer.no_memory() )
            return 0;
        return buffer.get_buffer() + threshold;
    }

    virtual void commitString( size_t actualLength )
    {
        buffer.resize( threshold + actualLength + 1 );
        buffer[ threshold + actualLength ] = '\0';
        markup.push_back( threshold );
        threshold += actualLength + 1;
    }

    virtual bool hasErrorOccurred() const { return errorOccurred || buffer.no_memory() || markup.no_memory(); }

    size_t size() const { return markup.size(); }
    void   clear()      { buffer.clear(); markup.clear(); threshold = 0; }

    const char *operator[]( size_t element ) const
    {
        return buffer.get_buffer() + markup[ element ];
    }

    void pop()
    {
        if ( markup.size() > 0 )
        {
            threshold = markup[ markup.size() - 1 ];
            markup.pop_back();
            buffer.resize( threshold );
        }
    }

protected:
    avector<char>   buffer;
    avector<size_t> markup;
    size_t          threshold;
};

#endif /* _STRINGSRECEIVER_H_ */

