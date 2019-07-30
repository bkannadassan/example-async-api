cmake_minimum_required(VERSION 3.1)

set(project_name example-async-api) ## rename your project here

project(${project_name})

set(CMAKE_CXX_STANDARD 11)

add_library(${project_name}-client-lib
        src/PWClientApiModels.hpp  
	src/PWClientAsync.hpp  
	src/PWClient.hpp
	src/PWClient.cpp)

add_library(${project_name}-lib
        src/AppComponent.hpp
        src/hello-world.cpp
        src/controller/MyController.cpp
        src/controller/MyController.hpp
        src/PWTCPConnectionProvider.cpp
        src/PWTCPConnectionProvider.hpp
        src/PWServer.hpp
        src/PWServer.cpp
        src/dto/MyDTOs.hpp
)

## link libs

find_package(oatpp 0.19.6 REQUIRED)
find_package(oatpp-swagger REQUIRED)
find_package(oatpp-curl 0.19.6 REQUIRED)

target_link_libraries(${project_name}-lib
        PUBLIC oatpp::oatpp
        PUBLIC oatpp::oatpp-curl
        PUBLIC oatpp::oatpp-test
)

target_link_libraries(${project_name}-client-lib
        PUBLIC oatpp::oatpp
        PUBLIC oatpp::oatpp-curl
        PUBLIC oatpp::oatpp-test
)

SET(CMAKE_CXX_FLAGS  "-levent -g -lefence")

target_include_directories(${project_name}-client-lib PUBLIC src)
target_include_directories(${project_name}-lib PUBLIC src)
include_directories("/usr/local/include/oatpp-0.19.6/oatpp-swagger")
include_directories("/usr/local/include/oatpp-0.19.6/oatpp")
include_directories("/usr/local/include/oatpp-0.19.6/oatpp-curl")

## add executables

add_executable(${project_name}-exe
        src/App.cpp
)
add_executable(${project_name}-client-exe
        src/AsyncExample.cpp
        src/AsyncExample.hpp
        src/App-Client.cpp
)
target_link_libraries(${project_name}-exe ${project_name}-lib)
target_link_libraries(${project_name}-client-exe ${project_name}-client-lib)
add_dependencies(${project_name}-exe ${project_name}-lib)
add_dependencies(${project_name}-client-exe ${project_name}-client-lib)

add_executable(${project_name}-test
	test/tests.cpp
)
target_link_libraries(${project_name}-test ${project_name}-lib)
add_dependencies(${project_name}-test ${project_name}-lib)

set_target_properties(${project_name}-lib ${project_name}-exe ${project_name}-test PROPERTIES
        CXX_STANDARD 11
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
)

set_target_properties(${project_name}-lib ${project_name}-client-exe ${project_name}-test PROPERTIES
        CXX_STANDARD 11
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
)
enable_testing()
add_test(project-tests ${project_name}-test)
