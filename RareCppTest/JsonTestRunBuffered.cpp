#define USE_BUFFERED_STREAMS

#define GET_RUNJSONTESTSRC_INCLUDES
#include "JsonTest.cpp"
#undef GET_RUNJSONTESTSRC_INCLUDES

namespace Buffered
{
#define GET_RUNJSONTESTSRC_CPP
#include "JsonTest.cpp"
#undef GET_RUNJSONTESTSRC_CPP
}

#undef USE_BUFFERED_STREAMS
