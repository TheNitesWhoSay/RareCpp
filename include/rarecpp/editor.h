#ifndef EDITOR_H
#define EDITOR_H
#include <algorithm>
#include <bitset>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <optional>
#include <tuple>
#include <type_traits>
#include <unordered_set>
#include <vector>
#include "reflect.h"

#include <iostream> // TODO: Temporary includes
#include <iomanip>
#include "json.h"

namespace RareEdit
{
    template <typename SizeType> struct IndexSize { using type = SizeType; };

    template <typename T>
    concept has_default_index_type = requires { typename T::default_index_type; };

    template <typename T>
    constexpr auto defaultIndexType()
    {
        if constexpr ( RareTs::Notes<T>::template hasNote<RareEdit::IndexSize>() )
            return RareTs::Notes<T>::template getNote<RareEdit::IndexSize>();
        else
            return std::type_identity<std::size_t>{};
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
    void readSelectionVector(const std::vector<std::uint8_t> & data, std::size_t & offset, std::vector<T> & integralVector)
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
    }

    template <typename T>
    void writeSelections(std::vector<std::uint8_t> & data, const T & t)
    {
        if constexpr ( !RareTs::is_static_array_v<std::remove_cvref_t<T>> )
            writeSelectionVector(data, t);
    }

    template <typename T>
    void readSelections(const std::vector<std::uint8_t> & data, std::size_t & offset, T & t)
    {
        if constexpr ( !std::is_null_pointer_v<std::remove_cvref_t<T>> )
            readSelectionVector(data, offset, t);
    }

    namespace u8bool
    {
        static constexpr std::uint8_t False = 0;
        static constexpr std::uint8_t True = 1;

        static inline bool read(const std::vector<std::uint8_t> & data, std::size_t & offset)
        {
            return data[offset++] != 0;
        }

        static inline void write(std::vector<std::uint8_t> & data, bool value)
        {
            std::uint8_t write = value ? 1 : 0;
            data.insert(data.end(), reinterpret_cast<const std::uint8_t*>(&write), reinterpret_cast<const std::uint8_t*>(&write)+sizeof(write));
        }

        static inline void writeTrue(std::vector<std::uint8_t> & data)
        {
            data.insert(data.end(), &True, &True+sizeof(True));
        }

        static inline void writeFalse(std::vector<std::uint8_t> & data)
        {
            data.insert(data.end(), &False, &False+sizeof(False));
        }
    };

    struct uint6_t // A 6-bit unsigned int [0, 64); as the high bits are available can be combined with path bits
    {
        std::uint8_t value = 0;
        
        uint6_t() = default;
        template <typename T> constexpr uint6_t(T && t) : value(static_cast<std::uint8_t>(t)) {}
        
        constexpr operator std::ptrdiff_t() { return static_cast<std::ptrdiff_t>(value); };
        constexpr operator std::size_t() { return static_cast<std::size_t>(value); };
        constexpr operator std::uint8_t() { return static_cast<std::uint8_t>(value); };
        template <typename T> constexpr auto operator==(T other) { return value == static_cast<std::uint8_t>(other); }
        template <typename T> constexpr auto operator<=>(T other) { return value <=> static_cast<std::uint8_t>(other); }
        constexpr auto operator &() { return &value; }
        constexpr auto & operator++() { ++value; return *this; }

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
    void undoSort(T & items, std::vector<I> & sourceIndexes)
    {
        std::size_t total = items.size();
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
    void redoSort(T & items, std::vector<I> & sourceIndexes)
    {
        std::size_t total = items.size();
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

    template <class PathTuple, class Value> class path_append {
        template <class ...> struct impl { using type = void; };
        template <class T, class ... Ts> struct impl<T, std::tuple<Ts...>> { using type = std::tuple<Ts..., T>; };
        template <class ... Ts> struct impl<void, std::tuple<Ts...>> { using type = void; };
    public:
        using type = typename impl<Value, PathTuple>::type;
    };
    template <class PathTuple, class Value> using path_append_t = typename path_append<PathTuple, Value>::type;

    template <class Pathway, class IndexTypes>
    struct PathTaggedIndexes
    {
        using Path = Pathway;
        using Indexes = IndexTypes;

        Indexes indexes;
    };

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

    enum class PathOp : std::uint8_t {
        
        /* The meaning of the lower 6-bits depends on the selection bit (second highest bit)...
             Branch: identifies a field or array index (for array sizes <= 64) which you'll be branching from
                     if array/container requires an index larger than 6 bits, then the next sizeof(containerIndex) bytes are the container index
             SelBranch: lower 6-bits unused, SelBranch indicates the operation applies over the selection for this container (may branch further down from here)
             LeafBranch: same as Branch except this also indicates it's the last branch in the sequence
             LeafSelBranch: same as SelBranch except this also indicates it's the last branch in the sequence */
        HighBits      = 0b11000000,
        LowBits       = 0b00111111,

        SelMask       = 0b01000000,
        LeafMask      = 0b10000000,

        Branch        = 0b00000000,
        SelBranch     = 0b01000000,
        LeafBranch    = 0b10000000,
        LeafSelBranch = 0b11000000
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

    template <class IndexTypeTuple>
    struct Indexes : IndexTypeTuple
    {
        using index_types = IndexTypeTuple;
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

    template <class T> struct is_selection_set {
        template <std::size_t ... Is> static constexpr auto impl(std::index_sequence<Is...>)
            -> std::bool_constant<(std::is_same_v<PathSelections, std::tuple_element_t<Is, T>> || ...)>;
        static constexpr bool value = decltype(impl(std::make_index_sequence<std::tuple_size_v<T>>()))::value;
    };
    template <> struct is_selection_set<std::tuple<>> { static constexpr bool value = false; };
    template <class T> inline constexpr bool is_selection_set_v = is_selection_set<T>::value; 

    template <class Edit, class default_index_type, class RootData, class T, class IndexTypeTuple, class Pathway>
    class EditPrimitive : public Indexes<IndexTypeTuple>
    {
        Edit & root;

    public:
        using Path = Pathway;

        using type = T;

        EditPrimitive(Edit & root, IndexTypeTuple indexes) :
            Indexes<IndexTypeTuple> {std::move(indexes)}, root(root) {}
        template <class U> void operator=(U && value) {
            root.template set<Pathway>(std::forward<U>(value), (IndexTypeTuple &)(*this));
        }
    };

    template <class Edit, class default_index_type, class RootData, class T, class IndexTypeTuple, class Pathway, std::size_t I, std::size_t ... Is>
    static constexpr auto editMember(std::index_sequence<Is...>);

    template <class Edit, class default_index_type, class RootData, class T, class IndexTypeTuple, class Pathway> struct edit_member
    {
        template <std::size_t I> using type = typename decltype(editMember<Edit, default_index_type, RootData, T, IndexTypeTuple, Pathway, I>(std::make_index_sequence<std::tuple_size_v<Pathway>>()))::type;
    };

    template <class Edit, class default_index_type, class RootData, class T, class IndexTypeTuple, class Pathway, std::size_t ... Is>
    struct EditMembers : Indexes<IndexTypeTuple>, RareTs::Class::adapt_member<edit_member<Edit, default_index_type, RootData, T, IndexTypeTuple, Pathway>::template type, T, Is>...
    {
        EditMembers(Edit & root, IndexTypeTuple indexes) :
            Indexes<IndexTypeTuple> {indexes},
            RareTs::Class::template adapt_member<edit_member<Edit, default_index_type, RootData, T, IndexTypeTuple, Pathway>::template type, T, Is> {{ root, indexes }}...,
            root(root) {}
    private:
        Edit & root;
    };

    template <class Edit, class default_index_type, class RootData, class T, class IndexTypeTuple, class Pathway, std::size_t... Is>
    EditMembers<Edit, default_index_type, RootData, T, IndexTypeTuple, Pathway, Is...> editMembers(std::index_sequence<Is...>);

    template <class Edit, class default_index_type, class RootData, class T, class IndexTypeTuple = std::tuple<>, class Pathway = std::tuple<>>
    using edit_members = decltype(editMembers<Edit, default_index_type, RootData, T, IndexTypeTuple, Pathway>(std::make_index_sequence<RareTs::Members<T>::total>()));

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
            else if constexpr ( Member::template hasNote<RareEdit::IndexSize>() )
                return Member::template getNote<RareEdit::IndexSize>();
            else
                return std::type_identity<DefaultIndexType>{};
        }
        else
            return std::type_identity<DefaultIndexType>{};
    };

    template <class DefaultIndexType, class Member>
    using index_type_t = typename decltype(RareEdit::template index_typer<DefaultIndexType, Member>())::type;

    template <class Edit, class default_index_type, class RootData, class T, class IndexTypeTuple, class Pathway, template <class...> class SubElement>
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
                static_assert(sizeof...(PathElements) > 0, "The pathway passed to get member cannot end with an index!");
                if constexpr ( sizeof...(PathElements) > 0 )
                    return getMemberImpl<std::remove_cvref_t<decltype(std::declval<U>()[0])>, LastMember, PathElements...>();
            }
        }

        using Member = typename RareTs::remove_cvref_t<decltype([]<std::size_t ... Is>(std::index_sequence<Is...>) {
            return getMemberImpl<RootData, void, std::tuple_element_t<Is, Pathway>...>();
        }(std::make_index_sequence<std::tuple_size_v<Pathway>>()))>::type;

        class RandomAccess : public Indexes<IndexTypeTuple>
        {

            using index_type = index_type_t<default_index_type, Member>;

            template <std::size_t... Is, size_t... Js>
            static constexpr auto selectionOpType(std::index_sequence<Is...>, std::index_sequence<Js...>) -> SubElement<Edit, default_index_type, RootData, T,
                std::tuple<std::tuple_element_t<Is, IndexTypeTuple>...>,
                std::tuple<std::tuple_element_t<Js, Pathway>..., PathSelections>>;
        
            template <std::size_t... Is, size_t... Js>
            static constexpr auto arrayOpType(std::index_sequence<Is...>, std::index_sequence<Js...>) -> SubElement<Edit, default_index_type, RootData, T,
                std::tuple<std::tuple_element_t<Is, IndexTypeTuple>..., index_type>,
                std::tuple<std::tuple_element_t<Js, Pathway>..., PathIndex<sizeof...(Is)>>>;

        protected:
            Edit & root;

            constexpr auto & memberRef() { return root.template getMemberReference<Pathway>(root.t, (Indexes<IndexTypeTuple> &)(*this)); }

        public:
            using Path = Pathway;

            using selection_op_type = decltype(selectionOpType(std::make_index_sequence<std::tuple_size_v<IndexTypeTuple>>(),
                std::make_index_sequence<std::tuple_size_v<Pathway>>()));
            using array_op_type = decltype(arrayOpType(std::make_index_sequence<std::tuple_size_v<IndexTypeTuple>>(),
                std::make_index_sequence<std::tuple_size_v<Pathway>>()));

            constexpr const auto & sel() const { return root.template getSelections<Pathway>(); }

            inline void clearSelections()
            {
                auto & sel = root.template getSelections<Pathway>();
                root.eventOffsets.push_back(root.events.size());
                root.events.push_back(uint8_t(Op::ClearSelections));
                root.template serializePathway<Pathway>((IndexTypeTuple &)(*this));

                writeSelectionVector(root.events, sel);
                RareTs::clear(sel);

                if constexpr ( Edit::template hasSelectionsChangedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>> )
                    root.user.selectionsChanged(PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{(Indexes<IndexTypeTuple> &)(*this)});
            }

            inline void selectAll()
            {
                auto & sel = root.template getSelections<Pathway>();
                root.eventOffsets.push_back(root.events.size());
                root.events.push_back(uint8_t(Op::SelectAll));
                root.template serializePathway<Pathway>((IndexTypeTuple &)(*this));

                writeSelectionVector(root.events, sel);
                RareTs::clear(sel);
                sel.assign(memberRef().size(), 0);
                std::iota(sel.begin(), sel.end(), (RareTs::element_type_t<RareTs::remove_cvref_t<decltype(sel)>>)0);

                if constexpr ( Edit::template hasSelectionsChangedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>> )
                    root.user.selectionsChanged(PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{(Indexes<IndexTypeTuple> &)(*this)});
            }

            inline void select(index_type i) // i must not be selected
            {
                auto & sel = root.template getSelections<Pathway>();
                root.eventOffsets.push_back(root.events.size());
                root.events.push_back(uint8_t(Op::Select));
                root.template serializePathway<Pathway>((IndexTypeTuple &)(*this));

                root.events.insert(root.events.end(), reinterpret_cast<const std::uint8_t*>(&i), reinterpret_cast<const std::uint8_t*>(&i)+sizeof(i));
                if ( std::find(sel.begin(), sel.end(), i) == sel.end() )
                    RareTs::append(sel, i);
                else
                    throw std::invalid_argument("Cannot select an index that is already selected");

                if constexpr ( Edit::template hasSelectionsChangedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>> )
                    root.user.selectionsChanged(PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{(Indexes<IndexTypeTuple> &)(*this)});
            }

            inline void select(const std::vector<index_type> & addedSelections) // addedSelections must not be selected
            {
                auto & sel = root.template getSelections<Pathway>();
                root.eventOffsets.push_back(root.events.size());
                root.events.push_back(uint8_t(Op::SelectN));
                root.template serializePathway<Pathway>((IndexTypeTuple &)(*this));

                index_type size = static_cast<index_type>(addedSelections.size());
                root.events.insert(root.events.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));
                root.events.insert(root.events.end(), reinterpret_cast<const std::uint8_t*>(&(addedSelections[0])),
                    reinterpret_cast<const std::uint8_t*>(&(addedSelections[0]))+sizeof(addedSelections[0])*static_cast<std::size_t>(size));

                for ( auto i : addedSelections )
                {
                    if ( std::find(sel.begin(), sel.end(), i) == sel.end() )
                        RareTs::append(sel, i);
                    else
                        throw std::invalid_argument("Cannot select an index that is already selected");
                }
                if constexpr ( Edit::template hasSelectionsChangedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>> )
                    root.user.selectionsChanged(PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{(Indexes<IndexTypeTuple> &)(*this)});
            }

            inline void deselect(index_type i) // i must be selected
            {
                auto & sel = root.template getSelections<Pathway>();
                root.eventOffsets.push_back(root.events.size());
                root.events.push_back(uint8_t(Op::Deselect));
                root.template serializePathway<Pathway>((IndexTypeTuple &)(*this));
                
                auto found = std::find(sel.begin(), sel.end(), i);
                index_type foundAt = static_cast<index_type>(std::distance(std::begin(sel), found));
                root.events.insert(root.events.end(), reinterpret_cast<const std::uint8_t*>(&i), reinterpret_cast<const std::uint8_t*>(&i)+sizeof(i));
                root.events.insert(root.events.end(), reinterpret_cast<const std::uint8_t*>(&foundAt), reinterpret_cast<const std::uint8_t*>(&foundAt)+sizeof(foundAt));

                if ( found == sel.end() )
                    throw std::invalid_argument("Cannot deselect an index that is not selected");
                else
                    sel.erase(found);

                if constexpr ( Edit::template hasSelectionsChangedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>> )
                    root.user.selectionsChanged(PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{(Indexes<IndexTypeTuple> &)(*this)});
            }

            inline void deselect(const std::vector<index_type> & removedSelections) // removeSelections must be selected
            {
                auto & sel = root.template getSelections<Pathway>();
                root.eventOffsets.push_back(root.events.size());
                root.events.push_back(uint8_t(Op::DeselectN));
                root.template serializePathway<Pathway>((IndexTypeTuple &)(*this));

                auto numRemoved = removedSelections.size();
                index_type size = static_cast<index_type>(numRemoved);
                root.events.insert(root.events.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));
                root.events.insert(root.events.end(), reinterpret_cast<const std::uint8_t*>(&removedSelections[0]),
                    reinterpret_cast<const std::uint8_t*>(&removedSelections[0])+sizeof(removedSelections[0])*size);

                for ( auto i : removedSelections )
                {
                    auto found = std::find(sel.begin(), sel.end(), i);
                    if ( found != sel.end() )
                    {
                        index_type foundAt = static_cast<index_type>(std::distance(std::begin(sel), found));
                        root.events.insert(root.events.end(), reinterpret_cast<const std::uint8_t*>(&foundAt), reinterpret_cast<const std::uint8_t*>(&foundAt)+sizeof(foundAt));
                        sel.erase(found);
                    }
                    else
                        throw std::invalid_argument("Cannot deselect an index that is not selected");
                }
                if constexpr ( Edit::template hasSelectionsChangedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>> )
                    root.user.selectionsChanged(PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{(Indexes<IndexTypeTuple> &)(*this)});
            }

            inline void toggleSelected(index_type i)
            {
                auto & sel = root.template getSelections<Pathway>();
                root.eventOffsets.push_back(root.events.size());
                root.events.push_back(uint8_t(Op::ToggleSelection));
                root.template serializePathway<Pathway>((IndexTypeTuple &)(*this));
                root.events.insert(root.events.end(), reinterpret_cast<const std::uint8_t*>(&i), reinterpret_cast<const std::uint8_t*>(&i)+sizeof(i));
                auto found = std::find(sel.begin(), sel.end(), i);
                u8bool::write(root.events, found != sel.end());
                if ( found != sel.end() )
                {
                    index_type foundAt = static_cast<index_type>(std::distance(std::begin(sel), found));
                    root.events.insert(root.events.end(), reinterpret_cast<const std::uint8_t*>(&foundAt), reinterpret_cast<const std::uint8_t*>(&foundAt)+sizeof(foundAt));
                    std::erase(sel, i);
                }
                else
                    RareTs::append(sel, i);

                if constexpr ( Edit::template hasSelectionsChangedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>> )
                    root.user.selectionsChanged(PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{(Indexes<IndexTypeTuple> &)(*this)});
            }

            inline void toggleSelected(const std::vector<index_type> & toggledSelections)
            {
                auto & sel = root.template getSelections<Pathway>();
                root.eventOffsets.push_back(root.events.size());
                root.events.push_back(uint8_t(Op::ToggleSelectionN));
                root.template serializePathway<Pathway>((IndexTypeTuple &)(*this));

                auto numToggled = toggledSelections.size();
                index_type size = static_cast<index_type>(numToggled);
                root.events.insert(root.events.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));
                root.events.insert(root.events.end(), reinterpret_cast<const std::uint8_t*>(&toggledSelections[0]),
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
                writeVecBoolData(root.events, wasSelected);
                if ( !prevSelIndexes.empty() )
                {
                    root.events.insert(root.events.end(), reinterpret_cast<const std::uint8_t*>(&prevSelIndexes[0]),
                        reinterpret_cast<const std::uint8_t*>(&prevSelIndexes[0])+sizeof(prevSelIndexes[0])*std::size(prevSelIndexes));
                }

                if constexpr ( Edit::template hasSelectionsChangedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>> )
                    root.user.selectionsChanged(PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{(Indexes<IndexTypeTuple> &)(*this)});
            }

            inline void sortSelection()
            {
                auto & sel = root.template getSelections<Pathway>();
                root.eventOffsets.push_back(root.events.size());
                root.events.push_back(uint8_t(Op::SortSelections));
                root.template serializePathway<Pathway>((IndexTypeTuple &)(*this));

                auto sourceIndexes = trackedSort<false, index_type>(sel);
                index_type serializedSize = static_cast<index_type>(sourceIndexes.size());
                root.events.insert(root.events.end(), reinterpret_cast<const std::uint8_t*>(&serializedSize),
                    reinterpret_cast<const std::uint8_t*>(&serializedSize)+sizeof(serializedSize));

                for ( auto index : sourceIndexes )
                {
                    index_type sourceIndex = static_cast<index_type>(index);
                    root.events.insert(root.events.end(), reinterpret_cast<const std::uint8_t*>(&sourceIndex),
                        reinterpret_cast<const std::uint8_t*>(&sourceIndex)+sizeof(sourceIndex));
                }
                if constexpr ( Edit::template hasSelectionsChangedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>> )
                    root.user.selectionsChanged(PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{(Indexes<IndexTypeTuple> &)(*this)});
            }

            inline void sortSelectionDescending()
            {
                auto & sel = root.template getSelections<Pathway>();
                root.eventOffsets.push_back(root.events.size());
                root.events.push_back(uint8_t(Op::SortSelectionsDesc));
                root.template serializePathway<Pathway>((IndexTypeTuple &)(*this));

                auto sourceIndexes = trackedSort<true, index_type>(sel);
                index_type serializedSize = static_cast<index_type>(sourceIndexes.size());
                root.events.insert(root.events.end(), reinterpret_cast<const std::uint8_t*>(&serializedSize),
                    reinterpret_cast<const std::uint8_t*>(&serializedSize)+sizeof(serializedSize));

                for ( auto index : sourceIndexes )
                {
                    index_type sourceIndex = static_cast<index_type>(index);
                    root.events.insert(root.events.end(), reinterpret_cast<const std::uint8_t*>(&sourceIndex),
                        reinterpret_cast<const std::uint8_t*>(&sourceIndex)+sizeof(sourceIndex));
                }
                if constexpr ( Edit::template hasSelectionsChangedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>> )
                    root.user.selectionsChanged(PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{(Indexes<IndexTypeTuple> &)(*this)});
            }
            
