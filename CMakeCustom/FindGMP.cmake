#.rst:
# FindGMP
# -------
#
# Finds the GMP library
#

find_package( PkgConfig QUIET )
if( PKG_CONFIG_FOUND )
	pkg_check_modules( PC_GMP QUIET gmp )
endif( PKG_CONFIG_FOUND )

find_path( GMP_INCLUDE_DIR
			NAMES gmpxx.h
			PATHS ${PC_GMP_INCLUDE_DIRS} /usr/include /usr/local/include /opt/local/include )

find_library( GMP_LIBRARY
				NAMES gmp
				PATHS ${PC_GMP_LIBRARY_DIRS} /usr/lib /usr/local/lib /opt/local/lib )

set( GMP_VERSION ${PC_GMP_VERSION} )
set( GMP_VERSION_STRING ${PC_GMP_VERSION} )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( GMP
#				FOUND_VAR GMP_FOUND
				REQUIRED_VARS GMP_INCLUDE_DIR GMP_LIBRARY
				VERSION_VAR GMP_VERSION )

if( GMP_FOUND )
	set( GMP_LIBRARIES ${GMP_LIBRARY} )
	set( GMP_INCLUDE_DIRS ${GMP_INCLUDE_DIR} )
	set( GMP_DEFINITIONS ${PC_GMP_CFLAGS_OTHER} )

	if( NOT TARGET GMP::GMP )
		add_library( GMP::GMP UNKNOWN IMPORTED )
		set_target_properties( GMP::GMP PROPERTIES
				IMPORTED_LOCATION "${GMP_LIBRARY}"
				INTERFACE_COMPILE_OPTION "${PC_GMP_CFLAGS_OTHER}"
				INTERFACE_INCLUDE_DIRECTORIES "${GMP_INCLUDE_DIR}" )
	endif( NOT TARGET GMP::GMP )
endif( GMP_FOUND )

mark_as_advanced( GMP_INCLUDE_DIR GMP_LIBRARY )
