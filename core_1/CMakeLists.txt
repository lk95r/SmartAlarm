add_library(core_1
${CMAKE_CURRENT_LIST_DIR}/core_1.cpp
${CMAKE_CURRENT_LIST_DIR}/core_1.h
)

target_link_libraries(core_1
pico_stdlib
pico_multicore
hardware_rtc
hardware_adc
pico_ss_oled
pico_sync
globals
)