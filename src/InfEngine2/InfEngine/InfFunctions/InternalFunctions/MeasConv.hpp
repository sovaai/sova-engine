#ifndef __MeasConv_hpp__
#define __MeasConv_hpp__

#include <InfEngine2/Functions/FunctionLib.hpp>
#include <InfEngine2/InfEngine/CoreRO.hpp>
#include <NanoLib/LogSystem.hpp>

#include "InternalFunction.hpp"

#include <NanoLib/NameIndex.hpp>

namespace InternalFunctions
{
    
    class MeasConv
    {
    public:
        
		static void Init( ExtICoreRO & aBase );
        
        static InfEngineErrors Apply( const char ** aArgs,
                const char *& aResult,
                unsigned int & aLength,
                nMemoryAllocator * aMemoryAllocator, 
                FunctionContext * aFunctionContext );
        
    public:
        
        static const DLFucntionArgInfo ARGS[];
        
        static const DLFucntionArgInfo VAR_ARGS;

        static const DLFunctionInfo DLF_INFO;
        
        enum Group {
            G_TIME,
            G_PRESSURE,
            G_LENGTH,
            G_INFORMATION,            
            G_MAGNETIC_FIELD,
            G_WEIGHT,
            G_POWER,
            G_VOLUME,
            G_AREA,
            G_RADIOACTIVITY,
            G_LIGHT,
            G_SPEED,
            G_TEMPERATURE,
            G_ANGLE,
            G_ENERGY,
        };
        
        
    private:
        
        static void InitTable();
        
    private:
        static ExtICoreRO * vBase;
        
        
        struct Unit
        {
            enum Group vGroup;
            
            double vMValue;
            
            double vSValue;
        };
        
        static NanoLib::NameIndex vUnitsNames;
        
        static avector<Unit> vUnitsValues;               
    };
    
}

#endif // __MeasConv_hpp__
