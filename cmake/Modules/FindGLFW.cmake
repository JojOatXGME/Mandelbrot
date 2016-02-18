# - Try to find GLFW
# Once done this will define
#  GLFW_FOUND - System has GLFW
#  GLFW_INCLUDE_DIRS - The GLFW include directories
#  GLFW_LIBRARIES - The libraries needed to use GLFW

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(GLFW_PKGCONF GLFW)

# Include dir
find_path(GLFW_INCLUDE_DIR
	NAMES "GLFW/glfw3.h"
	PATHS ${GLFW_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(GLFW_LIBRARY
	NAMES "glfw"
	PATHS ${GLFW_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(GLFW_PROCESS_INCLUDES GLFW_INCLUDE_DIR)
set(GLFW_PROCESS_LIBS GLFW_LIBRARY)
libfind_process(GLFW)
