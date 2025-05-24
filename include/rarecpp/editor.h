// MIT License, Copyright (c) 2025 Justin F https://github.com/TheNitesWhoSay/RareCpp/blob/master/LICENSE
#ifndef EDITOR_H
#define EDITOR_H
#include <algorithm>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <optional>
#include <span>
#include <tuple>
#include <type_traits>
#include <unordered_set>
#include <vector>
#include "reflect.h"

#include <iosfwd>
#include <iostream> // TODO: Temporary includes
#include <iomanip>
#include "json.h"

namespace RareEdit
{
    using RareTs::type_tags;
    template <typename SizeType> struct IndexSizeType { using type = SizeType; };

    template <typename SizeType> inline constexpr IndexSizeType<SizeType> IndexSize;

    template <typename T>
    constexpr auto defaultIndexType()
    {
        if constexpr ( RareTs::Notes<T>::template hasNote<RareEdit::IndexSizeType>() )
            return RareTs::Notes<T>::template getNote<RareEdit::IndexSizeType>();
        else
            return std::type_identity<std::size_t>{};
    }

    template <typename T> struct is_array_member : std::false_type {};
    template <typename T, std::size_t I> struct is_array_member<RareTs::Member<T, I>> :
        std::bool_constant<std::is_array_v<typename RareTs::Member<T, I>::type>> {};
    template <typename T> inline constexpr bool is_array_member_v = is_array_member<T>::value;

    template <typename T>
    inline constexpr std::size_t reflectedMemberCount() {
        if constexpr ( RareTs::is_macro_reflected_v<T> )
            return RareTs::Members<T>::total;
        else
            return 0;
    }

    template <class T, std::size_t Extent, std::size_t ... Extents>
    class flat_mdspan
    {
        T* data;

    public:
        using element_type = T;
        using extents = std::index_sequence<Extent, Extents...>;
        static constexpr std::size_t rank = 1+sizeof...(Extents);
        static constexpr std::size_t size = Extent*(Extents * ... * 1); // Sum of all extents
        constexpr flat_mdspan(T* data) : data(data) {}
    
        constexpr decltype(auto) operator[](std::size_t i) {
            if constexpr ( sizeof...(Extents) == 0 )
                return data[i];
            else
                return flat_mdspan<T, Extents...>(&data[size/Extent*i]);
        }
        constexpr decltype(auto) operator[](std::size_t i) const {
            if constexpr ( sizeof...(Extents) == 0 )
                return data[i];
            else
                return flat_mdspan<T, Extents...>(&data[size/Extent*i]);
        }
        struct It {
            flat_mdspan* obj;
            std::size_t i;
            constexpr void operator++() { ++i; }
            constexpr decltype(auto) operator*() const { return (*obj)[i]; }
            constexpr bool operator!=(It other) { return i != other.i; }
        };
        constexpr auto begin() { return It{this, 0}; }
        constexpr auto end() { return It{this, Extent}; }
        constexpr auto begin() const { return It{this, 0}; }
        constexpr auto end() const { return It{this, Extent}; }

        constexpr T* flatBegin() { return data; }
        constexpr T* flatEnd() { return data+size; }
        constexpr const T* flatBegin() const { return data; }
        constexpr const T* flatEnd() const { return data+size; }

        constexpr void operator=(const flat_mdspan & other) {
            std::copy(other.flatBegin(), other.flatEnd(), flatBegin());
        }
        constexpr void clear() { std::fill(flatBegin(), flatEnd(), T{}); }
        constexpr void fill(const T & value) { std::fill(flatBegin(), flatEnd(), value); }
        constexpr void swap(flat_mdspan & other) {
            for ( std::size_t i=0; i<size; ++i )
                std::swap(data[i], other.data[i]);
        }
    };

    template <class ...> struct is_flat_mdspan : std::false_type {};
    template <class T> struct is_flat_mdspan<const T> : is_flat_mdspan<T> {};
    template <class T, std::size_t ... Is> struct is_flat_mdspan<flat_mdspan<T, Is...>> : std::true_type {};
    template <class T> inline constexpr bool is_flat_mdspan_v = is_flat_mdspan<T>::value;

    template <typename T> constexpr auto array_header(T && array) {
        if constexpr ( std::rank_v<std::remove_reference_t<T>> == 0 ) return &array;
        else if constexpr ( std::rank_v<std::remove_reference_t<T>> == 1 ) return &array[0];
        else if constexpr ( std::rank_v<std::remove_reference_t<T>> == 2 ) return &array[0][0];
        else return array_header(array[0]);
    }

    template <typename T>
    constexpr auto as_1d(T & array) requires (std::is_array_v<std::remove_reference_t<T>>)
    {
        using U = std::remove_reference_t<T>;
        using Element = std::remove_all_extents_t<U>;
        constexpr auto rank = std::rank_v<U>;
        return [&]<std::size_t ... Is>(std::index_sequence<Is...>) {
            static_assert(sizeof(U) == sizeof(Element)*(std::extent_v<U, Is> * ... * 1), "array passed to as_1d was not contiguous!");
            return flat_mdspan<Element, std::extent_v<U, Is>...>(array_header(array));
        }(std::make_index_sequence<rank>());
    }

    inline void writeVecBoolData(std::vector<std::uint8_t> & data, const std::vector<bool> & vecBool) // Does not include the size
    {
        constexpr std::uint8_t zero = 0;
        std::size_t size = vecBool.size();
        std::size_t unpaddedByteCount = size/8;
        std::size_t byteCount = (size+7)/8;

        std::size_t byteStart = data.size();
        data.insert(data.end(), byteCount, zero);
        for ( std::size_t i=0; i<unpaddedByteCount; ++i )
        {
            data[byteStart + i] = static_cast<std::uint8_t>(
                int(vecBool[i*8]) +
                (int(vecBool[i*8+1]) << 1) +
                (int(vecBool[i*8+2]) << 2) +
                (int(vecBool[i*8+3]) << 3) +
                (int(vecBool[i*8+4]) << 4) +
                (int(vecBool[i*8+5]) << 5) +
                (int(vecBool[i*8+6]) << 6) +
                (int(vecBool[i*8+7]) << 7));
        }
        if ( unpaddedByteCount < byteCount )
        {
            std::size_t bitStart = unpaddedByteCount*8; 
            std::size_t bitCount = size%8;
            int lastByte = 0;
            for ( std::size_t i=0; i<bitCount; ++i )
                lastByte += (int(vecBool[bitStart+i]) << i);
        
            data[byteStart+unpaddedByteCount] = static_cast<std::uint8_t>(lastByte);
        }
    }

    inline void readVecBoolData(const std::vector<std::uint8_t> & data, std::size_t & offset, std::size_t bitSize, std::vector<bool> & bitset) // Does not include the size
    {
        bitset = std::vector<bool>(bitSize, false);
        std::size_t unpaddedReadableByteCount = bitSize/8;
        std::size_t readableByteCount = (bitSize+7)/8;
        for ( std::size_t i=0; i<unpaddedReadableByteCount; ++i )
        {
            std::uint8_t byte = data[offset+i];
            bitset[i*8] = (byte & 0x1) > 0;
            bitset[i*8+1] = (byte & 0x2) > 0;
            bitset[i*8+2] = (byte & 0x4) > 0;
            bitset[i*8+3] = (byte & 0x8) > 0;
            bitset[i*8+4] = (byte & 0x10) > 0;
            bitset[i*8+5] = (byte & 0x20) > 0;
            bitset[i*8+6] = (byte & 0x40) > 0;
            bitset[i*8+7] = (byte & 0x80) > 0;
        }
        if ( readableByteCount > unpaddedReadableByteCount )
        {
            std::uint8_t lastByte = data[offset+unpaddedReadableByteCount];
            std::uint8_t mask = 0x1;
            for ( std::size_t bit = unpaddedReadableByteCount*8; bit<bitSize; ++bit )
            {
                bitset[bit] = (lastByte & mask) > 0;
                mask <<= 1;
            }
        }
        offset += (bitSize+7)/8;
    }

    template <std::size_t N, typename Size = std::uint32_t, bool IncludeSize = true>
    void writeBitset(std::vector<std::uint8_t> & data, const std::bitset<N> & bitset)
    {
        constexpr std::uint8_t zero = 0;
        constexpr std::size_t unpaddedByteCount = N/8;
        constexpr std::size_t byteCount = (N+7)/8;
        constexpr Size size = static_cast<Size>(N);
        if constexpr ( IncludeSize )
            data.insert(data.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));

