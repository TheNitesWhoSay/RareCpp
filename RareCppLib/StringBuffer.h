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
#include <ios>

namespace BufferedStream
{
    struct EndL
    {
        static constexpr char value = '\n';
    };
    struct Os {};

    /// A faster alternative to std::stringstream; can similarly be used as both an ostream or istream,
    /// streaming to a StringBuffer with the << operator will occur much faster than to the ostream,
    /// although all stream flags will be ignored; to stream to ostream as regular with stream flags applied
    /// and less enhanced performance, cast to a ostream reference or use the "os" stream manipulator;
    /// you can stream objects that overload ostream/istream with StringBuffer as usual, stream flags will apply,
    /// though overloading streaming to StringBuffer can improve object streaming speed
    template <typename StreamType>
    class BasicStringBuffer : public std::streambuf, public StreamType
    {
        public:
            static constexpr EndL endl = {}; // Use StringBuffer::endl instead of std::endl to improve performance
            static constexpr Os os = {}; // Use this stream manipulator to start streaming as if to std::ostream, avoid this to improve performance

            BasicStringBuffer() : StreamType((std::streambuf*)this), src(nullptr), num(), inputInitialized(false) {}
            BasicStringBuffer(std::istream & source) : StreamType((std::streambuf*)this), src(&source), num(), inputInitialized(false) {}
            BasicStringBuffer(const std::string & str) : StreamType((std::streambuf*)this),
                data(str.begin(), str.end()), src(nullptr), num(), inputInitialized(false) {}
            BasicStringBuffer(const std::string & str, std::istream & source) : StreamType((std::streambuf*)this),
                data(str.begin(), str.end()), src(&source), num(), inputInitialized(false) {}

            virtual ~BasicStringBuffer() {}

            /// Append text as-is using the += operator
            inline void operator+=(const char & c)
            {
                data.push_back(c);
            }
            template <size_t N> inline void operator+=(const char (& str)[N])
            {
                data.insert(data.end(), &str[0], &str[N-1]);
            }
            inline void operator+=(const char* str)
            {
                data.insert(data.end(), str, str+strlen(str)*sizeof(char));
            }
            inline void operator+=(const std::string & str)
            {
                data.insert(data.end(), str.begin(), str.end());
            }
            
            /// Append text as-is using ostream operators
            inline BasicStringBuffer & operator<<(const char & c)
            {
                data.push_back(c);
                return *this;
            }
            template <size_t N> inline BasicStringBuffer & operator<<(const char (& str)[N])
            {
                data.insert(data.end(), &str[0], &str[N-1]);
                return *this;
            }
            inline BasicStringBuffer & operator<<(const char* str)
            {
                data.insert(data.end(), str, str+strlen(str)*sizeof(char));
                return *this;
            }
            inline BasicStringBuffer & operator<<(const std::string & str)
            {
                data.insert(data.end(), str.begin(), str.end());
                return *this;
            }
        
            /// Append regular base-10 numbers
            template <typename T>
            inline BasicStringBuffer & appendNumber(const T & value) {
                if constexpr ( std::is_same_v<bool, T> )
                {
                    if ( auto [p, ec] = std::to_chars((char*)num.data(), (char*)num.data() + num.size(), (int)value); ec == std::errc() )
                        data.insert(data.end(), num.data(), p);
                }
                else if ( auto [p, ec] = std::to_chars((char*)num.data(), (char*)num.data() + num.size(), value); ec == std::errc() )
                    data.insert(data.end(), num.data(), p);

                return *this;
            }
            
#ifdef __GNUC__
            inline BasicStringBuffer & appendNumber(const float & value) {
                (*this) += std::to_string(value);
                return *this;
            }
            inline BasicStringBuffer & appendNumber(const double & value) {
                (*this) += std::to_string(value);
                return *this;
            }
            inline BasicStringBuffer & appendNumber(const long double & value) {
                (*this) += std::to_string(value);
                return *this;
            }
#endif
        
            /// Append regular base-10 numbers using ostream operators
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
            inline BasicStringBuffer & operator<<(const EndL &)
            {
                (*this) += EndL::value;
                return *this;
            }
            inline std::ostream & operator<<(const Os &)
            {
                return (std::ostream &)(*this);
            }

