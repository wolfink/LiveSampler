#include <JuceHeader.h>
#if(_DEBUG)
#include "debug/Debug.h"
#endif

// Parameter Macros
#define IN_VOLUME_ID     "in volume"
#define IN_VOLUME_NAME   "Input Volume"
#define OUT_VOLUME_ID    "out volume"
#define OUT_VOLUME_NAME  "Output Volume"
#define MIX_ID           "mix"
#define MIX_NAME         "Mix"
#define SHIFT_FACTOR_ID   "pitch shift"
#define SHIFT_FACTOR_NAME "Pitch Shift"

template<class T>
using uptr = std::unique_ptr<T>;
