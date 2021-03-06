#-------------------------------------------------
#
# Project created by QtCreator 2015-10-29T23:14:11
#
#-------------------------------------------------

QT       -= gui

TARGET = pe_lib
TEMPLATE = lib

DEFINES += PE_LIB_LIBRARY
DEFINES += PE_BLISS_WINDOWS

SOURCES += \
    entropy.cpp \
    file_version_info.cpp \
    message_table.cpp \
    pe_base.cpp \
    pe_bound_import.cpp \
    pe_checksum.cpp \
    pe_debug.cpp \
    pe_directory.cpp \
    pe_dotnet.cpp \
    pe_exception.cpp \
    pe_exception_directory.cpp \
    pe_exports.cpp \
    pe_factory.cpp \
    pe_imports.cpp \
    pe_load_config.cpp \
    pe_properties.cpp \
    pe_properties_generic.cpp \
    pe_rebuilder.cpp \
    pe_relocations.cpp \
    pe_resource_manager.cpp \
    pe_resource_viewer.cpp \
    pe_resources.cpp \
    pe_rich_data.cpp \
    pe_section.cpp \
    pe_tls.cpp \
    resource_bitmap_reader.cpp \
    resource_bitmap_writer.cpp \
    resource_cursor_icon_reader.cpp \
    resource_cursor_icon_writer.cpp \
    resource_data_info.cpp \
    resource_message_list_reader.cpp \
    resource_string_table_reader.cpp \
    resource_version_info_reader.cpp \
    resource_version_info_writer.cpp \
    utils.cpp \
    version_info_editor.cpp \
    version_info_viewer.cpp

HEADERS +=\
        pe_lib_global.h \
    entropy.h \
    file_version_info.h \
    message_table.h \
    pe_base.h \
    pe_bliss.h \
    pe_bliss_resources.h \
    pe_bound_import.h \
    pe_checksum.h \
    pe_debug.h \
    pe_directory.h \
    pe_dotnet.h \
    pe_exception.h \
    pe_exception_directory.h \
    pe_exports.h \
    pe_factory.h \
    pe_imports.h \
    pe_load_config.h \
    pe_properties.h \
    pe_properties_generic.h \
    pe_rebuilder.h \
    pe_relocations.h \
    pe_resource_manager.h \
    pe_resource_viewer.h \
    pe_resources.h \
    pe_rich_data.h \
    pe_section.h \
    pe_structures.h \
    pe_tls.h \
    resource_bitmap_reader.h \
    resource_bitmap_writer.h \
    resource_cursor_icon_reader.h \
    resource_cursor_icon_writer.h \
    resource_data_info.h \
    resource_internal.h \
    resource_message_list_reader.h \
    resource_string_table_reader.h \
    resource_version_info_reader.h \
    resource_version_info_writer.h \
    stdint_defs.h \
    utils.h \
    version_info_editor.h \
    version_info_types.h \
    version_info_viewer.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix|win32: LIBS += -liconv
