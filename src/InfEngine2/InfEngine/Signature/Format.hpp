#ifndef __Format_hpp__
#define __Format_hpp__

#include <cinttypes>
#include <sys/types.h>
#include <cstring>

namespace Format
{
const uint32_t CHECKSUM_SIZE = 16;

// Возвращает указатель на значение, находящееся по указанному смещению.
#define BY_POS( aPtr, aShift, aType ) \
    ( aPtr ? reinterpret_cast<aType>( reinterpret_cast<const char *>(aPtr) + (aShift) ) : 0 )

// Возвращает указатель на значение, смещение для которого указано по указанному смещению.
#define BY_SHIFT( aPtr, aShift, aType ) \
    ( aPtr ? reinterpret_cast<aType>( reinterpret_cast<const char *>(aPtr) + *reinterpret_cast<const uint32_t*>( reinterpret_cast<const char *>(aPtr) + (aShift) ) ) : 0 )

// Возвращает указатель на значение из списка, по указанному номеру в списке и смещению к списку.
#define BY_TABLE( aPtr, aShiftToTable, aNum, aType ) \
    ( aPtr ? BY_POS( aPtr, BY_SHIFT( aPtr, aShiftToTable, const uint32_t* )[aNum], aType ) : 0 )

// Базовый класс для доступа к скомпилированными данными, находящимся в памяти в плоском массиве.
class Manipulator
{
public:
    virtual ~Manipulator() {};

    Manipulator( const char * aPtr = 0 )
    { Set( aPtr ); }

    inline void Set( const char * aPtr )
    { vPtr = aPtr; }

    inline const char * GetPtr() const
    { return vPtr; }

