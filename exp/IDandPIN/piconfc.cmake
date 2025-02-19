if (DEFINED PICONFC_PATH)
	message("Using Given PICONFC_PATH '${PICONFC_PATH}')")
else ()
	set(PICONFC_PATH "${CMAKE_CURRENT_LIST_DIR}/lib/piconfc/")
    message("Using local PICONFC_PATH '${PICONFC_PATH}')")
endif ()

add_subdirectory(${PICONFC_PATH}/src build/piconfc)