            constexpr auto selection() { return selection_op_type(root, (std::add_lvalue_reference_t<typename Indexes<IndexTypeTuple>::index_types>)(*this)); }

            RandomAccess(Edit & root, IndexTypeTuple && indexes) : Indexes<IndexTypeTuple> {std::move(indexes)}, root(root) {}

            auto operator[](std::size_t i) {
                #ifdef __INTELLISENSE__ // While gcc, clang and MSVC compilers accept using index_type, intellisense does not and complains IndexTypeTuple is incomplete
                return array_op_type(root, std::tuple_cat((std::add_lvalue_reference_t<IndexTypeTuple>)(*this), std::tuple(i)));
                #else
                return array_op_type(root, std::tuple_cat((std::add_lvalue_reference_t<IndexTypeTuple>)(*this), std::tuple{static_cast<index_type>(i)}));
                #endif
            }
        };

    public:
        struct Array : RandomAccess
        {
            Array(Edit & root, IndexTypeTuple indexes) : RandomAccess { root, std::move(indexes) } {}
        };

        struct Vector : RandomAccess
        {
            Vector(Edit & root, IndexTypeTuple indexes) : RandomAccess { root, std::move(indexes) } {}
            void reset() { RandomAccess::root.template reset<Pathway>((IndexTypeTuple &)(*this)); }
            void reserve(std::size_t size) { RandomAccess::root.template reserve<Pathway>(size, (IndexTypeTuple &)(*this)); }
            void trim() { RandomAccess::root.template trim<Pathway>((IndexTypeTuple &)(*this)); }
            void assignDefault(std::size_t size) { RandomAccess::root.template assignDefault<Pathway>(size, (IndexTypeTuple &)(*this)); }
            template <class U> void assign(std::size_t size, U && value) { RandomAccess::root.template assign<Pathway>(size, std::forward<U>(value), (IndexTypeTuple &)(*this)); }
            template <class U> void operator=(U && value) { RandomAccess::root.template set<Pathway>(std::forward<U>(value), (IndexTypeTuple &)(*this)); }
            template <class SetIndexes, class Value> void set(SetIndexes && indexes, Value && value) { RandomAccess::root.template set<Pathway>(indexes, std::forward<Value>(value), (IndexTypeTuple &)(*this)); }
            template <class U> void append(U && value) {
                if constexpr ( RareTs::is_iterable_v<std::remove_cvref_t<U>> )
                    RandomAccess::root.template appendN<Pathway>(std::forward<U>(value), (IndexTypeTuple &)(*this));
                else
                    RandomAccess::root.template append<Pathway>(std::forward<U>(value), (IndexTypeTuple &)(*this));
            }
            template <class InsertionIndex, class Value> void insert(InsertionIndex insertionIndex, Value && value) {
                if constexpr ( RareTs::is_iterable_v<std::remove_cvref_t<Value>> )
                    RandomAccess::root.template insertN<Pathway>(insertionIndex, std::forward<Value>(value), (IndexTypeTuple &)(*this));
                else
                    RandomAccess::root.template insert<Pathway>(insertionIndex, std::forward<Value>(value), (IndexTypeTuple &)(*this));
            }
            template <class RemovalIndex> void remove(RemovalIndex removalIndex) {
                if constexpr ( RareTs::is_iterable_v<std::remove_cvref_t<RemovalIndex>> )
                    RandomAccess::root.template removeN<Pathway>(removalIndex, (IndexTypeTuple &)(*this));
                else
                    RandomAccess::root.template remove<Pathway>(removalIndex, (IndexTypeTuple &)(*this));
            }
            void sort() { RandomAccess::root.template sort<Pathway>((IndexTypeTuple &)(*this)); }
            void sortDesc() { RandomAccess::root.template sortDesc<Pathway>((IndexTypeTuple &)(*this)); }
            void removeSelection() { RandomAccess::root.template removeL<Pathway>((IndexTypeTuple &)(*this)); }

            template <class U> void moveUp(U && movedIndex) {
                if constexpr ( RareTs::is_iterable_v<std::remove_cvref_t<U>> )
                    RandomAccess::root.template moveUpN<Pathway>(std::forward<U>(movedIndex), (IndexTypeTuple &)(*this));
                else
                    RandomAccess::root.template moveUp<Pathway>(std::forward<U>(movedIndex), (IndexTypeTuple &)(*this));
            }
            template <class U> void moveTop(U && movedIndex) {
                if constexpr ( RareTs::is_iterable_v<std::remove_cvref_t<U>> )
                    RandomAccess::root.template moveTopN<Pathway>(std::forward<U>(movedIndex), (IndexTypeTuple &)(*this));
                else
                    RandomAccess::root.template moveTop<Pathway>(std::forward<U>(movedIndex), (IndexTypeTuple &)(*this));
            }
            template <class U> void moveDown(U && movedIndex) {
                if constexpr ( RareTs::is_iterable_v<std::remove_cvref_t<U>> )
                    RandomAccess::root.template moveDownN<Pathway>(std::forward<U>(movedIndex), (IndexTypeTuple &)(*this));
                else
                    RandomAccess::root.template moveDown<Pathway>(std::forward<U>(movedIndex), (IndexTypeTuple &)(*this));
            }
            template <class U> void moveBottom(U && movedIndex) {
                if constexpr ( RareTs::is_iterable_v<std::remove_cvref_t<U>> )
                    RandomAccess::root.template moveBottomN<Pathway>(std::forward<U>(movedIndex), (IndexTypeTuple &)(*this));
                else
                    RandomAccess::root.template moveBottom<Pathway>(std::forward<U>(movedIndex), (IndexTypeTuple &)(*this));
            }
            template <class I, class U> void moveTo(U && movedIndex, I indexMovedTo) {
                if constexpr ( RareTs::is_iterable_v<std::remove_cvref_t<U>> )
                    RandomAccess::root.template moveToN<Pathway>(std::forward<U>(movedIndex), indexMovedTo, (IndexTypeTuple &)(*this));
                else
                    RandomAccess::root.template moveTo<Pathway>(std::forward<U>(movedIndex), indexMovedTo, (IndexTypeTuple &)(*this));
            }

            void moveSelectionsUp() { RandomAccess::root.template moveUpL<Pathway>((IndexTypeTuple &)(*this)); }
            void moveSelectionsTop() { RandomAccess::root.template moveTopL<Pathway>((IndexTypeTuple &)(*this)); }
            void moveSelectionsDown() { RandomAccess::root.template moveDownL<Pathway>((IndexTypeTuple &)(*this)); }
            void moveSelectionsBottom() { RandomAccess::root.template moveBottomL<Pathway>((IndexTypeTuple &)(*this)); }
            template <class I> void moveSelectionsTo(I indexMovedTo) { RandomAccess::root.template moveToL<Pathway>(indexMovedTo, (IndexTypeTuple &)(*this)); }
        };
    };

    template <class Edit, class default_index_type, class RootData, class T, class IndexTypeTuple, class Pathway>
    static constexpr auto editVector()
    {
        using element_type = RareTs::element_type_t<T>;
        if constexpr ( RareTs::is_reflected_v<element_type> )
            return std::type_identity<typename Editable<Edit, default_index_type, RootData, element_type, IndexTypeTuple, Pathway, edit_members>::Vector>{};
        else
            return std::type_identity<typename Editable<Edit, default_index_type, RootData, element_type, IndexTypeTuple, Pathway, EditPrimitive>::Vector>{};
    }

    template <class Edit, class default_index_type, class RootData, class T, class IndexTypeTuple, class Pathway>
    static constexpr auto editArray()
    {
        using element_type = RareTs::element_type_t<T>;
        if constexpr ( RareTs::is_reflected_v<element_type> )
            return std::type_identity<typename Editable<Edit, default_index_type, RootData, element_type, IndexTypeTuple, Pathway, edit_members>::Array>{};
        else
            return std::type_identity<typename Editable<Edit, default_index_type, RootData, element_type, IndexTypeTuple, Pathway, EditPrimitive>::Array>{};
    }

    template <class Edit, class default_index_type, class RootData, class T, class IndexTypeTuple, class Pathway, std::size_t I, std::size_t ... Is>
    static constexpr auto editMember(std::index_sequence<Is...>)
    {
        using Path = std::tuple<std::tuple_element_t<Is, Pathway>..., PathMember<I>>;
        using member_type = typename RareTs::Member<T, I>::type;
        if constexpr ( RareTs::is_static_array_v<member_type> )
            return decltype(editArray<Edit, default_index_type, RootData, member_type, IndexTypeTuple, Path>()){};
        else if constexpr ( RareTs::is_specialization_v<member_type, std::vector> ) // Vector
            return decltype(editVector<Edit, default_index_type, RootData, member_type, IndexTypeTuple, Path>()){};
        else if constexpr ( RareTs::is_reflected_v<member_type> ) // Reflected object
            return std::type_identity<edit_members<Edit, default_index_type, RootData, member_type, IndexTypeTuple, Path>>{};
        else // Primitive
            return std::type_identity<EditPrimitive<Edit, default_index_type, RootData, member_type, IndexTypeTuple, Path>>{};
    }

    template <typename T> struct is_selection_leaf_member {
        static constexpr bool value = T::isData && !T::isStatic &&
            (RareTs::is_static_array_v<typename T::type> ||
             RareTs::is_specialization_v<typename T::type, std::vector>);
    };

    template <typename T> inline constexpr bool is_selection_leaf_member_v = is_selection_leaf_member<T>::value;

    template <class T> static constexpr bool hasSelectionMember()
    {
        using member_type = typename T::type;
        if constexpr ( RareTs::is_in_class_reflected_v<member_type> )
        {
            constexpr auto total = RareTs::Members<member_type>::total;
            return [&]<std::size_t ... Is>(std::index_sequence<Is...>){
                return (hasSelectionMember<RareTs::Member<member_type, Is>>() || ...);
            }(std::make_index_sequence<total>());
        }
        else
        {
            return is_selection_leaf_member_v<T>;
        }
    };

    template <typename T> struct is_selection_member {
        static constexpr bool value = hasSelectionMember<T>();
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

    template <class DefaultIndexType, class T, std::size_t ... Is>
    struct object_selection : RareTs::Class::adapt_member<member_selection<DefaultIndexType, T>::template type, T, Is>...
    {
        template <std::size_t I> auto & fromMember()
        {
            if constexpr ( std::is_base_of_v<RareTs::Class::adapt_member<member_selection<DefaultIndexType, T>::template type, T, I>, object_selection<DefaultIndexType, T, Is...>> )
            {
                auto & [selection] = static_cast<RareTs::Class::adapt_member<member_selection<DefaultIndexType, T>::template type, T, I> &>(*this);
                return selection;
            }
            else
            {
                static std::nullptr_t null {};
                return null;
            }
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

    template <class T, class User, class Editor>
    struct EditRoot
    {
        using type = T;
        using default_index_type = typename decltype(defaultIndexType<T>())::type;
        decltype(RareEdit::selections<default_index_type, T>()) selections {};
        std::vector<std::uint8_t> events {std::uint8_t(0)}; // First byte is unused
        std::vector<std::uint64_t> eventOffsets {};
        Editor & editor;
        T & t;
        User & user;

        template <class U, class P, class Value>
        using ValueChangedOp = decltype(std::declval<U>().valueChanged(std::declval<P>(), std::declval<Value>(), std::declval<Value>()));

        template <class U, class P, class Value>
        using ElementAddedOp = decltype(std::declval<U>().elementAdded(std::declval<P>(), std::declval<Value>()));

        template <class U, class P, class Value>
        using ElementRemovedOp = decltype(std::declval<U>().elementRemoved(std::declval<P>(), std::declval<Value>()));

        template <class U, class P, class Value>
        using ElementMovedOp = decltype(std::declval<U>().elementMoved(std::declval<P>(), std::declval<Value>(), std::declval<Value>()));

        template <class U, class P>
        using SelectionsChangedOp = decltype(std::declval<U>().selectionsChanged(std::declval<P>()));

        template <class P, class Value>
        static constexpr bool hasValueChangedOp = RareTs::op_exists_v<ValueChangedOp, User, P, Value>;

        template <class P, class Value>
        static constexpr bool hasElementAddedOp = RareTs::op_exists_v<ElementAddedOp, User, P, Value>;

        template <class P, class Value>
        static constexpr bool hasElementRemovedOp = RareTs::op_exists_v<ElementRemovedOp, User, P, Value>;

        template <class P, class Value>
        static constexpr bool hasElementMovedOp = RareTs::op_exists_v<ElementMovedOp, User, P, Value>;

        template <class P>
        static constexpr bool hasSelectionsChangedOp = RareTs::op_exists_v<SelectionsChangedOp, User, P>;

        EditRoot(Editor & editor, T & t, User & user) : editor(editor), t(t), user(user) {}

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
        
        static constexpr std::nullptr_t noSelection {};
        
        template <class PathElement, class ... Pathway>
        auto & getSelectionsData(auto & selections)
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

        template <class Pathway>
        constexpr auto & getSelections() {
            if constexpr ( std::is_void_v<Pathway> )
            {
                return noSelection;
            }
            else
            {
                return [&]<std::size_t ... Is>(std::index_sequence<Is...>) -> auto & {
                    return getSelectionsData<std::tuple_element_t<Is, Pathway>...>(selections);
                }(std::make_index_sequence<std::tuple_size_v<Pathway>>());
            }
        }

        template <class Pathway>
        constexpr bool hasSelections() {
            return !std::is_null_pointer_v<std::remove_cvref_t<decltype(getSelections<Pathway>())>>;
        }

        template <class IndexTypeTuple, class U, class PathElement, class ... Pathway>
        auto & getMemberReferenceImpl(U & t, IndexTypeTuple & indexes)
        {
            if constexpr ( is_path_member_v<PathElement> )
            {
                using Member = RareTs::Member<U, PathElement::index>;
                if constexpr ( sizeof...(Pathway) == 0 )
                    return Member::value(t);
                else
                    return getMemberReferenceImpl<IndexTypeTuple, typename Member::type, Pathway...>(Member::value(t), indexes);
            }
            else if constexpr ( is_path_index_v<PathElement> )
            {
                if constexpr ( sizeof...(Pathway) == 0 )
                    return t[std::get<PathElement::index>(indexes)];
                else
                {
                    return getMemberReferenceImpl<IndexTypeTuple, std::remove_cvref_t<decltype(t[std::get<PathElement::index>(indexes)])>, Pathway...>(
                        t[std::get<PathElement::index>(indexes)], indexes);
                }
            }
            else
                static_assert(std::is_void_v<PathElement>, "Unexpected path element type!");
        }

        template <class Pathway, class IndexTypeTuple, class U>
        auto & getMemberReference(U & t, IndexTypeTuple & indexes)
        {
            auto & ref = [&]<std::size_t ... Is>(std::index_sequence<Is...>) -> auto & {
                return getMemberReferenceImpl<IndexTypeTuple, U, std::tuple_element_t<Is, Pathway>...>(t, indexes);
            }(std::make_index_sequence<std::tuple_size_v<Pathway>>());
            return ref;
        }

        template <class Pathway, class IndexTypeTuple>
        void serializePathway(IndexTypeTuple & indexes)
        {
            constexpr std::size_t path_count = std::tuple_size_v<Pathway>;
            constexpr std::size_t last_path = path_count-1;
            RareTs::forIndexes<path_count>([&](auto I){
                using PathElement = std::tuple_element_t<decltype(I)::value, Pathway>;
                if constexpr ( is_path_selections_v<PathElement> )
                {
                    if constexpr ( last_path == decltype(I)::value )
                        events.push_back(uint8_t(PathOp::LeafSelBranch));
                    else
                        events.push_back(uint8_t(PathOp::SelBranch));
                }
                else if constexpr ( is_path_member_v<PathElement> )
                {
                    static_assert(PathElement::index < 64, "Expected a lower member index...");
                    if constexpr ( last_path == decltype(I)::value )
                        events.push_back(uint8_t(PathOp::LeafBranch) | uint8_t(PathElement::index));
                    else
                        events.push_back(uint8_t(PathOp::Branch) | uint8_t(PathElement::index));
                }
                else if constexpr ( is_path_index_v<PathElement> )
                {
                    if constexpr ( last_path == decltype(I)::value )
                    {
                        auto index = std::get<PathElement::index>(indexes);
                        if constexpr ( std::is_same_v<uint6_t, decltype(index)> )
                            events.push_back(uint8_t(PathOp::LeafBranch) | uint8_t(index));
                        else
                        {
                            events.push_back(uint8_t(PathOp::LeafBranch));
                            events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&index), reinterpret_cast<const std::uint8_t*>(&index)+sizeof(index));
                        }
                    }
                    else
                    {
                        auto index = std::get<PathElement::index>(indexes);
                        if constexpr ( std::is_same_v<uint6_t, decltype(index)> )
                            events.push_back(uint8_t(PathOp::Branch) | uint8_t(index));
                        else
                        {
                            events.push_back(uint8_t(PathOp::Branch));
                            events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&index), reinterpret_cast<const std::uint8_t*>(&index)+sizeof(index));
                        }
                    }
                }
                else
                    static_assert(std::is_void_v<PathElement>, "Unrecognized path element!");
            });
        }

        template <class Member> void serializeValue(auto && value)
        {
            using value_type = std::remove_cvref_t<decltype(value)>;
            if constexpr ( RareTs::is_reflected_v<value_type> )
            {
                RareTs::Members<value_type>::forEach([&](auto member) {
                    serializeValue<decltype(member)>(member.value(value));
                });
            }
            else if constexpr ( std::is_same_v<value_type, std::string> )
            {
                auto size = static_cast<index_type_t<default_index_type, Member>>(value.size());
                events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));
                events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(value.c_str()), reinterpret_cast<const std::uint8_t*>(value.c_str()+size));
            }
            else if constexpr ( RareTs::is_static_array_v<value_type> )
            {
                constexpr auto size = static_cast<index_type_t<default_index_type, Member>>(RareTs::static_array_size_v<value_type>);
                events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));
                for ( auto & val : value )
                    serializeValue<Member>(val);
            }
            else if constexpr ( RareTs::is_iterable_v<value_type> )
            {
                auto size = static_cast<index_type_t<default_index_type, Member>>(value.size());
                events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));
                for ( auto & val : value )
                    serializeValue<Member>(val);
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

        template <class IndexTypeTuple, class U, class F, class LastMember, class SelPath, class PathElement, class ... Pathway>
        void operateOnImpl(U & t, IndexTypeTuple & indexes, F f)
        {
            if constexpr ( is_path_selections_v<PathElement> )
            {
                if constexpr ( std::is_void_v<SelPath> )
                    return;
                else
                {
                    auto & selData = getSelections<SelPath>();
                    for ( auto & sel : selData )
                    {
                        if constexpr ( sizeof...(Pathway) == 0 )
                            f(t[static_cast<std::size_t>(sel)], LastMember{});
                        else
                            operateOnImpl<IndexTypeTuple, std::remove_cvref_t<decltype(t[0])>, F, LastMember, void, Pathway...>(t[sel], indexes, f);
                    }
                }
            }
            else if constexpr ( is_path_member_v<PathElement> )
            {
                using Member = RareTs::Member<U, PathElement::index>;
                if constexpr ( sizeof...(Pathway) == 0 )
                    f(Member::value(t), Member{});
                else
                    operateOnImpl<IndexTypeTuple, typename Member::type, F, Member, path_append_t<SelPath, PathElement>, Pathway...>(Member::value(t), indexes, f);
            }
            else if constexpr ( is_path_index_v<PathElement> )
            {
                if constexpr ( sizeof...(Pathway) == 0 )
                    f(t[static_cast<std::size_t>(std::get<PathElement::index>(indexes))], LastMember{});
                else
                {
                    operateOnImpl<IndexTypeTuple, std::remove_cvref_t<decltype(t[static_cast<std::size_t>(std::get<PathElement::index>(indexes))])>,
                        F, LastMember, path_append_t<SelPath, void>, Pathway...>(
                        t[static_cast<std::size_t>(std::get<PathElement::index>(indexes))], indexes, f);
                }
            }
            else
                static_assert(std::is_void_v<PathElement>, "Unexpected path element type!");
        }
        
        template <class Pathway, class IndexTypeTuple, class U, class F>
        void operateOn(U & t, IndexTypeTuple & indexes, F f)
        {
            [&]<std::size_t ... Is>(std::index_sequence<Is...>) {
                operateOnImpl<IndexTypeTuple, U, F, void, std::tuple<>, std::tuple_element_t<Is, Pathway>...>(t, indexes, f);
            }(std::make_index_sequence<std::tuple_size_v<Pathway>>());
        }

        template <class U = T, class Member = void, class F = void, class IndexTypeTuple = std::tuple<>, class ... Pathway>
        void processEventRef(U & t, std::size_t & offset, F processEventOp, IndexTypeTuple indexes = std::tuple{})
        {
            using index_type = index_type_t<default_index_type, Member>;
            std::uint8_t value = events[offset];
            ++offset;

            switch ( PathOp(value & std::uint8_t(PathOp::HighBits)) )
            {
                case PathOp::SelBranch:
                    if constexpr ( RareTs::is_static_array_v<U> || RareTs::is_specialization_v<U, std::vector> )
                    {
                        //std::cout << "[{sel}]";
                        std::size_t selBranchStartOffset = offset;
                        auto & selData = getSelections<std::tuple<Pathway...>>();
                        for ( auto & sel : selData )
                        {
                            processEventRef(t[static_cast<std::size_t>(sel)], offset, processEventOp, indexes);
                            offset = selBranchStartOffset;
                        }
                    }
                    break;
                case PathOp::LeafSelBranch:
                    if constexpr ( RareTs::is_static_array_v<U> || RareTs::is_specialization_v<U, std::vector> )
                    {
                        //std::cout << "[{sel}]";
                        using element_type = RareTs::element_type_t<std::remove_cvref_t<U>>;
                        
                        std::size_t selBranchStartOffset = offset;
                        auto & selData = getSelections<std::tuple<Pathway...>>();
                        for ( auto & sel : selData )
                        {
                            processEventOp(t[static_cast<std::size_t>(sel)], indexes, RareTs::type_tags<element_type, Member, Pathway..., PathSelections>{});
                            offset = selBranchStartOffset;
                        }
                    }
                    break;
                case PathOp::Branch:
                {
                    //std::cout << "(PathOp::Branch|" << (value & std::uint8_t(PathOp::LowBits)) << ")";
                    if constexpr ( RareTs::is_reflected_v<U> ) // Branch to field
                    {
                        RareTs::Members<U>::at(std::size_t(value & std::uint8_t(PathOp::LowBits)), t, [&](auto member, auto & ref) {
                            //std::cout << "." << member.name;
                            processEventRef<std::remove_cvref_t<typename decltype(member)::type>,
                                decltype(member), F, IndexTypeTuple, Pathway..., PathMember<decltype(member)::index>>(ref, offset, processEventOp, indexes);
                        });
                    }
                    else if constexpr ( RareTs::is_static_array_v<U> || RareTs::is_specialization_v<U, std::vector> ) // Branch to index
                    {
                        index_type index {};
                        if constexpr ( std::is_same_v<index_type, uint6_t> )
                            index = static_cast<uint6_t>(value & std::uint8_t(PathOp::LowBits));
                        else
                            index = readIndex<index_type>(offset);

                        //std::cout << '[' << std::size_t{index} << ']';
                        processEventRef<std::remove_cvref_t<decltype(std::declval<U>()[0])>, Member, F, decltype(std::tuple_cat(indexes, std::tuple{index})),
                            Pathway..., PathIndex<std::tuple_size_v<IndexTypeTuple>>>(
                                t[static_cast<std::size_t>(index)], offset, processEventOp, std::tuple_cat(indexes, std::tuple{index}));
                    }
                }
                break;
                case PathOp::LeafBranch:
                {
                    if constexpr ( RareTs::is_reflected_v<U> ) // Op on field
                    {
                        RareTs::Members<U>::at(std::size_t(value & std::uint8_t(PathOp::LowBits)), t, [&](auto member, auto & ref) {
                            //std::cout << "." << member.name;
                            //value = events[offset]; // op
                            //++offset;
                            processEventOp(ref, indexes, RareTs::type_tags<std::remove_cvref_t<typename decltype(member)::type>,
                                RareTs::remove_cvref_t<decltype(member)>, Pathway..., PathMember<decltype(member)::index>>{});
                        });
                    }
                    else if constexpr ( RareTs::is_static_array_v<U> || RareTs::is_specialization_v<U, std::vector>  ) // Op on index
                    {
                        index_type index {};
                        if constexpr ( std::is_same_v<index_type, uint6_t> )
                            index = uint6_t(value & std::uint8_t(PathOp::LowBits));
                        else
                            index = readIndex<index_type>(offset);

                        //std::cout << "[" << std::size_t{index} << "]";
                        //value = events[offset]; // op
                        //++offset;
                        processEventOp(t[static_cast<std::size_t>(index)], std::tuple_cat(indexes, std::tuple{index}), RareTs::type_tags<RareTs::element_type_t<std::remove_cvref_t<U>>,
                            Member, Pathway..., PathIndex<std::tuple_size_v<IndexTypeTuple>>>{});
                    }
                }
                break;
                default:
                    throw std::runtime_error("how u here?");
            }
        }

        template <class Pathway, class IndexTypeTuple>
        void reset(IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Reset));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                if constexpr ( hasValueChangedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>, std::remove_cvref_t<decltype(ref)>> )
                {
                    auto prevValue = ref;
                    serializeValue<decltype(member)>(ref);
                    ref = {};
                    notifyValueChanged(user, PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{indexes}, prevValue, ref);
                }
                else
                {
                    serializeValue<decltype(member)>(ref);
                    ref = {};
                }
                auto & sel = getSelections<Pathway>();
                writeSelections(events, sel);
                clearSel(sel);
            });
        }

        template <class Pathway, class IndexTypeTuple>
        void reserve(std::size_t size, IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Reserve));
            serializePathway<Pathway>(indexes);
            
            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                serializeIndex<decltype(member)>(size);
                ref.reserve(size);
            });
        }

        template <class Pathway, class IndexTypeTuple>
        void trim(IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Trim));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto) {
                ref.shrink_to_fit();
            });
        }

        template <class Pathway, class Value, class IndexTypeTuple>
        void assign(std::size_t size, Value && value, IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Assign));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                serializeIndex<decltype(member)>(size);
                serializeValue<decltype(member)>(value);
                if constexpr ( hasValueChangedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>, Value> )
                {
                    auto prevValue = ref;
                    serializeValue<decltype(member)>(ref);
                    ref.assign(size, value);
                    notifyValueChanged(user, PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{indexes}, prevValue, ref);
                }
                else
                {
                    serializeValue<decltype(member)>(ref);
                    ref.assign(size, value);
                }
                auto & sel = getSelections<Pathway>();
                writeSelections(events, sel);
                clearSel(sel);
            });
        }

        template <class Pathway, class IndexTypeTuple>
        void assignDefault(std::size_t size, IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::AssignDefault));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                serializeIndex<decltype(member)>(size);
                if constexpr ( hasValueChangedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>, std::remove_cvref_t<decltype(ref)>> )
                {
                    auto prevValue = ref;
                    serializeValue<decltype(member)>(ref);
                    ref = std::remove_cvref_t<decltype(ref)>(size);
                    notifyValueChanged(user, PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{indexes}, prevValue, ref);
                }
                else
                {
                    serializeValue<decltype(member)>(ref);
                    ref = std::remove_cvref_t<decltype(ref)>(size);
                }
                auto & sel = getSelections<Pathway>();
                writeSelections(events, sel);
                clearSel(sel);
            });
        }

        template <class Pathway, class Value, class IndexTypeTuple>
        void set(Value && value, IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());

            constexpr bool isSelectionSet = is_selection_set_v<Pathway>;
            if constexpr ( isSelectionSet )
                events.push_back(uint8_t(Op::SetL));
            else
                events.push_back(uint8_t(Op::Set));

            serializePathway<Pathway>(indexes);

            if constexpr ( isSelectionSet ) // Pure selection set
            {
                bool first = true;
                operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                    if ( first )
                    {
                        first = false;
                        if constexpr ( hasValueChangedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>, Value> )
                        {
                            auto prevValue = ref;
                            serializeValue<decltype(member)>(static_cast<std::remove_cvref_t<decltype(ref)>>(value)); // Value set to
                            serializeValue<decltype(member)>(ref); // Value before changing
                            ref = std::forward<Value>(value);
                            notifyValueChanged(user, PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{indexes}, prevValue, ref);
                        }
                        else
                        {
                            serializeValue<decltype(member)>(static_cast<std::remove_cvref_t<decltype(ref)>>(value)); // Value set to
                            serializeValue<decltype(member)>(ref); // Value before changing
                            ref = std::forward<Value>(value);
                        }
                    }
                    else if constexpr ( hasValueChangedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>, Value> )
                    {
                        auto prevValue = ref;
                        serializeValue<decltype(member)>(ref); // Value before changing
                        ref = std::forward<Value>(value);
                        notifyValueChanged(user, PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{indexes}, prevValue, ref);
                    }
                    else
                    {
                        serializeValue<decltype(member)>(ref); // Value before changing
                        ref = std::forward<Value>(value);
                    }
                });
            }
            else // Non-selection set
            {
                operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                    if constexpr ( hasValueChangedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>, Value> )
                    {
                        auto prevValue = ref;
                        serializeValue<decltype(member)>(static_cast<std::remove_cvref_t<decltype(ref)>>(value)); // Value set to
                        serializeValue<decltype(member)>(ref); // Value before changing
                        ref = std::forward<Value>(value);
                        notifyValueChanged(user, PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{indexes}, prevValue, ref);
                    }
                    else
                    {
                        serializeValue<decltype(member)>(static_cast<std::remove_cvref_t<decltype(ref)>>(value)); // Value set to
                        serializeValue<decltype(member)>(ref); // Value before changing
                        ref = std::forward<Value>(value);
                    }

                    auto & sel = getSelections<Pathway>();
                    if constexpr ( RareTs::is_specialization_v<std::remove_cvref_t<decltype(sel)>, std::vector> )
                    {
                        writeSelections(events, sel);
                        clearSel(sel);
                    }
                });
            }
        }

        template <class Pathway, class SetIndexes, class Value, class IndexTypeTuple>
        void set(SetIndexes && setIndexes, Value && value, IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::SetN));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                using element_type = RareTs::element_type_t<std::remove_cvref_t<decltype(ref)>>;
                if constexpr ( !std::is_void_v<element_type> )
                {
                    if constexpr ( hasValueChangedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>, Value> )
                    {
                        auto prevValue = ref;
                        serializeIndex<decltype(member)>(std::size(setIndexes)); // Index count
                        for ( auto setIndex : setIndexes )
                            serializeIndex<decltype(member)>(setIndex); // Indexes

                        serializeValue<decltype(member)>(std::forward<Value>(value)); // Value set to

                        for ( auto setIndex : setIndexes )
                            serializeValue<decltype(member)>(ref[setIndex]); // Value before changing

                        for ( auto setIndex : setIndexes )
                            ref[setIndex] = std::forward<Value>(value); // Make the change

                        notifyValueChanged(user, PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{indexes}, prevValue, ref);
                    }
                    else
                    {
                        serializeIndex<decltype(member)>(std::size(setIndexes)); // Index count
                        for ( auto setIndex : setIndexes )
                            serializeIndex<decltype(member)>(setIndex); // Indexes

                        serializeValue<decltype(member)>(std::forward<Value>(value)); // Value set to

                        for ( auto setIndex : setIndexes )
                            serializeValue<decltype(member)>(ref[setIndex]); // Value before changing

                        for ( auto setIndex : setIndexes )
                            ref[setIndex] = std::forward<Value>(value); // Make the change
                    }
                }
            });
        }

        template <class Pathway, class Value, class IndexTypeTuple>
        void append(Value && value, IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Append));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                ref.push_back(std::forward<Value>(value));
                serializeValue<decltype(member)>(ref.back());
                if constexpr ( hasElementAddedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>, std::size_t> )
                    notifyElementAdded(user, PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{indexes}, ref.size()-1);
            });
        }

        template <class Pathway, class Values, class IndexTypeTuple>
        void appendN(Values && values, IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::AppendN));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                serializeIndex<decltype(member)>(std::size(values));
                for ( auto & value : values )
                {
                    ref.push_back(value);
                    serializeValue<decltype(member)>(ref.back());
                    if constexpr ( hasElementAddedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>, std::size_t> )
                        notifyElementAdded(user, PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{indexes}, ref.size()-1);
                }
            });
        }

        template <class Pathway, class InsertionIndex, class Value, class IndexTypeTuple>
        void insert(InsertionIndex insertionIndex, Value && value, IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Insert));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                serializeIndex<decltype(member)>(insertionIndex);
                serializeValue<decltype(member)>(value);
                ref.insert(std::next(ref.begin(), static_cast<std::ptrdiff_t>(insertionIndex)), std::forward<Value>(value));
                if constexpr ( hasElementAddedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>, std::size_t> )
                    notifyElementAdded(user, PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{indexes}, static_cast<std::size_t>(insertionIndex));

                auto & sel = getSelections<Pathway>();
                for ( auto & index : sel )
                {
                    if ( static_cast<InsertionIndex>(index) >= insertionIndex )
                        ++index;
                }
            });
        }

        template <class Pathway, class InsertionIndex, class Values, class IndexTypeTuple>
        void insertN(InsertionIndex insertionIndex, Values && values, IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::InsertN));
            serializePathway<Pathway>(indexes);
            
            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                serializeIndex<decltype(member)>(std::size(values));
                serializeIndex<decltype(member)>(insertionIndex);
                for ( auto & value : values )
                    serializeValue<decltype(member)>(value);
                
                ref.insert(std::next(ref.begin(), static_cast<std::ptrdiff_t>(insertionIndex)) , std::begin(values), std::end(values));
                if constexpr ( hasElementAddedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>, std::size_t> )
                {
                    auto limit = insertionIndex + static_cast<InsertionIndex>(std::size(values));
                    for ( auto i = insertionIndex; i < limit; ++i )
                        notifyElementAdded(user, PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{indexes}, static_cast<std::size_t>(i));
                }

                auto & sel = getSelections<Pathway>();
                for ( auto & index : sel )
                {
                    std::size_t count = values.size();
                    if ( static_cast<InsertionIndex>(index) >= insertionIndex )
                        index += count;
                }
            });
        }

        template <class Pathway, class RemovalIndex, class IndexTypeTuple>
        void remove(RemovalIndex removalIndex, IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Remove));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                
                using index_type = index_type_t<default_index_type, decltype(member)>;
                serializeIndex<decltype(member)>(removalIndex);
                serializeValue<decltype(member)>(ref[static_cast<std::size_t>(removalIndex)]);
                auto & sel = getSelections<Pathway>();
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
                    serializeIndex<decltype(member)>(foundAt);
                    sel.erase(found);
                }

                if constexpr ( hasElementRemovedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>, std::size_t> )
                {
                    auto value = ref[static_cast<std::size_t>(removalIndex)];
                    ref.erase(std::next(ref.begin(), static_cast<std::size_t>(removalIndex)));
                    notifyElementRemoved(user, PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{indexes}, static_cast<std::size_t>(removalIndex));
                }
                else
                    ref.erase(std::next(ref.begin(), static_cast<std::ptrdiff_t>(removalIndex)));
            });
        }

        template <class Pathway, class RemovalIndexes, class IndexTypeTuple>
        void removeN(RemovalIndexes && removalIndexes, IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::RemoveN));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {

                using index_type = index_type_t<default_index_type, decltype(member)>;
                serializeIndex<decltype(member)>(std::size(removalIndexes));
                // Sort indexes so largest indexes come first/are removed first
                std::sort(removalIndexes.begin(), removalIndexes.end(), std::greater<RareTs::element_type_t<std::remove_cvref_t<RemovalIndexes>>>{});
                for ( auto removalIndex : removalIndexes )
                    serializeIndex<decltype(member)>(removalIndex);
                for ( auto removalIndex : removalIndexes )
                    serializeValue<decltype(member)>(ref[static_cast<std::size_t>(removalIndex)]);
                for ( auto removalIndex : removalIndexes )
                    ref.erase(std::next(ref.begin(), static_cast<std::ptrdiff_t>(removalIndex)));

                auto & sel = getSelections<Pathway>();
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
                writeVecBoolData(events, removalIndexSelected);
                serializeIndex<decltype(member)>(static_cast<index_type>(prevSelIndexes.size()));
                if ( !prevSelIndexes.empty() )
                {
                    events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&prevSelIndexes[0]),
                        reinterpret_cast<const std::uint8_t*>(&prevSelIndexes[0])+std::size(prevSelIndexes)*sizeof(prevSelIndexes[0]));
                }

                if constexpr ( hasElementRemovedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>, std::size_t> )
                {
                    for ( auto removalIndex : removalIndexes )
                        notifyElementRemoved(user, PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{indexes}, static_cast<std::size_t>(removalIndex));
                }
            });
        }

        template <class Pathway, class IndexTypeTuple>
        void removeL(IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::RemoveL));
            serializePathway<Pathway>(indexes);
            auto & sel = getSelections<Pathway>();

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                auto selections = sel;
                std::sort(selections.begin(), selections.end(), std::greater<RareTs::element_type_t<std::remove_cvref_t<decltype(selections)>>>{});
                serializeIndex<decltype(member)>(std::size(selections));
                for ( auto i : selections )
                    serializeIndex<decltype(member)>(i);
                for ( auto i : selections )
                {
                    if constexpr ( hasElementRemovedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>, std::size_t> )
                    {
                        auto value = ref[i];
                        serializeValue<decltype(member)>(value);
                        ref.erase(std::next(ref.begin(), i));
                        notifyElementRemoved(user, PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{indexes}, static_cast<std::size_t>(i));
                    }
                    else
                    {
                        serializeValue<decltype(member)>(ref[i]);
                        ref.erase(std::next(ref.begin(), static_cast<std::ptrdiff_t>(i)));
                    }
                }
                writeSelections(events, sel);
                clearSel(sel);
            });
        }

        template <class Pathway, class IndexTypeTuple>
        void sort(IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::Sort));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                using index_type = index_type_t<default_index_type, decltype(member)>;
                auto sourceIndexes = trackedSort<false, index_type>(ref);
                index_type serializedSize = static_cast<index_type>(sourceIndexes.size());
                serializeIndex<decltype(member)>(serializedSize);
                for ( auto index : sourceIndexes )
                {
                    index_type sourceIndex = static_cast<index_type>(index);
                    serializeIndex<decltype(member)>(sourceIndex);
                }
                auto count = std::size(sourceIndexes);
                auto & sel = getSelections<Pathway>();
                auto newSel = std::remove_cvref_t<decltype(sel)>{};
                for ( std::size_t i=0; i<count; ++i )
                {
                    if ( std::find(sel.begin(), sel.end(), sourceIndexes[i]) != sel.end() )
                        RareTs::append(newSel, static_cast<index_type>(i));
                }
                std::swap(sel, newSel);
                if constexpr ( hasElementRemovedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>, std::size_t> )
                {
                    for ( std::size_t i=0; i<count; ++i )
                    {
                        if ( i != sourceIndexes[i] )
                            notifyElementMoved(user, PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{indexes}, static_cast<std::size_t>(sourceIndexes[i]), static_cast<std::size_t>(i));
                    }
                }
            });
        }

        template <class Pathway, class IndexTypeTuple>
        void sortDesc(IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::SortDesc));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                using index_type = index_type_t<default_index_type, decltype(member)>;
                auto sourceIndexes = trackedSort<true, index_type>(ref);
                index_type serializedSize = static_cast<index_type>(sourceIndexes.size());
                serializeIndex<decltype(member)>(serializedSize);
                for ( auto index : sourceIndexes )
                {
                    index_type sourceIndex = static_cast<index_type>(index);
                    serializeIndex<decltype(member)>(sourceIndex);
                }
                auto count = std::size(sourceIndexes);
                auto & sel = getSelections<Pathway>();
                auto newSel = std::remove_cvref_t<decltype(sel)>{};
                for ( std::size_t i=0; i<count; ++i )
                {
                    if ( std::find(sel.begin(), sel.end(), sourceIndexes[i]) != sel.end() )
                        RareTs::append(newSel, i);
                }
                std::swap(sel, newSel);
                if constexpr ( hasElementRemovedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>, std::size_t> )
                {
                    auto limit = std::size(sourceIndexes);
                    for ( std::size_t i=0; i<limit; ++i )
                    {
                        if ( i != sourceIndexes[i] )
                            notifyElementMoved(user, PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{indexes}, static_cast<std::size_t>(sourceIndexes[i]), static_cast<std::size_t>(i));
                    }
                }
            });
        }

        template <class Pathway, class MovedIndex, class IndexTypeTuple>
        void moveUp(MovedIndex movedIndex, IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveUp));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                serializeIndex<decltype(member)>(movedIndex);
                if ( movedIndex > 0 && static_cast<std::size_t>(movedIndex) < std::size(ref) )
                {
                    std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex-1)]);
                    mirrorSwapToSelection(getSelections<Pathway>(), static_cast<std::size_t>(movedIndex), static_cast<std::size_t>(movedIndex)-1);
                }
            });
        }

        template <class Pathway, class MovedIndexes, class IndexTypeTuple>
        void moveUpN(MovedIndexes && movedIndexes, IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveUpN));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                std::sort(movedIndexes.begin(), movedIndexes.end());

                serializeIndex<decltype(member)>(std::size(movedIndexes));
                for ( auto index : movedIndexes )
                    serializeIndex<decltype(member)>(index);

                std::size_t nextAvailable = 0;
                for ( auto movedIndex : movedIndexes )
                {
                    if ( movedIndex > nextAvailable && static_cast<std::size_t>(movedIndex) < std::size(ref) )
                    {
                        std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex-1)]);
                        mirrorSwapToSelection(getSelections<Pathway>(), movedIndex, movedIndex-1);
                        nextAvailable = movedIndex;
                    }
                    else if ( movedIndex == nextAvailable )
                        ++nextAvailable;
                }
            });
        }

        template <class Pathway, class IndexTypeTuple>
        void moveUpL(IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveUpL));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto) {
                auto & sel = getSelections<Pathway>();
                writeSelections(events, sel);
                auto movedIndexes = sel; // Copy so it can be sorted without changing selections
                std::sort(movedIndexes.begin(), movedIndexes.end());
                
                std::size_t nextAvailable = 0;
                for ( auto movedIndex : movedIndexes )
                {
                    if ( movedIndex > nextAvailable && static_cast<std::size_t>(movedIndex) < std::size(ref) )
                    {
                        std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex-1)]);
                        mirrorSwapToSelection(getSelections<Pathway>(), movedIndex, movedIndex-1);
                        nextAvailable = movedIndex;
                    }
                    else if ( movedIndex == nextAvailable )
                        ++nextAvailable;
                }
            });
        }

        template <class Pathway, class MovedIndex, class IndexTypeTuple>
        void moveTop(MovedIndex movedIndex, IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveTop));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                serializeIndex<decltype(member)>(movedIndex);
                auto size = std::size(ref);
                if ( movedIndex > 0 && static_cast<std::size_t>(movedIndex) < size )
                {
                    auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(movedIndex));
                    std::rotate(ref.begin(), it, it+1);
                    mirrorRotationToSelection(getSelections<Pathway>(), 0, static_cast<std::size_t>(movedIndex), static_cast<std::size_t>(movedIndex)+1);
                }
            });
        }

        template <class Pathway, class MovedIndexes, class IndexTypeTuple>
        void moveTopN(MovedIndexes && movedIndexes, IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveTopN));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                std::sort(movedIndexes.begin(), movedIndexes.end()); // lowest to highest
                serializeIndex<decltype(member)>(std::size(movedIndexes));
                for ( auto i : movedIndexes )
                    serializeIndex<decltype(member)>(i);

                writeSelections(events, getSelections<Pathway>());

                std::size_t size = std::size(ref);
                std::ptrdiff_t insertionIndex = 0;
                for ( auto i : movedIndexes )
                {
                    if ( static_cast<std::ptrdiff_t>(i) > insertionIndex && static_cast<std::size_t>(i) < size )
                    {
                        auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                        std::rotate(std::next(ref.begin(), insertionIndex), it, it+1);
                        mirrorRotationToSelection(getSelections<Pathway>(), static_cast<std::size_t>(insertionIndex), static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1);
                    }
                    ++insertionIndex;
                }
            });
        }

        template <class Pathway, class IndexTypeTuple>
        void moveTopL(IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveTopL));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto) {
                auto & sel = getSelections<Pathway>();
                writeSelections(events, sel);
                auto movedIndexes = sel; // Copy so it can be sorted without changing selections
                std::sort(movedIndexes.begin(), movedIndexes.end());

                std::size_t size = std::size(ref);
                std::ptrdiff_t insertionIndex = 0;
                for ( auto i : movedIndexes )
                {
                    if ( static_cast<std::ptrdiff_t>(i) > insertionIndex && static_cast<std::size_t>(i) < size )
                    {
                        auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                        std::rotate(std::next(ref.begin(), insertionIndex), it, it+1);
                        mirrorRotationToSelection(getSelections<Pathway>(), static_cast<std::size_t>(insertionIndex), static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1);
                    }
                    ++insertionIndex;
                }
            });
        }

        template <class Pathway, class MovedIndex, class IndexTypeTuple>
        void moveDown(MovedIndex movedIndex, IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveDown));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                serializeIndex<decltype(member)>(movedIndex);
                if ( static_cast<std::size_t>(movedIndex)+1 < std::size(ref) )
                {
                    std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex)+1]);
                    mirrorSwapToSelection(getSelections<Pathway>(), static_cast<std::size_t>(movedIndex), static_cast<std::size_t>(movedIndex)+1);
                }
            });
        }

        template <class Pathway, class MovedIndexes, class IndexTypeTuple>
        void moveDownN(MovedIndexes && movedIndexes, IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveDownN));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                std::sort(movedIndexes.begin(), movedIndexes.end(), std::greater<RareTs::element_type_t<std::remove_cvref_t<MovedIndexes>>>{});
                serializeIndex<decltype(member)>(std::size(movedIndexes));
                for ( auto i : movedIndexes )
                    serializeIndex<decltype(member)>(i);

                auto size = std::size(ref);
                if ( size > 0 )
                {
                    auto limit = size;
                    for ( auto movedIndex : movedIndexes )
                    {
                        if ( static_cast<std::size_t>(movedIndex)+1 < limit )
                        {
                            std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex)+1]);
                            mirrorSwapToSelection(getSelections<Pathway>(), movedIndex, movedIndex+1);
                            limit = movedIndex+1;
                        }
                        else if ( limit > 0 )
                            --limit;
                    }
                }
            });
        }

        template <class Pathway, class IndexTypeTuple>
        void moveDownL(IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveDownL));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                auto & sel = getSelections<Pathway>();
                writeSelections(events, sel);
                auto movedIndexes = sel; // Copy so it can be sorted without changing selections
                std::sort(movedIndexes.begin(), movedIndexes.end(), std::greater<index_type_t<default_index_type, decltype(member)>>{});

                auto size = std::size(ref);
                if ( size > 0 )
                {
                    auto limit = size;
                    for ( auto movedIndex : movedIndexes )
                    {
                        if ( static_cast<std::size_t>(movedIndex)+1 < limit )
                        {
                            std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex)+1]);
                            mirrorSwapToSelection(getSelections<Pathway>(), movedIndex, movedIndex+1);
                            if constexpr ( hasElementMovedOp<PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>, std::size_t> )
                                notifyElementMoved(user, PathTaggedIndexes<Pathway, Indexes<IndexTypeTuple>>{indexes}, movedIndex, movedIndex+1);
                        }
                        else if ( limit > 0 )
                            --limit;
                    }
                }

            });
        }

        template <class Pathway, class MovedIndex, class IndexTypeTuple>
        void moveBottom(MovedIndex movedIndex, IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveBottom));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                serializeIndex<decltype(member)>(movedIndex);
                if ( movedIndex >= 0 && static_cast<std::size_t>(movedIndex)+1 < std::size(ref) )
                {
                    auto it = std::next(ref.begin(), movedIndex);
                    std::rotate(it, it+1, ref.end());
                    mirrorRotationToSelection(getSelections<Pathway>(), static_cast<std::size_t>(movedIndex), static_cast<std::size_t>(movedIndex)+1, std::size(ref));
                }
            });
        }

        template <class Pathway, class MovedIndexes, class IndexTypeTuple>
        void moveBottomN(MovedIndexes && movedIndexes, IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveBottomN));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                std::sort(movedIndexes.begin(), movedIndexes.end(), std::greater<RareTs::element_type_t<std::remove_cvref_t<MovedIndexes>>>{});
                serializeIndex<decltype(member)>(std::size(movedIndexes));
                for ( auto i : movedIndexes )
                    serializeIndex<decltype(member)>(i);
                
                writeSelections(events, getSelections<Pathway>());

                std::size_t size = std::size(ref);
                std::ptrdiff_t insertionIndex = static_cast<std::ptrdiff_t>(size);
                for ( auto i : movedIndexes )
                {
                    if ( i >= 0 && static_cast<std::ptrdiff_t>(i)+1 < insertionIndex )
                    {
                        auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                        std::rotate(it, it+1, std::next(ref.begin(), insertionIndex));
                        mirrorRotationToSelection(getSelections<Pathway>(), static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertionIndex));
                    }
                    --insertionIndex;
                }
            });
        }

        template <class Pathway, class IndexTypeTuple>
        void moveBottomL(IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveBottomL));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                auto & sel = getSelections<Pathway>();
                writeSelections(events, sel);
                auto movedIndexes = sel; // Copy so it can be sorted without changing selections
                std::sort(movedIndexes.begin(), movedIndexes.end(), std::greater<index_type_t<default_index_type, decltype(member)>>{});

                std::size_t size = std::size(ref);
                std::ptrdiff_t insertionIndex = static_cast<std::ptrdiff_t>(size);
                for ( auto i : movedIndexes )
                {
                    if ( i >= 0 && static_cast<std::ptrdiff_t>(i)+1 < insertionIndex )
                    {
                        auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                        std::rotate(it, it+1, std::next(ref.begin(), insertionIndex));
                        mirrorRotationToSelection(getSelections<Pathway>(), static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertionIndex));
                    }
                    --insertionIndex;
                }
            });
        }

        template <class Pathway, class MovedIndex, class IndexMovedTo, class IndexTypeTuple>
        void moveTo(MovedIndex movedIndex, IndexMovedTo indexMovedTo, IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveTo));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                serializeIndex<decltype(member)>(indexMovedTo);
                serializeIndex<decltype(member)>(movedIndex);

                auto it = std::next(ref.begin(), movedIndex);
                if ( static_cast<std::ptrdiff_t>(indexMovedTo) < static_cast<std::ptrdiff_t>(movedIndex) && indexMovedTo >= 0 && static_cast<std::size_t>(movedIndex) < std::size(ref) )
                {
                    std::rotate(std::next(ref.begin(), indexMovedTo), it, it+1);
                    mirrorRotationToSelection(getSelections<Pathway>(), static_cast<std::size_t>(indexMovedTo), static_cast<std::size_t>(movedIndex), static_cast<std::size_t>(movedIndex)+1);
                }
                else if ( static_cast<std::ptrdiff_t>(indexMovedTo) > static_cast<std::ptrdiff_t>(movedIndex) && movedIndex >= 0 && static_cast<std::size_t>(indexMovedTo) < std::size(ref) )
                {
                    std::rotate(it, it+1, std::next(ref.begin(), indexMovedTo)+1);
                    mirrorRotationToSelection(getSelections<Pathway>(), static_cast<std::size_t>(movedIndex), static_cast<std::size_t>(movedIndex)+1, static_cast<std::size_t>(indexMovedTo)+1);
                }
            });
        }

        template <class Pathway, class MovedIndexes, class IndexMovedTo, class IndexTypeTuple>
        void moveToN(MovedIndexes && movedIndexes, IndexMovedTo indexMovedTo, IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveToN));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                std::sort(movedIndexes.begin(), movedIndexes.end());
                serializeIndex<decltype(member)>(indexMovedTo);
                serializeIndex<decltype(member)>(std::size(movedIndexes));
                for ( auto i : movedIndexes )
                    serializeIndex<decltype(member)>(i);

                auto & sel = getSelections<Pathway>();
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
                                leftChunkFirst = rightChunkFirst-leftChunkSize;
                            }
                            else // Left chunk closer to target, move right up to left
                            {
                                auto rightStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(rightChunkFirst));
                                auto rightEnd = std::next(rightStart, static_cast<std::ptrdiff_t>(rightChunkSize));
                                std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst+leftChunkSize)), rightStart, rightEnd);
                                mirrorRotationToSelection(sel, leftChunkFirst+leftChunkSize, rightChunkFirst, rightChunkFirst+rightChunkSize);
                                rightChunkFirst = leftChunkFirst+leftChunkSize;
                            }
                        }
                        if ( leftChunkFirst < static_cast<std::size_t>(indexMovedTo) ) // Rotate combined chunk rightwards to final position
                        {
                            auto chunkStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst));
                            auto chunkEnd = std::next(chunkStart, static_cast<std::ptrdiff_t>(countValidIndexes));
                            std::rotate(chunkStart, chunkEnd, std::next(chunkEnd, static_cast<std::ptrdiff_t>(static_cast<std::size_t>(indexMovedTo)-leftChunkFirst)));
                            mirrorRotationToSelection(sel, leftChunkFirst, leftChunkFirst+countValidIndexes, leftChunkFirst+countValidIndexes+static_cast<std::size_t>(indexMovedTo)-leftChunkFirst);
                        }
                        else if ( leftChunkFirst > static_cast<std::size_t>(indexMovedTo) ) // Rotate combined chunk leftwards to final position
                        {
                            auto chunkStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst));
                            auto chunkEnd = std::next(chunkStart, static_cast<std::ptrdiff_t>(countValidIndexes));
                            std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(indexMovedTo)), chunkStart, chunkEnd);
                            mirrorRotationToSelection(sel, static_cast<std::size_t>(indexMovedTo), leftChunkFirst, leftChunkFirst+countValidIndexes);
                        }
                    }
                }
            });
        }

        template <class Pathway, class IndexMovedTo, class IndexTypeTuple>
        void moveToL(IndexMovedTo indexMovedTo, IndexTypeTuple & indexes)
        {
            eventOffsets.push_back(events.size());
            events.push_back(uint8_t(Op::MoveToL));
            serializePathway<Pathway>(indexes);

            operateOn<Pathway>(t, indexes, [&](auto & ref, auto member) {
                auto & sel = getSelections<Pathway>();
                auto movedIndexes = sel; // Copy so it can be sorted without changing selections
                std::sort(movedIndexes.begin(), movedIndexes.end());
                serializeIndex<decltype(member)>(indexMovedTo);
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
                                leftChunkFirst = rightChunkFirst-leftChunkSize;
                            }
                            else // Left chunk closer to target, move right up to left
                            {
                                auto rightStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(rightChunkFirst));
                                auto rightEnd = std::next(rightStart, static_cast<std::ptrdiff_t>(rightChunkSize));
                                std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst+leftChunkSize)), rightStart, rightEnd);
                                mirrorRotationToSelection(sel, leftChunkFirst+leftChunkSize, rightChunkFirst, rightChunkFirst+rightChunkSize);
                                rightChunkFirst = leftChunkFirst+leftChunkSize;
                            }
                        }
                        if ( leftChunkFirst < static_cast<std::size_t>(indexMovedTo) ) // Rotate combined chunk rightwards to final position
                        {
                            auto chunkStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst));
                            auto chunkEnd = std::next(chunkStart, static_cast<std::ptrdiff_t>(countValidIndexes));
                            std::rotate(chunkStart, chunkEnd, std::next(chunkEnd, static_cast<std::ptrdiff_t>(static_cast<std::size_t>(indexMovedTo)-leftChunkFirst)));
                            mirrorRotationToSelection(sel, leftChunkFirst, leftChunkFirst+countValidIndexes, leftChunkFirst+countValidIndexes+static_cast<std::size_t>(indexMovedTo)-leftChunkFirst);
                        }
                        else if ( leftChunkFirst > static_cast<std::size_t>(indexMovedTo) ) // Rotate combined chunk leftwards to final position
                        {
                            auto chunkStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst));
                            auto chunkEnd = std::next(chunkStart, static_cast<std::ptrdiff_t>(countValidIndexes));
                            std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(indexMovedTo)), chunkStart, chunkEnd);
                            mirrorRotationToSelection(sel, static_cast<std::size_t>(indexMovedTo), leftChunkFirst, leftChunkFirst+countValidIndexes);
                        }
                    }
                }
            });
        }

        template <class Value, class Member>
        void readValue(std::size_t & offset, auto & value) const
        {
            if constexpr ( RareTs::is_reflected_v<Value> )
            {
                RareTs::Members<Value>::forEach([&](auto member) {
                    readValue<std::remove_cvref_t<typename decltype(member)::type>, decltype(member)>(offset, member.value(value));
                });
            }
            else if constexpr ( std::is_same_v<Value, std::string> )
            {
                auto stringSize = (index_type_t<default_index_type, Member> &)events[offset];
                offset += sizeof(stringSize);
                if ( stringSize > 0 )
                {
                    value = std::string{(const char*)&events[offset], stringSize};
                    offset += stringSize;
                }
            }
            else if constexpr ( RareTs::is_static_array_v<Value> )
            {
                constexpr std::size_t size = RareTs::static_array_size_v<Value>;
                offset += sizeof(size);
                for ( std::size_t i=0; i<size; ++i )
                    readValue<std::remove_cvref_t<decltype(std::declval<Value>()[0])>, Member>(offset, value[i]);
            }
            else if constexpr ( RareTs::is_iterable_v<Value> )
            {
                using index_type = index_type_t<default_index_type, Member>;
                auto size = static_cast<std::size_t>((index_type &)events[offset]);
                value.resize(std::size_t{size});
                offset += sizeof(index_type);
                for ( decltype(size) i=0; i<size; ++i )
                    readValue<std::remove_cvref_t<decltype(std::declval<Value>()[0])>, Member>(offset, value[i]);
            }
            else
            {
                value = (Value &)events[offset];
                offset += sizeof(Value);
            }
        }

        template <class Value, class Member>
        auto readValue(std::size_t & offset) const
        {
            if constexpr ( std::is_array_v<Value> ) // Avoid trying to return array[] types, make a vector
            {
                std::vector<RareTs::element_type_t<Value>> value { std::size_t{RareTs::static_array_size_v<Value>} };
                readValue<decltype(value), Member>(offset, value);
                return value;
            }
            else
            {
                std::remove_cvref_t<Value> value {};
                readValue<Value, Member>(offset, value);
                return value;
            }
        }

        template <class Value, class Member>
        auto peekValue(std::size_t offset) const // readValue but doesn't change offset
        {
            if constexpr ( std::is_array_v<Value> ) // Avoid trying to return array[] types, make a vector
            {
                std::vector<RareTs::element_type_t<Value>> value { std::size_t{RareTs::static_array_size_v<Value>} };
                readValue<decltype(value), Member>(offset, value);
                return value;
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

        template <class Usr, class Path>
        void notifySelectionsChanged(Usr & user, Path path)
        {
            user.selectionsChanged(path);
        }

        template <class Usr, class Path, class Element>
        void notifyElementAdded(Usr & user, Path path, Element && element)
        {
            user.elementAdded(path, std::forward<Element>(element));
        }

        template <class Usr, class Path>
        void notifyElementRemoved(Usr & user, Path path, std::size_t index)
        {
            user.elementRemoved(path, index);
        }

        template <class Usr, class Path>
        void notifyElementMoved(Usr & user, Path path, std::size_t oldIndex, std::size_t newIndex)
        {
            user.elementMoved(path, oldIndex, newIndex);
        }

        template <class Usr, class Path, class OldValue, class NewValue>
        void notifyValueChanged(Usr & user, Path path, OldValue && oldValue, NewValue && newValue)
        {
            user.valueChanged(path, std::forward<OldValue>(oldValue), std::forward<NewValue>(newValue));
        }

        void undoEvent(std::size_t eventIndex)
        {
            if ( eventIndex >= eventOffsets.size() )
                return;
            
            std::optional<std::size_t> secondaryOffset {}; // For selection ops with multiple callbacks/bifurcated offsets
            std::size_t offset = eventOffsets[eventIndex];
            std::uint8_t op = events[offset];
            ++offset;
            
            processEventRef<T>(t, offset, [&]<class type, class Member, class ... pathway>(auto & ref, auto indexes, RareTs::type_tags<type, Member, pathway...>) {

                using index_type = index_type_t<default_index_type, Member>;
                using path_pack = std::tuple<pathway...>;
                using sel_type = std::remove_cvref_t<decltype(getSelections<path_pack>())>;
                constexpr bool hasSelections = !std::is_null_pointer_v<sel_type>;

                switch ( Op(op) )
                {
                    case Op::Reset:
                    {
                        if constexpr ( hasSelections && requires { ref = readValue<type, Member>(offset); } )
                        {
                            ref = readValue<type, Member>(offset);
                            readSelections(events, offset, getSelections<path_pack>());
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
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( hasSelections && !std::is_void_v<element_type> && requires { ref.reserve(1); } )
                        {
                            readIndex<index_type>(offset); // new size (unused)
                            readValue<element_type, Member>(offset); // assigned value (unused)
                            auto size = static_cast<std::size_t>(readIndex<index_type>(offset)); // prev size

                            std::remove_cvref_t<decltype(ref)> prevContainer;
                            prevContainer.reserve(size);
                            for ( std::size_t i=0; i<size; ++i )
                                prevContainer.push_back(readValue<element_type, Member>(offset));
                                
                            ref = std::move(prevContainer);

                            readSelections(events, offset, getSelections<path_pack>());
                        }
                    }
                    break;
                    case Op::AssignDefault:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( hasSelections && !std::is_void_v<element_type> && requires { ref.reserve(1); } )
                        {
                            readIndex<index_type>(offset); // new size (unused)
                            auto size = static_cast<std::size_t>(readIndex<index_type>(offset)); // old size

                            std::remove_cvref_t<decltype(ref)> prevContainer;
                            prevContainer.reserve(size);
                            for ( std::size_t i=0; i<size; ++i )
                                prevContainer.push_back(readValue<element_type, Member>(offset));
                                
                            ref = std::move(prevContainer);

                            readSelections(events, offset, getSelections<path_pack>());
                        }
                    }
                    break;
                    case Op::ClearSelections:
                    {
                        if constexpr ( hasSelections )
                        {
                            auto & selections = getSelections<path_pack>();
                            sel_type prevSelections {};
                            readSelectionVector(events, offset, prevSelections);
                            std::swap(selections, prevSelections);
                        }
                        if constexpr ( hasSelectionsChangedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>> )
                            notifySelectionsChanged(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes});
                    }
                    break;
                    case Op::SelectAll:
                    {
                        if constexpr ( hasSelections )
                        {
                            auto & selections = getSelections<path_pack>();
                            sel_type prevSelections {};
                            readSelectionVector(events, offset, prevSelections);
                            std::swap(selections, prevSelections);

                            if constexpr ( hasSelectionsChangedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>> )
                                notifySelectionsChanged(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes});
                        }
                    }
                    break;
                    case Op::Select:
                    {
                        auto & selections = getSelections<path_pack>();
                        auto value = static_cast<std::size_t>(readIndex<index_type>(offset));
                        if constexpr ( requires { selections.erase(selections.begin()); } )
                        {
                            auto found = std::find(selections.begin(), selections.end(), value);
                            if ( found != selections.end() )
                                selections.erase(found);
                        }
                        
                        if constexpr ( hasSelectionsChangedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>> )
                            notifySelectionsChanged(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes});
                    }
                    break;
                    case Op::SelectN:
                    {
                        auto & selections = getSelections<path_pack>();
                        auto size = static_cast<std::size_t>(readIndex<index_type>(offset));
                        for ( std::size_t i=0; i<size; ++i )
                        {
                            auto index = static_cast<std::size_t>(readIndex<index_type>(offset));
                            if constexpr ( requires { selections.erase(selections.begin()); } )
                            {
                                auto found = std::find(selections.begin(), selections.end(), index);
                                if ( found != selections.end() )
                                    selections.erase(found);
                            }
                        }
                        if constexpr ( hasSelectionsChangedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>> )
                            notifySelectionsChanged(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes});
                    }
                    break;
                    case Op::Deselect:
                    {
                        auto & selections = getSelections<path_pack>();
                        auto value = readIndex<index_type>(offset);
                        auto selIndex = readIndex<index_type>(offset);
                        if constexpr ( requires { selections.erase(selections.begin()); } )
                        {
                            if ( std::find(selections.begin(), selections.end(), value) == selections.end() )
                                selections.insert(std::next(selections.begin(), static_cast<std::ptrdiff_t>(selIndex)), value);
                        }
                        
                        if constexpr ( hasSelectionsChangedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>> )
                            notifySelectionsChanged(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes});
                    }
                    break;
                    case Op::DeselectN:
                    {
                        auto & selections = getSelections<path_pack>();
                        auto size = static_cast<std::ptrdiff_t>(readIndex<index_type>(offset));

                        std::vector<index_type> deselectIndexes(static_cast<std::size_t>(size), 0);
                        for ( std::ptrdiff_t i=0; i<size; ++i )
                            deselectIndexes[static_cast<std::size_t>(i)] = readIndex<index_type>(offset);

                        std::vector<index_type> prevSelIndexes(static_cast<std::size_t>(size), 0);
                        for ( std::ptrdiff_t i=0; i<size; ++i )
                            prevSelIndexes[static_cast<std::size_t>(i)] = readIndex<index_type>(offset);

                        if constexpr ( requires { selections.insert(selections.begin(), 0); } )
                        {
                            for ( std::ptrdiff_t i=size-1; i>=0; --i )
                            {
                                selections.insert(std::next(selections.begin(), static_cast<std::ptrdiff_t>(prevSelIndexes[static_cast<std::size_t>(i)])),
                                    deselectIndexes[static_cast<std::size_t>(i)]);
                            }
                        }

                        if constexpr ( hasSelectionsChangedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>> )
                            notifySelectionsChanged(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes});
                    }
                    break;
                    case Op::ToggleSelection:
                    {
                        auto & selections = getSelections<path_pack>();
                        auto value = readIndex<index_type>(offset);
                        if constexpr ( requires { selections.erase(selections.begin()); } )
                        {
                            if ( u8bool::read(events, offset) ) // wasSelected
                                selections.insert(std::next(selections.begin(), static_cast<std::ptrdiff_t>(readIndex<index_type>(offset))), value);
                            else
                                std::erase(selections, value);
                        }
                        
                        if constexpr ( hasSelectionsChangedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>> )
                            notifySelectionsChanged(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes});
                    }
                    break;
                    case Op::ToggleSelectionN:
                    {
                        auto & selections = getSelections<path_pack>();
                        auto size = static_cast<std::ptrdiff_t>(readIndex<index_type>(offset));
                        std::vector<index_type> toggledIndexes(static_cast<std::size_t>(size), 0);
                        for ( std::ptrdiff_t i=0; i<size; ++i )
                            toggledIndexes[static_cast<std::size_t>(i)] = readIndex<index_type>(offset);

                        std::vector<bool> wasSelected {};
                        readVecBoolData(events, offset, static_cast<std::size_t>(size), wasSelected);

                        std::vector<index_type> prevSelIndexes(static_cast<std::size_t>(size), 0);
                        for ( std::ptrdiff_t i=0; i<size; ++i )
                            prevSelIndexes[static_cast<std::size_t>(i)] = readIndex<index_type>(offset);

                        for ( std::ptrdiff_t i=size-1; i>=0; --i )
                        {
                            if constexpr ( requires { selections.erase(selections.begin()); } )
                            {
                                auto found = std::find(selections.begin(), selections.end(), toggledIndexes[static_cast<std::size_t>(i)]);
                                if ( found == selections.end() )
                                    selections.insert(std::next(selections.begin(), static_cast<std::ptrdiff_t>(prevSelIndexes[static_cast<std::size_t>(i)])), toggledIndexes[static_cast<std::size_t>(i)]);
                                else
                                    selections.erase(found);
                            }
                        }

                        if constexpr ( hasSelectionsChangedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>> )
                            notifySelectionsChanged(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes});
                    }
                    break;
                    case Op::SortSelections:
                    {
                        if constexpr ( hasSelections )
                        {
                            auto & selections = getSelections<path_pack>();
                            auto size = readIndex<index_type>(offset);
                            std::vector<index_type_t<default_index_type, Member>> sourceIndexes(size, 0);
                            for ( std::size_t i=0; i<size; ++i )
                                sourceIndexes[i] = readIndex<index_type>(offset);

                            undoSort(selections, sourceIndexes);
                        }
                    }
                    break;
                    case Op::SortSelectionsDesc:
                    {
                        if constexpr ( hasSelections )
                        {
                            auto & selections = getSelections<path_pack>();
                            auto size = readIndex<index_type>(offset);
                            std::vector<index_type_t<default_index_type, Member>> sourceIndexes(size, 0);
                            for ( std::size_t i=0; i<size; ++i )
                                sourceIndexes[i] = readIndex<index_type>(offset);

                            undoSort(selections, sourceIndexes);
                        }
                    }
                    break;
                    case Op::Set:
                    {
                        readValue<type, Member>(offset); // newValue (unused)
                        auto prevValue = readValue<type, Member>(offset);
                        if constexpr ( RareTs::is_assignable_v<decltype(ref), decltype(prevValue)> )
                        {
                            if constexpr ( hasValueChangedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>, type> )
                            {
                                auto temp = ref;
                                ref = prevValue;
                                notifyValueChanged(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes}, temp, ref);
                            }
                            else
                                ref = prevValue;

                            if constexpr ( hasSelections )
                                readSelections(events, offset, getSelections<path_pack>());
                        }
                        
                    }
                    break;
                    case Op::SetN:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> )
                        {
                            auto count = static_cast<std::size_t>(readIndex<index_type>(offset));
                            std::vector<std::size_t> setIndexes {};
                            setIndexes.reserve(count);
                            for ( std::size_t i=0; i<count; ++i )
                                setIndexes.push_back(static_cast<std::size_t>(readIndex<index_type>(offset)));

                            readValue<element_type, Member>(offset); // new value (unused);
                            for ( auto index : setIndexes )
                                ref[index] = readValue<element_type, Member>(offset);
                        }
                    }
                    break;
                    case Op::SetL:
                    {
                        if ( !secondaryOffset )
                        {
                            secondaryOffset = offset; // Is set exclusively for the first visit of the selection
                            readValue<type, Member>(*secondaryOffset); // Advanced past valueSetTo (unused for undos, used for redos)
                        }
                        auto prevValue = readValue<type, Member>(*secondaryOffset);
                        if constexpr ( RareTs::is_assignable_v<decltype(ref), decltype(prevValue)> )
                        {
                            if constexpr ( hasValueChangedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>, type> )
                            {
                                auto temp = ref;
                                ref = prevValue;
                                notifyValueChanged(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes}, temp, ref);
                            }
                            else
                                ref = prevValue;
                        }
                    }
                    break;
                    case Op::Append:
                    {
                        if constexpr ( RareTs::has_pop_back_v<decltype(ref)> )
                            ref.pop_back();
                        if constexpr ( hasElementRemovedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>, std::size_t> )
                            notifyElementRemoved(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes}, ref.size());
                    }
                    break;
                    case Op::AppendN:
                    {
                        if constexpr ( requires { ref.size(); ref.erase(ref.begin(), ref.end()); } )
                        {
                            auto count = static_cast<std::ptrdiff_t>(readIndex<index_type>(offset));
                            auto size = static_cast<std::ptrdiff_t>(ref.size());
                            ref.erase(std::next(ref.begin(), size-count), ref.end());
                            if constexpr ( hasElementRemovedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>, std::size_t> )
                            {
                                for ( std::ptrdiff_t i=size; i>size-count; --i )
                                    notifyElementRemoved(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes}, static_cast<std::size_t>(i));
                            }
                        }
                    }
                    break;
                    case Op::Insert:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( hasSelections && !std::is_void_v<element_type> && requires { ref.erase(ref.begin()); } )
                        {
                            auto insertionIndex = readIndex<index_type>(offset);
                            ref.erase(std::next(ref.begin(), static_cast<std::ptrdiff_t>(insertionIndex)));

                            auto & sel = getSelections<path_pack>();
                            for ( auto & index : sel )
                            {
                                if ( static_cast<index_type>(index) >= insertionIndex )
                                    --index;
                            }
                        }
                    }
                    break;
                    case Op::InsertN:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && requires { ref.erase(ref.begin(), ref.end()); } )
                        {
                            auto insertionCount = readIndex<index_type>(offset);
                            auto insertionIndex = readIndex<index_type>(offset);
                            ref.erase(
                                std::next(ref.begin(), static_cast<std::ptrdiff_t>(insertionIndex)),
                                std::next(ref.begin(), static_cast<std::ptrdiff_t>(insertionIndex)+static_cast<std::ptrdiff_t>(insertionCount))
                            );

                            if constexpr ( hasSelections )
                            {
                                auto & sel = getSelections<path_pack>();
                                for ( auto & index : sel )
                                {
                                    if ( static_cast<index_type>(index) >= insertionIndex )
                                        index -= insertionCount;
                                }
                            }
                        }
                    }
                    break;
                    case Op::Remove:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && requires { ref.insert(ref.begin(), std::declval<element_type>()); } )
                        {
                            auto removalIndex = readIndex<index_type>(offset);
                            auto removedValue = readValue<element_type, Member>(offset);
                            ref.insert(std::next(ref.begin(), static_cast<std::ptrdiff_t>(removalIndex)), removedValue);
                            if ( u8bool::read(events, offset) )
                            {
                                if constexpr ( hasSelections )
                                {
                                    auto & sel = getSelections<path_pack>();
                                    auto prevSelIndex = readIndex<index_type>(offset);
                                    sel.insert(std::next(sel.begin(), static_cast<std::ptrdiff_t>(prevSelIndex)), removalIndex);
                                    for ( auto & i : sel )
                                    {
                                        if ( i > removalIndex )
                                            ++i;
                                    }
                                }
                            }
                        }
                    }
                    break;
                    case Op::RemoveN:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && requires { ref.insert(ref.begin(), std::declval<element_type>()); } )
                        {
                            std::ptrdiff_t removalCount = static_cast<std::ptrdiff_t>(readIndex<index_type>(offset));
                            std::vector<index_type> removalIndexes {}; // removalIndexes were pre-sorted highest index to lowest index
                            removalIndexes.reserve(static_cast<std::size_t>(removalCount));
                            for ( std::ptrdiff_t i=0; i<removalCount; ++i )
                                removalIndexes.push_back(readIndex<index_type>(offset));

                            std::vector<element_type> removedValues {};
                            removedValues.reserve(static_cast<std::size_t>(removalCount));
                            for ( std::ptrdiff_t i=0; i<removalCount; ++i )
                                removedValues.push_back(readValue<element_type, Member>(offset));
                            
                            if constexpr ( hasSelections )
                            {
                                auto & sel = getSelections<path_pack>();
                                for ( std::ptrdiff_t i=removalCount-1; i>=0; --i ) // insert values which were removed from the lowest indexes first
                                {
                                    auto reinsertedIndex = removalIndexes[static_cast<std::size_t>(i)];
                                    ref.insert(std::next(ref.begin(), static_cast<std::ptrdiff_t>(reinsertedIndex)), removedValues[static_cast<std::size_t>(i)]);
                                    for ( auto & s : sel )
                                    {
                                        if ( s >= reinsertedIndex )
                                            ++s;
                                    }
                                }

                                std::vector<bool> removalIndexSelected {};
                                readVecBoolData(events, offset, static_cast<std::size_t>(removalCount), removalIndexSelected);

                                std::size_t prevSelIndexCount = static_cast<std::size_t>(readIndex<index_type>(offset));
                                std::vector<index_type> prevSelIndexes(prevSelIndexCount, 0);
                                for ( std::size_t i=0; i<prevSelIndexCount; ++i )
                                    prevSelIndexes[i] = readIndex<index_type>(offset);

                                std::size_t unremovedSelCount = 0;
                                for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(removalCount)-1; i>=0; --i )
                                {
                                    if ( removalIndexSelected[static_cast<std::size_t>(i)] )
                                    {
                                        auto insertedValue = removalIndexes[static_cast<std::size_t>(i)];
                                        sel.insert(std::next(sel.begin(), static_cast<std::ptrdiff_t>(prevSelIndexes[unremovedSelCount])), insertedValue);
                                        ++unremovedSelCount;
                                    }
                                }
                            }
                        }
                    }
                    break;
                    case Op::RemoveL:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && requires { ref.insert(ref.begin(), std::declval<element_type>()); } )
                        {
                            std::ptrdiff_t removalCount = static_cast<std::ptrdiff_t>(readIndex<index_type>(offset));
                            std::vector<std::size_t> removalIndexes {}; // removalIndexes were pre-sorted highest index to lowest index
                            removalIndexes.reserve(static_cast<std::size_t>(removalCount));
                            for ( std::ptrdiff_t i=0; i<removalCount; ++i )
                                removalIndexes.push_back(readIndex<index_type>(offset));

                            std::vector<element_type> removedValues {};
                            removedValues.reserve(static_cast<std::size_t>(removalCount));
                            for ( std::ptrdiff_t i=0; i<removalCount; ++i )
                                removedValues.push_back(readValue<element_type, Member>(offset));

                            for ( std::ptrdiff_t i=removalCount-1; i>=0; --i ) // insert values which were removed from the lowest indexes first
                            {
                                ref.insert(std::next(ref.begin(), static_cast<std::ptrdiff_t>(removalIndexes[static_cast<std::size_t>(i)])), removedValues[static_cast<std::size_t>(i)]);
                                
                                if constexpr ( hasElementAddedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>, std::size_t> )
                                    notifyElementAdded(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes}, static_cast<std::size_t>(removalIndexes[static_cast<std::size_t>(i)]));
                            }

                            readSelections(events, offset, getSelections<path_pack>());
                        }
                    };
                    break;
                    case Op::Sort:
                    {
                        if constexpr ( RareTs::is_specialization_v<RareTs::remove_cvref_t<decltype(ref)>, std::vector> )
                        {
                            using index_type = index_type_t<default_index_type, Member>;
                            std::size_t count = static_cast<std::size_t>(readIndex<index_type>(offset));
                            std::vector<index_type> sourceIndexes {};
                            sourceIndexes.reserve(count);
                            for ( std::size_t i=0; i<count; ++i )
                                sourceIndexes.push_back(readIndex<index_type>(offset));

                            undoSort(ref, sourceIndexes);

                            auto & sel = getSelections<path_pack>();
                            sel_type oldSel {};
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( std::find(sel.begin(), sel.end(), i) != sel.end() )
                                    RareTs::append(oldSel, sourceIndexes[i]);
                            }

                            std::swap(sel, oldSel);
                        }
                    }
                    break;
                    case Op::SortDesc:
                    {
                        if constexpr ( RareTs::is_specialization_v<RareTs::remove_cvref_t<decltype(ref)>, std::vector> )
                        {
                            using index_type = index_type_t<default_index_type, Member>;
                            std::size_t count = static_cast<std::size_t>(readIndex<index_type>(offset));
                            std::vector<index_type> sourceIndexes {};
                            sourceIndexes.reserve(count);
                            for ( std::size_t i=0; i<count; ++i )
                                sourceIndexes.push_back(readIndex<index_type>(offset));
                            
                            undoSort(ref, sourceIndexes);

                            auto & sel = getSelections<path_pack>();
                            sel_type oldSel {};
                            for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(count)-1; i>=0; --i )
                            {
                                if ( std::find(sel.begin(), sel.end(), static_cast<index_type>(i)) != sel.end() )
                                    RareTs::append(oldSel, sourceIndexes[static_cast<std::size_t>(i)]);
                            }

                            std::swap(sel, oldSel);
                        }
                    }
                    break;
                    case Op::MoveUp:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto movedIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                            if ( movedIndex > 0 && movedIndex < std::size(ref) )
                            {
                                std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex-1)]);
                                mirrorSwapToSelection(getSelections<path_pack>(), movedIndex, movedIndex-1);
                            }
                        }
                    }
                    break;
                    case Op::MoveUpN:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto count = static_cast<std::size_t>(readIndex<index_type>(offset));
                            if ( count > 0 )
                            {
                                std::vector<index_type> movedIndexes(count, index_type{0});
                                for ( std::size_t i=0; i<count; ++i )
                                    movedIndexes[i] = readIndex<index_type>(offset);

                                std::size_t minimumIndexMoved = 0;
                                for ( std::size_t i=0; i<count; ++i )
                                {
                                    if ( minimumIndexMoved == movedIndexes[i] )
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
                                        mirrorSwapToSelection(getSelections<path_pack>(), i-1, i);
                                    }
                                }
                            }
                        }
                    }
                    break;
                    case Op::MoveUpL:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( hasSelections && !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto & sel = getSelections<path_pack>();
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
                        }
                    }
                    break;
                    case Op::MoveTop:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto movedIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                            if ( movedIndex > 0 && movedIndex < std::size(ref) )
                            {
                                auto it = ref.begin();
                                std::rotate(it, it+1, std::next(ref.begin(), static_cast<std::ptrdiff_t>(movedIndex))+1);
                                mirrorRotationToSelection(getSelections<path_pack>(), 0, 1, movedIndex+1);
                            }
                        }
                    }
                    break;
                    case Op::MoveTopN:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto count = static_cast<std::size_t>(readIndex<index_type>(offset));
                            std::vector<index_type> movedIndexes(count, index_type{0});
                            for ( std::size_t i=0; i<count; ++i )
                                movedIndexes[i] = readIndex<index_type>(offset);

                            std::size_t minimumIndexMoved = 0;
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( minimumIndexMoved == movedIndexes[i] )
                                    ++minimumIndexMoved;
                                else
                                    break;
                            }

                            std::size_t countValid = 0;
                            for ( auto i : movedIndexes )
                            {
                                if ( i > minimumIndexMoved && static_cast<std::size_t>(i) < std::size(ref) )
                                    ++countValid;
                            }

                            std::size_t i = minimumIndexMoved+countValid-1;
                            for ( auto it = movedIndexes.rbegin(); it != movedIndexes.rend(); ++it )
                            {
                                if ( *it > minimumIndexMoved && static_cast<std::size_t>(*it) < std::size(ref) )
                                {
                                    auto toMove = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                    auto dest = std::next(ref.begin(), static_cast<std::ptrdiff_t>(*it));
                                    std::rotate(toMove, toMove+1, dest+1);
                                    --i;
                                }
                            }

                            if constexpr ( hasSelections )
                            {
                                auto & sel = getSelections<path_pack>();
                                sel_type prevSel {};
                                readSelections(events, offset, prevSel);
                                std::swap(sel, prevSel);
                            }
                        }
                    }
                    break;
                    case Op::MoveTopL:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( hasSelections && !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto & sel = getSelections<path_pack>();
                            sel_type prevSel {};
                            readSelections(events, offset, prevSel);

                            auto movedIndexes = prevSel; // Copy so it can be sorted without changing selections
                            std::sort(movedIndexes.begin(), movedIndexes.end(), std::greater<RareTs::element_type_t<std::remove_cvref_t<decltype(movedIndexes)>>>{});
                            std::size_t toMoveIndex = std::size(prevSel)-1;
                            for ( auto i : movedIndexes )
                            {
                                if ( i > 0 && static_cast<std::size_t>(i) < std::size(ref) )
                                {
                                    auto toMove = std::next(ref.begin(), static_cast<std::ptrdiff_t>(toMoveIndex));
                                    auto dest = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                    std::rotate(toMove, toMove+1, dest+1);
                                    --toMoveIndex;
                                }
                            }
                            std::swap(sel, prevSel);
                        }
                    }
                    break;
                    case Op::MoveDown:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto movedIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                            if ( movedIndex+1 < std::size(ref) )
                            {
                                std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex+1)]);
                                mirrorSwapToSelection(getSelections<path_pack>(), movedIndex, movedIndex+1);
                            }
                        }
                    }
                    break;
                    case Op::MoveDownN:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto count = static_cast<std::size_t>(readIndex<index_type>(offset));
                            if ( count > 0 )
                            {
                                std::vector<index_type> movedIndexes(count, index_type{0});
                                for ( std::size_t i=0; i<count; ++i )
                                    movedIndexes[i] = readIndex<index_type>(offset);

                                std::size_t maximumIndexMoved = std::size(ref);
                                for ( std::size_t i=0; i<count; ++i )
                                {
                                    if ( movedIndexes[i]+1 == maximumIndexMoved )
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
                                        mirrorSwapToSelection(getSelections<path_pack>(), i, i+1);
                                    }
                                }
                            }
                        }
                    }
                    break;
                    case Op::MoveDownL:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( hasSelections && !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto & sel = getSelections<path_pack>();
                            sel_type prevSel {};
                            readSelections(events, offset, prevSel);

                            auto movedIndexes = prevSel;
                            std::sort(movedIndexes.begin(), movedIndexes.end(), std::greater<index_type>{});

                            std::size_t maximumIndexMoved = std::size(ref);
                            for ( std::size_t i=0; i<std::size(movedIndexes); ++i )
                            {
                                if ( movedIndexes[i]+1 == maximumIndexMoved )
                                    --maximumIndexMoved;
                                else
                                    break;
                            }

                            for ( auto it = movedIndexes.rbegin(); it != movedIndexes.rend(); ++it )
                            {
                                auto i = static_cast<std::size_t>(*it);
                                if ( static_cast<std::size_t>(i)+1 < maximumIndexMoved )
                                {
                                    std::swap(ref[static_cast<std::size_t>(i)], ref[static_cast<std::size_t>(i)+1]);
                                    if constexpr ( hasElementMovedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>, std::size_t> )
                                        notifyElementMoved(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes}, i+1, i);
                                }
                            }
                            std::swap(sel, prevSel);
                        }
                    }
                    break;
                    case Op::MoveBottom:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto movedIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                            if ( movedIndex+1 < std::size(ref) )
                            {
                                auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(movedIndex));
                                auto movedTo = std::next(ref.begin(), static_cast<std::ptrdiff_t>(std::size(ref))-1);
                                std::rotate(it, movedTo, ref.end());
                                mirrorRotationToSelection(getSelections<path_pack>(), movedIndex, std::size(ref)-1, std::size(ref));
                            }
                        }
                    }
                    break;
                    case Op::MoveBottomN:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto count = static_cast<std::size_t>(readIndex<index_type>(offset));
                            std::vector<index_type> movedIndexes(count, index_type{0});
                            for ( std::size_t i=0; i<count; ++i )
                                movedIndexes[i] = readIndex<index_type>(offset);

                            std::size_t maximumIndexMoved = std::size(ref);
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( movedIndexes[i]+1 == maximumIndexMoved )
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

                            std::size_t i = maximumIndexMoved-countValid;
                            for ( auto it = movedIndexes.rbegin(); it != movedIndexes.rend(); ++it )
                            {
                                if ( static_cast<std::size_t>(*it)+1 < maximumIndexMoved )
                                {
                                    auto toMove = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                    auto dest = std::next(ref.begin(), static_cast<std::ptrdiff_t>(*it));
                                    std::rotate(dest, toMove, toMove+1);
                                    ++i;
                                }
                            }

                            if constexpr ( hasSelections )
                            {
                                auto & sel = getSelections<path_pack>();
                                sel_type prevSel {};
                                readSelections(events, offset, prevSel);
                                std::swap(sel, prevSel);
                            }
                        }
                    }
                    break;
                    case Op::MoveBottomL:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( hasSelections && !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto & sel = getSelections<path_pack>();
                            sel_type prevSel {};
                            readSelections(events, offset, prevSel);

                            auto movedIndexes = prevSel; // Copy so it can be sorted without changing selections
                            std::sort(movedIndexes.begin(), movedIndexes.end());
                            std::size_t toMoveIndex = std::size(ref)-std::size(prevSel);
                            for ( auto i : movedIndexes )
                            {
                                if ( static_cast<std::size_t>(i)+1 < std::size(ref) )
                                {
                                    auto toMove = std::next(ref.begin(), static_cast<std::ptrdiff_t>(toMoveIndex));
                                    auto dest = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                    std::rotate(dest, toMove, toMove+1);
                                    ++toMoveIndex;
                                }
                            }

                            std::swap(sel, prevSel);
                        }
                    }
                    break;
                    case Op::MoveTo:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto indexMovedTo = static_cast<std::size_t>(readIndex<index_type>(offset));
                            auto movedIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                            
                            auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(movedIndex));
                            auto target = std::next(ref.begin(), static_cast<std::ptrdiff_t>(indexMovedTo));
                            if ( indexMovedTo < movedIndex && movedIndex < std::size(ref) )
                            {
                                std::rotate(target, target+1, it+1);
                                mirrorRotationToSelection(getSelections<path_pack>(), indexMovedTo, indexMovedTo+1, movedIndex+1);
                            }
                            else if ( indexMovedTo > movedIndex && indexMovedTo < std::size(ref) )
                            {
                                std::rotate(it, target, target+1);
                                mirrorRotationToSelection(getSelections<path_pack>(), movedIndex, indexMovedTo, indexMovedTo+1);
                            }
                        }
                    }
                    break;
                    case Op::MoveToN:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            // Calculate the rotates you would perform in the forward direction, then reverse the rotations
                            std::vector<Rotation> rotations {};
                            
                            auto indexMovedTo = static_cast<std::size_t>(readIndex<index_type>(offset));
                            auto countMovedIndexes = static_cast<std::size_t>(readIndex<index_type>(offset));
                            std::vector<index_type> movedIndexes(countMovedIndexes, index_type{0});
                            for ( std::size_t i=0; i<countMovedIndexes; ++i )
                                movedIndexes[i] = readIndex<index_type>(offset);

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
                                            rotations.push_back(Rotation::makeReverse(i, i+1, static_cast<std::size_t>(insertionIndex)));

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
                                        rotations.push_back(Rotation::makeReverse(movedIndexes[i-1]+1, rightChunkFirst, rightChunkFirst+rightChunkSize));
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
                            for ( auto it = rotations.rbegin(); it != rotations.rend(); ++it )
                                it->perform(ref);

                            if constexpr ( hasSelections )
                            {
                                auto & sel = getSelections<path_pack>();
                                sel_type prevSel {};
                                readSelections(events, offset, prevSel);
                                std::swap(sel, prevSel);
                            }
                        }
                    }
                    break;
                    case Op::MoveToL:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( hasSelections && !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            // Calculate the rotates you would perform in the forward direction, then reverse the rotations
                            std::vector<Rotation> rotations {};
                            
                            auto indexMovedTo = static_cast<std::size_t>(readIndex<index_type>(offset));
                            
                            auto & sel = getSelections<path_pack>();
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
                                            rotations.push_back(Rotation::makeReverse(i, i+1, static_cast<std::size_t>(insertionIndex)));

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
                                        rotations.push_back(Rotation::makeReverse(movedIndexes[i-1]+1, rightChunkFirst, rightChunkFirst+rightChunkSize));
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
                            for ( auto it = rotations.rbegin(); it != rotations.rend(); ++it )
                                it->perform(ref);

                            std::swap(sel, prevSel);
                        }
                    }
                    break;
                }
            });
        }

        void redoEvent(std::size_t eventIndex)
        {
            if ( eventIndex >= eventOffsets.size() )
                return;
            
            std::optional<std::size_t> secondaryOffset {}; // For selection ops with multiple callbacks/bifurcated offsets
            std::size_t offset = eventOffsets[eventIndex];
            std::uint8_t op = events[offset];
            ++offset;
            
            processEventRef<T>(t, offset, [&]<class type, class Member, class ... pathway>(auto & ref, auto indexes, RareTs::type_tags<type, Member, pathway...>) {

                using index_type = index_type_t<default_index_type, Member>;
                using path_pack = std::tuple<pathway...>;
                using sel_type = std::remove_cvref_t<decltype(getSelections<path_pack>())>;
                constexpr bool hasSelections = !std::is_null_pointer_v<sel_type>;

                switch ( Op(op) )
                {
                    case Op::Reset:
                    {
                        if constexpr ( !RareTs::is_static_array_v<std::remove_cvref_t<decltype(ref)>> )
                        {
                            ref = {};
                            clearSel(getSelections<path_pack>());
                        }
                    }
                    break;
                    case Op::Reserve:
                    {
                        if constexpr ( RareTs::is_specialization_v<std::remove_cvref_t<decltype(ref)>, std::vector> )
                            ref.reserve(static_cast<std::size_t>(readIndex<index_type>(offset)));
                    }
                    break;
                    case Op::Trim:
                    {
                        if constexpr ( RareTs::is_specialization_v<std::remove_cvref_t<decltype(ref)>, std::vector> )
                            ref.shrink_to_fit();
                    }
                    break;
                    case Op::Assign:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && !RareTs::is_static_array_v<std::remove_cvref_t<decltype(ref)>> && !std::is_same_v<std::string, std::remove_cvref_t<decltype(ref)>> )
                        {
                            std::size_t count = static_cast<std::size_t>(readIndex<index_type>(offset));
                            auto value = readValue<element_type, Member>(offset);
                            ref.assign(count, value);
                            clearSel(getSelections<path_pack>());
                        }
                    }
                    break;
                    case Op::AssignDefault:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && !RareTs::is_static_array_v<std::remove_cvref_t<decltype(ref)>> && !std::is_same_v<std::string, std::remove_cvref_t<decltype(ref)>> )
                        {
                            std::size_t size = static_cast<std::size_t>(readIndex<index_type>(offset));
                            ref = std::remove_cvref_t<decltype(ref)>(size);
                            clearSel(getSelections<path_pack>());
                        }
                    }
                    break;
                    case Op::ClearSelections:
                    {
                        auto & selections = getSelections<path_pack>();
                        clearSel(selections);
                        if constexpr ( hasSelectionsChangedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>> )
                            notifySelectionsChanged(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes});
                    }
                    break;
                    case Op::SelectAll:
                    {
                        if constexpr ( hasSelections && requires { std::size(ref); } )
                        {
                            auto & selections = getSelections<path_pack>();
                            RareTs::clear(selections);
                            selections.assign(std::size(ref), index_type{0});
                            std::iota(selections.begin(), selections.end(), index_type{0});

                            if constexpr ( hasSelectionsChangedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>> )
                                notifySelectionsChanged(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes});
                        }
                    }
                    break;
                    case Op::Select:
                    {
                        if constexpr ( hasSelections )
                        {
                            auto & selections = getSelections<path_pack>();
                            auto value = readIndex<index_type>(offset);
                            if ( std::find(selections.begin(), selections.end(), value) == selections.end() )
                                selections.push_back(value);
                        
                            if constexpr ( hasSelectionsChangedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>> )
                                notifySelectionsChanged(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes});
                        }
                    }
                    break;
                    case Op::SelectN:
                    {
                        if constexpr ( hasSelections )
                        {
                            auto & selections = getSelections<path_pack>();
                            auto size = static_cast<std::size_t>(readIndex<index_type>(offset));
                            for ( std::size_t i=0; i<size; ++i )
                            {
                                auto index = readIndex<index_type>(offset);
                                if constexpr ( requires { selections.erase(selections.begin()); } )
                                {
                                    if ( std::find(selections.begin(), selections.end(), index) == selections.end() )
                                        selections.push_back(index);
                                }
                            }
                            if constexpr ( hasSelectionsChangedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>> )
                                notifySelectionsChanged(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes});
                        }
                    }
                    break;
                    case Op::Deselect:
                    {
                        auto & selections = getSelections<path_pack>();
                        auto value = static_cast<std::size_t>(readIndex<index_type>(offset));
                        if constexpr ( requires { selections.erase(selections.begin()); } )
                        {
                            auto found = std::find(selections.begin(), selections.end(), value);
                            if ( found != selections.end() )
                                selections.erase(found);
                        }
                        
                        if constexpr ( hasSelectionsChangedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>> )
                            notifySelectionsChanged(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes});
                    }
                    break;
                    case Op::DeselectN:
                    {
                        auto & selections = getSelections<path_pack>();
                        auto size = static_cast<std::size_t>(readIndex<index_type>(offset));
                        for ( std::size_t i=0; i<size; ++i )
                        {
                            auto index = readIndex<index_type>(offset);
                            if constexpr ( requires { selections.erase(selections.begin()); } )
                            {
                                auto found = std::find(selections.begin(), selections.end(), index);
                                if ( found != selections.end() )
                                    selections.erase(found);
                            }
                        }
                        if constexpr ( hasSelectionsChangedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>> )
                            notifySelectionsChanged(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes});
                    }
                    break;
                    case Op::ToggleSelection:
                    {
                        auto & selections = getSelections<path_pack>();
                        auto value = readIndex<index_type>(offset);
                        readIndex<index_type>(offset);
                        if constexpr ( requires { selections.erase(selections.begin()); } )
                        {
                            auto found = std::find(selections.begin(), selections.end(), value);
                            if ( found == selections.end() )
                                RareTs::append(selections, value);
                            else
                                selections.erase(found);
                        }
                        
                        if constexpr ( hasSelectionsChangedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>> )
                            notifySelectionsChanged(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes});
                    }
                    break;
                    case Op::ToggleSelectionN:
                    {
                        auto & selections = getSelections<path_pack>();
                        auto size = static_cast<std::size_t>(readIndex<index_type>(offset));
                        for ( std::size_t i=0; i<size; ++i )
                        {
                            auto index = readIndex<index_type>(offset);
                            if constexpr ( requires { selections.erase(selections.begin()); } )
                            {
                                auto found = std::find(selections.begin(), selections.end(), index);
                                if ( found == selections.end() )
                                    RareTs::append(selections, index);
                                else
                                    selections.erase(found);
                            }
                        }
                        if constexpr ( hasSelectionsChangedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>> )
                            notifySelectionsChanged(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes});
                    }
                    break;
                    case Op::SortSelections:
                    {
                        if constexpr ( hasSelections )
                        {
                            auto & selections = getSelections<path_pack>();
                            auto size = readIndex<index_type>(offset);
                            std::vector<index_type_t<default_index_type, Member>> sourceIndexes(size, 0);
                            for ( std::size_t i=0; i<size; ++i )
                                sourceIndexes[i] = readIndex<index_type>(offset);

                            redoSort(selections, sourceIndexes);
                        }
                    }
                    break;
                    case Op::SortSelectionsDesc:
                    {
                        if constexpr ( hasSelections )
                        {
                            auto & selections = getSelections<path_pack>();
                            auto size = readIndex<index_type>(offset);
                            std::vector<index_type_t<default_index_type, Member>> sourceIndexes(size, 0);
                            for ( std::size_t i=0; i<size; ++i )
                                sourceIndexes[i] = readIndex<index_type>(offset);

                            redoSort(selections, sourceIndexes);
                        }
                    }
                    break;
                    case Op::Set:
                    {
                        auto newValue = readValue<type, Member>(offset);
                        readValue<type, Member>(offset); // prevValue (unused)
                        if constexpr ( RareTs::is_assignable_v<decltype(ref), decltype(newValue)> )
                        {
                            if constexpr ( hasValueChangedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>, type> )
                            {
                                auto temp = ref;
                                ref = newValue;
                                notifyValueChanged(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes}, temp, ref);
                            }
                            else
                                ref = newValue;

                            if constexpr ( hasSelections )
                                clearSel(getSelections<path_pack>());
                        }
                    }
                    break;
                    case Op::SetN:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> )
                        {
                            auto count = static_cast<std::size_t>(readIndex<index_type>(offset));
                            std::vector<std::size_t> setIndexes {};
                            setIndexes.reserve(count);
                            for ( std::size_t i=0; i<count; ++i )
                                setIndexes.push_back(static_cast<std::size_t>(readIndex<index_type>(offset)));

                            auto newValue = readValue<element_type, Member>(offset);
                            for ( auto index : setIndexes )
                                ref[index] = newValue;
                        }
                    }
                    break;
                    case Op::SetL:
                    {
                        if constexpr ( RareTs::is_assignable_v<decltype(ref), decltype(peekValue<type, Member>(*secondaryOffset))> )
                        {
                            if ( !secondaryOffset )
                                secondaryOffset = offset; // Is set exclusively for the first visit of the selection

                            if constexpr ( hasValueChangedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>, type> )
                            {
                                auto temp = ref;
                                ref = peekValue<type, Member>(*secondaryOffset);
                                notifyValueChanged(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes}, temp, ref);
                            }
                            else
                                ref = peekValue<type, Member>(*secondaryOffset);
                        }
                    }
                    break;
                    case Op::Append:
                    {
                        if constexpr ( RareTs::is_specialization_v<type, std::vector> )
                        {
                            auto value = readValue<std::remove_cvref_t<decltype(std::declval<type>()[0])>, Member>(offset);
                            ref.push_back(std::forward<decltype(value)>(value));
                            if constexpr ( hasElementAddedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>, std::size_t> )
                                notifyElementAdded(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes}, ref.size()-1);
                        }
                    }
                    break;
                    case Op::AppendN:
                    {
                        using element_type = RareTs::element_type_t<std::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && !RareTs::is_static_array_v<std::remove_cvref_t<decltype(ref)>> && !std::is_same_v<std::string, std::remove_cvref_t<decltype(ref)>> )
                        {
                            auto count = static_cast<std::size_t>(readIndex<index_type>(offset));
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                ref.push_back(readValue<element_type, Member>(offset));
                                if constexpr ( hasElementAddedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>, std::size_t> )
                                    notifyElementAdded(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes}, ref.size()-1);
                            }
                        }
                    }
                    break;
                    case Op::Insert:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && !RareTs::is_static_array_v<std::remove_cvref_t<decltype(ref)>> && !std::is_same_v<std::string, std::remove_cvref_t<decltype(ref)>> )
                        {
                            auto insertionIndex = readIndex<index_type>(offset);
                            auto insertedValue = readValue<element_type, Member>(offset);
                            ref.insert(std::next(ref.begin(), static_cast<std::ptrdiff_t>(insertionIndex)), insertedValue);

                            auto & sel = getSelections<path_pack>();
                            for ( auto & index : sel )
                            {
                                if ( static_cast<index_type>(index)  >= insertionIndex )
                                    ++index;
                            }
                        }
                    }
                    break;
                    case Op::InsertN:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && !RareTs::is_static_array_v<std::remove_cvref_t<decltype(ref)>> && !std::is_same_v<std::string, std::remove_cvref_t<decltype(ref)>> )
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

                            auto & sel = getSelections<path_pack>();
                            for ( auto & index : sel )
                            {
                                if ( static_cast<index_type>(index) >= insertionIndex )
                                    index += static_cast<index_type>(insertionCount);
                            }
                        }
                    }
                    break;
                    case Op::Remove:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && !RareTs::is_static_array_v<std::remove_cvref_t<decltype(ref)>> && !std::is_same_v<std::string, std::remove_cvref_t<decltype(ref)>> )
                        {
                            auto removalIndex = readIndex<index_type>(offset);
                            ref.erase(std::next(ref.begin(), static_cast<std::ptrdiff_t>(removalIndex)));
                            auto & sel = getSelections<path_pack>();
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
                        }
                    }
                    break;
                    case Op::RemoveN:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && !RareTs::is_static_array_v<std::remove_cvref_t<decltype(ref)>> && !std::is_same_v<std::string, std::remove_cvref_t<decltype(ref)>> )
                        {
                            auto & sel = getSelections<path_pack>();
                            std::size_t removalCount = static_cast<std::size_t>(readIndex<index_type>(offset));
                            for ( std::size_t i=0; i<removalCount; ++i )
                            {
                                std::size_t indexRemoved = static_cast<std::size_t>(readIndex<index_type>(offset));
                                ref.erase(std::next(ref.begin(), static_cast<std::ptrdiff_t>(indexRemoved)));
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
                    }
                    break;
                    case Op::RemoveL:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && !RareTs::is_static_array_v<std::remove_cvref_t<decltype(ref)>> && !std::is_same_v<std::string, std::remove_cvref_t<decltype(ref)>> )
                        {
                            std::size_t removalCount = static_cast<std::size_t>(readIndex<index_type>(offset));
                            for ( std::size_t i=0; i<removalCount; ++i )
                            {
                                auto index = readIndex<index_type>(offset);
                                ref.erase(std::next(ref.begin(), static_cast<std::ptrdiff_t>(index)));
                                if constexpr ( hasElementRemovedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>, std::size_t> )
                                    notifyElementRemoved(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes}, static_cast<std::size_t>(index));
                            }

                            clearSel(getSelections<path_pack>());
                        }
                    }
                    break;
                    case Op::Sort:
                    {
                        if constexpr ( RareTs::is_specialization_v<RareTs::remove_cvref_t<decltype(ref)>, std::vector> )
                        {
                            using index_type = index_type_t<default_index_type, Member>;
                            std::size_t count = static_cast<std::size_t>(readIndex<index_type>(offset));
                            std::vector<index_type> sourceIndexes {};
                            sourceIndexes.reserve(count);
                            for ( std::size_t i=0; i<count; ++i )
                                sourceIndexes.push_back(readIndex<index_type>(offset));
                            
                            redoSort(ref, sourceIndexes);

                            if constexpr ( hasSelections )
                            {
                                auto & sel = getSelections<path_pack>();
                                sel_type newSel{};
                                for ( std::size_t i=0; i<count; ++i )
                                {
                                    if ( std::find(sel.begin(), sel.end(), sourceIndexes[i]) != sel.end() )
                                        RareTs::append(newSel, static_cast<index_type>(i));
                                }

                                std::swap(sel, newSel);
                            }
                        }
                    }
                    break;
                    case Op::SortDesc:
                    {
                        if constexpr ( RareTs::is_specialization_v<RareTs::remove_cvref_t<decltype(ref)>, std::vector> )
                        {
                            using index_type = index_type_t<default_index_type, Member>;
                            std::size_t count = static_cast<std::size_t>(readIndex<index_type>(offset));
                            std::vector<index_type> sourceIndexes {};
                            sourceIndexes.reserve(count);
                            for ( std::size_t i=0; i<count; ++i )
                                sourceIndexes.push_back(readIndex<index_type>(offset));
                            
                            redoSort(ref, sourceIndexes);

                            if constexpr ( hasSelections )
                            {
                                auto & sel = getSelections<path_pack>();
                                sel_type newSel {};
                                for ( std::size_t i=0; i<count; ++i )
                                {
                                    if ( std::find(sel.begin(), sel.end(), sourceIndexes[i]) != sel.end() )
                                        RareTs::append(newSel, static_cast<index_type>(i));
                                }
                                std::swap(sel, newSel);
                            }
                        }
                    }
                    break;
                    case Op::MoveUp:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto movedIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                            if ( movedIndex > 0 && movedIndex < std::size(ref) )
                            {
                                std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex-1)]);
                                mirrorSwapToSelection(getSelections<path_pack>(), movedIndex, movedIndex-1);
                            }
                        }
                    }
                    break;
                    case Op::MoveUpN:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            std::size_t nextAvailable = 0;
                            auto count = static_cast<std::size_t>(readIndex<index_type>(offset));
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                auto movedIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                                if ( movedIndex > nextAvailable && static_cast<std::size_t>(movedIndex) < std::size(ref) )
                                {
                                    std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex-1)]);
                                    mirrorSwapToSelection(getSelections<path_pack>(), movedIndex, movedIndex-1);
                                }
                                else if ( movedIndex == nextAvailable )
                                    ++nextAvailable;
                            }
                        }
                    }
                    break;
                    case Op::MoveUpL:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( hasSelections && !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto & sel = getSelections<path_pack>();

                            auto movedIndexes = sel; // Copy so it can be sorted without changing selections
                            std::sort(movedIndexes.begin(), movedIndexes.end());
                
                            std::size_t nextAvailable = 0;
                            for ( auto movedIndex : movedIndexes )
                            {
                                if ( movedIndex > nextAvailable && static_cast<std::size_t>(movedIndex) < std::size(ref) )
                                {
                                    std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex-1)]);
                                    mirrorSwapToSelection(sel, movedIndex, movedIndex-1);
                                    nextAvailable = movedIndex;
                                }
                                else if ( movedIndex == nextAvailable )
                                    ++nextAvailable;
                            }
                        }
                    }
                    break;
                    case Op::MoveTop:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto movedIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                            if ( movedIndex > 0 && movedIndex < std::size(ref) )
                            {
                                auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(movedIndex));
                                std::rotate(ref.begin(), it, it+1);
                                mirrorRotationToSelection(getSelections<path_pack>(), 0, movedIndex, movedIndex+1);
                            }
                        }
                    }
                    break;
                    case Op::MoveTopN:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto & sel = getSelections<path_pack>();
                            auto count = static_cast<std::size_t>(readIndex<index_type>(offset));
                            std::vector<index_type> movedIndexes(count, index_type{0});
                            for ( std::size_t i=0; i<count; ++i )
                                movedIndexes[i] = readIndex<index_type>(offset);

                            std::size_t size = std::size(ref);
                            std::ptrdiff_t insertionIndex = 0;
                            for ( auto i : movedIndexes )
                            {
                                if ( static_cast<std::ptrdiff_t>(i) > insertionIndex && static_cast<std::size_t>(i) < size )
                                {
                                    auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                    std::rotate(std::next(ref.begin(), insertionIndex), it, it+1);
                                    mirrorRotationToSelection(sel, static_cast<std::size_t>(insertionIndex), static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1);
                                }
                                ++insertionIndex;
                            }
                        }
                    }
                    break;
                    case Op::MoveTopL:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( hasSelections && !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto & sel = getSelections<path_pack>();
                            sel_type prevSel {};
                            readSelections(events, offset, prevSel);
                            std::sort(prevSel.begin(), prevSel.end());

                            std::size_t size = std::size(ref);
                            std::ptrdiff_t insertionIndex = 0;
                            for ( auto i : prevSel )
                            {
                                if ( static_cast<std::ptrdiff_t>(i) > insertionIndex && static_cast<std::size_t>(i) < size )
                                {
                                    auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                    std::rotate(std::next(ref.begin(), insertionIndex), it, it+1);
                                    mirrorRotationToSelection(sel, static_cast<std::size_t>(insertionIndex), i, i+1);
                                }
                                ++insertionIndex;
                            }
                        }
                    }
                    break;
                    case Op::MoveDown:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto movedIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                            if ( movedIndex+1 < std::size(ref) )
                            {
                                std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex+1)]);
                                mirrorSwapToSelection(getSelections<path_pack>(), movedIndex, movedIndex+1);
                            }
                        }
                    }
                    break;
                    case Op::MoveDownN:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto size = std::size(ref);
                            if ( size > 0 )
                            {
                                auto limit = size;
                                auto count = static_cast<std::size_t>(readIndex<index_type>(offset));
                                for ( std::size_t i=0; i<count; ++i )
                                {
                                    auto movedIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                                    if ( static_cast<std::size_t>(movedIndex)+1 < limit )
                                    {
                                        std::swap(ref[static_cast<std::size_t>(movedIndex)], ref[static_cast<std::size_t>(movedIndex)+1]);
                                        mirrorSwapToSelection(getSelections<path_pack>(), movedIndex, movedIndex+1);
                                        limit = movedIndex+1;
                                    }
                                    else if ( limit > 0 )
                                        --limit;
                                }
                            }
                        }
                    }
                    break;
                    case Op::MoveDownL:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( hasSelections && !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto & sel = getSelections<path_pack>();
                            sel_type prevSel {};
                            readSelections(events, offset, prevSel);

                            auto movedIndexes = prevSel; // Copy so it can be sorted without changing selections
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
                                        mirrorSwapToSelection(sel, movedIndex, movedIndex+1);
                                        limit = movedIndex+1;
                                        
                                        if constexpr ( hasElementMovedOp<PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>, std::size_t> )
                                            notifyElementMoved(user, PathTaggedIndexes<path_pack, Indexes<decltype(indexes)>>{indexes}, movedIndex, movedIndex+1);
                                    }
                                    else if ( limit > 0 )
                                        --limit;
                                }
                            }
                        }
                    }
                    break;
                    case Op::MoveBottom:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto movedIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                            if ( static_cast<std::size_t>(movedIndex)+1 < std::size(ref) )
                            {
                                auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(movedIndex));
                                std::rotate(it, it+1, ref.end());
                                mirrorRotationToSelection(getSelections<path_pack>(), movedIndex, movedIndex+1, std::size(ref));
                            }
                        }
                    }
                    break;
                    case Op::MoveBottomN:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto & sel = getSelections<path_pack>();
                            auto count = static_cast<std::size_t>(readIndex<index_type>(offset));
                            std::vector<index_type> movedIndexes(count, index_type{0});
                            for ( std::size_t i=0; i<count; ++i )
                                movedIndexes[i] = readIndex<index_type>(offset);

                            std::size_t size = std::size(ref);
                            std::ptrdiff_t insertionIndex = static_cast<std::ptrdiff_t>(size);
                            for ( auto i : movedIndexes )
                            {
                                if ( i >= 0 && static_cast<std::ptrdiff_t>(i)+1 < insertionIndex )
                                {
                                    auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                    std::rotate(it, it+1, std::next(ref.begin(), insertionIndex));
                                    mirrorRotationToSelection(sel, i, i+1, static_cast<std::size_t>(insertionIndex));
                                }
                                --insertionIndex;
                            }
                        }
                    }
                    break;
                    case Op::MoveBottomL:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( hasSelections && !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto & sel = getSelections<path_pack>();
                            sel_type prevSel {};
                            readSelections(events, offset, prevSel);
                            auto movedIndexes = prevSel;
                            std::sort(movedIndexes.begin(), movedIndexes.end(), std::greater<index_type>{});
                            
                            std::size_t size = std::size(ref);
                            std::ptrdiff_t insertionIndex = static_cast<std::ptrdiff_t>(size);
                            for ( auto i : movedIndexes )
                            {
                                if ( i >= 0 && static_cast<std::ptrdiff_t>(i)+1 < insertionIndex )
                                {
                                    auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                    std::rotate(it, it+1, std::next(ref.begin(), insertionIndex));
                                    mirrorRotationToSelection(sel, i, i+1, static_cast<std::size_t>(insertionIndex));
                                }
                                --insertionIndex;
                            }
                        }
                    }
                    break;
                    case Op::MoveTo:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto indexMovedTo = static_cast<std::size_t>(readIndex<index_type>(offset));
                            auto movedIndex = static_cast<std::size_t>(readIndex<index_type>(offset));
                            
                            auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(movedIndex));
                            auto target = std::next(ref.begin(), static_cast<std::ptrdiff_t>(indexMovedTo));
                            if ( indexMovedTo < movedIndex && movedIndex < std::size(ref) )
                            {
                                std::rotate(target, it, it+1);
                                mirrorRotationToSelection(getSelections<path_pack>(), indexMovedTo, movedIndex, movedIndex+1);
                            }
                            else if ( indexMovedTo > movedIndex && indexMovedTo < std::size(ref) )
                            {
                                std::rotate(it, it+1, target+1);
                                mirrorRotationToSelection(getSelections<path_pack>(), movedIndex, movedIndex+1, indexMovedTo+1);
                            }
                        }
                    }
                    break;
                    case Op::MoveToN:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto & sel = getSelections<path_pack>();
                            auto indexMovedTo = static_cast<std::size_t>(readIndex<index_type>(offset));
                            auto countMovedIndexes = static_cast<std::size_t>(readIndex<index_type>(offset));
                            std::vector<index_type> movedIndexes(countMovedIndexes, index_type{0});
                            for ( std::size_t i=0; i<countMovedIndexes; ++i )
                                movedIndexes[i] = readIndex<index_type>(offset);

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
                                        {
                                            auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                            std::rotate(it, it+1, std::next(ref.begin(), insertionIndex));
                                            mirrorRotationToSelection(sel, i, i+1, static_cast<std::size_t>(insertionIndex));
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
                                        leftChunkFirst = static_cast<std::size_t>(movedIndexes[i+1])-leftChunkSize;
                                        ++leftChunkSize;
                                    }
                                    for ( std::size_t i=std::size(movedIndexes)-1; i>0 && movedIndexes[i-1] >= indexMovedTo; --i )
                                    {
                                        // rotate right chunk together with element[movedIndexes[i-1]]
                                        auto rightStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(rightChunkFirst));
                                        auto rightEnd = std::next(rightStart, static_cast<std::ptrdiff_t>(rightChunkSize));
                                        std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(movedIndexes[i-1])+1), rightStart, rightEnd);
                                        mirrorRotationToSelection(sel, movedIndexes[i-1]+1, rightChunkFirst, rightChunkFirst+rightChunkSize);
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
                                            leftChunkFirst = rightChunkFirst-leftChunkSize;
                                        }
                                        else // Left chunk closer to target, move right up to left
                                        {
                                            auto rightStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(rightChunkFirst));
                                            auto rightEnd = std::next(rightStart, static_cast<std::ptrdiff_t>(rightChunkSize));
                                            std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst+leftChunkSize)), rightStart, rightEnd);
                                            mirrorRotationToSelection(sel, leftChunkFirst+leftChunkSize, rightChunkFirst, rightChunkFirst+rightChunkSize);
                                            rightChunkFirst = leftChunkFirst+leftChunkSize;
                                        }
                                    }
                                    if ( leftChunkFirst < static_cast<std::size_t>(indexMovedTo) ) // Rotate combined chunk rightwards to final position
                                    {
                                        auto chunkStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst));
                                        auto chunkEnd = std::next(chunkStart, static_cast<std::ptrdiff_t>(countValidIndexes));
                                        std::rotate(chunkStart, chunkEnd, std::next(chunkEnd, static_cast<std::ptrdiff_t>(static_cast<std::size_t>(indexMovedTo)-leftChunkFirst)));
                                        mirrorRotationToSelection(sel, leftChunkFirst, leftChunkFirst+countValidIndexes, leftChunkFirst+countValidIndexes+static_cast<std::size_t>(indexMovedTo)-leftChunkFirst);
                                    }
                                    else if ( leftChunkFirst > static_cast<std::size_t>(indexMovedTo) ) // Rotate combined chunk leftwards to final position
                                    {
                                        auto chunkStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst));
                                        auto chunkEnd = std::next(chunkStart, static_cast<std::ptrdiff_t>(countValidIndexes));
                                        std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(indexMovedTo)), chunkStart, chunkEnd);
                                        mirrorRotationToSelection(sel, indexMovedTo, leftChunkFirst, leftChunkFirst+countValidIndexes);
                                    }
                                }
                            }
                        }
                    }
                    break;
                    case Op::MoveToL:
                    {
                        using element_type = RareTs::element_type_t<RareTs::remove_cvref_t<decltype(ref)>>;
                        if constexpr ( hasSelections && !std::is_void_v<element_type> && requires { ref.begin(); } )
                        {
                            auto indexMovedTo = static_cast<std::size_t>(readIndex<index_type>(offset));
                            
                            auto & sel = getSelections<path_pack>();
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
                                        {
                                            auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                            std::rotate(it, it+1, std::next(ref.begin(), insertionIndex));
                                            mirrorRotationToSelection(sel, i, i+1, static_cast<std::size_t>(insertionIndex));
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
                                        leftChunkFirst = static_cast<std::size_t>(movedIndexes[i+1])-leftChunkSize;
                                        ++leftChunkSize;
                                    }
                                    for ( std::size_t i=std::size(movedIndexes)-1; i>0 && movedIndexes[i-1] >= indexMovedTo; --i )
                                    {
                                        // rotate right chunk together with element[movedIndexes[i-1]]
                                        auto rightStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(rightChunkFirst));
                                        auto rightEnd = std::next(rightStart, static_cast<std::ptrdiff_t>(rightChunkSize));
                                        std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(movedIndexes[i-1])+1), rightStart, rightEnd);
                                        mirrorRotationToSelection(sel, movedIndexes[i-1]+1, rightChunkFirst, rightChunkFirst+rightChunkSize);
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
                                            leftChunkFirst = rightChunkFirst-leftChunkSize;
                                        }
                                        else // Left chunk closer to target, move right up to left
                                        {
                                            auto rightStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(rightChunkFirst));
                                            auto rightEnd = std::next(rightStart, static_cast<std::ptrdiff_t>(rightChunkSize));
                                            std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst+leftChunkSize)), rightStart, rightEnd);
                                            mirrorRotationToSelection(sel, leftChunkFirst+leftChunkSize, rightChunkFirst, rightChunkFirst+rightChunkSize);
                                            rightChunkFirst = leftChunkFirst+leftChunkSize;
                                        }
                                    }
                                    if ( leftChunkFirst < static_cast<std::size_t>(indexMovedTo) ) // Rotate combined chunk rightwards to final position
                                    {
                                        auto chunkStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst));
                                        auto chunkEnd = std::next(chunkStart, static_cast<std::ptrdiff_t>(countValidIndexes));
                                        std::rotate(chunkStart, chunkEnd, std::next(chunkEnd, static_cast<std::ptrdiff_t>(static_cast<std::size_t>(indexMovedTo)-leftChunkFirst)));
                                        mirrorRotationToSelection(sel, leftChunkFirst, leftChunkFirst+countValidIndexes, leftChunkFirst+countValidIndexes+static_cast<std::size_t>(indexMovedTo)-leftChunkFirst);
                                    }
                                    else if ( leftChunkFirst > static_cast<std::size_t>(indexMovedTo) ) // Rotate combined chunk leftwards to final position
                                    {
                                        auto chunkStart = std::next(ref.begin(), static_cast<std::ptrdiff_t>(leftChunkFirst));
                                        auto chunkEnd = std::next(chunkStart, static_cast<std::ptrdiff_t>(countValidIndexes));
                                        std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(indexMovedTo)), chunkStart, chunkEnd);
                                        mirrorRotationToSelection(sel, indexMovedTo, leftChunkFirst, leftChunkFirst+countValidIndexes);
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
            });
        }
    };

    template <class Data, class User> class Tracked;

    template <class T, class User>
    class Edit : private EditRoot<T, User, Edit<T, User>>, public edit_members<EditRoot<T, User, Edit<T, User>>, typename decltype(defaultIndexType<T>())::type, T, T>
    {
        Edit(T & t, User & user) :
            EditRoot<T, User, Edit<T, User>>(*this, t, user),
            edit_members<EditRoot<T, User, Edit<T, User>>, typename decltype(defaultIndexType<T>())::type, T, T>{(EditRoot<T, User, Edit<T, User>>&)*this, std::tuple{}} {}

        friend class Tracked<T, User>;
        friend struct EditRoot<T, User, Edit<T, User>>;
        friend class Event;

    public:
        using default_index_type = typename EditRoot<T, User, Edit<T, User>>::default_index_type;
    };

    template <class Data, class User>
    class Tracked : Data
    {
        static constexpr std::uint64_t flagElidedRedos    = 0x8000000000000000ull;
        static constexpr std::uint64_t maskElidedRedoSize = 0x7FFFFFFFFFFFFFFFull; // The total size of this elided redo branch, including sub-branches
        
        using edit_type = Edit<Data, User>;
        Edit<Data, User> editable;

        std::vector<std::uint64_t> actionFirstEvent; // Index of the first data-change event for the given action
        int actionReferenceCount = 0; // Referencing counting for the current action, new actions can only be created when the old action is closed
        std::uint64_t redoCount = 0; // How many undos have occured since the last user-action/how many redos are available
        std::uint64_t redoSize = 0; // The size of the range including the redoable actions (includes elided redos)

    protected:

        struct Editor // If moved-from, or maybe created with createSharedAction the action shouldn't be auto-submitted, allowing for brushing
        {             // However... it should be ensured that there is only ever one such action
                      // Maybe it's an optional contained within this class and a derefable wrapper for that including a bool is returned?
            Tracked<Data, User>* parent = nullptr;
            Editor(Tracked<Data, User>* parent) : parent(parent) {
                if ( parent != nullptr )
                    parent->actionReferenceCount++;
            }
            Editor(Editor && other) noexcept { std::swap(parent, other.parent); }
            ~Editor() {
                if ( parent != nullptr )
                    parent->actionReferenceCount--;
            }
            edit_type & operator*() noexcept { return parent->editable; }
            edit_type* operator->() noexcept { return &parent->editable; }
        };

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
            return Editor{this};
        };

        static inline Editor root {nullptr};

        template <class Input>
        using MakePath = PathTaggedIndexes<typename Input::Path, typename Input::Indexes>;

        #define PATH(...) MakePath<decltype(__VA_ARGS__)>

    public:
        const Edit<Data, User> & view;
        Tracked(User* user) : editable(*this, *user), view(editable) {}
        Tracked(User & user) : editable(*this, user), view(editable) {}

        const Data & read = (const Data &)*this;
        EditRoot<Data, User, edit_type> & history = static_cast<EditRoot<Data, User, edit_type> &>(editable);
        constexpr const Data & operator*() const noexcept { return read; }
        constexpr const Data* operator->() const noexcept { return this; }
        auto operator()() { 
            if ( actionReferenceCount == 0 )
            {
                if ( redoCount > 0 )
                    elideRedos();

                actionFirstEvent.push_back(editable.eventOffsets.size());
            }
            return Editor{this};
        };

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
            
            auto & editRoot = (EditRoot<Data, User, edit_type> &)editable;
            for ( std::int64_t i=nextActionStart-1; i>=actionEventStart; i-- )
            {
                editRoot.undoEvent(std::uint64_t(i));
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
            
            auto & editRoot = (EditRoot<Data, User, edit_type> &)editable;
            for ( auto i=actionEventStart; i<nextActionStart; i++ )
                editRoot.redoEvent(i);

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

        template <class Value, class Member>
        void printValue(std::size_t & offset) const
        {
            if constexpr ( RareTs::is_reflected_v<Value> )
            {
                RareTs::Members<Value>::forEach([&](auto member) {
                    printValue<std::remove_cvref_t<typename decltype(member)::type>, decltype(member)>(offset);
                    std::cout << " ";
                });
            }
            else if constexpr ( std::is_same_v<Value, std::string> )
            {
                auto stringSize = (index_type_t<typename edit_type::default_index_type, Member> &)editable.events[offset];
                offset += sizeof(stringSize);
                std::string string((const char*)&editable.events[offset], stringSize);
                offset += stringSize;
                std::cout << "\"" << string << "\"";
            }
            else if constexpr ( RareTs::is_static_array_v<Value> )
            {
                constexpr auto size = static_cast<index_type_t<typename edit_type::default_index_type, Member>>(RareTs::static_array_size_v<Value>);
                offset += sizeof(size);
                std::cout << "[" << static_cast<std::size_t>(size) << "]";
                for ( std::remove_cvref_t<decltype(size)> i=0; i<size; ++i )
                {
                    printValue<std::remove_cvref_t<decltype(std::declval<Value>()[0])>, Member>(offset);
                    std::cout << " ";
                }
            }
            else if constexpr ( RareTs::is_iterable_v<Value> )
            {
                auto size = (index_type_t<typename edit_type::default_index_type, Member> &)editable.events[offset];
                offset += sizeof(size);
                std::cout << "[" << static_cast<std::size_t>(size) << "]";
                for ( decltype(size) i=0; i<size; ++i )
                {
                    printValue<std::remove_cvref_t<decltype(std::declval<Value>()[0])>, Member>(offset);
                    std::cout << " ";
                }
            }
            else
            {
                Value value = (Value &)editable.events[offset];
                std::cout << Json::out(value);
                offset += sizeof(Value);
            }
        }

        template <class U, class Member = void>
        void printEvent(std::size_t & offset, std::uint8_t op) const
        {
            using base_index_type = index_type_t<typename edit_type::default_index_type, Member>;
            std::uint8_t value = editable.events[offset];
            ++offset;
            
            auto printOp = [&]<class type, class member_type>(RareTs::type_tags<type, member_type>){
                using index_type = index_type_t<typename edit_type::default_index_type, member_type>;
                switch ( Op(op) )
                {
                    case Op::Reset:
                    {
                        auto prevValue = editable.template readValue<type, member_type>(offset);
                        std::cout << ".reset() // " << Json::out(prevValue);
                    }
                    break;
                    case Op::Reserve:
                    {
                        auto size = editable.template readIndex<index_type>(offset);
                        std::cout << ".reserve(" << static_cast<std::size_t>(size) << ")";
                    }
                    break;
                    case Op::Trim:
                    {
                        std::cout << ".trim()";
                    }
                    break;
                    case Op::Assign:
                    {
                        using element = RareTs::element_type_t<std::remove_cvref_t<type>>;
                        if constexpr ( !std::is_void_v<element> )
                        {
                            auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                            auto value = editable.template readValue<element, member_type>(offset);
                            auto prevValue = editable.template readValue<type, member_type>(offset);
                            std::cout << ".assign(" << size << ", " << Json::out(value) << ") // " << Json::out(prevValue);
                        }
                    }
                    break;
                    case Op::AssignDefault:
                    {
                        auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                        auto prevValue = editable.template readValue<type, member_type>(offset);
                        std::cout << ".assign(" << size << ", {}) // " << Json::out(prevValue);
                    }
                    break;
                    case Op::ClearSelections:
                    {
                        std::cout << ".clearSelections(";
                        auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                        std::cout << size << ", {";
                        for ( std::size_t i=0; i<size; ++i )
                        {
                            if ( i > 0 )
                                std::cout << ", ";
                        
                            auto index = editable.template readIndex<index_type>(offset);
                            std::cout << static_cast<std::size_t>(index);
                        }
                        std::cout << "})";
                    }
                    break;
                    case Op::SelectAll:
                    {
                        std::cout << ".selectAll(";
                        auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                        std::cout << size << ", {";
                        for ( std::size_t i=0; i<size; ++i )
                        {
                            if ( i > 0 )
                                std::cout << ", ";
                        
                            auto index = editable.template readIndex<index_type>(offset);
                            std::cout << static_cast<std::size_t>(index);
                        }
                        std::cout << "})";
                    }
                    break;
                    case Op::Select:
                    {
                        std::cout << ".select(";
                        auto index = editable.template readIndex<index_type>(offset);
                        std::cout << static_cast<std::size_t>(index);
                        std::cout << ")";
                    }
                    break;
                    case Op::SelectN:
                    {
                        std::cout << ".selectN(";
                        auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                        std::cout << size << ", {";
                        for ( std::size_t i=0; i<size; ++i )
                        {
                            if ( i > 0 )
                                std::cout << ", ";

                            auto index = editable.template readIndex<index_type>(offset);
                            std::cout << static_cast<std::size_t>(index);
                        }
                        std::cout << "})";
                    }
                    break;
                    case Op::Deselect:
                    {
                        std::cout << ".deselect(";
                        printValue<std::size_t, member_type>(offset);
                        std::cout << ")";
                    }
                    break;
                    case Op::DeselectN:
                    {
                        std::cout << ".deselectN(";
                        auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                        std::cout << static_cast<std::size_t>(size) << ", {";
                        for ( std::size_t i=0; i<size; ++i )
                        {
                            if ( i > 0 )
                                std::cout << ", ";
                        
                            auto index = editable.template readIndex<index_type>(offset);
                            std::cout << static_cast<std::size_t>(index);
                        }
                        std::cout << "})";
                    }
                    break;
                    case Op::ToggleSelection:
                    {
                        std::cout << ".toggleSel(";
                        printValue<std::size_t, member_type>(offset);
                        std::cout << ")";
                    }
                    break;
                    case Op::ToggleSelectionN:
                    {
                        std::cout << ".toggleSelN(";
                        auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                        std::cout << size << ", {";
                        for ( std::size_t i=0; i<size; ++i )
                        {
                            if ( i > 0 )
                                std::cout << ", ";
                        
                            auto index = editable.template readIndex<index_type>(offset);
                            std::cout << static_cast<std::size_t>(index);
                        }
                        std::cout << "})";
                    }
                    break;
                    case Op::SortSelections:
                    {
                        std::cout << ".sortSel(";
                        auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                        std::cout << size << ", {";
                        for ( std::size_t i=0; i<size; ++i )
                        {
                            if ( i > 0 )
                                std::cout << ", ";
                        
                            auto index = editable.template readIndex<index_type>(offset);
                            std::cout << static_cast<std::size_t>(index);
                        }
                        std::cout << "})";
                    }
                    break;
                    case Op::SortSelectionsDesc:
                    {
                        std::cout << ".sortSelDesc(";
                        auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                        std::cout << size << ", {";
                        for ( std::size_t i=0; i<size; ++i )
                        {
                            if ( i > 0 )
                                std::cout << ", ";
                        
                            auto index = editable.template readIndex<index_type>(offset);
                            std::cout << static_cast<std::size_t>(index);
                        }
                        std::cout << "})";
                    }
                    break;
                    case Op::Set:
                    {
                        auto newValue = editable.template readValue<type, member_type>(offset);
                        auto prevValue = editable.template readValue<type, member_type>(offset);
                        std::cout << "{" << Json::out(prevValue) << "} = " << Json::out(newValue);
                    }
                    break;
                    case Op::SetN:
                    {
                        std::cout << ".setN(";
                        using element = RareTs::element_type_t<std::remove_cvref_t<type>>;
                        if constexpr ( !std::is_void_v<element> )
                        {
                            std::size_t size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                            std::cout << "{" << size << ", [";
                            for ( std::size_t i=0; i<size; ++i )
                            {
                                if ( i > 0 )
                                    std::cout << ", " << static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                                else
                                    std::cout << static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                            }
                            auto value = editable.template readValue<element, member_type>(offset);
                            std::cout << "], " << Json::out(value) << "}, {";
                            for ( std::size_t i=0; i<size; ++i )
                            {
                                if ( i > 0 )
                                    std::cout << ", " << Json::out(editable.template readValue<element, member_type>(offset));
                                else
                                    std::cout << Json::out(editable.template readValue<element, member_type>(offset));
                            }
                        }
                        std::cout << ")";
                    }
                    break;
                    case Op::SetL:
                    {
                        auto newValue = editable.template readValue<type, member_type>(offset);
                        std::cout << " = " << Json::out(newValue);
                    }
                    break;
                    case Op::Append:
                    {
                        std::cout << ".append(";
                        if constexpr ( RareTs::is_specialization_v<type, std::vector> )
                            printValue<std::remove_cvref_t<decltype(std::declval<type>()[0])>, member_type>(offset);
                        std::cout << ")";
                    }
                    break;
                    case Op::AppendN:
                    {
                        std::cout << ".appendN(";
                        if constexpr ( RareTs::is_specialization_v<type, std::vector> )
                        {
                            using element = RareTs::element_type_t<std::remove_cvref_t<type>>;
                            std::size_t size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                            std::cout << size << ", [";
                            for ( std::size_t i=0; i<size; ++i )
                            {
                                if ( i > 0 )
                                    std::cout << ", " << Json::out(editable.template readValue<element, member_type>(offset));
                                else
                                    std::cout << Json::out(editable.template readValue<element, member_type>(offset));
                            }
                        }
                        std::cout << ")";
                    }
                    break;
                    case Op::Insert:
                    {
                        std::cout << ".insert(" << static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                        if constexpr ( RareTs::is_specialization_v<type, std::vector> )
                        {
                            std::cout << ", {";
                            printValue<std::remove_cvref_t<decltype(std::declval<type>()[0])>, member_type>(offset);
                            std::cout << "}";
                        }
                        std::cout << ")";
                    }
                    break;
                    case Op::InsertN:
                    {
                        std::cout << ".insertN(";
                        if constexpr ( RareTs::is_specialization_v<type, std::vector> )
                        {
                            using element = RareTs::element_type_t<std::remove_cvref_t<type>>;
                            std::size_t size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                            std::size_t index = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                            std::cout << index << ", " << size << ", [";
                            for ( std::size_t i=0; i<size; ++i )
                            {
                                if ( i > 0 )
                                    std::cout << ", " << Json::out(editable.template readValue<element, member_type>(offset));
                                else
                                    std::cout << Json::out(editable.template readValue<element, member_type>(offset));
                            }
                            std::cout << "]";
                        }
                        std::cout << ")";
                    }
                    break;
                    case Op::Remove:
                    {
                        if constexpr ( RareTs::is_specialization_v<type, std::vector> )
                        {
                            std::cout << ".remove(" << static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                            std::cout << ") // {";
                            printValue<std::remove_cvref_t<decltype(std::declval<type>()[0])>, member_type>(offset);
                            std::cout << "}";
                        }
                    }
                    break;
                    case Op::RemoveN:
                    {
                        if constexpr ( RareTs::is_specialization_v<type, std::vector> )
                        {
                            std::size_t count = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                            std::cout << ".removeN(" << count << ", [";
                            using element = RareTs::element_type_t<std::remove_cvref_t<type>>;
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( i > 0 )
                                    std::cout << ", " << static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                                else
                                    std::cout << static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                            }
                            std::cout << "] // [";
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( i > 0 )
                                    std::cout << ", " << Json::out(editable.template readValue<element, member_type>(offset));
                                else
                                    std::cout << Json::out(editable.template readValue<element, member_type>(offset));
                            }
                            std::cout << "]";
                        }
                    }
                    break;
                    case Op::RemoveL:
                    {
                        if constexpr ( RareTs::is_specialization_v<type, std::vector> )
                        {
                            std::size_t count = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                            std::cout << ".removeL() //" << count << ", [";
                            using element = RareTs::element_type_t<std::remove_cvref_t<type>>;
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( i > 0 )
                                    std::cout << ", " << static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                                else
                                    std::cout << static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                            }
                            std::cout << "], [";
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( i > 0 )
                                    std::cout << ", " << Json::out(editable.template readValue<element, member_type>(offset));
                                else
                                    std::cout << Json::out(editable.template readValue<element, member_type>(offset));
                            }
                            std::cout << "]";
                        }
                    }
                    break;
                    case Op::Sort:
                    {
                        std::size_t count = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                        std::cout << ".sort() // " << count << ", [";
                        for ( std::size_t i=0; i<count; ++i )
                        {
                            if ( i > 0 )
                                std::cout << ", " << static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                            else
                                std::cout << static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                        }
                        std::cout << "]";
                    }
                    break;
                    case Op::SortDesc:
                    {
                        std::size_t count = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                        std::cout << ".sortDesc() // " << count << ", [";
                        for ( std::size_t i=0; i<count; ++i )
                        {
                            if ( i > 0 )
                                std::cout << ", " << static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                            else
                                std::cout << static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                        }
                        std::cout << "]";
                    }
                    break;
                    case Op::MoveUp:
                    {
                        std::cout << ".moveUp(" << static_cast<std::size_t>(editable.template readIndex<index_type>(offset)) << ")";
                    }
                    break;
                    case Op::MoveUpN:
                    {
                        auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                        std::cout << ".moveUpN(" << size << ", {";
                        for ( std::size_t i=0; i<size; ++i )
                            std::cout << (i==0 ? "" : ", ") << static_cast<std::size_t>(editable.template readIndex<index_type>(offset));

                        std::cout << "})";
                    }
                    break;
                    case Op::MoveUpL:
                    {
                        std::cout << ".moveUpSelections()";
                    }
                    break;
                    case Op::MoveTop:
                    {
                        std::cout << ".moveTop(" << static_cast<std::size_t>(editable.template readIndex<index_type>(offset)) << ")";
                    }
                    break;
                    case Op::MoveTopN:
                    {
                        auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                        std::cout << ".moveTopN(" << size << ", {";
                        for ( std::size_t i=0; i<size; ++i )
                            std::cout << (i==0 ? "" : ", ") << static_cast<std::size_t>(editable.template readIndex<index_type>(offset));

                        std::cout << "})";
                    }
                    break;
                    case Op::MoveTopL:
                    {
                        std::cout << ".moveSelectionsTop()";
                    }
                    break;
                    case Op::MoveDown:
                    {
                        std::cout << ".moveDown(" << static_cast<std::size_t>(editable.template readIndex<index_type>(offset)) << ")";
                    }
                    break;
                    case Op::MoveDownN:
                    {
                        auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                        std::cout << ".moveDownN(" << size << ", {";
                        for ( std::size_t i=0; i<size; ++i )
                            std::cout << (i==0 ? "" : ", ") << static_cast<std::size_t>(editable.template readIndex<index_type>(offset));

                        std::cout << "})";
                    }
                    break;
                    case Op::MoveDownL:
                    {
                        std::cout << ".moveSelectionsDown()";
                    }
                    break;
                    case Op::MoveBottom:
                    {
                        std::cout << ".moveBottom(" << static_cast<std::size_t>(editable.template readIndex<index_type>(offset)) << ")";
                    }
                    break;
                    case Op::MoveBottomN:
                    {
                        auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                        std::cout << ".moveBottomN(" << size << ", {";
                        for ( std::size_t i=0; i<size; ++i )
                            std::cout << (i==0 ? "" : ", ") << static_cast<std::size_t>(editable.template readIndex<index_type>(offset));

                        std::cout << "})";
                    }
                    break;
                    case Op::MoveBottomL:
                    {
                        std::cout << ".moveSelectionsBottom()";
                    }
                    break;
                    case Op::MoveTo:
                    {
                        std::cout << ".moveTo(" << static_cast<std::size_t>(editable.template readIndex<index_type>(offset)) << ", "
                            << static_cast<std::size_t>(editable.template readIndex<index_type>(offset)) << ")";
                    }
                    break;
                    case Op::MoveToN:
                    {
                        auto target = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                        auto size = static_cast<std::size_t>(editable.template readIndex<index_type>(offset));
                        std::cout << ".moveToN(" << target << ", " << size << ", {";
                        for ( std::size_t i=0; i<size; ++i )
                            std::cout << (i==0 ? "" : ", ") << static_cast<std::size_t>(editable.template readIndex<index_type>(offset));

                        std::cout << "})";
                    }
                    break;
                    case Op::MoveToL:
                    {
                        std::cout << ".moveSelectionsTo(" << static_cast<std::size_t>(editable.template readIndex<index_type>(offset)) << ")";
                    }
                    break;
                }
            };
            switch ( PathOp(value & std::uint8_t(PathOp::HighBits)) )
            {
                case PathOp::SelBranch:
                    if constexpr ( RareTs::is_static_array_v<U> || RareTs::is_specialization_v<U, std::vector> )
                    {
                        std::cout << "[{sel}]";
                        using element_type = RareTs::element_type_t<std::remove_cvref_t<U>>;
                        constexpr bool isLeaf = !RareTs::is_reflected_v<element_type> && std::is_void_v<RareTs::element_type_t<element_type>>;
                        if constexpr ( isLeaf )
                            printOp(RareTs::type_tags<element_type, Member>{});
                        else
                            printEvent<std::remove_cvref_t<decltype(std::declval<U>()[0])>, Member>(offset, op);
                    }
                    break;
                case PathOp::LeafSelBranch:
                    if constexpr ( RareTs::is_static_array_v<U> || RareTs::is_specialization_v<U, std::vector> )
                    {
                        std::cout << "[{sel}]";
                        using element_type = RareTs::element_type_t<std::remove_cvref_t<U>>;
                        printOp(RareTs::type_tags<element_type, Member>{});
                    }
                    break;
                case PathOp::Branch:
                {
                    if constexpr ( RareTs::is_reflected_v<U> ) // Branch to field
                    {
                        RareTs::Members<U>::at(std::size_t(value & std::uint8_t(PathOp::LowBits)), [&](auto member) {
                            std::cout << "." << member.name;
                            printEvent<std::remove_cvref_t<typename decltype(member)::type>, decltype(member)>(offset, op);
                        });
                    }
                    else if constexpr ( RareTs::is_static_array_v<U> || RareTs::is_specialization_v<U, std::vector> ) // Branch to index
                    {
                        base_index_type index {};
                        if constexpr ( std::is_same_v<uint6_t, base_index_type> )
                            index = static_cast<base_index_type>(value & std::uint8_t(PathOp::LowBits));
                        else
                            index = editable.template readIndex<base_index_type>(offset);

                        std::cout << '[' << static_cast<std::size_t>(index) << ']';
                        printEvent<std::remove_cvref_t<decltype(std::declval<U>()[0])>, Member>(offset, op);
                    }
                }
                break;
                case PathOp::LeafBranch:
                {
                    if constexpr ( RareTs::is_reflected_v<U> ) // Op on field
                    {
                        RareTs::Members<U>::at(std::size_t(value & std::uint8_t(PathOp::LowBits)), [&](auto member) {
                            std::cout << "." << member.name;
                            printOp(RareTs::type_tags<std::remove_cvref_t<typename decltype(member)::type>, RareTs::remove_cvref_t<decltype(member)>>{});
                        });
                    }
                    else if constexpr ( RareTs::is_static_array_v<U> ) // Op on index
                    {
                        base_index_type index {};
                        if constexpr ( std::is_same_v<uint6_t, base_index_type> )
                            index = static_cast<base_index_type>(value & std::uint8_t(PathOp::LowBits));
                        else
                            index = editable.template readIndex<base_index_type>(offset);

                        std::cout << "[" << static_cast<std::size_t>(index) << "]";
                        printOp(RareTs::type_tags<RareTs::element_type_t<std::remove_cvref_t<U>>, Member>{});
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
            printEvent<U, Member>(offset, op);
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
                    printEvent<typename Edit<Data, User>::type>(eventOffset);
                    std::cout << '\n';
                }
                std::cout << '\n';
            }
        }

        REFLECT(Tracked, flagElidedRedos, maskElidedRedoSize, editable, actionFirstEvent, actionReferenceCount, redoCount, redoSize)
    };
}

#endif