add_library(core_1
${CMAKE_CURRENT_LIST_DIR}/core_1.cpp
${CMAKE_CURRENT_LIST_DIR}/core_1.h
${CMAKE_CURRENT_LIST_DIR}/wifi_data.h
)

target_link_libraries(core_1
pico_stdlib
pico_multicore
hardware_rtc
pico_cyw43_arch_lwip_threadsafe_background
)