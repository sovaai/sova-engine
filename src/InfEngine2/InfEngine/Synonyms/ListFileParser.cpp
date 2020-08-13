#include "ListFileParser.hpp"

#include <cerrno>

#include <NanoLib/TextFuncs.hpp>

InfEngineErrors ListFileParser::ParseFromFile( const char * aFilePath )
{
	// Текущая строка файла.
	aTextString aLine;

	// Открытие файла.
	FILE * aFH = fopen( aFilePath, "r" );
	if( !aFH )
		ReturnWithError( INF_ENGINE_ERROR_FILE, "Can't open file \"%s\". Return code: %i", aFilePath, errno );

	// Чтение из файла.
	nlReturnCode nlr = nlrcSuccess;

	while((nlr = aLine.ReadString( aFH )) == nlrcSuccess )
	{
		InfEngineErrors iee = ParseString( aLine.ToConstChar(), aLine.size());
		if( INF_ENGINE_SUCCESS != iee )
			ReturnWithTrace( iee );
	}
	fclose( aFH );

	if( nlr != nlrcEOF )
		ReturnWithError( INF_ENGINE_ERROR_CANT_READ, "Can't read file list from \"%s\". Return code: %d", aFilePath, nlr );

	return INF_ENGINE_SUCCESS;
}

InfEngineErrors ListFileParser::ParseString( const char * aString, unsigned int aLength )
{
	char * name = nullptr;
	char * path = nullptr;
	unsigned int pos = 0;
	unsigned int name_begin = 0;
	unsigned int name_end = 0;
	unsigned int path_begin = 0;
	unsigned int path_end = 0;

	// Пропускаем ведущие пробелы.
	pos = TextFuncs::SkipSpaces( aString, 0, aLength );
	name_begin = path_begin = pos;

	// Если это строка-комментарий, пропускаем её.
	if( aLength - pos >= 2 && aString[pos] == '/' && aString[pos + 1] == '/' )
		return INF_ENGINE_SUCCESS;

	while( pos < aLength && !TextFuncs::IsSpace( aString[pos] ))
		++pos;

	// Проверяем, указана ли в текущей строке именованная ссылка на файл перед его адресом.
	if( pos < aLength )
	{
		name_end = pos;

		// Находим начало адреса файла.
		while( pos < aLength && TextFuncs::IsSpace( aString[pos] ))
			++pos;

		// Если пробельные символы есть только в конце строки.
		if( pos == aLength )
		{
			path_end = name_end;
		}
		else
		{
			// Остаток строки - это адрес файла.
			path_begin = pos;
			path_end = aLength + 1;
		}
	}
	else
	{
		// Ссылка на файл и его адрес - это вся строка.
		name_end = path_end = aLength + 1;
	}

	// Выделем память и копируем в неё имя ссылки на файл.
	name = nAllocateObjects( vMemoryAllocator, char, name_end - name_begin + 1 );
	if( !name )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	memcpy( name, aString + name_begin, name_end - name_begin );
	name[name_end - name_begin] = '\0';

	// Выделем память и копируем в неё адрес файла.
	path = nAllocateObjects( vMemoryAllocator, char, path_end - path_begin + 1 );
	if( !path )
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );
	memcpy( path, aString + path_begin, path_end - path_begin );
	name[path_end - path_begin] = '\0';

	// Сохроаняем выделенные данные.
	vPathList.push_back( path );
	if( vPathList.no_memory())
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	vNameList.push_back( path );
	if( vNameList.no_memory())
		ReturnWithError( INF_ENGINE_ERROR_NOFREE_MEMORY, INF_ENGINE_STRING_ERROR_CANT_ALLOCATE_MEMORY );

	return INF_ENGINE_SUCCESS;
}

void ListFileParser::Reset()
{
	// Очищаем контейнеры.
	vPathList.clear();
	vNameList.clear();
	vMemoryAllocator.Reset();
}
