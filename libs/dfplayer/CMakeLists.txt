file(GLOB FILES *.cpp *.h)

add_library(dfplayer ${FILES})

target_link_libraries(dfplayer
pico_stdlib
pico_multicore
hardware_rtc
hardware_uart
pico_ss_oled
bme_280
pico_sync
)

target_include_directories(dfplayer PUBLIC ${CMAKE_CURRENT_LIST_DIR})