            /// Append the result of std::ostream << t, avoid when possible to improve performance
            /// This enables streaming objects that overload ostream, overload streaming to StringBuffer where possible to improve performance
            template <typename T> inline BasicStringBuffer & operator<<(const T & t)
            {
                syncOutput();
                (std::ostream &)(*this) << t;
                return *this;
            }

            inline void nullTerminate()
            {
                if ( data.empty() || data.back() != '\0' )
                    data.push_back('\0');
            }
            inline void unterminate()
            {
                if ( !data.empty() && data.back() == '\0' )
                    data.pop_back();
            }
        
            size_t size() const noexcept
            {
                return data.size();
            }

            inline void clear()
            {
                data.clear();
                ((std::ostream*)this)->clear();
                ((std::istream*)this)->clear();
            }

            const char & operator[](size_t pos) const
            {
                return data[pos];
            }

            /// Warning: unless addNulTerminator is false, this will add a nul terminator to the end of the buffer if not already present
            /// This must be manually removed (perhaps by calling unterminate) before adding additional character data to avoid having
            /// a NUL terminator in the middle of string
            inline const char* c_str(bool addNulTerminator = true)
            {
                if ( addNulTerminator )
                    nullTerminate();

                return &(*this)[0];
            }

            inline std::string str() const
            {
                return std::string(data.begin(), data.end());
            }

            inline const std::vector<char> & vec() const
            {
                return data;
            }

            inline void swap(std::vector<char> & other)
            {
                data.swap(other);
                sync();
            }

            inline void swap(BasicStringBuffer<StreamType> & other)
            {
                data.swap(other.data);
                sync();
                other.sync();
            }

            virtual int sync()
            {
                syncInput();
                syncOutput();
                return 0;
            }

            std::istream* source()
            {
                return src;
            }

        protected:
            /// Returns true if gptr points to a valid next character, false otherwise
            inline bool syncInput()
            {
                auto eback = std::streambuf::eback(); // Input start
                auto gptr = std::streambuf::gptr(); // Input curr
                auto egptr = std::streambuf::egptr(); // Input end
                auto size = data.size()*sizeof(char);

                if ( size == 0 )
                {
                    setg(nullptr, nullptr, nullptr);
                    inputInitialized = false;
                    return false; // No characters available
                }
                else if ( char* start = &data[0];
                    gptr == nullptr || gptr == egptr || (start != eback || start+size > egptr) ) // Changes are required
                {
                    ((std::istream*)this)->clear();
                    if ( inputInitialized && eback != nullptr && gptr != nullptr && egptr != nullptr && gptr >= eback && gptr <= egptr )
                    {
                        auto offset = gptr-eback; // Calculate the offset, apply offset to the current data set
                        if ( offset >= (decltype(offset))size && (src != nullptr && src->good()) ) // Input offset was at end of vector or further
                        {
                            *this << src->rdbuf(); // Try reading from source stream
                            start = &data[0];
                            size = data.size()*sizeof(char);
                        }

                        if ( offset == size ) // Input position is at the end of the vector
                        {
                            setg(start, start+size, start+size); // Set gptr same as egptr
                            return false; // No characters available
                        }
                        else if ( offset > (decltype(offset))size ) // Input position is now out of bounds due to a size reduction of the vector
                        {
                            setg(start, nullptr, start+size); // Set gptr to nullptr as is proper for an underflow failures
                            return false; // No characters available
                        }
                        else // Offset somewhere within the vector
                        {
                            char* next = start+offset; // Point to the character at offset
                            setg(start, next, start+size); // Set the new input pointers
                            return true; // Characters available
                        }
                    }
                    else // Input not initialized or pointers are in an invalid state
                    {
                        setg(start, start, start+size); // Set input pointers to vector contents
                        inputInitialized = true;
                        return true; // Characters available
                    }
                }
                else // No changes are required to read the next character
                    return true; // Characters available
            }