        std::size_t byteStart = data.size();
        data.insert(data.end(), byteCount, zero);
        for ( std::size_t i=0; i<unpaddedByteCount; ++i )
        {
            data[byteStart + i] = static_cast<std::uint8_t>(
                int(bitset[i*8]) +
                (int(bitset[i*8+1]) << 1) +
                (int(bitset[i*8+2]) << 2) +
                (int(bitset[i*8+3]) << 3) +
                (int(bitset[i*8+4]) << 4) +
                (int(bitset[i*8+5]) << 5) +
                (int(bitset[i*8+6]) << 6) +
                (int(bitset[i*8+7]) << 7));
        }
        if constexpr ( unpaddedByteCount < byteCount )
        {
            constexpr std::size_t bitStart = unpaddedByteCount*8; 
            constexpr std::size_t bitCount = N%8;
            int lastByte = 0;
            for ( std::size_t i=0; i<bitCount; ++i )
                lastByte += (int(bitset[bitStart+i]) << i);
        
            data[byteStart+unpaddedByteCount] = static_cast<std::uint8_t>(lastByte);
        }
    }

    template <std::size_t N, typename Size = std::uint32_t>
    void readBitset(const std::vector<std::uint8_t> & data, std::size_t & offset, std::bitset<N> & bitset)
    {
        bitset.reset();
        std::size_t size = static_cast<std::size_t>((Size &)data[offset]);
        offset += sizeof(Size);
        std::size_t readableBitCount = std::min(N, size);
        std::size_t unpaddedReadableByteCount = readableBitCount/8;
        std::size_t readableByteCount = (readableBitCount+7)/8;
        for ( std::size_t i=0; i<unpaddedReadableByteCount; ++i )
        {
            std::uint8_t byte = data[offset+i];
            bitset[i*8] = (byte & 0x1) > 0;
            bitset[i*8+1] = (byte & 0x2) > 0;
            bitset[i*8+2] = (byte & 0x4) > 0;
            bitset[i*8+3] = (byte & 0x8) > 0;
            bitset[i*8+4] = (byte & 0x10) > 0;
            bitset[i*8+5] = (byte & 0x20) > 0;
            bitset[i*8+6] = (byte & 0x40) > 0;
            bitset[i*8+7] = (byte & 0x80) > 0;
        }
        if ( readableByteCount > unpaddedReadableByteCount )
        {
            std::uint8_t lastByte = data[offset+unpaddedReadableByteCount];
            std::uint8_t mask = 0x1;
            for ( std::size_t bit = unpaddedReadableByteCount*8; bit<readableBitCount; ++bit )
            {
                bitset[bit] = (lastByte & mask) > 0;
                mask <<= 1;
            }
        }
        offset += (size+7)/8;
    }

    template <typename T>
    void writeSelectionVector(std::vector<std::uint8_t> & data, const std::vector<T> & integralVector)
    {
        T size = (T)integralVector.size();
        data.insert(data.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));
        for ( auto & ref : integralVector )
        {
            T val = (T)ref;
            data.insert(data.end(), reinterpret_cast<const std::uint8_t*>(&val), reinterpret_cast<const std::uint8_t*>(&val)+sizeof(val));
        }
    }

    template <typename T>
    bool readSelectionVector(const std::vector<std::uint8_t> & data, std::size_t & offset, std::vector<T> & integralVector)
    {
        T size = (T &)data[offset];
        offset += sizeof(size);
        integralVector.clear();
        integralVector.reserve(size);
        for ( T i=0; i<size; ++i )
        {
            integralVector.insert(integralVector.end(), T((T &)data[offset]));
            offset += sizeof(T);
        }
        return size > 0;
    }

    template <typename T>
    void writeSelections(std::vector<std::uint8_t> & data, const T & t)
    {
        if constexpr ( !RareTs::is_static_array_v<std::remove_cvref_t<T>> && RareTs::is_specialization_v<T, std::vector> )
            writeSelectionVector(data, t);
    }

    template <typename T>
    bool readSelections(const std::vector<std::uint8_t> & data, std::size_t & offset, T & t)
    {
        if constexpr ( requires{readSelectionVector(data, offset, t);} )
            return readSelectionVector(data, offset, t);
        else
            return false;
    }

    namespace u8bool
    {
        inline constexpr std::uint8_t False = 0;
        inline constexpr std::uint8_t True = 1;

        inline bool read(const std::vector<std::uint8_t> & data, std::size_t & offset)
        {
            return data[offset++] != 0;
        }

        inline void write(std::vector<std::uint8_t> & data, bool value)
        {
            std::uint8_t write = value ? 1 : 0;
            data.insert(data.end(), reinterpret_cast<const std::uint8_t*>(&write), reinterpret_cast<const std::uint8_t*>(&write)+sizeof(write));
        }

        inline void writeTrue(std::vector<std::uint8_t> & data)
        {
            data.insert(data.end(), &True, &True+sizeof(True));
        }

        inline void writeFalse(std::vector<std::uint8_t> & data)
        {
            data.insert(data.end(), &False, &False+sizeof(False));
        }
    };

    struct uint6_t // A 6-bit unsigned int [0, 64); as the high bits are available can be combined with path bits
    {
        std::uint8_t value = 0;
        
        uint6_t() = default;
        template <typename T> constexpr uint6_t(T && t) : value(static_cast<std::uint8_t>(t)) {}
        
        constexpr operator std::ptrdiff_t() const { return static_cast<std::ptrdiff_t>(value); }
        constexpr operator std::size_t() const { return static_cast<std::size_t>(value); }
        constexpr operator std::uint8_t() const { return static_cast<std::uint8_t>(value); }
        constexpr auto operator==(std::size_t other) { return static_cast<std::size_t>(value) == other; }
        constexpr auto operator<=>(const uint6_t & other) const { return value <=> other.value; }
        template <typename T> constexpr auto operator<=>(const T & other) const requires(!std::is_same_v<uint6_t, T>) { return static_cast<T>(value) <=> other; }
        constexpr auto operator &() { return &value; }
        constexpr auto & operator++() { ++value; return *this; }
        constexpr auto & operator--() { --value; return *this; }
        constexpr auto & operator+=(std::uint8_t val) { value += val; return *this; }
        constexpr auto & operator-=(std::uint8_t val) { value -= val; return *this; }

        inline friend auto & operator<<(std::ostream & os, const uint6_t & num) { os << int(num.value); return os; }
    };
    static_assert(sizeof(uint6_t) == sizeof(uint8_t), "Unexpected uint6_t size");

    inline void readBoolByte(const std::vector<std::uint8_t> & data, std::size_t & offset, bool & value)
    {
        std::uint8_t val = (std::uint8_t &)data[offset];
        offset += sizeof(val);
        value = val == 0 ? false : true;
    }

    template <typename T>
    void clearSel(T & t)
    {
        RareTs::clear(t);
    }

    template <bool Desc = false, typename Primary, typename Secondary>
    constexpr void parallelQuickSort(Primary & primary, Secondary & secondary, std::ptrdiff_t l, std::ptrdiff_t r) noexcept {
        auto parallelSwap = [&](auto i1, auto i2) {
            std::swap(primary[static_cast<std::size_t>(i1)], primary[static_cast<std::size_t>(i2)]);
            std::swap(secondary[static_cast<std::size_t>(i1)], secondary[static_cast<std::size_t>(i2)]);
        };
        if ( l < r ) {
            std::ptrdiff_t i = l+(r-l)/2; // Take median index as the pivot, swap pivot with last element
            parallelSwap(i, r);
            auto pivotValue = primary[static_cast<std::size_t>(r)];
            i = l;
            for ( std::ptrdiff_t j=l; j<r; ++j ) {
                if constexpr ( Desc )
                {
                    if ( pivotValue < primary[static_cast<std::size_t>(j)] ) { // Swap elements greater than pivot to the earliest indexes
                        parallelSwap(i, j);
                        ++i;
                    }
                }
                else
                {
                    if ( primary[static_cast<std::size_t>(j)] < pivotValue ) { // Swap elements less than pivot to the earliest indexes
                        parallelSwap(i, j);
                        ++i;
                    }
                }
            }
            parallelSwap(i, r);
            parallelQuickSort<Desc>(primary, secondary, l, i-1); // Sort left sub-array (elements < pivotValue)
            parallelQuickSort<Desc>(primary, secondary, i+1, r); // Sort right sub-array (elements > pivotValue)
        }
    }

    template <bool Desc = false, typename I = std::size_t, typename T>
    [[nodiscard]] std::vector<I> trackedSort(T & items) // sort items, return vector of source indexes
    {
        if ( std::size(items) == 0 )
            return std::vector<I> {};
        else
        {
            std::vector<I> indexes(std::size(items), I{0});
            std::iota(indexes.begin(), indexes.end(), static_cast<I>(0));
            parallelQuickSort<Desc>(items, indexes, 0, std::ptrdiff_t(std::size(items))-1);
            return indexes;
        }
    }

    template <typename I = std::size_t, typename T>
    requires ( requires{std::declval<T>()[0] = std::declval<T>()[0];} )
    void undoSort(T & items, const std::span<I> & sourceIndexes)
    {
        std::size_t total = std::size(items);
        if ( total > 0 )
        {
            if ( total != sourceIndexes.size() )
            {
                throw std::invalid_argument("items.size(" + std::to_string(total) +
                    "!= sourceIndexes.size(" + std::to_string(sourceIndexes.size()) + ")");
            }
            std::remove_cvref_t<T> oldItems(total);
            for ( std::size_t i=0; i<total; ++i )
                oldItems[sourceIndexes[i]] = items[i];

            std::swap(items, oldItems);
        }
    }

    template <typename I = std::size_t, typename T>
    requires ( requires{std::declval<T>()[0] = std::declval<T>()[0];} )
    void redoSort(T & items, const std::span<I> & sourceIndexes)
    {
        std::size_t total = std::size(items);
        if ( total > 0 )
        {
            if ( total != sourceIndexes.size() )
            {
                throw std::invalid_argument("items.size(" + std::to_string(total) +
                    "!= sourceIndexes.size(" + std::to_string(sourceIndexes.size()) + ")");
            }
            std::remove_cvref_t<T> newItems(total);
            for ( std::size_t i=0; i<total; ++i )
                newItems[i] = items[sourceIndexes[i]];

            std::swap(items, newItems);
        }
    }

    template <class Keys, class Pathway, class EditorType>
    struct PathTaggedKeys : Keys // (aka: route) this combines the path to a particular (sub/)member and any map keys/array indexes
    {
        using pathway = Pathway;
        using keys = Keys;
        using editor_type = EditorType;

        template <std::size_t I> constexpr const auto & index() { return std::get<I>((Keys &)(*this));}
    };

    template <class Input>
    using MakePath = PathTaggedKeys<typename Input::keys, typename Input::path, typename Input::editor_type>;

    #define PATH(...) RareEdit::MakePath<decltype(__VA_ARGS__)>

    struct Rotation
    {
        std::size_t startIndex;
        std::size_t middleIndex;
        std::size_t endIndex;

        static Rotation makeReverse(std::size_t startIndex, std::size_t middleIndex, std::size_t endIndex)
        {
            if ( startIndex+endIndex-middleIndex > endIndex )
                throw std::exception();
            return Rotation{.startIndex = startIndex, .middleIndex = startIndex+endIndex-middleIndex, .endIndex = endIndex};
        }

        void perform(auto && container)
        {
            auto begin = std::begin(container);
            std::rotate(
                std::next(begin, static_cast<std::ptrdiff_t>(startIndex)),
                std::next(begin, static_cast<std::ptrdiff_t>(middleIndex)),
                std::next(begin, static_cast<std::ptrdiff_t>(endIndex))
            );
        }
    };

    template <class Selection>
    void mirrorSwapToSelection(Selection & selection, std::size_t leftIndex, std::size_t rightIndex)
    {
        if constexpr ( !std::is_null_pointer_v<Selection> )
        {
            using index_type = RareTs::element_type_t<std::remove_cvref_t<Selection>>;
            for ( auto & i : selection )
            {
                if ( static_cast<std::size_t>(i) == leftIndex )
                    i = static_cast<index_type>(rightIndex);
                else if ( static_cast<std::size_t>(i) == rightIndex )
                    i = static_cast<index_type>(leftIndex);
            }
        }
    }

    template <class Selection>
    void mirrorRotationToSelection(Selection & selection, std::size_t first, std::size_t middle, std::size_t last)
    {
        if constexpr ( !std::is_null_pointer_v<Selection> )
        {
            if ( !selection.empty() && first < middle && middle < last )
            {
                using index_type = RareTs::element_type_t<std::remove_cvref_t<Selection>>;
                // [first, middle) are moved forward last-middle indexes, [middle, last) are move back middle-first indexes
                auto forward = static_cast<index_type>(last-middle);
                auto backward = static_cast<index_type>(middle-first);
                for ( auto & i : selection )
                {
                    if ( i >= first && i < middle )
                        i += forward;
                    else if ( i >= middle && i < last )
                        i -= backward;
                }
            }
        }
    }

    template <class Indexes>
    void mirrorRotationToIndexes(Indexes & indexes, auto first, auto middle, auto last)
    {
        std::rotate(
            std::next(indexes.begin(), static_cast<std::ptrdiff_t>(first)),
            std::next(indexes.begin(), static_cast<std::ptrdiff_t>(middle)),
            std::next(indexes.begin(), static_cast<std::ptrdiff_t>(last)));
    }

    enum class PathOp : std::uint8_t {
        
        // The meaning of the lower 6-bits depends on the selection bit (second highest bit)...
        //   Branch: identifies a field or array index (for array sizes <= 64) which you'll be branching from
        //           if array/container requires an index larger than 6 bits, then the next sizeof(containerIndex) bytes are the container index
        //   SelBranch: lower 6-bits unused, SelBranch indicates the operation applies over the selection for this container (may branch further down from here)
        //   LeafBranch: same as Branch except this also indicates it's the last branch in the sequence
        //   LeafSelBranch: same as SelBranch except this also indicates it's the last branch in the sequence
        //                  special case: if this is the first element, it means use the root element
        HighBits      = 0b11000000,
        LowBits       = 0b00111111,

        SelMask       = 0b01000000,
        LeafMask      = 0b10000000,

        Branch        = 0b00000000,
        SelBranch     = 0b01000000,
        LeafBranch    = 0b10000000,
        LeafSelBranch = 0b11000000,
        RootPath      = LeafSelBranch
    };

    enum class Op : std::uint8_t {
        // The first {} is the data required to perform the operation, the --{} is additional data required for fast undos
        Reset, // {} ("value = {};") --{n, value0, ..., valueN, m, selections}
        Reserve, // {newSize}
        Trim,
        Assign, // {newSize, value} --{n, value0, ..., valueN, m, selections}
        AssignDefault, // {newSize} --{n, value0, ..., valueN, m, selections}
        ClearSelections, // --{n, index0, ..., indexN}
        SelectAll, // {} --{n, index0, ..., indexN}
        Select, // {index}
        SelectN, // {n, index0, ..., indexN}
        Deselect, // {index} --{selIndex}
        DeselectN,  // {n, index0, ..., indexN} --{selIndex0, ..., selIndexN}
        ToggleSelection, // {index} --{u8boolSelected, selIndex}
        ToggleSelectionN, // {n, index0, ..., indexN} --{bitsetIndexWasSelected, selIndex0, ..., selIndexN}
        SortSelections, // {n, index0SourceIndex, ..., indexNSourceIndex}
        SortSelectionsDesc, // {n, index0SourceIndex, ..., indexNSourceIndex}
        Set, // {value} --{prevValue}
        SetN, // {n, index0, ..., indexN, value} --{value0, ..., valueN}
        SetL, // {value} --{value0, ..., valueN} // Same as setN, except the selections make up the indexes
        Append, // {value}
        AppendN, // {n, value0, ..., valueN}
        Insert, // {index, value}
        InsertN, // {n, index, value0, ..., valueN}
        Remove, // {index} --{value, u8boolWasSelected, selIndex}
        RemoveN, // {n, index0, ..., indexN} --{value0, ..., valueN, bitsetIndexSelected, m, selIndex0, ..., selIndexM} where index0 > ... > indexN
        RemoveL, // {} --{n, index0, ..., indexN, value0, ..., valueN, selections} // Same as removeN, except the selections make up the indexes
        Sort, // {n, index0SourceIndex, ..., indexNSourceIndex}
        SortDesc, // {n, index0SourceIndex, ..., indexNSourceIndex}
        Swap, // {index0, index1}
        MoveUp, // {index}
        MoveUpN, // {n, index0, ..., indexN}
        MoveUpL, // {} --{selections} // Same as moveUpN, except the selections make up the indexes
        MoveTop, // {index}
        MoveTopN, // {n, index0, ... indexN} --{selections}
        MoveTopL, // {} --{selections} // Same as moveTopN, except the selections make up the indexes
        MoveDown, // {index}
        MoveDownN, // {n, index0, ... indexN}
        MoveDownL, // {} --{selections} // Same as moveDownN, except the selections make up the indexes
        MoveBottom, // {index}
        MoveBottomN, // {n, index0, ... indexN} --{selections}
        MoveBottomL, // {} --{selections} // Same as moveDownN, except the selections make up the indexes
        MoveTo, // {index, targetIndex}
        MoveToN, // {n, targetIndex, index0, ..., indexN} --{selections}
        MoveToL // {targetIndex} --{selections} // Same as moveToN, except the selections make up the indexes
    };

    // Go to the Ith member of the current object
    template <std::size_t I> struct PathMember {
        static constexpr std::size_t index = I;
    };
    template <class T> struct is_path_member { static constexpr bool value = false; };
    template <std::size_t I> struct is_path_member<PathMember<I>> { static constexpr bool value = true; };
    template <class T> inline constexpr bool is_path_member_v = is_path_member<T>::value;

    // Use the Ith tuple-index to perform an array access on the current object
    template <std::size_t I> struct PathIndex {
        static constexpr std::size_t index = I;
    };
    template <class T> struct is_path_index { static constexpr bool value = false; };
    template <std::size_t I> struct is_path_index<PathIndex<I>> { static constexpr bool value = true; };
    template <class T> inline constexpr bool is_path_index_v = is_path_index<T>::value;

    struct PathSelections {}; // The subsequent operation applies to all selected objects in the collection
    template <class T> struct is_path_selections { static constexpr bool value = false; };
    template <> struct is_path_selections<PathSelections> { static constexpr bool value = true; };
    template <class T> inline constexpr bool is_path_selections_v = is_path_selections<T>::value;

    template <class ... Ts> concept has_path_selections = (std::same_as<PathSelections, Ts> || ...);

    template <class Agent, class default_index_type, class RootData, class T, class Keys, class ... Pathway>
    class EditPrimitive : public Keys
    {
        Agent & agent;

    public:
        using keys = Keys;
        using path = type_tags<Pathway...>;
        using editor_type = typename Agent::editor_type;
        using type = T;

        EditPrimitive(Agent & agent, Keys keys) :
            Keys {std::move(keys)}, agent(agent) {}
        template <class U> void operator=(U && value) {
            if constexpr ( has_path_selections<Pathway...> )
                agent.template setL<Pathway...>(std::forward<U>(value), (Keys &)(*this));
            else
                agent.template set<Pathway...>(std::forward<U>(value), (Keys &)(*this));
        }
    };

    template <class Agent, class default_index_type, class RootData, class T, class Keys, std::size_t I, class ... Pathway>
    static constexpr auto editMember();

    template <class Agent, class default_index_type, class RootData, class T, class Keys, class ... Pathway> struct edit_member
    {
        template <std::size_t I> using type = typename decltype(editMember<Agent, default_index_type, RootData, T, Keys, I, Pathway...>())::type;
    };

    template <class ... Pathway>
    struct EditMembers
    {
        template <class Agent, class default_index_type, class RootData, class T, class Keys, std::size_t ... Is>
        struct Membs : Keys, RareTs::Class::adapt_member<edit_member<Agent, default_index_type, RootData, T, Keys, Pathway...>::template type, T, Is>...
        {
            using keys = Keys;
            using path = type_tags<Pathway...>;
            using editor_type = typename Agent::editor_type;

            Membs(Agent & agent, Keys keys) :
                Keys {keys},
                RareTs::Class::template adapt_member<edit_member<Agent, default_index_type, RootData, T, Keys, Pathway...>::template type, T, Is> {{ agent, keys }}...,
                agent(agent) {}

            template <std::size_t I> constexpr auto & fromMember()
            {
                auto & [editMember] = static_cast<RareTs::Class::adapt_member<edit_member<Agent, default_index_type, RootData, T, Keys, Pathway...>::template type, T, I> &>(*this);
                return editMember;
            }

            template <class U> constexpr void operator=(U && value) {
                if constexpr ( has_path_selections<Pathway...> )
                    agent.template setL<Pathway...>(std::forward<U>(value), (Keys &)(*this));
                else
                    agent.template set<Pathway...>(std::forward<U>(value), (Keys &)(*this));
            }

        private:
            Agent & agent;
        };
    };

    template <class Agent, class default_index_type, class RootData, class T, class Keys, class ... Pathway, std::size_t... Is>
    EditMembers<Pathway...>::template Membs<Agent, default_index_type, RootData, T, Keys, Is...> editMembers(std::index_sequence<Is...>);

    template <class Agent, class default_index_type, class RootData, class T, class Keys = std::tuple<>, class ... Pathway>
    using edit_members = decltype(editMembers<Agent, default_index_type, RootData, T, Keys, Pathway...>(std::make_index_sequence<RareTs::Members<T>::total>()));

    template <class DefaultIndexType, class Member>
    inline constexpr auto index_typer()
    {
        if constexpr ( !std::is_void_v<Member> )
        {
            if constexpr ( RareTs::is_static_array_v<typename Member::type> )
            {
                constexpr std::size_t size = RareTs::static_array_size_v<typename Member::type>;
                if constexpr ( size < 64 ) // 6-bit int
                    return std::type_identity<uint6_t>{};
                else if constexpr ( size <= 0xFF )
                    return std::type_identity<std::uint8_t>{};
                else if constexpr ( size <= 0xFFFF && sizeof(std::size_t) > sizeof(std::uint32_t) )
                    return std::type_identity<std::uint16_t>{};
                else if constexpr ( size <= 0xFFFFFFFF && sizeof(std::size_t) > sizeof(std::uint32_t) )
                    return std::type_identity<std::uint32_t>{};
                else
                    return std::type_identity<std::size_t>{};
            }
            else if constexpr ( Member::template hasNote<RareEdit::IndexSizeType>() )
                return Member::template getNote<RareEdit::IndexSizeType>();
            else
                return std::type_identity<DefaultIndexType>{};
        }
        else
            return std::type_identity<DefaultIndexType>{};
    };

    template <class DefaultIndexType, class Member>
    using index_type_t = typename decltype(RareEdit::index_typer<DefaultIndexType, Member>())::type;

    template <class Agent, class default_index_type, class RootData, class T, class Keys, template <class...> class SubElement, class ... Pathway>
    class Editable
    {
        template <class U, class LastMember, class PathElement, class ... PathElements>
        static constexpr auto getMemberImpl()
        {
            if constexpr ( is_path_selections_v<PathElement> )
            {
                if constexpr ( sizeof...(PathElements) == 0 )
                    return std::type_identity<LastMember> {};
                else
                    return getMemberImpl<std::remove_cvref_t<decltype(std::declval<U>()[0])>, LastMember, PathElements...>();
            }
            else if constexpr ( is_path_member_v<PathElement> )
            {
                if constexpr ( sizeof...(PathElements) == 0 )
                    return std::type_identity<RareTs::Member<U, PathElement::index>> {};
                else
                    return getMemberImpl<typename RareTs::Member<U, PathElement::index>::type, RareTs::Member<U, PathElement::index>, PathElements...>();
            }
            else if constexpr ( is_path_index_v<PathElement> )
            {
                if constexpr ( sizeof...(PathElements) == 0 )
                    return std::type_identity<LastMember> {};
                else
                    return getMemberImpl<std::remove_cvref_t<decltype(std::declval<U>()[0])>, LastMember, PathElements...>();
            }
        }

        class RandomAccess : public Keys
        {
            using index_type = index_type_t<default_index_type, typename std::remove_cvref_t<decltype(getMemberImpl<RootData, void, Pathway...>())>::type>;

            template <std::size_t... Is> static constexpr auto arrayOpType(std::index_sequence<Is...>) -> SubElement<
                Agent, default_index_type, RootData, T, std::tuple<std::tuple_element_t<Is, Keys>..., index_type>, Pathway..., PathIndex<sizeof...(Is)>>;
            template <std::size_t... Is> static constexpr auto subArrayOpType(std::index_sequence<Is...>) -> Editable<
                Agent, default_index_type, RootData, RareTs::element_type_t<T>, std::tuple<std::tuple_element_t<Is, Keys>..., index_type>, SubElement, Pathway..., PathIndex<sizeof...(Is)>>::Array;

        protected:
            Agent & agent;

            constexpr auto & memberRef() { return agent.template getMemberReference<Pathway...>(agent.t, (Keys &)(*this)); }

        public:
            using keys = Keys;
            using path = type_tags<Pathway...>;
            using editor_type = typename Agent::editor_type;
            using Route = PathTaggedKeys<Keys, path, typename Agent::editor_type>;

            using selection_op_type = SubElement<Agent, default_index_type, RootData, T, Keys, Pathway..., PathSelections>;
            using array_op_type = decltype(arrayOpType(std::make_index_sequence<std::tuple_size_v<Keys>>()));
            using sub_array_op_type = decltype(subArrayOpType(std::make_index_sequence<std::tuple_size_v<Keys>>()));

            constexpr const auto & sel() const { return agent.template getSelections<Pathway...>(); }

            inline void clearSelections()
            {
                auto & sel = agent.template getSelections<Pathway...>();
                agent.eventOffsets.push_back(agent.events.size());
                agent.events.push_back(uint8_t(Op::ClearSelections));
                agent.template serializePathway<Pathway...>((Keys &)(*this));

                writeSelectionVector(agent.events, sel);
                RareTs::clear(sel);

                if constexpr ( Agent::template hasSelectionsChangedOp<Route> )
                    agent.user.selectionsChanged(Route{(Keys &)(*this)});
            }

            inline void selectAll()
            {
                auto & sel = agent.template getSelections<Pathway...>();
                agent.eventOffsets.push_back(agent.events.size());
                agent.events.push_back(uint8_t(Op::SelectAll));
                agent.template serializePathway<Pathway...>((Keys &)(*this));

                writeSelectionVector(agent.events, sel);
                RareTs::clear(sel);
                sel.assign(memberRef().size(), 0);
                std::iota(sel.begin(), sel.end(), (RareTs::element_type_t<RareTs::remove_cvref_t<decltype(sel)>>)0);

                if constexpr ( Agent::template hasSelectionsChangedOp<Route> )
                    agent.user.selectionsChanged(Route{(Keys &)(*this)});
            }

            inline void select(index_type i) // i must not be selected
            {
                auto & sel = agent.template getSelections<Pathway...>();
                agent.eventOffsets.push_back(agent.events.size());
                agent.events.push_back(uint8_t(Op::Select));
                agent.template serializePathway<Pathway...>((Keys &)(*this));

                agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&i), reinterpret_cast<const std::uint8_t*>(&i)+sizeof(i));
                if ( std::find(sel.begin(), sel.end(), i) == sel.end() )
                    RareTs::append(sel, i);
                else
                    throw std::invalid_argument("Cannot select an index that is already selected");

                if constexpr ( Agent::template hasSelectionsChangedOp<Route> )
                    agent.user.selectionsChanged(Route{(Keys &)(*this)});
            }

            inline void select(const std::vector<index_type> & addedSelections) // addedSelections must not be selected
            {
                auto & sel = agent.template getSelections<Pathway...>();
                agent.eventOffsets.push_back(agent.events.size());
                agent.events.push_back(uint8_t(Op::SelectN));
                agent.template serializePathway<Pathway...>((Keys &)(*this));

                index_type size = static_cast<index_type>(addedSelections.size());
                agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));
                agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&(addedSelections[0])),
                    reinterpret_cast<const std::uint8_t*>(&(addedSelections[0]))+sizeof(addedSelections[0])*static_cast<std::size_t>(size));

                for ( auto i : addedSelections )
                {
                    if ( std::find(sel.begin(), sel.end(), i) == sel.end() )
                        RareTs::append(sel, i);
                    else
                        throw std::invalid_argument("Cannot select an index that is already selected");
                }
                if constexpr ( Agent::template hasSelectionsChangedOp<Route> )
                    agent.user.selectionsChanged(Route{(Keys &)(*this)});
            }

            inline void deselect(index_type i) // i must be selected
            {
                auto & sel = agent.template getSelections<Pathway...>();
                agent.eventOffsets.push_back(agent.events.size());
                agent.events.push_back(uint8_t(Op::Deselect));
                agent.template serializePathway<Pathway...>((Keys &)(*this));
                
                auto found = std::find(sel.begin(), sel.end(), i);
                index_type foundAt = static_cast<index_type>(std::distance(std::begin(sel), found));
                agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&i), reinterpret_cast<const std::uint8_t*>(&i)+sizeof(i));
                agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&foundAt), reinterpret_cast<const std::uint8_t*>(&foundAt)+sizeof(foundAt));

                if ( found == sel.end() )
                    throw std::invalid_argument("Cannot deselect an index that is not selected");
                else
                    sel.erase(found);

                if constexpr ( Agent::template hasSelectionsChangedOp<Route> )
                    agent.user.selectionsChanged(Route{(Keys &)(*this)});
            }

            inline void deselect(const std::vector<index_type> & removedSelections) // removeSelections must be selected
            {
                auto & sel = agent.template getSelections<Pathway...>();
                agent.eventOffsets.push_back(agent.events.size());
                agent.events.push_back(uint8_t(Op::DeselectN));
                agent.template serializePathway<Pathway...>((Keys &)(*this));

                auto numRemoved = removedSelections.size();
                index_type size = static_cast<index_type>(numRemoved);
                agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));
                agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&removedSelections[0]),
                    reinterpret_cast<const std::uint8_t*>(&removedSelections[0])+sizeof(removedSelections[0])*size);

                for ( auto i : removedSelections )
                {
                    auto found = std::find(sel.begin(), sel.end(), i);
                    if ( found != sel.end() )
                    {
                        index_type foundAt = static_cast<index_type>(std::distance(std::begin(sel), found));
                        agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&foundAt), reinterpret_cast<const std::uint8_t*>(&foundAt)+sizeof(foundAt));
                        sel.erase(found);
                    }
                    else
                        throw std::invalid_argument("Cannot deselect an index that is not selected");
                }
                if constexpr ( Agent::template hasSelectionsChangedOp<Route> )
                    agent.user.selectionsChanged(Route{(Keys &)(*this)});
            }

            inline void toggleSelected(index_type i)
            {
                auto & sel = agent.template getSelections<Pathway...>();
                agent.eventOffsets.push_back(agent.events.size());
                agent.events.push_back(uint8_t(Op::ToggleSelection));
                agent.template serializePathway<Pathway...>((Keys &)(*this));
                agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&i), reinterpret_cast<const std::uint8_t*>(&i)+sizeof(i));
                auto found = std::find(sel.begin(), sel.end(), i);
                u8bool::write(agent.events, found != sel.end());
                if ( found != sel.end() )
                {
                    index_type foundAt = static_cast<index_type>(std::distance(std::begin(sel), found));
                    agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&foundAt), reinterpret_cast<const std::uint8_t*>(&foundAt)+sizeof(foundAt));
                    std::erase(sel, i);
                }
                else
                    RareTs::append(sel, i);

                if constexpr ( Agent::template hasSelectionsChangedOp<Route> )
                    agent.user.selectionsChanged(Route{(Keys &)(*this)});
            }

            inline void toggleSelected(const std::vector<index_type> & toggledSelections)
            {
                auto & sel = agent.template getSelections<Pathway...>();
                agent.eventOffsets.push_back(agent.events.size());
                agent.events.push_back(uint8_t(Op::ToggleSelectionN));
                agent.template serializePathway<Pathway...>((Keys &)(*this));

                auto numToggled = toggledSelections.size();
                index_type size = static_cast<index_type>(numToggled);
                agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));
                agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&toggledSelections[0]),
                    reinterpret_cast<const std::uint8_t*>(&toggledSelections[0])+sizeof(toggledSelections[0])*static_cast<std::size_t>(size));

                std::vector<bool> wasSelected(numToggled, false);
                std::vector<index_type> prevSelIndexes {};
                prevSelIndexes.reserve(numToggled);
                for ( std::size_t i=0; i<toggledSelections.size(); ++i )
                {
                    auto found = std::find(sel.begin(), sel.end(), toggledSelections[i]);
                    if ( found != sel.end() )
                    {
                        wasSelected[i] = true;
                        prevSelIndexes.push_back(static_cast<index_type>(std::distance(sel.begin(), found)));
                        sel.erase(found);
                    }
                    else
                        RareTs::append(sel, toggledSelections[i]);
                }
                writeVecBoolData(agent.events, wasSelected);
                if ( !prevSelIndexes.empty() )
                {
                    agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&prevSelIndexes[0]),
                        reinterpret_cast<const std::uint8_t*>(&prevSelIndexes[0])+sizeof(prevSelIndexes[0])*std::size(prevSelIndexes));
                }

                if constexpr ( Agent::template hasSelectionsChangedOp<Route> )
                    agent.user.selectionsChanged(Route{(Keys &)(*this)});
            }

            inline void sortSelection()
            {
                auto & sel = agent.template getSelections<Pathway...>();
                agent.eventOffsets.push_back(agent.events.size());
                agent.events.push_back(uint8_t(Op::SortSelections));
                agent.template serializePathway<Pathway...>((Keys &)(*this));

                auto sourceIndexes = trackedSort<false, index_type>(sel);
                index_type serializedSize = static_cast<index_type>(sourceIndexes.size());
                agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&serializedSize),
                    reinterpret_cast<const std::uint8_t*>(&serializedSize)+sizeof(serializedSize));

                for ( auto index : sourceIndexes )
                {
                    index_type sourceIndex = static_cast<index_type>(index);
                    agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&sourceIndex),
                        reinterpret_cast<const std::uint8_t*>(&sourceIndex)+sizeof(sourceIndex));
                }
                
                if constexpr ( Agent::template hasSelectionsChangedOp<Route> )
                    agent.user.selectionsChanged(Route{(Keys &)(*this)});
            }

            inline void sortSelectionDescending()
            {
                auto & sel = agent.template getSelections<Pathway...>();
                agent.eventOffsets.push_back(agent.events.size());
                agent.events.push_back(uint8_t(Op::SortSelectionsDesc));
                agent.template serializePathway<Pathway...>((Keys &)(*this));

                auto sourceIndexes = trackedSort<true, index_type>(sel);
                index_type serializedSize = static_cast<index_type>(sourceIndexes.size());
                agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&serializedSize),
                    reinterpret_cast<const std::uint8_t*>(&serializedSize)+sizeof(serializedSize));

                for ( auto index : sourceIndexes )
                {
                    index_type sourceIndex = static_cast<index_type>(index);
                    agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&sourceIndex),
                        reinterpret_cast<const std::uint8_t*>(&sourceIndex)+sizeof(sourceIndex));
                }
                if constexpr ( Agent::template hasSelectionsChangedOp<Route> )
                    agent.user.selectionsChanged(Route{(Keys &)(*this)});
            }
            
            constexpr auto selection() { return selection_op_type(agent, (std::add_lvalue_reference_t<Keys>)(*this)); }

            RandomAccess(Agent & agent, Keys && keys) : Keys {std::move(keys)}, agent(agent) {}

            auto operator[](std::size_t i) const {
                if constexpr ( RareTs::is_static_array_v<T> ) // Multi-dimensional array
                    return sub_array_op_type(agent, std::tuple_cat((std::add_lvalue_reference_t<Keys>)(*this), std::tuple{static_cast<index_type>(i)}));
                else
                    return array_op_type(agent, std::tuple_cat((std::add_lvalue_reference_t<Keys>)(*this), std::tuple{static_cast<index_type>(i)}));
            }
        };

    public:
        struct Array : RandomAccess
        {
            Array(Agent & agent, Keys keys) : RandomAccess { agent, std::move(keys) } {}
            void reset() { RandomAccess::agent.template reset<Pathway...>((Keys &)(*this)); }
        };

        struct Vector : RandomAccess
        {
            Vector(Agent & agent, Keys keys) : RandomAccess { agent, std::move(keys) } {}
            void reset() { RandomAccess::agent.template reset<Pathway...>((Keys &)(*this)); }
            void reserve(std::size_t size) { RandomAccess::agent.template reserve<Pathway...>(size, (Keys &)(*this)); }
            void trim() { RandomAccess::agent.template trim<Pathway...>((Keys &)(*this)); }
            void assignDefault(std::size_t size) { RandomAccess::agent.template assignDefault<Pathway...>(size, (Keys &)(*this)); }
            template <class U> void assign(std::size_t size, U && value) { RandomAccess::agent.template assign<Pathway...>(size, std::forward<U>(value), (Keys &)(*this)); }
            template <class U> void operator=(U && value) {
                if constexpr ( has_path_selections<Pathway...> )
                    RandomAccess::agent.template setL<Pathway...>(std::forward<U>(value), (Keys &)(*this));
                else
                    RandomAccess::agent.template set<Pathway...>(std::forward<U>(value), (Keys &)(*this));
            }
            template <class U> void operator +=(U && value) { RandomAccess::agent.template plusEq<Pathway...>(std::forward<U>(value), (Keys &)(*this)); }
            template <class U> void operator -=(U && value) { RandomAccess::agent.template minusEq<Pathway...>(std::forward<U>(value), (Keys &)(*this)); }
            template <class U> void operator *=(U && value) { RandomAccess::agent.template multEq<Pathway...>(std::forward<U>(value), (Keys &)(*this)); }
            template <class U> void operator /=(U && value) { RandomAccess::agent.template divEq<Pathway...>(std::forward<U>(value), (Keys &)(*this)); }
            template <class U> void operator %=(U && value) { RandomAccess::agent.template modEq<Pathway...>(std::forward<U>(value), (Keys &)(*this)); }
            template <class U> void operator ^=(U && value) { RandomAccess::agent.template xorEq<Pathway...>(std::forward<U>(value), (Keys &)(*this)); }
            template <class U> void operator &=(U && value) { RandomAccess::agent.template andEq<Pathway...>(std::forward<U>(value), (Keys &)(*this)); }
            template <class U> void operator |=(U && value) { RandomAccess::agent.template orEq<Pathway...>(std::forward<U>(value), (Keys &)(*this)); }

            template <class SetIndexes, class Value> void set(SetIndexes && indexes, Value && value) { RandomAccess::agent.template setN<Pathway...>(indexes, std::forward<Value>(value), (Keys &)(*this)); }
            template <class U> void append(U && value) {
                if constexpr ( RareTs::is_iterable_v<std::remove_cvref_t<U>> && !RareTs::is_optional_v<std::remove_cvref_t<U>> )
                    RandomAccess::agent.template appendN<Pathway...>(std::forward<U>(value), (Keys &)(*this));
                else
                    RandomAccess::agent.template append<Pathway...>(std::forward<U>(value), (Keys &)(*this));
            }
            template <class InsertionIndex, class Value> void insert(InsertionIndex insertionIndex, Value && value) {
                if constexpr ( RareTs::is_iterable_v<std::remove_cvref_t<Value>> && !RareTs::is_optional_v<std::remove_cvref_t<Value>> )
                    RandomAccess::agent.template insertN<Pathway...>(insertionIndex, std::forward<Value>(value), (Keys &)(*this));
                else
                    RandomAccess::agent.template insert<Pathway...>(insertionIndex, std::forward<Value>(value), (Keys &)(*this));
            }
            template <class RemovalIndex> void remove(RemovalIndex removalIndex) {
                if constexpr ( RareTs::is_iterable_v<std::remove_cvref_t<RemovalIndex>> )
                    RandomAccess::agent.template removeN<Pathway...>(removalIndex, (Keys &)(*this));
                else
                    RandomAccess::agent.template remove<Pathway...>(removalIndex, (Keys &)(*this));
            }
            void sort() { RandomAccess::agent.template sort<Pathway...>((Keys &)(*this)); }
            void sortDesc() { RandomAccess::agent.template sortDesc<Pathway...>((Keys &)(*this)); }
            void removeSelection() { RandomAccess::agent.template removeL<Pathway...>((Keys &)(*this)); }

            template <class U> void swap(U && firstIndex, U && secondIndex) {
                RandomAccess::agent.template swap<Pathway...>(std::forward<U>(firstIndex), std::forward<U>(secondIndex), (Keys &)(*this));
            }
            template <class U> void moveUp(U && movedIndex) {
                if constexpr ( RareTs::is_iterable_v<std::remove_cvref_t<U>> )
                    RandomAccess::agent.template moveUpN<Pathway...>(std::forward<U>(movedIndex), (Keys &)(*this));
                else
                    RandomAccess::agent.template moveUp<Pathway...>(std::forward<U>(movedIndex), (Keys &)(*this));
            }
            template <class U> void moveTop(U && movedIndex) {
                if constexpr ( RareTs::is_iterable_v<std::remove_cvref_t<U>> )
                    RandomAccess::agent.template moveTopN<Pathway...>(std::forward<U>(movedIndex), (Keys &)(*this));
                else
                    RandomAccess::agent.template moveTop<Pathway...>(std::forward<U>(movedIndex), (Keys &)(*this));
            }
            template <class U> void moveDown(U && movedIndex) {
                if constexpr ( RareTs::is_iterable_v<std::remove_cvref_t<U>> )
                    RandomAccess::agent.template moveDownN<Pathway...>(std::forward<U>(movedIndex), (Keys &)(*this));
                else
                    RandomAccess::agent.template moveDown<Pathway...>(std::forward<U>(movedIndex), (Keys &)(*this));
            }
            template <class U> void moveBottom(U && movedIndex) {
                if constexpr ( RareTs::is_iterable_v<std::remove_cvref_t<U>> )
                    RandomAccess::agent.template moveBottomN<Pathway...>(std::forward<U>(movedIndex), (Keys &)(*this));
                else
                    RandomAccess::agent.template moveBottom<Pathway...>(std::forward<U>(movedIndex), (Keys &)(*this));
            }
            template <class I, class U> void moveTo(U && movedIndex, I indexMovedTo) {
                if constexpr ( RareTs::is_iterable_v<std::remove_cvref_t<U>> )
                    RandomAccess::agent.template moveToN<Pathway...>(std::forward<U>(movedIndex), indexMovedTo, (Keys &)(*this));
                else
                    RandomAccess::agent.template moveTo<Pathway...>(std::forward<U>(movedIndex), indexMovedTo, (Keys &)(*this));
            }

            void moveSelectionsUp() { RandomAccess::agent.template moveUpL<Pathway...>((Keys &)(*this)); }
            void moveSelectionsTop() { RandomAccess::agent.template moveTopL<Pathway...>((Keys &)(*this)); }
            void moveSelectionsDown() { RandomAccess::agent.template moveDownL<Pathway...>((Keys &)(*this)); }
            void moveSelectionsBottom() { RandomAccess::agent.template moveBottomL<Pathway...>((Keys &)(*this)); }
            template <class I> void moveSelectionsTo(I indexMovedTo) { RandomAccess::agent.template moveToL<Pathway...>(indexMovedTo, (Keys &)(*this)); }
        };
    };
    
    template <class Agent, class default_index_type, class RootData, class T, class Keys, class ... Pathway>
    static constexpr auto editVector();
    
    template <class Agent, class default_index_type, class RootData, class T, class Keys, class ... Pathway>
    static constexpr auto editArray();

    template <class Agent, class default_index_type, class RootData, class T, class Keys, class ... Pathway>
    struct EditOptional : Keys
    {
        using keys = Keys;
        using path = type_tags<Pathway...>;
        using editor_type = typename Agent::editor_type;
        using type = T;
        using optional_value_type = typename std::remove_cvref_t<type>::value_type;

    private:
        static constexpr auto derefType()
        {
            if constexpr ( RareTs::is_static_array_v<optional_value_type> )
                return decltype(editArray<Agent, default_index_type, RootData, optional_value_type, Keys, Pathway..., PathMember<0>>()){};
            else if constexpr ( RareTs::is_specialization_v<optional_value_type, std::vector> ) // Vector
                return decltype(editVector<Agent, default_index_type, RootData, optional_value_type, Keys, Pathway..., PathMember<0>>()){};
            else if constexpr ( RareTs::is_macro_reflected_v<optional_value_type> ) // Reflected object
                return std::type_identity<edit_members<Agent, default_index_type, RootData, optional_value_type, Keys, Pathway..., PathMember<0>>>{};
            else if constexpr ( RareTs::is_optional_v<optional_value_type> )
                return std::type_identity<EditOptional<Agent, default_index_type, RootData, optional_value_type, Keys, Pathway..., PathMember<0>>>{};
            else // Primitive
                return std::type_identity<EditPrimitive<Agent, default_index_type, RootData, optional_value_type, Keys, Pathway..., PathMember<0>>>{};
        }

        Agent & agent;
        typename decltype(derefType())::type deref;

    public:

        EditOptional(Agent & agent, Keys keys) :
            Keys {std::move(keys)}, agent(agent), deref{agent, (Keys &)(*this)} {}

        template <class U> void operator=(U && value) {
            if constexpr ( has_path_selections<Pathway...> )
                agent.template setL<Pathway...>(std::forward<U>(value), (Keys &)(*this));
            else
                agent.template set<Pathway...>(std::forward<U>(value), (Keys &)(*this));
        }
        constexpr auto operator->() { return &deref; }
    };

    template <class Agent, class default_index_type, class RootData, class T, class Keys, class ... Pathway>
    static constexpr auto editVector()
    {
        using element_type = RareTs::element_type_t<T>;
        if constexpr ( RareTs::is_reflected_v<element_type> )
            return std::type_identity<typename Editable<Agent, default_index_type, RootData, element_type, Keys, edit_members, Pathway...>::Vector>{};
        else if constexpr ( RareTs::is_optional_v<element_type> )
            return std::type_identity<typename Editable<Agent, default_index_type, RootData, element_type, Keys, EditOptional, Pathway...>::Vector>{};
        else
            return std::type_identity<typename Editable<Agent, default_index_type, RootData, element_type, Keys, EditPrimitive, Pathway...>::Vector>{};
    }

    template <class Agent, class default_index_type, class RootData, class T, class Keys, class ... Pathway>
    static constexpr auto editArray()
    {
        using element_type = RareTs::element_type_t<T>;
        if constexpr ( RareTs::is_static_array_v<element_type> )
        {
            using sub_element_type = RareTs::element_type_t<element_type>;
            if constexpr ( RareTs::is_reflected_v<sub_element_type> )
                return std::type_identity<typename Editable<Agent, default_index_type, RootData, element_type, Keys, edit_members, Pathway...>::Array>{};
            else if constexpr ( RareTs::is_optional_v<sub_element_type> )
                return std::type_identity<typename Editable<Agent, default_index_type, RootData, element_type, Keys, EditOptional, Pathway...>::Array>{};
            else
                return std::type_identity<typename Editable<Agent, default_index_type, RootData, element_type, Keys, EditPrimitive, Pathway...>::Array>{};
        }
        else if constexpr ( RareTs::is_reflected_v<element_type> )
            return std::type_identity<typename Editable<Agent, default_index_type, RootData, element_type, Keys, edit_members, Pathway...>::Array>{};
        else if constexpr ( RareTs::is_optional_v<element_type> )
            return std::type_identity<typename Editable<Agent, default_index_type, RootData, element_type, Keys, EditOptional, Pathway...>::Array>{};
        else
            return std::type_identity<typename Editable<Agent, default_index_type, RootData, element_type, Keys, EditPrimitive, Pathway...>::Array>{};
    }

    template <class Agent, class default_index_type, class RootData, class T, class Keys, std::size_t I, class ... Pathway>
    static constexpr auto editMember()
    {
        using member_type = typename RareTs::Member<T, I>::type;
        if constexpr ( RareTs::is_static_array_v<member_type> )
            return decltype(editArray<Agent, default_index_type, RootData, member_type, Keys, Pathway..., PathMember<I>>()){};
        else if constexpr ( RareTs::is_specialization_v<member_type, std::vector> ) // Vector
            return decltype(editVector<Agent, default_index_type, RootData, member_type, Keys, Pathway..., PathMember<I>>()){};
        else if constexpr ( RareTs::is_macro_reflected_v<member_type> ) // Reflected object
            return std::type_identity<edit_members<Agent, default_index_type, RootData, member_type, Keys, Pathway..., PathMember<I>>>{};
        else if constexpr ( RareTs::is_optional_v<member_type> )
            return std::type_identity<EditOptional<Agent, default_index_type, RootData, member_type, Keys, Pathway..., PathMember<I>>>{};
        else // Primitive
            return std::type_identity<EditPrimitive<Agent, default_index_type, RootData, member_type, Keys, Pathway..., PathMember<I>>>{};
    }

    template <typename T> struct is_selection_leaf_member {
        static constexpr bool value = T::isData && !T::isStatic &&
            (RareTs::is_static_array_v<typename T::type> ||
             RareTs::is_specialization_v<typename T::type, std::vector>);
    };

    template <typename T> inline constexpr bool is_selection_leaf_member_v = is_selection_leaf_member<T>::value;

    template <class T, std::size_t ... Is> static constexpr bool hasSelectionMember(std::index_sequence<Is...>)
    {
        using member_type = typename T::type;
        if constexpr ( RareTs::is_macro_reflected_v<member_type> )
        {
            return (hasSelectionMember<RareTs::Member<member_type, Is>>(
                std::make_index_sequence<reflectedMemberCount<typename RareTs::Member<member_type, Is>::type>()>()
            ) || ...);
        }
        else
            return is_selection_leaf_member_v<T>;
    };

    template <typename T> struct is_selection_member {
        static constexpr bool value = hasSelectionMember<T>(std::make_index_sequence<reflectedMemberCount<typename T::type>()>());
    };
    
    template <class DefaultIndexType, class T, std::size_t ... Is>
    struct object_selection;

    template <class DefaultIndexType, class T, std::size_t I>
    static constexpr auto memberSelection()
    {
        using member = RareTs::Member<T, I>;
        using member_type = typename member::type;
        if constexpr ( !is_selection_leaf_member_v<member> )
        {
            return RareTs::template Members<member_type>::template pack<is_selection_member>([&](auto ... member) {
                return object_selection<DefaultIndexType, member_type, decltype(member)::index...>{};
            });
        }
        else if constexpr ( RareTs::is_static_array_v<member_type> || RareTs::is_specialization_v<member_type, std::vector> )
            return std::vector<index_type_t<DefaultIndexType, member>> {};
        else
            static_assert(std::is_void_v<T>, "Unexpected member selection!");
    }

    template <class DefaultIndexType, class T>
    struct member_selection {
        template <std::size_t I> using type = decltype(memberSelection<DefaultIndexType, T, I>());
    };
    
    inline constexpr std::nullptr_t noSelection {};

    template <class DefaultIndexType, class T, std::size_t ... Is>
    struct object_selection : RareTs::Class::adapt_member<member_selection<DefaultIndexType, T>::template type, T, Is>...
    {
        template <std::size_t I> constexpr auto & fromMember()
        {
            if constexpr ( std::is_base_of_v<RareTs::Class::adapt_member<member_selection<DefaultIndexType, T>::template type, T, I>, object_selection<DefaultIndexType, T, Is...>> )
            {
                auto & [selection] = static_cast<RareTs::Class::adapt_member<member_selection<DefaultIndexType, T>::template type, T, I> &>(*this);
                return selection;
            }
            else
            {
                return noSelection;
            }
        }

        template <std::size_t I>
        void clear()
        {
            using U = RareTs::Class::adapt_member<member_selection<DefaultIndexType, T>::template type, T, I>;
            if constexpr ( requires { static_cast<U &>(*this).clear(); } )
                static_cast<U &>(*this).clear();
        }

        void clear()
        {
            (clear<Is>(), ...);
        }
    };

    template <class DefaultIndexType, class T, std::size_t ... Is>
    constexpr auto selections(std::index_sequence<Is...>) {
        return object_selection<DefaultIndexType, T, Is...>{};
    }

    template <class DefaultIndexType, class T> auto selections() {
        return RareTs::template Members<T>::template pack<is_selection_member>([&](auto & ... member) {
            return selections<DefaultIndexType, T>(std::index_sequence<RareTs::remove_cvref_t<decltype(member)>::index...>{});
        });
    }

    template <class T, class Edit>
    struct ReadEditPair
    {
        using read_type = T;
        using edit_type = Edit;

        const T & read;
        Edit edit;
    };

    template <class T, class User, class EditorType>
    struct Agent
    {
        using type = T;
        using editor_type = EditorType;
        using default_index_type = typename decltype(defaultIndexType<T>())::type;
        decltype(RareEdit::selections<default_index_type, T>()) selections {};
        std::vector<std::uint8_t> events {std::uint8_t(0)}; // First byte is unused
        std::vector<std::uint64_t> eventOffsets {};
        T & t;
        User & user;

        void clear()
        {
            selections.clear();
            eventOffsets.clear();
            events.clear();
        }

        template <class Usr, class Route, class Value>
        using ValueChangedOp = decltype(std::declval<Usr>().valueChanged(std::declval<Route>(), std::declval<Value>(), std::declval<Value>()));

        template <class Usr, class Route>
        using ElementAddedOp = decltype(std::declval<Usr>().elementAdded(std::declval<Route>(), std::declval<std::size_t>()));

        template <class Usr, class Route>
        using ElementRemovedOp = decltype(std::declval<Usr>().elementRemoved(std::declval<Route>(), std::declval<std::size_t>()));

        template <class Usr, class Route>
        using ElementMovedOp = decltype(std::declval<Usr>().elementMoved(std::declval<Route>(), std::declval<std::size_t>(), std::declval<std::size_t>()));

        template <class Usr, class Route>
        using SelectionsChangedOp = decltype(std::declval<Usr>().selectionsChanged(std::declval<Route>()));

        template <class Route, class Value>
        static constexpr bool hasValueChangedOp = RareTs::op_exists_v<ValueChangedOp, User, Route, Value>;

        template <class Route>
        static constexpr bool hasElementAddedOp = RareTs::op_exists_v<ElementAddedOp, User, Route>;

        template <class Route>
        static constexpr bool hasElementRemovedOp = RareTs::op_exists_v<ElementRemovedOp, User, Route>;

        template <class Route>
        static constexpr bool hasElementMovedOp = RareTs::op_exists_v<ElementMovedOp, User, Route>;

        template <class Route>
        static constexpr bool hasSelectionsChangedOp = RareTs::op_exists_v<SelectionsChangedOp, User, Route>;

        template <class Usr, class Route, class OldValue, class NewValue>
        static constexpr void notifyValueChanged(Usr & user, Route route, OldValue && oldValue, NewValue && newValue)
        {
            user.valueChanged(route, std::forward<OldValue>(oldValue), std::forward<NewValue>(newValue));
        }

        template <class Usr, class Route>
        static constexpr void notifyElementAdded(Usr & user, Route route, std::size_t index)
        {
            user.elementAdded(route, index);
        }

        template <class Usr, class Route>
        static constexpr void notifyElementRemoved(Usr & user, Route route, std::size_t index)
        {
            user.elementRemoved(route, index);
        }

        template <class Usr, class Route>
        static constexpr void notifyElementMoved(Usr & user, Route route, std::size_t oldIndex, std::size_t newIndex)
        {
            user.elementMoved(route, oldIndex, newIndex);
        }

        template <class Usr, class Route>
        static constexpr void notifySelectionsChanged(Usr & user, Route route)
        {
            user.selectionsChanged(route);
        }

        Agent(T & t, User & user) : t(t), user(user) {}

        auto getEventOffsetRange(std::size_t eventIndex) const
        {
            struct OffsetRange
            {
                std::uint64_t begin;
                std::uint64_t end;
            };
            return OffsetRange {
                eventOffsets[eventIndex],
                eventIndex == eventOffsets.size()-1 ? events.size() : eventOffsets[eventIndex+1]
            };
        }
        
        template <class PathElement, class ... Pathway>
        constexpr auto & getSelectionsData(auto & selections)
        {
            if constexpr ( is_path_member_v<PathElement> && !std::is_null_pointer_v<std::remove_cvref_t<decltype(selections)>> )
            {
                if constexpr ( sizeof...(Pathway) == 0 )
                    return selections.template fromMember<PathElement::index>();
                else
                    return getSelectionsData<Pathway...>(selections.template fromMember<PathElement::index>());
            }
            else
            {
                return noSelection;
            }
        }

        template <class ... Pathway>
        constexpr auto & getSelections() {
            if constexpr ( sizeof...(Pathway) == 0 )
                return noSelection;
            else
                return getSelectionsData<Pathway...>(selections);
        }

        template <class ... Pathway>
        static constexpr bool hasSelections() {
            return !std::is_null_pointer_v<std::remove_cvref_t<decltype(std::declval<Agent>().template getSelections<Pathway...>())>>;
        }

        template <class Keys, class U, class PathElement, class ... Pathway>
        auto & getMemberReferenceImpl(U & t, Keys & keys)
        {
            if constexpr ( is_path_member_v<PathElement> )
            {
                using Member = RareTs::Member<U, PathElement::index>;
                if constexpr ( sizeof...(Pathway) == 0 )
                    return Member::value(t);
                else
                    return getMemberReferenceImpl<Keys, typename Member::type, Pathway...>(Member::value(t), keys);
            }
            else if constexpr ( is_path_index_v<PathElement> )
            {
                if constexpr ( sizeof...(Pathway) == 0 )
                    return t[std::get<PathElement::index>(keys)];
                else
                {
                    return getMemberReferenceImpl<Keys, std::remove_cvref_t<decltype(t[std::get<PathElement::index>(keys)])>, Pathway...>(
                        t[std::get<PathElement::index>(keys)], keys);
                }
            }
            else
                static_assert(std::is_void_v<PathElement>, "Unexpected path element type!");
        }

        template <class ... Pathway, class Keys, class U>
        auto & getMemberReference(U & t, Keys & keys)
        {
            return getMemberReferenceImpl<Keys, U, Pathway...>(t, keys);
        }

        template <class PathElement, class ... Pathway, class Keys>
        void serializePathway(Keys & keys)
        {
            if constexpr ( sizeof...(Pathway) == 0 )
            {
                if constexpr ( is_path_selections_v<PathElement> )
                    events.push_back(uint8_t(PathOp::LeafSelBranch));
                else if constexpr ( is_path_member_v<PathElement> )
                {
                    static_assert(PathElement::index < 64, "Expected a lower member index...");
                    events.push_back(uint8_t(PathOp::LeafBranch) | uint8_t(PathElement::index));
                }
                else if constexpr ( is_path_index_v<PathElement> )
                {
                    if constexpr ( sizeof...(Pathway) == 0 )
                    {
                        auto index = std::get<PathElement::index>(keys);
                        if constexpr ( std::is_same_v<uint6_t, decltype(index)> )
                            events.push_back(uint8_t(PathOp::LeafBranch) | uint8_t(index));
                        else
                        {
                            events.push_back(uint8_t(PathOp::LeafBranch));
                            events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&index), reinterpret_cast<const std::uint8_t*>(&index)+sizeof(index));
                        }
                    }
                }
                else
                    static_assert(std::is_void_v<PathElement>, "Unrecognized path element!");
            }
            else // More path remaining
            {
                if constexpr ( is_path_selections_v<PathElement> )
                    events.push_back(uint8_t(PathOp::SelBranch));
                else if constexpr ( is_path_member_v<PathElement> )
                {
                    static_assert(PathElement::index < 64, "Expected a lower member index...");
                    events.push_back(uint8_t(PathOp::Branch) | uint8_t(PathElement::index));
                }
                else if constexpr ( is_path_index_v<PathElement> )
                {
                    auto index = std::get<PathElement::index>(keys);
                    if constexpr ( std::is_same_v<uint6_t, decltype(index)> )
                        events.push_back(uint8_t(PathOp::Branch) | uint8_t(index));
                    else
                    {
                        events.push_back(uint8_t(PathOp::Branch));
                        events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&index), reinterpret_cast<const std::uint8_t*>(&index)+sizeof(index));
                    }
                }
                else
                    static_assert(std::is_void_v<PathElement>, "Unrecognized path element!");

                serializePathway<Pathway...>(keys);
            }
        }

        template <class Member> void serializeValue(auto && value)
        {
            using value_type = std::remove_cvref_t<decltype(value)>;
            using index_type = index_type_t<default_index_type, Member>;
            if constexpr ( std::is_same_v<value_type, std::string> )
            {
                auto size = static_cast<index_type>(value.size());
                events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));
                events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(value.c_str()), reinterpret_cast<const std::uint8_t*>(value.c_str()+size));
            }
            else if constexpr ( RareTs::is_optional_v<value_type> )
            {
                bool hasValue = value.has_value();
                u8bool::write(events, hasValue);
                serializeValue<Member>(*value);
            }
            else if constexpr ( is_flat_mdspan_v<value_type> )
            {
                constexpr auto size = static_cast<index_type>(value_type::size);
                events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));
                for ( auto it = value.flatBegin(); it != value.flatEnd(); ++it )
                    serializeValue<Member>(*it);
            }
            else if constexpr ( std::is_array_v<value_type> )
            {
                auto span = as_1d<const value_type>(value);
                constexpr auto size = static_cast<index_type>(decltype(span)::size);
                events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));
                for ( auto it = span.flatBegin(); it != span.flatEnd(); ++it )
                    serializeValue<Member>(*it);
            }
            else if constexpr ( RareTs::is_static_array_v<value_type> )
            {
                constexpr auto size = static_cast<index_type>(RareTs::static_array_size_v<value_type>);
                events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));
                for ( auto & val : value )
                    serializeValue<Member>(value);
            }
            else if constexpr ( RareTs::is_iterable_v<value_type> )
            {
                auto size = static_cast<index_type>(value.size());
                events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));
                for ( auto & val : value )
                    serializeValue<Member>(val);
            }
            else if constexpr ( RareTs::is_reflected_v<value_type> )
            {
                RareTs::Members<value_type>::forEach([&](auto member) {
                    serializeValue<decltype(member)>(member.value(value));
                });
            }
            else
                events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&value), reinterpret_cast<const std::uint8_t*>(&value)+sizeof(value));
        }

        template <class Member> void serializeIndex(auto && index)
        {
            auto val = static_cast<index_type_t<default_index_type, Member>>(index);
            events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&val), reinterpret_cast<const std::uint8_t*>(&val)+sizeof(val));
        }

        template <class Member>
        void serializeValueIfFirst(auto & value, bool & first)
        {
            if ( first )
            {
                first = false;
                serializeValue<Member>(value);
            }
        }

        template <bool AfterSel, class Keys, class U, class F, class LastMember, class PathElement, class ... Pathway, class ... PathTraversed>
        void operateThruSelImpl(U & t, Keys & keys, F f, type_tags<PathTraversed...>)
        {
            if constexpr ( is_path_selections_v<PathElement> )
            {
                // Replace the PathSel element with a PathIndex; at present selections must be a top-level iterable preceding other uses of keys
                using index_type = index_type_t<default_index_type, LastMember>;
                auto & selData = getSelections<PathTraversed...>();
                for ( auto & sel : selData )
                {
                    auto newKeys = std::tuple_cat(std::tuple<index_type>{sel}, keys);
                    using path_tagged_keys = std::remove_cvref_t<decltype(newKeys)>;
                    if constexpr ( sizeof...(Pathway) == 0 )
                        f(t[static_cast<std::size_t>(sel)], type_tags<LastMember, PathTaggedKeys<path_tagged_keys, type_tags<PathTraversed..., PathIndex<0>>, EditorType>>{}, newKeys);
                    else
                        operateThruSelImpl<true, path_tagged_keys, std::remove_cvref_t<decltype(t[0])>, F, LastMember, Pathway...>(t[sel], newKeys, f, type_tags<PathTraversed..., PathIndex<0>>{});
                }
            }
            else if constexpr ( is_path_member_v<PathElement> )
            {
                using Member = RareTs::Member<U, PathElement::index>;
                if constexpr ( sizeof...(Pathway) == 0 )
                    f(Member::value(t), type_tags<Member, PathTaggedKeys<Keys, type_tags<PathTraversed..., PathElement>, EditorType>>{}, keys);
                else
                    operateThruSelImpl<AfterSel, Keys, typename Member::type, F, Member, Pathway...>(Member::value(t), keys, f, type_tags<PathTraversed..., PathElement>{});
            }
            else if constexpr ( is_path_index_v<PathElement> )
            {
                // If this is after the selection, PathIndex index is incremented so it's now correctly after the inserted sel index
                using path_elem = std::conditional_t<AfterSel, PathIndex<PathElement::index+1>, PathElement>;
                if constexpr ( sizeof...(Pathway) == 0 )
                {
                    f(t[static_cast<std::size_t>(std::get<path_elem::index>(keys))],
                        type_tags<LastMember, PathTaggedKeys<Keys, type_tags<PathTraversed..., path_elem>, EditorType>>{}, keys);
                }
                else
                {
                    operateThruSelImpl<AfterSel, Keys, std::remove_cvref_t<decltype(t[0])>, F, LastMember, Pathway...>(
                        t[static_cast<std::size_t>(std::get<path_elem::index>(keys))], keys, f, type_tags<PathTraversed..., path_elem>{});
                }
            }
            else
                static_assert(std::is_void_v<PathElement>, "Unexpected path element type!");
        }
        
        // When operating through selections, a modified keys tuple must be built and passed to f
        template <class ... Pathway, class Keys, class U, class F>
        void operateThruSel(U & t, Keys & keys, F f)
        {
            operateThruSelImpl<false, Keys, U, F, void, Pathway...>(t, keys, f, {});
        }

        template <class Keys, class U, class F, class LastMember, class PathElement, class ... Pathway, class ... PathTraversed>
        void operateOnImpl(U & t, Keys & keys, F f, type_tags<PathTraversed...>)
        {
            if constexpr ( is_path_selections_v<PathElement> )
            {
                auto & selData = getSelections<PathTraversed...>();
                for ( auto & sel : selData )
                {
                    if constexpr ( sizeof...(Pathway) == 0 )
                        f(t[static_cast<std::size_t>(sel)], type_tags<LastMember, PathTaggedKeys<Keys, type_tags<PathTraversed..., PathElement>, EditorType>>{});
                    else
                        operateOnImpl<Keys, std::remove_cvref_t<decltype(t[0])>, F, LastMember, Pathway...>(t[sel], keys, f, type_tags<PathTraversed..., PathElement>{});
                }
            }
            else if constexpr ( is_path_member_v<PathElement> )
            {
                if constexpr ( RareTs::is_optional_v<std::remove_cvref_t<U>> )
                {
                    if constexpr ( sizeof...(Pathway) == 0 )
                        f(t, type_tags<LastMember, PathTaggedKeys<Keys, type_tags<PathTraversed..., PathElement>, EditorType>>{});
                    else if ( t.has_value() )
                        operateOnImpl<Keys, typename std::remove_cvref_t<U>::value_type, F, LastMember, Pathway...>(*t, keys, f, type_tags<PathTraversed..., PathElement>{});
                }
                else
                {
                    using Member = RareTs::Member<U, PathElement::index>;
                    if constexpr ( sizeof...(Pathway) == 0 )
                        f(Member::value(t), type_tags<Member, PathTaggedKeys<Keys, type_tags<PathTraversed..., PathElement>, EditorType>>{});
                    else
                        operateOnImpl<Keys, typename Member::type, F, Member, Pathway...>(Member::value(t), keys, f, type_tags<PathTraversed..., PathElement>{});
                }
            }
            else if constexpr ( is_path_index_v<PathElement> )
            {
                if constexpr ( sizeof...(Pathway) == 0 )
                    f(t[static_cast<std::size_t>(std::get<PathElement::index>(keys))], type_tags<LastMember, PathTaggedKeys<Keys, type_tags<PathTraversed..., PathElement>, EditorType>>{});
                else
                {
                    operateOnImpl<Keys, std::remove_cvref_t<decltype(t[static_cast<std::size_t>(std::get<PathElement::index>(keys))])>,
                        F, LastMember, Pathway...>(t[static_cast<std::size_t>(std::get<PathElement::index>(keys))], keys, f, type_tags<PathTraversed..., PathElement>{});
                }
            }
            else
                static_assert(std::is_void_v<PathElement>, "Unexpected path element type!");
        }
        
        template <class ... Pathway, class Keys, class U, class F>
        void operateOn(U & t, Keys & keys, F f)
        {
            if constexpr ( sizeof...(Pathway) == 0 )
                f(t, type_tags<void, PathTaggedKeys<Keys, type_tags<>, EditorType>>{});
            else
                operateOnImpl<Keys, U, F, void, Pathway...>(t, keys, f, {});
        }
        
        template <class Keys, class U, class LastMember, class PathElement, class ... Pathway, class ... PathTraversed>
        auto editorFromPathImpl(auto && editor, U & t, Keys & keys, type_tags<PathTraversed...>)
        {
            if constexpr ( is_path_selections_v<PathElement> )
            {
                auto & selData = getSelections<PathTraversed...>();
                for ( auto & sel : selData )
                {
                    if constexpr ( sizeof...(Pathway) == 0 )
                        return ReadEditPair{((const U &)t)[sel], editor[sel]};
                    else
                        return editorFromPathImpl<Keys, std::remove_cvref_t<decltype(t[0])>, LastMember, Pathway...>(editor[sel], t[sel], keys, type_tags<PathTraversed..., PathElement>{});
                }
            }
            else if constexpr ( is_path_member_v<PathElement> )
            {
                if constexpr ( RareTs::is_optional_v<std::remove_cvref_t<U>> )
                {
                    if constexpr ( sizeof...(Pathway) == 0 )
                        return ReadEditPair{*((const U &)t), *editor};
                    else if ( t.has_value() )
                        return editorFromPathImpl<Keys, typename std::remove_cvref_t<U>::value_type, LastMember, Pathway...>(*editor, *t, keys, type_tags<PathTraversed..., PathElement>{});
                }
                else
                {
                    using Member = RareTs::Member<U, PathElement::index>;
                    if constexpr ( sizeof...(Pathway) == 0 )
                        return ReadEditPair{Member::value((const U &)t), editor.template fromMember<PathElement::index>()};
                    else
                        return editorFromPathImpl<Keys, typename Member::type, Member, Pathway...>(editor.template fromMember<PathElement::index>(), Member::value(t), keys, type_tags<PathTraversed..., PathElement>{});
                }
            }
            else if constexpr ( is_path_index_v<PathElement> )
            {
                auto index = static_cast<std::size_t>(std::get<PathElement::index>(keys));
                if constexpr ( sizeof...(Pathway) == 0 )
                    return ReadEditPair{((const U &)t)[index], editor[index]};
                else
                {
                    return editorFromPathImpl<Keys, std::remove_cvref_t<decltype(t[index])>,
                        LastMember, Pathway...>(editor[index], t[index], keys, type_tags<PathTraversed..., PathElement>{});
                }
            }
            else
                static_assert(std::is_void_v<PathElement>, "Unexpected path element type!");
        }

        template <class ... Pathway, class Keys, class U>
        auto editorFromPath(auto & editor, U & t, Keys & keys)
        {
            if constexpr ( sizeof...(Pathway) == 0 )
                return ReadEditPair{(const U &)t, editor};
            else
                return editorFromPathImpl<Keys, U, void, Pathway...>(editor, t, keys, {});
        }

        template <class ... Pathway, class Keys>
        void reset(Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Reset));
            serializePathway<Pathway...>(keys);

            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                using value_type = std::remove_cvref_t<decltype(ref)>;
                constexpr bool isIterable = RareTs::is_iterable_v<value_type>;
                if constexpr ( isIterable && hasElementRemovedOp<Route> ) // Iterable
                {
                    std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1;
                    serializeValue<Member>(ref);
                    ref = {};
                    for ( ; i>=0; --i )
                        notifyElementRemoved(user, Route{keys}, static_cast<std::size_t>(i)); // Issue remove changes
                }
                else if constexpr ( !isIterable && hasValueChangedOp<Route, value_type> ) // Non-iterables
                {
                    if constexpr ( std::is_array_v<typename Member::type> )
                    {
                        serializeValue<Member>(ref);
                        typename Member::type prevValue {};
                        auto refArray = as_1d<typename Member::type>(ref);
                        as_1d(prevValue) = refArray;
                        refArray.clear();
                        notifyValueChanged(user, Route{keys}, prevValue, ref); // Issue change notification
                    }
                    else
                    {
                        auto prevValue = ref;
                        serializeValue<Member>(ref);
                        ref = {};
                        notifyValueChanged(user, Route{keys}, prevValue, ref); // Issue change notification
                    }
                }
                else
                {
                    serializeValue<Member>(ref);
                    if constexpr ( std::is_array_v<typename Member::type> )
                        as_1d<typename Member::type>(ref).clear();
                    else
                        ref = {};
                }

                if constexpr ( hasSelections<Pathway...>() )
                {
                    auto & sel = getSelections<Pathway...>();
                    writeSelections(events, sel);
                    if constexpr ( hasSelectionsChangedOp<Route> )
                    {
                        if ( !std::empty(sel) )
                        {
                            clearSel(sel);
                            notifySelectionsChanged(user, Route{keys});
                        }
                    }
                    else
                        clearSel(sel);
                }
            });
        }

        template <class ... Pathway, class Keys>
        void reserve(std::size_t size, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Reserve));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serializeIndex<Member>(size);
                ref.reserve(size);
            });
        }

        template <class ... Pathway, class Keys>
        void trim(Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Trim));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                ref.shrink_to_fit();
            });
        }

        template <class ... Pathway, class Value, class Keys>
        void assign(std::size_t size, Value && value, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Assign));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serializeIndex<Member>(size);
                serializeValue<Member>(value);
                serializeValue<Member>(ref);
                if constexpr ( hasElementRemovedOp<Route> )
                {
                    std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1;
                    ref.assign(size, value);
                    for ( ; i>=0; --i )
                        notifyElementRemoved(user, Route{keys}, static_cast<std::size_t>(i));
                }
                else
                    ref.assign(size, value);

                if constexpr ( hasElementAddedOp<Route> )
                {
                    for ( std::size_t i=0; i<std::size(ref); ++i )
                        notifyElementAdded(user, Route{keys}, i);
                }

                if constexpr ( hasSelections<Pathway...>() )
                {
                    auto & sel = getSelections<Pathway...>();
                    writeSelections(events, sel);
                    if constexpr ( hasSelectionsChangedOp<Route> )
                    {
                        if ( !std::empty(sel) )
                        {
                            clearSel(sel);
                            notifySelectionsChanged(user, Route{keys});
                        }
                    }
                    else
                        clearSel(sel);
                }
            });
        }

        template <class ... Pathway, class Keys>
        void assignDefault(std::size_t size, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::AssignDefault));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serializeIndex<Member>(size);
                serializeValue<Member>(ref);
                
                if constexpr ( hasElementRemovedOp<Route> )
                {
                    std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1;
                    ref = std::remove_cvref_t<decltype(ref)>(size);
                    for ( ; i>=0; --i )
                        notifyElementRemoved(user, Route{keys}, static_cast<std::size_t>(i));
                }
                else
                    ref = std::remove_cvref_t<decltype(ref)>(size);

                if constexpr ( hasElementAddedOp<Route> )
                {
                    for ( std::size_t i=0; i<std::size(ref); ++i )
                        notifyElementAdded(user, Route{keys}, i);
                }

                if constexpr ( hasSelections<Pathway...>() )
                {
                    auto & sel = getSelections<Pathway...>();
                    writeSelections(events, sel);
                    if constexpr ( hasSelectionsChangedOp<Route> )
                    {
                        if ( !std::empty(sel) )
                        {
                            clearSel(sel);
                            notifySelectionsChanged(user, Route{keys});
                        }
                    }
                    else
                        clearSel(sel);
                }
            });
        }

        template <class ... Pathway, class Value, class Keys>
        void set(Value && value, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Set));
            if constexpr ( sizeof...(Pathway) > 0 )
                serializePathway<Pathway...>(keys);
            else
                events.push_back(uint8_t(PathOp::RootPath));

            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                using value_type = std::remove_cvref_t<decltype(ref)>;
                constexpr bool isIterable = !RareTs::is_optional_v<value_type> && RareTs::is_iterable_v<value_type>;
                if constexpr ( !isIterable && hasValueChangedOp<Route, value_type> )
                {
                    auto prevValue = ref;
                    serializeValue<Member>(static_cast<std::remove_cvref_t<decltype(ref)>>(value)); // Value set to
                    serializeValue<Member>(ref); // Value before changing
                    ref = std::forward<Value>(value);
                    notifyValueChanged(user, Route{keys}, prevValue, ref);
                }
                else
                {
                    serializeValue<Member>(static_cast<std::remove_cvref_t<decltype(ref)>>(value)); // Value set to
                    serializeValue<Member>(ref); // Value before changing
                        
                    if constexpr ( isIterable && hasElementRemovedOp<Route> )
                    {
                        std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1;
                        ref = std::forward<Value>(value);
                        for ( ; i>=0; --i )
                            notifyElementRemoved(user, Route{keys}, static_cast<std::size_t>(i));
                    }
                    else if constexpr ( requires{ref = std::forward<Value>(value);} )
                        ref = std::forward<Value>(value);

                    if constexpr ( isIterable && hasElementAddedOp<Route> )
                    {
                        for ( std::size_t i=0; i<std::size(ref); ++i )
                            notifyElementAdded(user, Route{keys}, i);
                    }
                }

                if constexpr ( hasSelections<Pathway...>() )
                {
                    auto & sel = getSelections<Pathway...>();
                    writeSelections(events, sel);
                    clearSel(sel);

                    if constexpr ( hasSelectionsChangedOp<Route> )
                        notifySelectionsChanged(user, Route{keys});
                }
            });
        }

        template <class ... Pathway, class SetIndexes, class Value, class Keys>
        void setN(SetIndexes && setIndexes, const Value & value, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::SetN));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                using element_type = RareTs::element_type_t<std::remove_cvref_t<decltype(ref)>>;
                using index_type = index_type_t<default_index_type, Member>;
                using ElemPath = type_tags<Pathway..., PathIndex<std::tuple_size_v<std::remove_cvref_t<decltype(keys)>>>>;
                using ElemKeys = std::remove_cvref_t<decltype(std::tuple_cat(keys, std::tuple<index_type>{0}))>;
                using ElemRoute = PathTaggedKeys<ElemKeys, ElemPath, EditorType>;
                constexpr bool isIterableElement = RareTs::is_iterable_v<element_type>;
                if constexpr ( !std::is_void_v<element_type> )
                {
                    if constexpr ( !isIterableElement && hasValueChangedOp<Route, element_type> )
                    {
                        serializeIndex<Member>(std::size(setIndexes)); // Index count
                        for ( auto setIndex : setIndexes )
                            serializeIndex<Member>(setIndex); // Indexes

                        serializeValue<Member>(value); // Value set to

                        for ( auto setIndex : setIndexes )
                            serializeValue<Member>(ref[setIndex]); // Value before changing

                        for ( auto setIndex : setIndexes )
                        {
                            auto prevValue = ref[setIndex];
                            ref[setIndex] = value; // Make the change
                            notifyValueChanged(user, ElemRoute{std::tuple_cat(keys, std::tuple<index_type>{setIndex})}, prevValue, ref[setIndex]);
                        }
                    }
                    else
                    {
                        serializeIndex<Member>(std::size(setIndexes)); // Index count
                        for ( auto setIndex : setIndexes )
                            serializeIndex<Member>(setIndex); // Indexes

                        serializeValue<Member>(value); // Value set to

                        for ( auto setIndex : setIndexes )
                            serializeValue<Member>(ref[setIndex]); // Value before changing

                        if constexpr ( isIterableElement )
                        {
                            for ( auto setIndex : setIndexes )
                            {
                                if constexpr ( hasElementRemovedOp<Route> )
                                {
                                    std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref[setIndex]))-1;
                                    ref[setIndex] = value; // Make the change
                                    for ( ; i>=0; --i )
                                        notifyElementRemoved(user, ElemRoute{std::tuple_cat(keys, std::tuple<index_type>{setIndex})}, static_cast<std::size_t>(i));
                                }
                                else
                                    ref[setIndex] = value; // Make the change
                                
                                if constexpr ( hasElementAddedOp<Route> )
                                {
                                    for ( std::size_t i=0; i<std::size(ref[setIndex]); ++i )
                                        notifyElementAdded(user, ElemRoute{std::tuple_cat(keys, std::tuple<index_type>{setIndex})}, i);
                                }
                            }
                        }
                        else if constexpr ( requires{ref[0] = value;} )
                        {
                            for ( auto setIndex : setIndexes )
                                ref[setIndex] = value; // Make the change
                        }
                    }
                }
            });
        }

        template <class ... Pathway, class Value, class Keys>
        void setL(Value && value, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::SetL));
            serializePathway<Pathway...>(keys);

            bool first = true;
            operateThruSel<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>, auto & newKeys) {
                std::cout << RareTs::toStr<Route>() << '\n';
                using value_type = std::remove_cvref_t<decltype(ref)>;
                constexpr bool isIterable = RareTs::is_iterable_v<value_type>;
                if ( first )
                {
                    first = false;
                    if constexpr ( !isIterable && hasValueChangedOp<Route, value_type> )
                    {
                        auto prevValue = ref;
                        serializeValue<Member>(static_cast<std::remove_cvref_t<decltype(ref)>>(value)); // Value set to
                        serializeValue<Member>(ref); // Value before changing
                        ref = std::forward<Value>(value);
                        notifyValueChanged(user, Route{newKeys}, prevValue, ref);
                    }
                    else
                    {
                        serializeValue<Member>(static_cast<std::remove_cvref_t<decltype(ref)>>(value)); // Value set to
                        serializeValue<Member>(ref); // Value before changing
                        if constexpr ( isIterable && hasElementRemovedOp<Route> )
                        {
                            std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1;
                            ref = std::forward<Value>(value);
                            for ( ; i>=0; --i )
                                notifyElementRemoved(user, Route{newKeys}, static_cast<std::size_t>(i));
                        }
                        else
                            ref = std::forward<Value>(value);

                        if constexpr ( isIterable && hasElementAddedOp<Route> )
                        {
                            for ( std::size_t i=0; i<std::size(ref); ++i )
                                notifyElementAdded(user, Route{newKeys}, i);
                        }
                    }
                }
                else if constexpr ( !isIterable && hasValueChangedOp<Route, value_type> )
                {
                    auto prevValue = ref;
                    serializeValue<Member>(ref); // Value before changing
                    ref = std::forward<Value>(value);
                    notifyValueChanged(user, Route{newKeys}, prevValue, ref);
                }
                else
                {
                    serializeValue<Member>(ref); // Value before changing
                    if constexpr ( isIterable && hasElementRemovedOp<Route> )
                    {
                        std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1;
                        ref = std::forward<Value>(value);
                        for ( ; i>=0; --i )
                            notifyElementRemoved(user, Route{newKeys}, static_cast<std::size_t>(i));
                    }
                    else
                        ref = std::forward<Value>(value);

                    if constexpr ( isIterable && hasElementAddedOp<Route> )
                    {
                        for ( std::size_t i=0; i<std::size(ref); ++i )
                            notifyElementAdded(user, Route{newKeys}, i);
                    }
                }
            });
        }

        template <class ... Pathway, class Value, class Keys>
        void plusEq(Value && value, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Set)); // "Set like operation"
            serializePathway<Pathway...>(keys);

            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                
                using value_type = std::remove_cvref_t<decltype(ref)>;
                if constexpr ( hasValueChangedOp<Route, value_type> )
                {
                    auto prevValue = ref;
                    ref += std::forward<Value>(value);
                    serializeValue<Member>(static_cast<std::remove_cvref_t<decltype(ref)>>(value)); // Value set to
                    serializeValue<Member>(prevValue); // Value before changing
                    notifyValueChanged(user, Route{keys}, prevValue, ref);
                }
                else
                {
                    serializeValue<Member>(ref + value); // Value set to
                    serializeValue<Member>(ref); // Value before changing
                    ref += std::forward<Value>(value);
                }
            });
        }

        template <class ... Pathway, class Value, class Keys>
        void minusEq(Value && value, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Set)); // "Set like operation"
            serializePathway<Pathway...>(keys);

            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                
                using value_type = std::remove_cvref_t<decltype(ref)>;
                if constexpr ( hasValueChangedOp<Route, value_type> )
                {
                    auto prevValue = ref;
                    ref -= std::forward<Value>(value);
                    serializeValue<Member>(static_cast<std::remove_cvref_t<decltype(ref)>>(value)); // Value set to
                    serializeValue<Member>(prevValue); // Value before changing
                    notifyValueChanged(user, Route{keys}, prevValue, ref);
                }
                else
                {
                    serializeValue<Member>(ref - value); // Value set to
                    serializeValue<Member>(ref); // Value before changing
                    ref -= std::forward<Value>(value);
                }
            });
        }

        template <class ... Pathway, class Value, class Keys>
        void multEq(Value && value, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Set)); // "Set like operation"
            serializePathway<Pathway...>(keys);

            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                
                using value_type = std::remove_cvref_t<decltype(ref)>;
                if constexpr ( hasValueChangedOp<Route, value_type> )
                {
                    auto prevValue = ref;
                    ref *= std::forward<Value>(value);
                    serializeValue<Member>(static_cast<std::remove_cvref_t<decltype(ref)>>(value)); // Value set to
                    serializeValue<Member>(prevValue); // Value before changing
                    notifyValueChanged(user, Route{keys}, prevValue, ref);
                }
                else
                {
                    serializeValue<Member>(ref * value); // Value set to
                    serializeValue<Member>(ref); // Value before changing
                    ref *= std::forward<Value>(value);
                }
            });
        }

        template <class ... Pathway, class Value, class Keys>
        void divEq(Value && value, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Set)); // "Set like operation"
            serializePathway<Pathway...>(keys);

            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                
                using value_type = std::remove_cvref_t<decltype(ref)>;
                if constexpr ( hasValueChangedOp<Route, value_type> )
                {
                    auto prevValue = ref;
                    ref /= std::forward<Value>(value);
                    serializeValue<Member>(static_cast<std::remove_cvref_t<decltype(ref)>>(value)); // Value set to
                    serializeValue<Member>(prevValue); // Value before changing
                    notifyValueChanged(user, Route{keys}, prevValue, ref);
                }
                else
                {
                    serializeValue<Member>(ref / value); // Value set to
                    serializeValue<Member>(ref); // Value before changing
                    ref /= std::forward<Value>(value);
                }
            });
        }

        template <class ... Pathway, class Value, class Keys>
        void modEq(Value && value, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Set)); // "Set like operation"
            serializePathway<Pathway...>(keys);

            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                
                using value_type = std::remove_cvref_t<decltype(ref)>;
                if constexpr ( hasValueChangedOp<Route, value_type> )
                {
                    auto prevValue = ref;
                    ref %= std::forward<Value>(value);
                    serializeValue<Member>(static_cast<std::remove_cvref_t<decltype(ref)>>(value)); // Value set to
                    serializeValue<Member>(prevValue); // Value before changing
                    notifyValueChanged(user, Route{keys}, prevValue, ref);
                }
                else
                {
                    serializeValue<Member>(ref % value); // Value set to
                    serializeValue<Member>(ref); // Value before changing
                    ref %= std::forward<Value>(value);
                }
            });
        }

        template <class ... Pathway, class Value, class Keys>
        void xorEq(Value && value, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Set)); // "Set like operation"
            serializePathway<Pathway...>(keys);

            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                
                using value_type = std::remove_cvref_t<decltype(ref)>;
                if constexpr ( hasValueChangedOp<Route, value_type> )
                {
                    auto prevValue = ref;
                    ref ^= std::forward<Value>(value);
                    serializeValue<Member>(static_cast<std::remove_cvref_t<decltype(ref)>>(value)); // Value set to
                    serializeValue<Member>(prevValue); // Value before changing
                    notifyValueChanged(user, Route{keys}, prevValue, ref);
                }
                else
                {
                    serializeValue<Member>(ref ^ value); // Value set to
                    serializeValue<Member>(ref); // Value before changing
                    ref ^= std::forward<Value>(value);
                }
            });
        }

        template <class ... Pathway, class Value, class Keys>
        void andEq(Value && value, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Set)); // "Set like operation"
            serializePathway<Pathway...>(keys);

            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                
                using value_type = std::remove_cvref_t<decltype(ref)>;
                if constexpr ( hasValueChangedOp<Route, value_type> )
                {
                    auto prevValue = ref;
                    ref &= std::forward<Value>(value);
                    serializeValue<Member>(static_cast<std::remove_cvref_t<decltype(ref)>>(value)); // Value set to
                    serializeValue<Member>(prevValue); // Value before changing
                    notifyValueChanged(user, Route{keys}, prevValue, ref);
                }
                else
                {
                    serializeValue<Member>(ref & value); // Value set to
                    serializeValue<Member>(ref); // Value before changing
                    ref &= std::forward<Value>(value);
                }
            });
        }

        template <class ... Pathway, class Value, class Keys>
        void orEq(Value && value, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Set)); // "Set like operation"
            serializePathway<Pathway...>(keys);

            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                
                using value_type = std::remove_cvref_t<decltype(ref)>;
                if constexpr ( hasValueChangedOp<Route, value_type> )
                {
                    auto prevValue = ref;
                    ref |= std::forward<Value>(value);
                    serializeValue<Member>(static_cast<std::remove_cvref_t<decltype(ref)>>(value)); // Value set to
                    serializeValue<Member>(prevValue); // Value before changing
                    notifyValueChanged(user, Route{keys}, prevValue, ref);
                }
                else
                {
                    serializeValue<Member>(ref | value); // Value set to
                    serializeValue<Member>(ref); // Value before changing
                    ref |= std::forward<Value>(value);
                }
            });
        }

        template <class ... Pathway, class Value, class Keys>
        void append(Value && value, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Append));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                ref.push_back(std::forward<Value>(value));
                serializeValue<Member>(ref.back());
                if constexpr ( hasElementAddedOp<Route> )
                    notifyElementAdded(user, Route{keys}, ref.size()-1);
            });
        }

        template <class ... Pathway, class Values, class Keys>
        void appendN(Values && values, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::AppendN));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serializeIndex<Member>(std::size(values));
                for ( auto & value : values )
                {
                    ref.push_back(value);
                    serializeValue<Member>(ref.back());
                    if constexpr ( hasElementAddedOp<Route> )
                        notifyElementAdded(user, Route{keys}, ref.size()-1);
                }
            });
        }

        template <class ... Pathway, class InsertionIndex, class Value, class Keys>
        void insert(InsertionIndex insertionIndex, Value && value, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Insert));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serializeIndex<Member>(insertionIndex);
                serializeValue<Member>(value);
                ref.insert(std::next(ref.begin(), static_cast<std::ptrdiff_t>(insertionIndex)), std::forward<Value>(value));
                if constexpr ( hasElementMovedOp<Route> )
                {
                    for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(std::size(ref))-1; i>static_cast<std::ptrdiff_t>(insertionIndex); --i )
                        notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(i-1), static_cast<std::size_t>(i));
                }
                if constexpr ( hasElementAddedOp<Route> )
                    notifyElementAdded(user, Route{keys}, static_cast<std::size_t>(insertionIndex));

                if constexpr ( hasSelections<Pathway...>() )
                {
                    auto & sel = getSelections<Pathway...>();
                    for ( auto & index : sel )
                    {
                        if ( static_cast<InsertionIndex>(index) >= insertionIndex )
                            ++index;
                    }
                    if constexpr ( hasSelectionsChangedOp<Route> )
                        notifySelectionsChanged(user, Route{keys});
                }
            });
        }

        template <class ... Pathway, class InsertionIndex, class Values, class Keys>
        void insertN(InsertionIndex insertionIndex, Values && values, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::InsertN));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serializeIndex<Member>(std::size(values));
                serializeIndex<Member>(insertionIndex);
                for ( auto & value : values )
                    serializeValue<Member>(value);
                
                ref.insert(std::next(ref.begin(), static_cast<std::ptrdiff_t>(insertionIndex)) , std::begin(values), std::end(values));
                if constexpr ( hasElementMovedOp<Route> )
                {
                    std::size_t countInserted = std::size(values);
                    std::size_t prevSize = std::size(ref) - countInserted;
                    for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(prevSize)-1; i>=static_cast<std::ptrdiff_t>(insertionIndex); --i )
                        notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(i), static_cast<std::size_t>(i)+countInserted);
                }
                if constexpr ( hasElementAddedOp<Route> )
                {
                    auto limit = insertionIndex + static_cast<InsertionIndex>(std::size(values));
                    for ( auto i = insertionIndex; i < limit; ++i )
                        notifyElementAdded(user, Route{keys}, static_cast<std::size_t>(i));
                }

                if constexpr ( hasSelections<Pathway...>() )
                {
                    auto & sel = getSelections<Pathway...>();
                    for ( auto & index : sel )
                    {
                        std::size_t count = values.size();
                        if ( static_cast<InsertionIndex>(index) >= insertionIndex )
                            index += count;
                    }
                    if constexpr ( hasSelectionsChangedOp<Route> )
                        notifySelectionsChanged(user, Route{keys});
                }
            });
        }

        template <class ... Pathway, class RemovalIndex, class Keys>
        void remove(RemovalIndex removalIndex, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Remove));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                
                using index_type = index_type_t<default_index_type, Member>;
                serializeIndex<Member>(removalIndex);
                serializeValue<Member>(ref[static_cast<std::size_t>(removalIndex)]);
                
                ref.erase(std::next(ref.begin(), static_cast<std::ptrdiff_t>(removalIndex)));
                if constexpr ( hasElementRemovedOp<Route> )
                    notifyElementRemoved(user, Route{keys}, static_cast<std::size_t>(removalIndex));

                if constexpr ( hasElementMovedOp<Route> )
                {
                    for ( std::size_t i=static_cast<std::size_t>(removalIndex); i<std::size(ref); ++i )
                        notifyElementMoved(user, Route{keys}, i+1, i);
                }

                auto & sel = getSelections<Pathway...>();
                auto found = sel.end();
                auto indexRemoved = static_cast<index_type>(removalIndex);
                for ( auto it = sel.begin(); it != sel.end(); ++it )
                {
                    if ( *it == indexRemoved )
                        found = it;
                    else if ( *it > indexRemoved )
                        --(*it);
                }

                u8bool::write(events, found != sel.end());
                if ( found != sel.end() )
                {
                    index_type foundAt = static_cast<index_type>(std::distance(sel.begin(), found));
                    serializeIndex<Member>(foundAt);
                    sel.erase(found);
                }
                if constexpr ( hasSelectionsChangedOp<Route> )
                    notifySelectionsChanged(user, Route{keys});
            });
        }

        template <class ... Pathway, class RemovalIndexes, class Keys>
        void removeN(RemovalIndexes && removalIndexes, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::RemoveN));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {

                using index_type = index_type_t<default_index_type, Member>;
                serializeIndex<Member>(std::size(removalIndexes));
                // Sort indexes so largest indexes come first/are removed first
                std::sort(removalIndexes.begin(), removalIndexes.end(), std::greater<RareTs::element_type_t<std::remove_cvref_t<RemovalIndexes>>>{});
                for ( auto removalIndex : removalIndexes )
                    serializeIndex<Member>(removalIndex);
                for ( auto removalIndex : removalIndexes )
                    serializeValue<Member>(ref[static_cast<std::size_t>(removalIndex)]);
                for ( auto removalIndex : removalIndexes )
                    ref.erase(std::next(ref.begin(), static_cast<std::ptrdiff_t>(removalIndex)));

                if constexpr ( hasElementRemovedOp<Route> )
                {
                    for ( auto removalIndex : removalIndexes )
                        notifyElementRemoved(user, Route{keys}, static_cast<std::size_t>(removalIndex));
                }
                if constexpr ( hasElementMovedOp<Route> )
                {
                    if ( !ref.empty() )
                    {
                        std::size_t sizeRemoved = std::size(removalIndexes);
                        std::size_t collectionIndex = static_cast<std::size_t>(removalIndexes[sizeRemoved-1]);
                        std::size_t moveDistance = 1;
                        for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(sizeRemoved)-1; i>0; --i )
                        {
                            for ( std::size_t j=static_cast<std::size_t>(removalIndexes[static_cast<std::size_t>(i)]);
                                j<static_cast<std::size_t>(removalIndexes[static_cast<std::size_t>(i-1)]-1); ++j )
                            {
                                notifyElementMoved(user, Route{keys}, collectionIndex+moveDistance, collectionIndex);
                                ++collectionIndex;
                            }
                            ++moveDistance;
                        }
                        for ( ; collectionIndex < std::size(ref); ++collectionIndex )
                            notifyElementMoved(user, Route{keys}, collectionIndex+moveDistance, collectionIndex);
                    }
                }

                if constexpr ( hasSelections<Pathway...>() )
                {
                    auto & sel = getSelections<Pathway...>();
                    std::vector<bool> removalIndexSelected(std::size(removalIndexes), false);
                    std::vector<index_type> prevSelIndexes {};
                    prevSelIndexes.reserve(std::size(removalIndexes));
                    for ( std::size_t i=0; i<removalIndexes.size(); ++i )
                    {
                        auto indexRemoved = removalIndexes[i];
                        auto found = sel.end();
                        for ( auto it = sel.begin(); it != sel.end(); ++it )
                        {
                            if ( *it == static_cast<index_type>(indexRemoved) )
                                found = it;
                            else if ( *it > static_cast<index_type>(indexRemoved) )
                                --(*it);
                        }
                        if ( found != sel.end() )
                        {
                            removalIndexSelected[i] = true;
                            prevSelIndexes.push_back(static_cast<index_type>(std::distance(sel.begin(), found)));
                            sel.erase(found);
                        }
                    }
                    if constexpr ( hasSelectionsChangedOp<Route> )
                        notifySelectionsChanged(user, Route{keys});

                    writeVecBoolData(events, removalIndexSelected);
                    serializeIndex<Member>(static_cast<index_type>(prevSelIndexes.size()));
                    if ( !prevSelIndexes.empty() )
                    {
                        events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&prevSelIndexes[0]),
                            reinterpret_cast<const std::uint8_t*>(&prevSelIndexes[0])+std::size(prevSelIndexes)*sizeof(prevSelIndexes[0]));
                    }
                }
            });
        }

        template <class ... Pathway, class Keys>
        void removeL(Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::RemoveL));
            serializePathway<Pathway...>(keys);
            auto & sel = getSelections<Pathway...>();
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                auto selections = sel;
                std::sort(selections.begin(), selections.end(), std::greater<RareTs::element_type_t<std::remove_cvref_t<decltype(selections)>>>{});
                serializeIndex<Member>(std::size(selections));
                for ( auto i : selections )
                    serializeIndex<Member>(i);
                for ( auto i : selections )
                {
                    if constexpr ( hasElementRemovedOp<Route> )
                    {
                        auto value = ref[i];
                        serializeValue<Member>(value);
                        ref.erase(std::next(ref.begin(), static_cast<std::ptrdiff_t>(i)));
                        notifyElementRemoved(user, Route{keys}, static_cast<std::size_t>(i));
                    }
                    else
                    {
                        serializeValue<Member>(ref[i]);
                        ref.erase(std::next(ref.begin(), static_cast<std::ptrdiff_t>(i)));
                    }
                }
                if constexpr ( hasElementMovedOp<Route> )
                {
                    if ( !ref.empty() )
                    {
                        std::size_t sizeRemoved = std::size(selections);
                        std::size_t collectionIndex = static_cast<std::size_t>(selections[sizeRemoved-1]);
                        std::size_t moveDistance = 1;
                        for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(sizeRemoved)-1; i>0; --i )
                        {
                            for ( std::size_t j=selections[static_cast<std::size_t>(i)]; j<selections[static_cast<std::size_t>(i-1)]-1; ++j )
                            {
                                notifyElementMoved(user, Route{keys}, collectionIndex + moveDistance, collectionIndex);
                                ++collectionIndex;
                            }
                            ++moveDistance;
                        }
                        for ( ; collectionIndex < std::size(ref); ++collectionIndex )
                            notifyElementMoved(user, Route{keys}, collectionIndex+moveDistance, collectionIndex);
                    }
                }
                writeSelections(events, sel);
                clearSel(sel);
                if constexpr ( hasSelectionsChangedOp<Route> )
                    notifySelectionsChanged(user, Route{keys});
            });
        }

        template <class ... Pathway, class Keys>
        void sort(Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Sort));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                using index_type = index_type_t<default_index_type, Member>;
                auto sourceIndexes = trackedSort<false, index_type>(ref);
                index_type serializedSize = static_cast<index_type>(sourceIndexes.size());
                serializeIndex<Member>(serializedSize);
                for ( auto index : sourceIndexes )
                {
                    index_type sourceIndex = static_cast<index_type>(index);
                    serializeIndex<Member>(sourceIndex);
                }
                auto count = std::size(sourceIndexes);
                auto & sel = getSelections<Pathway...>();
                auto newSel = std::remove_cvref_t<decltype(sel)>{};
                for ( std::size_t i=0; i<count; ++i )
                {
                    if ( std::find(sel.begin(), sel.end(), sourceIndexes[i]) != sel.end() )
                        RareTs::append(newSel, static_cast<index_type>(i));
                }
                std::swap(sel, newSel);
                if constexpr ( hasElementMovedOp<Route> )
                {
                    for ( std::size_t i=0; i<count; ++i )
                    {
                        if ( i != sourceIndexes[i] )
                            notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(sourceIndexes[i]), static_cast<std::size_t>(i));
                    }
                }
                if constexpr ( hasSelectionsChangedOp<Route> )
                    notifySelectionsChanged(user, Route{keys});
            });
        }

        template <class ... Pathway, class Keys>
        void sortDesc(Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::SortDesc));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                using index_type = index_type_t<default_index_type, Member>;
                auto sourceIndexes = trackedSort<true, index_type>(ref);
                index_type serializedSize = static_cast<index_type>(sourceIndexes.size());
                serializeIndex<Member>(serializedSize);
                for ( auto index : sourceIndexes )
                {
                    index_type sourceIndex = static_cast<index_type>(index);
                    serializeIndex<Member>(sourceIndex);
                }
                auto count = std::size(sourceIndexes);
                auto & sel = getSelections<Pathway...>();
                auto newSel = std::remove_cvref_t<decltype(sel)>{};
                for ( std::size_t i=0; i<count; ++i )
                {
                    if ( std::find(sel.begin(), sel.end(), sourceIndexes[i]) != sel.end() )
                        RareTs::append(newSel, i);
                }
                std::swap(sel, newSel);
                if constexpr ( hasElementMovedOp<Route> )
                {
                    auto limit = std::size(sourceIndexes);
                    for ( std::size_t i=0; i<limit; ++i )
                    {
                        if ( i != sourceIndexes[i] )
                            notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(sourceIndexes[i]), static_cast<std::size_t>(i));
                    }
                }
                if constexpr ( hasSelectionsChangedOp<Route> )
                    notifySelectionsChanged(user, Route{keys});
            });
        }

        template <class ... Pathway, class MovedIndex, class Keys>
        void swap(MovedIndex firstIndex, MovedIndex secondIndex, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Swap));
            serializePathway<Pathway...>(keys);

            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serializeIndex<Member>(firstIndex);
                serializeIndex<Member>(secondIndex);
                if ( firstIndex != secondIndex && static_cast<std::size_t>(firstIndex) < std::size(ref) && static_cast<std::size_t>(secondIndex) < std::size(ref) )
                {
                    std::swap(ref[static_cast<std::size_t>(firstIndex)], ref[static_cast<std::size_t>(secondIndex)]);
                    mirrorSwapToSelection(getSelections<Pathway...>(), static_cast<std::size_t>(firstIndex), static_cast<std::size_t>(secondIndex));
                    if constexpr ( hasElementMovedOp<Route> )
                    {
                        notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(firstIndex), static_cast<std::size_t>(secondIndex));
                        notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(secondIndex), static_cast<std::size_t>(firstIndex));
                    }
                    if constexpr ( hasSelections<Pathway...>() && hasSelectionsChangedOp<Route> )
                        notifySelectionsChanged(user, Route{keys});
                }
            });
        }

        template <class ... Pathway, class MovedIndex, class Keys>
        void moveUp(MovedIndex movedIndex, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveUp));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serializeIndex<Member>(movedIndex);
                if ( movedIndex > 0 && static_cast<std::size_t>(movedIndex) < std::size(ref) )
                {
                    std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex-1)]);
                    mirrorSwapToSelection(getSelections<Pathway...>(), static_cast<std::size_t>(movedIndex), static_cast<std::size_t>(movedIndex)-1);
                    if constexpr ( hasElementMovedOp<Route> )
                    {
                        notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(movedIndex), static_cast<std::size_t>(movedIndex)-1);
                        notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(movedIndex)-1, static_cast<std::size_t>(movedIndex));
                    }
                    if constexpr ( hasSelections<Pathway...>() && hasSelectionsChangedOp<Route> )
                        notifySelectionsChanged(user, Route{keys});
                }
            });
        }

        template <class ... Pathway, class MovedIndexes, class Keys>
        void moveUpN(MovedIndexes && movedIndexes, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveUpN));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                std::sort(movedIndexes.begin(), movedIndexes.end());

                serializeIndex<Member>(std::size(movedIndexes));
                for ( auto index : movedIndexes )
                    serializeIndex<Member>(index);

                std::size_t nextAvailable = 0;
                for ( auto movedIndex : movedIndexes )
                {
                    if ( movedIndex > nextAvailable && static_cast<std::size_t>(movedIndex) < std::size(ref) )
                    {
                        std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex-1)]);
                        mirrorSwapToSelection(getSelections<Pathway...>(), movedIndex, movedIndex-1);
                        nextAvailable = movedIndex;
                    }
                    else if ( movedIndex == nextAvailable )
                        ++nextAvailable;
                }

                if constexpr ( hasElementMovedOp<Route> )
                {
                    nextAvailable = 0;
                    std::size_t blockSize = 1;
                    for ( std::size_t i=0; i<std::size(movedIndexes); ++i )
                    {
                        if ( movedIndexes[i] > nextAvailable && static_cast<std::size_t>(movedIndexes[i]) < std::size(ref) )
                        {
                            if ( i+1 < std::size(movedIndexes) && movedIndexes[i]+1 == movedIndexes[i+1] )
                                ++blockSize;
                            else
                            {
                                notifyElementMoved(user, Route{keys}, movedIndexes[i]-blockSize, movedIndexes[i]);
                                blockSize = 1;
                            }
                            notifyElementMoved(user, Route{keys}, movedIndexes[i], movedIndexes[i]-1);
                        }
                        else if ( movedIndexes[i] == nextAvailable )
                            ++nextAvailable;
                    }
                }
                if constexpr ( hasSelectionsChangedOp<Route> )
                    notifySelectionsChanged(user, Route{keys});
            });
        }

        template <class ... Pathway, class Keys>
        void moveUpL(Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveUpL));
            serializePathway<Pathway...>(keys); 
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                auto & sel = getSelections<Pathway...>();
                writeSelections(events, sel);
                auto movedIndexes = sel; // Copy so it can be sorted without changing selections
                std::sort(movedIndexes.begin(), movedIndexes.end());
                
                std::size_t nextAvailable = 0;
                for ( auto movedIndex : movedIndexes )
                {
                    if ( movedIndex > nextAvailable && static_cast<std::size_t>(movedIndex) < std::size(ref) )
                    {
                        std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex-1)]);
                        mirrorSwapToSelection(getSelections<Pathway...>(), movedIndex, movedIndex-1);
                        nextAvailable = movedIndex;
                    }
                    else if ( movedIndex == nextAvailable )
                        ++nextAvailable;
                }

                if constexpr ( hasElementMovedOp<Route> )
                {
                    nextAvailable = 0;
                    std::size_t blockSize = 1;
                    for ( std::size_t i=0; i<std::size(movedIndexes); ++i )
                    {
                        if ( movedIndexes[i] > nextAvailable && static_cast<std::size_t>(movedIndexes[i]) < std::size(ref) )
                        {
                            if ( i+1 < std::size(movedIndexes) && movedIndexes[i]+1 == movedIndexes[i+1] )
                                ++blockSize;
                            else
                            {
                                notifyElementMoved(user, Route{keys}, movedIndexes[i]-blockSize, movedIndexes[i]);
                                blockSize = 1;
                            }
                            notifyElementMoved(user, Route{keys}, movedIndexes[i], movedIndexes[i]-1);
                        }
                        else if ( movedIndexes[i] == nextAvailable )
                            ++nextAvailable;
                    }
                }
                if constexpr ( hasSelectionsChangedOp<Route> )
                    notifySelectionsChanged(user, Route{keys});
            });
        }

        template <class ... Pathway, class MovedIndex, class Keys>
        void moveTop(MovedIndex movedIndex, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveTop));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serializeIndex<Member>(movedIndex);
                auto size = std::size(ref);
                if ( movedIndex > 0 && static_cast<std::size_t>(movedIndex) < size )
                {
                    auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(movedIndex));
                    std::rotate(ref.begin(), it, it+1); // [0, movedIndex) are moved forward 1... [movedIndex] is moved to 0
                    mirrorRotationToSelection(getSelections<Pathway...>(), 0, static_cast<std::size_t>(movedIndex), static_cast<std::size_t>(movedIndex)+1);
                    if constexpr ( hasElementMovedOp<Route> )
                    {
                        notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(movedIndex), 0);
                        for ( std::size_t i=0; i<static_cast<std::size_t>(movedIndex); ++i )
                            notifyElementMoved(user, Route{keys}, i, i+1);
                    }
                    if constexpr ( hasSelections<Pathway...>() && hasSelectionsChangedOp<Route> )
                        notifySelectionsChanged(user, Route{keys});
                }
            });
        }

        template <class ... Pathway, class MovedIndexes, class Keys>
        void moveTopN(MovedIndexes && movedIndexes, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveTopN));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                std::sort(movedIndexes.begin(), movedIndexes.end()); // lowest to highest
                serializeIndex<Member>(std::size(movedIndexes));
                for ( auto i : movedIndexes )
                    serializeIndex<Member>(i);

                writeSelections(events, getSelections<Pathway...>());
                
                std::size_t size = std::size(ref);
                std::vector<std::size_t> trackedIndexes {};
                if constexpr ( hasElementMovedOp<Route> )
                {
                    trackedIndexes.assign(size, 0);
                    std::iota(trackedIndexes.begin(), trackedIndexes.end(), 0);
                }

                std::ptrdiff_t insertionIndex = 0;
                for ( auto i : movedIndexes )
                {
                    if ( static_cast<std::ptrdiff_t>(i) > insertionIndex && static_cast<std::size_t>(i) < size )
                    {
                        auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                        std::rotate(std::next(ref.begin(), insertionIndex), it, it+1);
                        mirrorRotationToSelection(getSelections<Pathway...>(), static_cast<std::size_t>(insertionIndex), static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1);
                        if constexpr ( hasElementMovedOp<Route> )
                            mirrorRotationToIndexes(trackedIndexes, insertionIndex, i, i+1);
                    }
                    ++insertionIndex;
                }

                if constexpr ( hasElementMovedOp<Route> )
                {
                    for ( std::size_t i=0; i<size; ++i )
                    {
                        if ( i != trackedIndexes[i] )
                            notifyElementMoved(user, Route{keys}, trackedIndexes[i], i);
                    }
                }
                if constexpr ( hasSelectionsChangedOp<Route> )
                    notifySelectionsChanged(user, Route{keys});
            });
        }

        template <class ... Pathway, class Keys>
        void moveTopL(Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveTopL));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                auto & sel = getSelections<Pathway...>();
                writeSelections(events, sel);
                auto movedIndexes = sel; // Copy so it can be sorted without changing selections
                std::sort(movedIndexes.begin(), movedIndexes.end());

                std::size_t size = std::size(ref);
                std::vector<std::size_t> trackedIndexes {};
                if constexpr ( hasElementMovedOp<Route> )
                {
                    trackedIndexes.assign(size, 0);
                    std::iota(trackedIndexes.begin(), trackedIndexes.end(), 0);
                }

                std::ptrdiff_t insertionIndex = 0;
                for ( auto i : movedIndexes )
                {
                    if ( static_cast<std::ptrdiff_t>(i) > insertionIndex && static_cast<std::size_t>(i) < size )
                    {
                        auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                        std::rotate(std::next(ref.begin(), insertionIndex), it, it+1);
                        mirrorRotationToSelection(getSelections<Pathway...>(), static_cast<std::size_t>(insertionIndex), static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1);
                        if constexpr ( hasElementMovedOp<Route> )
                            mirrorRotationToIndexes(trackedIndexes, insertionIndex, i, i+1);
                    }
                    ++insertionIndex;
                }

                if constexpr ( hasElementMovedOp<Route> )
                {
                    for ( std::size_t i=0; i<size; ++i )
                    {
                        if ( i != trackedIndexes[i] )
                            notifyElementMoved(user, Route{keys}, trackedIndexes[i], i);
                    }
                }
                if constexpr ( hasSelectionsChangedOp<Route> )
                    notifySelectionsChanged(user, Route{keys});
            });
        }

        template <class ... Pathway, class MovedIndex, class Keys>
        void moveDown(MovedIndex movedIndex, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveDown));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serializeIndex<Member>(movedIndex);
                if ( static_cast<std::size_t>(movedIndex)+1 < std::size(ref) )
                {
                    std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex)+1]);
                    mirrorSwapToSelection(getSelections<Pathway...>(), static_cast<std::size_t>(movedIndex), static_cast<std::size_t>(movedIndex)+1);
                    if constexpr ( hasElementMovedOp<Route> )
                    {
                        notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(movedIndex), static_cast<std::size_t>(movedIndex)+1);
                        notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(movedIndex)+1, static_cast<std::size_t>(movedIndex));
                    }
                    if constexpr ( hasSelections<Pathway...>() && hasSelectionsChangedOp<Route> )
                        notifySelectionsChanged(user, Route{keys});
                }
            });
        }

        template <class ... Pathway, class MovedIndexes, class Keys>
        void moveDownN(MovedIndexes && movedIndexes, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveDownN));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                std::sort(movedIndexes.begin(), movedIndexes.end(), std::greater<RareTs::element_type_t<std::remove_cvref_t<MovedIndexes>>>{});
                serializeIndex<Member>(std::size(movedIndexes));
                for ( auto i : movedIndexes )
                    serializeIndex<Member>(i);

                auto size = std::size(ref);
                if ( size > 0 )
                {
                    auto limit = size;
                    for ( auto movedIndex : movedIndexes )
                    {
                        if ( static_cast<std::size_t>(movedIndex)+1 < limit )
                        {
                            std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex)+1]);
                            mirrorSwapToSelection(getSelections<Pathway...>(), movedIndex, movedIndex+1);
                            limit = movedIndex+1;
                        }
                        else if ( limit > 0 )
                            --limit;
                    }

                    if constexpr ( hasElementMovedOp<Route> )
                    {
                        limit = size;
                        std::size_t count = std::size(movedIndexes);
                        std::size_t blockSize = 1;
                        for ( std::size_t i=0; i<count; ++i )
                        {
                            if ( static_cast<std::size_t>(movedIndexes[i])+1 < limit )
                            {
                                if ( i+1 < count && movedIndexes[i]-1 == movedIndexes[i+1] )
                                    ++blockSize;
                                else
                                {
                                    notifyElementMoved(user, Route{keys}, movedIndexes[i]+blockSize, movedIndexes[i]);
                                    blockSize = 1;
                                }
                                notifyElementMoved(user, Route{keys}, movedIndexes[i], movedIndexes[i]+1);
                                limit = movedIndexes[i]+1;
                            }
                            else if ( limit > 0 )
                                --limit;
                        }
                    }
                    if constexpr ( hasSelectionsChangedOp<Route> )
                        notifySelectionsChanged(user, Route{keys});
                }
            });
        }

        template <class ... Pathway, class Keys>
        void moveDownL(Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveDownL));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                using index_type = index_type_t<default_index_type, Member>;
                auto & sel = getSelections<Pathway...>();
                writeSelections(events, sel);
                auto movedIndexes = sel; // Copy so it can be sorted without changing selections
                std::sort(movedIndexes.begin(), movedIndexes.end(), std::greater<index_type>{});

                auto size = std::size(ref);
                if ( size > 0 )
                {
                    auto limit = size;
                    for ( auto movedIndex : movedIndexes )
                    {
                        if ( static_cast<std::size_t>(movedIndex)+1 < limit )
                        {
                            std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex)+1]);
                            mirrorSwapToSelection(getSelections<Pathway...>(), movedIndex, movedIndex+1);
                        }
                        else if ( limit > 0 )
                            --limit;
                    }

                    if constexpr ( hasElementMovedOp<Route> )
                    {
                        limit = size;
                        std::size_t count = std::size(movedIndexes);
                        std::size_t blockSize = 1;
                        for ( std::size_t i=0; i<count; ++i )
                        {
                            if ( static_cast<std::size_t>(movedIndexes[i])+1 < limit )
                            {
                                if ( i+1 < count && movedIndexes[i]-1 == movedIndexes[i+1] )
                                    ++blockSize;
                                else
                                {
                                    notifyElementMoved(user, Route{keys}, movedIndexes[i]+blockSize, movedIndexes[i]);
                                    blockSize = 1;
                                }
                                notifyElementMoved(user, Route{keys}, movedIndexes[i], movedIndexes[i]+1);
                                limit = movedIndexes[i]+1;
                            }
                            else if ( limit > 0 )
                                --limit;
                        }
                    }
                    if constexpr ( hasSelectionsChangedOp<Route> )
                        notifySelectionsChanged(user, Route{keys});
                }

            });
        }

        template <class ... Pathway, class MovedIndex, class Keys>
        void moveBottom(MovedIndex movedIndex, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveBottom));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serializeIndex<Member>(movedIndex);
                if ( movedIndex >= 0 && static_cast<std::size_t>(movedIndex)+1 < std::size(ref) )
                {
                    auto it = std::next(ref.begin(), movedIndex);
                    std::rotate(it, it+1, ref.end());
                    mirrorRotationToSelection(getSelections<Pathway...>(), static_cast<std::size_t>(movedIndex), static_cast<std::size_t>(movedIndex)+1, std::size(ref));
                    if constexpr ( hasElementMovedOp<Route> )
                    {
                        notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(movedIndex), std::size(ref)-1);
                        for ( std::size_t i=static_cast<std::size_t>(movedIndex)+1; i<std::size(ref); ++i )
                            notifyElementMoved(user, Route{keys}, i, i-1);
                    }
                    if constexpr ( hasSelections<Pathway...>() && hasSelectionsChangedOp<Route> )
                        notifySelectionsChanged(user, Route{keys});
                }
            });
        }

        template <class ... Pathway, class MovedIndexes, class Keys>
        void moveBottomN(MovedIndexes && movedIndexes, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveBottomN));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                std::sort(movedIndexes.begin(), movedIndexes.end(), std::greater<RareTs::element_type_t<std::remove_cvref_t<MovedIndexes>>>{});
                serializeIndex<Member>(std::size(movedIndexes));
                for ( auto i : movedIndexes )
                    serializeIndex<Member>(i);
                
                writeSelections(events, getSelections<Pathway...>());

                std::size_t size = std::size(ref);
                std::vector<std::size_t> trackedIndexes {};
                if constexpr ( hasElementMovedOp<Route> )
                {
                    trackedIndexes.assign(size, 0);
                    std::iota(trackedIndexes.begin(), trackedIndexes.end(), 0);
                }

                std::ptrdiff_t insertionIndex = static_cast<std::ptrdiff_t>(size);
                for ( auto i : movedIndexes )
                {
                    if ( i >= 0 && static_cast<std::ptrdiff_t>(i)+1 < insertionIndex )
                    {
                        auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                        std::rotate(it, it+1, std::next(ref.begin(), insertionIndex));
                        mirrorRotationToSelection(getSelections<Pathway...>(), static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertionIndex));
                        if constexpr ( hasElementMovedOp<Route> )
                            mirrorRotationToIndexes(trackedIndexes, i, i+1, insertionIndex);
                    }
                    --insertionIndex;
                }

                if constexpr ( hasElementMovedOp<Route> )
                {
                    for ( std::size_t i=0; i<size; ++i )
                    {
                        if ( i != trackedIndexes[i] )
                            notifyElementMoved(user, Route{keys}, trackedIndexes[i], i);
                    }
                }
                if constexpr ( hasSelectionsChangedOp<Route> )
                    notifySelectionsChanged(user, Route{keys});
            });
        }

        template <class ... Pathway, class Keys>
        void moveBottomL(Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveBottomL));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                using index_type = index_type_t<default_index_type, Member>;
                auto & sel = getSelections<Pathway...>();
                writeSelections(events, sel);
                auto movedIndexes = sel; // Copy so it can be sorted without changing selections
                std::sort(movedIndexes.begin(), movedIndexes.end(), std::greater<index_type>{});

                std::size_t size = std::size(ref);
                std::vector<std::size_t> trackedIndexes {};
                if constexpr ( hasElementMovedOp<Route> )
                {
                    trackedIndexes.assign(size, 0);
                    std::iota(trackedIndexes.begin(), trackedIndexes.end(), 0);
                }

                std::ptrdiff_t insertionIndex = static_cast<std::ptrdiff_t>(size);
                for ( auto i : movedIndexes )
                {
                    if ( i >= 0 && static_cast<std::ptrdiff_t>(i)+1 < insertionIndex )
                    {
                        auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                        std::rotate(it, it+1, std::next(ref.begin(), insertionIndex));
                        mirrorRotationToSelection(getSelections<Pathway...>(), static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertionIndex));
                        if constexpr ( hasElementMovedOp<Route> )
                            mirrorRotationToIndexes(trackedIndexes, i, i+1, insertionIndex);
                    }
                    --insertionIndex;
                }

                if constexpr ( hasElementMovedOp<Route> )
                {
                    for ( std::size_t i=0; i<size; ++i )
                    {
                        if ( i != trackedIndexes[i] )
                            notifyElementMoved(user, Route{keys}, trackedIndexes[i], i);
                    }
                }
                if constexpr ( hasSelectionsChangedOp<Route> )
                    notifySelectionsChanged(user, Route{keys});
            });
        }

        template <class ... Pathway, class MovedIndex, class IndexMovedTo, class Keys>
        void moveTo(MovedIndex movedIndex, IndexMovedTo indexMovedTo, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveTo));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serializeIndex<Member>(indexMovedTo);
                serializeIndex<Member>(movedIndex);

                auto it = std::next(ref.begin(), movedIndex);
                if ( static_cast<std::ptrdiff_t>(indexMovedTo) < static_cast<std::ptrdiff_t>(movedIndex) && indexMovedTo >= 0 && static_cast<std::size_t>(movedIndex) < std::size(ref) )
                {
                    std::rotate(std::next(ref.begin(), indexMovedTo), it, it+1);
                    mirrorRotationToSelection(getSelections<Pathway...>(), static_cast<std::size_t>(indexMovedTo), static_cast<std::size_t>(movedIndex), static_cast<std::size_t>(movedIndex)+1);
                    if constexpr ( hasElementMovedOp<Route> )
                    {
                        notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(movedIndex), static_cast<std::size_t>(indexMovedTo));
                        for ( std::size_t i=static_cast<std::size_t>(indexMovedTo); i<static_cast<std::size_t>(movedIndex); ++i )
                            notifyElementMoved(user, Route{keys}, i, i+1);
                    }
                    if constexpr ( hasSelections<Pathway...>() && hasSelectionsChangedOp<Route> )
                        notifySelectionsChanged(user, Route{keys});
                }
                else if ( static_cast<std::ptrdiff_t>(indexMovedTo) > static_cast<std::ptrdiff_t>(movedIndex) && movedIndex >= 0 && static_cast<std::size_t>(indexMovedTo) < std::size(ref) )
                {
                    std::rotate(it, it+1, std::next(ref.begin(), indexMovedTo)+1);
                    mirrorRotationToSelection(getSelections<Pathway...>(), static_cast<std::size_t>(movedIndex), static_cast<std::size_t>(movedIndex)+1, static_cast<std::size_t>(indexMovedTo)+1);
                    if constexpr ( hasElementMovedOp<Route> )
                    {
                        notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(movedIndex), static_cast<std::size_t>(indexMovedTo));
                        for ( std::size_t i=static_cast<std::size_t>(movedIndex)+1; i<static_cast<std::size_t>(indexMovedTo)+1; ++i )
                            notifyElementMoved(user, Route{keys}, i, i-1);
                    }
                    if constexpr ( hasSelections<Pathway...>() && hasSelectionsChangedOp<Route> )
                        notifySelectionsChanged(user, Route{keys});
                }
            });
        }

        template <class ... Pathway, class MovedIndexes, class IndexMovedTo, class Keys>
        void moveToN(MovedIndexes && movedIndexes, IndexMovedTo indexMovedTo, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveToN));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                std::sort(movedIndexes.begin(), movedIndexes.end());
                serializeIndex<Member>(indexMovedTo);
                serializeIndex<Member>(std::size(movedIndexes));
                for ( auto i : movedIndexes )
                    serializeIndex<Member>(i);

                auto & sel = getSelections<Pathway...>();
                writeSelections(events, sel);

                auto size = std::size(ref);
                if ( indexMovedTo < 0 )
                    indexMovedTo = 0;
                else if ( static_cast<std::size_t>(indexMovedTo) >= size && size > 0 )
                    indexMovedTo = static_cast<IndexMovedTo>(size-1);

                if ( !std::empty(movedIndexes) && indexMovedTo >= 0 )
                {
                    std::size_t countValidIndexes = 0;
                    for ( auto movedIndex : movedIndexes )
                    {
                        if ( movedIndex >= 0 && static_cast<std::size_t>(movedIndex) < std::size(ref) )
                            ++countValidIndexes;
                    }
                    if ( countValidIndexes == 0 )
                        return;

                    std::vector<std::size_t> trackedIndexes {};
                    if constexpr ( hasElementMovedOp<Route> )
                    {
                        trackedIndexes.assign(size, 0);
                        std::iota(trackedIndexes.begin(), trackedIndexes.end(), 0);
                    }

                    if ( static_cast<std::size_t>(indexMovedTo) + countValidIndexes >= size ) // Operation collapses to moveToEnd
                    {
                        std::ptrdiff_t insertionIndex = static_cast<std::ptrdiff_t>(size);
                        for ( auto mit = movedIndexes.rbegin(); mit != movedIndexes.rend(); ++mit ) // movedIndexes sorted least to greatest, so reverse read
                        {
                            auto i = *mit;
                            if ( static_cast<std::ptrdiff_t>(i) >= 0 && static_cast<std::ptrdiff_t>(i)+1 < insertionIndex )
                            {
                                auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                std::rotate(it, it+1, std::next(ref.begin(), insertionIndex));
                                mirrorRotationToSelection(sel, static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertionIndex));
                                if constexpr ( hasElementMovedOp<Route> )
                                    mirrorRotationToIndexes(trackedIndexes, static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertionIndex));
                            }
                            --insertionIndex;
                        }
                    }
                    else // Non-collapsed moveTo operation
                    {
                        std::size_t minValidIndex = 0;
                        std::size_t maxValidIndex = 0;
                        for ( std::size_t i=0; i<std::size(movedIndexes); ++i )
                        {
                            if ( movedIndexes[i] >= 0 )
                            {
                                minValidIndex = static_cast<std::size_t>(movedIndexes[i]);
                                break;
                            }
                        }
                        for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(std::size(movedIndexes))-1; i>=0; --i )
                        {
                            if ( static_cast<std::ptrdiff_t>(movedIndexes[static_cast<std::size_t>(i)]) < static_cast<std::ptrdiff_t>(size) )
                            {
                                maxValidIndex = static_cast<std::size_t>(movedIndexes[static_cast<std::size_t>(i)]);
                                break;
                            }
                        }

                        std::size_t leftChunkSize = minValidIndex < static_cast<std::size_t>(indexMovedTo) ? 1 : 0;
                        std::size_t rightChunkSize = maxValidIndex >= static_cast<std::size_t>(indexMovedTo) ? 1 : 0;
                        std::size_t leftChunkFirst = minValidIndex;
                        std::size_t rightChunkFirst = maxValidIndex;

                        for ( std::size_t i=0; i+1<std::size(movedIndexes) && static_cast<std::size_t>(movedIndexes[i+1]) < static_cast<std::size_t>(indexMovedTo); ++i )
                        {
                            // rotate left chunk together with element[movedIndexes[i+1]]
                            auto leftStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst));
                            auto leftEnd = std::next(leftStart, static_cast<std::ptrdiff_t>(leftChunkSize));
                            std::rotate(leftStart, leftEnd, std::next(ref.begin(), static_cast<std::ptrdiff_t>(movedIndexes[i+1])));
                            mirrorRotationToSelection(sel, leftChunkFirst, leftChunkFirst+leftChunkSize, movedIndexes[i+1]);
                            if constexpr ( hasElementMovedOp<Route> )
                                mirrorRotationToIndexes(trackedIndexes, leftChunkFirst, leftChunkFirst+leftChunkSize, movedIndexes[i+1]);
                            leftChunkFirst = static_cast<std::size_t>(movedIndexes[i+1])-leftChunkSize;
                            ++leftChunkSize;
                        }
                        for ( std::size_t i=std::size(movedIndexes)-1; i>0 && static_cast<std::size_t>(movedIndexes[i-1]) >= static_cast<std::size_t>(indexMovedTo); --i )
                        {
                            // rotate right chunk together with element[movedIndexes[i-1]]
                            auto rightStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(rightChunkFirst));
                            auto rightEnd = std::next(rightStart, static_cast<std::ptrdiff_t>(rightChunkSize));
                            std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(movedIndexes[i-1])+1), rightStart, rightEnd);
                            mirrorRotationToSelection(sel, static_cast<std::size_t>(movedIndexes[i-1])+1, rightChunkFirst, rightChunkFirst+rightChunkSize);
                            if constexpr ( hasElementMovedOp<Route> )
                                mirrorRotationToIndexes(trackedIndexes, static_cast<std::size_t>(movedIndexes[i-1])+1, rightChunkFirst, rightChunkFirst+rightChunkSize);
                            rightChunkFirst = static_cast<std::size_t>(movedIndexes[i-1]);
                            ++rightChunkSize;
                        }
                        if ( leftChunkFirst+leftChunkSize != rightChunkFirst && leftChunkSize > 0 && rightChunkSize > 0 ) // Need to combine the chunks then rotate combined to final position
                        {
                            std::size_t leftDistance = leftChunkFirst < static_cast<std::size_t>(indexMovedTo) ? static_cast<std::size_t>(indexMovedTo)-leftChunkFirst : leftChunkFirst-static_cast<std::size_t>(indexMovedTo);
                            std::size_t rightDistance = rightChunkFirst < static_cast<std::size_t>(indexMovedTo)+leftChunkSize ? static_cast<std::size_t>(indexMovedTo)+leftChunkSize-rightChunkFirst : rightChunkFirst-static_cast<std::size_t>(indexMovedTo)-leftChunkSize;
                            if ( rightDistance < leftDistance ) // Right chunk closer to target, move left up to right
                            {
                                auto leftStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst));
                                auto leftEnd = std::next(leftStart, static_cast<std::ptrdiff_t>(leftChunkSize));
                                std::rotate(leftStart, leftEnd, std::next(ref.begin(), static_cast<std::ptrdiff_t>(rightChunkFirst)));
                                mirrorRotationToSelection(sel, leftChunkFirst, leftChunkFirst+leftChunkSize, rightChunkFirst);
                                if constexpr ( hasElementMovedOp<Route> )
                                    mirrorRotationToIndexes(trackedIndexes, leftChunkFirst, leftChunkFirst+leftChunkSize, rightChunkFirst);
                                leftChunkFirst = rightChunkFirst-leftChunkSize;
                            }
                            else // Left chunk closer to target, move right up to left
                            {
                                auto rightStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(rightChunkFirst));
                                auto rightEnd = std::next(rightStart, static_cast<std::ptrdiff_t>(rightChunkSize));
                                std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst+leftChunkSize)), rightStart, rightEnd);
                                mirrorRotationToSelection(sel, leftChunkFirst+leftChunkSize, rightChunkFirst, rightChunkFirst+rightChunkSize);
                                if constexpr ( hasElementMovedOp<Route> )
                                    mirrorRotationToIndexes(trackedIndexes, leftChunkFirst+leftChunkSize, rightChunkFirst, rightChunkFirst+rightChunkSize);
                                rightChunkFirst = leftChunkFirst+leftChunkSize;
                            }
                        }
                        if ( leftChunkFirst < static_cast<std::size_t>(indexMovedTo) ) // Rotate combined chunk rightwards to final position
                        {
                            auto chunkStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst));
                            auto chunkEnd = std::next(chunkStart, static_cast<std::ptrdiff_t>(countValidIndexes));
                            std::rotate(chunkStart, chunkEnd, std::next(chunkEnd, static_cast<std::ptrdiff_t>(static_cast<std::size_t>(indexMovedTo)-leftChunkFirst)));
                            mirrorRotationToSelection(sel, leftChunkFirst, leftChunkFirst+countValidIndexes, leftChunkFirst+countValidIndexes+static_cast<std::size_t>(indexMovedTo)-leftChunkFirst);
                            if constexpr ( hasElementMovedOp<Route> )
                                mirrorRotationToIndexes(trackedIndexes, leftChunkFirst, leftChunkFirst+countValidIndexes, leftChunkFirst+countValidIndexes+static_cast<std::size_t>(indexMovedTo)-leftChunkFirst);
                        }
                        else if ( leftChunkFirst > static_cast<std::size_t>(indexMovedTo) ) // Rotate combined chunk leftwards to final position
                        {
                            auto chunkStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst));
                            auto chunkEnd = std::next(chunkStart, static_cast<std::ptrdiff_t>(countValidIndexes));
                            std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(indexMovedTo)), chunkStart, chunkEnd);
                            mirrorRotationToSelection(sel, static_cast<std::size_t>(indexMovedTo), leftChunkFirst, leftChunkFirst+countValidIndexes);
                            if constexpr ( hasElementMovedOp<Route> )
                                mirrorRotationToIndexes(trackedIndexes, static_cast<std::size_t>(indexMovedTo), leftChunkFirst, leftChunkFirst+countValidIndexes);
                        }
                    }
                    
                    if constexpr ( hasElementMovedOp<Route> )
                    {
                        for ( std::size_t i=0; i<size; ++i )
                        {
                            if ( i != trackedIndexes[i] )
                                notifyElementMoved(user, Route{keys}, trackedIndexes[i], i);
                        }
                    }
                    if constexpr ( hasSelectionsChangedOp<Route> )
                        notifySelectionsChanged(user, Route{keys});
                }
            });
        }

        template <class ... Pathway, class IndexMovedTo, class Keys>
        void moveToL(IndexMovedTo indexMovedTo, Keys & keys)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveToL));
            serializePathway<Pathway...>(keys);
            
            operateOn<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                auto & sel = getSelections<Pathway...>();
                auto movedIndexes = sel; // Copy so it can be sorted without changing selections
                std::sort(movedIndexes.begin(), movedIndexes.end());
                serializeIndex<Member>(indexMovedTo);
                writeSelections(events, sel);

                auto size = std::size(ref);
                if ( indexMovedTo < 0 )
                    indexMovedTo = 0;
                else if ( static_cast<std::size_t>(indexMovedTo) >= size && size > 0 )
                    indexMovedTo = static_cast<IndexMovedTo>(size-1);

                if ( !std::empty(movedIndexes) && indexMovedTo >= 0 )
                {
                    std::size_t countValidIndexes = 0;
                    for ( auto movedIndex : movedIndexes )
                    {
                        if ( movedIndex >= 0 && static_cast<std::size_t>(movedIndex) < std::size(ref) )
                            ++countValidIndexes;
                    }
                    if ( countValidIndexes == 0 )
                        return;

                    std::vector<std::size_t> trackedIndexes {};
                    if constexpr ( hasElementMovedOp<Route> )
                    {
                        trackedIndexes.assign(size, 0);
                        std::iota(trackedIndexes.begin(), trackedIndexes.end(), 0);
                    }

                    if ( static_cast<std::size_t>(indexMovedTo) + countValidIndexes >= size ) // Operation collapses to moveToEnd
                    {
                        std::ptrdiff_t insertionIndex = static_cast<std::ptrdiff_t>(size);
                        for ( auto mit = movedIndexes.rbegin(); mit != movedIndexes.rend(); ++mit ) // movedIndexes sorted least to greatest, so reverse read
                        {
                            auto i = *mit;
                            if ( static_cast<std::ptrdiff_t>(i) >= 0 && static_cast<std::ptrdiff_t>(i)+1 < insertionIndex )
                            {
                                auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                std::rotate(it, it+1, std::next(ref.begin(), insertionIndex));
                                mirrorRotationToSelection(sel, static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertionIndex));
                                if constexpr ( hasElementMovedOp<Route> )
                                    mirrorRotationToIndexes(trackedIndexes, static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertionIndex));
                            }
                            --insertionIndex;
                        }
                    }
                    else // Non-collapsed moveTo operation
                    {
                        std::size_t minValidIndex = 0;
                        std::size_t maxValidIndex = 0;
                        for ( std::size_t i=0; i<std::size(movedIndexes); ++i )
                        {
                            if ( static_cast<std::ptrdiff_t>(movedIndexes[i]) >= 0 )
                            {
                                minValidIndex = static_cast<std::size_t>(movedIndexes[i]);
                                break;
                            }
                        }
                        for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(std::size(movedIndexes))-1; i>=0; --i )
                        {
                            if ( static_cast<std::ptrdiff_t>(movedIndexes[static_cast<std::size_t>(i)]) < static_cast<std::ptrdiff_t>(size) )
                            {
                                maxValidIndex = static_cast<std::size_t>(movedIndexes[static_cast<std::size_t>(i)]);
                                break;
                            }
                        }

                        std::size_t leftChunkSize = minValidIndex < static_cast<std::size_t>(indexMovedTo) ? 1 : 0;
                        std::size_t rightChunkSize = maxValidIndex >= static_cast<std::size_t>(indexMovedTo) ? 1 : 0;
                        std::size_t leftChunkFirst = minValidIndex;
                        std::size_t rightChunkFirst = maxValidIndex;

                        for ( std::size_t i=0; i+1<std::size(movedIndexes) && static_cast<std::size_t>(movedIndexes[i+1]) < static_cast<std::size_t>(indexMovedTo); ++i )
                        {
                            // rotate left chunk together with element[movedIndexes[i+1]]
                            auto leftStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst));
                            auto leftEnd = std::next(leftStart, static_cast<std::ptrdiff_t>(leftChunkSize));
                            std::rotate(leftStart, leftEnd, std::next(ref.begin(), static_cast<std::ptrdiff_t>(movedIndexes[i+1])));
                            mirrorRotationToSelection(sel, leftChunkFirst, leftChunkFirst+leftChunkSize, movedIndexes[i+1]);
                            if constexpr ( hasElementMovedOp<Route> )
                                mirrorRotationToIndexes(trackedIndexes, leftChunkFirst, leftChunkFirst+leftChunkSize, movedIndexes[i+1]);
                            leftChunkFirst = static_cast<std::size_t>(movedIndexes[i+1])-leftChunkSize;
                            ++leftChunkSize;
                        }
                        for ( std::size_t i=std::size(movedIndexes)-1; i>0 && static_cast<std::size_t>(movedIndexes[i-1]) >= static_cast<std::size_t>(indexMovedTo); --i )
                        {
                            // rotate right chunk together with element[movedIndexes[i-1]]
                            auto rightStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(rightChunkFirst));
                            auto rightEnd = std::next(rightStart, static_cast<std::ptrdiff_t>(rightChunkSize));
                            std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(movedIndexes[i-1])+1), rightStart, rightEnd);
                            mirrorRotationToSelection(sel, movedIndexes[i-1]+1, rightChunkFirst, rightChunkFirst+rightChunkSize);
                            if constexpr ( hasElementMovedOp<Route> )
                                mirrorRotationToIndexes(trackedIndexes, movedIndexes[i-1]+1, rightChunkFirst, rightChunkFirst+rightChunkSize);
                            rightChunkFirst = static_cast<std::size_t>(movedIndexes[i-1]);
                            ++rightChunkSize;
                        }
                        if ( leftChunkFirst+leftChunkSize != rightChunkFirst && leftChunkSize > 0 && rightChunkSize > 0 ) // Need to combine the chunks then rotate combined to final position
                        {
                            std::size_t leftDistance = leftChunkFirst < static_cast<std::size_t>(indexMovedTo) ? static_cast<std::size_t>(indexMovedTo)-leftChunkFirst : leftChunkFirst-static_cast<std::size_t>(indexMovedTo);
                            std::size_t rightDistance = rightChunkFirst < static_cast<std::size_t>(indexMovedTo)+leftChunkSize ? static_cast<std::size_t>(indexMovedTo)+leftChunkSize-rightChunkFirst : rightChunkFirst-static_cast<std::size_t>(indexMovedTo)-leftChunkSize;
                            if ( rightDistance < leftDistance ) // Right chunk closer to target, move left up to right
                            {
                                auto leftStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst));
                                auto leftEnd = std::next(leftStart, static_cast<std::ptrdiff_t>(leftChunkSize));
                                std::rotate(leftStart, leftEnd, std::next(ref.begin(), static_cast<std::ptrdiff_t>(rightChunkFirst)));
                                mirrorRotationToSelection(sel, leftChunkFirst, leftChunkFirst+leftChunkSize, rightChunkFirst);
                                if constexpr ( hasElementMovedOp<Route> )
                                    mirrorRotationToIndexes(trackedIndexes, leftChunkFirst, leftChunkFirst+leftChunkSize, rightChunkFirst);
                                leftChunkFirst = rightChunkFirst-leftChunkSize;
                            }
                            else // Left chunk closer to target, move right up to left
                            {
                                auto rightStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(rightChunkFirst));
                                auto rightEnd = std::next(rightStart, static_cast<std::ptrdiff_t>(rightChunkSize));
                                std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst+leftChunkSize)), rightStart, rightEnd);
                                mirrorRotationToSelection(sel, leftChunkFirst+leftChunkSize, rightChunkFirst, rightChunkFirst+rightChunkSize);
                                if constexpr ( hasElementMovedOp<Route> )
                                    mirrorRotationToIndexes(trackedIndexes, leftChunkFirst+leftChunkSize, rightChunkFirst, rightChunkFirst+rightChunkSize);
                                rightChunkFirst = leftChunkFirst+leftChunkSize;
                            }
                        }
                        if ( leftChunkFirst < static_cast<std::size_t>(indexMovedTo) ) // Rotate combined chunk rightwards to final position
                        {
                            auto chunkStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst));
                            auto chunkEnd = std::next(chunkStart, static_cast<std::ptrdiff_t>(countValidIndexes));
                            std::rotate(chunkStart, chunkEnd, std::next(chunkEnd, static_cast<std::ptrdiff_t>(static_cast<std::size_t>(indexMovedTo)-leftChunkFirst)));
                            mirrorRotationToSelection(sel, leftChunkFirst, leftChunkFirst+countValidIndexes, leftChunkFirst+countValidIndexes+static_cast<std::size_t>(indexMovedTo)-leftChunkFirst);
                            if constexpr ( hasElementMovedOp<Route> )
                                mirrorRotationToIndexes(trackedIndexes, leftChunkFirst, leftChunkFirst+countValidIndexes, leftChunkFirst+countValidIndexes+static_cast<std::size_t>(indexMovedTo)-leftChunkFirst);
                        }
                        else if ( leftChunkFirst > static_cast<std::size_t>(indexMovedTo) ) // Rotate combined chunk leftwards to final position
                        {
                            auto chunkStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst));
                            auto chunkEnd = std::next(chunkStart, static_cast<std::ptrdiff_t>(countValidIndexes));
                            std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(indexMovedTo)), chunkStart, chunkEnd);
                            mirrorRotationToSelection(sel, static_cast<std::size_t>(indexMovedTo), leftChunkFirst, leftChunkFirst+countValidIndexes);
                            if constexpr ( hasElementMovedOp<Route> )
                                mirrorRotationToIndexes(trackedIndexes, static_cast<std::size_t>(indexMovedTo), leftChunkFirst, leftChunkFirst+countValidIndexes);
                        }
                    }
                    
                    if constexpr ( hasElementMovedOp<Route> )
                    {
                        for ( std::size_t i=0; i<size; ++i )
                        {
                            if ( i != trackedIndexes[i] )
                                notifyElementMoved(user, Route{keys}, trackedIndexes[i], i);
                        }
                    }
                    if constexpr ( hasSelectionsChangedOp<Route> )
                        notifySelectionsChanged(user, Route{keys});
                }
            });
        }

        template <class Value, class Member>
        void readValue(std::size_t & offset, auto && value) const
        {
            using U = std::remove_cvref_t<Value>;
            using index_type = index_type_t<default_index_type, Member>;
            if constexpr ( std::is_same_v<U, std::string> )
            {
                auto stringSize = (index_type &)events[offset];
                offset += sizeof(stringSize);
                if ( stringSize > 0 )
                {
                    value = std::string{(const char*)&events[offset], stringSize};
                    offset += stringSize;
                }
            }
            else if constexpr ( RareTs::is_optional_v<U> )
            {
                if ( u8bool::read(events, offset) ) // optional.has_value()
                {
                    value = typename U::value_type{};
                    readValue<typename U::value_type, Member>(offset, *value);
                }
                else
                    value = std::nullopt;
            }
            else if constexpr ( is_flat_mdspan_v<U> )
            {
                offset += sizeof(index_type); // Skip over array size
                for ( auto it = value.flatBegin(); it != value.flatEnd(); ++it )
                    readValue<typename U::element_type, Member>(offset, *it);
            }
            else if constexpr ( std::is_array_v<U> )
            {
                auto span = as_1d<Value>(value);
                offset += sizeof(index_type); // Skip over array size
                for ( auto it = span.flatBegin(); it != span.flatEnd(); ++it )
                    readValue<typename decltype(span)::element_type, Member>(offset, *it);
            }
            else if constexpr ( RareTs::is_static_array_v<U> )
            {
                constexpr std::size_t size = RareTs::static_array_size_v<U>;
                offset += sizeof(size); // Skip over array size
                for ( std::size_t i=0; i<size; ++i )
                    readValue<std::remove_cvref_t<decltype(std::declval<U>()[0])>, Member>(offset, value[i]);
            }
            else if constexpr ( RareTs::is_iterable_v<U> && requires{value.resize(std::size_t{0});} )
            {
                auto size = static_cast<std::size_t>((index_type &)events[offset]);
                value.resize(std::size_t{size});
                offset += sizeof(index_type);
                for ( decltype(size) i=0; i<size; ++i )
                    readValue<std::remove_cvref_t<decltype(std::declval<U>()[0])>, Member>(offset, value[i]);
            }
            else if constexpr ( RareTs::is_reflected_v<U> )
            {
                RareTs::Members<U>::forEach([&](auto member) {
                    readValue<std::remove_cvref_t<typename decltype(member)::type>, decltype(member)>(offset, member.value(value));
                });
            }
            else
            {
                value = (U &)events[offset];
                offset += sizeof(U);
            }
        }

        template <class Value, class Member>
        auto readValue(std::size_t & offset) const
        {
            static_assert(!std::is_array_v<Value>, "Cannot return array[] types, use the readValue reference overload");
            std::remove_cvref_t<Value> value {};
            readValue<Value, Member>(offset, value);
            return value;
        }

        template <class Value, class Member>
        auto peekValue(std::size_t offset) const // readValue but doesn't change offset
        {
            if constexpr ( std::is_array_v<Value> ) // Avoid trying to return array[] types, make a vector
            {
                using element_type = std::remove_cvref_t<RareTs::element_type_t<Value>>;
                if constexpr ( std::is_array_v<element_type> )
                {
                    using sub_elem_type = std::remove_cvref_t<RareTs::element_type_t<element_type>>;
                    std::vector<std::vector<sub_elem_type>> value(RareTs::static_array_size_v<Value>);
                    for ( auto & vec : value )
                        vec.assign(RareTs::static_array_size_v<element_type>, {});

                    readValue<decltype(value), Member>(offset, value);
                    return value;
                }
                else
                {
                    std::vector<RareTs::element_type_t<Value>> value { std::size_t{RareTs::static_array_size_v<Value>} };
                    readValue<decltype(value), Member>(offset, value);
                    return value;
                }
            }
            else
            {
                std::remove_cvref_t<Value> value {};
                readValue<Value, Member>(offset, value);
                return value;
            }
        }

        template <class index_type>
        auto readIndex(std::size_t & offset) const {
            auto index = (index_type &)events[offset];
            offset += sizeof(index);
            return index;
        }

        template <class index_type>
        auto readIndex(std::ptrdiff_t & offset) const {
            auto index = (index_type &)events[static_cast<std::size_t>(offset)];
            offset += sizeof(index);
            return index;
        }

        template <class index_type>
        std::span<const index_type> readIndexes(std::size_t & offset, std::size_t count) const {
            std::span<const index_type> indexes(reinterpret_cast<const index_type*>(&events[offset]), count);
            offset += sizeof(index_type)*count;
            return indexes;
        }

        template <class index_type>
        std::span<const index_type> readIndexes(std::ptrdiff_t & offset, std::size_t count) const {
            std::span<const index_type> indexes(reinterpret_cast<const index_type*>(&events[static_cast<std::size_t>(offset)]), count);
            offset += sizeof(index_type)*count;
            return indexes;
        }

        template <class value_type, class Member, class ... Pathway>
        void processUndoEvent(std::uint8_t op, std::size_t & offset, std::optional<std::size_t> & secondaryOffset, auto & ref, auto keys)
        {
            using index_type = index_type_t<default_index_type, Member>;
            using path_pack = type_tags<Pathway...>;
            using sel_type = std::remove_cvref_t<decltype(getSelections<Pathway...>())>;
            using element_type = RareTs::element_type_t<value_type>;
            using Route = PathTaggedKeys<decltype(keys), path_pack, EditorType>;
            using ElemPath = type_tags<Pathway..., PathIndex<std::tuple_size_v<std::remove_cvref_t<decltype(keys)>>>>;
            using ElemKeys = std::remove_cvref_t<decltype(std::tuple_cat(keys, std::tuple<index_type>{0}))>;
            using ElemRoute = PathTaggedKeys<ElemKeys, ElemPath, EditorType>;
            constexpr bool hasSelections = !std::is_null_pointer_v<sel_type> && RareTs::is_specialization_v<sel_type, std::vector>;
            constexpr bool hasSelChangeOp = hasSelections && hasSelectionsChangedOp<Route>;
            constexpr bool isIterable = RareTs::is_iterable_v<value_type>;
            constexpr bool hasMoveOps = !std::is_void_v<element_type> && requires { ref.begin(); };

            switch ( Op(op) )
            {
                case Op::Reset:
                {
                    if constexpr ( !isIterable && hasValueChangedOp<Route, value_type> )
                    {
                        if constexpr ( std::is_array_v<typename Member::type> )
                        {
                            typename Member::type prevValue {};
                            auto refArray = as_1d<typename Member::type>(ref);
                            as_1d(prevValue) = refArray;
                            readValue<value_type, Member>(offset, refArray);
                            notifyValueChanged(user, Route{keys}, prevValue, ref); // Issue change notification
                        }
                        else
                        {
                            auto prevValue = ref;
                            ref = readValue<value_type, Member>(offset);
                            notifyValueChanged(user, Route{keys}, prevValue, ref);
                        }
                    }
                    else if constexpr ( isIterable && hasElementAddedOp<Route> )
                    {
                        ref = readValue<value_type, Member>(offset);
                        for ( std::size_t i=0; i<std::size(ref); ++i )
                            notifyElementAdded(user, Route{keys}, i);
                    }
                    else if constexpr ( is_array_member_v<Member> && requires { ref[0]; } )
                        readValue<value_type, Member>(offset, ref);
                    else
                        ref = readValue<value_type, Member>(offset);

                    if constexpr ( hasSelections )
                    {
                        readSelections(events, offset, getSelections<Pathway...>());
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::Reserve:
                {
                    if constexpr ( requires { ref.shrink_to_fit(); } )
                        ref.shrink_to_fit();
                }
                break;
                case Op::Trim:
                {
                    // NoOp
                }
                break;
                case Op::Assign:
                {
                    if constexpr ( hasSelections && isIterable && requires { ref.reserve(1); } )
                    {
                        readIndex<index_type>(offset); // new size (unused)
                        readValue<element_type, Member>(offset); // assigned value (unused)
                        auto size = static_cast<std::size_t>(readIndex<index_type>(offset)); // prev size

                        value_type prevContainer {};
                        prevContainer.reserve(size);
                        for ( std::size_t i=0; i<size; ++i )
                            prevContainer.push_back(readValue<element_type, Member>(offset));

                        std::swap(ref, prevContainer);
                        if constexpr ( hasElementRemovedOp<Route> )
                        {
                            std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(prevContainer))-1;
                            for ( ; i>=0; --i )
                                notifyElementRemoved(user, Route{keys}, static_cast<std::size_t>(i));
                        }
                        if constexpr ( hasElementAddedOp<Route> )
                        {
                            for ( std::size_t i=0; i<std::size(ref); ++i )
                                notifyElementAdded(user, Route{keys}, i);
                        }

                        if constexpr ( hasSelChangeOp )
                        {
                            if ( readSelections(events, offset, getSelections<Pathway...>()) )
                                notifySelectionsChanged(user, Route{keys});
                        }
                        else
                            readSelections(events, offset, getSelections<Pathway...>());
                    }
                }
                break;
                case Op::AssignDefault:
                {
                    if constexpr ( hasSelections && isIterable && requires { ref.reserve(1); } )
                    {
                        readIndex<index_type>(offset); // new size (unused)
                        auto size = static_cast<std::size_t>(readIndex<index_type>(offset)); // old size

                        value_type prevContainer {};
                        prevContainer.reserve(size);
                        for ( std::size_t i=0; i<size; ++i )
                            prevContainer.push_back(readValue<element_type, Member>(offset));

                        std::swap(ref, prevContainer);
                        if constexpr ( hasElementRemovedOp<Route> )
                        {
                            std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(prevContainer))-1;
                            for ( ; i>=0; --i )
                                notifyElementRemoved(user, Route{keys}, static_cast<std::size_t>(i));
                        }
                        if constexpr ( hasElementAddedOp<Route> )
                        {
                            for ( std::size_t i=0; i<std::size(ref); ++i )
                                notifyElementAdded(user, Route{keys}, i);
                        }

                        if constexpr ( hasSelChangeOp )
                        {
                            if ( readSelections(events, offset, getSelections<Pathway...>()) )
                                notifySelectionsChanged(user, Route{keys});
                        }
                        else
                            readSelections(events, offset, getSelections<Pathway...>());
                    }
                }
                break;
                case Op::ClearSelections:
                {
                    if constexpr ( hasSelections )
                    {
                        auto & sel = getSelections<Pathway...>();
                        sel_type prevSelections {};
                        readSelectionVector(events, offset, prevSelections);
                        std::swap(sel, prevSelections);
                    }
                    if constexpr ( hasSelChangeOp )
                        notifySelectionsChanged(user, Route{keys});
                }
                break;
                case Op::SelectAll:
                {
                    if constexpr ( hasSelections )
                    {
                        auto & sel = getSelections<Pathway...>();
                        sel_type prevSelections {};
                        readSelectionVector(events, offset, prevSelections);
                        std::swap(sel, prevSelections);

                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::Select:
                {
                    if constexpr ( hasSelections )
                    {
                        auto & sel = getSelections<Pathway...>();
                        auto value = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto found = std::find(sel.begin(), sel.end(), value);
                        if ( found != sel.end() )
                            sel.erase(found);

                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::SelectN:
                {
                    if constexpr ( hasSelections )
                    {
                        auto & sel = getSelections<Pathway...>();
                        auto size = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto selIndexes = readIndexes<index_type>(offset, size);
                        for ( std::size_t i=0; i<size; ++i )
                        {
                            auto found = std::find(sel.begin(), sel.end(), selIndexes[i]);
                            if ( found != sel.end() )
                                sel.erase(found);
                        }
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::Deselect:
                {
                    if constexpr ( hasSelections )
                    {
                    auto & sel = getSelections<Pathway...>();
                    auto value = readIndex<index_type>(offset);
                    auto selIndex = readIndex<index_type>(offset);
                    if ( std::find(sel.begin(), sel.end(), value) == sel.end() )
                        sel.insert(std::next(sel.begin(), static_cast<std::ptrdiff_t>(selIndex)), value);
                        
                    if constexpr ( hasSelChangeOp )
                        notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::DeselectN:
                {
                    if constexpr ( hasSelections )
                    {
                        auto & sel = getSelections<Pathway...>();
                        auto size = static_cast<std::ptrdiff_t>(readIndex<index_type>(offset));
                        auto deselectIndexes = readIndexes<index_type>(offset, static_cast<std::size_t>(size));
                        auto prevSelIndexes = readIndexes<index_type>(offset, static_cast<std::size_t>(size));

                        for ( std::ptrdiff_t i=size-1; i>=0; --i )
                        {
                            sel.insert(std::next(sel.begin(), static_cast<std::ptrdiff_t>(prevSelIndexes[static_cast<std::size_t>(i)])),
                                deselectIndexes[static_cast<std::size_t>(i)]);
                        }

                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::ToggleSelection:
                {
                    if constexpr ( hasSelections )
                    {
                        auto & sel = getSelections<Pathway...>();
                        auto value = readIndex<index_type>(offset);
                        if ( u8bool::read(events, offset) ) // wasSelected
                            sel.insert(std::next(sel.begin(), static_cast<std::ptrdiff_t>(readIndex<index_type>(offset))), value);
                        else
                            std::erase(sel, value);
                        
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::ToggleSelectionN:
                {
                    if constexpr ( hasSelections )
                    {
                        auto & sel = getSelections<Pathway...>();
                        auto size = static_cast<std::ptrdiff_t>(readIndex<index_type>(offset));
                        auto toggledIndexes = readIndexes<index_type>(offset, static_cast<std::size_t>(size));

                        std::vector<bool> wasSelected {};
                        readVecBoolData(events, offset, static_cast<std::size_t>(size), wasSelected);

                        auto prevSelIndexes = readIndexes<index_type>(offset, static_cast<std::size_t>(size));

                        for ( std::ptrdiff_t i=size-1; i>=0; --i )
                        {
                            auto found = std::find(sel.begin(), sel.end(), toggledIndexes[static_cast<std::size_t>(i)]);
                            if ( found == sel.end() )
                                sel.insert(std::next(sel.begin(), static_cast<std::ptrdiff_t>(prevSelIndexes[static_cast<std::size_t>(i)])), toggledIndexes[static_cast<std::size_t>(i)]);
                            else
                                sel.erase(found);
                        }

                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::SortSelections:
                {
                    if constexpr ( hasSelections )
                    {
                        auto & sel = getSelections<Pathway...>();
                        auto size = readIndex<index_type>(offset);
                        auto sourceIndexes = readIndexes<index_type>(offset, size);

                        undoSort(sel, sourceIndexes);

                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::SortSelectionsDesc:
                {
                    if constexpr ( hasSelections )
                    {
                        auto & sel = getSelections<Pathway...>();
                        auto size = readIndex<index_type>(offset);
                        auto sourceIndexes = readIndexes<index_type>(offset, size);

                        undoSort(sel, sourceIndexes);

                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::Set:
                {
                    if constexpr ( RareTs::is_assignable_v<decltype(ref), value_type> )
                    {
                        readValue<value_type, Member>(offset); // newValue (unused)
                        auto prevValue = readValue<value_type, Member>(offset);
                        if constexpr ( !isIterable && hasValueChangedOp<Route, value_type> )
                        {
                            auto temp = ref;
                            ref = prevValue;
                            notifyValueChanged(user, Route{keys}, temp, ref);
                        }
                        else
                        {
                            if constexpr ( isIterable && hasElementRemovedOp<Route> )
                            {
                                std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1;
                                ref = prevValue;
                                for ( ; i>=0; --i )
                                    notifyElementRemoved(user, Route{keys}, static_cast<std::size_t>(i));
                            }
                            else
                                ref = prevValue;

                            if constexpr ( isIterable && hasElementAddedOp<Route> )
                            {
                                for ( std::size_t i=0; i<std::size(ref); ++i )
                                    notifyElementAdded(user, Route{keys}, i);
                            }
                        }

                        if constexpr ( hasSelections )
                        {
                            readSelections(events, offset, getSelections<Pathway...>());
                            if constexpr ( hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::SetN:
                {
                    if constexpr ( requires{ref[0];} && requires{ref[0] = std::declval<decltype(ref[0])>();} )
                    {
                        constexpr bool isIterableElement = RareTs::is_iterable_v<element_type>;
                        auto count = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto setIndexes = readIndexes<index_type>(offset, count);

                        readValue<element_type, Member>(offset); // new value (unused);
                            
                        if constexpr ( !isIterableElement && hasValueChangedOp<Route, element_type> )
                        {
                            for ( auto index : setIndexes )
                            {
                                auto prevValue = readValue<element_type, Member>(offset);
                                std::swap(ref[index], prevValue);
                                notifyValueChanged(user, ElemRoute{std::tuple_cat(keys, std::tuple<index_type>{index})}, prevValue, ref[index]);
                            }
                        }
                        else if constexpr ( isIterableElement && hasElementRemovedOp<Route> )
                        {
                            for ( auto index : setIndexes )
                            {
                                std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref[index]))-1;
                                ref[index] = readValue<element_type, Member>(offset);
                                for ( ; i>=0; --i )
                                    notifyElementRemoved(user, ElemRoute{std::tuple_cat(keys, std::tuple<index_type>{index})}, static_cast<std::size_t>(i));

                                if constexpr ( hasElementAddedOp<Route> )
                                {
                                    for ( std::size_t j=0; j<std::size(ref[index]); ++j )
                                        notifyElementAdded(user, ElemRoute{std::tuple_cat(keys, std::tuple<index_type>{index})}, j);
                                }
                            }
                        }
                        else
                        {
                            for ( auto index : setIndexes )
                            {
                                ref[index] = readValue<element_type, Member>(offset);
                                if constexpr ( isIterableElement && hasElementAddedOp<Route> )
                                {
                                    for ( std::size_t i=0; i<std::size(ref[index]); ++i )
                                        notifyElementAdded(user, ElemRoute{std::tuple_cat(keys, std::tuple<index_type>{index})}, i);
                                }
                            }
                        }
                    }
                }
                break;
                case Op::SetL:
                {
                    if constexpr ( RareTs::is_assignable_v<decltype(ref), value_type> )
                    {
                        if ( !secondaryOffset )
                        {
                            secondaryOffset = offset; // Is set exclusively for the first visit of the selection
                            readValue<value_type, Member>(*secondaryOffset); // Advanced past valueSetTo (unused for undos, used for redos)
                        }
                        auto prevValue = readValue<value_type, Member>(*secondaryOffset);
                        if constexpr ( !isIterable && hasValueChangedOp<Route, value_type> )
                        {
                            auto temp = ref;
                            ref = prevValue;
                            notifyValueChanged(user, Route{keys}, temp, ref);
                        }
                        else
                        {
                            if constexpr ( isIterable && hasElementRemovedOp<Route> )
                            {
                                std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1;
                                ref = prevValue;
                                for ( ; i>=0; --i )
                                    notifyElementRemoved(user, Route{keys}, static_cast<std::size_t>(i));
                            }
                            else
                                ref = prevValue;

                            if constexpr ( isIterable && hasElementAddedOp<Route> )
                            {
                                for ( std::size_t i=0; i<std::size(ref); ++i )
                                    notifyElementAdded(user, Route{keys}, i);
                            }
                        }
                    }
                }
                break;
                case Op::Append:
                {
                    if constexpr ( RareTs::has_pop_back_v<decltype(ref)> )
                        ref.pop_back();
                    if constexpr ( hasElementRemovedOp<Route> && requires { ref.size(); } )
                        notifyElementRemoved(user, Route{keys}, ref.size());
                }
                break;
                case Op::AppendN:
                {
                    if constexpr ( requires { ref.size(); ref.erase(ref.begin(), ref.end()); } )
                    {
                        auto count = static_cast<std::ptrdiff_t>(readIndex<index_type>(offset));
                        auto size = static_cast<std::ptrdiff_t>(ref.size());
                        ref.erase(std::next(ref.begin(), size-count), ref.end());
                        if constexpr ( hasElementRemovedOp<Route> )
                        {
                            for ( std::ptrdiff_t i=size-1; i>=size-count; --i )
                                notifyElementRemoved(user, Route{keys}, static_cast<std::size_t>(i));
                        }
                    }
                }
                break;
                case Op::Insert:
                {
                    if constexpr ( hasSelections && !std::is_void_v<element_type> && requires { ref.erase(ref.begin()); } )
                    {
                        auto insertionIndex = readIndex<index_type>(offset);
                        ref.erase(std::next(ref.begin(), static_cast<std::ptrdiff_t>(insertionIndex)));
                        if constexpr ( hasElementRemovedOp<Route> )
                            notifyElementRemoved(user, Route{keys}, static_cast<std::size_t>(insertionIndex));
                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            for ( std::size_t i=static_cast<std::size_t>(insertionIndex); i<std::size(ref); ++i )
                                notifyElementMoved(user, Route{keys}, i+1, i);
                        }

                        if constexpr ( hasSelections )
                        {
                            auto & sel = getSelections<Pathway...>();
                            for ( auto & index : sel )
                            {
                                if ( static_cast<index_type>(index) >= insertionIndex )
                                    --index;
                            }
                            if constexpr ( hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::InsertN:
                {
                    if constexpr ( !std::is_void_v<element_type> && requires { ref.erase(ref.begin(), ref.end()); } )
                    {
                        auto insertionCount = readIndex<index_type>(offset);
                        auto insertionIndex = readIndex<index_type>(offset);
                        ref.erase(
                            std::next(ref.begin(), static_cast<std::ptrdiff_t>(insertionIndex)),
                            std::next(ref.begin(), static_cast<std::ptrdiff_t>(insertionIndex)+static_cast<std::ptrdiff_t>(insertionCount))
                        );
                        if constexpr ( hasElementRemovedOp<Route> )
                        {
                            for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(insertionIndex)+static_cast<std::ptrdiff_t>(insertionCount)-1;
                                i >= static_cast<std::ptrdiff_t>(insertionIndex); --i )
                            {
                                notifyElementRemoved(user, Route{keys}, static_cast<std::size_t>(i));
                            }
                        }
                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            for ( std::size_t i=static_cast<std::size_t>(insertionIndex);
                                i+1<static_cast<std::size_t>(insertionIndex)+static_cast<std::size_t>(insertionCount); ++i )
                            {
                                notifyElementMoved(user, Route{keys}, i+static_cast<std::size_t>(insertionCount), i);
                            }
                        }
                        if constexpr ( hasSelections )
                        {
                            auto & sel = getSelections<Pathway...>();
                            for ( auto & index : sel )
                            {
                                if ( static_cast<index_type>(index) >= insertionIndex )
                                    index -= insertionCount;
                            }
                            if constexpr ( hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::Remove:
                {
                    if constexpr ( !std::is_void_v<element_type> && requires { ref.insert(ref.begin(), std::declval<element_type>()); } )
                    {
                        auto removalIndex = readIndex<index_type>(offset);
                        auto removedValue = readValue<element_type, Member>(offset);
                        ref.insert(std::next(ref.begin(), static_cast<std::ptrdiff_t>(removalIndex)), removedValue);
                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(std::size(ref))-1; i>static_cast<std::ptrdiff_t>(removalIndex); --i )
                                notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(i-1), static_cast<std::size_t>(i));
                        }
                        if constexpr ( hasElementAddedOp<Route> )
                            notifyElementAdded(user, Route{keys}, removalIndex);

                        if ( u8bool::read(events, offset) )
                        {
                            if constexpr ( hasSelections )
                            {
                                auto & sel = getSelections<Pathway...>();
                                auto prevSelIndex = readIndex<index_type>(offset);
                                sel.insert(std::next(sel.begin(), static_cast<std::ptrdiff_t>(prevSelIndex)), removalIndex);
                                for ( auto & i : sel )
                                {
                                    if ( i > removalIndex )
                                        ++i;
                                }
                            }
                        }
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::RemoveN:
                {
                    if constexpr ( !std::is_void_v<element_type> && requires { ref.insert(ref.begin(), std::declval<element_type>()); } )
                    {
                        std::ptrdiff_t removalCount = static_cast<std::ptrdiff_t>(readIndex<index_type>(offset));
                        auto removalIndexes = readIndexes<index_type>(offset, static_cast<std::size_t>(removalCount)); // removalIndexes were pre-sorted highest index to lowest index

                        std::vector<element_type> removedValues {};
                        removedValues.reserve(static_cast<std::size_t>(removalCount));
                        for ( std::ptrdiff_t i=0; i<removalCount; ++i )
                            removedValues.push_back(readValue<element_type, Member>(offset));
                            
                        for ( std::ptrdiff_t i=removalCount-1; i>=0; --i ) // insert values which were removed from the lowest indexes first
                        {
                            auto reinsertedIndex = removalIndexes[static_cast<std::size_t>(i)];
                            ref.insert(std::next(ref.begin(), static_cast<std::ptrdiff_t>(reinsertedIndex)), removedValues[static_cast<std::size_t>(i)]);
                            if constexpr ( hasSelections )
                            {
                                auto & sel = getSelections<Pathway...>();
                                for ( auto & s : sel )
                                {
                                    if ( s >= reinsertedIndex )
                                        ++s;
                                }
                            }
                        }
                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            std::ptrdiff_t collectionIndex = static_cast<std::ptrdiff_t>(std::size(ref))-1;
                            std::ptrdiff_t moveDistance = static_cast<std::ptrdiff_t>(std::size(removalIndexes));
                            for ( ; collectionIndex>static_cast<std::ptrdiff_t>(removalIndexes[0]); --collectionIndex )
                                notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(collectionIndex-moveDistance), static_cast<std::size_t>(collectionIndex));
                            for ( std::size_t i=1; i<std::size(removalIndexes); ++i )
                            {
                                --moveDistance;
                                for ( collectionIndex = static_cast<std::ptrdiff_t>(removalIndexes[i-1])-1; collectionIndex > static_cast<std::ptrdiff_t>(removalIndexes[i]); --collectionIndex )
                                    notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(collectionIndex-moveDistance), static_cast<std::size_t>(collectionIndex));
                            }
                        }
                        if constexpr ( hasElementAddedOp<Route> )
                        {
                            for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(std::size(removalIndexes))-1; i>=0; --i )
                                notifyElementAdded(user, Route{keys}, static_cast<std::size_t>(removalIndexes[static_cast<std::size_t>(i)]));
                        }

                        if constexpr ( hasSelections )
                        {
                            auto & sel = getSelections<Pathway...>();
                            std::vector<bool> removalIndexSelected {};
                            readVecBoolData(events, offset, static_cast<std::size_t>(removalCount), removalIndexSelected);

                            std::size_t prevSelIndexCount = static_cast<std::size_t>(readIndex<index_type>(offset));
                            auto prevSelIndexes = readIndexes<index_type>(offset, prevSelIndexCount);

                            std::size_t unremovedI = std::size(prevSelIndexes)-1;
                            for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(removalCount)-1; i>=0; --i )
                            {
                                if ( removalIndexSelected[static_cast<std::size_t>(i)] )
                                {
                                    auto insertedValue = removalIndexes[static_cast<std::size_t>(i)];
                                    sel.insert(std::next(sel.begin(), static_cast<std::ptrdiff_t>(prevSelIndexes[unremovedI])), insertedValue);
                                    --unremovedI;
                                }
                            }
                                
                            if constexpr ( hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::RemoveL:
                {
                    if constexpr ( !std::is_void_v<element_type> && requires { ref.insert(ref.begin(), std::declval<element_type>()); } )
                    {
                        std::ptrdiff_t removalCount = static_cast<std::ptrdiff_t>(readIndex<index_type>(offset));
                        auto removalIndexes = readIndexes<index_type>(offset, static_cast<std::size_t>(removalCount)); // removalIndexes were pre-sorted highest index to lowest index

                        std::vector<element_type> removedValues {};
                        removedValues.reserve(static_cast<std::size_t>(removalCount));
                        for ( std::ptrdiff_t i=0; i<removalCount; ++i )
                            removedValues.push_back(readValue<element_type, Member>(offset));

                        for ( std::ptrdiff_t i=removalCount-1; i>=0; --i ) // insert values which were removed from the lowest indexes first
                            ref.insert(std::next(ref.begin(), static_cast<std::ptrdiff_t>(removalIndexes[static_cast<std::size_t>(i)])), removedValues[static_cast<std::size_t>(i)]);

                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            std::ptrdiff_t collectionIndex = static_cast<std::ptrdiff_t>(std::size(ref))-1;
                            std::ptrdiff_t moveDistance = static_cast<std::ptrdiff_t>(std::size(removalIndexes));
                            for ( ; collectionIndex>static_cast<std::ptrdiff_t>(removalIndexes[0]); --collectionIndex )
                                notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(collectionIndex-moveDistance), static_cast<std::size_t>(collectionIndex));
                            for ( std::size_t i=1; i<std::size(removalIndexes); ++i )
                            {
                                --moveDistance;
                                for ( collectionIndex = static_cast<std::ptrdiff_t>(removalIndexes[i-1])-1; collectionIndex > static_cast<std::ptrdiff_t>(removalIndexes[i]); --collectionIndex )
                                    notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(collectionIndex-moveDistance), static_cast<std::size_t>(collectionIndex));
                            }
                        }
                            
                        if constexpr ( hasElementAddedOp<Route> )
                        {
                            for ( std::ptrdiff_t i=removalCount-1; i>=0; --i ) // insert values which were removed from the lowest indexes first
                                notifyElementAdded(user, Route{keys}, static_cast<std::size_t>(removalIndexes[static_cast<std::size_t>(i)]));
                        }

                        readSelections(events, offset, getSelections<Pathway...>());

                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                };
                break;
                case Op::Sort:
                {
                    if constexpr ( RareTs::is_specialization_v<value_type, std::vector> )
                    {
                        std::size_t count = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto sourceIndexes = readIndexes<index_type>(offset, count);

                        undoSort(ref, sourceIndexes);

                        if constexpr ( hasSelections )
                        {
                            auto & sel = getSelections<Pathway...>();
                            sel_type oldSel {};
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( std::find(sel.begin(), sel.end(), i) != sel.end() )
                                    RareTs::append(oldSel, sourceIndexes[i]);
                            }

                            std::swap(sel, oldSel);
                        }

                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(count)-1; i>=0; --i )
                            {
                                if ( i != static_cast<std::ptrdiff_t>(sourceIndexes[static_cast<std::size_t>(i)]) )
                                    notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(i), static_cast<std::size_t>(sourceIndexes[static_cast<std::size_t>(i)]));
                            }
                        }
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::SortDesc:
                {
                    if constexpr ( RareTs::is_specialization_v<value_type, std::vector> )
                    {
                        std::size_t count = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto sourceIndexes = readIndexes<index_type>(offset, count);
                            
                        undoSort(ref, sourceIndexes);

                        if constexpr ( hasSelections )
                        {
                            auto & sel = getSelections<Pathway...>();
                            sel_type oldSel {};
                            for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(count)-1; i>=0; --i )
                            {
                                if ( std::find(sel.begin(), sel.end(), static_cast<index_type>(i)) != sel.end() )
                                    RareTs::append(oldSel, sourceIndexes[static_cast<std::size_t>(i)]);
                            }

                            std::swap(sel, oldSel);
                        }

                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(count)-1; i>=0; --i )
                            {
                                if ( i != static_cast<std::ptrdiff_t>(sourceIndexes[static_cast<std::size_t>(i)]) )
                                    notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(i), static_cast<std::size_t>(sourceIndexes[static_cast<std::size_t>(i)]));
                            }
                        }
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::Swap:
                {
                    if constexpr ( hasMoveOps )
                    {
                        auto firstIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto secondIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                        if ( firstIndex != secondIndex && firstIndex < std::size(ref) && secondIndex < std::size(ref) )
                        {
                            std::swap(ref[firstIndex], ref[secondIndex]);
                            mirrorSwapToSelection(getSelections<Pathway...>(), firstIndex, secondIndex);
                            if constexpr ( hasElementMovedOp<Route> )
                            {
                                notifyElementMoved(user, Route{keys}, secondIndex, firstIndex);
                                notifyElementMoved(user, Route{keys}, firstIndex, secondIndex);
                            }
                            if constexpr ( hasSelections && hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::MoveUp:
                {
                    if constexpr ( hasMoveOps )
                    {
                        auto movedIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                        if ( movedIndex > 0 && movedIndex < std::size(ref) )
                        {
                            std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex-1)]);
                            mirrorSwapToSelection(getSelections<Pathway...>(), movedIndex, movedIndex-1);
                            if constexpr ( hasElementMovedOp<Route> )
                            {
                                notifyElementMoved(user, Route{keys}, movedIndex-1, movedIndex);
                                notifyElementMoved(user, Route{keys}, movedIndex, movedIndex-1);
                            }
                            if constexpr ( hasSelections && hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::MoveUpN:
                {
                    if constexpr ( hasMoveOps )
                    {
                        auto count = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto movedIndexes = readIndexes<index_type>(offset, count);
                        if ( count > 0 )
                        {
                            std::size_t minimumIndexMoved = 0;
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( minimumIndexMoved == static_cast<std::size_t>(movedIndexes[i]) )
                                    ++minimumIndexMoved;
                                else
                                    break;
                            }

                            for ( auto it = movedIndexes.rbegin(); it != movedIndexes.rend(); ++it )
                            {
                                auto i = static_cast<std::size_t>(*it);
                                if ( i > minimumIndexMoved && i < std::size(ref) )
                                {
                                    std::swap(ref[i-1], ref[i]);
                                    mirrorSwapToSelection(getSelections<Pathway...>(), i-1, i);
                                }
                            }
                        }

                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            std::size_t nextAvailable = 0;
                            std::size_t blockSize = 1;
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( movedIndexes[i] > nextAvailable && static_cast<std::size_t>(movedIndexes[i]) < std::size(ref) )
                                {
                                    notifyElementMoved(user, Route{keys}, movedIndexes[i]-1, movedIndexes[i]);
                                    if ( i+1 < std::size(movedIndexes) && movedIndexes[i]+1 == movedIndexes[i+1] )
                                        ++blockSize;
                                    else
                                    {
                                        notifyElementMoved(user, Route{keys}, movedIndexes[i], movedIndexes[i]-blockSize);
                                        blockSize = 1;
                                    }
                                }
                                else if ( movedIndexes[i] == nextAvailable )
                                    ++nextAvailable;
                            }
                        }
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::MoveUpL:
                {
                    if constexpr ( hasSelections && hasMoveOps )
                    {
                        auto & sel = getSelections<Pathway...>();
                        sel_type prevSel {};
                        readSelections(events, offset, prevSel);

                        auto movedIndexes = prevSel;
                        auto count = movedIndexes.size();
                        std::sort(movedIndexes.begin(), movedIndexes.end());

                        std::size_t minimumIndexMoved = 0;
                        for ( std::size_t i=0; i<count; ++i )
                        {
                            if ( minimumIndexMoved == movedIndexes[i] )
                                ++minimumIndexMoved;
                            else
                                break;
                        }

                        auto size = std::size(ref);
                        for ( auto it = movedIndexes.rbegin(); it != movedIndexes.rend(); ++it )
                        {
                            auto i = static_cast<std::size_t>(*it);
                            if ( i > minimumIndexMoved && i < size )
                                std::swap(ref[i-1], ref[i]);
                        }
                        std::swap(sel, prevSel);

                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            std::size_t nextAvailable = 0;
                            std::size_t blockSize = 1;
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( movedIndexes[i] > nextAvailable && static_cast<std::size_t>(movedIndexes[i]) < std::size(ref) )
                                {
                                    notifyElementMoved(user, Route{keys}, movedIndexes[i]-1, movedIndexes[i]);
                                    if ( i+1 < std::size(movedIndexes) && movedIndexes[i]+1 == movedIndexes[i+1] )
                                        ++blockSize;
                                    else
                                    {
                                        notifyElementMoved(user, Route{keys}, movedIndexes[i], movedIndexes[i]-blockSize);
                                        blockSize = 1;
                                    }
                                }
                                else if ( movedIndexes[i] == nextAvailable )
                                    ++nextAvailable;
                            }
                        }
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::MoveTop:
                {
                    if constexpr ( hasMoveOps )
                    {
                        auto movedIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                        if ( movedIndex > 0 && movedIndex < std::size(ref) )
                        {
                            auto it = ref.begin();
                            std::rotate(it, it+1, std::next(ref.begin(), static_cast<std::ptrdiff_t>(movedIndex))+1);
                            mirrorRotationToSelection(getSelections<Pathway...>(), 0, 1, movedIndex+1);
                            if constexpr ( hasElementMovedOp<Route> )
                            {
                                notifyElementMoved(user, Route{keys}, 0, movedIndex);
                                for ( std::size_t i=0; i<static_cast<std::size_t>(movedIndex); ++i )
                                    notifyElementMoved(user, Route{keys}, i+1, i);
                            }
                            if constexpr ( hasSelections && hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::MoveTopN:
                {
                    if constexpr ( hasMoveOps )
                    {
                        auto count = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto movedIndexes = readIndexes<index_type>(offset, count);

                        std::size_t minimumIndexMoved = 0;
                        for ( std::size_t i=0; i<count; ++i )
                        {
                            if ( minimumIndexMoved == static_cast<std::size_t>(movedIndexes[i]) )
                                ++minimumIndexMoved;
                            else
                                break;
                        }

                        std::size_t size = std::size(ref);
                        std::size_t countValid = 0;
                        for ( auto i : movedIndexes )
                        {
                            if ( i > minimumIndexMoved && static_cast<std::size_t>(i) < size )
                                ++countValid;
                        }

                        std::vector<std::size_t> trackedIndexes {};
                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            trackedIndexes.assign(size, 0);
                            std::iota(trackedIndexes.begin(), trackedIndexes.end(), 0);
                        }

                        std::size_t i = minimumIndexMoved+countValid-1;
                        for ( auto it = movedIndexes.rbegin(); it != movedIndexes.rend(); ++it )
                        {
                            if ( static_cast<std::size_t>(*it) > minimumIndexMoved && static_cast<std::size_t>(*it) < size )
                            {
                                auto toMove = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                auto dest = std::next(ref.begin(), static_cast<std::ptrdiff_t>(*it));
                                std::rotate(toMove, toMove+1, dest+1);
                                if constexpr ( hasElementMovedOp<Route> )
                                    mirrorRotationToIndexes(trackedIndexes, i, i+1, *it+1);

                                --i;
                            }
                        }

                        if constexpr ( hasSelections )
                        {
                            auto & sel = getSelections<Pathway...>();
                            sel_type prevSel {};
                            readSelections(events, offset, prevSel);
                            std::swap(sel, prevSel);
                        }
                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            for ( i=0; i<size; ++i )
                            {
                                if ( i != trackedIndexes[i] )
                                    notifyElementMoved(user, Route{keys}, i, trackedIndexes[i]);
                            }
                        }
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::MoveTopL:
                {
                    if constexpr ( hasSelections && hasMoveOps )
                    {
                        auto & sel = getSelections<Pathway...>();
                        sel_type prevSel {};
                        readSelections(events, offset, prevSel);

                        auto movedIndexes = prevSel; // Copy so it can be sorted without changing selections
                        std::sort(movedIndexes.begin(), movedIndexes.end(), std::greater<index_type>{});

                        std::size_t size = std::size(ref);
                        std::vector<std::size_t> trackedIndexes {};
                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            trackedIndexes.assign(size, 0);
                            std::iota(trackedIndexes.begin(), trackedIndexes.end(), 0);
                        }

                        std::size_t toMoveIndex = std::size(prevSel)-1;
                        for ( auto i : movedIndexes )
                        {
                            if ( i > 0 && static_cast<std::size_t>(i) < size )
                            {
                                auto toMove = std::next(ref.begin(), static_cast<std::ptrdiff_t>(toMoveIndex));
                                auto dest = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                std::rotate(toMove, toMove+1, dest+1);
                                if constexpr ( hasElementMovedOp<Route> )
                                    mirrorRotationToIndexes(trackedIndexes, toMoveIndex, toMoveIndex+1, i+1);

                                --toMoveIndex;
                            }
                        }
                        std::swap(sel, prevSel);

                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            for ( std::size_t i=0; i<size; ++i )
                            {
                                if ( i != trackedIndexes[i] )
                                    notifyElementMoved(user, Route{keys}, i, trackedIndexes[i]);
                            }
                        }
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::MoveDown:
                {
                    if constexpr ( hasMoveOps )
                    {
                        auto movedIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                        if ( movedIndex+1 < std::size(ref) )
                        {
                            std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex+1)]);
                            mirrorSwapToSelection(getSelections<Pathway...>(), movedIndex, movedIndex+1);
                            if constexpr ( hasElementMovedOp<Route> )
                            {
                                notifyElementMoved(user, Route{keys}, movedIndex+1, movedIndex);
                                notifyElementMoved(user, Route{keys}, movedIndex, movedIndex+1);
                            }
                            if constexpr ( hasSelections && hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::MoveDownN:
                {
                    if constexpr ( hasMoveOps )
                    {
                        auto count = static_cast<std::size_t>(readIndex<index_type>(offset));
                        if ( count > 0 )
                        {
                            auto movedIndexes = readIndexes<index_type>(offset, count);

                            std::size_t maximumIndexMoved = std::size(ref);
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( static_cast<std::size_t>(movedIndexes[i])+1 == maximumIndexMoved )
                                    --maximumIndexMoved;
                                else
                                    break;
                            }

                            for ( auto it = movedIndexes.rbegin(); it != movedIndexes.rend(); ++it )
                            {
                                auto i = static_cast<std::size_t>(*it);
                                if ( i+1 < maximumIndexMoved )
                                {
                                    std::swap(ref[i], ref[i+1]);
                                    mirrorSwapToSelection(getSelections<Pathway...>(), i, i+1);
                                }
                            }

                            if constexpr ( hasElementMovedOp<Route> )
                            {
                                std::size_t limit = std::size(ref);
                                std::size_t blockSize = 1;
                                for ( std::size_t i=0; i<count; ++i )
                                {
                                    if ( static_cast<std::size_t>(movedIndexes[i])+1 < limit )
                                    {
                                        if ( i+1 < count && movedIndexes[i]-1 == movedIndexes[i+1] )
                                            ++blockSize;
                                        else
                                        {
                                            notifyElementMoved(user, Route{keys}, movedIndexes[i], movedIndexes[i]+blockSize);
                                            blockSize = 1;
                                        }
                                        notifyElementMoved(user, Route{keys}, movedIndexes[i]+1, movedIndexes[i]);
                                        limit = movedIndexes[i]+1;
                                    }
                                    else if ( limit > 0 )
                                        --limit;
                                }
                            }
                            if constexpr ( hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::MoveDownL:
                {
                    if constexpr ( hasSelections && hasMoveOps )
                    {
                        auto & sel = getSelections<Pathway...>();
                        sel_type prevSel {};
                        readSelections(events, offset, prevSel);

                        auto movedIndexes = prevSel;
                        std::sort(movedIndexes.begin(), movedIndexes.end(), std::greater<index_type>{});
                        std::size_t count = std::size(movedIndexes);

                        std::size_t maximumIndexMoved = std::size(ref);
                        for ( std::size_t i=0; i<count; ++i )
                        {
                            if ( static_cast<std::size_t>(movedIndexes[i])+1 == maximumIndexMoved )
                                --maximumIndexMoved;
                            else
                                break;
                        }

                        for ( auto it = movedIndexes.rbegin(); it != movedIndexes.rend(); ++it )
                        {
                            auto i = static_cast<std::size_t>(*it);
                            if ( static_cast<std::size_t>(i)+1 < maximumIndexMoved )
                                std::swap(ref[static_cast<std::size_t>(i)], ref[static_cast<std::size_t>(i)+1]);
                        }
                        std::swap(sel, prevSel);

                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            std::size_t limit = std::size(ref);
                            std::size_t blockSize = 1;
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( static_cast<std::size_t>(movedIndexes[i])+1 < limit )
                                {
                                    if ( i+1 < count && movedIndexes[i]-1 == movedIndexes[i+1] )
                                        ++blockSize;
                                    else
                                    {
                                        notifyElementMoved(user, Route{keys}, movedIndexes[i], movedIndexes[i]+blockSize);
                                        blockSize = 1;
                                    }
                                    notifyElementMoved(user, Route{keys}, movedIndexes[i]+1, movedIndexes[i]);
                                    limit = movedIndexes[i]+1;
                                }
                                else if ( limit > 0 )
                                    --limit;
                            }
                        }
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::MoveBottom:
                {
                    if constexpr ( hasMoveOps )
                    {
                        auto movedIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                        if ( movedIndex+1 < std::size(ref) )
                        {
                            auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(movedIndex));
                            auto movedTo = std::next(ref.begin(), static_cast<std::ptrdiff_t>(std::size(ref))-1);
                            std::rotate(it, movedTo, ref.end());
                            mirrorRotationToSelection(getSelections<Pathway...>(), movedIndex, std::size(ref)-1, std::size(ref));
                            if constexpr ( hasElementMovedOp<Route> )
                            {
                                notifyElementMoved(user, Route{keys}, std::size(ref)-1, movedIndex);
                                for ( std::size_t i=movedIndex; i<std::size(ref)-1; ++i )
                                    notifyElementMoved(user, Route{keys}, i, i+1);
                            }
                            if constexpr ( hasSelections && hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::MoveBottomN:
                {
                    if constexpr ( hasMoveOps )
                    {
                        auto count = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto movedIndexes = readIndexes<index_type>(offset, count);

                        std::size_t size = std::size(ref);
                        std::size_t maximumIndexMoved = size;
                        for ( std::size_t i=0; i<count; ++i )
                        {
                            if ( static_cast<std::size_t>(movedIndexes[i])+1 == maximumIndexMoved )
                                --maximumIndexMoved;
                            else
                                break;
                        }

                        std::size_t countValid = 0;
                        for ( auto i : movedIndexes )
                        {
                            if ( static_cast<std::size_t>(i)+1 < maximumIndexMoved )
                                ++countValid;
                        }

                        std::vector<std::size_t> trackedIndexes {};
                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            trackedIndexes.assign(size, 0);
                            std::iota(trackedIndexes.begin(), trackedIndexes.end(), 0);
                        }

                        std::size_t i = maximumIndexMoved-countValid;
                        for ( auto it = movedIndexes.rbegin(); it != movedIndexes.rend(); ++it )
                        {
                            if ( static_cast<std::size_t>(*it)+1 < maximumIndexMoved )
                            {
                                auto toMove = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                auto dest = std::next(ref.begin(), static_cast<std::ptrdiff_t>(*it));
                                std::rotate(dest, toMove, toMove+1);
                                if constexpr ( hasElementMovedOp<Route> )
                                    mirrorRotationToIndexes(trackedIndexes, *it, i, i+1);
                                ++i;
                            }
                        }

                        if constexpr ( hasSelections )
                        {
                            auto & sel = getSelections<Pathway...>();
                            sel_type prevSel {};
                            readSelections(events, offset, prevSel);
                            std::swap(sel, prevSel);
                        }

                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            for ( i=0; i<size; ++i )
                            {
                                if ( i != trackedIndexes[i] )
                                    notifyElementMoved(user, Route{keys}, trackedIndexes[i], i);
                            }
                        }
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::MoveBottomL:
                {
                    if constexpr ( hasSelections && hasMoveOps )
                    {
                        auto & sel = getSelections<Pathway...>();
                        sel_type prevSel {};
                        readSelections(events, offset, prevSel);

                        auto movedIndexes = prevSel; // Copy so it can be sorted without changing selections
                        std::sort(movedIndexes.begin(), movedIndexes.end());

                        std::size_t size = std::size(ref);
                        std::vector<std::size_t> trackedIndexes {};
                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            trackedIndexes.assign(size, 0);
                            std::iota(trackedIndexes.begin(), trackedIndexes.end(), 0);
                        }

                        std::size_t toMoveIndex = std::size(ref)-std::size(prevSel);
                        for ( auto i : movedIndexes )
                        {
                            if ( static_cast<std::size_t>(i)+1 < size )
                            {
                                auto toMove = std::next(ref.begin(), static_cast<std::ptrdiff_t>(toMoveIndex));
                                auto dest = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                std::rotate(dest, toMove, toMove+1);
                                if constexpr ( hasElementMovedOp<Route> )
                                    mirrorRotationToIndexes(trackedIndexes, i, toMoveIndex, toMoveIndex+1);
                                ++toMoveIndex;
                            }
                        }

                        std::swap(sel, prevSel);

                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            for ( std::size_t i=0; i<size; ++i )
                            {
                                if ( i != trackedIndexes[i] )
                                    notifyElementMoved(user, Route{keys}, trackedIndexes[i], i);
                            }
                        }
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::MoveTo:
                {
                    if constexpr ( hasMoveOps )
                    {
                        auto indexMovedTo = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto movedIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                            
                        auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(movedIndex));
                        auto target = std::next(ref.begin(), static_cast<std::ptrdiff_t>(indexMovedTo));
                        if ( indexMovedTo < movedIndex && movedIndex < std::size(ref) )
                        {
                            std::rotate(target, target+1, it+1);
                            mirrorRotationToSelection(getSelections<Pathway...>(), indexMovedTo, indexMovedTo+1, movedIndex+1);
                            if constexpr ( hasElementMovedOp<Route> )
                            {
                                notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(indexMovedTo), static_cast<std::size_t>(movedIndex));
                                for ( std::size_t i=static_cast<std::size_t>(indexMovedTo)+1; i<static_cast<std::size_t>(movedIndex)+1; ++i )
                                    notifyElementMoved(user, Route{keys}, i, i-1);
                            }
                            if constexpr ( hasSelections && hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                        else if ( indexMovedTo > movedIndex && indexMovedTo < std::size(ref) )
                        {
                            std::rotate(it, target, target+1);
                            mirrorRotationToSelection(getSelections<Pathway...>(), movedIndex, indexMovedTo, indexMovedTo+1);
                            if constexpr ( hasElementMovedOp<Route> )
                            {
                                notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(indexMovedTo), static_cast<std::size_t>(movedIndex));
                                for ( std::size_t i=static_cast<std::size_t>(movedIndex); i<static_cast<std::size_t>(indexMovedTo); ++i )
                                    notifyElementMoved(user, Route{keys}, i, i+1);
                            }
                            if constexpr ( hasSelections && hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::MoveToN:
                {
                    if constexpr ( hasMoveOps )
                    {
                        // Calculate the rotates you would perform in the forward direction, then reverse the rotations
                        std::vector<Rotation> rotations {};
                            
                        auto indexMovedTo = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto countMovedIndexes = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto movedIndexes = readIndexes<index_type>(offset, countMovedIndexes);

                        auto size = std::size(ref);
                        if ( !std::empty(movedIndexes) )
                        {
                            std::size_t countValidIndexes = 0;
                            for ( auto movedIndex : movedIndexes )
                            {
                                if ( movedIndex >= 0 && static_cast<std::size_t>(movedIndex) < std::size(ref) )
                                    ++countValidIndexes;
                            }
                            if ( countValidIndexes == 0 )
                                return;

                            if ( static_cast<std::size_t>(indexMovedTo) + countValidIndexes >= size ) // Operation collapses to moveToEnd
                            {
                                std::ptrdiff_t insertionIndex = static_cast<std::ptrdiff_t>(size);
                                for ( auto mit = movedIndexes.rbegin(); mit != movedIndexes.rend(); ++mit ) // movedIndexes sorted least to greatest, so reverse read
                                {
                                    auto i = *mit;
                                    if ( i >= 0 && static_cast<std::ptrdiff_t>(i)+1 < insertionIndex )
                                        rotations.push_back(Rotation::makeReverse(static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertionIndex)));

                                    --insertionIndex;
                                }
                            }
                            else // Non-collapsed moveTo operation
                            {
                                std::size_t minValidIndex = 0;
                                std::size_t maxValidIndex = 0;
                                for ( std::size_t i=0; i<std::size(movedIndexes); ++i )
                                {
                                    if ( static_cast<std::ptrdiff_t>(movedIndexes[i]) >= 0 )
                                    {
                                        minValidIndex = static_cast<std::size_t>(movedIndexes[i]);
                                        break;
                                    }
                                }
                                for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(std::size(movedIndexes))-1; i>=0; --i )
                                {
                                    if ( static_cast<std::ptrdiff_t>(movedIndexes[static_cast<std::size_t>(i)]) < static_cast<std::ptrdiff_t>(size) )
                                    {
                                        maxValidIndex = static_cast<std::size_t>(movedIndexes[static_cast<std::size_t>(i)]);
                                        break;
                                    }
                                }

                                std::size_t leftChunkSize = minValidIndex < static_cast<std::size_t>(indexMovedTo) ? 1 : 0;
                                std::size_t rightChunkSize = maxValidIndex >= static_cast<std::size_t>(indexMovedTo) ? 1 : 0;
                                std::size_t leftChunkFirst = minValidIndex;
                                std::size_t rightChunkFirst = maxValidIndex;

                                for ( std::size_t i=0; i+1<std::size(movedIndexes) && static_cast<std::size_t>(movedIndexes[i+1]) < indexMovedTo; ++i )
                                {
                                    // rotate left chunk together with element[movedIndexes[i+1]]
                                    rotations.push_back(Rotation::makeReverse(leftChunkFirst, leftChunkFirst+leftChunkSize, movedIndexes[i+1]));
                                    leftChunkFirst = static_cast<std::size_t>(movedIndexes[i+1])-leftChunkSize;
                                    ++leftChunkSize;
                                }
                                for ( std::size_t i=std::size(movedIndexes)-1; i>0 && static_cast<std::size_t>(movedIndexes[i-1]) >= indexMovedTo; --i )
                                {
                                    // rotate right chunk together with element[movedIndexes[i-1]]
                                    rotations.push_back(Rotation::makeReverse(static_cast<std::size_t>(movedIndexes[i-1])+1, rightChunkFirst, rightChunkFirst+rightChunkSize));
                                    rightChunkFirst = static_cast<std::size_t>(movedIndexes[i-1]);
                                    ++rightChunkSize;
                                }
                                if ( leftChunkFirst+leftChunkSize != rightChunkFirst && leftChunkSize > 0 && rightChunkSize > 0 ) // Need to combine the chunks then rotate combined to final position
                                {
                                    std::size_t leftDistance = leftChunkFirst < static_cast<std::size_t>(indexMovedTo) ? static_cast<std::size_t>(indexMovedTo)-leftChunkFirst : leftChunkFirst-static_cast<std::size_t>(indexMovedTo);
                                    std::size_t rightDistance = rightChunkFirst < static_cast<std::size_t>(indexMovedTo)+leftChunkSize ? static_cast<std::size_t>(indexMovedTo)+leftChunkSize-rightChunkFirst : rightChunkFirst-static_cast<std::size_t>(indexMovedTo)-leftChunkSize;
                                    if ( rightDistance < leftDistance ) // Right chunk closer to target, move left up to right
                                    {
                                        rotations.push_back(Rotation::makeReverse(leftChunkFirst, leftChunkFirst+leftChunkSize, rightChunkFirst));
                                        leftChunkFirst = rightChunkFirst-leftChunkSize;
                                    }
                                    else // Left chunk closer to target, move right up to left
                                    {
                                        rotations.push_back(Rotation::makeReverse(leftChunkFirst+leftChunkSize, rightChunkFirst, rightChunkFirst+rightChunkSize));
                                        rightChunkFirst = leftChunkFirst+leftChunkSize;
                                    }
                                }
                                if ( leftChunkFirst < static_cast<std::size_t>(indexMovedTo) ) // Rotate combined chunk rightwards to final position
                                    rotations.push_back(Rotation::makeReverse(leftChunkFirst, leftChunkFirst+countValidIndexes, indexMovedTo+leftChunkSize+rightChunkSize));
                                else if ( leftChunkFirst > static_cast<std::size_t>(indexMovedTo) ) // Rotate combined chunk leftwards to final position
                                    rotations.push_back(Rotation::makeReverse(indexMovedTo, leftChunkFirst, leftChunkFirst+countValidIndexes));
                            }
                        }

                        std::vector<std::size_t> trackedIndexes {};
                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            trackedIndexes.assign(size, 0);
                            std::iota(trackedIndexes.begin(), trackedIndexes.end(), 0);
                        }
                        for ( auto it = rotations.rbegin(); it != rotations.rend(); ++it )
                        {
                            it->perform(ref);
                            if constexpr ( hasElementMovedOp<Route> )
                                it->perform(trackedIndexes);
                        }

                        if constexpr ( hasSelections )
                        {
                            auto & sel = getSelections<Pathway...>();
                            sel_type prevSel {};
                            readSelections(events, offset, prevSel);
                            std::swap(sel, prevSel);
                        }
                    
                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            for ( std::size_t i=0; i<size; ++i )
                            {
                                if ( i != trackedIndexes[i] )
                                    notifyElementMoved(user, Route{keys}, trackedIndexes[i], i);
                            }
                        }
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::MoveToL:
                {
                    if constexpr ( hasSelections && hasMoveOps )
                    {
                        // Calculate the rotates you would perform in the forward direction, then reverse the rotations
                        std::vector<Rotation> rotations {};
                            
                        auto indexMovedTo = static_cast<std::size_t>(readIndex<index_type>(offset));
                            
                        auto & sel = getSelections<Pathway...>();
                        sel_type prevSel {};
                        readSelections(events, offset, prevSel);
                        auto movedIndexes = prevSel; // Copy so it can be sorted without changing selections
                        std::sort(movedIndexes.begin(), movedIndexes.end());

                        auto size = std::size(ref);
                        if ( !std::empty(movedIndexes) )
                        {
                            std::size_t countValidIndexes = 0;
                            for ( auto movedIndex : movedIndexes )
                            {
                                if ( movedIndex >= 0 && static_cast<std::size_t>(movedIndex) < std::size(ref) )
                                    ++countValidIndexes;
                            }
                            if ( countValidIndexes == 0 )
                                return;

                            if ( static_cast<std::size_t>(indexMovedTo) + countValidIndexes >= size ) // Operation collapses to moveToEnd
                            {
                                std::ptrdiff_t insertionIndex = static_cast<std::ptrdiff_t>(size);
                                for ( auto mit = movedIndexes.rbegin(); mit != movedIndexes.rend(); ++mit ) // movedIndexes sorted least to greatest, so reverse read
                                {
                                    auto i = *mit;
                                    if ( i >= 0 && static_cast<std::ptrdiff_t>(i)+1 < insertionIndex )
                                        rotations.push_back(Rotation::makeReverse(i, static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertionIndex)));

                                    --insertionIndex;
                                }
                            }
                            else // Non-collapsed moveTo operation
                            {
                                std::size_t minValidIndex = 0;
                                std::size_t maxValidIndex = 0;
                                for ( std::size_t i=0; i<std::size(movedIndexes); ++i )
                                {
                                    if ( movedIndexes[i] >= 0 )
                                    {
                                        minValidIndex = static_cast<std::size_t>(movedIndexes[i]);
                                        break;
                                    }
                                }
                                for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(std::size(movedIndexes))-1; i>=0; --i )
                                {
                                    if ( static_cast<std::ptrdiff_t>(movedIndexes[static_cast<std::size_t>(i)]) < static_cast<std::ptrdiff_t>(size) )
                                    {
                                        maxValidIndex = static_cast<std::size_t>(movedIndexes[static_cast<std::size_t>(i)]);
                                        break;
                                    }
                                }

                                std::size_t leftChunkSize = minValidIndex < static_cast<std::size_t>(indexMovedTo) ? 1 : 0;
                                std::size_t rightChunkSize = maxValidIndex >= static_cast<std::size_t>(indexMovedTo) ? 1 : 0;
                                std::size_t leftChunkFirst = minValidIndex;
                                std::size_t rightChunkFirst = maxValidIndex;

                                for ( std::size_t i=0; i+1<std::size(movedIndexes) && movedIndexes[i+1] < indexMovedTo; ++i )
                                {
                                    // rotate left chunk together with element[movedIndexes[i+1]]
                                    rotations.push_back(Rotation::makeReverse(leftChunkFirst, leftChunkFirst+leftChunkSize, movedIndexes[i+1]));
                                    leftChunkFirst = static_cast<std::size_t>(movedIndexes[i+1])-leftChunkSize;
                                    ++leftChunkSize;
                                }
                                for ( std::size_t i=std::size(movedIndexes)-1; i>0 && movedIndexes[i-1] >= indexMovedTo; --i )
                                {
                                    // rotate right chunk together with element[movedIndexes[i-1]]
                                    rotations.push_back(Rotation::makeReverse(static_cast<std::size_t>(movedIndexes[i-1])+1, rightChunkFirst, rightChunkFirst+rightChunkSize));
                                    rightChunkFirst = static_cast<std::size_t>(movedIndexes[i-1]);
                                    ++rightChunkSize;
                                }
                                if ( leftChunkFirst+leftChunkSize != rightChunkFirst && leftChunkSize > 0 && rightChunkSize > 0 ) // Need to combine the chunks then rotate combined to final position
                                {
                                    std::size_t leftDistance = leftChunkFirst < static_cast<std::size_t>(indexMovedTo) ? static_cast<std::size_t>(indexMovedTo)-leftChunkFirst : leftChunkFirst-static_cast<std::size_t>(indexMovedTo);
                                    std::size_t rightDistance = rightChunkFirst < static_cast<std::size_t>(indexMovedTo)+leftChunkSize ? static_cast<std::size_t>(indexMovedTo)+leftChunkSize-rightChunkFirst : rightChunkFirst-static_cast<std::size_t>(indexMovedTo)-leftChunkSize;
                                    if ( rightDistance < leftDistance ) // Right chunk closer to target, move left up to right
                                    {
                                        rotations.push_back(Rotation::makeReverse(leftChunkFirst, leftChunkFirst+leftChunkSize, rightChunkFirst));
                                        leftChunkFirst = rightChunkFirst-leftChunkSize;
                                    }
                                    else // Left chunk closer to target, move right up to left
                                    {
                                        rotations.push_back(Rotation::makeReverse(leftChunkFirst+leftChunkSize, rightChunkFirst, rightChunkFirst+rightChunkSize));
                                        rightChunkFirst = leftChunkFirst+leftChunkSize;
                                    }
                                }
                                if ( leftChunkFirst < static_cast<std::size_t>(indexMovedTo) ) // Rotate combined chunk rightwards to final position
                                    rotations.push_back(Rotation::makeReverse(leftChunkFirst, leftChunkFirst+countValidIndexes, indexMovedTo+leftChunkSize+rightChunkSize));
                                else if ( leftChunkFirst > static_cast<std::size_t>(indexMovedTo) ) // Rotate combined chunk leftwards to final position
                                    rotations.push_back(Rotation::makeReverse(indexMovedTo, leftChunkFirst, leftChunkFirst+countValidIndexes));
                            }
                        }

                        std::vector<std::size_t> trackedIndexes {};
                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            trackedIndexes.assign(size, 0);
                            std::iota(trackedIndexes.begin(), trackedIndexes.end(), 0);
                        }
                        for ( auto it = rotations.rbegin(); it != rotations.rend(); ++it )
                        {
                            it->perform(ref);
                            if constexpr ( hasElementMovedOp<Route> )
                                it->perform(trackedIndexes);
                        }

                        std::swap(sel, prevSel);
                    
                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            for ( std::size_t i=0; i<size; ++i )
                            {
                                if ( i != trackedIndexes[i] )
                                    notifyElementMoved(user, Route{keys}, trackedIndexes[i], i);
                            }
                        }
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
            }
        }

        template <class value_type, class Member, class ... Pathway>
        void processRedoEvent(std::uint8_t op, std::size_t & offset, std::optional<std::size_t> & secondaryOffset, auto & ref, auto keys)
        {
            using index_type = index_type_t<default_index_type, Member>;
            using path_pack = type_tags<Pathway...>;
            using sel_type = std::remove_cvref_t<decltype(getSelections<Pathway...>())>;
            using element_type = RareTs::element_type_t<value_type>;
            using Route = PathTaggedKeys<decltype(keys), path_pack, EditorType>;
            using ElemPath = type_tags<Pathway..., PathIndex<std::tuple_size_v<std::remove_cvref_t<decltype(keys)>>>>;
            using ElemKeys = std::remove_cvref_t<decltype(std::tuple_cat(keys, std::tuple<index_type>{0}))>;
            using ElemRoute = PathTaggedKeys<ElemKeys, ElemPath, EditorType>;
            constexpr bool hasSelections = !std::is_null_pointer_v<sel_type> && RareTs::is_specialization_v<sel_type, std::vector>;
            constexpr bool hasSelChangeOp = hasSelections && hasSelectionsChangedOp<Route>;
            constexpr bool isIterable = RareTs::is_iterable_v<value_type>;
            constexpr bool hasMoveOps = !std::is_void_v<element_type> && requires { ref.begin(); };

            switch ( Op(op) )
            {
                case Op::Reset:
                {
                    if constexpr ( isIterable && hasElementRemovedOp<Route> ) // Iterable
                    {
                        std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1;
                        ref = {};
                        for ( ; i>=0; --i )
                            notifyElementRemoved(user, Route{keys}, static_cast<std::size_t>(i)); // Issue remove changes
                    }
                    else if constexpr ( !isIterable && hasValueChangedOp<Route, value_type> ) // Non-iterables
                    {
                        if constexpr ( std::is_array_v<typename Member::type> )
                        {
                            typename Member::type prevValue {};
                            auto refArray = as_1d<typename Member::type>(ref);
                            as_1d(prevValue) = refArray;
                            refArray.clear();
                            notifyValueChanged(user, Route{keys}, prevValue, ref); // Issue change notification
                        }
                        else
                        {
                            auto prevValue = ref;
                            ref = {};
                            notifyValueChanged(user, Route{keys}, prevValue, ref); // Issue change notification
                        }
                    }
                    else
                    {
                        if constexpr ( std::is_array_v<value_type> && requires { ref[0]; } )
                            as_1d<value_type>(ref).clear();
                        else
                            ref = {};
                    }
                            
                    if constexpr ( hasSelections )
                    {
                        auto & sel = getSelections<Pathway...>();
                        if constexpr ( hasSelChangeOp )
                        {
                            if ( !std::empty(sel) )
                            {
                                clearSel(sel);
                                notifySelectionsChanged(user, Route{keys});
                            }
                        }
                        else
                            clearSel(sel);
                    }
                }
                break;
                case Op::Reserve:
                {
                    if constexpr ( RareTs::is_specialization_v<value_type, std::vector> )
                        ref.reserve(static_cast<std::size_t>(readIndex<index_type>(offset)));
                }
                break;
                case Op::Trim:
                {
                    if constexpr ( RareTs::is_specialization_v<value_type, std::vector> )
                        ref.shrink_to_fit();
                }
                break;
                case Op::Assign:
                {
                    if constexpr ( !std::is_void_v<element_type> && !RareTs::is_static_array_v<value_type> && !std::is_same_v<std::string, value_type> && requires{ref.assign(0, {});} )
                    {
                        std::size_t count = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto value = readValue<element_type, Member>(offset);

                        if constexpr ( hasElementRemovedOp<Route> )
                        {
                            std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1;
                            ref.assign(count, value);
                            for ( ; i>=0; --i )
                                notifyElementRemoved(user, Route{keys}, static_cast<std::size_t>(i)); // Issue remove changes
                        }
                        else
                            ref.assign(count, value);

                        if constexpr ( hasElementAddedOp<Route> )
                        {
                            for ( std::size_t i=0; i<std::size(ref); ++i )
                                notifyElementAdded(user, Route{keys}, i);
                        }

                        if constexpr ( hasSelections )
                        {
                            auto & sel = getSelections<Pathway...>();
                            writeSelections(events, sel);
                            if constexpr ( hasSelChangeOp )
                            {
                                if ( !std::empty(sel) )
                                {
                                    clearSel(sel);
                                    notifySelectionsChanged(user, Route{keys});
                                }
                            }
                            else
                                clearSel(sel);
                        }
                    }
                }
                break;
                case Op::AssignDefault:
                {
                    if constexpr ( !std::is_void_v<element_type> && !RareTs::is_static_array_v<value_type> && !std::is_same_v<std::string, value_type> && requires{ref = value_type(0);} )
                    {
                        std::size_t size = static_cast<std::size_t>(readIndex<index_type>(offset));
                        if constexpr ( hasElementRemovedOp<Route> )
                        {
                            std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1;
                            ref = value_type(size);
                            for ( ; i>=0; --i )
                                notifyElementRemoved(user, Route{keys}, static_cast<std::size_t>(i)); // Issue remove changes
                        }
                        else
                            ref = value_type(size);

                        if constexpr ( hasElementAddedOp<Route> )
                        {
                            for ( std::size_t i=0; i<std::size(ref); ++i )
                                notifyElementAdded(user, Route{keys}, i);
                        }

                        if constexpr ( hasSelections )
                        {
                            auto & sel = getSelections<Pathway...>();
                            writeSelections(events, sel);
                            if constexpr ( hasSelChangeOp )
                            {
                                if ( !std::empty(sel) )
                                {
                                    clearSel(sel);
                                    notifySelectionsChanged(user, Route{keys});
                                }
                            }
                            else
                                clearSel(sel);
                        }

                    }
                }
                break;
                case Op::ClearSelections:
                {
                    auto & sel = getSelections<Pathway...>();
                    clearSel(sel);

                    if constexpr ( hasSelChangeOp )
                        notifySelectionsChanged(user, Route{keys});
                }
                break;
                case Op::SelectAll:
                {
                    if constexpr ( hasSelections && requires { std::size(ref); } )
                    {
                        auto & sel = getSelections<Pathway...>();
                        RareTs::clear(sel);
                        sel.assign(std::size(ref), index_type{0});
                        std::iota(sel.begin(), sel.end(), index_type{0});

                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::Select:
                {
                    if constexpr ( hasSelections )
                    {
                        auto & sel = getSelections<Pathway...>();
                        auto value = readIndex<index_type>(offset);
                        if ( std::find(sel.begin(), sel.end(), value) == sel.end() )
                            sel.push_back(value);
                        
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::SelectN:
                {
                    if constexpr ( hasSelections )
                    {
                        auto & sel = getSelections<Pathway...>();
                        auto size = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto selIndexes = readIndexes<index_type>(offset, size);
                        for ( std::size_t i=0; i<size; ++i )
                        {
                            if ( std::find(sel.begin(), sel.end(), selIndexes[i]) == sel.end() )
                                sel.push_back(selIndexes[i]);
                        }
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::Deselect:
                {
                    if constexpr ( hasSelections )
                    {
                        auto & sel = getSelections<Pathway...>();
                        auto value = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto found = std::find(sel.begin(), sel.end(), value);
                        if ( found != sel.end() )
                            sel.erase(found);
                        
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::DeselectN:
                {
                    if constexpr ( hasSelections )
                    {
                        auto & sel = getSelections<Pathway...>();
                        auto size = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto selIndexes = readIndexes<index_type>(offset, size);
                        for ( std::size_t i=0; i<size; ++i )
                        {
                            auto found = std::find(sel.begin(), sel.end(), selIndexes[i]);
                            if ( found != sel.end() )
                                sel.erase(found);
                        }
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::ToggleSelection:
                {
                    if constexpr ( hasSelections )
                    {
                        auto & sel = getSelections<Pathway...>();
                        auto value = readIndex<index_type>(offset);
                        readIndex<index_type>(offset);
                        auto found = std::find(sel.begin(), sel.end(), value);
                        if ( found == sel.end() )
                            RareTs::append(sel, value);
                        else
                            sel.erase(found);
                        
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::ToggleSelectionN:
                {
                    if constexpr ( hasSelections )
                    {
                        auto & sel = getSelections<Pathway...>();
                        auto size = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto selIndexes = readIndexes<index_type>(offset, size);
                        for ( std::size_t i=0; i<size; ++i )
                        {
                            auto found = std::find(sel.begin(), sel.end(), selIndexes[i]);
                            if ( found == sel.end() )
                                RareTs::append(sel, selIndexes[i]);
                            else
                                sel.erase(found);
                        }
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::SortSelections:
                {
                    if constexpr ( hasSelections && requires{redoSort(getSelections<Pathway...>(), readIndexes<index_type>(offset, 0));} )
                    {
                        auto & sel = getSelections<Pathway...>();
                        auto size = readIndex<index_type>(offset);
                        auto sourceIndexes = readIndexes<index_type>(offset, size);

                        redoSort(sel, sourceIndexes);

                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::SortSelectionsDesc:
                {
                    if constexpr ( hasSelections && requires{redoSort(getSelections<Pathway...>(), readIndexes<index_type>(offset, 0));} )
                    {
                        auto & sel = getSelections<Pathway...>();
                        auto size = readIndex<index_type>(offset);
                        auto sourceIndexes = readIndexes<index_type>(offset, size);

                        redoSort(sel, sourceIndexes);

                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::Set:
                {
                    if constexpr ( RareTs::is_assignable_v<decltype(ref), value_type> )
                    {
                        auto newValue = readValue<value_type, Member>(offset);
                        readValue<value_type, Member>(offset); // prevValue (unused)
                        if constexpr ( !isIterable && hasValueChangedOp<Route, value_type> )
                        {
                            auto temp = ref;
                            ref = newValue;
                            notifyValueChanged(user, Route{keys}, temp, ref);
                        }
                        else
                        {
                            if constexpr ( isIterable && hasElementRemovedOp<Route> )
                            {
                                std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1;
                                ref = newValue;
                                for ( ; i>=0; --i )
                                    notifyElementRemoved(user, Route{keys}, static_cast<std::size_t>(i));
                            }
                            else
                                ref = newValue;

                            if constexpr ( isIterable && hasElementAddedOp<Route> )
                            {
                                for ( std::size_t i=0; i<std::size(ref); ++i )
                                    notifyElementAdded(user, Route{keys}, i);
                            }
                        }

                        if constexpr ( hasSelections )
                        {
                            clearSel(getSelections<Pathway...>());
                            if constexpr ( hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::SetN:
                {
                    if constexpr ( !std::is_void_v<element_type> && requires{ref[0] = std::declval<element_type>();} )
                    {
                        constexpr bool isIterableElement = RareTs::is_iterable_v<element_type>;
                        auto count = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto setIndexes = readIndexes<index_type>(offset, count);

                        auto newValue = readValue<element_type, Member>(offset);

                        if constexpr ( !isIterableElement && hasValueChangedOp<Route, element_type> )
                        {
                            for ( auto index : setIndexes )
                            {
                                auto prevValue = ref[index];
                                ref[index] = newValue;
                                notifyValueChanged(user, ElemRoute{std::tuple_cat(keys, std::tuple<index_type>{index})}, prevValue, ref[index]);
                            }
                        }
                        else if constexpr ( isIterableElement && hasElementRemovedOp<Route> )
                        {
                            for ( auto index : setIndexes )
                            {
                                std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref[index]))-1;
                                ref[index] = newValue;
                                for ( ; i>=0; --i )
                                    notifyElementRemoved(user, ElemRoute{std::tuple_cat(keys, std::tuple<index_type>{index})}, static_cast<std::size_t>(i));
                                if constexpr ( hasElementAddedOp<Route> )
                                {
                                    for ( std::size_t j=0; j<std::size(ref[index]); ++j )
                                        notifyElementAdded(user, ElemRoute{std::tuple_cat(keys, std::tuple<index_type>{index})}, j);
                                }
                            }
                        }
                        else
                        {
                            for ( auto index : setIndexes )
                            {
                                ref[index] = newValue;
                                if constexpr ( isIterableElement && hasElementAddedOp<Route> )
                                {
                                    for ( std::size_t i=0; i<std::size(ref[index]); ++i )
                                        notifyElementAdded(user, ElemRoute{std::tuple_cat(keys, std::tuple<index_type>{index})}, i);
                                }
                            }
                        }
                    }
                }
                break;
                case Op::SetL:
                {
                    if constexpr ( RareTs::is_assignable_v<decltype(ref), value_type> )
                    {
                        if ( !secondaryOffset )
                            secondaryOffset = offset; // Is set exclusively for the first visit of the selection

                        if constexpr ( hasValueChangedOp<Route, value_type> )
                        {
                            auto temp = ref;
                            ref = peekValue<value_type, Member>(*secondaryOffset);
                            notifyValueChanged(user, Route{keys}, temp, ref);
                        }
                        else
                        {
                            if constexpr ( isIterable && hasElementRemovedOp<Route> )
                            {
                                std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1;
                                ref = peekValue<value_type, Member>(*secondaryOffset);
                                for ( ; i>=0; --i )
                                    notifyElementRemoved(user, Route{keys}, static_cast<std::size_t>(i));
                            }
                            else
                                ref = peekValue<value_type, Member>(*secondaryOffset);

                            if constexpr ( isIterable && hasElementAddedOp<Route> )
                            {
                                for ( std::size_t i=0; i<std::size(ref); ++i )
                                    notifyElementAdded(user, Route{keys}, i);
                            }
                        }
                    }
                }
                break;
                case Op::Append:
                {
                    if constexpr ( RareTs::is_specialization_v<value_type, std::vector> )
                    {
                        auto value = readValue<element_type, Member>(offset);
                        ref.push_back(value);
                        if constexpr ( hasElementAddedOp<Route> )
                            notifyElementAdded(user, Route{keys}, ref.size()-1);
                    }
                }
                break;
                case Op::AppendN:
                {
                    if constexpr ( !std::is_void_v<element_type> && !RareTs::is_static_array_v<value_type> && !std::is_same_v<std::string, value_type> && requires{ref.push_back(element_type{});} )
                    {
                        auto count = static_cast<std::size_t>(readIndex<index_type>(offset));
                        for ( std::size_t i=0; i<count; ++i )
                        {
                            ref.push_back(readValue<element_type, Member>(offset));
                            if constexpr ( hasElementAddedOp<Route> )
                                notifyElementAdded(user, Route{keys}, ref.size()-1);
                        }
                    }
                }
                break;
                case Op::Insert:
                {
                    if constexpr ( !std::is_void_v<element_type> && !RareTs::is_static_array_v<value_type> && !std::is_same_v<std::string, value_type> && requires{ref.insert(ref.begin(), std::declval<element_type>());} )
                    {
                        auto insertionIndex = readIndex<index_type>(offset);
                        auto insertedValue = readValue<element_type, Member>(offset);
                        ref.insert(std::next(ref.begin(), static_cast<std::ptrdiff_t>(insertionIndex)), insertedValue);

                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(std::size(ref))-1; i>static_cast<std::ptrdiff_t>(insertionIndex); --i )
                                notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(i-1), static_cast<std::size_t>(i));
                        }
                        if constexpr ( hasElementAddedOp<Route> )
                            notifyElementAdded(user, Route{keys}, static_cast<std::size_t>(insertionIndex));

                        if constexpr ( hasSelections )
                        {
                            auto & sel = getSelections<Pathway...>();
                            for ( auto & index : sel )
                            {
                                if ( static_cast<index_type>(index)  >= insertionIndex )
                                    ++index;
                            }
                            if constexpr ( hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::InsertN:
                {
                    if constexpr ( !std::is_void_v<element_type> && !RareTs::is_static_array_v<value_type> && !std::is_same_v<std::string, value_type> && requires {ref.insert(ref.begin(), std::declval<element_type>());} )
                    {
                        std::size_t insertionCount = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto insertionIndex = readIndex<index_type>(offset);
                        for ( std::size_t i=0; i<insertionCount; ++i )
                        {
                            ref.insert(
                                std::next(ref.begin(), static_cast<std::ptrdiff_t>(insertionIndex+i)),
                                readValue<element_type, Member>(offset)
                            );
                        }
                            
                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            std::size_t prevSize = std::size(ref) - insertionCount;
                            for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(prevSize)-1; i>=static_cast<std::ptrdiff_t>(insertionIndex); --i )
                                notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(i), static_cast<std::size_t>(i)+insertionCount);
                        }
                        if constexpr ( hasElementAddedOp<Route> )
                        {
                            auto limit = static_cast<std::size_t>(insertionIndex) + insertionCount;
                            for ( auto i = static_cast<std::size_t>(insertionIndex); i < limit; ++i )
                                notifyElementAdded(user, Route{keys}, i);
                        }

                        if constexpr ( hasSelections )
                        {
                            auto & sel = getSelections<Pathway...>();
                            for ( auto & index : sel )
                            {
                                if ( static_cast<index_type>(index) >= insertionIndex )
                                    index += static_cast<index_type>(insertionCount);
                            }
                            if constexpr ( hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::Remove:
                {
                    if constexpr ( !std::is_void_v<element_type> && !RareTs::is_static_array_v<value_type> && !std::is_same_v<std::string, value_type> && requires{ref.erase(ref.begin());} )
                    {
                        auto removalIndex = readIndex<index_type>(offset);
                        ref.erase(std::next(ref.begin(), static_cast<std::ptrdiff_t>(removalIndex)));
                        if constexpr ( hasElementRemovedOp<Route> )
                            notifyElementRemoved(user, Route{keys}, static_cast<std::size_t>(removalIndex));

                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            for ( std::size_t i=static_cast<std::size_t>(removalIndex); i<std::size(ref); ++i )
                                notifyElementMoved(user, Route{keys}, i+1, i);
                        }

                        if constexpr ( hasSelections )
                        {
                            auto & sel = getSelections<Pathway...>();
                            auto found = sel.end();
                            for ( auto it = sel.begin(); it != sel.end(); ++it )
                            {
                                if ( *it == removalIndex )
                                    found = it;
                                else if ( *it > removalIndex )
                                    --(*it);
                            }
                            if ( found != sel.end() )
                                sel.erase(found);

                            if constexpr ( hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::RemoveN:
                {
                    if constexpr ( !std::is_void_v<element_type> && !RareTs::is_static_array_v<value_type> && !std::is_same_v<std::string, value_type> && requires{ref.erase(ref.begin());} )
                    {
                        auto & sel = getSelections<Pathway...>();
                        std::size_t removalCount = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto removalIndexes = readIndexes<index_type>(offset, removalCount);
                        for ( std::size_t i=0; i<removalCount; ++i )
                        {
                            auto indexRemoved = removalIndexes[i];
                            ref.erase(std::next(ref.begin(), static_cast<std::ptrdiff_t>(indexRemoved)));
                            if constexpr ( hasSelections )
                            {
                                auto found = sel.end();
                                for ( auto it = sel.begin(); it != sel.end(); ++it )
                                {
                                    if ( *it == indexRemoved )
                                        found = it;
                                    else if ( *it > indexRemoved )
                                        --(*it);
                                }
                                if ( found != sel.end() )
                                    sel.erase(found);
                            }
                        }
                            
                        if constexpr ( hasElementRemovedOp<Route> )
                        {
                            for ( auto removalIndex : removalIndexes )
                                notifyElementRemoved(user, Route{keys}, static_cast<std::size_t>(removalIndex));
                        }
                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            if ( !ref.empty() )
                            {
                                std::size_t sizeRemoved = std::size(removalIndexes);
                                std::size_t collectionIndex = static_cast<std::size_t>(removalIndexes[sizeRemoved-1]);
                                std::size_t moveDistance = 1;
                                for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(sizeRemoved)-1; i>0; --i )
                                {
                                    for ( std::size_t j=removalIndexes[static_cast<std::size_t>(i)]; j<removalIndexes[static_cast<std::size_t>(i-1)]-1; ++j )
                                    {
                                        notifyElementMoved(user, Route{keys}, collectionIndex+moveDistance, collectionIndex);
                                        ++collectionIndex;
                                    }
                                    ++moveDistance;
                                }
                                for ( ; collectionIndex < std::size(ref); ++collectionIndex )
                                    notifyElementMoved(user, Route{keys}, collectionIndex+moveDistance, collectionIndex);
                            }
                        }
                        if constexpr ( hasSelections && hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::RemoveL:
                {
                    if constexpr ( !std::is_void_v<element_type> && !RareTs::is_static_array_v<value_type> && !std::is_same_v<std::string, value_type> && requires{ref.erase(ref.begin());} )
                    {
                        std::size_t removalCount = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto removalIndexes = readIndexes<index_type>(offset, removalCount);
                        for ( std::size_t i=0; i<removalCount; ++i )
                        {
                            ref.erase(std::next(ref.begin(), static_cast<std::ptrdiff_t>(removalIndexes[i])));
                            if constexpr ( hasElementRemovedOp<Route> )
                                notifyElementRemoved(user, Route{keys}, static_cast<std::size_t>(removalIndexes[i]));
                        }
                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            if ( !ref.empty() )
                            {
                                std::size_t sizeRemoved = std::size(removalIndexes);
                                std::size_t collectionIndex = static_cast<std::size_t>(removalIndexes[sizeRemoved-1]);
                                std::size_t moveDistance = 1;
                                for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(sizeRemoved)-1; i>0; --i )
                                {
                                    for ( std::size_t j=removalIndexes[static_cast<std::size_t>(i)];
                                        j<removalIndexes[static_cast<std::size_t>(i-1)]-1; ++j )
                                    {
                                        notifyElementMoved(user, Route{keys}, collectionIndex+moveDistance, collectionIndex);
                                        ++collectionIndex;
                                    }
                                    ++moveDistance;
                                }
                                for ( ; collectionIndex < std::size(ref); ++collectionIndex )
                                    notifyElementMoved(user, Route{keys}, collectionIndex+moveDistance, collectionIndex);
                            }
                        }

                        clearSel(getSelections<Pathway...>());
                        if constexpr ( hasSelections && hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::Sort:
                {
                    if constexpr ( RareTs::is_specialization_v<value_type, std::vector> )
                    {
                        std::size_t count = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto sourceIndexes = readIndexes<index_type>(offset, count);
                            
                        redoSort(ref, sourceIndexes);

                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( i != sourceIndexes[i] )
                                    notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(sourceIndexes[i]), static_cast<std::size_t>(i));
                            }
                        }

                        if constexpr ( hasSelections )
                        {
                            auto & sel = getSelections<Pathway...>();
                            sel_type newSel{};
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( std::find(sel.begin(), sel.end(), sourceIndexes[i]) != sel.end() )
                                    RareTs::append(newSel, static_cast<index_type>(i));
                            }

                            std::swap(sel, newSel);
                            if constexpr ( hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::SortDesc:
                {
                    if constexpr ( RareTs::is_specialization_v<value_type, std::vector> )
                    {
                        std::size_t count = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto sourceIndexes = readIndexes<index_type>(offset, count);
                            
                        redoSort(ref, sourceIndexes);

                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( i != sourceIndexes[i] )
                                    notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(sourceIndexes[i]), static_cast<std::size_t>(i));
                            }
                        }

                        if constexpr ( hasSelections )
                        {
                            auto & sel = getSelections<Pathway...>();
                            sel_type newSel {};
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( std::find(sel.begin(), sel.end(), sourceIndexes[i]) != sel.end() )
                                    RareTs::append(newSel, static_cast<index_type>(i));
                            }
                            std::swap(sel, newSel);
                            if constexpr ( hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::Swap:
                {
                    if constexpr ( hasMoveOps )
                    {
                        auto firstIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto secondIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                        if ( firstIndex != secondIndex && firstIndex < std::size(ref) && secondIndex < std::size(ref) )
                        {
                            std::swap(ref[firstIndex], ref[secondIndex]);
                            mirrorSwapToSelection(getSelections<Pathway...>(), firstIndex, secondIndex);
                            if constexpr ( hasElementMovedOp<Route> )
                            {
                                notifyElementMoved(user, Route{keys}, firstIndex, secondIndex);
                                notifyElementMoved(user, Route{keys}, secondIndex, firstIndex);
                            }
                            if constexpr ( hasSelections && hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::MoveUp:
                {
                    if constexpr ( hasMoveOps )
                    {
                        auto movedIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                        if ( movedIndex > 0 && movedIndex < std::size(ref) )
                        {
                            std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex-1)]);
                            mirrorSwapToSelection(getSelections<Pathway...>(), movedIndex, movedIndex-1);
                            if constexpr ( hasElementMovedOp<Route> )
                            {
                                notifyElementMoved(user, Route{keys}, movedIndex, movedIndex-1);
                                notifyElementMoved(user, Route{keys}, movedIndex-1, movedIndex);
                            }
                            if constexpr ( hasSelections && hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::MoveUpN:
                {
                    if constexpr ( hasMoveOps )
                    {
                        std::size_t nextAvailable = 0;
                        auto count = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto movedIndexes = readIndexes<index_type>(offset, count);
                        for ( std::size_t i=0; i<count; ++i )
                        {
                            auto movedIndex = static_cast<std::size_t>(movedIndexes[i]);
                            if ( movedIndex > nextAvailable && static_cast<std::size_t>(movedIndex) < std::size(ref) )
                            {
                                std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex-1)]);
                                mirrorSwapToSelection(getSelections<Pathway...>(), movedIndex, movedIndex-1);
                            }
                            else if ( movedIndex == nextAvailable )
                                ++nextAvailable;
                        }

                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            nextAvailable = 0;
                            std::size_t blockSize = 1;
                            for ( std::size_t i=0; i<std::size(movedIndexes); ++i )
                            {
                                if ( movedIndexes[i] > nextAvailable && static_cast<std::size_t>(movedIndexes[i]) < std::size(ref) )
                                {
                                    if ( i+1 < std::size(movedIndexes) && movedIndexes[i]+1 == movedIndexes[i+1] )
                                        ++blockSize;
                                    else
                                    {
                                        notifyElementMoved(user, Route{keys}, movedIndexes[i]-blockSize, movedIndexes[i]);
                                        blockSize = 1;
                                    }
                                    notifyElementMoved(user, Route{keys}, movedIndexes[i], movedIndexes[i]-1);
                                }
                                else if ( movedIndexes[i] == nextAvailable )
                                    ++nextAvailable;
                            }
                        }
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::MoveUpL:
                {
                    if constexpr ( hasSelections && hasMoveOps )
                    {
                        auto & sel = getSelections<Pathway...>();

                        auto movedIndexes = sel; // Copy so it can be sorted without changing selections
                        std::sort(movedIndexes.begin(), movedIndexes.end());
                
                        std::size_t nextAvailable = 0;
                        for ( auto movedIndex : movedIndexes )
                        {
                            if ( movedIndex > nextAvailable && static_cast<std::size_t>(movedIndex) < std::size(ref) )
                            {
                                std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex)-1]);
                                mirrorSwapToSelection(sel, movedIndex, static_cast<std::size_t>(movedIndex)-1);
                                nextAvailable = movedIndex;
                            }
                            else if ( movedIndex == nextAvailable )
                                ++nextAvailable;
                        }

                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            nextAvailable = 0;
                            std::size_t blockSize = 1;
                            for ( std::size_t i=0; i<std::size(movedIndexes); ++i )
                            {
                                if ( movedIndexes[i] > nextAvailable && static_cast<std::size_t>(movedIndexes[i]) < std::size(ref) )
                                {
                                    if ( i+1 < std::size(movedIndexes) && movedIndexes[i]+1 == movedIndexes[i+1] )
                                        ++blockSize;
                                    else
                                    {
                                        notifyElementMoved(user, Route{keys}, movedIndexes[i]-blockSize, movedIndexes[i]);
                                        blockSize = 1;
                                    }
                                    notifyElementMoved(user, Route{keys}, movedIndexes[i], movedIndexes[i]-1);
                                }
                                else if ( movedIndexes[i] == nextAvailable )
                                    ++nextAvailable;
                            }
                        }
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::MoveTop:
                {
                    if constexpr ( hasMoveOps )
                    {
                        auto movedIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                        if ( movedIndex > 0 && movedIndex < std::size(ref) )
                        {
                            auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(movedIndex));
                            std::rotate(ref.begin(), it, it+1);
                            mirrorRotationToSelection(getSelections<Pathway...>(), 0, movedIndex, movedIndex+1);
                            if constexpr ( hasElementMovedOp<Route> )
                            {
                                notifyElementMoved(user, Route{keys}, movedIndex, 0);
                                for ( std::size_t i=0; i<static_cast<std::size_t>(movedIndex); ++i )
                                    notifyElementMoved(user, Route{keys}, i, i+1);
                            }
                            if constexpr ( hasSelections && hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::MoveTopN:
                {
                    if constexpr ( hasMoveOps )
                    {
                        auto & sel = getSelections<Pathway...>();
                        auto count = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto movedIndexes = readIndexes<index_type>(offset, count);

                        std::size_t size = std::size(ref);
                        std::vector<std::size_t> trackedIndexes {};
                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            trackedIndexes.assign(size, 0);
                            std::iota(trackedIndexes.begin(), trackedIndexes.end(), 0);
                        }

                        std::ptrdiff_t insertionIndex = 0;
                        for ( auto i : movedIndexes )
                        {
                            if ( static_cast<std::ptrdiff_t>(i) > insertionIndex && static_cast<std::size_t>(i) < size )
                            {
                                auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                std::rotate(std::next(ref.begin(), insertionIndex), it, it+1);
                                mirrorRotationToSelection(sel, static_cast<std::size_t>(insertionIndex), static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1);
                                if constexpr ( hasElementMovedOp<Route> )
                                    mirrorRotationToIndexes(trackedIndexes, insertionIndex, i, i+1);
                            }
                            ++insertionIndex;
                        }

                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            for ( std::size_t i=0; i<size; ++i )
                            {
                                if ( i != trackedIndexes[i] )
                                    notifyElementMoved(user, Route{keys}, trackedIndexes[i], i);
                            }
                        }
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::MoveTopL:
                {
                    if constexpr ( hasSelections && hasMoveOps )
                    {
                        auto & sel = getSelections<Pathway...>();
                        sel_type prevSel {};
                        readSelections(events, offset, prevSel);
                        std::sort(prevSel.begin(), prevSel.end());

                        std::size_t size = std::size(ref);
                        std::vector<std::size_t> trackedIndexes {};
                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            trackedIndexes.assign(size, 0);
                            std::iota(trackedIndexes.begin(), trackedIndexes.end(), 0);
                        }

                        std::ptrdiff_t insertionIndex = 0;
                        for ( auto i : prevSel )
                        {
                            if ( static_cast<std::ptrdiff_t>(i) > insertionIndex && static_cast<std::size_t>(i) < size )
                            {
                                auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                std::rotate(std::next(ref.begin(), insertionIndex), it, it+1);
                                mirrorRotationToSelection(sel, static_cast<std::size_t>(insertionIndex), i, static_cast<std::size_t>(i)+1);
                                if constexpr ( hasElementMovedOp<Route> )
                                    mirrorRotationToIndexes(trackedIndexes, insertionIndex, i, i+1);
                            }
                            ++insertionIndex;
                        }

                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            for ( std::size_t i=0; i<size; ++i )
                            {
                                if ( i != trackedIndexes[i] )
                                    notifyElementMoved(user, Route{keys}, trackedIndexes[i], i);
                            }
                        }
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::MoveDown:
                {
                    if constexpr ( hasMoveOps )
                    {
                        auto movedIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                        if ( movedIndex+1 < std::size(ref) )
                        {
                            std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex+1)]);
                            mirrorSwapToSelection(getSelections<Pathway...>(), movedIndex, movedIndex+1);
                            if constexpr ( hasElementMovedOp<Route> )
                            {
                                notifyElementMoved(user, Route{keys}, movedIndex, movedIndex+1);
                                notifyElementMoved(user, Route{keys}, movedIndex+1, movedIndex);
                            }
                            if constexpr ( hasSelections && hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::MoveDownN:
                {
                    if constexpr ( hasMoveOps )
                    {
                        auto size = std::size(ref);
                        if ( size > 0 )
                        {
                            auto limit = size;
                            auto count = static_cast<std::size_t>(readIndex<index_type>(offset));
                            auto movedIndexes = readIndexes<index_type>(offset, count);
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                auto movedIndex = movedIndexes[i];
                                if ( static_cast<std::size_t>(movedIndex)+1 < limit )
                                {
                                    std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex)+1]);
                                    mirrorSwapToSelection(getSelections<Pathway...>(), movedIndex, static_cast<std::size_t>(movedIndex)+1);
                                    limit = static_cast<std::size_t>(movedIndex)+1;
                                }
                                else if ( limit > 0 )
                                    --limit;
                            }

                            if constexpr ( hasElementMovedOp<Route> )
                            {
                                limit = size;
                                std::size_t blockSize = 1;
                                for ( std::size_t i=0; i<count; ++i )
                                {
                                    if ( static_cast<std::size_t>(movedIndexes[i])+1 < limit )
                                    {
                                        if ( i+1 < count && movedIndexes[i]-1 == movedIndexes[i+1] )
                                            ++blockSize;
                                        else
                                        {
                                            notifyElementMoved(user, Route{keys}, movedIndexes[i]+blockSize, movedIndexes[i]);
                                            blockSize = 1;
                                        }
                                        notifyElementMoved(user, Route{keys}, movedIndexes[i], movedIndexes[i]+1);
                                        limit = movedIndexes[i]+1;
                                    }
                                    else if ( limit > 0 )
                                        --limit;
                                }
                            }
                            if constexpr ( hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::MoveDownL:
                {
                    if constexpr ( hasSelections && hasMoveOps )
                    {
                        auto & sel = getSelections<Pathway...>();
                        sel_type prevSel {};
                        readSelections(events, offset, prevSel);

                        auto movedIndexes = prevSel; // Copy so it can be sorted without changing selections
                        std::sort(movedIndexes.begin(), movedIndexes.end(), std::greater<index_type>{});
                        std::size_t count = std::size(movedIndexes);

                        auto size = std::size(ref);
                        if ( size > 0 )
                        {
                            auto limit = size;
                            for ( auto movedIndex : movedIndexes )
                            {
                                if ( static_cast<std::size_t>(movedIndex)+1 < limit )
                                {
                                    std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex)+1]);
                                    mirrorSwapToSelection(sel, movedIndex, static_cast<std::size_t>(movedIndex)+1);
                                    limit = static_cast<std::size_t>(movedIndex)+1;
                                }
                                else if ( limit > 0 )
                                    --limit;
                            }

                            if constexpr ( hasElementMovedOp<Route> )
                            {
                                limit = size;
                                std::size_t blockSize = 1;
                                for ( std::size_t i=0; i<count; ++i )
                                {
                                    if ( static_cast<std::size_t>(movedIndexes[i])+1 < limit )
                                    {
                                        if ( i+1 < count && movedIndexes[i]-1 == movedIndexes[i+1] )
                                            ++blockSize;
                                        else
                                        {
                                            notifyElementMoved(user, Route{keys}, movedIndexes[i]+blockSize, movedIndexes[i]);
                                            blockSize = 1;
                                        }
                                        notifyElementMoved(user, Route{keys}, movedIndexes[i], movedIndexes[i]+1);
                                        limit = movedIndexes[i]+1;
                                    }
                                    else if ( limit > 0 )
                                        --limit;
                                }
                            }
                            if constexpr ( hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::MoveBottom:
                {
                    if constexpr ( hasMoveOps )
                    {
                        auto movedIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                        if ( static_cast<std::size_t>(movedIndex)+1 < std::size(ref) )
                        {
                            auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(movedIndex));
                            std::rotate(it, it+1, ref.end());
                            mirrorRotationToSelection(getSelections<Pathway...>(), movedIndex, movedIndex+1, std::size(ref));
                            if constexpr ( hasElementMovedOp<Route> )
                            {
                                notifyElementMoved(user, Route{keys}, movedIndex, std::size(ref)-1);
                                for ( std::size_t i=movedIndex+1; i<std::size(ref); ++i )
                                    notifyElementMoved(user, Route{keys}, i, i-1);
                            }
                            if constexpr ( hasSelections && hasSelChangeOp )
                                notifySelectionsChanged(user,Route{keys});
                        }
                    }
                }
                break;
                case Op::MoveBottomN:
                {
                    if constexpr ( hasMoveOps )
                    {
                        auto & sel = getSelections<Pathway...>();
                        auto count = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto movedIndexes = readIndexes<index_type>(offset, count);

                        std::size_t size = std::size(ref);
                        std::vector<std::size_t> trackedIndexes {};
                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            trackedIndexes.assign(size, 0);
                            std::iota(trackedIndexes.begin(), trackedIndexes.end(), 0);
                        }

                        std::ptrdiff_t insertionIndex = static_cast<std::ptrdiff_t>(size);
                        for ( auto i : movedIndexes )
                        {
                            if ( i >= 0 && static_cast<std::ptrdiff_t>(i)+1 < insertionIndex )
                            {
                                auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                std::rotate(it, it+1, std::next(ref.begin(), insertionIndex));
                                mirrorRotationToSelection(sel, i, static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertionIndex));
                                if constexpr ( hasElementMovedOp<Route> )
                                    mirrorRotationToIndexes(trackedIndexes, i, i+1, insertionIndex);
                            }
                            --insertionIndex;
                        }

                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            for ( std::size_t i=0; i<size; ++i )
                            {
                                if ( i != trackedIndexes[i] )
                                    notifyElementMoved(user, Route{keys}, trackedIndexes[i], i);
                            }
                        }
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::MoveBottomL:
                {
                    if constexpr ( hasSelections && hasMoveOps )
                    {
                        auto & sel = getSelections<Pathway...>();
                        sel_type prevSel {};
                        readSelections(events, offset, prevSel);
                        auto movedIndexes = prevSel;
                        std::sort(movedIndexes.begin(), movedIndexes.end(), std::greater<index_type>{});
                            
                        std::size_t size = std::size(ref);
                        std::vector<std::size_t> trackedIndexes {};
                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            trackedIndexes.assign(size, 0);
                            std::iota(trackedIndexes.begin(), trackedIndexes.end(), 0);
                        }

                        std::ptrdiff_t insertionIndex = static_cast<std::ptrdiff_t>(size);
                        for ( auto i : movedIndexes )
                        {
                            if ( i >= 0 && static_cast<std::ptrdiff_t>(i)+1 < insertionIndex )
                            {
                                auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                std::rotate(it, it+1, std::next(ref.begin(), insertionIndex));
                                mirrorRotationToSelection(sel, i, static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertionIndex));
                                if constexpr ( hasElementMovedOp<Route> )
                                    mirrorRotationToIndexes(trackedIndexes, i, i+1, insertionIndex);
                            }
                            --insertionIndex;
                        }

                        if constexpr ( hasElementMovedOp<Route> )
                        {
                            for ( std::size_t i=0; i<size; ++i )
                            {
                                if ( i != trackedIndexes[i] )
                                    notifyElementMoved(user, Route{keys}, trackedIndexes[i], i);
                            }
                        }
                        if constexpr ( hasSelChangeOp )
                            notifySelectionsChanged(user, Route{keys});
                    }
                }
                break;
                case Op::MoveTo:
                {
                    if constexpr ( hasMoveOps )
                    {
                        auto indexMovedTo = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto movedIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                            
                        auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(movedIndex));
                        auto target = std::next(ref.begin(), static_cast<std::ptrdiff_t>(indexMovedTo));
                        if ( indexMovedTo < movedIndex && movedIndex < std::size(ref) )
                        {
                            std::rotate(target, it, it+1);
                            mirrorRotationToSelection(getSelections<Pathway...>(), indexMovedTo, movedIndex, movedIndex+1);
                            if constexpr ( hasElementMovedOp<Route> )
                            {
                                notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(movedIndex), static_cast<std::size_t>(indexMovedTo));
                                for ( std::size_t i=static_cast<std::size_t>(indexMovedTo); i<static_cast<std::size_t>(movedIndex); ++i )
                                    notifyElementMoved(user, Route{keys}, i, i+1);
                            }
                            if constexpr ( hasSelections && hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                        else if ( indexMovedTo > movedIndex && indexMovedTo < std::size(ref) )
                        {
                            std::rotate(it, it+1, target+1);
                            mirrorRotationToSelection(getSelections<Pathway...>(), movedIndex, movedIndex+1, indexMovedTo+1);
                            if constexpr ( hasElementMovedOp<Route> )
                            {
                                notifyElementMoved(user, Route{keys}, static_cast<std::size_t>(movedIndex), static_cast<std::size_t>(indexMovedTo));
                                for ( std::size_t i=static_cast<std::size_t>(movedIndex)+1; i<static_cast<std::size_t>(indexMovedTo)+1; ++i )
                                    notifyElementMoved(user, Route{keys}, i, i-1);
                            }
                            if constexpr ( hasSelections && hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::MoveToN:
                {
                    if constexpr ( hasMoveOps )
                    {
                        auto & sel = getSelections<Pathway...>();
                        auto indexMovedTo = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto countMovedIndexes = static_cast<std::size_t>(readIndex<index_type>(offset));
                        auto movedIndexes = readIndexes<index_type>(offset, countMovedIndexes);

                        auto size = std::size(ref);
                        if ( !std::empty(movedIndexes) )
                        {
                            std::size_t countValidIndexes = 0;
                            for ( auto movedIndex : movedIndexes )
                            {
                                if ( movedIndex >= 0 && static_cast<std::size_t>(movedIndex) < std::size(ref) )
                                    ++countValidIndexes;
                            }
                            if ( countValidIndexes == 0 )
                                return;

                            std::vector<std::size_t> trackedIndexes {};
                            if constexpr ( hasElementMovedOp<Route> )
                            {
                                trackedIndexes.assign(size, 0);
                                std::iota(trackedIndexes.begin(), trackedIndexes.end(), 0);
                            }

                            if ( static_cast<std::size_t>(indexMovedTo) + countValidIndexes >= size ) // Operation collapses to moveToEnd
                            {
                                std::ptrdiff_t insertionIndex = static_cast<std::ptrdiff_t>(size);
                                for ( auto mit = movedIndexes.rbegin(); mit != movedIndexes.rend(); ++mit ) // movedIndexes sorted least to greatest, so reverse read
                                {
                                    auto i = *mit;
                                    if ( i >= 0 && static_cast<std::ptrdiff_t>(i)+1 < insertionIndex )
                                    {
                                        auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                        std::rotate(it, it+1, std::next(ref.begin(), insertionIndex));
                                        mirrorRotationToSelection(sel, i, static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertionIndex));
                                        if constexpr ( hasElementMovedOp<Route> )
                                            mirrorRotationToIndexes(trackedIndexes, i, i+1, static_cast<std::size_t>(insertionIndex));
                                    }
                                    --insertionIndex;
                                }
                            }
                            else // Non-collapsed moveTo operation
                            {
                                std::size_t minValidIndex = 0;
                                std::size_t maxValidIndex = 0;
                                for ( std::size_t i=0; i<std::size(movedIndexes); ++i )
                                {
                                    if ( movedIndexes[i] >= 0 )
                                    {
                                        minValidIndex = static_cast<std::size_t>(movedIndexes[i]);
                                        break;
                                    }
                                }
                                for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(std::size(movedIndexes))-1; i>=0; --i )
                                {
                                    if ( static_cast<std::ptrdiff_t>(movedIndexes[static_cast<std::size_t>(i)]) < static_cast<std::ptrdiff_t>(size) )
                                    {
                                        maxValidIndex = static_cast<std::size_t>(movedIndexes[static_cast<std::size_t>(i)]);
                                        break;
                                    }
                                }

                                std::size_t leftChunkSize = minValidIndex < static_cast<std::size_t>(indexMovedTo) ? 1 : 0;
                                std::size_t rightChunkSize = maxValidIndex >= static_cast<std::size_t>(indexMovedTo) ? 1 : 0;
                                std::size_t leftChunkFirst = minValidIndex;
                                std::size_t rightChunkFirst = maxValidIndex;

                                for ( std::size_t i=0; i+1<std::size(movedIndexes) && movedIndexes[i+1] < indexMovedTo; ++i )
                                {
                                    // rotate left chunk together with element[movedIndexes[i+1]]
                                    auto leftStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst));
                                    auto leftEnd = std::next(leftStart, static_cast<std::ptrdiff_t>(leftChunkSize));
                                    std::rotate(leftStart, leftEnd, std::next(ref.begin(), static_cast<std::ptrdiff_t>(movedIndexes[i+1])));
                                    mirrorRotationToSelection(sel, leftChunkFirst, leftChunkFirst+leftChunkSize, movedIndexes[i+1]);
                                    if constexpr ( hasElementMovedOp<Route> )
                                        mirrorRotationToIndexes(trackedIndexes, leftChunkFirst, leftChunkFirst+leftChunkSize, movedIndexes[i+1]);
                                    leftChunkFirst = static_cast<std::size_t>(movedIndexes[i+1])-leftChunkSize;
                                    ++leftChunkSize;
                                }
                                for ( std::size_t i=std::size(movedIndexes)-1; i>0 && movedIndexes[i-1] >= indexMovedTo; --i )
                                {
                                    // rotate right chunk together with element[movedIndexes[i-1]]
                                    auto rightStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(rightChunkFirst));
                                    auto rightEnd = std::next(rightStart, static_cast<std::ptrdiff_t>(rightChunkSize));
                                    std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(movedIndexes[i-1])+1), rightStart, rightEnd);
                                    mirrorRotationToSelection(sel, static_cast<std::size_t>(movedIndexes[i-1])+1, rightChunkFirst, rightChunkFirst+rightChunkSize);
                                    if constexpr ( hasElementMovedOp<Route> )
                                        mirrorRotationToIndexes(trackedIndexes, movedIndexes[i-1]+1, rightChunkFirst, rightChunkFirst+rightChunkSize);
                                    rightChunkFirst = static_cast<std::size_t>(movedIndexes[i-1]);
                                    ++rightChunkSize;
                                }
                                if ( leftChunkFirst+leftChunkSize != rightChunkFirst && leftChunkSize > 0 && rightChunkSize > 0 ) // Need to combine the chunks then rotate combined to final position
                                {
                                    std::size_t leftDistance = leftChunkFirst < static_cast<std::size_t>(indexMovedTo) ? static_cast<std::size_t>(indexMovedTo)-leftChunkFirst : leftChunkFirst-static_cast<std::size_t>(indexMovedTo);
                                    std::size_t rightDistance = rightChunkFirst < static_cast<std::size_t>(indexMovedTo)+leftChunkSize ? static_cast<std::size_t>(indexMovedTo)+leftChunkSize-rightChunkFirst : rightChunkFirst-static_cast<std::size_t>(indexMovedTo)-leftChunkSize;
                                    if ( rightDistance < leftDistance ) // Right chunk closer to target, move left up to right
                                    {
                                        auto leftStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst));
                                        auto leftEnd = std::next(leftStart, static_cast<std::ptrdiff_t>(leftChunkSize));
                                        std::rotate(leftStart, leftEnd, std::next(ref.begin(), static_cast<std::ptrdiff_t>(rightChunkFirst)));
                                        mirrorRotationToSelection(sel, leftChunkFirst, leftChunkFirst+leftChunkSize, rightChunkFirst);
                                        if constexpr ( hasElementMovedOp<Route> )
                                            mirrorRotationToIndexes(trackedIndexes, leftChunkFirst, leftChunkFirst+leftChunkSize, rightChunkFirst);
                                        leftChunkFirst = rightChunkFirst-leftChunkSize;
                                    }
                                    else // Left chunk closer to target, move right up to left
                                    {
                                        auto rightStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(rightChunkFirst));
                                        auto rightEnd = std::next(rightStart, static_cast<std::ptrdiff_t>(rightChunkSize));
                                        std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst+leftChunkSize)), rightStart, rightEnd);
                                        mirrorRotationToSelection(sel, leftChunkFirst+leftChunkSize, rightChunkFirst, rightChunkFirst+rightChunkSize);
                                        if constexpr ( hasElementMovedOp<Route> )
                                            mirrorRotationToIndexes(trackedIndexes, leftChunkFirst+leftChunkSize, rightChunkFirst, rightChunkFirst+rightChunkSize);
                                        rightChunkFirst = leftChunkFirst+leftChunkSize;
                                    }
                                }
                                if ( leftChunkFirst < static_cast<std::size_t>(indexMovedTo) ) // Rotate combined chunk rightwards to final position
                                {
                                    auto chunkStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst));
                                    auto chunkEnd = std::next(chunkStart, static_cast<std::ptrdiff_t>(countValidIndexes));
                                    std::rotate(chunkStart, chunkEnd, std::next(chunkEnd, static_cast<std::ptrdiff_t>(static_cast<std::size_t>(indexMovedTo)-leftChunkFirst)));
                                    mirrorRotationToSelection(sel, leftChunkFirst, leftChunkFirst+countValidIndexes, leftChunkFirst+countValidIndexes+static_cast<std::size_t>(indexMovedTo)-leftChunkFirst);
                                    if constexpr ( hasElementMovedOp<Route> )
                                        mirrorRotationToIndexes(trackedIndexes, leftChunkFirst, leftChunkFirst+countValidIndexes, leftChunkFirst+countValidIndexes+static_cast<std::size_t>(indexMovedTo)-leftChunkFirst);
                                }
                                else if ( leftChunkFirst > static_cast<std::size_t>(indexMovedTo) ) // Rotate combined chunk leftwards to final position
                                {
                                    auto chunkStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst));
                                    auto chunkEnd = std::next(chunkStart, static_cast<std::ptrdiff_t>(countValidIndexes));
                                    std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(indexMovedTo)), chunkStart, chunkEnd);
                                    mirrorRotationToSelection(sel, indexMovedTo, leftChunkFirst, leftChunkFirst+countValidIndexes);
                                    if constexpr ( hasElementMovedOp<Route> )
                                        mirrorRotationToIndexes(trackedIndexes, indexMovedTo, leftChunkFirst, leftChunkFirst+countValidIndexes);
                                }
                            }
                    
                            if constexpr ( hasElementMovedOp<Route> )
                            {
                                for ( std::size_t i=0; i<size; ++i )
                                {
                                    if ( i != trackedIndexes[i] )
                                        notifyElementMoved(user, Route{keys}, trackedIndexes[i], i);
                                }
                            }
                            if constexpr ( hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
                case Op::MoveToL:
                {
                    if constexpr ( hasSelections && hasMoveOps )
                    {
                        auto indexMovedTo = static_cast<std::size_t>(readIndex<index_type>(offset));
                            
                        auto & sel = getSelections<Pathway...>();
                        sel_type prevSel {};
                        readSelections(events, offset, prevSel);
                        auto movedIndexes = prevSel; // Copy so it can be sorted without changing selections
                        std::sort(movedIndexes.begin(), movedIndexes.end());

                        auto size = std::size(ref);
                        if ( !std::empty(movedIndexes) )
                        {
                            std::size_t countValidIndexes = 0;
                            for ( auto movedIndex : movedIndexes )
                            {
                                if ( movedIndex >= 0 && static_cast<std::size_t>(movedIndex) < std::size(ref) )
                                    ++countValidIndexes;
                            }
                            if ( countValidIndexes == 0 )
                                return;

                            std::vector<std::size_t> trackedIndexes {};
                            if constexpr ( hasElementMovedOp<Route> )
                            {
                                trackedIndexes.assign(size, 0);
                                std::iota(trackedIndexes.begin(), trackedIndexes.end(), 0);
                            }

                            if ( static_cast<std::size_t>(indexMovedTo) + countValidIndexes >= size ) // Operation collapses to moveToEnd
                            {
                                std::ptrdiff_t insertionIndex = static_cast<std::ptrdiff_t>(size);
                                for ( auto mit = movedIndexes.rbegin(); mit != movedIndexes.rend(); ++mit ) // movedIndexes sorted least to greatest, so reverse read
                                {
                                    auto i = *mit;
                                    if ( i >= 0 && static_cast<std::ptrdiff_t>(i)+1 < insertionIndex )
                                    {
                                        auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                        std::rotate(it, it+1, std::next(ref.begin(), insertionIndex));
                                        mirrorRotationToSelection(sel, i, static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertionIndex));
                                        if constexpr ( hasElementMovedOp<Route> )
                                            mirrorRotationToIndexes(trackedIndexes, i, i+1, static_cast<std::size_t>(insertionIndex));
                                    }
                                    --insertionIndex;
                                }
                            }
                            else // Non-collapsed moveTo operation
                            {
                                std::size_t minValidIndex = 0;
                                std::size_t maxValidIndex = 0;
                                for ( std::size_t i=0; i<std::size(movedIndexes); ++i )
                                {
                                    if ( movedIndexes[i] >= 0 )
                                    {
                                        minValidIndex = static_cast<std::size_t>(movedIndexes[i]);
                                        break;
                                    }
                                }
                                for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(std::size(movedIndexes))-1; i>=0; --i )
                                {
                                    if ( static_cast<std::ptrdiff_t>(movedIndexes[static_cast<std::size_t>(i)]) < static_cast<std::ptrdiff_t>(size) )
                                    {
                                        maxValidIndex = static_cast<std::size_t>(movedIndexes[static_cast<std::size_t>(i)]);
                                        break;
                                    }
                                }

                                std::size_t leftChunkSize = minValidIndex < static_cast<std::size_t>(indexMovedTo) ? 1 : 0;
                                std::size_t rightChunkSize = maxValidIndex >= static_cast<std::size_t>(indexMovedTo) ? 1 : 0;
                                std::size_t leftChunkFirst = minValidIndex;
                                std::size_t rightChunkFirst = maxValidIndex;

                                for ( std::size_t i=0; i+1<std::size(movedIndexes) && movedIndexes[i+1] < indexMovedTo; ++i )
                                {
                                    // rotate left chunk together with element[movedIndexes[i+1]]
                                    auto leftStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst));
                                    auto leftEnd = std::next(leftStart, static_cast<std::ptrdiff_t>(leftChunkSize));
                                    std::rotate(leftStart, leftEnd, std::next(ref.begin(), static_cast<std::ptrdiff_t>(movedIndexes[i+1])));
                                    mirrorRotationToSelection(sel, leftChunkFirst, leftChunkFirst+leftChunkSize, movedIndexes[i+1]);
                                    if constexpr ( hasElementMovedOp<Route> )
                                        mirrorRotationToIndexes(trackedIndexes, leftChunkFirst, leftChunkFirst+leftChunkSize, movedIndexes[i+1]);
                                    leftChunkFirst = static_cast<std::size_t>(movedIndexes[i+1])-leftChunkSize;
                                    ++leftChunkSize;
                                }
                                for ( std::size_t i=std::size(movedIndexes)-1; i>0 && movedIndexes[i-1] >= indexMovedTo; --i )
                                {
                                    // rotate right chunk together with element[movedIndexes[i-1]]
                                    auto rightStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(rightChunkFirst));
                                    auto rightEnd = std::next(rightStart, static_cast<std::ptrdiff_t>(rightChunkSize));
                                    std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(movedIndexes[i-1])+1), rightStart, rightEnd);
                                    mirrorRotationToSelection(sel, static_cast<std::size_t>(movedIndexes[i-1])+1, rightChunkFirst, rightChunkFirst+rightChunkSize);
                                    if constexpr ( hasElementMovedOp<Route> )
                                        mirrorRotationToIndexes(trackedIndexes, movedIndexes[i-1]+1, rightChunkFirst, rightChunkFirst+rightChunkSize);
                                    rightChunkFirst = static_cast<std::size_t>(movedIndexes[i-1]);
                                    ++rightChunkSize;
                                }
                                if ( leftChunkFirst+leftChunkSize != rightChunkFirst && leftChunkSize > 0 && rightChunkSize > 0 ) // Need to combine the chunks then rotate combined to final position
                                {
                                    std::size_t leftDistance = leftChunkFirst < static_cast<std::size_t>(indexMovedTo) ? static_cast<std::size_t>(indexMovedTo)-leftChunkFirst : leftChunkFirst-static_cast<std::size_t>(indexMovedTo);
                                    std::size_t rightDistance = rightChunkFirst < static_cast<std::size_t>(indexMovedTo)+leftChunkSize ? static_cast<std::size_t>(indexMovedTo)+leftChunkSize-rightChunkFirst : rightChunkFirst-static_cast<std::size_t>(indexMovedTo)-leftChunkSize;
                                    if ( rightDistance < leftDistance ) // Right chunk closer to target, move left up to right
                                    {
                                        auto leftStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst));
                                        auto leftEnd = std::next(leftStart, static_cast<std::ptrdiff_t>(leftChunkSize));
                                        std::rotate(leftStart, leftEnd, std::next(ref.begin(), static_cast<std::ptrdiff_t>(rightChunkFirst)));
                                        mirrorRotationToSelection(sel, leftChunkFirst, leftChunkFirst+leftChunkSize, rightChunkFirst);
                                        if constexpr ( hasElementMovedOp<Route> )
                                            mirrorRotationToIndexes(trackedIndexes, leftChunkFirst, leftChunkFirst+leftChunkSize, rightChunkFirst);
                                        leftChunkFirst = rightChunkFirst-leftChunkSize;
                                    }
                                    else // Left chunk closer to target, move right up to left
                                    {
                                        auto rightStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(rightChunkFirst));
                                        auto rightEnd = std::next(rightStart, static_cast<std::ptrdiff_t>(rightChunkSize));
                                        std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst+leftChunkSize)), rightStart, rightEnd);
                                        mirrorRotationToSelection(sel, leftChunkFirst+leftChunkSize, rightChunkFirst, rightChunkFirst+rightChunkSize);
                                        if constexpr ( hasElementMovedOp<Route> )
                                            mirrorRotationToIndexes(trackedIndexes, leftChunkFirst+leftChunkSize, rightChunkFirst, rightChunkFirst+rightChunkSize);
                                        rightChunkFirst = leftChunkFirst+leftChunkSize;
                                    }
                                }
                                if ( leftChunkFirst < static_cast<std::size_t>(indexMovedTo) ) // Rotate combined chunk rightwards to final position
                                {
                                    auto chunkStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst));
                                    auto chunkEnd = std::next(chunkStart, static_cast<std::ptrdiff_t>(countValidIndexes));
                                    std::rotate(chunkStart, chunkEnd, std::next(chunkEnd, static_cast<std::ptrdiff_t>(static_cast<std::size_t>(indexMovedTo)-leftChunkFirst)));
                                    mirrorRotationToSelection(sel, leftChunkFirst, leftChunkFirst+countValidIndexes, leftChunkFirst+countValidIndexes+static_cast<std::size_t>(indexMovedTo)-leftChunkFirst);
                                    if constexpr ( hasElementMovedOp<Route> )
                                        mirrorRotationToIndexes(trackedIndexes, leftChunkFirst, leftChunkFirst+countValidIndexes, leftChunkFirst+countValidIndexes+static_cast<std::size_t>(indexMovedTo)-leftChunkFirst);
                                }
                                else if ( leftChunkFirst > static_cast<std::size_t>(indexMovedTo) ) // Rotate combined chunk leftwards to final position
                                {
                                    auto chunkStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst));
                                    auto chunkEnd = std::next(chunkStart, static_cast<std::ptrdiff_t>(countValidIndexes));
                                    std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(indexMovedTo)), chunkStart, chunkEnd);
                                    mirrorRotationToSelection(sel, indexMovedTo, leftChunkFirst, leftChunkFirst+countValidIndexes);
                                    if constexpr ( hasElementMovedOp<Route> )
                                        mirrorRotationToIndexes(trackedIndexes, indexMovedTo, leftChunkFirst, leftChunkFirst+countValidIndexes);
                                }
                            }
                    
                            if constexpr ( hasElementMovedOp<Route> )
                            {
                                for ( std::size_t i=0; i<size; ++i )
                                {
                                    if ( i != trackedIndexes[i] )
                                        notifyElementMoved(user, Route{keys}, trackedIndexes[i], i);
                                }
                            }
                            if constexpr ( hasSelChangeOp )
                                notifySelectionsChanged(user, Route{keys});
                        }
                    }
                }
                break;
            }
        }

        template <class U, bool Undo, bool AfterSel = false, class Member = void, std::size_t ... MemberIndexes, class Keys = std::tuple<>, class ... Pathway>
        void processEventRef(U & t, std::size_t & offset, std::optional<std::size_t> & secondaryOffset, std::uint8_t op,
            std::index_sequence<MemberIndexes...>, Keys keys = std::tuple{}, type_tags<Pathway...> = {})
        {
            constexpr bool hasSel = hasSelections<Pathway...>();
            using index_type = index_type_t<default_index_type, Member>;
            std::uint8_t value = events[offset++];

            switch ( PathOp(value & std::uint8_t(PathOp::HighBits)) )
            {
                case PathOp::SelBranch:
                    if constexpr ( hasSel && requires{t[0];} )
                    {
                        std::size_t selBranchStartOffset = offset;
                        auto & selData = getSelections<Pathway...>();
                        for ( auto & sel : selData )
                        {
                            using U2 = std::remove_cvref_t<decltype(t[0])>;
                            processEventRef<U2, Undo, true, Member>( // sel is prepended since sels are always before first PathIndex
                                t[static_cast<std::size_t>(sel)], offset, secondaryOffset, op, std::make_index_sequence<reflectedMemberCount<U2>()>(),
                                std::tuple_cat(std::tuple<index_type>{sel}, keys), type_tags<Pathway..., PathIndex<0>>{});

                            offset = selBranchStartOffset;
                        }
                    }
                    break;
                case PathOp::LeafSelBranch:
                    if constexpr ( sizeof...(Pathway) == 0 && Undo )
                        processUndoEvent<std::remove_cvref_t<U>, void>(op, offset, secondaryOffset, t, keys);
                    else if constexpr ( sizeof...(Pathway) == 0 && !Undo )
                        processRedoEvent<std::remove_cvref_t<U>, void>(op, offset, secondaryOffset, t, keys);
                    else if constexpr ( hasSel && requires{t[0];} )
                    {
                        using element_type = std::remove_cvref_t<RareTs::element_type_t<std::remove_cvref_t<U>>>;
                        
                        std::size_t selBranchStartOffset = offset;
                        auto & selData = getSelections<Pathway...>();
                        for ( auto & sel : selData )
                        {
                            if constexpr ( Undo )
                            {
                                processUndoEvent<element_type, Member, Pathway..., PathIndex<0>>(
                                    op, offset, secondaryOffset, t[static_cast<std::size_t>(sel)], std::tuple_cat(std::tuple<index_type>{sel}, keys));
                            }
                            else
                            {
                                processRedoEvent<element_type, Member, Pathway..., PathIndex<0>>(
                                    op, offset, secondaryOffset, t[static_cast<std::size_t>(sel)], std::tuple_cat(std::tuple<index_type>{sel}, keys));
                            }

                            offset = selBranchStartOffset;
                        }
                    }
                    break;
                case PathOp::Branch:
                {
                    if constexpr ( RareTs::is_static_array_v<U> || RareTs::is_specialization_v<U, std::vector> ) // Branch to index
                    {
                        index_type index {};
                        if constexpr ( std::is_same_v<index_type, uint6_t> )
                            index = static_cast<uint6_t>(value & std::uint8_t(PathOp::LowBits));
                        else
                            index = readIndex<index_type>(offset);
                        
                        using U2 = std::remove_cvref_t<decltype(std::declval<U>()[0])>;
                        processEventRef<U2, Undo, AfterSel, Member>(
                            t[static_cast<std::size_t>(index)], offset, secondaryOffset, op, std::make_index_sequence<reflectedMemberCount<U2>()>(),
                            std::tuple_cat(keys, std::tuple{index}), type_tags<Pathway..., PathIndex<std::tuple_size_v<Keys>>>{});
                    }
                    else if constexpr ( RareTs::is_macro_reflected_v<U> ) // Branch to field
                    {
                        (void)((MemberIndexes == std::size_t(value & std::uint8_t(PathOp::LowBits)) ?
                            (processEventRef<std::remove_cvref_t<typename RareTs::Member<U, MemberIndexes>::type>, Undo, AfterSel, RareTs::Member<U, MemberIndexes>>(
                                RareTs::Member<U, MemberIndexes>::value(t), offset, secondaryOffset, op,
                                std::make_index_sequence<reflectedMemberCount<std::remove_cvref_t<typename RareTs::Member<U, MemberIndexes>::type>>()>(),
                                keys, type_tags<Pathway..., PathMember<MemberIndexes>>{}), true) : false) || ...);

                        //RareTs::Members<U>::at(std::size_t(value & std::uint8_t(PathOp::LowBits)), t, [&](auto member, auto & ref) {
                        //    using U2 = std::remove_cvref_t<typename decltype(member)::type>;
                        //    processEventRef<U2, Undo, AfterSel, decltype(member)>(
                        //        ref, offset, secondaryOffset, op, std::make_index_sequence<reflectedMemberCount<U2>()>(),
                        //        keys, type_tags<Pathway..., PathMember<decltype(member)::index>>{});
                        //});
                    }
                }
                break;
                case PathOp::LeafBranch:
                {
                    if constexpr ( RareTs::is_static_array_v<U> || RareTs::is_specialization_v<U, std::vector>  ) // Op on index
                    {
                        using PathIdx = PathIndex<std::tuple_size_v<Keys>>;
                        index_type index {};
                        if constexpr ( std::is_same_v<index_type, uint6_t> )
                            index = uint6_t(value & std::uint8_t(PathOp::LowBits));
                        else
                            index = readIndex<index_type>(offset);

                        if constexpr ( Undo )
                        {
                            processUndoEvent<std::remove_cvref_t<RareTs::element_type_t<std::remove_cvref_t<U>>>, Member, Pathway..., PathIdx>(
                                op, offset, secondaryOffset, t[static_cast<std::size_t>(index)], std::tuple_cat(keys, std::tuple{index}));
                        }
                        else
                        {
                            processRedoEvent<std::remove_cvref_t<RareTs::element_type_t<std::remove_cvref_t<U>>>, Member, Pathway..., PathIdx>(
                                op, offset, secondaryOffset, t[static_cast<std::size_t>(index)], std::tuple_cat(keys, std::tuple{index}));
                        }
                    }
                    else if constexpr ( RareTs::is_macro_reflected_v<U> ) // Op on field
                    {
                        std::size_t fieldIndex = std::size_t(value & std::uint8_t(PathOp::LowBits));
                        if constexpr ( Undo )
                        {
                            (void)((MemberIndexes == fieldIndex ?
                                (processUndoEvent<std::remove_cvref_t<typename RareTs::Member<U, MemberIndexes>::type>,
                                    RareTs::Member<U, MemberIndexes>, Pathway..., PathMember<RareTs::Member<U, MemberIndexes>::index>>(
                                    op, offset, secondaryOffset, RareTs::Member<U, MemberIndexes>::value(t), keys), true) : false) || ...);
                        }
                        else
                        {
                            (void)((MemberIndexes == fieldIndex ?
                                (processRedoEvent<std::remove_cvref_t<typename RareTs::Member<U, MemberIndexes>::type>,
                                    RareTs::Member<U, MemberIndexes>, Pathway..., PathMember<RareTs::Member<U, MemberIndexes>::index>>(
                                    op, offset, secondaryOffset, RareTs::Member<U, MemberIndexes>::value(t), keys), true) : false) || ...);
                        }

                        //if constexpr ( Undo ) {
                        //    RareTs::Members<U>::at(std::size_t(value & std::uint8_t(PathOp::LowBits)), t, [&](auto member, auto & ref) {
                        //        processUndoEvent<std::remove_cvref_t<typename decltype(member)::type>, decltype(member), Pathway..., PathMember<decltype(member)::index>>(
                        //            op, offset, secondaryOffset, ref, keys);
                        //    });
                        //} else {
                        //    RareTs::Members<U>::at(std::size_t(value & std::uint8_t(PathOp::LowBits)), t, [&](auto member, auto & ref) {
                        //        processRedoEvent<std::remove_cvref_t<typename decltype(member)::type>, decltype(member), Pathway..., PathMember<decltype(member)::index>>(
                        //            op, offset, secondaryOffset, ref, keys);
                        //    });
                        //}
                    }
                }
                break;
                default:
                    throw std::runtime_error("Invalid PathOp detected");
            }
        }

        void undoEvent(std::size_t eventIndex)
        {
            if ( eventIndex >= eventOffsets.size() )
                return;
            
            std::size_t offset = eventOffsets[eventIndex];
            std::optional<std::size_t> secondaryOffset {}; // For selection ops with multiple callbacks/bifurcated offsets
            std::uint8_t op = events[offset];
            ++offset;
            
            processEventRef<T, true>(t, offset, secondaryOffset, op, std::make_index_sequence<reflectedMemberCount<T>()>(), std::tuple{}, type_tags<>{});
        }

        void redoEvent(std::size_t eventIndex)
        {
            if ( eventIndex >= eventOffsets.size() )
                return;
            
            std::optional<std::size_t> secondaryOffset {}; // For selection ops with multiple callbacks/bifurcated offsets
            std::size_t offset = eventOffsets[eventIndex];
            std::uint8_t op = events[offset];
            ++offset;
            
            processEventRef<T, false>(t, offset, secondaryOffset, op, std::make_index_sequence<reflectedMemberCount<T>()>(), std::tuple{}, type_tags<>{});
        }
    };

    template <class Data, class User>
    requires RareTs::is_macro_reflected_v<Data> && std::is_object_v<User>
    class Tracked;

    template <class T, class User, class Editor>
    class EditRoot : private Agent<T, User, Editor>, public edit_members<Agent<T, User, Editor>, typename decltype(defaultIndexType<T>())::type, T, T>
    {
        EditRoot(T & t, User & user) :
            Agent<T, User, Editor>(t, user),
            edit_members<Agent<T, User, Editor>, typename decltype(defaultIndexType<T>())::type, T, T>{(Agent<T, User, Editor>&)*this, std::tuple{}} {}

        friend class Tracked<T, User>;
        friend struct Agent<T, User, Editor>;

    public:
        using default_index_type = typename Agent<T, User, Editor>::default_index_type;

        void assign(const T & value) {
            std::tuple keys {}; // No keys/array indexes as this is the root
            Agent<T, User, Editor>::template set<>(value, keys);
        }
    };
    
    template <typename T, typename PathToElem,
    typename EditorType = typename PathToElem::editor_type,
    typename ReadEditPair = decltype(std::declval<EditorType>().editFromPath(PathToElem{}))>
    class TrackedElement;

    template <class Tracked>
    class Editor // If moved-from, or maybe created with createSharedAction the action shouldn't be auto-submitted, allowing for brushing
    {            // However... it should be ensured that there is only ever one such action
                 // Maybe it's an optional contained within this class and a derefable wrapper for that including a bool is returned?
        Tracked* parent = nullptr;

        using tracked_type = Tracked;
        template <typename, typename, typename, typename> friend class TrackedElement;

    public:
        Editor(Tracked* parent) : parent(parent) {
            if ( parent != nullptr )
                parent->actionReferenceCount++;
        }
        Editor(Editor && other) noexcept { std::swap(parent, other.parent); }
        ~Editor() {
            if ( parent != nullptr )
                parent->actionReferenceCount--;
        }
        auto & operator*() noexcept { return parent->editable; }
        auto operator->() noexcept { return &parent->editable; }

        template <class Keys, class ... Pathway>
        auto editFromPath(PathTaggedKeys<Keys, type_tags<Pathway...>, Editor<Tracked>> path)
        {
            auto & agent = (Agent<typename Tracked::data_type, typename Tracked::user_type, Editor> &)parent->editable;
            return agent.template editorFromPath<Pathway...>(**this, static_cast<typename Tracked::data_type &>(*parent), path);
        }
    };

    template <class Data, class User>
    requires RareTs::is_macro_reflected_v<Data> && std::is_object_v<User>
    class Tracked : Data
    {
        using data_type = Data;
        using user_type = User;
        using edit_root = EditRoot<Data, User, Editor<Tracked>>;

        static constexpr std::uint64_t flagElidedRedos    = 0x8000000000000000ull;
        static constexpr std::uint64_t maskElidedRedoSize = 0x7FFFFFFFFFFFFFFFull; // The total size of this elided redo branch, including sub-branches

        edit_root editable;

        std::vector<std::uint64_t> actionFirstEvent; // Index of the first data-change event for action[i] (presently the only persistent data for actions)
        int actionReferenceCount = 0; // Referencing counting for the current action, new actions can only be created when the old action is closed
        std::uint64_t redoCount = 0; // How many undos have occured since the last user-action/how many redos are available
        std::uint64_t redoSize = 0; // The size of the range including the redoable actions (includes elided redos)

        friend class Editor<Tracked>;

    protected:

        void elideRedos()
        {
            if ( redoCount == 0 )
                throw std::logic_error("Redos cannot be elided when there are no redos");
            
            actionFirstEvent.push_back(redoSize | flagElidedRedos);
            redoCount = 0;
            redoSize = 0;
        }

        auto createAction()
        {
            if ( actionReferenceCount == 0 )
            {
                if ( redoCount > 0 )
                    elideRedos();

                actionFirstEvent.push_back(editable.eventOffsets.size());
            }
            return Editor<Tracked>{this};
        };

        static inline Editor<Tracked> root {nullptr}; // Represents the root of the data structure, used by client code to create paths

    public:
        const edit_root & view; // Used to view selections data (and potentially other info associated with particular data paths)
        Tracked(User* user) : editable(*this, *user), view(editable) {}
        Tracked(User & user) : editable(*this, user), view(editable) {}

        const Data & read = (const Data &)*this; // A read only version of the user data
        Agent<Data, User, Editor<Tracked>> & history = static_cast<Agent<Data, User, Editor<Tracked>> &>(editable);
        constexpr const Data & operator*() const noexcept { return read; }
        constexpr const Data* operator->() const noexcept { return this; }
        auto operator()() { 
            if ( actionReferenceCount == 0 )
            {
                if ( redoCount > 0 )
                    elideRedos();

                actionFirstEvent.push_back(editable.eventOffsets.size());
            }
            return Editor<Tracked>{this};
        };

        void clearHistory()
        {
            if ( actionReferenceCount != 0 )
                throw std::logic_error("Cannot clear history while an action is active");

            history.clear();
            redoCount = 0;
            redoSize = 0;
            actionFirstEvent.clear();
        }

        // Initializes the stored data with the given input, this initialization is tracked of initTracked is true
        // If there's an active action or any stored actions a logic_error will be thrown
        template <bool initTracked = false>
        void initData(auto && data)
        {
            if ( !actionFirstEvent.empty() )
                throw std::logic_error("Cannot init an object that already has history!");
            else if ( actionReferenceCount > 0 )
                throw std::logic_error("Cannot init an object that already has an active action!");

            if constexpr ( initTracked )
                createAction()->assign(data);
            else
                std::swap((Data&)*this, data);
        }

        void undoAction()
        {
            if ( redoSize >= actionFirstEvent.size() )
                return;

            std::size_t totalActions = actionFirstEvent.size();
            std::uint64_t actionIndex = totalActions-redoSize-1;

            while ( (actionFirstEvent[actionIndex] & flagElidedRedos) == flagElidedRedos ) // Find the next unelided action
                actionIndex -= ((actionFirstEvent[actionIndex] & maskElidedRedoSize)+1);

            std::int64_t actionEventStart = static_cast<std::int64_t>(actionFirstEvent[actionIndex]);
            std::int64_t nextActionStart = static_cast<std::size_t>(actionIndex)<totalActions-1 ?
                static_cast<std::int64_t>(actionFirstEvent[actionIndex+1]) :
                static_cast<std::int64_t>(editable.eventOffsets.size());
            
            auto & agent = (Agent<Data, User, Editor<Tracked>> &)editable;
            for ( std::int64_t i=nextActionStart-1; i>=actionEventStart; i-- )
            {
                agent.undoEvent(std::uint64_t(i));
                //std::cout << '\n';
            }

            redoCount++;
            redoSize = totalActions-actionIndex;
        }

        void redoAction()
        {
            if ( redoCount == 0 )
                return;

            std::size_t totalActions = actionFirstEvent.size();
            std::uint64_t actionIndex = totalActions-redoCount;
            
            std::uint64_t actionEventStart = actionFirstEvent[actionIndex];
            std::uint64_t nextActionStart = actionIndex<totalActions-1 ? actionFirstEvent[actionIndex+1] : editable.eventOffsets.size();
            
            auto & agent = (Agent<Data, User, Editor<Tracked>> &)editable;
            for ( auto i=actionEventStart; i<nextActionStart; i++ )
                agent.redoEvent(i);

            redoCount--;
            if ( redoCount == 0 )
                redoSize = 0;
            else
            {
                actionIndex = totalActions-1;
                std::size_t unelidedCount = 0;
                while ( unelidedCount < redoCount )
                {
                    if ( (actionFirstEvent[actionIndex] & flagElidedRedos) == flagElidedRedos )
                        actionIndex -= ((actionFirstEvent[actionIndex] & maskElidedRedoSize)+1);
                    else
                    {
                        ++unelidedCount;
                        --actionIndex;
                    }
                }
                redoSize = totalActions-actionIndex;
            }
        }

        auto & put(std::ostream & os, auto && value) const
        {
            os << (RareTs::promote_char_t<decltype(value)>)value;
            return *this;
        }

        template <class type, class member_type>
        auto & putValue(std::ostream & os, std::size_t & offset) const
        {
            if constexpr ( std::is_array_v<type> )
            {
                type t {};
                editable.template readValue<type, member_type>(offset, t);
                os << Json::out<Json::Statics::Excluded, RareTs::NoNote, 0, Json::Output::twoSpaces, type>(t);
            }
            else
                os << Json::out(editable.template readValue<type, member_type>(offset));

            return *this;
        }

        template <class type, class member_type>
        auto & putValues(std::ostream & os, std::size_t & offset, std::size_t size) const
        {
            os << "{";
            if constexpr ( std::is_array_v<type> )
            {
                for ( std::size_t i=0; i<size; ++i )
                {
                    type t {};
                    editable.template readValue<type, member_type>(offset, t);
                    os << (i>0 ? ", " : "") << Json::out(t);
                }
            }
            else
            {
                for ( std::size_t i=0; i<size; ++i )
                    os << (i>0 ? ", " : "") << Json::out(editable.template readValue<type, member_type>(offset));
            }
            os << "}";
            return *this;
        }

        template <class index_type>
        auto & putIndex(std::ostream & os, std::size_t & offset) const
        {
            os << (RareTs::promote_char_t<index_type>)editable.template readIndex<index_type>(offset);
            return *this;
        }

        template <class index_type>
        auto & putIndexes(std::ostream & os, std::size_t & offset, std::size_t size) const
        {
            auto indexes = editable.template readIndexes<index_type>(offset, size);
            os << "{";
            for ( std::size_t i=0; i<size; ++i )
                os << (i>0 ? ", " : "") << (RareTs::promote_char_t<index_type>)indexes[i];

            os << "}";
            return *this;
        }

        template <class type, class member_type>
        void printEventOp(std::size_t & offset, Op op) const
        {
            auto & os = std::cout; // TODO: Lift
            using element = RareTs::element_type_t<std::remove_cvref_t<type>>;
            using index_type = index_type_t<typename edit_root::default_index_type, member_type>;
            switch ( op )
            {
                case Op::Reset: // .reset() // prevValue
                    put(os, ".reset() // ").template putValue<type, member_type>(os, offset);
                    break;
                case Op::Reserve: // .reserve(size)
                    put(os, ".reserve(").template putIndex<index_type>(os, offset).put(os, ")");
                    break;
                case Op::Trim: // .trim()
                    put(os, ".trim()");
                    break;
                case Op::Assign: // .assign(size, value) // prevValue
                    if constexpr ( !std::is_void_v<element> )
                        put(os, ".assign(").template putIndex<index_type>(os, offset).template putValue<type, member_type>(os, offset).put(os, ") // ").template putValue<type, member_type>(os, offset);
                    break;
                case Op::AssignDefault: // .assign(size, {}) // prevValue
                    put(os, ".assign(").template putIndex<index_type>(os, offset).template putValue<type, member_type>(os, offset).put(os, ", {}) // ").template putValue<type, member_type>(os, offset);
                    break;
                case Op::ClearSelections: // .clearSelections() // size, selIndexes
                {
                    auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                    put(os, "clearSelections() // ").put(os, size).put(os, ", ").template putIndexes<index_type>(os, offset, size);
                }
                break;
                case Op::SelectAll: // .selectAll() // size, selIndexes
                {
                    auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                    put(os, ".selectAll() // ").put(os, size).put(os, ", ").template putIndexes<index_type>(os, offset, size);
                }
                break;
                case Op::Select: // .select(index)
                    put(os, ".select(").template putIndex<index_type>(os, offset).put(os, ")");
                    break;
                case Op::SelectN: // .selectN(size, selIndexes)
                {
                    auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                    put(os, ".selectN(").put(os, size).put(os, ", ").template putIndexes<index_type>(os, offset, size).put(os, ")");
                }
                break;
                case Op::Deselect: // .deselect(index)
                    put(os, "deselect(").template putIndex<index_type>(os, offset);
                    break;
                case Op::DeselectN:
                {
                    auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                    put(os, "deselectN(").template putIndex<index_type>(os, offset).put(os, ", ").template putIndexes<index_type>(os, offset, size).put(os, ")");
                }
                break;
                case Op::ToggleSelection:
                    put(os, "toggelSel(").template putIndex<index_type>(os, offset).put(os, ")");
                    break;
                case Op::ToggleSelectionN:
                {
                    auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                    put(os, "toggleSelN(").put(os, size).put(os, ", ").template putIndexes<index_type>(os, offset, size).put(os, ")");
                }
                break;
                case Op::SortSelections:
                {
                    auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                    put(os, "sortSel(").put(os, size).put(os, ", ").template putIndexes<index_type>(os, offset, size).put(os, ")");
                }
                break;
                case Op::SortSelectionsDesc:
                {
                    auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                    put(os, "sortSelDesc(").put(os, size).put(os, ", ").template putIndexes<index_type>(os, offset, size).put(os, ")");
                }
                break;
                case Op::Set:
                    put(os, " = ").template putValue<type, member_type>(os, offset).put(os, " // ").template putValue<type, member_type>(os, offset);
                    break;
                case Op::SetN:
                if constexpr ( !std::is_void_v<element> )
                {
                    std::size_t size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                    put(os, "setN(").template putIndex<index_type>(os, offset).put(os, ", ").template putIndexes<index_type>(os, offset, size).put(os, " = ").template putValue<type, member_type>(os, offset)
                        .put(os, ") // ").template putValues<type, member_type>(os, offset, size);
                }
                break;
                case Op::SetL:
                    put(os, " = ").template putValue<type, member_type>(os, offset);
                    break;
                case Op::Append:
                if constexpr ( RareTs::is_specialization_v<type, std::vector> )
                    put(os, ".append(").template putValue<std::remove_cvref_t<decltype(std::declval<type>()[0])>, member_type>(os, offset).put(os, ")");
                break;
                case Op::AppendN:
                if constexpr ( RareTs::is_specialization_v<type, std::vector> )
                {
                    std::size_t size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                    put(os, ".appendN(").put(os, size).put(os, ", ").template putValues<element, member_type>(os, offset, size).put(os, ")");
                }
                break;
                case Op::Insert:
                if constexpr ( RareTs::is_specialization_v<type, std::vector> )
                    put(os, ".insert(").template putIndex<index_type>(os, offset).put(os, ", ").template putValue<std::remove_cvref_t<decltype(std::declval<type>()[0])>, member_type>(os, offset).put(os, ")");
                break;
                case Op::InsertN:
                if constexpr ( RareTs::is_specialization_v<type, std::vector> )
                {
                    std::size_t size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                    put(os, ".insertN(").template putIndex<index_type>(os, offset).put(os, ", ").put(os, size).put(os, ", ").template putValues<element, member_type>(os, offset, size).put(os, ")");
                }
                break;
                case Op::Remove:
                if constexpr ( RareTs::is_specialization_v<type, std::vector> )
                    put(os, ".remove(").template putIndex<index_type>(os, offset).put(os, ") // {").template putValue<std::remove_cvref_t<decltype(std::declval<type>()[0])>, member_type>(os, offset).put(os, ")");
                break;
                case Op::RemoveN:
                if constexpr ( RareTs::is_specialization_v<type, std::vector> )
                {
                    std::size_t count = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                    put(os, ".removeN(").template putIndexes<index_type>(os, offset, count).put(os, ") // ").template putIndexes<index_type>(os, offset, count);
                }
                break;
                case Op::RemoveL:
                if constexpr ( RareTs::is_specialization_v<type, std::vector> )
                {
                    std::size_t count = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                    put(os, ".removeL() // ").put(os, count).put(os, ", ").template putIndexes<index_type>(os, offset, count).put(os, ") // ").template putValues<element, member_type>(os, offset, count);
                }
                break;
                case Op::Sort:
                {
                    std::size_t count = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                    put(os, ".sort() // ").put(os, count).template putIndexes<index_type>(os, offset, count);
                }
                break;
                case Op::SortDesc:
                {
                    std::size_t count = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                    put(os, ".sortDesc() // ").put(os, count).template putIndexes<index_type>(os, offset, count);
                }
                break;
                case Op::Swap:
                    put(os, ".swap(").template putIndex<index_type>(os, offset).put(os, ", ").template putIndex<index_type>(os, offset).put(os, ")");
                    break;
                case Op::MoveUp:
                    put(os, ".moveUp(").template putIndex<index_type>(os, offset).put(os, ")");
                    break;
                case Op::MoveUpN:
                {
                    auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                    put(os, ".moveUpN(").put(os, size).put(os, ", ").template putIndexes<index_type>(os, offset, size).put(os, ")");
                }
                break;
                case Op::MoveUpL:
                    put(os, ".moveSelectionsUp()");
                    break;
                case Op::MoveTop:
                    put(os, ".moveTop(").template putIndex<index_type>(os, offset).put(os, ")");
                    break;
                case Op::MoveTopN:
                {
                    auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                    put(os, ".moveTopN(").put(os, size).put(os, ", ").template putIndexes<index_type>(os, offset, size).put(os, ")");
                }
                break;
                case Op::MoveTopL:
                    put(os, ".moveSelectionsTop()");
                    break;
                case Op::MoveDown:
                    put(os, ".moveDown(").template putIndex<index_type>(os, offset).put(os, ")");
                    break;
                case Op::MoveDownN:
                {
                    auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                    put(os, ".moveDownN(").put(os, size).put(os, ", ").template putIndexes<index_type>(os, offset, size).put(os, ")");
                }
                break;
                case Op::MoveDownL:
                    put(os, ".moveSelectionsDown()");
                    break;
                case Op::MoveBottom:
                    put(os, ".moveBottom(").template putIndex<index_type>(os, offset).put(os, ")");
                    break;
                case Op::MoveBottomN:
                {
                    auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                    put(os, ".moveBottomN(").put(os, size).put(os, ", ").template putIndexes<index_type>(os, offset, size).put(os, ")");
                }
                break;
                case Op::MoveBottomL:
                    put(os, ".moveSelectionsBottom()");
                    break;
                case Op::MoveTo:
                    put(os, ".moveTo(").template putIndex<index_type>(os, offset).put(os, ", ").template putIndex<index_type>(os, offset).put(os, ")");
                    break;
                case Op::MoveToN:
                {
                    auto target = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                    auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                    put(os, ".moveToN(").put(os, target).put(os, ", ").put(os, size).put(os, ", ").template putIndexes<index_type>(os, offset, size).put(os, ")");
                }
                break;
                case Op::MoveToL:
                    put(os, ".moveSelectionsTo(").template putIndex<index_type>(os, offset);
                    break;
            }
        }

        template <class U, class Member = void, std::size_t ... Is>
        void printEvent(std::size_t & offset, std::uint8_t op, std::index_sequence<Is...>) const
        {
            using base_index_type = index_type_t<typename edit_root::default_index_type, Member>;
            std::uint8_t value = editable.events[offset];
            ++offset;
            
            switch ( PathOp(value & std::uint8_t(PathOp::HighBits)) )
            {
                case PathOp::SelBranch:
                    if constexpr ( RareTs::is_static_array_v<U> || RareTs::is_specialization_v<U, std::vector> )
                    {
                        std::cout << "[{sel}]";
                        using element_type = RareTs::element_type_t<std::remove_cvref_t<U>>;
                        constexpr bool isLeaf = !RareTs::is_reflected_v<element_type> && std::is_void_v<RareTs::element_type_t<element_type>>;
                        if constexpr ( isLeaf )
                            printEventOp<element_type, Member>(offset, Op(op));
                        else
                            printEvent<std::remove_cvref_t<decltype(std::declval<U>()[0])>, Member>(offset, op, std::make_index_sequence<reflectedMemberCount<std::remove_cvref_t<decltype(std::declval<U>()[0])>>()>());
                    }
                    break;
                case PathOp::LeafSelBranch:
                    if constexpr ( std::is_same_v<std::remove_cvref_t<Data>, std::remove_cvref_t<U>> ) // Root
                        printEventOp<U, Member>(offset, Op(op));
                    else if constexpr ( RareTs::is_static_array_v<U> || RareTs::is_specialization_v<U, std::vector> )
                    {
                        std::cout << "[{sel}]";
                        using element_type = RareTs::element_type_t<std::remove_cvref_t<U>>;
                        printEventOp<element_type, Member>(offset, Op(op));
                    }
                    break;
                case PathOp::Branch:
                {
                    if constexpr ( RareTs::is_static_array_v<U> || RareTs::is_specialization_v<U, std::vector> ) // Branch to index
                    {
                        base_index_type index {};
                        if constexpr ( std::is_same_v<uint6_t, base_index_type> )
                            index = static_cast<base_index_type>(value & std::uint8_t(PathOp::LowBits));
                        else
                            index = editable.template readIndex<base_index_type>(offset);

                        std::cout << '[' << static_cast<std::size_t>(index) << ']';
                        printEvent<std::remove_cvref_t<decltype(std::declval<U>()[0])>, Member>(offset, op, std::make_index_sequence<reflectedMemberCount<std::remove_cvref_t<decltype(std::declval<U>()[0])>>()>());
                    }
                    else if constexpr ( RareTs::is_optional_v<U> ) // Deref into optional
                    {
                        printEvent<typename U::value_type, Member>(offset, op, std::make_index_sequence<reflectedMemberCount<std::remove_cvref_t<typename U::value_type>>()>());
                    }
                    else if constexpr ( RareTs::is_macro_reflected_v<U> ) // Branch to field
                    {
                        std::size_t memberIndex = std::size_t(value & std::uint8_t(PathOp::LowBits));
                        (void)(
                            (Is == memberIndex ? (
                                std::cout << "." << RareTs::Member<U, Is>::name,
                                printEvent<std::remove_cvref_t<typename RareTs::Member<U, Is>::type>, RareTs::Member<U, Is>>(offset, op, std::make_index_sequence<reflectedMemberCount<std::remove_cvref_t<typename RareTs::Member<U, Is>::type>>()>()),
                                true) : true) && ...
                        );

                        //RareTs::Members<U>::at(std::size_t(value & std::uint8_t(PathOp::LowBits)), [&](auto member) {
                        //    std::cout << "." << member.name;
                        //    printEvent<std::remove_cvref_t<typename decltype(member)::type>, decltype(member)>(offset, op, std::make_index_sequence<reflectedMemberCount<U>()>());
                        //});
                    }
                }
                break;
                case PathOp::LeafBranch:
                {
                    if constexpr ( RareTs::is_static_array_v<U> || RareTs::is_specialization_v<U, std::vector> ) // Op on index
                    {
                        base_index_type index {};
                        if constexpr ( std::is_same_v<uint6_t, base_index_type> )
                            index = static_cast<base_index_type>(value & std::uint8_t(PathOp::LowBits));
                        else
                            index = editable.template readIndex<base_index_type>(offset);

                        std::cout << "[" << static_cast<std::size_t>(index) << "]";
                        printEventOp<RareTs::element_type_t<std::remove_cvref_t<U>>, Member>(offset, Op(op));
                    }
                    else if constexpr ( RareTs::is_optional_v<U> ) // Operate on optional
                    {
                        printEventOp<typename U::value_type, Member>(offset, Op(op));
                    }
                    else if constexpr ( RareTs::is_reflected_v<U> ) // Op on field
                    {
                        std::size_t memberIndex = std::size_t(value & std::uint8_t(PathOp::LowBits));
                        (void)(
                            (Is == memberIndex ? (
                                std::cout << "." << RareTs::Member<U, Is>::name,
                                printEventOp<typename RareTs::Member<U, Is>::type, RareTs::Member<U, Is>>(offset, Op(op)),
                                true) : true) && ...
                        );
                        //RareTs::Members<U>::at(memberIndex, [&](auto member) {
                        //    std::cout << "." << member.name;
                        //    printEventOp<typename decltype(member)::type, decltype(member)>(offset, Op(value));
                        //});
                    }
                }
                break;
                default: throw std::exception(); // Unreachable
            }
        }

        template <class U, class Member = void>
        void printEvent(std::size_t & offset) const
        {
            std::uint8_t op = editable.events[offset];
            ++offset;
            printEvent<U, Member>(offset, op, std::make_index_sequence<reflectedMemberCount<U>()>());
        }

        void printChangeHistory() const
        {
            std::size_t totalActions = actionFirstEvent.size();
            for ( std::size_t actionIndex=0; actionIndex<totalActions; ++actionIndex )
            {
                std::size_t currActionStart = actionFirstEvent[actionIndex];
                if ( (currActionStart & flagElidedRedos) == flagElidedRedos )
                {
                    auto elisionCount = currActionStart & maskElidedRedoSize;
                    std::cout << "Action[" << actionIndex << "] marks the previous " << elisionCount << " action(s) as elided redos\n";
                    continue;
                }

                std::size_t nextActionStart = flagElidedRedos;
                for ( std::size_t i=1; (nextActionStart & flagElidedRedos) == flagElidedRedos; ++i ) // Find the next event that isn't an elision marker
                    nextActionStart = actionIndex+i<totalActions ? (actionFirstEvent[actionIndex+i]) : editable.eventOffsets.size();

                if ( currActionStart == nextActionStart )
                {
                    std::cout << "Action[" << actionIndex << "] is blank\n";
                    continue;
                }

                std::cout << "Action[" << actionIndex << "] contains events [" << currActionStart << ", " << nextActionStart << ")\n";
                for ( std::size_t eventIndex = currActionStart; eventIndex < nextActionStart; ++eventIndex )
                {
                    auto [eventOffset, byteEnd] = editable.getEventOffsetRange(eventIndex);

                    std::cout << "  [" << std::setw(2) << eventIndex << ','
                        << std::setw(3) << eventOffset << ','
                        << std::setw(2) << (byteEnd-editable.eventOffsets[eventIndex]) << "] " << std::hex << std::uppercase;
                    for ( std::size_t i=eventOffset; i<byteEnd; ++i )
                        std::cout << (editable.events[i] <= 0xF ? "0" : "") << int(editable.events[i]) << ' ';
                    std::cout << std::nouppercase << std::dec;

                    std::cout << " // edit";
                    printEvent<Data>(eventOffset);
                    std::cout << '\n';
                }
                std::cout << '\n';
            }
        }

        REFLECT(Tracked, flagElidedRedos, maskElidedRedoSize, editable, actionFirstEvent, actionReferenceCount, redoCount, redoSize)
    };

    template <typename T, typename PathToElem, typename EditorType, typename ReadEditPair>
    class TrackedElement : public ReadEditPair
    {
        using keys_type = typename PathToElem::keys;
        using tracked_type = typename EditorType::tracked_type;

        TrackedElement(EditorType action, keys_type keys)
            : ReadEditPair{action.editFromPath(PathToElem{keys})}, action(std::move(action)), view(ReadEditPair::edit) {}

    public:

        EditorType action;
        const ReadEditPair::edit_type & view;

        TrackedElement(tracked_type* tracked, keys_type keys) : TrackedElement(tracked->operator()(), keys) {}

        template <class ... Ts> TrackedElement(TrackedElement<Ts...>* parentElement, keys_type keys)
            : TrackedElement(parentElement->action.parent->operator()(), keys) {}
    
        TrackedElement(TrackedElement && other)
            : ReadEditPair{std::move((ReadEditPair &)other)}, action(std::move(other.action)) {}

        constexpr const T* operator->() const { return &(ReadEditPair::read); }
        constexpr auto operator()() { return &(ReadEditPair::edit); }
    };
}

#endif