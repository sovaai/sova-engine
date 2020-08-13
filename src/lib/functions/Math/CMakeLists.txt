add_external_function( Math		BigGreater		BigGreater.cpp		MemoryAllocator		"" )
add_external_function( Math		BigLess			BigLess.cpp			MemoryAllocator		"" )
add_external_function( Math		Random			Random.cpp			MemoryAllocator		"" )

add_external_function( Math		Diff		Diff.cpp		"MemoryAllocator;NumberConvertor"		Gmp )
add_external_function( Math		Divide		Divide.cpp		"MemoryAllocator;NumberConvertor"		Gmp )
add_external_function( Math		Greater		Greater.cpp		"MemoryAllocator;NumberConvertor"		Gmp )
add_external_function( Math		Less		Less.cpp		"MemoryAllocator;NumberConvertor"		Gmp )
add_external_function( Math		Minus		Minus.cpp		"MemoryAllocator;NumberConvertor"		Gmp )
add_external_function( Math		Multiply	Multiply.cpp	"MemoryAllocator;NumberConvertor"		Gmp )
add_external_function( Math		Plus		Plus.cpp		"MemoryAllocator;NumberConvertor"		Gmp )
add_external_function( Math		Remainder	Remainder.cpp	"MemoryAllocator;NumberConvertor"		Gmp )

set( DL_FUNCTIONS ${DL_FUNCTIONS} PARENT_SCOPE )
