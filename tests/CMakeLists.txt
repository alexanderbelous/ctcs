add_executable(ctcs_test "ctcs_test.cpp")

target_link_libraries(ctcs_test PUBLIC ctcs::ctcs)

# For Visual Studio: Use Unicode Character Set.
target_compile_definitions(ctcs_test PRIVATE _UNICODE)

# Set warning level
if(MSVC)
  target_compile_options(ctcs_test PRIVATE /W4 /permissive-)
else()
  target_compile_options(ctcs_test PRIVATE -Wall -Wextra -Wpedantic)
endif()
