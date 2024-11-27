
include(ExternalProject)
include(cmake/util.cmake)

if (ENABLE_NNDEPLOY_RAPIDJSON STREQUAL "OFF")
elseif (ENABLE_NNDEPLOY_RAPIDJSON STREQUAL "ON")
  add_definitions(-DENABLE_NNDEPLOY_RAPIDJSON)
  option(RAPIDJSON_BUILD_DOC "Build rapidjson documentation." OFF)
  option(RAPIDJSON_BUILD_EXAMPLES "Build rapidjson examples." OFF)
  option(RAPIDJSON_BUILD_TESTS "Build rapidjson perftests and unittests." OFF)
  option(RAPIDJSON_BUILD_THIRDPARTY_GTEST
    "Use gtest installation in `thirdparty/gtest` by default if available" OFF)
  set(LIBS RapidJSON)
  set(RAPIDJSON_ROOT ${PROJECT_SOURCE_DIR}/third_party/rapidjson)
  add_subdirectory_if_no_target(${RAPIDJSON_ROOT} ${LIBS})
  include_directories(${RAPIDJSON_ROOT}/include)
  set(NNDEPLOY_THIRD_PARTY_LIBRARY ${NNDEPLOY_THIRD_PARTY_LIBRARY} ${LIBS})
else()
endif()