include(FindPkgConfig)
if (HAVE_REVENGE)
	pkg_check_modules(LIBQXP libqxp-0.0)
	if (LIBQXP_FOUND)
		message("Building QuarkXPress Importer")
		include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/scribus ${LIBREVENGE_INCLUDE_DIRS} ${LIBREVENGE_GENERATORS_INCLUDE_DIRS} ${LIBREVENGE_STREAM_INCLUDE_DIRS} ${LIBQXP_INCLUDE_DIRS})
		set(IMPORTQXP_PLUGIN_MOC_CLASSES importqxp.h importqxpplugin.h)
		set(IMPORTQXP_PLUGIN_SOURCES ../revenge/rawpainter.cpp importqxp.cpp importqxpplugin.cpp)
		set(SCRIBUS_IMPORTQXP_PLUGIN "importqxp")
		QT5_WRAP_CPP(IMPORTQXP_PLUGIN_MOC_SOURCES ${IMPORTQXP_PLUGIN_MOC_CLASSES})
		add_library(${SCRIBUS_IMPORTQXP_PLUGIN} MODULE ${IMPORTQXP_PLUGIN_SOURCES} ${IMPORTQXP_PLUGIN_MOC_SOURCES})
		target_link_libraries(${SCRIBUS_IMPORTQXP_PLUGIN} ${PLUGIN_LIBRARIES} ${LIBREVENGE_LDFLAGS} ${LIBREVENGE_GENERATORS_LDFLAGS} ${LIBREVENGE_STREAM_LDFLAGS} ${LIBQXP_LDFLAGS})
		install(TARGETS ${SCRIBUS_IMPORTQXP_PLUGIN} LIBRARY DESTINATION ${PLUGINDIR} PERMISSIONS ${PLUGIN_PERMISSIONS})
		add_dependencies(${SCRIBUS_IMPORTQXP_PLUGIN} ${EXE_NAME})
	endif ()
endif ()
