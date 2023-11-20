if (TARGET tinyusb_device)
    add_executable(hello_world
            hello_world.c
            )

    target_link_libraries(hello_world pico_stdlib)

    pico_enable_stdio_usb(hello_world 1)
    pico_enable_stdio_uart(hello_world 0)

    pico_add_extra_outputs(hello_world)

    example_auto_set_url(hello_world)
elseif(PICO_ON_DEVICE)
    message(WARNING "not building hello_world")
endif()
