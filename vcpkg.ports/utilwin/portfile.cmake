include(vcpkg_common_functions)
# find_program(NMAKE nmake)
# vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

set(SOURCE_PATH "${CURRENT_PORT_DIR}/../../src" )

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA
)

# set(LIBRARY_TYPE STATIC)
set(LIBRARY_TYPE "shared")
set(IMPORT_TYPE LOCATION)

vcpkg_install_cmake()

# file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include)
file(WRITE "${CURRENT_PACKAGES_DIR}/share/utilwin/copyright" "private")

set(VCPKG_POLICY_ALLOW_RESTRICTED_HEADERS enabled)
