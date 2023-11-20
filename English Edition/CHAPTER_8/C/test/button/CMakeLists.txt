if (TARGET tinyusb_device)
    add_executable(button
            button.c
            )

    target_link_libraries(button pico_stdlib)

    pico_enable_stdio_usb(button 1)
    pico_enable_stdio_uart(button 0)

    pico_add_extra_outputs(button)


    example_auto_set_url(button)
elseif(PICO_ON_DEVICE)
    message(WARNING "not building button")
endif()
