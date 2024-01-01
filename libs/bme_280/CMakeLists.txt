file(GLOB FILES *.cpp *.h)

add_library(bme_280 ${FILES})



target_link_libraries(bme_280
pico_stdlib
pico_multicore
hardware_timer
pico_multicore
hardware_pio
hardware_clocks
hardware_gpio
hardware_i2c
globals
)

target_include_directories(bme_280 PUBLIC ${CMAKE_CURRENT_LIST_DIR})