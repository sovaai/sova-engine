#include "RawHashTable.hpp"
#include <cstring>

RawHashTable::RawHashTable( unsigned int aInitialSize, double aCompletionRate )
{
    vInitialSize = aInitialSize;
    vCompletionRate = aCompletionRate;

    Init();
}

RawHashTable::~RawHashTable()
{
}

bool RawHashTable::IsOpen() const
{
    return vIsOpen;
}

void RawHashTable::Init()
{
    vColl = 0;
    vFilled = 0;

    vTable.resize( vInitialSize );
    if( vTable.no_memory() )
    {
        vIsOpen = false;
        return;
    }

    memset( vTable.get_buffer(), 0, sizeof( Node* ) * vInitialSize);
    vIsOpen = true;
}

void RawHashTable::Resize()
{
    vIsOpen = false;
    vColl = 0;
    vFilled = 0;

    vTable.resize( vTable.size() * 2 + 1 );
    if( vTable.no_memory() )
        return;

    memset( vTable.get_buffer(), 0, sizeof( Node* ) * vTable.size() );

    for( unsigned int node_n = 0; node_n < vNodes.size(); ++node_n )
    {
        unsigned int n = MakeN( vNodes[node_n]->vKey );
        NodeIterator *& aNodeIterator = vTable[n];

        NodeIterator * aNewIterator = nAllocateObject( vMemoryAllocator, NodeIterator );
        if( !aNewIterator )
            return;

        aNewIterator->vId = node_n;

        if( !aNodeIterator )
        {
            // Мы вставляем первый элемент в подсписок.
            aNewIterator->vNext = nullptr;
            ++vFilled;
        }
        else
        {
            // В подсписке уже есть элементы.
            aNewIterator->vNext = aNodeIterator;
            ++vColl;
        }
        aNodeIterator = aNewIterator;
    }

    vIsOpen = true;
}

void RawHashTable::Clear()
{
    if( !vIsOpen )
        return;

    vMemoryAllocator.Reset();
    vTable.clear();
    vNodes.clear();

    Init();
}

const unsigned int * RawHashTable::SearchInSubList(const NodeIterator & aNodeIterator, const unsigned long long & aHashCode, const char * aSrc, unsigned int aSize ) const
{
    const NodeIterator * aIter = &aNodeIterator;

    // Ищем в списке заданный элемент.
    while ( aIter )
    {
        const Node & aNode = *vNodes[aIter->vId];

        // Проверяем на совпадение длину элементов и хэш-коды.
        if( aHashCode == aNode.vKey )
        {
            // Проверяем на совпадение данные.
            unsigned int aBlockCnt = aSize >> 8;
            bool equal = true;
            for( unsigned int block_n = 0; block_n < aBlockCnt; ++block_n )
                if( *( reinterpret_cast<const long long*>( aSrc ) + block_n ) != *( reinterpret_cast<const long long*>( aNode.vData ) + block_n ) )
                {
                    equal = false;
                    break;
                }

            aBlockCnt <<= 3;
            if( equal )
            {
                if( !memcmp( aSrc + aBlockCnt, aNode.vData + aBlockCnt, aSize ) )
                    return &aIter->vId;
            }
        }

        // Переходим к следующему элементу в списке.
        aIter = aIter->vNext;
    }

    return nullptr;
}

const unsigned int * RawHashTable::InsertInSubList( NodeIterator *& aNodeIterator, const unsigned long long & aHashCode, const char * aSrc, unsigned int aSize, bool vCopyData  )
{
	if( aNodeIterator )
	{
    // Проверяем, не добавлялся ли данный элемент в подсписок ранее.
    const unsigned int * aId = SearchInSubList( *aNodeIterator, aHashCode, aSrc, aSize );
    if( aId )
        return aId;
	}

    // Выделяем память под данные и копируем их.
    char * aSrcBuffer;
    if( vCopyData )
    {
        aSrcBuffer = nAllocateObjects( vMemoryAllocator, char, aSize );
        if( !aSrcBuffer )
            return nullptr;
        memcpy( aSrcBuffer, aSrc, aSize );
    }

    // Создаём новый элемент хэш-таблицы.
    Node * aNewNode = nAllocateObject( vMemoryAllocator, Node );
    if( !aNewNode )
        return nullptr;
    new((nMemoryAllocatorElementPtr*)aNewNode) Node( aHashCode, vCopyData ? aSrcBuffer : aSrc, aSize, vCopyData );

    NodeIterator * aNewIterator = nAllocateObject( vMemoryAllocator, NodeIterator );
    if( !aNewIterator )
        return nullptr;

    // Добавляем новый элемент в общий список элементов.
    vNodes.push_back( aNewNode );
    if( vNodes.no_memory() )
        return nullptr;

    aNewIterator->vId = vNodes.size() - 1;
    if( !aNodeIterator )
    {
        // Мы вставляем первый элемент в подсписок.
        aNewIterator->vNext = nullptr;
        ++vFilled;
    }
    else
    {
        // В подсписке уже есть элементы.
        aNewIterator->vNext = aNodeIterator;
        ++vColl;
    }
    aNodeIterator = aNewIterator;

    return &aNewIterator->vId;
}

