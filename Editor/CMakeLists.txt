file(GLOB_RECURSE EDITOR_SOURCE CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp
)
add_executable(IgnisEditor ${EDITOR_SOURCE})

target_include_directories(IgnisEditor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source)

target_link_libraries(IgnisEditor PRIVATE Ignis)
