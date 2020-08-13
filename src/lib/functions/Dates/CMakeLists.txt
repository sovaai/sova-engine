cmake_minimum_required( VERSION 2.6 )

add_external_function( Dates	DateDist		DateDist.cpp		MemoryAllocator		"" )
add_external_function( Dates	DateGreater		DateGreater.cpp		MemoryAllocator		"" )
add_external_function( Dates	DateLess		DateLess.cpp		MemoryAllocator		"" )
add_external_function( Dates	Extract			Extract.cpp			MemoryAllocator		"" )
add_external_function( Dates	TimeGreater		TimeGreater.cpp		MemoryAllocator		"" )
add_external_function( Dates	TimeLess		TimeLess.cpp		MemoryAllocator		"" )

set( DL_FUNCTIONS ${DL_FUNCTIONS} PARENT_SCOPE )