    inline operator bool() const
    { return vPtr; }

protected:
    const char * vPtr;
};

// Элемент, который всегда находится по одному и тому же смещению.
#define ITEM( aPrevNum, aCurNum, aName, aType, aSize ) \
    private:\
    static const uint32_t SHIFT_TO_##aCurNum = SHIFT_TO_##aPrevNum + (aSize);\
    public:\
    inline aType Get##aName() const \
    { return BY_POS( vPtr, SHIFT_TO_##aPrevNum, aType ); }

// Элемент, для которого в плоском массиве смещение хранится всегда в одном и том же месте.
#define SHIFT( aPrevNum, aCurNum, aName, aType, aSize ) \
    private:\
    static const uint32_t SHIFT_TO_##aCurNum = SHIFT_TO_##aPrevNum + (aSize);\
    public:\
    inline aType Get##aName() const \
    { return BY_SHIFT( vPtr, SHIFT_TO_##aPrevNum, aType ); }

// Элемент таблицы, для которой в плоском массиве смещение всегда указывается в одном и том же месте.
#define TABLE( aPrevNum, aCurNum, aName, aType, aSize ) \
    private:\
    static const uint32_t SHIFT_TO_##aCurNum = SHIFT_TO_##aPrevNum + (aSize);\
    public:\
    inline aType Get##aName( size_t aNum ) const \
    { return BY_TABLE( vPtr, SHIFT_TO_##aPrevNum, aNum, aType ); }\
    
    

// Макрос для конструирования манипуляторов.
#define MANIPULATOR( aName, ... ) \
    class aName:public Manipulator\
    {\
    public:\
        virtual ~aName() {}\
        aName( const char * aPtr = 0 )\
            :Manipulator( aPtr )\
        {}\
        aName( const uint32_t * aPtr = 0 )\
            :Manipulator( reinterpret_cast<const char *>( aPtr ) )\
        {}\
    private:\
        static const uint32_t SHIFT_TO_0 = 0;\
        __VA_ARGS__\
    };


#define ITEM1(  aName, aType, aSize ) ITEM(  0,  1,  aName, aType, aSize )
#define ITEM2(  aName, aType, aSize ) ITEM(  1,  2,  aName, aType, aSize )
#define ITEM3(  aName, aType, aSize ) ITEM(  2,  3,  aName, aType, aSize )
#define ITEM4(  aName, aType, aSize ) ITEM(  3,  4,  aName, aType, aSize )
#define ITEM5(  aName, aType, aSize ) ITEM(  4,  5,  aName, aType, aSize )
#define ITEM6(  aName, aType, aSize ) ITEM(  5,  6,  aName, aType, aSize )
#define ITEM7(  aName, aType, aSize ) ITEM(  6,  7,  aName, aType, aSize )
#define ITEM8(  aName, aType, aSize ) ITEM(  7,  8,  aName, aType, aSize )
#define ITEM9(  aName, aType, aSize ) ITEM(  8,  9,  aName, aType, aSize )
#define ITEM10( aName, aType, aSize ) ITEM(  9, 10, aName, aType, aSize )
#define ITEM11( aName, aType, aSize ) ITEM( 10, 11, aName, aType, aSize )
#define ITEM12( aName, aType, aSize ) ITEM( 11, 12, aName, aType, aSize )
#define ITEM13( aName, aType, aSize ) ITEM( 12, 13, aName, aType, aSize )
#define ITEM14( aName, aType, aSize ) ITEM( 13, 14, aName, aType, aSize )
#define ITEM15( aName, aType, aSize ) ITEM( 14, 15, aName, aType, aSize )

#define SHIFT1( aName, aType, aSize ) SHIFT(   0,  1, aName, aType, aSize )
#define SHIFT2( aName, aType, aSize ) SHIFT(   1,  2, aName, aType, aSize )
#define SHIFT3( aName, aType, aSize ) SHIFT(   2,  3, aName, aType, aSize )
#define SHIFT4( aName, aType, aSize ) SHIFT(   3,  4, aName, aType, aSize )
#define SHIFT5( aName, aType, aSize ) SHIFT(   4,  5, aName, aType, aSize )
#define SHIFT6( aName, aType, aSize ) SHIFT(   5,  6, aName, aType, aSize )
#define SHIFT7( aName, aType, aSize ) SHIFT(   6,  7, aName, aType, aSize )
#define SHIFT8( aName, aType, aSize ) SHIFT(   7,  8, aName, aType, aSize )
#define SHIFT9( aName, aType, aSize ) SHIFT(   8,  9, aName, aType, aSize )
#define SHIFT10( aName, aType, aSize ) SHIFT(  9, 10, aName, aType, aSize )
#define SHIFT11( aName, aType, aSize ) SHIFT( 10, 11, aName, aType, aSize )
#define SHIFT12( aName, aType, aSize ) SHIFT( 11, 12, aName, aType, aSize )
#define SHIFT13( aName, aType, aSize ) SHIFT( 12, 13, aName, aType, aSize )
#define SHIFT14( aName, aType, aSize ) SHIFT( 13, 14, aName, aType, aSize )
#define SHIFT15( aName, aType, aSize ) SHIFT( 14, 15, aName, aType, aSize )

#define TABLE1( aName, aType, aSize ) TABLE(   0,  1, aName, aType, aSize )
#define TABLE2( aName, aType, aSize ) TABLE(   1,  2, aName, aType, aSize )
#define TABLE3( aName, aType, aSize ) TABLE(   2,  3, aName, aType, aSize )
#define TABLE4( aName, aType, aSize ) TABLE(   3,  4, aName, aType, aSize )
#define TABLE5( aName, aType, aSize ) TABLE(   4,  5, aName, aType, aSize )
#define TABLE6( aName, aType, aSize ) TABLE(   5,  6, aName, aType, aSize )
#define TABLE7( aName, aType, aSize ) TABLE(   6,  7, aName, aType, aSize )
#define TABLE8( aName, aType, aSize ) TABLE(   7,  8, aName, aType, aSize )
#define TABLE9( aName, aType, aSize ) TABLE(   8,  9, aName, aType, aSize )
#define TABLE10( aName, aType, aSize ) TABLE(  9, 10, aName, aType, aSize )
#define TABLE11( aName, aType, aSize ) TABLE( 10, 11, aName, aType, aSize )
#define TABLE12( aName, aType, aSize ) TABLE( 11, 12, aName, aType, aSize )
#define TABLE13( aName, aType, aSize ) TABLE( 12, 13, aName, aType, aSize )
#define TABLE14( aName, aType, aSize ) TABLE( 13, 14, aName, aType, aSize )
#define TABLE15( aName, aType, aSize ) TABLE( 14, 15, aName, aType, aSize )


// Одно из полей заголовка некоторого объекта.
#define FIELD( aPrev, aCur , aName, aType, aSize ) \
    public:\
        aType const aName;\
    private:\
        static const size_t FIELD_SHIFT_##aCur = FIELD_SHIFT_##aPrev + aSize;

// Инструкция инициализации поля заголовка.
#define FLD_INIT( aNum, aName, aType ) \
    aName( reinterpret_cast<aType>( buffer + FIELD_SHIFT_##aNum ) )

// Инструкция инициализации поля заголовка.
#define FIELD_ITEM1( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_ITEM2( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_ITEM3( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_ITEM4( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_ITEM5( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_ITEM6( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_ITEM7( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_ITEM8( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_ITEM9( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_ITEM10( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_ITEM11( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_ITEM12( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_ITEM13( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_ITEM14( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_ITEM15( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )

// Инструкция инициализации поля заголовка.
#define FIELD_SHIFT1( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_SHIFT2( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_SHIFT3( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_SHIFT4( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_SHIFT5( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_SHIFT6( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_SHIFT7( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_SHIFT8( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_SHIFT9( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_SHIFT10( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_SHIFT11( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_SHIFT12( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_SHIFT13( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_SHIFT14( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )
#define FIELD_SHIFT15( aPrev, aCur , aName, aType, aSize ) FIELD( aPrev, aCur , aName, aType, aSize )


// Методы для работы с таблицей.
#define TABLE_METHODS( aName, aType, aSize )\
    public:\
        inline void InitTable_##aName( size_t aRowsCount, size_t aShift ) \
        {\
            *aName = aShift;\
            *aName##Number = aRowsCount;\
        }\
        inline unsigned int SizeOfTable_##aName() const\
        { return *aName##Number * aSize; }\
        static inline unsigned int SizeOfTable_##aName( size_t aRowsCount )\
        { return aRowsCount * aSize; }\
        inline void AssignTableRow_##aName( size_t aRowN, size_t aShift )\
        { reinterpret_cast<aType>(buffer+*aName)[aRowN] = aShift; }\

// Инструкция инициализации поля заголовка и методов для работы с ним.
#define FIELD_TABLE1( aPrev, aCur , aName, aType, aSize ) \
    FIELD( aPrev, aCur , aName, aType, aSize ) \
    TABLE_METHODS( aName, aType, aSize )\

#define FIELD_TABLE2( aPrev, aCur , aName, aType, aSize ) \
    FIELD( aPrev, aCur , aName, aType, aSize ) \
    TABLE_METHODS( aName, aType, aSize )\
    
#define FIELD_TABLE3( aPrev, aCur , aName, aType, aSize ) \
    FIELD( aPrev, aCur , aName, aType, aSize ) \
    TABLE_METHODS( aName, aType, aSize )\
    
#define FIELD_TABLE4( aPrev, aCur , aName, aType, aSize ) \
    FIELD( aPrev, aCur , aName, aType, aSize ) \
    TABLE_METHODS( aName, aType, aSize )\
    
#define FIELD_TABLE5( aPrev, aCur , aName, aType, aSize ) \
    FIELD( aPrev, aCur , aName, aType, aSize ) \
    TABLE_METHODS( aName, aType, aSize )\
    
#define FIELD_TABLE6( aPrev, aCur , aName, aType, aSize ) \
    FIELD( aPrev, aCur , aName, aType, aSize ) \
    TABLE_METHODS( aName, aType, aSize )\
    
#define FIELD_TABLE7( aPrev, aCur , aName, aType, aSize ) \
    FIELD( aPrev, aCur , aName, aType, aSize ) \
    TABLE_METHODS( aName, aType, aSize )\
    
#define FIELD_TABLE8( aPrev, aCur , aName, aType, aSize ) \
    FIELD( aPrev, aCur , aName, aType, aSize ) \
    TABLE_METHODS( aName, aType, aSize )\
    
#define FIELD_TABLE9( aPrev, aCur , aName, aType, aSize ) \
    FIELD( aPrev, aCur , aName, aType, aSize ) \
    TABLE_METHODS( aName, aType, aSize )\
    
#define FIELD_TABLE10( aPrev, aCur , aName, aType, aSize ) \
    FIELD( aPrev, aCur , aName, aType, aSize ) \
    TABLE_METHODS( aName, aType, aSize )\
    
#define FIELD_TABLE11( aPrev, aCur , aName, aType, aSize ) \
    FIELD( aPrev, aCur , aName, aType, aSize ) \
    TABLE_METHODS( aName, aType, aSize )\
    
#define FIELD_TABLE12( aPrev, aCur , aName, aType, aSize ) \
    FIELD( aPrev, aCur , aName, aType, aSize ) \
    TABLE_METHODS( aName, aType, aSize )\
    
#define FIELD_TABLE13( aPrev, aCur , aName, aType, aSize ) \
    FIELD( aPrev, aCur , aName, aType, aSize ) \
    TABLE_METHODS( aName, aType, aSize )\
    
#define FIELD_TABLE14( aPrev, aCur , aName, aType, aSize ) \
    FIELD( aPrev, aCur , aName, aType, aSize ) \
    TABLE_METHODS( aName, aType, aSize )\
    
#define FIELD_TABLE15( aPrev, aCur , aName, aType, aSize ) \
    FIELD( aPrev, aCur , aName, aType, aSize ) \
    TABLE_METHODS( aName, aType, aSize )\
    

// Инструкция инициализации поля заголовка и меьодов для работы с ним.
#define FIELD_EXT(  aPrev, aCur , aName, aType, aSize, aKind ) \
    FIELD_##aKind(  aPrev, aCur , aName, aType, aSize )

#define FIELD1( aName, aType, aSize ) FIELD( 0, 1, aName, aType, aSize)
#define FIELD2( aName, aType, aSize ) FIELD( 1, 2, aName, aType, aSize)
#define FIELD3( aName, aType, aSize ) FIELD( 2, 3, aName, aType, aSize)
#define FIELD4( aName, aType, aSize ) FIELD( 3, 4, aName, aType, aSize)
#define FIELD5( aName, aType, aSize ) FIELD( 4, 5, aName, aType, aSize)
#define FIELD6( aName, aType, aSize ) FIELD( 5, 6, aName, aType, aSize)
#define FIELD7( aName, aType, aSize ) FIELD( 6, 7, aName, aType, aSize)
#define FIELD8( aName, aType, aSize ) FIELD( 7, 8, aName, aType, aSize)
#define FIELD9( aName, aType, aSize ) FIELD( 8, 9, aName, aType, aSize)
#define FIELD10( aName, aType, aSize ) FIELD( 9, 10, aName, aType, aSize)
#define FIELD11( aName, aType, aSize ) FIELD( 10, 11, aName, aType, aSize)
#define FIELD12( aName, aType, aSize ) FIELD( 11, 12, aName, aType, aSize)
#define FIELD13( aName, aType, aSize ) FIELD( 12, 13, aName, aType, aSize)
#define FIELD14( aName, aType, aSize ) FIELD( 13, 14, aName, aType, aSize)
#define FIELD15( aName, aType, aSize ) FIELD( 14, 15, aName, aType, aSize)

#define FIELD1_EXT( aKind, aName, aType, aSize ) FIELD_EXT( 0, 1, aName, aType, aSize, aKind )
#define FIELD2_EXT( aKind, aName, aType, aSize ) FIELD_EXT( 1, 2, aName, aType, aSize, aKind )
#define FIELD3_EXT( aKind, aName, aType, aSize ) FIELD_EXT( 2, 3, aName, aType, aSize, aKind )
#define FIELD4_EXT( aKind, aName, aType, aSize ) FIELD_EXT( 3, 4, aName, aType, aSize, aKind )
#define FIELD5_EXT( aKind, aName, aType, aSize ) FIELD_EXT( 4, 5, aName, aType, aSize, aKind )
#define FIELD6_EXT( aKind, aName, aType, aSize ) FIELD_EXT( 5, 6, aName, aType, aSize, aKind )
#define FIELD7_EXT( aKind, aName, aType, aSize ) FIELD_EXT( 6, 7, aName, aType, aSize, aKind )
#define FIELD8_EXT( aKind, aName, aType, aSize ) FIELD_EXT( 7, 8, aName, aType, aSize, aKind )
#define FIELD9_EXT( aKind, aName, aType, aSize ) FIELD_EXT( 8, 9, aName, aType, aSize, aKind )
#define FIELD10_EXT( aKind, aName, aType, aSize ) FIELD_EXT( 9, 10, aName, aType, aSize, aKind )
#define FIELD11_EXT( aKind, aName, aType, aSize ) FIELD_EXT( 10, 11, aName, aType, aSize, aKind )
#define FIELD12_EXT( aKind, aName, aType, aSize ) FIELD_EXT( 11, 12, aName, aType, aSize, aKind )
#define FIELD13_EXT( aKind, aName, aType, aSize ) FIELD_EXT( 12, 13, aName, aType, aSize, aKind )
#define FIELD14_EXT( aKind, aName, aType, aSize ) FIELD_EXT( 13, 14, aName, aType, aSize, aKind )
#define FIELD15_EXT( aKind, aName, aType, aSize ) FIELD_EXT( 14, 15, aName, aType, aSize, aKind )

#define FLD_INIT1( aName, aType ) ,FLD_INIT( 0, aName, aType)
#define FLD_INIT2( aName, aType ) ,FLD_INIT( 1, aName, aType)
#define FLD_INIT3( aName, aType ) ,FLD_INIT( 2, aName, aType)
#define FLD_INIT4( aName, aType ) ,FLD_INIT( 3, aName, aType)
#define FLD_INIT5( aName, aType ) ,FLD_INIT( 4, aName, aType)
#define FLD_INIT6( aName, aType ) ,FLD_INIT( 5, aName, aType)
#define FLD_INIT7( aName, aType ) ,FLD_INIT( 6, aName, aType)
#define FLD_INIT8( aName, aType ) ,FLD_INIT( 7, aName, aType)
#define FLD_INIT9( aName, aType ) ,FLD_INIT( 8, aName, aType)
#define FLD_INIT10( aName, aType ) ,FLD_INIT( 9, aName, aType)
#define FLD_INIT11( aName, aType ) ,FLD_INIT( 10, aName, aType)
#define FLD_INIT12( aName, aType ) ,FLD_INIT( 11, aName, aType)
#define FLD_INIT13( aName, aType ) ,FLD_INIT( 12, aName, aType)
#define FLD_INIT14( aName, aType ) ,FLD_INIT( 13, aName, aType)
#define FLD_INIT15( aName, aType ) ,FLD_INIT( 14, aName, aType)

// Макрос для конструирования заголовков для скомпилированных объектов.
#define HEADER( aNum, aName, aFields, aConstructor )\
    class aName\
    {\
        private:\
            char * buffer;\
            static const size_t FIELD_SHIFT_0 = 0;\
        public:\
            aFields\
        public:\
            static const size_t HeaderSize = FIELD_SHIFT_##aNum;\
        public:\
            aName( char * aBuffer = nullptr )\
                :buffer( aBuffer? aBuffer : vBuffer )\
                aConstructor\
                { memset( buffer, 0, FIELD_SHIFT_##aNum ); }\
            inline const char * GetData() { return buffer; }\
        private:\
            char vBuffer[FIELD_SHIFT_##aNum];\
    };



#define HEADER_FIELDS_0


#define HEADER_FIELDS_1( aKind1, aName1, aType1, aSize1 ) \
    HEADER_FIELDS_0\
    FIELD1_EXT( aKind1, aName1, aType1, aSize1 )


#define HEADER_FIELDS_2( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2 ) \
    HEADER_FIELDS_1( aKind1, aName1, aType1, aSize1 )\
    FIELD2_EXT( aKind2, aName2, aType2, aSize2 )


#define HEADER_FIELDS_3( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3 ) \
    HEADER_FIELDS_2( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2 ) \
    FIELD3_EXT( aKind3, aName3, aType3, aSize3 )


#define HEADER_FIELDS_4( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                         aKind4, aName4, aType4, aSize4 ) \
    HEADER_FIELDS_3( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3 ) \
    FIELD4_EXT( aKind4, aName4, aType4, aSize4 )


#define HEADER_FIELDS_5( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                         aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5 ) \
    HEADER_FIELDS_4( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                     aKind4, aName4, aType4, aSize4 ) \
    FIELD5_EXT( aKind5, aName5, aType5, aSize5 )


#define HEADER_FIELDS_6( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                         aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6 ) \
    HEADER_FIELDS_5( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                     aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5 ) \
    FIELD6_EXT( aKind6, aName6, aType6, aSize6 )


#define HEADER_FIELDS_7( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                         aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                         aName7, aType7, aSize7 ) \
    HEADER_FIELDS_6( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                     aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6 ) \
    FIELD7( aName7, aType7, aSize7 )


#define HEADER_FIELDS_8( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                         aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                         aName7, aType7, aSize7, aName8, aType8, aSize8 ) \
    HEADER_FIELDS_7( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                     aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                     aName7, aType7, aSize7 ) \
    FIELD8( aName8, aType8, aSize8 )


#define HEADER_FIELDS_9( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                         aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                         aName7, aType7, aSize7, aName8, aType8, aSize8, aName9, aType9, aSize9 ) \
    HEADER_FIELDS_8( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                     aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                     aName7, aType7, aSize7, aName8, aType8, aSize8 ) \
    FIELD9( aName9, aType9, aSize9 )


#define HEADER_FIELDS_10( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                          aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                          aName7, aType7, aSize7, aName8, aType8, aSize8, aName9, aType9, aSize9, \
                          aName10, aType10, aSize10 ) \
    HEADER_FIELDS_9( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                     aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                     aName7, aType7, aSize7, aName8, aType8, aSize8, aName9, aType9, aSize9 ) \
    FIELD10( aName10, aType10, aSize10 )


#define HEADER_FIELDS_11( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                          aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                          aName7, aType7, aSize7, aName8, aType8, aSize8, aName9, aType9, aSize9, \
                          aName10, aType10, aSize10, aName11, aType11, aSize11 ) \
    HEADER_FIELDS_10( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                      aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                      aName7, aType7, aSize7, aName8, aType8, aSize8, aName9, aType9, aSize9, \
                      aName10, aType10, aSize10 ) \
    FIELD11( aName11, aType11, aSize11 )


#define HEADER_FIELDS_12( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                          aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                          aName7, aType7, aSize7, aName8, aType8, aSize8, aName9, aType9, aSize9, \
                          aName10, aType10, aSize10, aName11, aType11, aSize11, aName12, aType12, aSize12 ) \
    HEADER_FIELDS_11( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                      aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                      aName7, aType7, aSize7, aName8, aType8, aSize8, aName9, aType9, aSize9, \
                      aName10, aType10, aSize10, aName11, aType11, aSize11 ) \
    FIELD12( aName12, aType12, aSize12 )


#define HEADER_FIELDS_13( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                          aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                          aName7, aType7, aSize7, aName8, aType8, aSize8, aName9, aType9, aSize9, \
                          aName10, aType10, aSize10, aName11, aType11, aSize11, aName12, aType12, aSize12, \
                          aName13, aType13, aSize13) \
    HEADER_FIELDS_12( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                      aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                      aName7, aType7, aSize7, aName8, aType8, aSize8, aName9, aType9, aSize9, \
                      aName10, aType10, aSize10, aName11, aType11, aSize11, aName12, aType12, aSize12 ) \
    FIELD13( aName13, aType13, aSize13 )

#define HEADER_FIELDS_14( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                          aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                          aName7, aType7, aSize7, aName8, aType8, aSize8, aName9, aType9, aSize9, \
                          aName10, aType10, aSize10, aName11, aType11, aSize11, aName12, aType12, aSize12, \
                          aName13, aType13, aSize13, aName14, aType14, aSize14) \
    HEADER_FIELDS_13( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                      aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                      aName7, aType7, aSize7, aName8, aType8, aSize8, aName9, aType9, aSize9, \
                      aName10, aType10, aSize10, aName11, aType11, aSize11, aName12, aType12, aSize12, \
                      aName13, aType13, aSize13) \
    FIELD14( aName14, aType14, aSize14 )

#define HEADER_FIELDS_15( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                          aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                          aName7, aType7, aSize7, aName8, aType8, aSize8, aName9, aType9, aSize9, \
                          aName10, aType10, aSize10, aName11, aType11, aSize11, aName12, aType12, aSize12, \
                          aName13, aType13, aSize13, aName14, aType14, aSize14, aName15, aType15, aSize15) \
    HEADER_FIELDS_14( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                      aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                      aName7, aType7, aSize7, aName8, aType8, aSize8, aName9, aType9, aSize9, \
                      aName10, aType10, aSize10, aName11, aType11, aSize11, aName12, aType12, aSize12, \
                      aName13, aType13, aSize13, aName14, aType14, aSize14) \
    FIELD15( aName15, aType15, aSize15 )



#define HEADER_FLD_INIT_0


#define HEADER_FLD_INIT_1( aName1, aType1 ) \
    HEADER_FLD_INIT_0\
    FLD_INIT1( aName1, aType1 )


#define HEADER_FLD_INIT_2( aName1, aType1, aName2, aType2 ) \
    HEADER_FLD_INIT_1( aName1, aType1 ) \
    FLD_INIT2( aName2, aType2 )


#define HEADER_FLD_INIT_3( aName1, aType1, aName2, aType2, aName3, aType3 ) \
    HEADER_FLD_INIT_2( aName1, aType1, aName2, aType2 ) \
    FLD_INIT3( aName3, aType3 )


#define HEADER_FLD_INIT_4( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4 ) \
    HEADER_FLD_INIT_3( aName1, aType1, aName2, aType2, aName3, aType3 ) \
    FLD_INIT4( aName4, aType4 )


#define HEADER_FLD_INIT_5( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5 ) \
    HEADER_FLD_INIT_4( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4 ) \
    FLD_INIT5( aName5, aType5 )


#define HEADER_FLD_INIT_6( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6 ) \
    HEADER_FLD_INIT_5( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5 ) \
    FLD_INIT6( aName6, aType6 )


#define HEADER_FLD_INIT_7( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                           aName7, aType7 ) \
    HEADER_FLD_INIT_6( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6 ) \
    FLD_INIT7( aName7, aType7 )


#define HEADER_FLD_INIT_8( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                           aName7, aType7, aName8, aType8 ) \
    HEADER_FLD_INIT_7( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                       aName7, aType7 ) \
    FLD_INIT8( aName8, aType8 )


#define HEADER_FLD_INIT_9( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                           aName7, aType7, aName8, aType8, aName9, aType9 ) \
    HEADER_FLD_INIT_8( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                       aName7, aType7, aName8, aType8 ) \
    FLD_INIT9( aName9, aType9 )


#define HEADER_FLD_INIT_10( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                            aName7, aType7, aName8, aType8, aName9, aType9, aName10, aType10  ) \
    HEADER_FLD_INIT_9( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                       aName7, aType7, aName8, aType8, aName9, aType9 ) \
    FLD_INIT10( aName10, aType10 )

#define HEADER_FLD_INIT_11( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                            aName7, aType7, aName8, aType8, aName9, aType9, aName10, aType10, aName11, aType11  ) \
    HEADER_FLD_INIT_10( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                        aName7, aType7, aName8, aType8, aName9, aType9, aName10, aType10  ) \
    FLD_INIT11( aName11, aType11 )

#define HEADER_FLD_INIT_12( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                            aName7, aType7, aName8, aType8, aName9, aType9, aName10, aType10, aName11, aType11, aName12, aType12  ) \
    HEADER_FLD_INIT_11( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                        aName7, aType7, aName8, aType8, aName9, aType9, aName10, aType10, aName11, aType11  ) \
    FLD_INIT12( aName12, aType12 )

#define HEADER_FLD_INIT_13( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                            aName7, aType7, aName8, aType8, aName9, aType9, aName10, aType10, aName11, aType11, aName12, aType12, \
                            aName13, aType13) \
    HEADER_FLD_INIT_12( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                        aName7, aType7, aName8, aType8, aName9, aType9, aName10, aType10, aName11, aType11, aName12, aType12  ) \
    FLD_INIT13( aName13, aType13 )

#define HEADER_FLD_INIT_14( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                            aName7, aType7, aName8, aType8, aName9, aType9, aName10, aType10, aName11, aType11, aName12, aType12, \
                            aName13, aType13, aName14, aType14) \
    HEADER_FLD_INIT_13( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                        aName7, aType7, aName8, aType8, aName9, aType9, aName10, aType10, aName11, aType11, aName12, aType12, \
                        aName13, aType13) \
    FLD_INIT14( aName14, aType14 )

#define HEADER_FLD_INIT_15( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                            aName7, aType7, aName8, aType8, aName9, aType9, aName10, aType10, aName11, aType11, aName12, aType12, \
                            aName13, aType13, aName14, aType14, aName15, aType15) \
    HEADER_FLD_INIT_14( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                        aName7, aType7, aName8, aType8, aName9, aType9, aName10, aType10, aName11, aType11, aName12, aType12, \
                        aName13, aType13, aName14, aType14) \
    FLD_INIT15( aName15, aType15 )





#define MANIPULATOR_FIELDS_0


#define MANIPULATOR_FIELDS_1( aKind1, aName1, aType1, aSize1 ) \
    MANIPULATOR_FIELDS_0\
    aKind1( aName1, const aType1, aSize1 )


#define MANIPULATOR_FIELDS_2( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2 ) \
    MANIPULATOR_FIELDS_1( aKind1, aName1, aType1, aSize1 ) \
    aKind2( aName2, const aType2, aSize2 )


#define MANIPULATOR_FIELDS_3( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3  ) \
    MANIPULATOR_FIELDS_2( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2 ) \
    aKind3( aName3, const aType3, aSize3 )


#define MANIPULATOR_FIELDS_4( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                              aKind4, aName4, aType4, aSize4 ) \
    MANIPULATOR_FIELDS_3( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3  ) \
    aKind4( aName4, const aType4, aSize4 )


#define MANIPULATOR_FIELDS_5( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                              aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5 ) \
    MANIPULATOR_FIELDS_4( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                          aKind4, aName4, aType4, aSize4 ) \
    aKind5( aName5, const aType5, aSize5 )


#define MANIPULATOR_FIELDS_6( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                              aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6 ) \
    MANIPULATOR_FIELDS_5( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                          aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5 ) \
    aKind6( aName6, const aType6, aSize6 )


#define MANIPULATOR_FIELDS_7( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                              aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                              aKind7, aName7, aType7, aSize7 ) \
    MANIPULATOR_FIELDS_6( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                          aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6 ) \
    aKind7( aName7, const aType7, aSize7 )


#define MANIPULATOR_FIELDS_8( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                              aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                              aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8 ) \
    MANIPULATOR_FIELDS_7( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                          aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                          aKind7, aName7, aType7, aSize7 ) \
    aKind8( aName8, const aType8, aSize8 )


#define MANIPULATOR_FIELDS_9( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                              aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                              aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9 ) \
    MANIPULATOR_FIELDS_8( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                          aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                          aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8 ) \
    aKind9( aName9, const aType9, aSize9 )


#define MANIPULATOR_FIELDS_10( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                               aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                               aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9, \
                               aKind10, aName10, aType10, aSize10) \
    MANIPULATOR_FIELDS_9( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                          aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                          aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9 ) \
    aKind10( aName10, const aType10, aSize10 )


#define MANIPULATOR_FIELDS_11( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                               aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                               aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9, \
                               aKind10, aName10, aType10, aSize10, aKind11, aName11, aType11, aSize11 ) \
    MANIPULATOR_FIELDS_10( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                           aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                           aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9, \
                           aKind10, aName10, aType10, aSize10 ) \
    aKind11( aName11, const aType11, aSize11 )


#define MANIPULATOR_FIELDS_12( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                               aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                               aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9, \
                               aKind10, aName10, aType10, aSize10, aKind11, aName11, aType11, aSize11, aKind12, aName12, aType12, aSize12 ) \
    MANIPULATOR_FIELDS_11( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                           aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                           aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9, \
                           aKind10, aName10, aType10, aSize10, aKind11, aName11, aType11, aSize11 ) \
    aKind12( aName12, const aType12, aSize12 )


#define MANIPULATOR_FIELDS_13( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                               aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                               aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9, \
                               aKind10, aName10, aType10, aSize10, aKind11, aName11, aType11, aSize11, aKind12, aName12, aType12, aSize12, \
                               aKind13, aName13, aType13, aSize13 ) \
    MANIPULATOR_FIELDS_12( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                           aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                           aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9, \
                           aKind10, aName10, aType10, aSize10, aKind11, aName11, aType11, aSize11, aKind12, aName12, aType12, aSize12 ) \
    aKind13( aName13, const aType13, aSize13 )