            inline void syncOutput()
            {
                auto pbase = std::streambuf::pbase(); // Output start
                auto pptr = std::streambuf::pptr(); // Output curr
                auto epptr = std::streambuf::epptr(); // Output end
                auto size = data.size()*sizeof(char);

                if ( pbase != nullptr && pptr != nullptr && epptr != nullptr && pptr >= pbase && pptr < epptr ) // At some valid, non-end position
                {
                    auto offset = pptr-pbase; // Calculate the offset, apply offset to the current data set
                    if ( offset >= (decltype(offset))size && (src != nullptr && src->good()) ) // Output offset was at end of vector or further
                    {
                        *this << src->rdbuf(); // Try reading from source stream
                        size = data.size()*sizeof(char);
                    }

                    if ( offset >= (decltype(offset))size ) // Output position is at the end of the vector or out of bounds due to vector size reduction
                    {
                        if ( size == 0 )
                            setp(nullptr, nullptr);
                        else
                            setp(&data[0]+size, &data[0]+size);
                    }
                    else // Output position somewhere within the vector
                        setp(&data[0] + offset, &data[0]+size);
                }
                else if ( size == 0 && (pbase != nullptr || pptr != nullptr || epptr != nullptr) ) // At an invalid position or at the end, and pointers not already null
                    setp(nullptr, nullptr);
                else if ( size > 0 && (pbase != &data[0]+size || pptr != &data[0]+size || epptr != &data[0]+size) )
                    setp(&data[0]+size, &data[0]+size);
            }

            /// Setup the read for std::istream
            virtual int underflow()
            {
                if ( syncInput() )
                    return std::streambuf::traits_type::to_int_type(*gptr()); // Return next character
                else
                    return std::streambuf::traits_type::eof(); // Return EOF
            }

            /// Appends the character that came through std::ostream
            virtual int overflow(int c)
            {
                if ( pptr() < epptr() && pptr() != nullptr && epptr() != nullptr )
                {
                    syncOutput();
                    if ( pptr() < epptr() && pptr() != nullptr && epptr() != nullptr )
                    {
                        *pptr() = (char)c;
                        pbump(1);
                    }
                    else
                        data.push_back((char)c);
                }
                else
                    data.push_back((char)c);

                return c;
            }

            virtual int pbackfail(int c = std::streambuf::traits_type::eof())
            {
                syncInput();
                auto eback = std::streambuf::eback(); // Input start
                auto gptr = std::streambuf::gptr(); // Input curr
                auto egptr = std::streambuf::egptr(); // Input end
                if ( inputInitialized && eback != nullptr && gptr != nullptr && egptr != nullptr && gptr > eback && gptr <= egptr )
                {
                    if ( c == std::streambuf::traits_type::eof() )
                    {
                        setg(eback, gptr-1, egptr);
                        return std::streambuf::traits_type::not_eof(c);
                    }
                    else if ( gptr[-1] == c )
                    {
                        setg(eback, gptr-1, egptr);
                        return c;
                    }
                    else if constexpr ( !std::is_base_of<std::istream, StreamType>::value )
                    {
                        setg(eback, gptr-1, egptr);
                        *gptr = c;
                        return c;
                    }
                }
                return std::streambuf::traits_type::eof();
            }

            virtual std::streambuf* setbuf(char* s, std::streamsize n)
            {
                if ( s == nullptr || n == 0 )
                    clear();
                else if ( s != nullptr && n > 0 )
                {
                    data.assign(s, s+n);
                    inputInitialized = false;
                    syncInput();
                }
                return this;
            }

