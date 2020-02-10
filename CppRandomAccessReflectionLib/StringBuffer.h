#ifndef STRINGBUFFER_H
#define STRINGBUFFER_H
#include <vector>
#include <streambuf>
#include <ostream>
#include <istream>
#include <string>
#include <array>
#include <charconv>
#include <system_error>

/**
    A faster alternative to std::stringstream; can similarly be used as both an ostream or istream,
    streaming to a StringBuffer with the << operator will occur much faster than to the ostream,
    although all stream flags will be ignored; to stream to ostream as regular with stream flags applied
    and less enhanced performance, cast to a ostream reference or use the "os" stream manipulator;
    you can stream objects that overload ostream/istream with StringBuffer as usual, stream flags will apply,
    though overloading streaming to StringBuffer can improve object streaming speed
*/
class StringBuffer : public std::vector<char>, public std::streambuf, public std::ostream, public std::istream
{
    public:
        struct EndL { static constexpr char value = '\n'; };
        struct Os {};
        static constexpr EndL endl = {}; // Use StringBuffer::endl instead of std::endl to improve performance
        static constexpr Os os = {}; // Use this stream manipulator to start streaming as if to std::ostream, avoid this to improve performance

        StringBuffer() : std::ostream(this), std::istream(this), num() {}
        StringBuffer(const std::string & str) : std::vector<char>(str.begin(), str.end()), std::ostream(this), std::istream(this), num() {}
        virtual ~StringBuffer() {}

        /// Append text as-is using the += operator
        inline void operator+=(const char & c) { push_back(c); }
        template <size_t N> inline void operator+=(const char (& str)[N]) { insert(std::vector<char>::end(), &str[0], &str[N-1]); }
        inline void operator+=(const std::string & str) { insert(std::vector<char>::end(), str.begin(), str.end()); }

        /// Append text as-is using ostream operators
        inline StringBuffer & operator<<(const char & c) { push_back(c); return *this; }
        template <size_t N> inline StringBuffer & operator<<(const char (& str)[N]) { insert(std::vector<char>::end(), &str[0], &str[N-1]); return *this; }
        inline StringBuffer & operator<<(const std::string & str) { insert(std::vector<char>::end(), str.begin(), str.end()); return *this; }
        
        /// Append regular base-10 numbers using ostream operators
        template <typename T>
        inline StringBuffer & appendNumber(const T & value) {
            if ( auto [p, ec] = std::to_chars(num.data(), num.data() + num.size(), value); ec == std::errc() )
                std::vector<char>::insert(std::vector<char>::end(), num.data(), p);

            return *this;
        }
        inline StringBuffer & operator<<(const short & value) { return appendNumber(value); }
        inline StringBuffer & operator<<(const unsigned short & value) { return appendNumber(value); }
        inline StringBuffer & operator<<(const int & value) { return appendNumber(value); }
        inline StringBuffer & operator<<(const unsigned int & value) { return appendNumber(value); }
        inline StringBuffer & operator<<(const long & value) { return appendNumber(value); }
        inline StringBuffer & operator<<(const unsigned long & value) { return appendNumber(value); }
        inline StringBuffer & operator<<(const long long & value) { return appendNumber(value); }
        inline StringBuffer & operator<<(const unsigned long long & value) { return appendNumber(value); }
        inline StringBuffer & operator<<(const bool & value) { return appendNumber(value); }
        inline StringBuffer & operator<<(const float & value) { return appendNumber(value); }
        inline StringBuffer & operator<<(const double & value) { return appendNumber(value); }
        inline StringBuffer & operator<<(const long double & value) { return appendNumber(value); }

        /// Stream manipulators
        inline StringBuffer & operator<<(const EndL &) { (*this) += EndL::value; return *this; }
        inline std::ostream & operator<<(const Os &) { return (std::ostream &)(*this); }

        /// Append the result of std::ostream << t, avoid when possible to improve performance
        /// This enables streaming objects that overload ostream, overload streaming to StringBuffer where possible to improve performance
        template <typename T> inline StringBuffer & operator<<(const T & t) { (std::ostream &)(*this) << t; return *this; }

        inline void nullTerminate() { if ( empty() || back() != '\0' ) push_back('\0'); }
        inline void unterminate() { if ( !empty() && back() == '\0' ) pop_back(); }
        
        inline void clear() {
            ((std::vector<char>*)this)->clear();
            ((std::ostream*)this)->clear();
            ((std::istream*)this)->clear();
        }

        inline size_t size() { return std::vector<char>::size(); }

        /// Warning: unless addNulTerminator is false, this will add a nul terminator to the end of the buffer if not already present
        /// This must be manually removed (perhaps by calling unterminate) before adding additional character data to avoid having
        /// a NUL terminator in the middle of string
        inline const char* c_str(bool addNulTerminator = true) { if ( addNulTerminator ) { nullTerminate(); } return &(*this)[0]; }
        inline std::string str() { return std::string(begin(), std::vector<char>::end()); }

    protected:
        /// Appends the character that came through std::ostream
        virtual int overflow(int c) { push_back((char)c); return 0; }

        // Setup the read for std::istream
        virtual int underflow() {
            if ( gptr() == egptr() )
            {
                char* start = &((*this)[0]);
                char* end = start + size();
                setg(start, start, end);
            }
            return gptr() == egptr() ? std::char_traits<char>::eof() : std::char_traits<char>::to_int_type(*gptr());
        }

    private:
        std::array<char, 256> num;
};

std::ostream & operator<<(std::ostream & os, StringBuffer & sb)
{
    return sb.size() > 0 ? os.write(&sb[0], sb.size()) : os;
}

#endif