#define MANIPULATOR_FIELDS_14( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                               aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                               aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9, \
                               aKind10, aName10, aType10, aSize10, aKind11, aName11, aType11, aSize11, aKind12, aName12, aType12, aSize12, \
                               aKind13, aName13, aType13, aSize13, aKind14, aName14, aType14, aSize14 ) \
    MANIPULATOR_FIELDS_13( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                           aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                           aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9, \
                           aKind10, aName10, aType10, aSize10, aKind11, aName11, aType11, aSize11, aKind12, aName12, aType12, aSize12, \
                           aKind13, aName13, aType13, aSize13 ) \
    aKind14( aName14, const aType14, aSize14 )

#define MANIPULATOR_FIELDS_15( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                               aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                               aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9, \
                               aKind10, aName10, aType10, aSize10, aKind11, aName11, aType11, aSize11, aKind12, aName12, aType12, aSize12, \
                               aKind13, aName13, aType13, aSize13, aKind14, aName14, aType14, aSize14, aKind15, aName15, aType15, aSize15 ) \
    MANIPULATOR_FIELDS_14( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                           aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                           aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9, \
                           aKind10, aName10, aType10, aSize10, aKind11, aName11, aType11, aSize11, aKind12, aName12, aType12, aSize12, \
                           aKind13, aName13, aType13, aSize13, aKind14, aName14, aType14, aSize14 ) \
    aKind15( aName15, const aType15, aSize15 )