            virtual std::streambuf::pos_type seekoff(std::streambuf::off_type off, std::ios_base::seekdir dir,
                std::ios_base::openmode which = std::ios_base::in | std::ios_base::out)
            {
                bool repositionRead = (which & std::ios_base::in) == std::ios_base::in && std::is_base_of<std::istream, StreamType>::value;
                bool repositionWrite = (which & std::ios_base::out) == std::ios_base::out && std::is_base_of<std::ostream, StreamType>::value;

                if ( repositionRead || repositionWrite )
                {
                    if ( repositionWrite )
                        syncOutput();

                    if ( repositionRead )
                        syncInput();

                    if ( (gptr() != nullptr || !repositionRead) && (pptr() != nullptr || !repositionWrite) )
                    {
                        std::streambuf::off_type newInputOffset = -1;
                        std::streambuf::off_type newOutputOffset = -1;
                        if ( repositionRead )
                        {
                            if ( dir == std::ios_base::beg )
                                newInputOffset = 0;
                            else if ( dir == std::ios_base::cur )
                                newInputOffset = gptr()-eback();
                            else if ( dir == std::ios_base::end )
                                newInputOffset = data.size()*sizeof(char);

                            if ( newInputOffset + off < 0 || &data[0] + newInputOffset + off > &data[0]+data.size()*sizeof(char) )
                                return std::streambuf::pos_type(std::streambuf::off_type(-1));
                        }

                        if ( repositionWrite )
                        {
                            if ( dir == std::ios_base::beg )
                                newOutputOffset = 0;
                            else if ( dir == std::ios_base::cur )
                                newOutputOffset = pptr()-pbase();
                            else if ( dir == std::ios_base::end )
                                newOutputOffset = data.size()*sizeof(char);

                            if ( newOutputOffset + off < 0 || &data[0] + newOutputOffset + off > epptr() )
                                return std::streambuf::pos_type(std::streambuf::off_type(-1));
                        }

                        if ( repositionRead )
                            setg(&data[0], &data[0]+newInputOffset+off, &data[0]+data.size()*sizeof(char));

                        if ( repositionWrite )
                            setp(&data[0]+newOutputOffset+off, &data[0]+data.size()*sizeof(char));

                        return std::streambuf::pos_type(repositionRead ? newInputOffset : newOutputOffset);
                    }
                }
                return std::streambuf::pos_type(std::streambuf::off_type(-1));
            }

            virtual std::streambuf::pos_type seekpos(std::streambuf::pos_type sp,
                std::ios_base::openmode which = std::ios_base::in | std::ios_base::out)
            {
                return seekoff(std::streambuf::off_type(sp), std::ios_base::beg, which);
            }

        private:
            bool inputInitialized;
            std::istream* src;
            std::array<char, 256> num;
            std::vector<char> data;
    };

    template <>
    class BasicStringBuffer<std::istream> : public std::streambuf, public std::istream, private std::ostream
    {
        public:
            BasicStringBuffer() : std::istream((std::streambuf*)this), std::ostream((std::streambuf*)this), src(nullptr), num(), inputInitialized(false) {}
            BasicStringBuffer(std::istream & source) : std::istream((std::streambuf*)this), std::ostream((std::streambuf*)this), src(&source), num(), inputInitialized(false) {}
            BasicStringBuffer(const std::string & str) : std::istream((std::streambuf*)this), std::ostream((std::streambuf*)this),
                data(str.begin(), str.end()), src(nullptr), num(), inputInitialized(false) {}
            BasicStringBuffer(const std::string & str, std::istream & source) : std::istream((std::streambuf*)this), std::ostream((std::streambuf*)this),
                data(str.begin(), str.end()), src(&source), num(), inputInitialized(false) {}

            virtual ~BasicStringBuffer() {}

            inline void operator<<(std::streambuf* sb)
            {
                (std::ostream &)(*this) << sb;
            }
            
            inline void nullTerminate()
            {
                if ( data.empty() || data.back() != '\0' )
                    data.push_back('\0');
            }
            inline void unterminate()
            {
                if ( !data.empty() && data.back() == '\0' )
                    data.pop_back();
            }
        
            size_t size() const noexcept
            {
                return data.size();
            }

            inline void clear()
            {
                data.clear();
                ((std::ostream*)this)->clear();
                ((std::istream*)this)->clear();
            }

            const char & operator[](size_t pos) const
            {
                return data[pos];
            }

            /// Warning: unless addNulTerminator is false, this will add a nul terminator to the end of the buffer if not already present
            /// This must be manually removed (perhaps by calling unterminate) before adding additional character data to avoid having
            /// a NUL terminator in the middle of string
            inline const char* c_str(bool addNulTerminator = true)
            {
                if ( addNulTerminator )
                    nullTerminate();

                return &(*this)[0];
            }

