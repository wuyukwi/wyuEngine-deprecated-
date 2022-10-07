set(ENGINE_ROOT_DIR "${ROOT_DIR}/engine")
set(THIRD_PARTY_DIR "${ENGINE_ROOT_DIR}/3rdparty")
set(ENGINE_ASSET_DIR "/asset")

add_subdirectory(3rdparty)

set(TARGET_NAME Engine)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE ENGINE_HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${ENGINE_HEADERS} ${ENGINE_SOURCES})

add_library(${TARGET_NAME} ${ENGINE_HEADERS} ${ENGINE_SOURCES})

#option(ENABLE_PHYSICS_DEBUG_RENDERER "Enable Physics Debug Renderer" OFF)
# only support physics debug render at windows platform
#if(NOT WIN32)
#  if(ENABLE_PHYSICS_DEBUG_RENDERER)
#    message(WARNING "Disable Physics Debug Renderer")
#    set(ENABLE_PHYSICS_DEBUG_RENDERER OFF CACHE BOOL "" FORCE)
#  endif()
#endif()
#if(ENABLE_PHYSICS_DEBUG_RENDERER)
#  set(JOLT_ASSET_DIR "/jolt-asset")
#endif()

set(d3d9_lib ${THIRD_PARTY_DIR}/d3d9/lib/x64/d3d9.lib)
set(d3dx9_lib ${THIRD_PARTY_DIR}/d3d9/lib/x64/d3dx9.lib)
set(dinput8_lib ${THIRD_PARTY_DIR}/d3d9/lib/x64/dinput8.lib)
set(xinput.lib ${THIRD_PARTY_DIR}/d3d9/lib/x64/xinput.lib)
set(dxguid_lib ${THIRD_PARTY_DIR}/d3d9/lib/x64/dxguid.lib)
set(xaudio2_lib xaudio2.lib)

target_link_libraries(${TARGET_NAME} PUBLIC ${d3d9_lib})
target_link_libraries(${TARGET_NAME} PUBLIC ${d3dx9_lib})
target_link_libraries(${TARGET_NAME} PUBLIC ${dinput8_lib})
target_link_libraries(${TARGET_NAME} PUBLIC ${xinput.lib})
target_link_libraries(${TARGET_NAME} PUBLIC ${dxguid_lib})
target_link_libraries(${TARGET_NAME} PUBLIC ${xaudio2_lib})
target_link_libraries(${TARGET_NAME} PUBLIC json11)
#target_link_libraries(${TARGET_NAME} PUBLIC Jolt)

if(IMGUI)
	target_link_libraries(${TARGET_NAME} PUBLIC imgui)
endif()


target_include_directories(${TARGET_NAME}
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
	${THIRD_PARTY_DIR}/d3d9/include
)
