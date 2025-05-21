
cmake_minimum_required(VERSION 3.5)

project(bayan VERSION 0.1.1) 
if(WIN32)
    set(BOOST_ROOT "C:\\Users\\Tatiana\\vcpkg_\\installed\\x64-windows")
    set(Boost_NO_SYSTEM_PATHS ON)  
    
    set(Boost_ARCHITECTURE "-x64")  

endif()

find_package(Boost REQUIRED COMPONENTS program_options )


add_executable(bayan 
	main.cpp
	 )



set (ALL_SOURCES bayan)

set_target_properties(${ALL_SOURCES} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

foreach(s ${ALL_SOURCES})
	if (MSVC)
		target_compile_options(${s} PRIVATE
			/W4
		)
		
	else ()
		target_compile_options(${s} PRIVATE
			-Wall 
		)
	endif()
endforeach()

target_link_libraries(bayan
    PRIVATE
        Boost::program_options
)

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

