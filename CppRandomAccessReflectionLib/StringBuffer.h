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

/// A faster alternative to std::stringstream; can similarly be used as both an ostream or istream,
/// streaming to a StringBuffer with the << operator will occur much faster than to the ostream,
/// although all stream flags will be ignored; to stream to ostream as regular with stream flags applied
/// and less enhanced performance, cast to a ostream reference or use the "os" stream manipulator;
/// you can stream objects that overload ostream/istream with StringBuffer as usual, stream flags will apply,
/// though overloading streaming to StringBuffer can improve object streaming speed
template <typename StreamType>
class BasicStringBuffer : public std::vector<char>, public std::streambuf, public StreamType
{
    public:
        struct EndL { static constexpr char value = '\n'; };
        struct Os {};
        static constexpr EndL endl = {}; // Use StringBuffer::endl instead of std::endl to improve performance
        static constexpr Os os = {}; // Use this stream manipulator to start streaming as if to std::ostream, avoid this to improve performance
        
        BasicStringBuffer() : StreamType((std::streambuf*)this), source(nullptr), num(), inputInitialized(false) {}
        BasicStringBuffer(std::istream & source) : StreamType((std::streambuf*)this), source(&source), num(), inputInitialized(false) {}
        BasicStringBuffer(const std::string & str) : std::vector<char>(str.begin(), str.end()),
            StreamType((std::streambuf*)this), source(nullptr), num(), inputInitialized(false) {}
        BasicStringBuffer(const std::string & str, std::istream & source) : std::vector<char>(str.begin(), str.end()),
            StreamType((std::streambuf*)this), source(source), num(), inputInitialized(false) {}
        virtual ~BasicStringBuffer() {}

        /// Append text as-is using the += operator
        inline void operator+=(const char & c) { push_back(c); }
        template <size_t N> inline void operator+=(const char (& str)[N]) { insert(std::vector<char>::end(), &str[0], &str[N-1]); }
        inline void operator+=(const std::string & str) { insert(std::vector<char>::end(), str.begin(), str.end()); }

        /// Append text as-is using ostream operators
        inline BasicStringBuffer & operator<<(const char & c) { push_back(c); return *this; }
        template <size_t N> inline BasicStringBuffer & operator<<(const char (& str)[N]) { insert(std::vector<char>::end(), &str[0], &str[N-1]); return *this; }
        inline BasicStringBuffer & operator<<(const std::string & str) { insert(std::vector<char>::end(), str.begin(), str.end()); return *this; }
        
        /// Append regular base-10 numbers using ostream operators
        template <typename T>
        inline BasicStringBuffer & appendNumber(const T & value) {
            if ( auto [p, ec] = std::to_chars(num.data(), num.data() + num.size(), value); ec == std::errc() )
                std::vector<char>::insert(std::vector<char>::end(), num.data(), p);

            return *this;
        }
        inline BasicStringBuffer & operator<<(const short & value) { return appendNumber(value); }
        inline BasicStringBuffer & operator<<(const unsigned short & value) { return appendNumber(value); }
        inline BasicStringBuffer & operator<<(const int & value) { return appendNumber(value); }
        inline BasicStringBuffer & operator<<(const unsigned int & value) { return appendNumber(value); }
        inline BasicStringBuffer & operator<<(const long & value) { return appendNumber(value); }
        inline BasicStringBuffer & operator<<(const unsigned long & value) { return appendNumber(value); }
        inline BasicStringBuffer & operator<<(const long long & value) { return appendNumber(value); }
        inline BasicStringBuffer & operator<<(const unsigned long long & value) { return appendNumber(value); }
        inline BasicStringBuffer & operator<<(const bool & value) { return appendNumber(value); }
        inline BasicStringBuffer & operator<<(const float & value) { return appendNumber(value); }
        inline BasicStringBuffer & operator<<(const double & value) { return appendNumber(value); }
        inline BasicStringBuffer & operator<<(const long double & value) { return appendNumber(value); }

        /// Stream manipulators
        inline BasicStringBuffer & operator<<(const EndL &) { (*this) += EndL::value; return *this; }
        inline std::ostream & operator<<(const Os &) { return (std::ostream &)(*this); }

