#include <dlfcn.h>
#include <ctime>

#include <NanoLib/nMemoryAllocator.hpp>

#include <InfEngine2/Functions/FunctionLib.hpp>

#include <cstdlib>

/**
 *  Проверяет корректность переданных программе аргументов.
 * @param argc - количество переданных программе аргументов
 * @param argv - список переданных программе аргументов
 */
bool ValidateArgs( int argc, char * argv[] )
{
    return argc > 1;
}

/**
 *  Выводит на экран подсказку.
 */
void PrintHint( )
{
    fprintf( stderr,
             "FuncMon - gets DL function information from dynamic library\n"
             "Usage:\n"
             "\tFuncMon ToUpper.so\n"
             "\tFuncMon ToUpper.so some_string_as_arg\n"
             "\tFuncMon --fversion ToUpper.so\n"
             "\tFuncMon --check ToUpper.so\n" );
}

/**
 *  Выводит на экран информацию о функции.
 * @param ldf_info - информация о функции.
 */
void PrintInfo( const DLFunctionInfo * dlf_info )
{
    printf( "Name: %s\n", dlf_info->name );
    printf( "Short name: %s\n", dlf_info->short_name );
    printf( "Version: %i.%i.%i\n", dlf_info->version.a, dlf_info->version.b, dlf_info->version.c );
    if( dlf_info->VarArgType )
        printf( "Args(%i+)\n", dlf_info->argc );
    else
        printf( "Args(%i):\n", dlf_info->argc );

    for( unsigned int arg_n = 0; arg_n < dlf_info->argc; ++arg_n )
    {
        printf( "\t" );
        switch( dlf_info->argt[arg_n].type )
        {
            case DLFAT_TEXT:
                printf( "TEXT" );
                break;

            case DLFAT_DYNAMIC:
                printf( "DYNAMIC" );
                break;

            case DLFAT_VARIABLE:
                printf( "VARIABLE" );
                break;

            case DLFAT_EXTENDED:
                printf( "EXTENDED" );
                break;

            case DLFAT_ANY:
                printf( "ANY" );
                break;

            case DLFAT_IF:
                printf( "IF" );
                break;
                
            case DLFAT_BOOL:
                printf( "BOOL" );
                break;

            default:
                printf( "UNKNOWN" );
                exit( 1 );
        }
        if( dlf_info->argt[arg_n].default_value )
            printf( " = \"%s\"", dlf_info->argt[arg_n].default_value );
        printf( "\n" );
    }

    if( dlf_info->VarArgType )
    {
        printf( "\t" );
        switch( dlf_info->VarArgType->type )
        {
            case DLFAT_TEXT:
                printf( "TEXT" );
                break;

            case DLFAT_DYNAMIC:
                printf( "DYNAMIC" );
                break;

            case DLFAT_VARIABLE:
                printf( "VARIABLE" );
                break;

            case DLFAT_EXTENDED:
                printf( "EXTENDED" );
                break;

            case DLFAT_ANY:
                printf( "ANY" );
                break;

            case DLFAT_IF:
                printf( "IF" );
                break;
            
            case DLFAT_BOOL:
                printf( "BOOL" );
                break;

            default:
                printf( "UNKNOWN" );
                exit( 1 );
        }
        if( dlf_info->VarArgType->default_value )
            printf( " = \"%s\"", dlf_info->VarArgType->default_value );
        printf( "\n\t...\n" );
    }

    switch( dlf_info->rest )
    {
        case DLFRT_TEXT:
            printf( "Result type: TEXT\n" );
            break;

        case DLFRT_BOOL:
            printf( "Result type: BOOL\n" );
            break;

        default:
            printf( "Result type: UNKNOWN\n" );
            exit( 1 );
    }

    printf( "Description: %s\n", dlf_info->description );
    printf( "\nAPI Version:         %i\n", dlf_info->DLFunctionInterfaceVer );
    printf( "Minimal API Version: %i\n", dlf_info->DLFunctionInterfaceMinVer );
    printf( "InfData Protocol Version: %i\n", dlf_info->InfDataProtocolVer );
}

/**
 *  Вызывает функцию с заданными аргументами и выводит на экран её результат.
 * @param hd - хэндлер динамической библиотеки, в которой содержится функция.
 * @param dlf_info - адрес структуры с описанием функции.
 * @param argc - количество передаваемых в функцию аргументов.
 * @param argv - список передаваемых аргументов.
 */
