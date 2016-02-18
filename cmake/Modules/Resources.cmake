add_executable(embedfile "${CMAKE_CURRENT_LIST_DIR}/embedfile.c")
set_target_properties(embedfile PROPERTIES LINKER_LANGUAGE C)
set_target_properties(embedfile PROPERTIES C_STANDARD 11)

function(ADD_RESOURCES target header)
	# create name for header guard
	string(REGEX REPLACE "[^0-9a-zA-Z]" "_" header_guard "${header}")
	string(TOUPPER "${header_guard}" header_guard)

	# get abolute path of header
	get_filename_component(header "${CMAKE_CURRENT_BINARY_DIR}/include/${header}" ABSOLUTE)

	# write header guard to file and add includes
	file(WRITE "${header}" "#ifndef ${header_guard}\n#define ${header_guard}\n")
	file(APPEND "${header}" "#include <string>\n")

	# iterate over all files and create list of cfiles
	set(cfiles)
	foreach(ifile ${ARGN})
		# get file information
		get_filename_component(ifile "${ifile}" ABSOLUTE)
		file(RELATIVE_PATH rpath "${CMAKE_SOURCE_DIR}" "${ifile}")
		# get varname
		string(REGEX REPLACE "[/\\]+" ";" varname "${rpath}")
		string(REGEX REPLACE "[^0-9a-zA-Z;]" "_" varname "${varname}")
		# create path for c-file
		set(cfile "${CMAKE_CURRENT_BINARY_DIR}/${rpath}.cpp")
		get_filename_component(cfile_dir "${cfile}" DIRECTORY)
		file(MAKE_DIRECTORY "${cfile_dir}")
		# create c-file on build
		add_custom_command(
			OUTPUT "${cfile}"
			COMMAND embedfile "${ifile}" "${cfile}" ${varname}
			DEPENDS "${ifile}" embedfile)
		# save path to cfile in `cfiles`
		list(APPEND cfiles "${cfile}")
		# add variable to header
		list(GET varname -1 varname_last)
		list(REMOVE_AT varname -1)
		foreach(namespace ${varname})
			file(APPEND "${header}" "namespace ${namespace} {")
		endforeach()
		file(APPEND "${header}" "extern const std::string ${varname_last};")
		foreach(namespace ${varname})
			file(APPEND "${header}" "}")
		endforeach()
		file(APPEND "${header}" "\n")
	endforeach()

	# close header guard
	file(APPEND "${header}" "#endif\n")

	# create target
	add_library("${target}" STATIC ${cfiles})
	target_include_directories("${target}" PUBLIC
		"${CMAKE_CURRENT_BINARY_DIR}/include/")
	set_target_properties("${target}" PROPERTIES LINKER_LANGUAGE CXX)
	set_target_properties("${target}" PROPERTIES CXX_STANDARD 11)
endfunction()
