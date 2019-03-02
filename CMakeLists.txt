# CMake version
cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

# Include cmake modules

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(cmake/Custom.cmake)

# Meta information about the project
set(META_PROJECT_NAME        "FluidSim")

# Declare project
project(${META_PROJECT_NAME} C CXX)

option(GLEW_STATIC "Use static GLEW" True)

find_package(GLFW REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_DEBUG_POSTFIX "d")

set(Target ${META_PROJECT_NAME})

set(CONDITIONCHECK_LEVEL_VARIABLE_NAME "D_CONDITIONCHECK_LEVEL")
set(BinaryInstallDirectory "bin")

set(SourceDirectory                                     "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(GraphicsDirectory                                   "${SourceDirectory}/graphics")
set(SimulationDirectory                                 "${SourceDirectory}/simulation")
set(UiDirectory                                         "${SourceDirectory}/ui")
set(UtilDirectory                                       "${SourceDirectory}/util")
set(Sources
    ${SourceDirectory}/Object.h
    ${SourceDirectory}/Object.cpp
    ${SourceDirectory}/ActiveObject.h
    ${SourceDirectory}/ActiveObject.cpp
    ${SourceDirectory}/MovableObject.h
    ${SourceDirectory}/MovableObject.cpp
    ${SourceDirectory}/Scene.h
    ${SourceDirectory}/Scene.cpp
    ${SourceDirectory}/FluidSim.cpp

    ${GraphicsDirectory}/Arrow.h
    ${GraphicsDirectory}/Arrow.cpp
    ${GraphicsDirectory}/BufferO.h
    ${GraphicsDirectory}/BufferO.cpp
    ${GraphicsDirectory}/Camera.h
    ${GraphicsDirectory}/Camera.cpp
    ${GraphicsDirectory}/DrawableObject.h
    ${GraphicsDirectory}/DrawableObject.cpp
    ${GraphicsDirectory}/Framebuffer.h
    ${GraphicsDirectory}/Framebuffer.cpp
    ${GraphicsDirectory}/Geometry.h
    ${GraphicsDirectory}/Geometry.cpp
    ${GraphicsDirectory}/GridRenderer.h
    ${GraphicsDirectory}/GridRenderer.cpp
    ${GraphicsDirectory}/IndexBufO.h
    ${GraphicsDirectory}/IndexBufO.cpp
    ${GraphicsDirectory}/IndexCombination.h
    ${GraphicsDirectory}/IndexCombination.cpp
    ${GraphicsDirectory}/LightSource.h
    ${GraphicsDirectory}/LightSource.cpp
    ${GraphicsDirectory}/SunLightSource.h
    ${GraphicsDirectory}/SunLightSource.cpp
    ${GraphicsDirectory}/Material.h
    ${GraphicsDirectory}/Material.cpp
    ${GraphicsDirectory}/Program.h
    ${GraphicsDirectory}/Program.cpp
    ${GraphicsDirectory}/Shader.h
    ${GraphicsDirectory}/Shader.cpp
    ${GraphicsDirectory}/Texture2DFixedSize.h
    ${GraphicsDirectory}/Texture2DFixedSize.cpp
    ${GraphicsDirectory}/TracerRenderer.h
    ${GraphicsDirectory}/TracerRenderer.cpp
    ${GraphicsDirectory}/TriangleNetObject.h
    ${GraphicsDirectory}/TriangleNetObject.cpp
    ${GraphicsDirectory}/VectorFieldRenderer.h
    ${GraphicsDirectory}/VectorFieldRenderer.cpp
    ${GraphicsDirectory}/VertexArrO.h
    ${GraphicsDirectory}/VertexArrO.cpp
    ${GraphicsDirectory}/VertexAttribArray.h
    ${GraphicsDirectory}/VertexAttribArray.cpp
    ${GraphicsDirectory}/VertexBufO.h
    ${GraphicsDirectory}/VertexBufO.cpp
    ${GraphicsDirectory}/VortonOctHeapRenderer.h
    ${GraphicsDirectory}/VortonOctHeapRenderer.cpp
    ${GraphicsDirectory}/VortonRenderer.h
    ${GraphicsDirectory}/VortonRenderer.cpp
    ${GraphicsDirectory}/VortonRendererOctHeap.h
    ${GraphicsDirectory}/VortonRendererOctHeap.cpp
    ${GraphicsDirectory}/VortonSimRenderer.h
    ${GraphicsDirectory}/VortonSimRenderer.cpp

    ${SimulationDirectory}/AdvectParticlesOperation.cpp
    ${SimulationDirectory}/AdvectParticlesOperation.h
    ${SimulationDirectory}/ApplyVelocityGridOperation.cpp
    ${SimulationDirectory}/ApplyVelocityGridOperation.h
    ${SimulationDirectory}/EmitParticlesOperation.cpp
    ${SimulationDirectory}/EmitParticlesOperation.h
    ${SimulationDirectory}/FindBoundingBoxOperation.cpp
    ${SimulationDirectory}/FindBoundingBoxOperation.h
    ${SimulationDirectory}/KillParticlesByAgeOperation.cpp
    ${SimulationDirectory}/KillParticlesByAgeOperation.h
    ${SimulationDirectory}/WindOperation.cpp
    ${SimulationDirectory}/WindOperation.h
    ${SimulationDirectory}/Particle.cpp
    ${SimulationDirectory}/Particle.h
    ${SimulationDirectory}/ParticleOperation.cpp
    ${SimulationDirectory}/ParticleOperation.h
    ${SimulationDirectory}/ParticleSystem.cpp
    ${SimulationDirectory}/ParticleSystem.h
    ${SimulationDirectory}/RigidBody.cpp
    ${SimulationDirectory}/RigidBody.h
    ${SimulationDirectory}/RigidBodySim.cpp
    ${SimulationDirectory}/RigidBodySim.h
    ${SimulationDirectory}/RigidBodySphere.cpp
    ${SimulationDirectory}/RigidBodySphere.h
    ${SimulationDirectory}/SolveBoundaryConditionsOperation.cpp
    ${SimulationDirectory}/SolveBoundaryConditionsOperation.h
    ${SimulationDirectory}/SolveBoundaryConditionsVortonsOperation.cpp
    ${SimulationDirectory}/SolveBoundaryConditionsVortonsOperation.h
    ${SimulationDirectory}/Supervorton.cpp
    ${SimulationDirectory}/Supervorton.h
    ${SimulationDirectory}/UniformGrid.cpp
    ${SimulationDirectory}/UniformGridGeometry.cpp
    ${SimulationDirectory}/UniformGridGeometry.h
    ${SimulationDirectory}/UniformGrid.h
    ${SimulationDirectory}/UpdateFluidOperation.cpp
    ${SimulationDirectory}/UpdateFluidOperation.h
    ${SimulationDirectory}/VorticityDistribution.cpp
    ${SimulationDirectory}/VorticityDistribution.h
    ${SimulationDirectory}/Vorton.cpp
    ${SimulationDirectory}/Vorton.h
    ${SimulationDirectory}/VortonOctHeap.cpp
    ${SimulationDirectory}/VortonOctHeapElement.cpp
    ${SimulationDirectory}/VortonOctHeapElement.h
    ${SimulationDirectory}/VortonOctHeap.h

    ${UiDirectory}/GLViewer.h
    ${UiDirectory}/GLViewer.cpp

    ${UtilDirectory}/Debug.h
    ${UtilDirectory}/fsmath.cpp
    ${UtilDirectory}/fsmath.h
    ${UtilDirectory}/Log.h
    ${UtilDirectory}/StrUtils.h

    )

set(group_all "Source and Header Files")
source_group_by_path(${SourceDirectory} "\\\\.h$|\\\\.cpp$|\\\\.inl$" ${group_all} ${Sources})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

add_executable(${Target} ${Sources})

set_target_properties(${Target} PROPERTIES COMPILE_DEFINITIONS "$<$<CONFIG:DEBUG>:${CONDITIONCHECK_LEVEL_VARIABLE_NAME}=0>;$<$<CONFIG:RELEASE>:${CONDITIONCHECK_LEVEL_VARIABLE_NAME}=3>")

target_include_directories(${Target} PUBLIC ${GLEW_INCLUDE_DIRS} ${GLM_INCLUDE_DIR} ${GLFW_INCLUDE_DIR})
target_link_libraries(${Target} ${CMAKE_THREAD_LIBS_INIT} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLFW_LIBRARIES})
if (GLEW_STATIC)
    target_compile_options(${Target} PUBLIC -DGLEW_STATIC=1)
endif()

install(TARGETS ${Target} RUNTIME DESTINATION ${BinaryInstallDirectory} COMPONENT runtime)
