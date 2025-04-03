add_executable(freertos_ampel
  main.cpp 
  )

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(freertos_ampel pico_stdlib)
pico_enable_stdio_usb(freertos_ampel 1)
pico_enable_stdio_uart(freertos_ampel 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(freertos_ampel)

target_link_libraries(freertos_ampel FreeRTOS pico-cpp)
