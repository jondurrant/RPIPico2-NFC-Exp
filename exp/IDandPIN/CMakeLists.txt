cmake_minimum_required(VERSION 3.12)

set(NAME IDandPIN) # <-- Name your project/executable here!
set(PICO_BOARD pico)
set(PICO_PLATFORM rp2040)

include("$ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake")
project(${NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
# Initialize the SDK
pico_sdk_init()

set(PICONFC_PATH "${CMAKE_CURRENT_LIST_DIR}/../../lib/piconfc/")
include(piconfc.cmake)

set(SSD1306_DIR "${CMAKE_CURRENT_LIST_DIR}/../../lib/pico-ssd1306")
include(ssd1306.cmake)

add_subdirectory(src)

#Set up files for the release packages
install(CODE "execute_process(COMMAND $ENV{HOME}/bin/picoDeploy.sh ${CMAKE_CURRENT_BINARY_DIR}/src/${NAME}.elf)")

# Set up files for the release packages
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/src/${NAME}.uf2
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_GENERATOR "ZIP" "TGZ")
include(CPack)
