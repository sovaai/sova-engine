#ifndef __Weight_hpp__
#define __Weight_hpp__

/**
 *  Класс для манипулирования весом шаблона.
 */
class PatternWeight
{
    public:
        /**
         *  Конструктор.
         * @param aWeight - значение веса.
         */
        PatternWeight( unsigned int aWeight = 0 )
                { vWeight = aWeight; }


    public:
        /**
         *  Изменение части веса, определяемой количеством условий, содержащихся в шаблоне.
         * @param aConditionsNumber - количество условий содержащихся в шаблоне.
         */
        void SetConditionsNumber( unsigned int aConditionsNumber )
                { vWeight |= 0x3FC0 & ( ( ( aConditionsNumber > 0xFF ) ? 0xFF : aConditionsNumber ) << 6 ); }

        /**
         *  Изменение части веса, определяемой длиной в "словах" шаблон-вопроса в раскрытом виде.
         * @param aLength - длина шаблон вопроса в "словах".
         */
        void SetLength( unsigned int aLength )
                { vWeight |= 0x3FFFC000 & ( ( ( aLength > 0xFFFF ) ? 0xFFFF : aLength ) << 14 ); }

        /** Изменение части вес, определяемой наличием условия на переменную TOPIC. */
        void SetTopicWeight()
                { vWeight |= 0x10; }

        /** Изменение части веса, определяемой наличием условия на переменные класса TREE. */
        void SetTreeWeight()
                { vWeight |= 0x20; }

        /** Изменение части веса, определяемой наличием условия на переменную THAT_ANCHOR. */
        void SetAnchorWeight()
                { vWeight |= 0x80000000; }

        /** Изменение части веса, определяемой отсутствием звездочек в шаблон-вопросе. */
        void SetUnstarred()
                { vWeight |= 0x40000000; }

    public:
        /**
         *  Добавление веса условия к существующему весу.
         * @param aWeight - вес добавляемого условия.
         */
        void AddWeight( PatternWeight aWeight )
                {
                    if( aWeight & 0x80000000 )
                        vWeight |= 0x80000000;

                    if( aWeight & 0x40000000 )
                        vWeight |= 0x40000000;

                    if( aWeight & 0x00000020 )
                        vWeight |= 0x00000020;

                    if( aWeight & 0x00000010 )
                        vWeight |= 0x00000010;

                    unsigned int value;

                    value = ((vWeight&0x3FC0)>>6) + ((aWeight&0x3FC0)>>6);
                    if( value >= 0xFF )
                        value = 0xFF;
                    vWeight = (vWeight&0xFFFFC03F) | (value<<6);

                    value = ((vWeight&0x3FFFC000)>>14) + ((aWeight&0x3FFFC000)>>14);
                    if( value >= 0xFF )
                        value = 0xFFFF;
                    vWeight = (vWeight&0xC0003FFF) | (value<<14);

                }

        /**
         *  Добавление веса условия к существующему весу.
         * @param aWeight - вес добавляемого условия.
         */
        void AddWeight( unsigned int aWeight )
                { AddWeight( (PatternWeight)aWeight ); }


    public:
        /** Преобразование к unsigned int. */
        operator unsigned int() const
                { return vWeight; }

        /**
         *  Добавление веса условия к существующему весу.
         * @param aWeight - вес добавляемого условия.
         */
        PatternWeight& operator =( unsigned int aWeight )
                { vWeight = aWeight; return *this; }

        /**
         *  Добавление веса условия к существующему весу.
         * @param aWeight - вес добавляемого условия.
         */
        PatternWeight& operator =( const PatternWeight aWeight )
                { vWeight = aWeight.vWeight; return *this; }


    private:
        /** Значение веса. */
        unsigned int vWeight;
};

#endif  /* __Weight_hpp__ */
