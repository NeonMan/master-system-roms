project(bios C)

# BIOS menu ROM
ADD_EXECUTABLE(bios-menu 
        bios-menu.c
        bm-state.c
        system-info.c
        rom-tools.c
        rom-info.c
        mapper-test.c
		x-bootloader.c
        )
ADD_DEPENDENCIES(bios-menu crt0-sms)
TARGET_LINK_LIBRARIES(bios-menu crc16-xmodem uart sms vdp console io)

ADD_CUSTOM_TARGET(bios-menu-bin ALL
        COMMAND makebin -s 32768 bios-menu.ihx bios-menu.sms
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/fix-checksum.py bios-menu.sms
		DEPENDS bios-menu
    )