
cmake_minimum_required(VERSION 3.5)

project(bayan VERSION 0.1.1) 
if(WIN32)
    	set(BOOST_ROOT "C:\\Users\\Tatiana\\vcpkg\\installed\\x64-windows")
	# set(Boost_DEBUG ON)
	set(Boost_NO_SYSTEM_PATHS ON)

	set(OPENSSL_ROOT_DIR "C:\\Users\\Tatiana\\vcpkg\\packages\\openssl_x64-windows-static")
	#set(Boost_USE_STATIC_LIBS ON)


endif()

find_package(Boost REQUIRED COMPONENTS program_options )
find_package(OpenSSL REQUIRED)

if(Boost_FOUND)
    message(STATUS "Boost found: ${Boost_VERSION}")
    message(STATUS "Include dirs: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Library dirs: ${Boost_LIBRARY_DIRS}")
else()
    message(FATAL_ERROR "Boost NOT found!")

endif()  
  
message(STATUS "OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message(STATUS "OpenSSL libraries: ${OPENSSL_LIBRARIES}")

add_executable(bayan 
	main.cpp	
	parse_command_line.h
	parse_command_line.cpp
	process.h
	process.cpp
	settings.h
	block_hash.h
	block_hash.cpp
	 )

target_include_directories(bayan PRIVATE ${Boost_INCLUDE_DIRS})

set (ALL_SOURCES bayan)

set_target_properties(${ALL_SOURCES} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

foreach(s ${ALL_SOURCES})
	if (MSVC)
		target_compile_options(${s} PRIVATE
			/W3
		)
		
	else ()
		target_compile_options(${s} PRIVATE
			-Wall 
		)
	endif()
endforeach()

target_link_libraries(bayan  Boost::program_options)
target_link_libraries(bayan  OpenSSL::SSL OpenSSL::Crypto)

if(WIN32)
    target_link_libraries(bayan ws2_32)
    target_link_libraries(bayan Crypt32)
endif()

install(TARGETS bayan RUNTIME DESTINATION bin)

# Настройки CPack
set(CPACK_GENERATOR "DEB") 
set(CPACK_PACKAGE_NAME "bayan")
set(CPACK_PACKAGE_FILE_NAME "bayan")  
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "t.o.nikolaeva@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "find duplicates")

# Отключаем ненужные генераторы
set(CPACK_SOURCE_GENERATOR "")  # Отключаем генерацию исходников
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY FALSE)

include(CPack)

