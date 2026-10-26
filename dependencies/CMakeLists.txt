# SPDX-FileCopyrightText: 2022 Barndollar Music, Ltd.
#
# SPDX-License-Identifier: Apache-2.0

include(FetchContent)

if(BMMidi_ENABLE_TESTING)
  FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest
      GIT_TAG 063de7e9578f82b369302001269680b4b1553359  # 2026-08-10 (v1.18.0)
      EXCLUDE_FROM_ALL)
  FetchContent_MakeAvailable(googletest)
endif()
