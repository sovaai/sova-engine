cmake_minimum_required( VERSION 3.7 )

project( pts2 )
add_library( ${PROJECT_NAME} STATIC
    PatriciaTree.cpp
    PatriciaTreeFS.cpp
    ReverseIndexMixin.cpp
    PatriciaTreeOverlay_Standard.cpp
    PatriciaTreeOverlay_Compact.cpp
    PatriciaTreeOverlay_StandardRaw.cpp
    PatriciaTreeOverlay_Generic.cpp
    CrossTree_Standard.cpp
    CrossTree_StandardRaw.cpp
    CrossTree_Compact.cpp )
target_link_libraries( ${PROJECT_NAME} PUBLIC fstorage )