const unsigned int * RawHashTable::Insert( const char * aSrc, unsigned int aSize, bool aCopyData )
{
    if( !vIsOpen )
        return nullptr;

    // Проверяем, достигнут ли предел заполнения хэш-таблицы.
    if( (double)vFilled / vTable.size() > vCompletionRate)
        Resize();

    if( !vIsOpen )
        return nullptr;

    // Считаем хэш-код.
    MakeHash( aSrc, aSize, vTmpHash );

    // Выбираем ячейку хэш-таблицы.
    unsigned int n = MakeN( vTmpHash );

    // Вставляем и разрешаем при необходимости коллизию.
    return InsertInSubList( vTable[n], vTmpHash, aSrc, aSize, aCopyData );

}

const unsigned int * RawHashTable::Search( const char * aSrc, unsigned int aSize ) const
{
    if( !vIsOpen )
        return nullptr;

    // Считаем хэш-код.
    MakeHash( aSrc, aSize, vTmpHash );

    // Выбираем ячейку хэш-таблицы.
    unsigned int n = MakeN( vTmpHash );

    // Вставляем элемент и разрешаем при необходимости коллизию.
    return SearchInSubList( *vTable[n], vTmpHash, aSrc, aSize );

}

const char * RawHashTable::GetItem( unsigned int aId ) const
{
    return aId < vNodes.size() ? vNodes[aId]->vData : nullptr;
}

const unsigned int * RawHashTable::GetItemSize( unsigned int aId ) const
{
    return aId < vNodes.size() ? &vNodes[aId]->vSize : nullptr;
}

unsigned int RawHashTable::Count() const
{
    return vNodes.size();
}

unsigned int RawHashTable::GetCollistiosCount() const
{
    return vColl;
}

unsigned int RawHashTable::GetMaxListLen() const
{
    int max = 0;

    for( unsigned int list_n = 0; list_n < vTable.size(); ++list_n )
    {
        NodeIterator * aIter = vTable[list_n];
        if( aIter )
        {
            int len = 0;
            while ( aIter )
            {
                ++len;
                aIter = aIter->vNext;
            }
            max = max < len ? len : max;
        }
    }

    return max;
}

double RawHashTable::GetAvgListLen() const
{
    int sum = 0;
    int cnt = 0;

    for( unsigned int list_n = 0; list_n < vTable.size(); ++list_n )
    {
        NodeIterator * aIter = vTable[list_n];
        if( aIter )
        {
            int len = 0;
            ++cnt;
            while ( aIter )
            {
                ++len;
                aIter = aIter->vNext;
            }
            sum += len;
        }
    }

    return ((double)sum)/cnt;
}

void RawHashTable::MakeHash( const char * aSrc, unsigned int aSize, unsigned long long &  aHashCode ) const
{
//    Альтернативный способ вычисления хэш-кода.
//    MD5_CTX md5;
//    MD5Init( &md5 );
//    MD5Update( &md5, reinterpret_cast<const unsigned char *>( aSrc ), aSize );
//    unsigned long long buf[2];
//    MD5Final( reinterpret_cast<unsigned char*>( buf ), &md5 );
//    aHashCode = (buf[1] ^ buf[2]) ^ (aSize << 12);

    aHashCode = 0;

    unsigned int aBlockCnt = aSize / 8;
    for( unsigned int block_n = 0; block_n < aBlockCnt; ++block_n )
        aHashCode ^= *( reinterpret_cast<const unsigned long long*>(aSrc) + block_n );

    aBlockCnt *= 8;
    for( unsigned int byte_n = 0; byte_n < aSize % 8; ++byte_n)
        reinterpret_cast<unsigned char *>( &aHashCode )[byte_n] ^= aSrc[aBlockCnt + byte_n];
}

unsigned int RawHashTable::MakeN( const unsigned long long & aHashCode ) const
{
    return aHashCode % vTable.size();
}

