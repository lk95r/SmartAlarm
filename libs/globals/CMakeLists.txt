file(GLOB FILES *.cpp *.h)

add_library(globals ${FILES})

target_link_libraries(globals
pico_stdlib
pico_multicore
hardware_rtc
pico_ss_oled
pico_sync
bme_280
dfplayer
)

target_include_directories(globals PUBLIC ${CMAKE_CURRENT_LIST_DIR})