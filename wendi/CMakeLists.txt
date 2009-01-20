ADD_EXECUTABLE(wendi wendi.cc CodeBlock.cc output_graph.cc output_txt.cc)
TARGET_LINK_LIBRARIES(wendi ${SDL_LIBRARY} wenboicore)
INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIR})

