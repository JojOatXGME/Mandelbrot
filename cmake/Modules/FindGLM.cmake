# - Try to find header for GLM
# Once done this will define
#  GLM_FOUND - System has GLM headers
#  GLM_INCLUDE_DIRS - The GLM include directories

if (NOT GLM_ROOT_DIR)
	set(GLM_ROOT_DIR "$ENV{GLM_ROOT_DIR}")
endif()

find_path(GLM_INCLUDE_DIR
	NAMES "glm/glm.hpp"
	PATHS
	${GLM_ROOT_DIR})

set(GLM_INCLUDE_DIRS "${GLM_INCLUDE_DIR}")

find_package_handle_standard_args(GLM DEFAULT_MSG
	GLM_INCLUDE_DIR)
