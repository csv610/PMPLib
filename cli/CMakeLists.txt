add_executable(meshinfo meshinfo.cpp)
target_link_libraries(meshinfo pmp)

add_executable(meshfilter meshfilter.cpp)
target_link_libraries(meshfilter pmp)

add_executable(meshsimplify meshsimplify.cpp)
target_link_libraries(meshsimplify pmp)

add_executable(meshsubdivide meshsubdivide.cpp)
target_link_libraries(meshsubdivide pmp)

add_executable(meshfill meshfill.cpp)
target_link_libraries(meshfill pmp)

add_executable(meshfair meshfair.cpp)
target_link_libraries(meshfair pmp)

add_executable(meshparam meshparam.cpp)
target_link_libraries(meshparam pmp)

add_executable(meshremesh meshremesh.cpp)
target_link_libraries(meshremesh pmp)

add_custom_target(cli)
add_dependencies(cli meshinfo meshfilter meshsimplify meshsubdivide meshfill meshfair meshparam meshremesh)