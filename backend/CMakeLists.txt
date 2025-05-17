cmake_minimum_required(VERSION 3.12)

# Add knapsack library
file(GLOB KNAPSACK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(knapsack ${KNAPSACK_SOURCES})

# Set C++ standard
target_compile_features(knapsack PUBLIC cxx_std_17)

# Platform-specific compiler flags
if(MSVC)
  # MSVC compiler flags (Windows)
  target_compile_options(knapsack PRIVATE /W4)
else()
  # GCC/Clang compiler flags (Linux, macOS)
  target_compile_options(knapsack PRIVATE -Wall -Wextra)
endif()

# Include directories - using generator expressions for better compatibility
target_include_directories(knapsack PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Create main executable
add_executable(knapsack_main
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

# Print the GLSL version being used
message(STATUS "Using GLSL version: ${GLSL_VERSION}")

# Link the executable with the knapsack library
target_link_libraries(knapsack_main PRIVATE knapsack)

# Platform-specific library linking
if(APPLE)
  # Find the full paths to the SFML libraries
  find_library(SFML_GRAPHICS_LIBRARY NAMES sfml-graphics PATHS /opt/homebrew/lib REQUIRED)
  find_library(SFML_WINDOW_LIBRARY NAMES sfml-window PATHS /opt/homebrew/lib REQUIRED)
  find_library(SFML_SYSTEM_LIBRARY NAMES sfml-system PATHS /opt/homebrew/lib REQUIRED)
  find_library(SFML_AUDIO_LIBRARY NAMES sfml-audio PATHS /opt/homebrew/lib REQUIRED)
  find_library(SFML_NETWORK_LIBRARY NAMES sfml-network PATHS /opt/homebrew/lib REQUIRED)
  
  # Link SFML and ImGui to the main executable
  target_link_libraries(knapsack_main PRIVATE 
      ${SFML_GRAPHICS_LIBRARY}
      ${SFML_WINDOW_LIBRARY}
      ${SFML_SYSTEM_LIBRARY}
      ${SFML_AUDIO_LIBRARY}
      ${SFML_NETWORK_LIBRARY}
      imgui
      ${OPENGL_LIBRARIES}
  )
elseif(WIN32)
  # On Windows, use the SFML CMake targets directly
  target_link_libraries(knapsack_main PRIVATE 
      sfml-graphics 
      sfml-window 
      sfml-system
      sfml-audio
      sfml-network
      imgui
      ${OPENGL_LIBRARIES}
      opengl32 # Explicitly link OpenGL on Windows
  )
  
  # Copy SFML DLLs to output directory for Windows
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SFML_DLLS 
      "E:/Libraries/SFML-3.0.0/bin/sfml-graphics-d-3.dll"
      "E:/Libraries/SFML-3.0.0/bin/sfml-window-d-3.dll"
      "E:/Libraries/SFML-3.0.0/bin/sfml-system-d-3.dll"
      "E:/Libraries/SFML-3.0.0/bin/sfml-audio-d-3.dll"
      "E:/Libraries/SFML-3.0.0/bin/sfml-network-d-3.dll"
    )
  else()
    set(SFML_DLLS 
      "E:/Libraries/SFML-3.0.0/bin/sfml-graphics-3.dll"
      "E:/Libraries/SFML-3.0.0/bin/sfml-window-3.dll"
      "E:/Libraries/SFML-3.0.0/bin/sfml-system-3.dll"
      "E:/Libraries/SFML-3.0.0/bin/sfml-audio-3.dll"
      "E:/Libraries/SFML-3.0.0/bin/sfml-network-3.dll"
    )
  endif()
  
  # Add post-build command to copy DLLs
  foreach(DLL ${SFML_DLLS})
    add_custom_command(TARGET knapsack_main POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${DLL}
      $<TARGET_FILE_DIR:knapsack_main>
    )
  endforeach()
else()
  # Linux and other platforms
  target_link_libraries(knapsack_main PRIVATE 
      sfml-graphics 
      sfml-window 
      sfml-system
      sfml-audio
      sfml-network
      imgui
      ${OPENGL_LIBRARIES}
  )
endif()

# Install targets (optional)
include(GNUInstallDirs)
install(TARGETS knapsack knapsack_main 
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
