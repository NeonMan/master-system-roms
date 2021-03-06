project(relocatable-roms C)
ENABLE_LANGUAGE(C)

#Configure SDCC to make z80 binaries
set(CMAKE_C_FLAGS "-mz80 --std-sdcc89 --xram-size 0x1000 --code-loc 0xc070 --data-loc 0xd800 --no-std-crt0 \"${CMAKE_BINARY_DIR}/sms/crt0-sms-ram.rel\"")
set(CMAKE_EXE_LINKER_FLAGS "-Wl -w")

ADD_EXECUTABLE(rel-sample rel-sample.c)
ADD_DEPENDENCIES(rel-sample crt0-sms-ram)
TARGET_LINK_LIBRARIES(rel-sample console vdp)

ADD_EXECUTABLE(simple-bios simple-bios.c)
ADD_DEPENDENCIES(simple-bios crt0-sms-ram)
TARGET_LINK_LIBRARIES(simple-bios io)

ADD_EXECUTABLE(xmdump xmdump.c)
ADD_DEPENDENCIES(xmdump crt0-sms-ram)
TARGET_LINK_LIBRARIES(xmdump uart crc16-xmodem console vdp)

ADD_EXECUTABLE(test-canary test-canary.c)
ADD_DEPENDENCIES(test-canary crt0-sms-ram)
TARGET_LINK_LIBRARIES(test-canary console vdp)

ADD_EXECUTABLE(test-mapper test-mapper.c)
ADD_DEPENDENCIES(test-mapper crt0-sms-ram)
TARGET_LINK_LIBRARIES(test-mapper console vdp uart)

ADD_EXECUTABLE(uart-sync uart-sync.c)
ADD_DEPENDENCIES(uart-sync crt0-sms-ram)
TARGET_LINK_LIBRARIES(uart-sync console vdp uart)

#Make .bin ROMs from the .ihx files
ADD_CUSTOM_TARGET(uart-sync-bin ALL
        COMMAND makebin -p -s 57344 uart-sync.ihx uart-sync-raw.sms
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/trim.py uart-sync-raw.sms uart-sync.sms 49152
		COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/fix-checksum.py uart-sync-raw.sms 8
		DEPENDS uart-sync
    )
	
ADD_CUSTOM_TARGET(rel-sample-bin ALL
        COMMAND makebin -p -s 57344 rel-sample.ihx rel-sample-raw.sms
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/trim.py rel-sample-raw.sms rel-sample.sms 49152
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/fix-checksum.py rel-sample-raw.sms 8
		DEPENDS rel-sample
    )

ADD_CUSTOM_TARGET(simple-bios-bin ALL
        COMMAND makebin -p -s 57344 simple-bios.ihx simple-bios-raw.sms
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/trim.py simple-bios-raw.sms simple-bios.sms 49152
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/fix-checksum.py simple-bios-raw.sms 8
		DEPENDS simple-bios
    )

ADD_CUSTOM_TARGET(xmdump-bin ALL
        COMMAND makebin -p -s 57344 xmdump.ihx xmdump-raw.sms
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/trim.py xmdump-raw.sms xmdump.sms 49152
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/fix-checksum.py xmdump-raw.sms 8
		DEPENDS xmdump
    )

ADD_CUSTOM_TARGET(test-canary-bin ALL
        COMMAND makebin -p -s 57344 test-canary.ihx test-canary-raw.sms
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/trim.py test-canary-raw.sms test-canary.sms 49152
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/fix-checksum.py test-canary-raw.sms 8
		DEPENDS test-canary
    )

ADD_CUSTOM_TARGET(test-mapper-bin ALL
        COMMAND makebin -p -s 57344 test-mapper.ihx test-mapper-raw.sms
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/trim.py test-mapper-raw.sms test-mapper.sms 49152
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/fix-checksum.py test-mapper-raw.sms 8
		DEPENDS test-mapper
    )
