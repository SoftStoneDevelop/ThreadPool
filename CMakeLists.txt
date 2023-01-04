include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env.cmake: ${LOCAL_ENV}")

cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(NAME ThreadPool)
project(${NAME} VERSION 1.0.0)

# Add source to this project's executable.
file(GLOB_RECURSE SOURCES 
${PROJECT_SOURCE_DIR}/Src/*.cpp
)
add_executable(${PROJECT_NAME} ${SOURCES})

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

if (WIN32)
  message(STATUS "CREATING BUILD FOR WINDOWS")
  target_include_directories(${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    )
elseif (UNIX)
    message(STATUS "CREATING BUILD FOR UNIX")
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

    target_include_directories(${PROJECT_NAME} PUBLIC
      ${PROJECT_SOURCE_DIR}/src
    )
endif()