            inline std::string str() const
            {
                return std::string(data.begin(), data.end());
            }

            inline const std::vector<char> & vec() const
            {
                return data;
            }

            inline void swap(std::vector<char> & other)
            {
                data.swap(other);
                sync();
            }

            virtual int sync()
            {
                syncInput();
                syncOutput();
                return 0;
            }

            std::istream* source()
            {
                return src;
            }

        protected:
            /// Returns true if gptr points to a valid next character, false otherwise
            inline bool syncInput()
            {
                auto eback = std::streambuf::eback(); // Input start
                auto gptr = std::streambuf::gptr(); // Input curr
                auto egptr = std::streambuf::egptr(); // Input end
                auto size = data.size()*sizeof(char);

                if ( size == 0 )
                {
                    setg(nullptr, nullptr, nullptr);
                    inputInitialized = false;
                    return false; // No characters available
                }
                else if ( char* start = &data[0];
                    gptr == nullptr || gptr == egptr || (start != eback || start+size > egptr) ) // Changes are required
                {
                    ((std::istream*)this)->clear();
                    if ( inputInitialized && eback != nullptr && gptr != nullptr && egptr != nullptr && gptr >= eback && gptr <= egptr )
                    {
                        auto offset = gptr-eback; // Calculate the offset, apply offset to the current data set
                        if ( offset >= (decltype(offset))size && (src != nullptr && src->good()) ) // Input offset was at end of vector or further
                        {
                            *this << src->rdbuf(); // Try reading from source stream
                            start = &data[0];
                            size = data.size()*sizeof(char);
                        }

                        if ( offset == size ) // Input position is at the end of the vector
                        {
                            setg(start, start+size, start+size); // Set gptr same as egptr
                            return false; // No characters available
                        }
                        else if ( offset > (decltype(offset))size ) // Input position is now out of bounds due to a size reduction of the vector
                        {
                            setg(start, nullptr, start+size); // Set gptr to nullptr as is proper for an underflow failures
                            return false; // No characters available
                        }
                        else // Offset somewhere within the vector
                        {
                            char* next = start+offset; // Point to the character at offset
                            setg(start, next, start+size); // Set the new input pointers
                            return true; // Characters available
                        }
                    }
                    else // Input not initialized or pointers are in an invalid state
                    {
                        setg(start, start, start+size); // Set input pointers to vector contents
                        inputInitialized = true;
                        return true; // Characters available
                    }
                }
                else // No changes are required to read the next character
                    return true; // Characters available
            }

            inline void syncOutput()
            {
                auto pbase = std::streambuf::pbase(); // Output start
                auto pptr = std::streambuf::pptr(); // Output curr
                auto epptr = std::streambuf::epptr(); // Output end
                auto size = data.size()*sizeof(char);

                if ( pbase != nullptr && pptr != nullptr && epptr != nullptr && pptr >= pbase && pptr < epptr ) // At some valid, non-end position
                {
                    auto offset = pptr-pbase; // Calculate the offset, apply offset to the current data set
                    if ( offset >= (decltype(offset))size && (src != nullptr && src->good()) ) // Output offset was at end of vector or further
                    {
                        *this << src->rdbuf(); // Try reading from source stream
                        size = data.size()*sizeof(char);
                    }

                    if ( offset >= (decltype(offset))size ) // Output position is at the end of the vector or out of bounds due to vector size reduction
                    {
                        if ( size == 0 )
                            setp(nullptr, nullptr);
                        else
                            setp(&data[0]+size, &data[0]+size);
                    }
                    else // Output position somewhere within the vector
                        setp(&data[0] + offset, &data[0]+size);
                }
                else if ( size == 0 && (pbase != nullptr || pptr != nullptr || epptr != nullptr) ) // At an invalid position or at the end, and pointers not already null
                    setp(nullptr, nullptr);
                else if ( size > 0 && (pbase != &data[0]+size || pptr != &data[0]+size || epptr != &data[0]+size) )
                    setp(&data[0]+size, &data[0]+size);
            }

