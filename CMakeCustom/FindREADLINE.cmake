#.rst:
# FindReadline
# -------
#
# Finds the Readline library
#

find_package( PkgConfig QUIET )
if( PKG_CONFIG_FOUND )
	pkg_check_modules( PC_READLINE QUIET readline )
endif( PKG_CONFIG_FOUND )

find_path( READLINE_INCLUDE_DIR
			NAMES readline/readline.h
			PATHS ${PC_READLINE_INCLUDE_DIRS} /usr/include /usr/local/include /opt/local/include )

find_library( READLINE_LIBRARY
				NAMES readline
				PATHS ${PC_READLINE_LIBRARY_DIRS} /usr/lib /usr/local/lib /opt/local/lib )

set( READLINE_VERSION ${PC_READLINE_VERSION} )
set( READLINE_VERSION_STRING ${PC_READLINE_VERSION} )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( READLINE
#				FOUND_VAR READLINE_FOUND
				REQUIRED_VARS READLINE_INCLUDE_DIR READLINE_LIBRARY
				VERSION_VAR READLINE_VERSION )

if( READLINE_FOUND )
	set( READLINE_LIBRARIES ${READLINE_LIBRARY} )
	set( READLINE_INCLUDE_DIRS ${READLINE_INCLUDE_DIR} )
	set( READLINE_DEFINITIONS ${PC_READLINE_CFLAGS_OTHER} )

	if( NOT TARGET READLINE::READLINE )
		add_library( READLINE::READLINE UNKNOWN IMPORTED )
		set_target_properties( READLINE::READLINE PROPERTIES
				IMPORTED_LOCATION "${READLINE_LIBRARY}"
				INTERFACE_COMPILE_OPTION "${PC_READLINE_CFLAGS_OTHER}"
				INTERFACE_INCLUDE_DIRECTORIES "${READLINE_INCLUDE_DIR}" )
	endif( NOT TARGET READLINE::READLINE )
endif( READLINE_FOUND )

mark_as_advanced( READLINE_INCLUDE_DIR READLINE_LIBRARY )