        /// Append the result of std::ostream << t, avoid when possible to improve performance
        /// This enables streaming objects that overload ostream, overload streaming to StringBuffer where possible to improve performance
        template <typename T> inline BasicStringBuffer & operator<<(const T & t) { (std::ostream &)(*this) << t; return *this; }

        inline void nullTerminate() { if ( empty() || back() != '\0' ) push_back('\0'); }
        inline void unterminate() { if ( !empty() && back() == '\0' ) pop_back(); }
        
        inline void clear() {
            ((std::vector<char>*)this)->clear();
            ((std::ostream*)this)->clear();
            ((std::istream*)this)->clear();
        }

        /// Warning: unless addNulTerminator is false, this will add a nul terminator to the end of the buffer if not already present
        /// This must be manually removed (perhaps by calling unterminate) before adding additional character data to avoid having
        /// a NUL terminator in the middle of string
        inline const char* c_str(bool addNulTerminator = true) { if ( addNulTerminator ) { nullTerminate(); } return &(*this)[0]; }
        inline std::string str() { return std::string(begin(), std::vector<char>::end()); }

    protected:
        /// Appends the character that came through std::ostream
        virtual int overflow(int c) { push_back((char)c); return 0; }

        /// Setup the read for std::istream
        virtual int underflow() {
            auto eback = std::streambuf::eback(); // Input start
            auto gptr = std::streambuf::gptr(); // Input curr
            auto egptr = std::streambuf::egptr(); // Input end

            auto size = std::vector<char>::size()*sizeof(char);
            if ( size == 0 )
            {
                setg(nullptr, nullptr, nullptr);
                inputInitialized = false;
                return std::streambuf::traits_type::eof(); // Return EOF
            }
            else if ( char* start = &((std::vector<char> &)(*this))[0];
                gptr == nullptr || gptr == egptr || (start != eback || start+size > egptr) ) // Changes are required
            {
                if ( inputInitialized && eback != nullptr && gptr != nullptr && egptr != nullptr && gptr >= eback && gptr <= egptr )
                {
                    auto offset = gptr-eback; // Calculate the offset, apply offset to the current data set
                    if ( offset >= (decltype(offset))size || (source != nullptr && source->good()) ) // Input offset was at end of vector or further
                    {
                        *this << source->rdbuf(); // Try reading from source stream
                        start = &((std::vector<char> &)(*this))[0];
                        size = std::vector<char>::size()*sizeof(char);
                    }

                    if ( offset == size ) // Input position is at the end of the vector
                    {
                        setg(start, start+size, start+size); // Set gptr same as egptr
                        return std::streambuf::traits_type::eof(); // Return EOF
                    }
                    else if ( offset > (decltype(offset))size ) // Input position is now out of bounds due to a size reduction of the vector
                    {
                        setg(start, nullptr, start+size); // Set gptr to nullptr as is proper for an underflow failures
                        throw std::out_of_range("Input position: " + std::to_string(offset/sizeof(char))
                            + " is out of range for StringBuffer of size: " + std::to_string(size/sizeof(char)));
                    }
                    else // Offset somewhere within the vector
                    {
                        char* next = start+offset; // Point to the character at offset
                        setg(start, next, start+size); // Set the new input pointers
                        return std::streambuf::traits_type::to_int_type(*next); // Return next character
                    }
                }
                else // Input not initialized or pointers are in an invalid state
                {
                    setg(start, start, start+size); // Set input pointers to vector contents
                    inputInitialized = true;
                    return std::streambuf::traits_type::to_int_type(start[0]); // Return first character
                }
            }
            else // Can perform the read without making any changes to the StringBuffer
                return std::streambuf::traits_type::to_int_type(*gptr); // Return next character
        }

    private:
        bool inputInitialized;
        std::istream* source;
        std::array<char, 256> num;
};

using StringBuffer = BasicStringBuffer<std::iostream>;
using OStringBuffer = BasicStringBuffer<std::ostream>;
using IStringBuffer = BasicStringBuffer<std::istream>;

std::ostream & operator<<(std::ostream & os, BasicStringBuffer<std::ostream> & sb)
{
    return sb.size() > 0 ? os.write(&sb[0], sb.size()) : os;
}

std::ostream & operator<<(std::ostream & os, BasicStringBuffer<std::iostream> & sb)
{
    return sb.size() > 0 ? os.write(&sb[0], sb.size()) : os;
}

