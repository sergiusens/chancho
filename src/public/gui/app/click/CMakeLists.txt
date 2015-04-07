if(CLICK_MODE)
    configure_file(manifest.json.in ${CMAKE_CURRENT_BINARY_DIR}/manifest.json)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/manifest.json ${CMAKE_CURRENT_SOURCE_DIR}/apparmor.json
            DESTINATION ${CMAKE_INSTALL_PREFIX})
endif(CLICK_MODE)