#define HEADER1( aClassName, aKind1, aName1, aType1, aSize1 )\
    HEADER( 1, aClassName##Header,\
        HEADER_FIELDS_1( aKind1, aName1, aType1, aSize1 ),\
        HEADER_FLD_INIT_1( aName1, aType1 ),\
    )\
    MANIPULATOR( aClassName##Manipulator,\
        MANIPULATOR_FIELDS_1( aKind1, aName1, aType1, aSize1 ) \
    )

#define HEADER2( aClassName, aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2 ) \
    HEADER( 2, aClassName##Header,\
        HEADER_FIELDS_2( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2 ),\
        HEADER_FLD_INIT_2( aName1, aType1, aName2, aType2 ) \
    )\
    MANIPULATOR( aClassName##Manipulator,\
        MANIPULATOR_FIELDS_2( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2 ) \
    )

#define HEADER3( aClassName, aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3 ) \
    HEADER( 3, aClassName##Header,\
        HEADER_FIELDS_3( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3 ),\
        HEADER_FLD_INIT_3( aName1, aType1, aName2, aType2, aName3, aType3 ) \
    )\
    MANIPULATOR( aClassName##Manipulator,\
        MANIPULATOR_FIELDS_3( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3  ) \
    )

#define HEADER4( aClassName,\
    aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3,\
    aKind4, aName4, aType4, aSize4 )\
    HEADER( 4, aClassName##Header,\
        HEADER_FIELDS_4( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                         aKind4, aName4, aType4, aSize4 ),\
        HEADER_FLD_INIT_4( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4 ) \
    )\
    MANIPULATOR( aClassName##Manipulator,\
        MANIPULATOR_FIELDS_4( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                              aKind4, aName4, aType4, aSize4 ) \
    )

