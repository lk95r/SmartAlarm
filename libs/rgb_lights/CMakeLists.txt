file(GLOB FILES *.cpp *.h)

add_library(rgb_lights ${FILES})

pico_generate_pio_header(rgb_lights ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)

target_link_libraries(rgb_lights
pico_stdlib
pico_multicore
hardware_timer
pico_multicore
hardware_pio
hardware_clocks
hardware_gpio

)

target_include_directories(rgb_lights PUBLIC ${CMAKE_CURRENT_LIST_DIR})