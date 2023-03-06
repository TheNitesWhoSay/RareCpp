#undef USE_BUFFERED_STREAMS

#define GET_RUNJSONTESTSRC_INCLUDES
#include "json_test.cpp"
#undef GET_RUNJSONTESTSRC_INCLUDES

namespace Unbuffered
{
#define GET_RUNJSONTESTSRC_CPP
#include "json_test.cpp"
#undef GET_RUNJSONTESTSRC_CPP
}
