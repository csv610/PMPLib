set(CMAKE_DISABLE_DEPRECATED_WARNINGS "-Wno-deprecated-declarations")

find_program(ARGPARSE_EXECUTABLE argparse)

function(add_cli_tool NAME)
    add_executable(${NAME} ${NAME}.cpp)
    target_link_libraries(${NAME} pmp)
    target_include_directories(${NAME} PRIVATE ${argparse_SOURCE_DIR}/include)
    set_source_files_properties(${NAME}.cpp PROPERTIES COMPILE_FLAGS "${CMAKE_DISABLE_DEPRECATED_WARNINGS}")
endfunction()

add_cli_tool(meshinfo)
add_cli_tool(meshfilter)
add_cli_tool(meshsimplify)
add_cli_tool(meshsubdivide)
add_cli_tool(meshfill)
add_cli_tool(meshfair)
add_cli_tool(meshparam)
add_cli_tool(meshremesh)

add_custom_target(cli)
add_dependencies(cli meshinfo meshfilter meshsimplify meshsubdivide meshfill meshfair meshparam meshremesh)