#define HEADER5( aClassName,\
    aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3,\
    aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5 )\
    HEADER( 5, aClassName##Header,\
        HEADER_FIELDS_5( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                         aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5 ), \
        HEADER_FLD_INIT_5( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5 ) \
    )\
    MANIPULATOR( aClassName##Manipulator,\
        MANIPULATOR_FIELDS_5( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                              aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5 ) \
    )

#define HEADER6( aClassName,\
    aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3,\
    aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6 )\
    HEADER( 6, aClassName##Header,\
        HEADER_FIELDS_6( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                         aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6 ), \
        HEADER_FLD_INIT_6( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6 ) \
    )\
    MANIPULATOR( aClassName##Manipulator,\
        MANIPULATOR_FIELDS_6( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                              aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6 ) \
    )


#define HEADER7( aClassName,\
    aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3,\
    aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6,\
    aKind7, aName7, aType7, aSize7 )\
    HEADER( 7, aClassName##Header,\
        HEADER_FIELDS_7( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                         aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                         aName7, aType7, aSize7 ), \
        HEADER_FLD_INIT_7( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                           aName7, aType7 ) \
    )\
    MANIPULATOR( aClassName##Manipulator,\
        MANIPULATOR_FIELDS_7( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                              aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                              aKind7, aName7, aType7, aSize7 ) \
    )