void CallFunc( void * hd, const DLFunctionInfo * dlf_info, int argc, char * argv[] )
{
    /** Полуение адреса функции. */
    FDLFucntion DLFunction = (FDLFucntion)dlsym( hd, dlf_info->name );
    if( !DLFunction )
    {
        printf( "%s\n", dlerror( ) );
        exit( 1 );
    }

    /** пПодготовка аргументов.*/
    const char ** args = new const char *[argc + 1];
    for( int arg_n = 0; arg_n < argc; ++arg_n )
        args[arg_n] = argv[arg_n];
    args[argc] = nullptr;

    const char * result;
    unsigned int length;

    /** Вызов функции, вывод результата. */
    nMemoryAllocator tmpMemoryAllocator;    
    InfEngineErrors iee = DLFunction( args, result, length, &tmpMemoryAllocator, nullptr );
    delete[] args;
    if( iee != INF_ENGINE_SUCCESS )
    {
        if( iee == INF_ENGINE_ERROR_ARGC )
        {
            fprintf( stderr, "Error: wrong arguments number\n" );
            exit( 1 );
        }
        else
        {
            fprintf( stderr, "Error: failed to call function  %s\n", dlf_info->name );
            exit( 1 );
        }
    }

    switch( dlf_info->rest )
    {
        case DLFRT_TEXT:
            printf( "%s\n", result );
            break;

        case DLFRT_BOOL:
            printf( "%s\n", result[0] != '\0' ? "True" : "False" );
            break;

        default:
            printf( "Error: Result type: UNKNOWN\n" );
            exit( 1 );
    }
}

void printVersion( const char * aDLPath )
{
    void * hd = dlopen( aDLPath, RTLD_LAZY );
    if( !hd )
    {
        printf( "Failed to open file \"%s\" with error: %s\n", aDLPath, dlerror( ) );
        exit( 1 );
    }

    FGetDLFucntionInfo GetDLFucntionInfo = (FGetDLFucntionInfo)dlsym( hd, "GetDLFucntionInfo" );
    if( !GetDLFucntionInfo )
    {
        printf( "%s\n", dlerror( ) );
        exit( 1 );
    }

    const DLFunctionInfo * dlf_info = GetDLFucntionInfo( );
    if( !dlf_info )
    {
        printf( "ERROR: Information not found\n" );
        exit( 1 );
    }

    printf( "%i.%i.%i\n", dlf_info->version.a, dlf_info->version.b, dlf_info->version.c );
}

void checkFunction( const char * aDLPath )
{
    printf( "\n  Loading library - " );
    void * hd = dlopen( aDLPath, RTLD_LAZY );
    if( !hd )
    {
        printf( "%s\n", dlerror( ) );
        exit( 1 );
    }
    printf( "[SUCCESS]\n" );

    printf( "  Loading DL function info - " );
    FGetDLFucntionInfo GetDLFucntionInfo = (FGetDLFucntionInfo)dlsym( hd, DLF_ENTRY_POINT );
    if( !GetDLFucntionInfo )
    {
        printf( "%s\n", dlerror( ) );
        exit( 1 );
    }

    const DLFunctionInfo * dlf_info = GetDLFucntionInfo( );
    if( !dlf_info )
    {
        printf( "ERROR: Information not found\n" );
        exit( 1 );
    }
    printf( "[SUCCESS]\n" );

    printf( "  Loading DL function - " );
    FDLFucntion DLFunction = (FDLFucntion)dlsym( hd, dlf_info->name );
    if( !DLFunction )
    {
        printf( "%s\n", dlerror( ) );
        exit( 1 );
    }
    printf( "[SUCCESS]\n" );

    printf( "  Loading checking function - " );
    FCheckDLFucntion CheckDLFucntion = (FCheckDLFucntion)dlsym( hd, CHECK_ENTRY_POINT );
    if( !CheckDLFucntion )
    {
        printf( "%s\n", dlerror( ) );
        exit( 1 );
    }
    printf( "[SUCCESS]\n" );

    if( CheckDLFucntion( ) == INF_ENGINE_SUCCESS )
        printf( "\n[SUCCESS]\n" );
    else
        printf( "\n[FAIL]\n" );
}

int main( int argc, char * argv[] )
{
    // Инициализация рандомизатора.
    srand( time( nullptr ) );

    if( ValidateArgs( argc, argv ) )
    {
        if( argc == 3 )
        {
            for( int arg_n = 1; arg_n < argc; ++arg_n )
            {
                if( !strcmp( argv[arg_n], "--fversion" ) )
                {
                    printVersion( arg_n == 1 ? argv[2] : argv[1] );
                    return 0;
                }
                else if( !strcmp( argv[arg_n], "--check" ) )
                {
                    checkFunction( arg_n == 1 ? argv[2] : argv[1] );
                    return 0;
                }
            }
        }

        void * hd = dlopen( argv[1], RTLD_LAZY );
        if( !hd )
        {
            printf( "%s\n", dlerror( ) );
            exit( 1 );
        }

        FGetDLFucntionInfo GetDLFucntionInfo = (FGetDLFucntionInfo)dlsym( hd, DLF_ENTRY_POINT );
        if( !GetDLFucntionInfo )
        {
            printf( "%s\n", dlerror( ) );
            exit( 1 );
        }

        const DLFunctionInfo * dlf_info = GetDLFucntionInfo( );
        if( !dlf_info )
        {
            printf( "ERROR: Information not found\n" );
            exit( 1 );
        }

        if( argc == 2 )
        {
            PrintInfo( dlf_info );
        }
        else
        {
            CallFunc( hd, dlf_info, argc - 2, argv + 2 );
        }

        dlclose( hd );
    }
    else
    {
        PrintHint( );
        exit( 1 );
    }

    return 0;
}
