set(GENMON_DEFAULT_SCRIPTS
    disktemp
    gmail
    monBandwidth
    monCPU
    monUSB
    sysstat
    datetime
    dkspuse
    googcal
    monBat
    monTime
    monWIFI
    twit
)

install(PROGRAMS ${GENMON_DEFAULT_SCRIPTS} DESTINATION "${CMAKE_INSTALL_FULL_LIBEXECDIR}/${CMAKE_PROJECT_NAME}-scripts")