#define HEADER8( aClassName,\
    aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3,\
    aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6,\
    aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8 )\
    HEADER( 8, aClassName##Header,\
        HEADER_FIELDS_8( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                         aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                         aName7, aType7, aSize7, aName8, aType8, aSize8 ), \
        HEADER_FLD_INIT_8( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                           aName7, aType7, aName8, aType8 ) \
    )\
    MANIPULATOR( aClassName##Manipulator,\
        MANIPULATOR_FIELDS_8( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                              aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                              aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8 ) \
    )

#define HEADER9( aClassName,\
    aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3,\
    aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6,\
    aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9 )\
    HEADER( 9, aClassName##Header,\
        HEADER_FIELDS_9( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                         aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                         aName7, aType7, aSize7, aName8, aType8, aSize8, aName9, aType9, aSize9 ), \
        HEADER_FLD_INIT_9( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                           aName7, aType7, aName8, aType8, aName9, aType9 ) \
    )\
    MANIPULATOR( aClassName##Manipulator,\
        MANIPULATOR_FIELDS_9( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                              aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                              aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9 ) \
    )

#define HEADER10( aClassName,\
    aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3,\
    aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6,\
    aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9,\
    aKind10, aName10, aType10, aSize10 )\
    HEADER( 10, aClassName##Header,\
        HEADER_FIELDS_10( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                          aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                          aName7, aType7, aSize7, aName8, aType8, aSize8, aName9, aType9, aSize9, \
                          aName10, aType10, aSize10 ), \
        HEADER_FLD_INIT_10( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                            aName7, aType7, aName8, aType8, aName9, aType9, aName10, aType10  ) \
    )\
    MANIPULATOR( aClassName##Manipulator,\
        MANIPULATOR_FIELDS_10( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                               aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                               aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9, \
                               aKind10, aName10, aType10, aSize10 ) \
    )

