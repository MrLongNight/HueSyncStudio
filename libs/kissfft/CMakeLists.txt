add_library(kissfft STATIC
    kiss_fft.c
    kiss_fft.h
    _kiss_fft_guts.h
)

target_include_directories(kissfft PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