            /// Setup the read for std::istream
            virtual int underflow()
            {
                if ( syncInput() )
                    return std::streambuf::traits_type::to_int_type(*gptr()); // Return next character
                else
                    return std::streambuf::traits_type::eof(); // Return EOF
            }

            /// Appends the character that came through std::ostream
            virtual int overflow(int c)
            {
                if ( pptr() < epptr() && pptr() != nullptr && epptr() != nullptr )
                {
                    syncOutput();
                    if ( pptr() < epptr() && pptr() != nullptr && epptr() != nullptr )
                    {
                        *pptr() = (char)c;
                        pbump(1);
                    }
                    else
                        data.push_back((char)c);
                }
                else
                    data.push_back((char)c);

                return c;
            }

            virtual int pbackfail(int c = std::streambuf::traits_type::eof())
            {
                syncInput();
                auto eback = std::streambuf::eback(); // Input start
                auto gptr = std::streambuf::gptr(); // Input curr
                auto egptr = std::streambuf::egptr(); // Input end
                if ( inputInitialized && eback != nullptr && gptr != nullptr && egptr != nullptr && gptr > eback && gptr <= egptr )
                {
                    if ( c == std::streambuf::traits_type::eof() )
                    {
                        setg(eback, gptr-1, egptr);
                        return std::streambuf::traits_type::not_eof(c);
                    }
                    else if ( gptr[-1] == c )
                    {
                        setg(eback, gptr-1, egptr);
                        return c;
                    }
                }
                return std::streambuf::traits_type::eof();
            }

            virtual std::streambuf* setbuf(char* s, std::streamsize n)
            {
                if ( s == nullptr || n == 0 )
                    clear();
                else if ( s != nullptr && n > 0 )
                {
                    data.assign(s, s+n);
                    inputInitialized = false;
                    syncInput();
                }
                return this;
            }

            virtual std::streambuf::pos_type seekoff(std::streambuf::off_type off, std::ios_base::seekdir dir,
                std::ios_base::openmode which = std::ios_base::in | std::ios_base::out)
            {
                bool repositionRead = (which & std::ios_base::in) == std::ios_base::in;
                bool repositionWrite = false;

                if ( repositionRead || repositionWrite )
                {
                    if ( repositionWrite )
                        syncOutput();

                    if ( repositionRead )
                        syncInput();

                    if ( (gptr() != nullptr || !repositionRead) && (pptr() != nullptr || !repositionWrite) )
                    {
                        std::streambuf::off_type newInputOffset = -1;
                        std::streambuf::off_type newOutputOffset = -1;
                        if ( repositionRead )
                        {
                            if ( dir == std::ios_base::beg )
                                newInputOffset = 0;
                            else if ( dir == std::ios_base::cur )
                                newInputOffset = gptr()-eback();
                            else if ( dir == std::ios_base::end )
                                newInputOffset = data.size()*sizeof(char);

                            if ( newInputOffset + off < 0 || &data[0] + newInputOffset + off > &data[0]+data.size()*sizeof(char) )
                                return std::streambuf::pos_type(std::streambuf::off_type(-1));
                        }

                        if ( repositionWrite )
                        {
                            if ( dir == std::ios_base::beg )
                                newOutputOffset = 0;
                            else if ( dir == std::ios_base::cur )
                                newOutputOffset = pptr()-pbase();
                            else if ( dir == std::ios_base::end )
                                newOutputOffset = data.size()*sizeof(char);

                            if ( newOutputOffset + off < 0 || &data[0] + newOutputOffset + off > epptr() )
                                return std::streambuf::pos_type(std::streambuf::off_type(-1));
                        }

                        if ( repositionRead )
                            setg(&data[0], &data[0]+newInputOffset+off, &data[0]+data.size()*sizeof(char));

                        if ( repositionWrite )
                            setp(&data[0]+newOutputOffset+off, &data[0]+data.size()*sizeof(char));

                        return std::streambuf::pos_type(repositionRead ? newInputOffset : newOutputOffset);
                    }
                }
                return std::streambuf::pos_type(std::streambuf::off_type(-1));
            }

