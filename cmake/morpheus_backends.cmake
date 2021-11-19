# Kokkos only defines the variables if the backends are ON Define aux variables
# that exist as on/off
macro(CHECK_KOKKOS_BACKEND BE)
  if(Kokkos_ENABLE_${BE})
    set(MORPHEUS_ENABLE_${BE} ON)
  else()
    set(MORPHEUS_ENABLE_${BE} OFF)
  endif()
  global_set(Morpheus_ENABLE_${BE} ${MORPHEUS_ENABLE_${BE}})
endmacro(CHECK_KOKKOS_BACKEND)

check_kokkos_backend(CUDA)
check_kokkos_backend(HIP)
check_kokkos_backend(OPENMP)
check_kokkos_backend(SERIAL)
