find_package(Sphinx REQUIRED QUIET)

set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/Docs/Sphinx)

execute_process(COMMAND
	${SPHINX_EXECUTABLE} -b html
	-Dbreathe_projects.Cinder=${DOXYGEN_OUTPUT_DIR}/xml
	${SPHINX_SOURCE} ${SPHINX_BUILD} 
	OUTPUT_QUIET
)
