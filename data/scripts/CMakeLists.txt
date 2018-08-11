configure_file (${CMAKE_CURRENT_SOURCE_DIR}/monBat.in ${CMAKE_CURRENT_BINARY_DIR}/monBat)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/monCPU.in ${CMAKE_CURRENT_BINARY_DIR}/monCPU)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/monUSB.in ${CMAKE_CURRENT_BINARY_DIR}/monUSB)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/monWIFI.in ${CMAKE_CURRENT_BINARY_DIR}/monWIFI)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/sysstat.in ${CMAKE_CURRENT_BINARY_DIR}/sysstat)

set(GENMON_DEFAULT_SCRIPTS
    gmail.ex
    monBandwidth
    disktemp
    ${CMAKE_CURRENT_BINARY_DIR}/monCPU
    ${CMAKE_CURRENT_BINARY_DIR}/monUSB
    ${CMAKE_CURRENT_BINARY_DIR}/sysstat
    datetime
    dkspuse
    googcal
    ${CMAKE_CURRENT_BINARY_DIR}/monBat
    monTime
    ${CMAKE_CURRENT_BINARY_DIR}/monWIFI
    twit.ex
)

install(PROGRAMS ${GENMON_DEFAULT_SCRIPTS} DESTINATION "${CMAKE_INSTALL_FULL_LIBEXECDIR}/${CMAKE_PROJECT_NAME}-scripts")