            virtual std::streambuf::pos_type seekpos(std::streambuf::pos_type sp,
                std::ios_base::openmode which = std::ios_base::in | std::ios_base::out)
            {
                return seekoff(std::streambuf::off_type(sp), std::ios_base::beg, which);
            }

        private:
            bool inputInitialized;
            std::istream* src;
            std::array<char, 256> num;
            std::vector<char> data;
    };

    using StringBuffer = BasicStringBuffer<std::iostream>;
    using OStringBuffer = BasicStringBuffer<std::ostream>;
    using IStringBuffer = BasicStringBuffer<std::istream>;

    static std::ostream & operator<<(std::ostream & os, BasicStringBuffer<std::ostream> & sb)
    {
        return sb.size() > 0 ? os.write(&sb[0], sb.size()) : os;
    }

    static std::ostream & operator<<(std::ostream & os, BasicStringBuffer<std::iostream> & sb)
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
            BasicStringBufferPtr(std::istream & is) : sb(new StringBuffer(is)), os(nullptr)
            {
                *sb << is.rdbuf();
                ((std::basic_ios<char>*)sb)->clear();
            }
            BasicStringBufferPtr(std::ostream & os) : sb(new StringBuffer()), os(&os) {}
            BasicStringBufferPtr(std::iostream & ios) : sb(new StringBuffer(ios)), os(&ios)
            {
                *sb << ios.rdbuf();
                ((std::basic_ios<char>*)sb)->clear();
            }

            virtual ~BasicStringBufferPtr()
            {
                if ( os != nullptr )
                {
                    *os << *sb;
                    delete sb;
                }
            }

            inline StringBuffer & operator*() const
            {
                return *sb;
            }
            inline StringBuffer* operator->() const
            {
                return sb;
            }

            inline void flush() const
            {
                if ( os != nullptr )
                {
                    *os << *sb;
                    os = nullptr;
                }
            }

        private:
            mutable StringBuffer* sb;
            mutable std::ostream* os;
    };

    template <>
    class BasicStringBufferPtr<std::ostream>
    {
        public:
            BasicStringBufferPtr(OStringBuffer & sb) : sb(&sb), os(nullptr) {}
            BasicStringBufferPtr(StringBuffer & sb) : sb(new OStringBuffer()), os(&sb) {}
            BasicStringBufferPtr(std::ostream & os) : sb(new OStringBuffer()), os(&os) {}
            BasicStringBufferPtr(std::iostream & ios) : sb(new OStringBuffer()), os(&ios) {}

            virtual ~BasicStringBufferPtr()
            {
                if ( os != nullptr )
                {
                    *os << *sb;
                    delete sb;
                }
            }

            inline OStringBuffer & operator*() const
            {
                return *sb;
            }
            inline OStringBuffer* operator->() const
            {
                return sb;
            }

            inline void flush() const
            {
                if ( os != nullptr )
                {
                    *os << *sb;
                    os = nullptr;
                }
            }

        private:
            mutable OStringBuffer* sb;
            mutable std::ostream* os;
    };

    template <>
    class BasicStringBufferPtr<std::istream>
    {
        public:
            BasicStringBufferPtr(IStringBuffer & sb) : sb(&sb) {}
            BasicStringBufferPtr(StringBuffer & sb) : sb(new IStringBuffer((std::istream &)sb))
            {
                *this->sb << sb.rdbuf();
                ((std::basic_ios<char>*)this->sb)->clear();
            }
            BasicStringBufferPtr(std::istream & is) : sb(new IStringBuffer(is))
            {
                *sb << is.rdbuf();
                ((std::basic_ios<char>*)sb)->clear();
            }
            BasicStringBufferPtr(std::iostream & ios) : sb(new IStringBuffer(ios))
            {
                *sb << ios.rdbuf();
                int i = 5;
                ((std::basic_ios<char>*)sb)->clear();
            }

            virtual ~BasicStringBufferPtr() {}

            inline IStringBuffer & operator*() const
            {
                return *sb;
            }
            inline IStringBuffer* operator->() const
            {
                return sb;
            }

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
}

#endif