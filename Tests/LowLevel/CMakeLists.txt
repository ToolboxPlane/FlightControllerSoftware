add_subdirectory(Mock)

make_test(Drivers/bno055_uart)
target_link_libraries(Test_Drivers_bno055_uart PRIVATE Mock_HAL_uart)

make_test(Drivers/bno055)
target_link_libraries(Test_Drivers_bno055 PRIVATE Mock_Drivers_bno055_uart)

make_test(Drivers/ring_buffer)

make_test(Drivers/sbus)
target_link_libraries(Test_Drivers_sbus PRIVATE Mock_HAL_uart)

make_test(Drivers/ppm)
target_link_libraries(Test_Drivers_ppm PRIVATE Mock_HAL_pwm16bit)
target_link_libraries(Test_Drivers_ppm PRIVATE Mock_avr_io)

make_test(Drivers/protobuf)
target_link_libraries(Test_Drivers_protobuf PRIVATE Mock_HAL_uart)
target_link_libraries(Test_Drivers_protobuf PRIVATE Mock_Messages_MessageDecoding)
target_link_libraries(Test_Drivers_protobuf PRIVATE Mock_Messages_MessageEncoding)

make_test(Components/imu)
target_link_libraries(Test_Components_imu PRIVATE Mock_Drivers_bno055)
target_link_libraries(Test_Components_imu PRIVATE Mock_util_delay)
target_link_libraries(Test_Components_imu PRIVATE Mock_Application_error_handler)

make_test(Components/remote)
target_link_libraries(Test_Components_remote PRIVATE Mock_Drivers_sbus)

make_test(Components/flightcomputer)
target_link_libraries(Test_Components_flightcomputer PRIVATE Mock_Drivers_protobuf)

make_test(Components/actuators)
target_link_libraries(Test_Components_actuators PRIVATE Mock_Drivers_ppm)

make_test(Components/system)
target_link_libraries(Test_Components_system PRIVATE Mock_avr_io)
target_link_libraries(Test_Components_system PRIVATE Mock_avr_wdt)
target_link_libraries(Test_Components_system PRIVATE Mock_avr_interrupt)
target_link_libraries(Test_Components_system PRIVATE Mock_HAL_timer8bit)
target_link_libraries(Test_Components_system PRIVATE Mock_Application_error_handler)

make_test(Application/controller)

make_test(Application/error_handler)
target_link_libraries(Test_Application_error_handler PRIVATE Mock_avr_io)
target_link_libraries(Test_Application_error_handler PRIVATE Mock_avr_wdt)

make_test(Application/mode_handler)
target_link_libraries(Test_Application_mode_handler PRIVATE Mock_Application_error_handler)
target_link_libraries(Test_Application_mode_handler PRIVATE Mock_Components_imu)
target_link_libraries(Test_Application_mode_handler PRIVATE Mock_Components_remote)
target_link_libraries(Test_Application_mode_handler PRIVATE Mock_Components_flightcomputer)

add_custom_target(RunAllTests DEPENDS ${AllTests})

add_custom_target(coverage.info
        COMMAND lcov --capture --directory ${CMAKE_CURRENT_BINARY_DIR} --output-file ${CMAKE_CURRENT_BINARY_DIR}/coverage.info
        DEPENDS ${AllTests})

add_custom_target(Coverage
        COMMAND genhtml ${CMAKE_CURRENT_BINARY_DIR}/coverage.info --output-directory ${CMAKE_CURRENT_BINARY_DIR}/coverage
        DEPENDS coverage.info)

add_custom_target(report.xml
        COMMAND junit2html . --merge report.xml
        DEPENDS ${AllTests})

add_custom_target(LLReport
        COMMAND junit2html report.xml --report-matrix LLReport.html
        DEPENDS report.xml)
