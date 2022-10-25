set(third_party_folder ThirdParty)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(json11)


if(IMGUI)
	 include(imgui.cmake)
	 set_target_properties(imgui PROPERTIES FOLDER ${third_party_folder}/imgui)
endif()

set_target_properties(json11 PROPERTIES FOLDER "3rdparty")
set_target_properties(imgui PROPERTIES FOLDER "3rdparty")

if(OPENGL)
	add_subdirectory(opengl)
endif()

#if(NOT TARGET Jolt)
#    option(TARGET_HELLO_WORLD "" OFF)
#    option(TARGET_PERFORMANCE_TEST "" OFF)
#    option(TARGET_SAMPLES "" OFF)
#    option(TARGET_UNIT_TESTS "" OFF)
#    option(TARGET_VIEWER "" OFF)
#
#    if(ENABLE_PHYSICS_DEBUG_RENDERER)
#        option(TARGET_TEST_FRAMEWORK "" ON)
#    else()
#        option(TARGET_TEST_FRAMEWORK "" OFF)
#    endif()
#
#    add_subdirectory(JoltPhysics/Build)
#
#    if(ENABLE_PHYSICS_DEBUG_RENDERER)
#        set_target_properties(Jolt TestFramework
#            PROPERTIES 
#            FOLDER ${third_party_folder}/JoltPhysics
#            MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
#    else()
#        set_target_properties(Jolt
#            PROPERTIES 
#           FOLDER ${third_party_folder}/JoltPhysics
#            MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
#    endif()
#
#endif()