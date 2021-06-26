IF (NOT Kokkos_ROOT)
  SET(Kokkos_ROOT $ENV{METIS_ROOT})
ENDIF()
#we need to find one of the valid versions from the list below
KOKKOSKERNELS_FIND_IMPORTED(METIS
  LIBRARY metis
  LIBRARY_PATHS ${METIS_LIBRARY_DIRS}
  HEADERS metis.h
  HEADER_PATHS ${METIS_INCLUDE_DIRS})