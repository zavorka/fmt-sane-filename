cmake_minimum_required(VERSION 3.16)
project(fmt-sane-filename)

add_library(midget strbuf.c)
add_executable(fmt-sane-filename fmt-sane-filename.c)
target_link_libraries(fmt-sane-filename midget)