#define HEADER11( aClassName,\
    aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3,\
    aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6,\
    aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9,\
    aKind10, aName10, aType10, aSize10, aKind11, aName11, aType11, aSize11 )\
    HEADER( 11, aClassName##Header,\
        HEADER_FIELDS_11( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                          aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                          aName7, aType7, aSize7, aName8, aType8, aSize8, aName9, aType9, aSize9, \
                          aName10, aType10, aSize10, aName11, aType11, aSize11 ), \
        HEADER_FLD_INIT_11( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                            aName7, aType7, aName8, aType8, aName9, aType9, aName10, aType10, aName11, aType11  ) \
    )\
    MANIPULATOR( aClassName##Manipulator,\
        MANIPULATOR_FIELDS_11( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                               aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                               aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9, \
                               aKind10, aName10, aType10, aSize10, aKind11, aName11, aType11, aSize11 ) \
    )

#define HEADER12( aClassName,\
    aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3,\
    aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6,\
    aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9,\
    aKind10, aName10, aType10, aSize10, aKind11, aName11, aType11, aSize11, aKind12, aName12, aType12, aSize12 )\
    HEADER( 12, aClassName##Header,\
        HEADER_FIELDS_12( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                          aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                          aName7, aType7, aSize7, aName8, aType8, aSize8, aName9, aType9, aSize9, \
                          aName10, aType10, aSize10, aName11, aType11, aSize11, aName12, aType12, aSize12 ), \
        HEADER_FLD_INIT_12( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                            aName7, aType7, aName8, aType8, aName9, aType9, aName10, aType10, aName11, aType11, aName12, aType12  ) \
    )\
    MANIPULATOR( aClassName##Manipulator,\
        MANIPULATOR_FIELDS_12( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                               aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                               aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9, \
                               aKind10, aName10, aType10, aSize10, aKind11, aName11, aType11, aSize11, aKind12, aName12, aType12, aSize12 ) \
    )

