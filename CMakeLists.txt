file(GLOB BLACKBOX_SRC_FILES
	"*.cpp"
	"*.h"
)

add_library(blackbox SHARED ${BLACKBOX_SRC_FILES})
target_compile_definitions(blackbox PUBLIC DLL_PLATFORM)
target_compile_definitions(blackbox PRIVATE LIB_EXPORTS)

set_target_properties(blackbox PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bins/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bins/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bins/bin/plugins"
)
