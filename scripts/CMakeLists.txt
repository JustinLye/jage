if(WIN32)
  set(VENV_PYTHON "${CMAKE_SOURCE_DIR}/.conan/Scripts/python.exe")
else()
  set(VENV_PYTHON "${CMAKE_SOURCE_DIR}/.conan/bin/python")
endif()

add_custom_target(run_scripts_tests ALL
  COMMAND "${VENV_PYTHON}" -m pytest ${CMAKE_CURRENT_SOURCE_DIR}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  USES_TERMINAL
  COMMENT "Running Scripts Tests"
)
