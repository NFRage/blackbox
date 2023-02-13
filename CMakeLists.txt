file(GLOB BLACKBOX_SRC_FILES CONFIGURE_DEPENDS
	"*.cpp"
	"*.h"
)

add_library(blackbox SHARED ${BLACKBOX_SRC_FILES})
target_compile_definitions(blackbox PUBLIC DLL_PLATFORM)
target_compile_definitions(blackbox PRIVATE BLACKBOX_EXPORTS)
target_precompile_headers(blackbox PRIVATE "blackbox_pch.h")
target_link_libraries(blackbox PRIVATE
    spdlog::spdlog
)

set_target_properties(blackbox PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${SPEED_ROOT_DIR}/bins/lib/$<0:>"
    LIBRARY_OUTPUT_DIRECTORY "${SPEED_ROOT_DIR}/bins/lib/$<0:>"
    RUNTIME_OUTPUT_DIRECTORY "${SPEED_ROOT_DIR}/bins/bin/plugins/$<0:>"
)
