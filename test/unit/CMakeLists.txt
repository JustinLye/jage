function(add_unit_test)
  cmake_parse_arguments(PARSE_ARGV 0 ARG "" "TARGET_NAME"
                        "SOURCE_FILES;DEPENDANTS;LINK_LIBS")
  if(DEFINED ARG_UNPARSED_ARGS)
    message(
      FATAL_ERROR
        "Error! Unrecogized arguments passed to 'add_unit_test': ${ARG_UNPARSED_ARGS}."
    )
  elseif(NOT DEFINED ARG_TARGET_NAME)
    message(
      FATAL_ERROR "Error! TARGET_NAME argument is required by 'add_unit_test'.")
  elseif(NOT DEFINED ARG_SOURCE_FILES)
    message(
      FATAL_ERROR "Error! SOURCE_FILES argument is required by 'add_unit_test'."
    )
  else()
    list(LENGTH ARG_SOURCE_FILES SOURCE_FILES_SIZE)
    if(1 GREATER ${SOURCE_FILES_SIZE})
      message(
        FATAL_ERROR
          "Error! Must provide 1 or more SOURCE_FILES to 'add_unit_test'. Number of source files provided is ${SOURCE_FILES_SIZE}."
      )
    endif()
  endif()

  set(LINK_LIBS jage::test::lib)
  if(DEFINED ARG_LINK_LIBS)
    list(APPEND LINK_LIBS ${ARG_LINK_LIBS})
  endif()

  set(EXECUTABLE_TARGET_NAME jage_unit_test_${ARG_TARGET_NAME})
  add_executable(${EXECUTABLE_TARGET_NAME} ${ARG_SOURCE_FILES})
  target_link_libraries(${EXECUTABLE_TARGET_NAME} ${LINK_LIBS})

  set(RUN_TARGET_NAME run_${EXECUTABLE_TARGET_NAME})
  add_custom_target(
    ${RUN_TARGET_NAME}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE_TARGET_NAME}
    DEPENDS ${EXECUTABLE_TARGET_NAME})

  add_dependencies(run_all_jage_unit_tests ${RUN_TARGET_NAME})

  if(DEFINED ARG_DEPENDANTS)
    foreach(DEPENDANT ${ARG_DEPENDANTS})
      add_dependencies(${DEPENDANT} ${RUN_TARGET_NAME})
    endforeach()
  endif()

endfunction()

add_custom_target(run_all_jage_unit_tests)
add_subdirectory(jage)
