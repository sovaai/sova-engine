#.rst:
# FindMemcached
# -------
#
# Finds the Memcached library
#

find_package( PkgConfig QUIET )
if( PKG_CONFIG_FOUND )
	pkg_check_modules( PC_MEMCACHED QUIET memcached )
endif( PKG_CONFIG_FOUND )

find_path( MEMCACHED_INCLUDE_DIR
			NAMES libmemcached/memcached.h
			PATHS ${PC_MEMCACHED_INCLUDE_DIRS} /usr/include /usr/local/include /opt/local/include )

find_library( MEMCACHED_LIBRARY
				NAMES memcached
				PATHS ${PC_MEMCACHED_LIBRARY_DIRS} /usr/lib /usr/local/lib /opt/local/lib )

set( MEMCACHED_VERSION ${PC_MEMCACHED_VERSION} )
set( MEMCACHED_VERSION_STRING ${PC_MEMCACHED_VERSION} )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( MEMCACHED
#				FOUND_VAR MEMCACHED_FOUND
				REQUIRED_VARS MEMCACHED_INCLUDE_DIR MEMCACHED_LIBRARY
				VERSION_VAR MEMCACHED_VERSION )

if( MEMCACHED_FOUND )
	set( MEMCACHED_LIBRARIES ${MEMCACHED_LIBRARY} )
	set( MEMCACHED_INCLUDE_DIRS ${MEMCACHED_INCLUDE_DIR} )
	set( MEMCACHED_DEFINITIONS ${PC_MEMCACHED_CFLAGS_OTHER} )

	if( NOT TARGET MEMCACHED::MEMCACHED )
		add_library( MEMCACHED::MEMCACHED UNKNOWN IMPORTED )
		set_target_properties( MEMCACHED::MEMCACHED PROPERTIES
				IMPORTED_LOCATION "${MEMCACHED_LIBRARY}"
				INTERFACE_COMPILE_OPTION "${PC_MEMCACHED_CFLAGS_OTHER}"
				INTERFACE_INCLUDE_DIRECTORIES "${MEMCACHED_INCLUDE_DIR}" )
	endif( NOT TARGET MEMCACHED::MEMCACHED )
endif( MEMCACHED_FOUND )

mark_as_advanced( MEMCACHED_INCLUDE_DIR MEMCACHED_LIBRARY )