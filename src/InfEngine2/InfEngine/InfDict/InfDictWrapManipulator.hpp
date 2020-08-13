#ifndef __InfDictWrapManipulator_hpp__
#define __InfDictWrapManipulator_hpp__

#include "InfDictManipulator.hpp"

/**
 *  Класс объединяющий несколько одноимённых словарей в один.
 */
class InfDictWrapManipulator
{
    public:

        // Методы объединения одноимённых словарей из основной и пользовательской базы.
        enum JoiningMethod
        {
            jmAddiction = 0,			// Строки добавляемого словаря дописываются после в конец списока уже имеющихся строк.
            jmSubstitution = 1,			// Список имеющихся строк заменяется на список строк добавляемого словаря.
			
			jmDefault = jmSubstitution,	// Метод, применяемый по умолчанию.
        };
        
    public:
        InfDictWrapManipulator( )
                : vLinesNum(0)
                , vSelfLinesNum(0)
                , vVariantsNum(0)
                , vSubdictsNum(0) 
                , vDLData( nullptr )
        {}
                
        void SetDLData( const DLDataRO * aDLData )
                { vDLData = aDLData; }

        // Сброс состояния манипулятора.
        inline void Reset()
        {
            vDicts.clear();
            vLinesNum = 0;
            vSelfLinesNum = 0;
            vVariantsNum = 0;
            vSubdictsNum = 0;
            vDLData = nullptr;
        }

        // Добавление словаря.
        inline InfEngineErrors Attach( const InfDictManipulator & aDict, JoiningMethod aJoiningMethod, const DLDataRO * aDataRO )
        {
            if( jmAddiction ==  aJoiningMethod )
            {
                // Добавление нового словаря к предыдущим.
                vLinesNum += aDict.GetNum();
                vSelfLinesNum += aDict.GetSelfLinesNum();
                vVariantsNum += aDict.GetVariantsNumber();
                vSubdictsNum += aDict.GetSubdictsCount();
            }
            else if( jmSubstitution ==  aJoiningMethod )
            {
                // Замена предыдущих словарей на новый.
                vDicts.clear();
				vAttachedToData.clear();
                vLinesNum = aDict.GetNum();
                vSelfLinesNum = aDict.GetSelfLinesNum();
                vVariantsNum = aDict.GetVariantsNumber();
                vSubdictsNum = aDict.GetSubdictsCount();
            }

            vDicts.push_back( aDict );
            if( vDicts.no_memory() )
                return INF_ENGINE_ERROR_NOFREE_MEMORY;
			vAttachedToData.push_back( aDataRO );
            if( vAttachedToData.no_memory() )
                return INF_ENGINE_ERROR_NOFREE_MEMORY;

            return INF_ENGINE_SUCCESS;
        }


    public:
        /** Получение имени словаря. **/
        inline const char * GetName() const
            { return vDicts.size() ? vDicts.back().GetName() : nullptr; }

        /** Получение идентификатора словаря.**/
        inline unsigned int GetID() const
            { return vDicts.size() ? vDicts.back().GetID() : 0; }

        /** Получение количества строк в словаре. **/
        inline unsigned int GetNum()
            { return vLinesNum; }
        
        /** Получение количества строк в словаре, не считая подсловари. **/
        inline unsigned int GetSelfLinesNum()
            { return vSelfLinesNum; }                
        
        /** Получение количества вариантов раскрытия словаря. **/
        unsigned int GetVariantsNumber() const
            { return vVariantsNum; }
        
        /** Получение количества подсловарей словаря. **/
        unsigned int GetSubdictsNumber() const
            { return vSubdictsNum; }

        /** Получение aNum-ой строки. **/
        InfEngineErrors GetString( unsigned int aNum, InfDictStringManipulator & aDictString, bool aGetMainString = false )
        {                                           
            unsigned int ln = 0;
            // Поиск словаря, содержащего заданную строкую
            for( unsigned int dict_n = 0; dict_n < vDicts.size(); ++dict_n )
            {
                if( aNum < ln + vDicts[dict_n].GetNum() )
				{					
                    InfEngineErrors iee = vDicts[dict_n].GetString( aNum - ln, aDictString, vDLData, aGetMainString );
					
					if( INF_ENGINE_SUCCESS == iee )
						aDictString.SetDLData( vAttachedToData[dict_n] );
					
					return iee;
				}
                ln += vDicts[dict_n].GetNum();
            }

            // Строки с заданным номером не существует.
            return INF_ENGINE_ERROR_INV_ARGS;
        }
        
        /** Получение идентификатора aNum-ого подсловаря. **/
        unsigned int GetSubdictID( unsigned int aNum )
        {                                    
            unsigned int ln = 0;
            // Поиск словаря, содержащего заданный подсловарь.
            for( unsigned int dict_n = 0; dict_n < vDicts.size(); ++dict_n )
            {
                if( aNum < ln + vDicts[dict_n].GetSubdictsCount() )
                    return vDicts[dict_n].GetSubDictID( aNum - ln );
                ln += vDicts[dict_n].GetSubdictsCount();
            }

            // Подсловаря с заданным номером не существует.
            return static_cast<unsigned int>(-1);
        }

    private:

        // Список объединённых словарей.
        avector<InfDictManipulator> vDicts;
		
		// Список баз, в которых сохранены объединённые словари.
		avector<const DLDataRO*> vAttachedToData;

        // Суммарное количество строк в словарях.
        unsigned int vLinesNum;
        
        // Суммарное количество строк в словарях, не считая подсловарей.
        unsigned int vSelfLinesNum;
        
        // Суммарное количество вариантов раскрытия словаря.
        unsigned int vVariantsNum;
        
        // Суммарное количество подсловарей словаря.
        unsigned int vSubdictsNum;
        
        // База.
        const DLDataRO * vDLData;
};

#endif /** __InfDictWrapManipulator_hpp__ */
