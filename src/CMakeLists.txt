# 收集源文件和头文件

set(HEADERS
    mainwindow.h
    constants/constants.h
    constants/enums.h
    redis/redisclient.h
    models/connectionconfig.h
    widgets/connectionpanel.h
    widgets/leftmenupanel.h
    widgets/righttopwidget.h
    widgets/keymanagerpage.h
    widgets/datasummarypage.h
    widgets/monitormenupage.h
    widgets/configmenupage.h
    delegates/base/basedelegate.h
    delegates/connectionpanel/connectionpaneldelegate.h
    dialogs/connectiondialog.h
    dialogs/settingsdialog.h
    dialogs/addkeydialog.h
    utils/fontmanager.h
    utils/dpitools.h
    utils/commonhelper.h
)

set(SOURCES
    main.cpp
    mainwindow.cpp
    redis/redisclient.cpp
    models/connectionconfig.cpp
    widgets/connectionpanel.cpp
    widgets/leftmenupanel.cpp
    widgets/righttopwidget.cpp
    widgets/keymanagerpage.cpp
    widgets/datasummarypage.cpp
    widgets/monitormenupage.cpp
    widgets/configmenupage.cpp
    delegates/base/basedelegate.cpp
    delegates/connectionpanel/connectionpaneldelegate.cpp
    dialogs/connectiondialog.cpp
    dialogs/settingsdialog.cpp
    dialogs/addkeydialog.cpp
    utils/fontmanager.cpp
    utils/dpitools.cpp
    utils/commonhelper.cpp
)

set(APP_PLATFORM_SOURCES)
if(WIN32)
    list(APPEND APP_PLATFORM_SOURCES app.rc)
endif()

# 资源文件
set(QRC_FILES
    resources/styles.qrc
    resources/images.qrc
)

# 创建可执行文件
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(RedisDesk
        MANUAL_FINALIZATION
        ${SOURCES}
        ${HEADERS}
        ${APP_PLATFORM_SOURCES}
    )
else()
    add_executable(RedisDesk
        ${SOURCES}
        ${HEADERS}
        ${APP_PLATFORM_SOURCES}
    )
endif()

# 处理资源文件
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    foreach(qrc_file ${QRC_FILES})
        get_filename_component(qrc_name ${qrc_file} NAME)
        set_source_files_properties(${qrc_file} PROPERTIES
            QT_RESOURCE_ALIAS ${qrc_name}
        )
    endforeach()

    qt_add_resources(RedisDesk "app_resources"
        PREFIX "/"
        FILES ${QRC_FILES}
    )
else()
    qt5_add_resources(RCC_SOURCES ${QRC_FILES})
    target_sources(RedisDesk PRIVATE ${RCC_SOURCES})
endif()

# IDE虚拟文件夹分组
foreach(source_file ${SOURCES})
    get_filename_component(file_dir "${source_file}" DIRECTORY)
    if(file_dir STREQUAL "")
        source_group("Source Files" FILES ${source_file})
    else()
        string(REPLACE "/" "\\" group_name "Source Files\\${file_dir}")
        source_group("${group_name}" FILES ${source_file})
    endif()
endforeach()

foreach(header_file ${HEADERS})
    get_filename_component(file_dir "${header_file}" DIRECTORY)
    if(file_dir STREQUAL "")
        source_group("Header Files" FILES ${header_file})
    else()
        string(REPLACE "/" "\\" group_name "Header Files\\${file_dir}")
        source_group("${group_name}" FILES ${header_file})
    endif()
endforeach()

foreach(qrc_file ${QRC_FILES})
    get_filename_component(file_dir "${qrc_file}" DIRECTORY)
    if(file_dir STREQUAL "")
        source_group("Resources" FILES ${qrc_file})
    else()
        string(REPLACE "/" "\\" group_name "Resources\\${file_dir}")
        source_group("${group_name}" FILES ${qrc_file})
    endif()
endforeach()

foreach(platform_file ${APP_PLATFORM_SOURCES})
    source_group("Resources" FILES ${platform_file})
endforeach()

# 头文件组织
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    target_sources(RedisDesk PRIVATE
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES ${HEADERS}
    )
endif()

# 设置目标属性
set_target_properties(RedisDesk PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    OUTPUT_NAME "RedisDesk"
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)

# 包含目录
target_include_directories(RedisDesk PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/constants
)

# 链接库
target_link_libraries(RedisDesk PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)

# 添加编译定义
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(RedisDesk PRIVATE DEBUG=1)
endif()

# 设置编译器编码选项
if(MSVC)
    target_compile_options(RedisDesk PRIVATE /utf-8)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(RedisDesk PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# Qt6最终化
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_finalize_executable(RedisDesk)
endif()

# 安装配置
install(TARGETS RedisDesk
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY resources/images/ DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/images)
install(DIRECTORY resources/styles/ DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/styles)

message(STATUS "${PROJECT_NAME} 配置完成")
