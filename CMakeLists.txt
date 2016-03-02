cmake_minimum_required(VERSION 2.8)	 # Проверка версии CMake.
					# Если версия установленой программы
					# старее указаной, произайдёт аварийный выход.

project(ap_server)

set(SOURCE_EXE server.c)

add_executable(final ${SOURCE_EXE})

target_link_libraries(final pthread)
