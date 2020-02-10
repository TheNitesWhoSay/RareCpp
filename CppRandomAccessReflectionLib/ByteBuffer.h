#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H
#include <vector>
#include <streambuf>
#include <ostream>
#include <istream>
#include <system_error>
#include <cstdint>

class ByteBuffer : public std::vector<uint8_t>, public std::streambuf, public std::ostream, public std::istream
{
    public:
        struct Os {};
        static constexpr Os os = {}; // Use this stream manipulator to start streaming as if to std::ostream, avoid this to improve performance

        ByteBuffer() : std::ostream(this), std::istream(this) {}
        virtual ~ByteBuffer() {}

        /// Stream manipulators
        inline std::ostream & operator<<(const Os &) { return (std::ostream &)(*this); }

        /// Append the result of std::ostream << t, avoid when possible to improve performance
        /// This enables streaming objects that overload ostream, overload streaming to Byte Buffer where possible to improve performance
        template <typename T> inline ByteBuffer & operator<<(const T & t) { (std::ostream &)(*this) << t; return *this; }

    protected:
        /// Appends the character that came through std::ostream
        virtual int overflow(int c) { push_back((uint8_t)c); return 0; }

        // Setup the read for std::istream
        virtual int underflow() {
            if ( gptr() == egptr() )
            {
                uint8_t* start = &((*this)[0]);
                uint8_t* end = start + std::vector<uint8_t>::size();
                setg((char*)start, (char*)start, (char*)end);
            }
            return gptr() == egptr() ? std::char_traits<char>::eof() : std::char_traits<char>::to_int_type(*gptr());
        }
};

std::ostream & operator<<(std::ostream & os, ByteBuffer & sb)
{
    return os.write((char*)&sb[0], sb.size());
}

#endif