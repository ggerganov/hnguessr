add_executable(hnguessr main.cpp utils.cpp state.cpp story.cpp state-ui.cpp)
target_include_directories(hnguessr PRIVATE ..)
target_link_libraries(hnguessr PRIVATE imgui imgui-ws ${CMAKE_THREAD_LIBS_INIT})
