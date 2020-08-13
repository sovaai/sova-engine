cmake_minimum_required( VERSION 3.0 )

project( ap-process-server )

add_executable( ${PROJECT_NAME} ap-process-server.c )

target_link_libraries( ${PROJECT_NAME} PRIVATE apcs )

target_compile_definitions( ${PROJECT_NAME} PRIVATE
	-DDEFAULT_CONFIGFILE=\"/usr/local/InfEngine/conf/ap-process-server.conf\"
	-DDEFAULT_FILTER_PATH=\"/usr/local/InfEngine/bin/InfServer\"
	-DUSE_INF_TERMS
	-DCSDATA_EXTENDED
	)
