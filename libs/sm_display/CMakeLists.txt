file(GLOB FILES *.cpp *.h)

add_library(sm_display ${FILES})

target_link_libraries(sm_display
pico_stdlib
pico_multicore
hardware_rtc
pico_ss_oled
dfplayer
bme_280
globals
)

target_include_directories(sm_display PUBLIC ${CMAKE_CURRENT_LIST_DIR})