#define HEADER13( aClassName,\
    aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3,\
    aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6,\
    aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9,\
    aKind10, aName10, aType10, aSize10, aKind11, aName11, aType11, aSize11, aKind12, aName12, aType12, aSize12,\
    aKind13, aName13, aType13, aSize13 )\
    HEADER( 13, aClassName##Header,\
        HEADER_FIELDS_13( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                          aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                          aName7, aType7, aSize7, aName8, aType8, aSize8, aName9, aType9, aSize9, \
                          aName10, aType10, aSize10, aName11, aType11, aSize11, aName12, aType12, aSize12,\
                          aName13, aType13, aSize13 ), \
        HEADER_FLD_INIT_13( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                            aName7, aType7, aName8, aType8, aName9, aType9, aName10, aType10, aName11, aType11, aName12, aType12, \
                            aName13, aType13 ) \
    )\
    MANIPULATOR( aClassName##Manipulator,\
        MANIPULATOR_FIELDS_13( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                               aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                               aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9, \
                               aKind10, aName10, aType10, aSize10, aKind11, aName11, aType11, aSize11, aKind12, aName12, aType12, aSize12, \
                               aKind13, aName13, aType13, aSize13) \
    )

#define HEADER14( aClassName,\
    aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3,\
    aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6,\
    aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9,\
    aKind10, aName10, aType10, aSize10, aKind11, aName11, aType11, aSize11, aKind12, aName12, aType12, aSize12,\
    aKind13, aName13, aType13, aSize13, aKind14, aName14, aType14, aSize14 )\
    HEADER( 14, aClassName##Header,\
        HEADER_FIELDS_14( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                          aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                          aName7, aType7, aSize7, aName8, aType8, aSize8, aName9, aType9, aSize9, \
                          aName10, aType10, aSize10, aName11, aType11, aSize11, aName12, aType12, aSize12,\
                          aName13, aType13, aSize13, aName14, aType14, aSize14 ), \
        HEADER_FLD_INIT_14( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                            aName7, aType7, aName8, aType8, aName9, aType9, aName10, aType10, aName11, aType11, aName12, aType12, \
                            aName13, aType13, aName14, aType14 ) \
    )\
    MANIPULATOR( aClassName##Manipulator,\
        MANIPULATOR_FIELDS_14( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                               aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                               aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9, \
                               aKind10, aName10, aType10, aSize10, aKind11, aName11, aType11, aSize11, aKind12, aName12, aType12, aSize12, \
                               aKind13, aName13, aType13, aSize13, aKind14, aName14, aType14, aSize14) \
    )

#define HEADER15( aClassName,\
    aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3,\
    aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6,\
    aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9,\
    aKind10, aName10, aType10, aSize10, aKind11, aName11, aType11, aSize11, aKind12, aName12, aType12, aSize12,\
    aKind13, aName13, aType13, aSize13, aKind14, aName14, aType14, aSize14, aKind15, aName15, aType15, aSize15 )\
    HEADER( 15, aClassName##Header,\
        HEADER_FIELDS_15( aName1, aType1, aSize1, aName2, aType2, aSize2, aName3, aType3, aSize3, \
                          aName4, aType4, aSize4, aName5, aType5, aSize5, aName6, aType6, aSize6, \
                          aName7, aType7, aSize7, aName8, aType8, aSize8, aName9, aType9, aSize9, \
                          aName10, aType10, aSize10, aName11, aType11, aSize11, aName12, aType12, aSize12,\
                          aName13, aType13, aSize13, aName14, aType14, aSize14, aName15, aType15, aSize15 ), \
        HEADER_FLD_INIT_15( aName1, aType1, aName2, aType2, aName3, aType3, aName4, aType4, aName5, aType5, aName6, aType6, \
                            aName7, aType7, aName8, aType8, aName9, aType9, aName10, aType10, aName11, aType11, aName12, aType12, \
                            aName13, aType13, aName14, aType14, aName15, aType15 ) \
    )\
    MANIPULATOR( aClassName##Manipulator,\
        MANIPULATOR_FIELDS_15( aKind1, aName1, aType1, aSize1, aKind2, aName2, aType2, aSize2, aKind3, aName3, aType3, aSize3, \
                               aKind4, aName4, aType4, aSize4, aKind5, aName5, aType5, aSize5, aKind6, aName6, aType6, aSize6, \
                               aKind7, aName7, aType7, aSize7, aKind8, aName8, aType8, aSize8, aKind9, aName9, aType9, aSize9, \
                               aKind10, aName10, aType10, aSize10, aKind11, aName11, aType11, aSize11, aKind12, aName12, aType12, aSize12, \
                               aKind13, aName13, aType13, aSize13, aKind14, aName14, aType14, aSize14, aKind15, aName15, aType15, aSize15) \
    )

//////////////////////////////////////////////////////////////////////////////////////////////////

}

#endif // __Format_hpp__
