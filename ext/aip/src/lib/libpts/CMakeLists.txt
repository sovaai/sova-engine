cmake_minimum_required( VERSION 3.7 )

project( pts )
add_library( ${PROJECT_NAME} STATIC
    pat_tree.c
    pts.c
    pat_imp.c
    pts_imp.c
    ptsfs.c )
target_include_directories( ${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../.. )