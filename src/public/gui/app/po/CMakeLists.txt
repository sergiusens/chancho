include(FindGettext)
find_program(GETTEXT_XGETTEXT_EXECUTABLE xgettext)

message("-- Project name is ${PROJECT_NAME}")
set(POT_FILE ${PROJECT_NAME}.pot)
file(GLOB PO_FILES *.po)

# Creates the .pot file containing the translations template
add_custom_target(${POT_FILE} ALL
    COMMENT "Generating translation template"
    COMMAND ${INTLTOOL_EXTRACT} --update --type=gettext/ini
        --srcdir=${CMAKE_SOURCE_DIR}
    COMMAND ${GETTEXT_XGETTEXT_EXECUTABLE} -o ${POT_FILE}
        -D ${CMAKE_CURRENT_SOURCE_DIR}
        -D ${CMAKE_CURRENT_BINARY_DIR}
        --from-code=UTF-8
        --c++ --qt --add-comments=TRANSLATORS
        --keyword=tr --keyword=tr:1,2 --keyword=N_
        --package-name='${PROJECT_NAME}'
        --copyright-holder='Manuel de la Pena <mandel@themacaque.com>'
        ${I18N_SRC_FILES}
    COMMAND ${CMAKE_COMMAND} -E copy ${POT_FILE} ${CMAKE_CURRENT_SOURCE_DIR})

# Builds the binary translations catalog for each language
# it finds source translations (*.po) for
foreach(PO_FILE ${PO_FILES})
    get_filename_component(LANG ${PO_FILE} NAME_WE)
    gettext_process_po_files(${LANG} ALL PO_FILES ${PO_FILE})
    set(INSTALL_DIR ${CMAKE_INSTALL_LOCALEDIR}/${LANG}/LC_MESSAGES)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${LANG}.gmo
            DESTINATION ${INSTALL_DIR}
            RENAME ${PROJECT_NAME}.mo)
endforeach(PO_FILE)
