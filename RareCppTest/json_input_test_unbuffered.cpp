#undef USE_BUFFERED_STREAMS

#define GET_RUNJSONINPUTTESTSRC_INCLUDES
#include "json_input_test.cpp"
#undef GET_RUNJSONINPUTTESTSRC_INCLUDES

namespace Unbuffered
{
#define GET_RUNJSONINPUTTESTSRC_CPP
#include "json_input_test.cpp"
#undef GET_RUNJSONINPUTTESTSRC_CPP
}
