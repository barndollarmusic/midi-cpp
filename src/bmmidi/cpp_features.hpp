#ifndef BMMIDI_CPP_FEATURES_HPP
#define BMMIDI_CPP_FEATURES_HPP

// C++17 inline variable support:
#ifndef BMMIDI_INLINE_VAR
  #ifdef __cpp_inline_variables
    #define BMMIDI_INLINE_VAR inline
  #else
    #define BMMIDI_INLINE_VAR
  #endif
#endif

#endif  // BMMIDI_CPP_FEATURES_HPP