/// See StringBufferPtr
template <typename StreamType>
class BasicStringBufferPtr;

template <>
class BasicStringBufferPtr<std::iostream>
{
    public:
        BasicStringBufferPtr(StringBuffer & sb) : sb(&sb), os(nullptr) {}
        BasicStringBufferPtr(std::istream & is) : sb(new StringBuffer(is)), os(nullptr) { *sb << is.rdbuf(); }
        BasicStringBufferPtr(std::ostream & os) : sb(new StringBuffer()), os(&os) {}
        BasicStringBufferPtr(std::iostream & ios) : sb(new StringBuffer(ios)), os(&ios) { *sb << ios.rdbuf(); ((std::ostream*)sb)->clear(); }
        virtual ~BasicStringBufferPtr() {
            if ( os != nullptr )
            {
                *os << *sb;
                delete sb;
            }
        }
        inline StringBuffer & operator*() const { return *sb; }
        inline StringBuffer* operator->() const { return sb; }
        inline void flush() const { if ( os != nullptr ) { *os << *sb; os = nullptr; } }

    private:
        mutable StringBuffer* sb;
        mutable std::ostream* os;
};

template <>
class BasicStringBufferPtr<std::ostream>
{
    public:
        BasicStringBufferPtr(OStringBuffer & sb) : sb(&sb), os(nullptr) {}
        BasicStringBufferPtr(StringBuffer & sb) : sb((OStringBuffer*)&sb), os(nullptr) {}
        BasicStringBufferPtr(std::ostream & os) : sb(new OStringBuffer()), os(&os) {}
        BasicStringBufferPtr(std::iostream & ios) : sb(new OStringBuffer()), os(&ios) {}
        virtual ~BasicStringBufferPtr() {
            if ( os != nullptr )
            {
                *os << *sb;
                delete sb;
            }
        }
        inline OStringBuffer & operator*() const { return *sb; }
        inline OStringBuffer* operator->() const { return sb; }
        inline void flush() const { if ( os != nullptr ) { *os << *sb; os = nullptr; } }

    private:
        mutable OStringBuffer* sb;
        mutable std::ostream* os;
};

template <>
class BasicStringBufferPtr<std::istream>
{
    public:
        BasicStringBufferPtr(IStringBuffer & sb) : sb(&sb) {}
        BasicStringBufferPtr(StringBuffer & sb) : sb((IStringBuffer*)&sb) {}
        BasicStringBufferPtr(std::istream & is) : sb(new IStringBuffer(is)) { *sb >> is.rdbuf(); }
        BasicStringBufferPtr(std::iostream & ios) : sb(new IStringBuffer(ios)) { *sb >> ios.rdbuf(); }
        virtual ~BasicStringBufferPtr() {}
        inline IStringBuffer & operator*() const { return *sb; }
        inline IStringBuffer* operator->() const { return sb; }

    private:
        mutable IStringBuffer* sb;
};

/// Functions accepting StringBufferPtr allow you to pass a reference to one of...
/// StringBuffer, OStringBuffer, IStringBuffer, std::istream, std::ostream, std::iostream, or another StringBufferPtr
/// StringBufferPtrs can be used as though they were pointers to a StringBuffer
/// If constructed with std::istream or std::iostream any existing contents will be read in
/// If constructed with std::ostream or std::iostream all contents will be sent to that stream on destruction
using StringBufferPtr = const BasicStringBufferPtr<std::iostream> &;

/// Functions accepting OStringBufferPtr allow you to pass a reference to one of...
/// OStringBuffer, StringBuffer, std::ostream, std::iostream, or another OStringBufferPtr
/// OStringBufferPtrs can be used as though they were pointers to an OStringBuffer
/// If constructed with std::ostream or std::iostream all contents will be sent to that stream on destruction
using OStringBufferPtr = const BasicStringBufferPtr<std::ostream> &;

/// Functions accepting IStringBufferPtr allow you to pass a reference to one of...
/// IStringbuffer, StringBuffer, std::istream, std::iostream, or another IStringBufferPtr
/// IStringBufferPtrs can be used as though they were pointers to an IStringBuffer
/// If constructed with std::istream or std::iostream any existing contents will be read in
using IStringBufferPtr = const BasicStringBufferPtr<std::istream> &;

#endif