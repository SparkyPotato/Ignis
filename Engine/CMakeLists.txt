set(THREADS_PREFER_PTHREAD_FLAG ON)

file(GLOB_RECURSE ENGINE_SOURCE CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/Include/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp
)
add_library(Ignis SHARED ${ENGINE_SOURCE})

target_include_directories(Ignis PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)
target_include_directories(Ignis PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source)

target_compile_features(Ignis PUBLIC cxx_std_20)
target_compile_definitions(Ignis PRIVATE BUILD_IGNIS)

find_package(Threads REQUIRED)
target_link_libraries(Ignis PRIVATE Threads::Threads)
