# 收集源文件和头文件
set(SOURCES
    main.cpp
    mainwindow.cpp

)

set(HEADERS
    mainwindow.h
    constants/constants.h
)

# 资源文件
set(QRC_FILES
    resources/styles.qrc
    resources/images.qrc
)

# 创建可执行文件
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(my-admin
        MANUAL_FINALIZATION
        ${SOURCES}
        ${HEADERS}
    )
else()
    add_executable(my-admin
        ${SOURCES}
        ${HEADERS}
    )
endif()

# 处理资源文件
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    # 为每个资源文件设置相对路径别名
    foreach(qrc_file ${QRC_FILES})
        get_filename_component(qrc_name ${qrc_file} NAME)
        set_source_files_properties(${qrc_file} PROPERTIES
            QT_RESOURCE_ALIAS ${qrc_name}
        )
    endforeach()
    
    qt_add_resources(my-admin "app_resources"
        PREFIX "/"
        FILES ${QRC_FILES}
    )
else()
    # Qt5处理方式
    qt5_add_resources(RCC_SOURCES ${QRC_FILES})
    target_sources(my-admin PRIVATE ${RCC_SOURCES}
        constants/constants.h)
endif()

# IDE虚拟文件夹分组 - 统一组织为Source Files和Header Files主文件夹

# 处理源文件(.cpp)，放在Source Files主文件夹下
foreach(source_file ${SOURCES})
    # 获取文件所在目录的相对路径
    get_filename_component(file_dir "${source_file}" DIRECTORY)
    if(file_dir STREQUAL "")
        # 根目录下的文件，直接放在Source Files下
        source_group("Source Files" FILES ${source_file})
    else()
        # 子目录下的文件，放在Source Files/子目录下
        string(REPLACE "/" "\\" group_name "Source Files\\${file_dir}")
        source_group("${group_name}" FILES ${source_file})
    endif()
endforeach()

# 处理头文件(.h)，放在Header Files主文件夹下
foreach(header_file ${HEADERS})
    # 获取文件所在目录的相对路径
    get_filename_component(file_dir "${header_file}" DIRECTORY)
    if(file_dir STREQUAL "")
        # 根目录下的文件，直接放在Header Files下
        source_group("Header Files" FILES ${header_file})
    else()
        # 子目录下的文件，放在Header Files/子目录下
        string(REPLACE "/" "\\" group_name "Header Files\\${file_dir}")
        source_group("${group_name}" FILES ${header_file})
    endif()
endforeach()

# 处理资源文件，放在Resources主文件夹下
foreach(qrc_file ${QRC_FILES})
    # 获取文件所在目录的相对路径
    get_filename_component(file_dir "${qrc_file}" DIRECTORY)
    if(file_dir STREQUAL "")
        # 根目录下的文件，直接放在Resources下
        source_group("Resources" FILES ${qrc_file})
    else()
        # 子目录下的文件，放在Resources/子目录下
        string(REPLACE "/" "\\" group_name "Resources\\${file_dir}")
        source_group("${group_name}" FILES ${qrc_file})
    endif()
endforeach()


# 对于Qt 6，仍然需要FILE_SET组织头文件用于构建
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    target_sources(my-admin PRIVATE
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES ${HEADERS}
    )
endif()

# 设置目标属性
set_target_properties(my-admin PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    OUTPUT_NAME "my-admin"
    # 设置PDB文件输出目录（Windows调试信息）
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)

# 包含目录
target_include_directories(my-admin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/constants
    # ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
    # ${CMAKE_CURRENT_SOURCE_DIR}/models
    # ${CMAKE_CURRENT_SOURCE_DIR}/widgets
    # ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

# 链接库
target_link_libraries(my-admin PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)

# 添加编译定义
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(my-admin PRIVATE DEBUG=1)
endif()

# Qt6最终化
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_finalize_executable(my-admin)
endif()

# 安装配置
install(TARGETS my-admin
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# 安装资源文件（如果需要）
install(DIRECTORY resources/images/ DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/images)
install(DIRECTORY resources/styles/ DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/styles)

# 添加单元测试支持
find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)

# 测试用例
enable_testing()

message(STATUS "${PROJECT_NAME} 配置完成")
