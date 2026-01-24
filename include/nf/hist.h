// MIT License, Copyright (c) 2025-2026 Justin F https://github.com/TheNitesWhoSay/RareCpp/blob/master/LICENSE
#ifndef NFHIST_H
#define NFHIST_H
#include <algorithm>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iosfwd>
#include <numeric>
#include <optional>
#include <span>
#include <sstream>
#include <tuple>
#include <type_traits>
#include <unordered_set>
#include <vector>
#ifndef REFLECT_H // This check, while normally redundant to have here, helps the file work on godbolt
#include "../rarecpp/reflect.h"
#endif
#ifndef JSON_H // This check, while normally redundant to have here, helps the file work on godbolt
#include "../rarecpp/json.h"
#endif

// Nf C++: data history library
namespace nf_hist
{
    using RareTs::type_tags;

    template <typename Size_type> struct index_size_type { using type = Size_type; };

    template <typename Size_type> inline constexpr index_size_type<Size_type> index_size;

    template <typename Attached_type> struct attach_data_type { using type = Attached_type; };

    template <typename Attached_type> inline constexpr attach_data_type<Attached_type> attach_data;

    inline constexpr std::nullopt_t ref_null_opt = std::nullopt;

    template <typename T>
    constexpr auto def_index_type()
    {
        if constexpr ( RareTs::Notes<T>::template hasNote<nf_hist::index_size_type>() )
            return RareTs::Notes<T>::template getNote<nf_hist::index_size_type>();
        else
            return std::type_identity<std::size_t>{};
    }

    template <typename T> struct is_array_member : std::false_type {};
    template <typename T, std::size_t I> struct is_array_member<RareTs::Member<T, I>> :
        std::bool_constant<std::is_array_v<typename RareTs::Member<T, I>::type>> {};
    template <typename T> inline constexpr bool is_array_member_v = is_array_member<T>::value;

    template <typename T>
    inline constexpr std::size_t reflected_member_count() {
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
        template <typename U>
        struct It {
            U* obj;
            std::size_t i;
            constexpr void operator++() { ++i; }
            constexpr decltype(auto) operator*() const { return (*obj)[i]; }
            constexpr bool operator!=(It other) { return i != other.i; }
        };
        constexpr auto begin() { return It{this, 0}; }
        constexpr auto end() { return It{this, Extent}; }
        constexpr auto begin() const { return It{this, 0}; }
        constexpr auto end() const { return It{this, Extent}; }

        constexpr T* flat_begin() { return data; }
        constexpr T* flat_end() { return data+size; }
        constexpr const T* flat_begin() const { return data; }
        constexpr const T* flat_end() const { return data+size; }

        constexpr void operator=(const flat_mdspan & other) {
            std::copy(other.flat_begin(), other.flat_end(), flat_begin());
        }
        constexpr void clear() { std::fill(flat_begin(), flat_end(), T{}); }
        constexpr void fill(const T & value) { std::fill(flat_begin(), flat_end(), value); }
        constexpr void swap(flat_mdspan && other) {
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
        using deref = std::remove_reference_t<T>;
        using elem = std::remove_all_extents_t<deref>;
        constexpr auto rank = std::rank_v<deref>;
        return [&]<std::size_t ... Is>(std::index_sequence<Is...>) {
            static_assert(sizeof(deref) == sizeof(elem)*(std::extent_v<deref, Is> * ... * 1), "array passed to as_1d was not contiguous!");
            return flat_mdspan<elem, std::extent_v<deref, Is>...>(array_header(array));
        }(std::make_index_sequence<rank>());
    }

    inline void write_vec_bool_data(std::vector<std::uint8_t> & data, const std::vector<bool> & vec_bool) // Does not include the size
    {
        constexpr std::uint8_t zero = 0;
        std::size_t size = vec_bool.size();
        std::size_t unpadded_byte_count = size/8;
        std::size_t byte_count = (size+7)/8;

        std::size_t byte_start = data.size();
        data.insert(data.end(), byte_count, zero);
        for ( std::size_t i=0; i<unpadded_byte_count; ++i )
        {
            data[byte_start + i] = static_cast<std::uint8_t>(
                int(vec_bool[i*8]) +
                (int(vec_bool[i*8+1]) << 1) +
                (int(vec_bool[i*8+2]) << 2) +
                (int(vec_bool[i*8+3]) << 3) +
                (int(vec_bool[i*8+4]) << 4) +
                (int(vec_bool[i*8+5]) << 5) +
                (int(vec_bool[i*8+6]) << 6) +
                (int(vec_bool[i*8+7]) << 7));
        }
        if ( unpadded_byte_count < byte_count )
        {
            std::size_t bit_start = unpadded_byte_count*8; 
            std::size_t bit_count = size%8;
            int last_byte = 0;
            for ( std::size_t i=0; i<bit_count; ++i )
                last_byte += (int(vec_bool[bit_start+i]) << i);
        
            data[byte_start+unpadded_byte_count] = static_cast<std::uint8_t>(last_byte);
        }
    }

    inline void read_vec_bool_data(const std::vector<std::uint8_t> & data, std::size_t & offset, std::size_t bit_size, std::vector<bool> & bitset) // Does not include the size
    {
        bitset = std::vector<bool>(bit_size, false);
        std::size_t unpadded_readable_byte_count = bit_size/8;
        std::size_t readable_byte_count = (bit_size+7)/8;
        for ( std::size_t i=0; i<unpadded_readable_byte_count; ++i )
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
        if ( readable_byte_count > unpadded_readable_byte_count )
        {
            std::uint8_t last_byte = data[offset+unpadded_readable_byte_count];
            std::uint8_t mask = 0x1;
            for ( std::size_t bit = unpadded_readable_byte_count*8; bit<bit_size; ++bit )
            {
                bitset[bit] = (last_byte & mask) > 0;
                mask <<= 1;
            }
        }
        offset += (bit_size+7)/8;
    }

    template <std::size_t N, typename Size = std::uint32_t, bool Include_size = true>
    void write_bitset(std::vector<std::uint8_t> & data, const std::bitset<N> & bitset)
    {
        constexpr std::uint8_t zero = 0;
        constexpr std::size_t unpadded_byte_count = N/8;
        constexpr std::size_t byte_count = (N+7)/8;
        constexpr Size size = static_cast<Size>(N);
        if constexpr ( Include_size )
            data.insert(data.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));

        std::size_t byte_start = data.size();
        data.insert(data.end(), byte_count, zero);
        for ( std::size_t i=0; i<unpadded_byte_count; ++i )
        {
            data[byte_start + i] = static_cast<std::uint8_t>(
                int(bitset[i*8]) +
                (int(bitset[i*8+1]) << 1) +
                (int(bitset[i*8+2]) << 2) +
                (int(bitset[i*8+3]) << 3) +
                (int(bitset[i*8+4]) << 4) +
                (int(bitset[i*8+5]) << 5) +
                (int(bitset[i*8+6]) << 6) +
                (int(bitset[i*8+7]) << 7));
        }
        if constexpr ( unpadded_byte_count < byte_count )
        {
            constexpr std::size_t bit_start = unpadded_byte_count*8; 
            constexpr std::size_t bit_count = N%8;
            int last_byte = 0;
            for ( std::size_t i=0; i<bit_count; ++i )
                last_byte += (int(bitset[bit_start+i]) << i);
        
            data[byte_start+unpadded_byte_count] = static_cast<std::uint8_t>(last_byte);
        }
    }

    template <std::size_t N, typename Size = std::uint32_t>
    void read_bitset(const std::vector<std::uint8_t> & data, std::size_t & offset, std::bitset<N> & bitset)
    {
        bitset.reset();
        std::size_t size = static_cast<std::size_t>((Size &)data[offset]);
        offset += sizeof(Size);
        std::size_t readable_bit_count = std::min(N, size);
        std::size_t unpadded_readable_byte_count = readable_bit_count/8;
        std::size_t readable_byte_count = (readable_bit_count+7)/8;
        for ( std::size_t i=0; i<unpadded_readable_byte_count; ++i )
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
        if ( readable_byte_count > unpadded_readable_byte_count )
        {
            std::uint8_t last_byte = data[offset+unpadded_readable_byte_count];
            std::uint8_t mask = 0x1;
            for ( std::size_t bit = unpadded_readable_byte_count*8; bit<readable_bit_count; ++bit )
            {
                bitset[bit] = (last_byte & mask) > 0;
                mask <<= 1;
            }
        }
        offset += (size+7)/8;
    }

    template <typename T>
    void write_selection_vector(std::vector<std::uint8_t> & data, const std::vector<T> & integral_vec)
    {
        T size = (T)integral_vec.size();
        data.insert(data.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));
        for ( auto & ref : integral_vec )
        {
            T val = (T)ref;
            data.insert(data.end(), reinterpret_cast<const std::uint8_t*>(&val), reinterpret_cast<const std::uint8_t*>(&val)+sizeof(val));
        }
    }

    template <typename T>
    bool read_selection_vector(const std::vector<std::uint8_t> & data, std::size_t & offset, std::vector<T> & integral_vec)
    {
        T size = *(reinterpret_cast<const T*>(&data[offset]));
        offset += sizeof(size);
        integral_vec.clear();
        integral_vec.reserve(size);
        for ( T i=0; i<size; ++i )
        {
            integral_vec.insert(integral_vec.end(), T(*(reinterpret_cast<const T*>(&data[offset]))));
            offset += sizeof(T);
        }
        return size > 0;
    }

    template <typename T>
    void write_selections(std::vector<std::uint8_t> & data, const T & t)
    {
        if constexpr ( !RareTs::is_static_array_v<std::remove_cvref_t<T>> && RareTs::is_specialization_v<T, std::vector> )
            write_selection_vector(data, t);
    }

    template <typename T>
    bool read_selections(const std::vector<std::uint8_t> & data, std::size_t & offset, T & t)
    {
        if constexpr ( requires{read_selection_vector(data, offset, t);} )
            return read_selection_vector(data, offset, t);
        else
            return false;
    }

    namespace u8bool
    {
        inline constexpr std::uint8_t false_ = 0;
        inline constexpr std::uint8_t true_ = 1;

        inline bool read(const std::vector<std::uint8_t> & data, std::size_t & offset)
        {
            return data[offset++] != 0;
        }

        inline void write(std::vector<std::uint8_t> & data, bool value)
        {
            std::uint8_t write = value ? 1 : 0;
            data.insert(data.end(), reinterpret_cast<const std::uint8_t*>(&write), reinterpret_cast<const std::uint8_t*>(&write)+sizeof(write));
        }

        inline void write_true(std::vector<std::uint8_t> & data)
        {
            data.insert(data.end(), &true_, &true_+sizeof(true_));
        }

        inline void write_false(std::vector<std::uint8_t> & data)
        {
            data.insert(data.end(), &false_, &false_+sizeof(false_));
        }
    };

    /// A 6-bit unsigned int [0, 64); as the high bits are available can be combined with path bits
    struct uint6_t
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
        constexpr uint6_t & operator++() { ++value; return *this; }
        constexpr uint6_t & operator--() { --value; return *this; }
        constexpr uint6_t & operator+=(std::uint8_t val) { value += val; return *this; }
        constexpr uint6_t & operator-=(std::uint8_t val) { value -= val; return *this; }

        inline friend std::ostream & operator<<(std::ostream & os, const uint6_t & num) { os << int(num.value); return os; }
    };
    static_assert(sizeof(uint6_t) == sizeof(uint8_t), "Unexpected uint6_t size");

    inline void read_bool_byte(const std::vector<std::uint8_t> & data, std::size_t & offset, bool & value)
    {
        std::uint8_t val = (std::uint8_t &)data[offset];
        offset += sizeof(val);
        value = val == 0 ? false : true;
    }

    template <typename T>
    void clear_sel(T & t)
    {
        RareTs::clear(t);
    }

    template <bool Desc = false, typename Primary, typename Secondary>
    constexpr void parallel_quick_sort(Primary & primary, Secondary & secondary, std::ptrdiff_t l, std::ptrdiff_t r) noexcept {
        auto parallel_swap = [&](auto i1, auto i2) {
            std::swap(primary[static_cast<std::size_t>(i1)], primary[static_cast<std::size_t>(i2)]);
            std::swap(secondary[static_cast<std::size_t>(i1)], secondary[static_cast<std::size_t>(i2)]);
        };
        if ( l < r ) {
            std::ptrdiff_t i = l+(r-l)/2; // Take median index as the pivot, swap pivot with last element
            parallel_swap(i, r);
            auto pivot_value = primary[static_cast<std::size_t>(r)];
            i = l;
            for ( std::ptrdiff_t j=l; j<r; ++j ) {
                if constexpr ( Desc )
                {
                    if ( pivot_value < primary[static_cast<std::size_t>(j)] ) { // Swap elements greater than pivot to the earliest indexes
                        parallel_swap(i, j);
                        ++i;
                    }
                }
                else
                {
                    if ( primary[static_cast<std::size_t>(j)] < pivot_value ) { // Swap elements less than pivot to the earliest indexes
                        parallel_swap(i, j);
                        ++i;
                    }
                }
            }
            parallel_swap(i, r);
            parallel_quick_sort<Desc>(primary, secondary, l, i-1); // Sort left sub-array (elements < pivot_value)
            parallel_quick_sort<Desc>(primary, secondary, i+1, r); // Sort right sub-array (elements > pivot_value)
        }
    }

    /// Sorts the items via quick-sort and returns a vector of source indexes that can be used to undo the action
    /// @param items the items to sort
    /// @return a vector of source indexes
    template <bool Desc = false, typename I = std::size_t, typename T>
    [[nodiscard]] std::vector<I> tracked_sort(T & items)
    {
        if ( std::size(items) == 0 )
            return std::vector<I> {};
        else
        {
            std::vector<I> indexes(std::size(items), I{0});
            std::iota(indexes.begin(), indexes.end(), static_cast<I>(0));
            parallel_quick_sort<Desc>(items, indexes, 0, std::ptrdiff_t(std::size(items))-1);
            return indexes;
        }
    }

    template <typename I = std::size_t, typename T>
    requires ( requires{std::declval<T>()[0] = std::declval<T>()[0];} )
    void undo_sort(T & items, const std::span<I> & source_indexes)
    {
        std::size_t total = std::size(items);
        if ( total > 0 )
        {
            if ( total != source_indexes.size() )
            {
                throw std::invalid_argument("items.size(" + std::to_string(total) +
                    "!= source_indexes.size(" + std::to_string(source_indexes.size()) + ")");
            }
            std::remove_cvref_t<T> old_items(total);
            for ( std::size_t i=0; i<total; ++i )
                old_items[source_indexes[i]] = items[i];

            std::swap(items, old_items);
        }
    }

    template <typename I = std::size_t, typename T>
    requires ( requires{std::declval<T>()[0] = std::declval<T>()[0];} )
    void redo_sort(T & items, const std::span<I> & source_indexes)
    {
        std::size_t total = std::size(items);
        if ( total > 0 )
        {
            if ( total != source_indexes.size() )
            {
                throw std::invalid_argument("items.size(" + std::to_string(total) +
                    "!= source_indexes.size(" + std::to_string(source_indexes.size()) + ")");
            }
            std::remove_cvref_t<T> new_items(total);
            for ( std::size_t i=0; i<total; ++i )
                new_items[i] = items[source_indexes[i]];

            std::swap(items, new_items);
        }
    }

    /// (aka: route) this combines the path to a particular (sub/)member and any map keys/array indexes
    template <class Keys, class Pathway, class Editor_type>
    struct path_tagged_keys : Keys
    {
        using pathway = Pathway;
        using keys = Keys;
        using editor_type = Editor_type;

        template <std::size_t I> constexpr const auto & index() { return std::get<I>((Keys &)(*this));}
    };

    template <class Input>
    using make_path = path_tagged_keys<typename Input::nf_keys, typename Input::nf_path, typename Input::nf_editor_type>;

    struct rotation
    {
        std::size_t start_index;
        std::size_t middle_index;
        std::size_t end_index;

        static rotation make_reverse(std::size_t start_index, std::size_t middle_index, std::size_t end_index)
        {
            if ( start_index+end_index-middle_index > end_index )
                throw std::exception();
            return rotation{.start_index = start_index, .middle_index = start_index+end_index-middle_index, .end_index = end_index};
        }

        void perform(auto && container)
        {
            auto begin = std::begin(container);
            std::rotate(
                std::next(begin, static_cast<std::ptrdiff_t>(start_index)),
                std::next(begin, static_cast<std::ptrdiff_t>(middle_index)),
                std::next(begin, static_cast<std::ptrdiff_t>(end_index))
            );
        }
    };

    template <class Selection>
    void mirror_swap_to_selection(Selection & selection, std::size_t left_index, std::size_t right_index)
    {
        if constexpr ( !std::is_null_pointer_v<Selection> )
        {
            using index_type = RareTs::element_type_t<std::remove_cvref_t<Selection>>;
            for ( auto & i : selection )
            {
                if ( static_cast<std::size_t>(i) == left_index )
                    i = static_cast<index_type>(right_index);
                else if ( static_cast<std::size_t>(i) == right_index )
                    i = static_cast<index_type>(left_index);
            }
        }
    }

    template <class T>
    void mirror_swap_to(T & collection, std::size_t left_index, std::size_t right_index)
    {
        if constexpr ( !std::is_null_pointer_v<T> )
            std::swap(collection[left_index], collection[right_index]);
    }

    template <class Selection>
    void mirror_rotation_to_selection(Selection & selection, std::size_t first, std::size_t middle, std::size_t last)
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

    template <class Collection>
    void mirror_rotation_to(Collection & collection, auto first, auto middle, auto last)
    {
        if constexpr ( !std::is_null_pointer_v<Collection> )
        {
            std::rotate(
                std::next(collection.begin(), static_cast<std::ptrdiff_t>(first)),
                std::next(collection.begin(), static_cast<std::ptrdiff_t>(middle)),
                std::next(collection.begin(), static_cast<std::ptrdiff_t>(last)));
        }
    }

    
    // The meaning of the lower 6-bits depends on the selection bit (second highest bit)...
    //  branch: identifies a field or array index (for array sizes <= 64) which you'll be branching from
    //      if array/container requires an index larger than 6 bits, then the next sizeof(containerIndex) bytes are the container index
    //  sel_branch: lower 6-bits unused, sel_branch indicates the operation applies over the selection for this container (may branch further down from here)
    //  leaf_branch: same as branch except this also indicates it's the last branch in the sequence
    //  leaf_branch: same as branch except this also indicates it's the last branch in the sequence
    //  leaf_sel_branch: same as sel_branch except this also indicates it's the last branch in the sequence
    //      special case: if this is the first element, it means use the root element */
    enum class path_op : std::uint8_t {
        high_bits       = 0b11000000, //!< the two highest bits of the path_op byte, identifies the type of branch
        low_bits        = 0b00111111, //!< the six lowest bits of the path_op byte, an index or other data associated with the given branch type

        sel_mask        = 0b01000000, //!< mask for checking whether this path_op is branching into a selection
        leaf_mask       = 0b10000000, //!< mask for checking whether this path_op refers to the final leaf in a path

        branch          = 0b00000000, //!< after ANDing with high bits, a path_op with this value can be said to be a regular branch
        sel_branch      = 0b01000000, //!< after ANDing with high bits, a path_op with this value can be said to be a selection branch
        leaf_branch     = 0b10000000, //!< after ANDing with high bits, a path_op with this value can be said to be a leaf branch
        leaf_sel_branch = 0b11000000, //!< after ANDing with high bits, a path_op with this value can be said to be a leaf selection branch
        root_path       = leaf_sel_branch //!< if this is the first path_op in a path, it means the path refers to the root element
    };

    enum class op : std::uint8_t {
        // The first {} is the data required to perform the operation, the --{} is additional data required for fast undos
        init,  //!< {value}
        reset, //!< {} ("value = {};") --{n, value_0, ..., value_n, m, selections}
        reserve, //!< {new_size}
        trim, //!< {} -- // no data is associated with the trim operation
        assign, //!< {new_size, value} --{n, value_0, ..., value_n, m, selections}
        assign_default, //!< {new_size} --{n, value_0, ..., value_n, m, selections}
        clear_selections, //!< --{n, index_0, ..., index_n}
        select_all, //!< {} --{n, index_0, ..., index_n}
        select, //!< {index}
        select_n, //!< {n, index_0, ..., index_n}
        deselect, //!< {index} --{sel_index}
        deselect_n, //!< {n, index_0, ..., index_n} --{sel_index_0, ..., sel_index_n}
        toggle_selection, //!< {index} --{u8_bool_selected, sel_index}
        toggle_selection_n, //!< {n, index_0, ..., index_n} --{bitset_index_was_selected, sel_index_0, ..., sel_index_n}
        sort_selections, //!< {n, index_0_source_index, ..., index_n_source_index}
        sort_selections_desc, //!< {n, index_0_source_index, ..., index_n_source_index}
        set, //!< {value} --{prev_value}
        set_n, //!< {n, index_0, ..., index_n, value} --{value_0, ..., value_n}
        set_l, //!< {value} --{value_0, ..., value_n} // Same as set_n, except the selections make up the indexes
        append, //!< {value}
        append_n, //!< {n, value_0, ..., value_n}
        insert, //!< {index, value}
        insert_n, //!< {n, index, value_0, ..., value_n}
        remove, //!< {index} --{value, u8_bool_was_selected, sel_index}
        remove_n, //!< {n, index_0, ..., index_n} --{value_0, ..., value_n, bitset_index_selected, m, sel_index_0, ..., sel_index_m} where index_0 > ... > index_n
        remove_l, //!< {} --{n, index_0, ..., index_n, value_0, ..., value_n, selections} // Same as remove_n, except the selections make up the indexes
        sort, //!< {n, index_0_source_index, ..., index_n_source_index}
        sort_desc, //!< {n, index_0_source_index, ..., index_n_source_index}
        swap, //!< {index_0, index_1}
        move_up, //!< {index}
        move_up_n, //!< {n, index_0, ..., index_n}
        move_up_l, //!< {} --{selections} // Same as move_up_n, except the selections make up the indexes
        move_top, //!< {index}
        move_top_n, //!< {n, index_0, ... index_n} --{selections}
        move_top_l, //!< {} --{selections} // Same as move_top_n, except the selections make up the indexes
        move_down, //!< {index}
        move_down_n, //!< {n, index_0, ... index_n}
        move_down_l, //!< {} --{selections} // Same as move_down_n, except the selections make up the indexes
        move_bottom, //!< {index}
        move_bottom_n, //!< {n, index_0, ... index_n} --{selections}
        move_bottom_l, //!< {} --{selections} // Same as move_down_n, except the selections make up the indexes
        move_to, //!< {index, target_index}
        move_to_n, //!< {n, target_index, index_0, ..., index_n} --{selections}
        move_to_l //!< {target_index} --{selections} // Same as move_to_n, except the selections make up the indexes
    };

    constexpr bool is_sel_change_op(op operation)
    {
        switch ( operation )
        {
            case op::clear_selections: case op::select_all: case op::select: case op::select_n:
            case op::deselect: case op::deselect_n: case op::toggle_selection: case op::toggle_selection_n:
            case op::sort_selections: case op::sort_selections_desc:
                return true;
            default:
                return false;
        }
    }

    /// Go to the Ith member of the current object
    template <std::size_t I> struct path_member {
        static constexpr std::size_t index = I;
    };
    template <class T> struct is_path_member { static constexpr bool value = false; };
    template <std::size_t I> struct is_path_member<path_member<I>> { static constexpr bool value = true; };
    template <class T> inline constexpr bool is_path_member_v = is_path_member<T>::value;

    /// Use the Ith tuple-index to perform an array access on the current object
    template <std::size_t I> struct path_index {
        static constexpr std::size_t index = I;
    };
    template <class T> struct is_path_index { static constexpr bool value = false; };
    template <std::size_t I> struct is_path_index<path_index<I>> { static constexpr bool value = true; };
    template <class T> inline constexpr bool is_path_index_v = is_path_index<T>::value;

    /// The subsequent operation applies to all selected objects in the collection
    struct path_selections {};
    template <class T> struct is_path_selections { static constexpr bool value = false; };
    template <> struct is_path_selections<path_selections> { static constexpr bool value = true; };
    template <class T> inline constexpr bool is_path_selections_v = is_path_selections<T>::value;

    template <class ... Ts> concept has_path_selections = (std::same_as<path_selections, Ts> || ...);

    template <class Agent, class Default_index_type, class Root_data, class T, class Keys, class ... Pathway>
    class edit_primitive : public Keys
    {
        Agent & agent;

    public:
        using nf_keys = Keys;
        using nf_path = type_tags<Pathway...>;
        using nf_editor_type = typename Agent::editor_type;
        using type = T;

        edit_primitive(Agent & agent, Keys keys) :
            Keys {std::move(keys)}, agent(agent) {}
        template <class U> void operator=(U && value) {
            if constexpr ( has_path_selections<Pathway...> )
                agent.template set_l<Pathway...>(std::forward<U>(value), (Keys &)(*this));
            else
                agent.template set<Pathway...>(std::forward<U>(value), (Keys &)(*this));
        }
        template <class U> void operator +=(U && value) { agent.template plus_eq_<Pathway...>(std::forward<U>(value), (Keys &)(*this)); }
        template <class U> void operator -=(U && value) { agent.template minus_eq_<Pathway...>(std::forward<U>(value), (Keys &)(*this)); }
        template <class U> void operator *=(U && value) { agent.template mult_eq_<Pathway...>(std::forward<U>(value), (Keys &)(*this)); }
        template <class U> void operator /=(U && value) { agent.template div_eq_<Pathway...>(std::forward<U>(value), (Keys &)(*this)); }
        template <class U> void operator %=(U && value) { agent.template mod_eq_<Pathway...>(std::forward<U>(value), (Keys &)(*this)); }
        template <class U> void operator ^=(U && value) { agent.template xor_eq_<Pathway...>(std::forward<U>(value), (Keys &)(*this)); }
        template <class U> void operator &=(U && value) { agent.template and_eq_<Pathway...>(std::forward<U>(value), (Keys &)(*this)); }
        template <class U> void operator |=(U && value) { agent.template or_eq_<Pathway...>(std::forward<U>(value), (Keys &)(*this)); }
    };

    template <class Agent, class Default_index_type, class Root_data, class T, class Keys, std::size_t I, class ... Pathway>
    static constexpr auto get_edit_member();

    template <class Agent, class Default_index_type, class Root_data, class T, class Keys, class ... Pathway> struct edit_member
    {
        template <std::size_t I> using type = typename decltype(get_edit_member<Agent, Default_index_type, Root_data, T, Keys, I, Pathway...>())::type;
    };

    template <class ... Pathway>
    struct edit_members
    {
        template <class Agent, class Default_index_type, class Root_data, class T, class Keys, std::size_t ... Is>
        struct membs : Keys, RareTs::Class::adapt_member<edit_member<Agent, Default_index_type, Root_data, T, Keys, Pathway...>::template type, T, Is>...
        {
            using nf_keys = Keys;
            using nf_path = type_tags<Pathway...>;
            using nf_editor_type = typename Agent::editor_type;

            membs(Agent & agent, Keys keys) :
                Keys {keys},
                RareTs::Class::template adapt_member<edit_member<Agent, Default_index_type, Root_data, T, Keys, Pathway...>::template type, T, Is> {{ agent, keys }}...,
                agent(agent) {}

            template <std::size_t I> constexpr auto & from_member()
            {
                auto & [edit_memb] = static_cast<RareTs::Class::adapt_member<edit_member<Agent, Default_index_type, Root_data, T, Keys, Pathway...>::template type, T, I> &>(*this);
                return edit_memb;
            }

            template <class U> constexpr void operator=(U && value) {
                if constexpr ( has_path_selections<Pathway...> )
                    agent.template set_l<Pathway...>(std::forward<U>(value), (Keys &)(*this));
                else
                    agent.template set<Pathway...>(std::forward<U>(value), (Keys &)(*this));
            }

        private:
            Agent & agent;
        };
    };

    template <class Agent, class Default_index_type, class Root_data, class T, class Keys, class ... Pathway, std::size_t... Is>
    edit_members<Pathway...>::template membs<Agent, Default_index_type, Root_data, T, Keys, Is...> edit_members_indexer(std::index_sequence<Is...>) { return {}; }

    template <class Agent, class Default_index_type, class Root_data, class T, class Keys = std::tuple<>, class ... Pathway>
    using edit_members_t = decltype(edit_members_indexer<Agent, Default_index_type, Root_data, T, Keys, Pathway...>(std::make_index_sequence<RareTs::Members<T>::total>()));

    template <typename T, std::size_t ... Is>
    constexpr std::size_t get_max_extent(std::index_sequence<Is...> = {})
    {
        if constexpr ( std::is_array_v<T> )
        {
            if constexpr ( sizeof...(Is) != std::rank_v<T> )
                return get_max_extent<T>(std::make_index_sequence<std::rank_v<T>>());
            else
                return std::max({std::extent_v<T, Is>...});
        }
        else
            return RareTs::static_array_size_v<T>;
    }

    template <typename T, std::size_t ... Is>
    constexpr std::size_t get_collapsed_extent(std::index_sequence<Is...> = {})
    {
        if constexpr ( std::is_array_v<T> )
        {
            if constexpr ( sizeof...(Is) != std::rank_v<T> )
                return get_collapsed_extent<T>(std::make_index_sequence<std::rank_v<T>>());
            else
                return (std::extent_v<T, Is> * ...);
        }
        else
            return RareTs::static_array_size_v<T>;
    }

    /// Needed specifically when an equivalent 1d index type needs to be determined for an md array
    /// @return a 1d index type appropriate to index all slots in a multi-dimensional array
    template <class Member>
    inline constexpr auto collapsed_index_typer()
    {
        static_assert(!std::is_void_v<Member>, "Collapsed index typer requires a non-void member");
        static_assert(RareTs::is_static_array_v<typename Member::type>, "Collapsed index typer requires a static array");
        constexpr std::size_t size = get_collapsed_extent<std::remove_cvref_t<typename Member::type>>();
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

    template <class Member>
    using collapsed_index_type_t = typename decltype(nf_hist::collapsed_index_typer<Member>())::type;

    template <class Default_index_type, class Member>
    inline constexpr auto index_typer()
    {
        if constexpr ( !std::is_void_v<Member> )
        {
            if constexpr ( RareTs::is_static_array_v<typename Member::type> )
            {
                // TODO: revisit in C++26, individual multi-dimensional array dimensions could use smaller indexes if the dimension index was passed
                // Presently, passing that everywhere required to do so is hard on BigObj limitations & compile-times
                constexpr std::size_t size = get_max_extent<std::remove_cvref_t<typename Member::type>>();
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
            else if constexpr ( Member::template hasNote<nf_hist::index_size_type>() )
                return Member::template getNote<nf_hist::index_size_type>();
            else
                return std::type_identity<Default_index_type>{};
        }
        else
            return std::type_identity<Default_index_type>{};
    };

    template <class Default_index_type, class Member>
    using index_type_t = typename decltype(nf_hist::index_typer<Default_index_type, Member>())::type;

    template <class Agent, class Default_index_type, class Root_data, class T, class Keys, template <class...> class Sub_element, class ... Pathway>
    class editable
    {
        template <class U, class Last_member, class Path_element, class ... Path_elements>
        static constexpr auto index_impl()
        {
            if constexpr ( is_path_selections_v<Path_element> )
            {
                if constexpr ( sizeof...(Path_elements) == 0 )
                    return std::type_identity<index_type_t<Default_index_type, Last_member>> {};
                else
                    return index_impl<std::remove_cvref_t<decltype(std::declval<U>()[0])>, Last_member, Path_elements...>();
            }
            else if constexpr ( is_path_member_v<Path_element> )
            {
                if constexpr ( sizeof...(Path_elements) == 0 )
                    return std::type_identity<index_type_t<Default_index_type, RareTs::Member<U, Path_element::index>>> {};
                else
                    return index_impl<typename RareTs::Member<U, Path_element::index>::type, RareTs::Member<U, Path_element::index>, Path_elements...>();
            }
            else if constexpr ( is_path_index_v<Path_element> )
            {
                if constexpr ( sizeof...(Path_elements) == 0 )
                    return std::type_identity<index_type_t<Default_index_type, Last_member>> {};
                else
                    return index_impl<std::remove_cvref_t<decltype(std::declval<U>()[0])>, Last_member, Path_elements...>();
            }
        }

        class random_access : public Keys
        {
            using index_type = typename decltype(index_impl<Root_data, void, Pathway...>())::type;

            template <std::size_t... Is> static constexpr auto array_op_typer(std::index_sequence<Is...>) -> Sub_element<
                Agent, Default_index_type, Root_data, T, std::tuple<std::tuple_element_t<Is, Keys>..., index_type>, Pathway..., path_index<sizeof...(Is)>> { return {}; }
            template <std::size_t... Is> static constexpr auto sub_array_op_typer(std::index_sequence<Is...>) -> typename editable<
                Agent, Default_index_type, Root_data, RareTs::element_type_t<T>, std::tuple<std::tuple_element_t<Is, Keys>..., index_type>, Sub_element, Pathway..., path_index<sizeof...(Is)>>::array { return {}; }

        protected:
            Agent & agent;

            constexpr auto & member_ref() { return agent.template get_member_reference<Pathway...>(agent.t, (Keys &)(*this)); }

        public:
            using nf_keys = Keys;
            using nf_path = type_tags<Pathway...>;
            using nf_editor_type = typename Agent::editor_type;
            using route = path_tagged_keys<Keys, nf_path, typename Agent::editor_type>;

            using selection_op_type = Sub_element<Agent, Default_index_type, Root_data, T, Keys, Pathway..., path_selections>;
            using array_op_type = decltype(array_op_typer(std::make_index_sequence<std::tuple_size_v<Keys>>()));
            using sub_array_op_type = decltype(sub_array_op_typer(std::make_index_sequence<std::tuple_size_v<Keys>>()));
            
            constexpr const auto & sel() const { return agent.template get_selections<Pathway...>(); }
            constexpr auto & attached_data(std::size_t i) const { return agent.template get_attached_data<Pathway...>()[i]; }
            constexpr const auto & read_attached_data() const { return agent.template get_attached_data<Pathway...>(); }

            inline void clear_selections()
            {
                auto & sel = agent.template get_selections<Pathway...>();
                if ( !sel.empty() )
                {
                    agent.event_offsets.push_back(agent.events.size());
                    agent.events.push_back(uint8_t(op::clear_selections));
                    agent.template serialize_pathway<Pathway...>((Keys &)(*this));

                    write_selection_vector(agent.events, sel);
                    RareTs::clear(sel);

                    if constexpr ( Agent::template has_selections_changed_op<route> )
                        agent.user.selections_changed(route{(Keys &)(*this)});
                }
            }

            inline void select_all()
            {
                auto & sel = agent.template get_selections<Pathway...>();
                agent.event_offsets.push_back(agent.events.size());
                agent.events.push_back(uint8_t(op::select_all));
                agent.template serialize_pathway<Pathway...>((Keys &)(*this));

                write_selection_vector(agent.events, sel);
                RareTs::clear(sel);
                sel.assign(member_ref().size(), 0);
                std::iota(sel.begin(), sel.end(), (RareTs::element_type_t<RareTs::remove_cvref_t<decltype(sel)>>)0);

                if constexpr ( Agent::template has_selections_changed_op<route> )
                    agent.user.selections_changed(route{(Keys &)(*this)});
            }

            /// Selects the given index
            /// @param i the index to be selected (must not be selected before calling select)
            inline void select(index_type i)
            {
                auto & sel = agent.template get_selections<Pathway...>();
                agent.event_offsets.push_back(agent.events.size());
                agent.events.push_back(uint8_t(op::select));
                agent.template serialize_pathway<Pathway...>((Keys &)(*this));

                agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&i), reinterpret_cast<const std::uint8_t*>(&i)+sizeof(i));
                if ( std::find(sel.begin(), sel.end(), i) == sel.end() )
                    RareTs::append(sel, i);
                else
                    throw std::invalid_argument("Cannot select an index that is already selected");

                if constexpr ( Agent::template has_selections_changed_op<route> )
                    agent.user.selections_changed(route{(Keys &)(*this)});
            }

            /// Selects the given indexes
            /// @param added_selections the indexes to be selected (must not be selected before calling select)
            inline void select(const std::vector<index_type> & added_selections)
            {
                auto & sel = agent.template get_selections<Pathway...>();
                agent.event_offsets.push_back(agent.events.size());
                agent.events.push_back(uint8_t(op::select_n));
                agent.template serialize_pathway<Pathway...>((Keys &)(*this));

                index_type size = static_cast<index_type>(added_selections.size());
                agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));
                agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&(added_selections[0])),
                    reinterpret_cast<const std::uint8_t*>(&(added_selections[0]))+sizeof(added_selections[0])*static_cast<std::size_t>(size));

                for ( auto i : added_selections )
                {
                    if ( std::find(sel.begin(), sel.end(), i) == sel.end() )
                        RareTs::append(sel, i);
                    else
                        throw std::invalid_argument("Cannot select an index that is already selected");
                }
                if constexpr ( Agent::template has_selections_changed_op<route> )
                    agent.user.selections_changed(route{(Keys &)(*this)});
            }

            /// Deselects the given index
            /// @param i the index to deselect (this index must be selected before calling deselect)
            inline void deselect(index_type i)
            {
                auto & sel = agent.template get_selections<Pathway...>();
                agent.event_offsets.push_back(agent.events.size());
                agent.events.push_back(uint8_t(op::deselect));
                agent.template serialize_pathway<Pathway...>((Keys &)(*this));
                
                auto found = std::find(sel.begin(), sel.end(), i);
                index_type found_at = static_cast<index_type>(std::distance(std::begin(sel), found));
                agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&i), reinterpret_cast<const std::uint8_t*>(&i)+sizeof(i));
                agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&found_at), reinterpret_cast<const std::uint8_t*>(&found_at)+sizeof(found_at));

                if ( found == sel.end() )
                    throw std::invalid_argument("Cannot deselect an index that is not selected");
                else
                    sel.erase(found);

                if constexpr ( Agent::template has_selections_changed_op<route> )
                    agent.user.selections_changed(route{(Keys &)(*this)});
            }

            /// Deselects the given indexes
            /// @param removed_selections the indexes to deselect (these indexes must be selected before calling deselect)
            inline void deselect(const std::vector<index_type> & removed_selections)
            {
                auto & sel = agent.template get_selections<Pathway...>();
                agent.event_offsets.push_back(agent.events.size());
                agent.events.push_back(uint8_t(op::deselect_n));
                agent.template serialize_pathway<Pathway...>((Keys &)(*this));

                auto num_removed = removed_selections.size();
                index_type size = static_cast<index_type>(num_removed);
                agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));
                agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&removed_selections[0]),
                    reinterpret_cast<const std::uint8_t*>(&removed_selections[0])+sizeof(removed_selections[0])*size);

                for ( auto i : removed_selections )
                {
                    auto found = std::find(sel.begin(), sel.end(), i);
                    if ( found != sel.end() )
                    {
                        index_type found_at = static_cast<index_type>(std::distance(std::begin(sel), found));
                        agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&found_at), reinterpret_cast<const std::uint8_t*>(&found_at)+sizeof(found_at));
                        sel.erase(found);
                    }
                    else
                        throw std::invalid_argument("Cannot deselect an index that is not selected");
                }
                if constexpr ( Agent::template has_selections_changed_op<route> )
                    agent.user.selections_changed(route{(Keys &)(*this)});
            }

            inline void toggle_selected(index_type i)
            {
                auto & sel = agent.template get_selections<Pathway...>();
                agent.event_offsets.push_back(agent.events.size());
                agent.events.push_back(uint8_t(op::toggle_selection));
                agent.template serialize_pathway<Pathway...>((Keys &)(*this));
                agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&i), reinterpret_cast<const std::uint8_t*>(&i)+sizeof(i));
                auto found = std::find(sel.begin(), sel.end(), i);
                u8bool::write(agent.events, found != sel.end());
                if ( found != sel.end() )
                {
                    index_type found_at = static_cast<index_type>(std::distance(std::begin(sel), found));
                    agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&found_at), reinterpret_cast<const std::uint8_t*>(&found_at)+sizeof(found_at));
                    std::erase(sel, i);
                }
                else
                    RareTs::append(sel, i);

                if constexpr ( Agent::template has_selections_changed_op<route> )
                    agent.user.selections_changed(route{(Keys &)(*this)});
            }

            inline void toggle_selected(const std::vector<index_type> & toggled_selections)
            {
                auto & sel = agent.template get_selections<Pathway...>();
                agent.event_offsets.push_back(agent.events.size());
                agent.events.push_back(uint8_t(op::toggle_selection_n));
                agent.template serialize_pathway<Pathway...>((Keys &)(*this));

                auto num_toggled = toggled_selections.size();
                index_type size = static_cast<index_type>(num_toggled);
                agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));
                agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&toggled_selections[0]),
                    reinterpret_cast<const std::uint8_t*>(&toggled_selections[0])+sizeof(toggled_selections[0])*static_cast<std::size_t>(size));

                std::vector<bool> was_selected(num_toggled, false);
                std::vector<index_type> prev_sel_indexes {};
                prev_sel_indexes.reserve(num_toggled);
                for ( std::size_t i=0; i<toggled_selections.size(); ++i )
                {
                    auto found = std::find(sel.begin(), sel.end(), toggled_selections[i]);
                    if ( found != sel.end() )
                    {
                        was_selected[i] = true;
                        prev_sel_indexes.push_back(static_cast<index_type>(std::distance(sel.begin(), found)));
                        sel.erase(found);
                    }
                    else
                        RareTs::append(sel, toggled_selections[i]);
                }
                write_vec_bool_data(agent.events, was_selected);
                if ( !prev_sel_indexes.empty() )
                {
                    agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&prev_sel_indexes[0]),
                        reinterpret_cast<const std::uint8_t*>(&prev_sel_indexes[0])+sizeof(prev_sel_indexes[0])*std::size(prev_sel_indexes));
                }

                if constexpr ( Agent::template has_selections_changed_op<route> )
                    agent.user.selections_changed(route{(Keys &)(*this)});
            }

            inline void sort_selection()
            {
                auto & sel = agent.template get_selections<Pathway...>();
                agent.event_offsets.push_back(agent.events.size());
                agent.events.push_back(uint8_t(op::sort_selections));
                agent.template serialize_pathway<Pathway...>((Keys &)(*this));

                auto source_indexes = tracked_sort<false, index_type>(sel);
                index_type serialized_size = static_cast<index_type>(source_indexes.size());
                agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&serialized_size),
                    reinterpret_cast<const std::uint8_t*>(&serialized_size)+sizeof(serialized_size));

                for ( auto index : source_indexes )
                {
                    index_type source_index = static_cast<index_type>(index);
                    agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&source_index),
                        reinterpret_cast<const std::uint8_t*>(&source_index)+sizeof(source_index));
                }
                
                if constexpr ( Agent::template has_selections_changed_op<route> )
                    agent.user.selections_changed(route{(Keys &)(*this)});
            }

            inline void sort_selection_descending()
            {
                auto & sel = agent.template get_selections<Pathway...>();
                agent.event_offsets.push_back(agent.events.size());
                agent.events.push_back(uint8_t(op::sort_selections_desc));
                agent.template serialize_pathway<Pathway...>((Keys &)(*this));

                auto source_indexes = tracked_sort<true, index_type>(sel);
                index_type serialized_size = static_cast<index_type>(source_indexes.size());
                agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&serialized_size),
                    reinterpret_cast<const std::uint8_t*>(&serialized_size)+sizeof(serialized_size));

                for ( auto index : source_indexes )
                {
                    index_type source_index = static_cast<index_type>(index);
                    agent.events.insert(agent.events.end(), reinterpret_cast<const std::uint8_t*>(&source_index),
                        reinterpret_cast<const std::uint8_t*>(&source_index)+sizeof(source_index));
                }
                if constexpr ( Agent::template has_selections_changed_op<route> )
                    agent.user.selections_changed(route{(Keys &)(*this)});
            }
            
            constexpr auto selection() { return selection_op_type(agent, (std::add_lvalue_reference_t<Keys>)(*this)); }

            random_access(Agent & agent, Keys && keys) : Keys {std::move(keys)}, agent(agent) {}

            auto operator[](std::size_t i) const {
                if constexpr ( RareTs::is_static_array_v<T> ) // Multi-dimensional array
                    return sub_array_op_type(agent, std::tuple_cat((std::add_lvalue_reference_t<Keys>)(*this), std::tuple{static_cast<index_type>(i)}));
                else
                    return array_op_type(agent, std::tuple_cat((std::add_lvalue_reference_t<Keys>)(*this), std::tuple{static_cast<index_type>(i)}));
            }
        };

    public:
        struct array : random_access
        {
            array(Agent & agent, Keys keys) : random_access { agent, std::move(keys) } {}
            void reset() { random_access::agent.template reset<Pathway...>((Keys &)(*this)); }
        };

        struct vector : random_access
        {
            vector(Agent & agent, Keys keys) : random_access { agent, std::move(keys) } {}
            void reset() { random_access::agent.template reset<Pathway...>((Keys &)(*this)); }
            void reserve(std::size_t size) { random_access::agent.template reserve<Pathway...>(size, (Keys &)(*this)); }
            void trim() { random_access::agent.template trim<Pathway...>((Keys &)(*this)); }
            void assign_default(std::size_t size) { random_access::agent.template assign_default<Pathway...>(size, (Keys &)(*this)); }
            template <class U> void assign(std::size_t size, U && value) { random_access::agent.template assign<Pathway...>(size, std::forward<U>(value), (Keys &)(*this)); }
            template <class U> void operator=(U && value) {
                if constexpr ( has_path_selections<Pathway...> )
                    random_access::agent.template set_l<Pathway...>(std::forward<U>(value), (Keys &)(*this));
                else
                    random_access::agent.template set<Pathway...>(std::forward<U>(value), (Keys &)(*this));
            }

            template <class Indexes, class Value> void set(Indexes && indexes, Value && value) { random_access::agent.template set_n<Pathway...>(indexes, std::forward<Value>(value), (Keys &)(*this)); }
            template <class U> void append(U && value) {
                if constexpr ( RareTs::is_iterable_v<std::remove_cvref_t<U>> && !RareTs::is_optional_v<std::remove_cvref_t<U>> )
                    random_access::agent.template append_n<Pathway...>(std::forward<U>(value), (Keys &)(*this));
                else
                    random_access::agent.template append<Pathway...>(std::forward<U>(value), (Keys &)(*this));
            }
            template <class Index, class Value> void insert(Index insertion_index, Value && value) {
                if constexpr ( RareTs::is_iterable_v<std::remove_cvref_t<Value>> && !RareTs::is_optional_v<std::remove_cvref_t<Value>> )
                    random_access::agent.template insert_n<Pathway...>(insertion_index, std::forward<Value>(value), (Keys &)(*this));
                else
                    random_access::agent.template insert<Pathway...>(insertion_index, std::forward<Value>(value), (Keys &)(*this));
            }
            template <class Index> void remove(Index removal_index) {
                if constexpr ( RareTs::is_iterable_v<std::remove_cvref_t<Index>> )
                    random_access::agent.template remove_n<Pathway...>(removal_index, (Keys &)(*this));
                else
                    random_access::agent.template remove<Pathway...>(removal_index, (Keys &)(*this));
            }
            void sort() { random_access::agent.template sort<Pathway...>((Keys &)(*this)); }
            void sort_desc() { random_access::agent.template sort_desc<Pathway...>((Keys &)(*this)); }
            void remove_selection() { random_access::agent.template remove_l<Pathway...>((Keys &)(*this)); }

            template <class U> void swap(U && first_index, U && second_index) {
                random_access::agent.template swap<Pathway...>(std::forward<U>(first_index), std::forward<U>(second_index), (Keys &)(*this));
            }
            template <class U> void move_up(U && moved_index) {
                if constexpr ( RareTs::is_iterable_v<std::remove_cvref_t<U>> )
                    random_access::agent.template move_up_n<Pathway...>(std::forward<U>(moved_index), (Keys &)(*this));
                else
                    random_access::agent.template move_up<Pathway...>(std::forward<U>(moved_index), (Keys &)(*this));
            }
            template <class U> void move_top(U && moved_index) {
                if constexpr ( RareTs::is_iterable_v<std::remove_cvref_t<U>> )
                    random_access::agent.template move_top_n<Pathway...>(std::forward<U>(moved_index), (Keys &)(*this));
                else
                    random_access::agent.template move_top<Pathway...>(std::forward<U>(moved_index), (Keys &)(*this));
            }
            template <class U> void move_down(U && moved_index) {
                if constexpr ( RareTs::is_iterable_v<std::remove_cvref_t<U>> )
                    random_access::agent.template move_down_n<Pathway...>(std::forward<U>(moved_index), (Keys &)(*this));
                else
                    random_access::agent.template move_down<Pathway...>(std::forward<U>(moved_index), (Keys &)(*this));
            }
            template <class U> void move_bottom(U && moved_index) {
                if constexpr ( RareTs::is_iterable_v<std::remove_cvref_t<U>> )
                    random_access::agent.template move_bottom_n<Pathway...>(std::forward<U>(moved_index), (Keys &)(*this));
                else
                    random_access::agent.template move_bottom<Pathway...>(std::forward<U>(moved_index), (Keys &)(*this));
            }
            template <class I, class U> void move_to(U && moved_index, I index_moved_to) {
                if constexpr ( RareTs::is_iterable_v<std::remove_cvref_t<U>> )
                    random_access::agent.template move_to_n<Pathway...>(std::forward<U>(moved_index), index_moved_to, (Keys &)(*this));
                else
                    random_access::agent.template move_to<Pathway...>(std::forward<U>(moved_index), index_moved_to, (Keys &)(*this));
            }

            void move_selections_up() { random_access::agent.template move_up_l<Pathway...>((Keys &)(*this)); }
            void move_selections_top() { random_access::agent.template move_top_l<Pathway...>((Keys &)(*this)); }
            void move_selections_down() { random_access::agent.template move_down_l<Pathway...>((Keys &)(*this)); }
            void move_selections_bottom() { random_access::agent.template move_bottom_l<Pathway...>((Keys &)(*this)); }
            template <class Index> void move_selections_to(Index index_moved_to) { random_access::agent.template move_to_l<Pathway...>(index_moved_to, (Keys &)(*this)); }
        };
    };
    
    template <class Agent, class Default_index_type, class Root_data, class T, class Keys, class ... Pathway>
    static constexpr auto edit_vector();
    
    template <class Agent, class Default_index_type, class Root_data, class T, class Keys, class ... Pathway>
    static constexpr auto edit_array();

    template <class Agent, class Default_index_type, class Root_data, class T, class Keys, class ... Pathway>
    struct edit_optional : Keys
    {
        using nf_keys = Keys;
        using nf_path = type_tags<Pathway...>;
        using nf_editor_type = typename Agent::editor_type;
        using type = T;
        using optional_value_type = typename std::remove_cvref_t<type>::value_type;

    private:
        static constexpr auto deref_type()
        {
            if constexpr ( RareTs::is_static_array_v<optional_value_type> )
                return decltype(edit_array<Agent, Default_index_type, Root_data, optional_value_type, Keys, Pathway..., path_member<0>>()){};
            else if constexpr ( RareTs::is_specialization_v<optional_value_type, std::vector> ) // Vector
                return decltype(edit_vector<Agent, Default_index_type, Root_data, optional_value_type, Keys, Pathway..., path_member<0>>()){};
            else if constexpr ( RareTs::is_macro_reflected_v<optional_value_type> ) // Reflected object
                return std::type_identity<edit_members_t<Agent, Default_index_type, Root_data, optional_value_type, Keys, Pathway..., path_member<0>>>{};
            else if constexpr ( RareTs::is_optional_v<optional_value_type> )
                return std::type_identity<edit_optional<Agent, Default_index_type, Root_data, optional_value_type, Keys, Pathway..., path_member<0>>>{};
            else // Primitive
                return std::type_identity<edit_primitive<Agent, Default_index_type, Root_data, optional_value_type, Keys, Pathway..., path_member<0>>>{};
        }

        Agent & agent;
        typename decltype(deref_type())::type deref;

    public:

        edit_optional(Agent & agent, Keys keys) :
            Keys {std::move(keys)}, agent(agent), deref{agent, (Keys &)(*this)} {}

        template <class U> void operator=(U && value) {
            if constexpr ( has_path_selections<Pathway...> )
                agent.template set_l<Pathway...>(std::forward<U>(value), (Keys &)(*this));
            else
                agent.template set<Pathway...>(std::forward<U>(value), (Keys &)(*this));
        }
        constexpr auto operator->() { return &deref; }
    };

    template <class Agent, class Default_index_type, class Root_data, class T, class Keys, class ... Pathway>
    static constexpr auto edit_vector()
    {
        using element_type = RareTs::element_type_t<T>;
        if constexpr ( RareTs::is_reflected_v<element_type> )
            return std::type_identity<typename editable<Agent, Default_index_type, Root_data, element_type, Keys, edit_members_t, Pathway...>::vector>{};
        else if constexpr ( RareTs::is_optional_v<element_type> )
            return std::type_identity<typename editable<Agent, Default_index_type, Root_data, element_type, Keys, edit_optional, Pathway...>::vector>{};
        else
            return std::type_identity<typename editable<Agent, Default_index_type, Root_data, element_type, Keys, edit_primitive, Pathway...>::vector>{};
    }

    template <class Agent, class Default_index_type, class Root_data, class T, class Keys, class ... Pathway>
    static constexpr auto edit_array()
    {
        using element_type = RareTs::element_type_t<T>;
        if constexpr ( RareTs::is_static_array_v<element_type> )
        {
            using sub_element_type = RareTs::element_type_t<element_type>;
            if constexpr ( RareTs::is_reflected_v<sub_element_type> )
                return std::type_identity<typename editable<Agent, Default_index_type, Root_data, element_type, Keys, edit_members_t, Pathway...>::array>{};
            else if constexpr ( RareTs::is_optional_v<sub_element_type> )
                return std::type_identity<typename editable<Agent, Default_index_type, Root_data, element_type, Keys, edit_optional, Pathway...>::array>{};
            else
                return std::type_identity<typename editable<Agent, Default_index_type, Root_data, element_type, Keys, edit_primitive, Pathway...>::array>{};
        }
        else if constexpr ( RareTs::is_reflected_v<element_type> )
            return std::type_identity<typename editable<Agent, Default_index_type, Root_data, element_type, Keys, edit_members_t, Pathway...>::array>{};
        else if constexpr ( RareTs::is_optional_v<element_type> )
            return std::type_identity<typename editable<Agent, Default_index_type, Root_data, element_type, Keys, edit_optional, Pathway...>::array>{};
        else
            return std::type_identity<typename editable<Agent, Default_index_type, Root_data, element_type, Keys, edit_primitive, Pathway...>::array>{};
    }

    template <class Agent, class Default_index_type, class Root_data, class T, class Keys, std::size_t I, class ... Pathway>
    static constexpr auto get_edit_member()
    {
        using member_type = typename RareTs::Member<T, I>::type;
        if constexpr ( RareTs::is_static_array_v<member_type> )
            return decltype(edit_array<Agent, Default_index_type, Root_data, member_type, Keys, Pathway..., path_member<I>>()){};
        else if constexpr ( RareTs::is_specialization_v<member_type, std::vector> ) // Vector
            return decltype(edit_vector<Agent, Default_index_type, Root_data, member_type, Keys, Pathway..., path_member<I>>()){};
        else if constexpr ( RareTs::is_macro_reflected_v<member_type> ) // Reflected object
            return std::type_identity<edit_members_t<Agent, Default_index_type, Root_data, member_type, Keys, Pathway..., path_member<I>>>{};
        else if constexpr ( RareTs::is_optional_v<member_type> )
            return std::type_identity<edit_optional<Agent, Default_index_type, Root_data, member_type, Keys, Pathway..., path_member<I>>>{};
        else // Primitive
            return std::type_identity<edit_primitive<Agent, Default_index_type, Root_data, member_type, Keys, Pathway..., path_member<I>>>{};
    }

    template <typename T> struct is_selection_leaf_member {
        static constexpr bool value = T::isData && !T::isStatic &&
            (RareTs::is_static_array_v<typename T::type> ||
             RareTs::is_specialization_v<typename T::type, std::vector>);
    };

    template <typename T> inline constexpr bool is_selection_leaf_member_v = is_selection_leaf_member<T>::value;

    template <class T, std::size_t ... Is> static constexpr bool has_selection_member(std::index_sequence<Is...>)
    {
        using member_type = typename T::type;
        if constexpr ( RareTs::is_macro_reflected_v<member_type> )
        {
            return (has_selection_member<RareTs::Member<member_type, Is>>(
                std::make_index_sequence<reflected_member_count<typename RareTs::Member<member_type, Is>::type>()>()
            ) || ...);
        }
        else
            return is_selection_leaf_member_v<T>;
    };

    template <typename T> struct is_selection_member {
        static constexpr bool value = has_selection_member<T>(std::make_index_sequence<reflected_member_count<typename T::type>()>());
    };

    template <class Default_index_type, class Member, class Attached_type>
    struct leaf_data
    {
        std::vector<Attached_type> attached_data;
        std::vector<index_type_t<Default_index_type, Member>> sel;
    };
    
    template <class Default_index_type, class T, std::size_t ... Is>
    struct object_selection;

    template <class Default_index_type, class T, std::size_t I>
    static constexpr auto get_member_selection()
    {
        using member = RareTs::Member<T, I>;
        using member_type = typename member::type;
        if constexpr ( !is_selection_leaf_member_v<member> )
        {
            return RareTs::template Members<member_type>::template pack<is_selection_member>([&](auto ... member) {
                return object_selection<Default_index_type, member_type, decltype(member)::index...>{};
            });
        }
        else if constexpr ( RareTs::is_static_array_v<member_type> || RareTs::is_specialization_v<member_type, std::vector> )
        {
            if constexpr ( member::template hasNote<attach_data_type>() )
            {
                using attached_type = typename std::remove_cvref_t<decltype(member::template getNote<attach_data_type>())>::type;
                if constexpr ( !std::is_void_v<attached_type> )
                    return leaf_data<Default_index_type, member, attached_type> {};
                else
                    return std::vector<index_type_t<Default_index_type, member>> {};
            }
            else
                return std::vector<index_type_t<Default_index_type, member>> {};
        }
        else
            static_assert(std::is_void_v<T>, "Unexpected member selection!");
    }

    template <class Default_index_type, class T>
    struct member_selection {
        template <std::size_t I> using type = decltype(get_member_selection<Default_index_type, T, I>());
    };
    
    inline constexpr std::nullptr_t no_selection {};
    inline constexpr std::nullptr_t no_attached_data {};

    template <class Default_index_type, class T, std::size_t ... Is>
    struct object_selection : RareTs::Class::adapt_member<member_selection<Default_index_type, T>::template type, T, Is>...
    {
        template <std::size_t I> constexpr auto & from_member()
        {
            if constexpr ( std::is_base_of_v<RareTs::Class::adapt_member<member_selection<Default_index_type, T>::template type, T, I>, object_selection<Default_index_type, T, Is...>> )
            {
                auto & [selection] = static_cast<RareTs::Class::adapt_member<member_selection<Default_index_type, T>::template type, T, I> &>(*this);
                return selection;
            }
            else
            {
                return no_selection;
            }
        }

        template <std::size_t I, class ... Pathway, class Sizer>
        constexpr void init_attached_datum(Sizer sizer)
        {
            auto & [leaf_data] = static_cast<RareTs::Class::adapt_member<member_selection<Default_index_type, T>::template type, T, I> &>(*this);
            if constexpr ( requires { leaf_data.template init_attached_data<Pathway..., path_member<I>>(sizer); } )
            {
                leaf_data.template init_attached_data<Pathway..., path_member<I>>(sizer);
            }
            else if constexpr ( requires { leaf_data.attached_data.clear(); } )
            {
                using attached_data_vec = std::remove_cvref_t<decltype(leaf_data.attached_data)>;
                leaf_data.attached_data = attached_data_vec(sizer(type_tags<Pathway..., path_member<I>>{}));
            }
        }

        template <class ... Pathway, class Sizer>
        constexpr void init_attached_data([[maybe_unused]] Sizer sizer)
        {
            (init_attached_datum<Is, Pathway...>(sizer), ...);
        }

        template <std::size_t I>
        void clear()
        {
            using sel_member = RareTs::Class::adapt_member<member_selection<Default_index_type, T>::template type, T, I>;
            if constexpr ( requires { static_cast<sel_member &>(*this).clear(); } )
                static_cast<sel_member &>(*this).clear();
            else if constexpr ( requires { static_cast<sel_member &>(*this).sel.clear(); } )
                static_cast<sel_member &>(*this).sel.clear();
        }

        void clear()
        {
            (clear<Is>(), ...);
        }
    };

    template <class Default_index_type, class T, std::size_t ... Is>
    constexpr auto selections(std::index_sequence<Is...>) {
        return object_selection<Default_index_type, T, Is...>{};
    }

    template <class Default_index_type, class T> auto selections() {
        return RareTs::template Members<T>::template pack<is_selection_member>([&](auto & ... member) {
            return selections<Default_index_type, T>(std::index_sequence<RareTs::remove_cvref_t<decltype(member)>::index...>{});
        });
    }

    template <class T, class Edit>
    struct read_edit_pair
    {
        using read_type = T;
        using edit_type = Edit;

        const T & read;
        Edit edit;
    };

    template <class T, class User, class Editor_type>
    struct agent
    {
        using type = T;
        using editor_type = Editor_type;
        using default_index_type = typename decltype(def_index_type<T>())::type;
        decltype(nf_hist::selections<default_index_type, T>()) selections {};
        std::vector<std::uint8_t> events {std::uint8_t(0)}; /// First byte is unused
        std::vector<std::uint64_t> event_offsets {};
        T & t;
        User & user;

        void clear()
        {
            selections.clear();
            event_offsets.clear();
            events = {std::uint8_t(0)};
        }

        /// Trims events such that the event currently at new_first_event becomes event at the 0th event offset
        /// @param new_first_event the event you wish to come first after the trim
        /// @return the count of trimmed events
        std::size_t trim(std::size_t new_first_event)
        {
            if ( new_first_event >= event_offsets.size() )
            {
                new_first_event = event_offsets.size();
                event_offsets.clear();
                events = {std::uint8_t(0)};
                return new_first_event;
            }
            else if ( new_first_event > 0 )
            {
                std::uint64_t new_first_event_offset = event_offsets[new_first_event];
                event_offsets.erase(event_offsets.begin(), std::next(event_offsets.begin(), static_cast<std::ptrdiff_t>(new_first_event)));
                events.erase(std::next(events.begin(), 1), std::next(events.begin(), static_cast<std::ptrdiff_t>(new_first_event_offset)));
                for ( auto & event_offset : event_offsets )
                    event_offset -= (new_first_event_offset-1);

                return new_first_event;
            }
            else
                return 0;
        }

        template <class Usr, class Route, class Value>
        using value_changed_op = decltype(std::declval<Usr>().value_changed(std::declval<Route>(), std::declval<Value>(), std::declval<Value>()));

        template <class Usr, class Route>
        using element_added_op = decltype(std::declval<Usr>().element_added(std::declval<Route>(), std::declval<std::size_t>()));

        template <class Usr, class Route>
        using element_removed_op = decltype(std::declval<Usr>().element_removed(std::declval<Route>(), std::declval<std::size_t>()));

        template <class Usr, class Route>
        using element_moved_op = decltype(std::declval<Usr>().element_moved(std::declval<Route>(), std::declval<std::size_t>(), std::declval<std::size_t>()));

        template <class Usr, class Route>
        using selections_changed_op = decltype(std::declval<Usr>().selections_changed(std::declval<Route>()));

        template <class Route, class Value>
        static constexpr bool has_value_changed_op = RareTs::op_exists_v<value_changed_op, User, Route, Value>;

        template <class Route>
        static constexpr bool has_element_added_op = RareTs::op_exists_v<element_added_op, User, Route>;

        template <class Route>
        static constexpr bool has_element_removed_op = RareTs::op_exists_v<element_removed_op, User, Route>;

        template <class Route>
        static constexpr bool has_element_moved_op = RareTs::op_exists_v<element_moved_op, User, Route>;

        template <class Route>
        static constexpr bool has_selections_changed_op = RareTs::op_exists_v<selections_changed_op, User, Route>;

        template <class Usr, class Route, class Old_value, class New_value>
        static constexpr void notify_value_changed(Usr & user, Route route, Old_value && old_value, New_value && new_value)
        {
            user.value_changed(route, std::forward<Old_value>(old_value), std::forward<New_value>(new_value));
        }

        template <class Usr, class Route>
        static constexpr void notify_element_added(Usr & user, Route route, std::size_t index)
        {
            user.element_added(route, index);
        }

        template <class Usr, class Route>
        static constexpr void notify_element_removed(Usr & user, Route route, std::size_t index)
        {
            user.element_removed(route, index);
        }

        template <class Usr, class Route>
        static constexpr void notify_element_moved(Usr & user, Route route, std::size_t old_index, std::size_t new_index)
        {
            user.element_moved(route, old_index, new_index);
        }

        template <class Usr, class Route>
        static constexpr void notify_selections_changed(Usr & user, Route route)
        {
            user.selections_changed(route);
        }

        agent(T & t, User & user) : t(t), user(user) {}

        auto get_event_offset_range(std::size_t event_index) const
        {
            struct OffsetRange
            {
                std::uint64_t begin;
                std::uint64_t end;
            };
            return OffsetRange {
                event_offsets[event_index],
                event_index == event_offsets.size()-1 ? events.size() : event_offsets[event_index+1]
            };
        }
        
        template <class Path_element, class ... Pathway>
        constexpr auto & get_selections_data(auto & selections)
        {
            if constexpr ( is_path_member_v<Path_element> && !std::is_null_pointer_v<std::remove_cvref_t<decltype(selections)>> )
            {
                if constexpr ( sizeof...(Pathway) == 0 )
                    return selections.template from_member<Path_element::index>();
                else
                    return get_selections_data<Pathway...>(selections.template from_member<Path_element::index>());
            }
            else
            {
                return no_selection;
            }
        }

        template <class ... Pathway>
        constexpr auto & get_selections() {
            if constexpr ( sizeof...(Pathway) == 0 )
                return no_selection;
            else
            {
                auto & sel_data = get_selections_data<Pathway...>(selections);
                if constexpr ( requires{sel_data.sel;} )
                    return sel_data.sel;
                else
                    return sel_data;
            }
        }

        template <class ... Pathway>
        constexpr auto & get_attached_data() {
            if constexpr ( sizeof...(Pathway) == 0 )
                return no_attached_data;
            else
            {
                auto & sel_data = get_selections_data<Pathway...>(selections);
                if constexpr ( requires{sel_data.attached_data;} )
                    return sel_data.attached_data;
                else
                    return no_attached_data;
            }
        }

        template <class ... Pathway>
        static constexpr bool has_selections() {
            return !std::is_null_pointer_v<std::remove_cvref_t<decltype(std::declval<agent>().template get_selections<Pathway...>())>>;
        }

        template <class ... Pathway>
        static constexpr bool has_attached_data() {
            return !std::is_null_pointer_v<std::remove_cvref_t<decltype(std::declval<agent>().template get_attached_data<Pathway...>())>>;
        }

        template <class Keys, class U, class Path_element, class ... Pathway>
        auto & get_member_reference_impl(U & t, Keys & keys)
        {
            if constexpr ( is_path_member_v<Path_element> )
            {
                using member = RareTs::Member<U, Path_element::index>;
                if constexpr ( sizeof...(Pathway) == 0 )
                    return member::value(t);
                else
                    return get_member_reference_impl<Keys, typename member::type, Pathway...>(member::value(t), keys);
            }
            else if constexpr ( is_path_index_v<Path_element> )
            {
                if constexpr ( sizeof...(Pathway) == 0 )
                    return t[std::get<Path_element::index>(keys)];
                else
                {
                    return get_member_reference_impl<Keys, std::remove_cvref_t<decltype(t[std::get<Path_element::index>(keys)])>, Pathway...>(
                        t[std::get<Path_element::index>(keys)], keys);
                }
            }
            else
                static_assert(std::is_void_v<Path_element>, "Unexpected path element type!");
        }

        template <class ... Pathway, class Keys, class U>
        auto & get_member_reference(U & t, Keys & keys)
        {
            return get_member_reference_impl<Keys, U, Pathway...>(t, keys);
        }

        template <class Path_element, class ... Pathway, class Keys>
        void serialize_pathway(Keys & keys)
        {
            if constexpr ( sizeof...(Pathway) == 0 )
            {
                if constexpr ( is_path_selections_v<Path_element> )
                    events.push_back(uint8_t(path_op::leaf_sel_branch));
                else if constexpr ( is_path_member_v<Path_element> )
                {
                    static_assert(Path_element::index < 64, "Expected a lower member index...");
                    events.push_back(uint8_t(path_op::leaf_branch) | uint8_t(Path_element::index));
                }
                else if constexpr ( is_path_index_v<Path_element> )
                {
                    if constexpr ( sizeof...(Pathway) == 0 )
                    {
                        auto index = std::get<Path_element::index>(keys);
                        if constexpr ( std::is_same_v<uint6_t, decltype(index)> )
                            events.push_back(uint8_t(path_op::leaf_branch) | uint8_t(index));
                        else
                        {
                            events.push_back(uint8_t(path_op::leaf_branch));
                            events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&index), reinterpret_cast<const std::uint8_t*>(&index)+sizeof(index));
                        }
                    }
                }
                else
                    static_assert(std::is_void_v<Path_element>, "Unrecognized path element!");
            }
            else // More path remaining
            {
                if constexpr ( is_path_selections_v<Path_element> )
                    events.push_back(uint8_t(path_op::sel_branch));
                else if constexpr ( is_path_member_v<Path_element> )
                {
                    static_assert(Path_element::index < 64, "Expected a lower member index...");
                    events.push_back(uint8_t(path_op::branch) | uint8_t(Path_element::index));
                }
                else if constexpr ( is_path_index_v<Path_element> )
                {
                    auto index = std::get<Path_element::index>(keys);
                    if constexpr ( std::is_same_v<uint6_t, decltype(index)> )
                        events.push_back(uint8_t(path_op::branch) | uint8_t(index));
                    else
                    {
                        events.push_back(uint8_t(path_op::branch));
                        events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&index), reinterpret_cast<const std::uint8_t*>(&index)+sizeof(index));
                    }
                }
                else
                    static_assert(std::is_void_v<Path_element>, "Unrecognized path element!");

                serialize_pathway<Pathway...>(keys);
            }
        }

        template <class Member> void serialize_value(auto && value)
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
                bool has_value = value.has_value();
                u8bool::write(events, has_value);
                if ( has_value )
                    serialize_value<Member>(*value);
            }
            else if constexpr ( is_flat_mdspan_v<value_type> )
            {
                using collapsed_index_type = collapsed_index_type_t<Member>;
                constexpr auto size = static_cast<collapsed_index_type>(value_type::size);
                events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));
                for ( auto it = value.flat_begin(); it != value.flat_end(); ++it )
                    serialize_value<Member>(*it);
            }
            else if constexpr ( std::is_array_v<value_type> )
            {
                auto span = as_1d<const value_type>(value);
                using collapsed_index_type = collapsed_index_type_t<Member>;
                constexpr auto size = static_cast<collapsed_index_type>(decltype(span)::size);
                events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));
                for ( auto it = span.flat_begin(); it != span.flat_end(); ++it )
                    serialize_value<Member>(*it);
            }
            else if constexpr ( RareTs::is_static_array_v<value_type> )
            {
                constexpr auto size = static_cast<index_type>(RareTs::static_array_size_v<value_type>);
                events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));
                for ( auto & val : value )
                    serialize_value<Member>(value);
            }
            else if constexpr ( RareTs::is_iterable_v<value_type> )
            {
                auto size = static_cast<index_type>(value.size());
                events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&size), reinterpret_cast<const std::uint8_t*>(&size)+sizeof(size));
                for ( auto & val : value )
                    serialize_value<Member>(val);
            }
            else if constexpr ( RareTs::is_reflected_v<value_type> )
            {
                RareTs::Members<value_type>::forEach([&](auto member) {
                    serialize_value<decltype(member)>(member.value(value));
                });
            }
            else
                events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&value), reinterpret_cast<const std::uint8_t*>(&value)+sizeof(value));
        }

        template <class Member> void serialize_index(auto && index)
        {
            auto val = static_cast<index_type_t<default_index_type, Member>>(index);
            events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&val), reinterpret_cast<const std::uint8_t*>(&val)+sizeof(val));
        }

        template <bool After_sel, class Keys, class U, class F, class Last_member, class Path_element, class ... Pathway, class ... Path_traversed>
        void operate_thru_sel_impl(U & t, Keys & keys, F f, type_tags<Path_traversed...>)
        {
            if constexpr ( is_path_selections_v<Path_element> )
            {
                // Replace the path_sel element with a path_index; at present selections must be a top-level iterable preceding other uses of keys
                using index_type = index_type_t<default_index_type, Last_member>;
                auto & sel_data = get_selections<Path_traversed...>();
                for ( auto & sel : sel_data )
                {
                    auto new_keys = std::tuple_cat(std::tuple<index_type>{sel}, keys);
                    using ptk = std::remove_cvref_t<decltype(new_keys)>;
                    if constexpr ( sizeof...(Pathway) == 0 )
                        f(t[static_cast<std::size_t>(sel)], type_tags<Last_member, path_tagged_keys<ptk, type_tags<Path_traversed..., path_index<0>>, Editor_type>>{}, new_keys);
                    else
                        operate_thru_sel_impl<true, ptk, std::remove_cvref_t<decltype(t[0])>, F, Last_member, Pathway...>(t[sel], new_keys, f, type_tags<Path_traversed..., path_index<0>>{});
                }
            }
            else if constexpr ( is_path_member_v<Path_element> )
            {
                using member = RareTs::Member<U, Path_element::index>;
                if constexpr ( sizeof...(Pathway) == 0 )
                    f(member::value(t), type_tags<member, path_tagged_keys<Keys, type_tags<Path_traversed..., Path_element>, Editor_type>>{}, keys);
                else
                    operate_thru_sel_impl<After_sel, Keys, typename member::type, F, member, Pathway...>(member::value(t), keys, f, type_tags<Path_traversed..., Path_element>{});
            }
            else if constexpr ( is_path_index_v<Path_element> )
            {
                // If this is after the selection, path_index index is incremented so it's now correctly after the inserted sel index
                using path_elem = std::conditional_t<After_sel, path_index<Path_element::index+1>, Path_element>;
                if constexpr ( sizeof...(Pathway) == 0 )
                {
                    f(t[static_cast<std::size_t>(std::get<path_elem::index>(keys))],
                        type_tags<Last_member, path_tagged_keys<Keys, type_tags<Path_traversed..., path_elem>, Editor_type>>{}, keys);
                }
                else
                {
                    operate_thru_sel_impl<After_sel, Keys, std::remove_cvref_t<decltype(t[0])>, F, Last_member, Pathway...>(
                        t[static_cast<std::size_t>(std::get<path_elem::index>(keys))], keys, f, type_tags<Path_traversed..., path_elem>{});
                }
            }
            else
                static_assert(std::is_void_v<Path_element>, "Unexpected path element type!");
        }
        
        /// When operating through selections, a modified keys tuple must be built and passed to f
        /// @param t the root object you will be operating on
        /// @param keys the keys/array-indexes in the route being operated on
        /// @param f the lambda to call with a reference to the data being operated on (together with the route and other type info)
        template <class ... Pathway, class Keys, class U, class F>
        void operate_thru_sel(U & t, Keys & keys, F f)
        {
            operate_thru_sel_impl<false, Keys, U, F, void, Pathway...>(t, keys, f, {});
        }

        template <class Keys, class U, class F, class Last_member, class Path_element, class ... Pathway, class ... Path_traversed>
        void operate_on_impl(U & t, Keys & keys, F f, type_tags<Path_traversed...>)
        {
            if constexpr ( is_path_selections_v<Path_element> )
            {
                auto & sel_data = get_selections<Path_traversed...>();
                for ( auto & sel : sel_data )
                {
                    if constexpr ( sizeof...(Pathway) == 0 )
                        f(t[static_cast<std::size_t>(sel)], type_tags<Last_member, path_tagged_keys<Keys, type_tags<Path_traversed..., Path_element>, Editor_type>>{});
                    else
                        operate_on_impl<Keys, std::remove_cvref_t<decltype(t[0])>, F, Last_member, Pathway...>(t[sel], keys, f, type_tags<Path_traversed..., Path_element>{});
                }
            }
            else if constexpr ( is_path_member_v<Path_element> )
            {
                if constexpr ( RareTs::is_optional_v<std::remove_cvref_t<U>> )
                {
                    if constexpr ( sizeof...(Pathway) == 0 )
                        f(t, type_tags<Last_member, path_tagged_keys<Keys, type_tags<Path_traversed..., Path_element>, Editor_type>>{});
                    else if ( t.has_value() )
                        operate_on_impl<Keys, typename std::remove_cvref_t<U>::value_type, F, Last_member, Pathway...>(*t, keys, f, type_tags<Path_traversed..., Path_element>{});
                }
                else
                {
                    using member = RareTs::Member<U, Path_element::index>;
                    if constexpr ( sizeof...(Pathway) == 0 )
                        f(member::value(t), type_tags<member, path_tagged_keys<Keys, type_tags<Path_traversed..., Path_element>, Editor_type>>{});
                    else
                        operate_on_impl<Keys, typename member::type, F, member, Pathway...>(member::value(t), keys, f, type_tags<Path_traversed..., Path_element>{});
                }
            }
            else if constexpr ( is_path_index_v<Path_element> )
            {
                if constexpr ( sizeof...(Pathway) == 0 )
                    f(t[static_cast<std::size_t>(std::get<Path_element::index>(keys))], type_tags<Last_member, path_tagged_keys<Keys, type_tags<Path_traversed..., Path_element>, Editor_type>>{});
                else
                {
                    operate_on_impl<Keys, std::remove_cvref_t<decltype(t[static_cast<std::size_t>(std::get<Path_element::index>(keys))])>,
                        F, Last_member, Pathway...>(t[static_cast<std::size_t>(std::get<Path_element::index>(keys))], keys, f, type_tags<Path_traversed..., Path_element>{});
                }
            }
            else
                static_assert(std::is_void_v<Path_element>, "Unexpected path element type!");
        }
        
        template <class ... Pathway, class Keys, class U, class F>
        void operate_on(U & t, Keys & keys, F f)
        {
            if constexpr ( sizeof...(Pathway) == 0 )
                f(t, type_tags<void, path_tagged_keys<Keys, type_tags<>, Editor_type>>{});
            else
                operate_on_impl<Keys, U, F, void, Pathway...>(t, keys, f, {});
        }
        
        template <class Keys, class U, class Last_member, class Path_element, class ... Pathway, class ... Path_traversed>
        auto editor_from_path_impl(auto && editor, U & t, Keys & keys, type_tags<Path_traversed...>)
        {
            if constexpr ( is_path_selections_v<Path_element> )
            {
                auto & sel_data = get_selections<Path_traversed...>();
                for ( auto & sel : sel_data )
                {
                    if constexpr ( sizeof...(Pathway) == 0 )
                        return read_edit_pair{((const U &)t)[sel], editor[sel]};
                    else
                        return editor_from_path_impl<Keys, std::remove_cvref_t<decltype(t[0])>, Last_member, Pathway...>(editor[sel], t[sel], keys, type_tags<Path_traversed..., Path_element>{});
                }
            }
            else if constexpr ( is_path_member_v<Path_element> )
            {
                if constexpr ( RareTs::is_optional_v<std::remove_cvref_t<U>> )
                {
                    if constexpr ( sizeof...(Pathway) == 0 )
                    {
                        if ( t.has_value() )
                            return read_edit_pair{*((const U &)t), *editor};
                        else
                            return read_edit_pair{ref_null_opt, *editor};
                    }
                    else if ( t.has_value() )
                        return editor_from_path_impl<Keys, typename std::remove_cvref_t<U>::value_type, Last_member, Pathway...>(*editor, *t, keys, type_tags<Path_traversed..., Path_element>{});
                }
                else
                {
                    using member = RareTs::Member<U, Path_element::index>;
                    if constexpr ( sizeof...(Pathway) == 0 )
                        return read_edit_pair{member::value((const U &)t), editor.template from_member<Path_element::index>()};
                    else
                        return editor_from_path_impl<Keys, typename member::type, member, Pathway...>(editor.template from_member<Path_element::index>(), member::value(t), keys, type_tags<Path_traversed..., Path_element>{});
                }
            }
            else if constexpr ( is_path_index_v<Path_element> )
            {
                auto index = static_cast<std::size_t>(std::get<Path_element::index>(keys));
                if constexpr ( sizeof...(Pathway) == 0 )
                    return read_edit_pair{((const U &)t)[index], editor[index]};
                else
                {
                    return editor_from_path_impl<Keys, std::remove_cvref_t<decltype(t[index])>,
                        Last_member, Pathway...>(editor[index], t[index], keys, type_tags<Path_traversed..., Path_element>{});
                }
            }
            else
                static_assert(std::is_void_v<Path_element>, "Unexpected path element type!");
        }

        template <class ... Pathway, class Keys, class U>
        auto editor_from_path(auto & editor, U & t, Keys & keys)
        {
            if constexpr ( sizeof...(Pathway) == 0 )
                return read_edit_pair{(const U &)t, editor};
            else
                return editor_from_path_impl<Keys, U, void, Pathway...>(editor, t, keys, {});
        }

        constexpr void init_attached_data()
        {
            auto data_sizer = [&]<class ... Pathway>(type_tags<Pathway...>) {
                std::size_t data_size = 0;
                if constexpr ( has_attached_data<Pathway...>() )
                {
                    std::tuple<> keys {};
                    operate_on<Pathway...>(t, keys, [&data_size]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                        data_size = std::size(ref);
                    });
                }
                return data_size;
            };
            selections.template init_attached_data<>(data_sizer);
        }
        
        /// Creates an init event saving the existing data, does not make data change or send notifications
        /// @param keys the keys/array indexes of the path to the data being initialized
        template <class ... Pathway, class Keys>
        void record_initialization(Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::init));
            if constexpr ( sizeof...(Pathway) > 0 )
                serialize_pathway<Pathway...>(keys);
            else
                events.push_back(uint8_t(path_op::root_path));

            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serialize_value<Member>(ref);
            });
        }

        template <class ... Pathway, class Keys>
        void reset(Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::reset));
            serialize_pathway<Pathway...>(keys);

            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                using value_type = std::remove_cvref_t<decltype(ref)>;
                constexpr bool is_iterable = RareTs::is_iterable_v<value_type>;
                if constexpr ( is_iterable && has_element_removed_op<Route> ) // Iterable
                {
                    std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1;
                    serialize_value<Member>(ref);
                    for ( ; i>=0; --i )
                        notify_element_removed(user, Route{keys}, static_cast<std::size_t>(i)); // Issue remove changes
                    
                    ref = {};
                    if constexpr ( has_attached_data<Pathway...>() )
                        get_attached_data<Pathway...>() = {};
                }
                else if constexpr ( !is_iterable && has_value_changed_op<Route, value_type> ) // Non-iterables (also no selections)
                {
                    if constexpr ( std::is_array_v<typename Member::type> )
                    {
                        serialize_value<Member>(ref);
                        typename Member::type prev_value {};
                        auto ref_array = as_1d<typename Member::type>(ref);
                        as_1d(prev_value) = ref_array;
                        ref_array.clear();
                        notify_value_changed(user, Route{keys}, prev_value, ref); // Issue change notification
                    }
                    else
                    {
                        auto prev_value = ref;
                        serialize_value<Member>(ref);
                        ref = {};
                        notify_value_changed(user, Route{keys}, prev_value, ref); // Issue change notification
                    }
                }
                else
                {
                    serialize_value<Member>(ref);
                    if constexpr ( std::is_array_v<typename Member::type> )
                        as_1d<typename Member::type>(ref).clear();
                    else
                        ref = {};
                    
                    if constexpr ( has_attached_data<Pathway...>() )
                        get_attached_data<Pathway...>() = {};
                }
                    
                if constexpr ( has_selections<Pathway...>() )
                {
                    auto & sel = get_selections<Pathway...>();
                    write_selections(events, sel);
                    if constexpr ( has_selections_changed_op<Route> )
                    {
                        if ( !std::empty(sel) )
                        {
                            clear_sel(sel);
                            notify_selections_changed(user, Route{keys});
                        }
                    }
                    else
                        clear_sel(sel);
                }
            });
        }

        template <class ... Pathway, class Keys>
        void reserve(std::size_t size, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::reserve));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serialize_index<Member>(size);
                ref.reserve(size);
            });
        }

        template <class ... Pathway, class Keys>
        void trim(Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::trim));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                ref.shrink_to_fit();
            });
        }

        template <class ... Pathway, class Value, class Keys>
        void assign(std::size_t size, Value && value, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::assign));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serialize_index<Member>(size);
                serialize_value<Member>(value);
                serialize_value<Member>(ref);
                if constexpr ( has_element_removed_op<Route> )
                {
                    for ( std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1; i>=0; --i )
                        notify_element_removed(user, Route{keys}, static_cast<std::size_t>(i));
                }
                ref.assign(size, value);
                if constexpr ( has_attached_data<Pathway...>() )
                    get_attached_data<Pathway...>() = std::remove_cvref_t<decltype(get_attached_data<Pathway...>())>(size); // = std::vector(size) ctor

                if constexpr ( has_element_added_op<Route> )
                {
                    for ( std::size_t i=0; i<std::size(ref); ++i )
                        notify_element_added(user, Route{keys}, i);
                }

                if constexpr ( has_selections<Pathway...>() )
                {
                    auto & sel = get_selections<Pathway...>();
                    write_selections(events, sel);
                    if constexpr ( has_selections_changed_op<Route> )
                    {
                        if ( !std::empty(sel) )
                        {
                            clear_sel(sel);
                            notify_selections_changed(user, Route{keys});
                        }
                    }
                    else
                        clear_sel(sel);
                }
            });
        }

        template <class ... Pathway, class Keys>
        void assign_default(std::size_t size, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::assign_default));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serialize_index<Member>(size);
                serialize_value<Member>(ref);
                
                if constexpr ( has_element_removed_op<Route> )
                {
                    for ( std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1; i>=0; --i )
                        notify_element_removed(user, Route{keys}, static_cast<std::size_t>(i));
                }
                ref = std::remove_cvref_t<decltype(ref)>(size); // = std::vector(size) ctor
                if constexpr ( has_attached_data<Pathway...>() )
                    get_attached_data<Pathway...>() = std::remove_cvref_t<decltype(get_attached_data<Pathway...>())>(size); // = std::vector(size) ctor

                if constexpr ( has_element_added_op<Route> )
                {
                    for ( std::size_t i=0; i<std::size(ref); ++i )
                        notify_element_added(user, Route{keys}, i);
                }

                if constexpr ( has_selections<Pathway...>() )
                {
                    auto & sel = get_selections<Pathway...>();
                    write_selections(events, sel);
                    if constexpr ( has_selections_changed_op<Route> )
                    {
                        if ( !std::empty(sel) )
                        {
                            clear_sel(sel);
                            notify_selections_changed(user, Route{keys});
                        }
                    }
                    else
                        clear_sel(sel);
                }
            });
        }

        template <class ... Pathway, class Value, class Keys>
        void set(Value && value, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::set));
            if constexpr ( sizeof...(Pathway) > 0 )
                serialize_pathway<Pathway...>(keys);
            else
                events.push_back(uint8_t(path_op::root_path));

            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                using value_type = std::remove_cvref_t<decltype(ref)>;
                constexpr bool is_iterable = !RareTs::is_optional_v<value_type> && RareTs::is_iterable_v<value_type>;
                if constexpr ( !is_iterable && has_value_changed_op<Route, value_type> )
                {
                    auto prev_value = ref;
                    serialize_value<Member>(static_cast<std::remove_cvref_t<decltype(ref)>>(value)); // Value set to
                    serialize_value<Member>(ref); // Value before changing
                    ref = std::forward<Value>(value);
                    notify_value_changed(user, Route{keys}, prev_value, ref);
                }
                else
                {
                    serialize_value<Member>(static_cast<std::remove_cvref_t<decltype(ref)>>(value)); // Value set to
                    serialize_value<Member>(ref); // Value before changing
                        
                    if constexpr ( is_iterable && has_element_removed_op<Route> )
                    {
                        for ( std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1; i>=0; --i )
                            notify_element_removed(user, Route{keys}, static_cast<std::size_t>(i));
                    }
                    if constexpr ( requires{ref = std::forward<Value>(value);} )
                    {
                        ref = std::forward<Value>(value);
                        if constexpr ( has_attached_data<Pathway...>() )
                            get_attached_data<Pathway...>() = std::remove_cvref_t<decltype(get_attached_data<Pathway...>())>(std::size(ref)); // = std::vector(size)
                    }
                    if constexpr ( is_iterable && has_element_added_op<Route> )
                    {
                        for ( std::size_t i=0; i<std::size(ref); ++i )
                            notify_element_added(user, Route{keys}, i);
                    }
                }

                if constexpr ( has_selections<Pathway...>() )
                {
                    auto & sel = get_selections<Pathway...>();
                    write_selections(events, sel);
                    clear_sel(sel);

                    if constexpr ( has_selections_changed_op<Route> )
                        notify_selections_changed(user, Route{keys});
                }
            });
        }

        template <class ... Pathway, class Set_indexes, class Value, class Keys>
        void set_n(Set_indexes && set_indexes, const Value & value, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::set_n));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                using element_type = RareTs::element_type_t<std::remove_cvref_t<decltype(ref)>>;
                using index_type = index_type_t<default_index_type, Member>;
                using elem_path = type_tags<Pathway..., path_index<std::tuple_size_v<std::remove_cvref_t<decltype(keys)>>>>;
                using elem_keys = std::remove_cvref_t<decltype(std::tuple_cat(keys, std::tuple<index_type>{0}))>;
                using elem_route = path_tagged_keys<elem_keys, elem_path, Editor_type>;
                constexpr bool is_iterable_element = RareTs::is_iterable_v<element_type>;
                if constexpr ( !std::is_void_v<element_type> )
                {
                    if constexpr ( !is_iterable_element && has_value_changed_op<Route, element_type> )
                    {
                        serialize_index<Member>(std::size(set_indexes)); // Index count
                        for ( auto set_index : set_indexes )
                            serialize_index<Member>(set_index); // Indexes

                        serialize_value<Member>(value); // Value set to

                        for ( auto set_index : set_indexes )
                            serialize_value<Member>(ref[set_index]); // Value before changing

                        for ( auto set_index : set_indexes )
                        {
                            auto prev_value = ref[set_index];
                            ref[set_index] = value; // Make the change
                            notify_value_changed(user, elem_route{std::tuple_cat(keys, std::tuple<index_type>{set_index})}, prev_value, ref[set_index]);
                        }
                    }
                    else
                    {
                        serialize_index<Member>(std::size(set_indexes)); // Index count
                        for ( auto set_index : set_indexes )
                            serialize_index<Member>(set_index); // Indexes

                        serialize_value<Member>(value); // Value set to

                        for ( auto set_index : set_indexes )
                            serialize_value<Member>(ref[set_index]); // Value before changing

                        if constexpr ( is_iterable_element )
                        {
                            for ( auto set_index : set_indexes )
                            {
                                if constexpr ( has_element_removed_op<Route> )
                                {
                                    for ( std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref[set_index]))-1; i>=0; --i )
                                        notify_element_removed(user, elem_route{std::tuple_cat(keys, std::tuple<index_type>{set_index})}, static_cast<std::size_t>(i));
                                }
                                ref[set_index] = value; // Make the change
                                
                                if constexpr ( has_element_added_op<Route> )
                                {
                                    for ( std::size_t i=0; i<std::size(ref[set_index]); ++i )
                                        notify_element_added(user, elem_route{std::tuple_cat(keys, std::tuple<index_type>{set_index})}, i);
                                }
                            }
                        }
                        else if constexpr ( requires{ref[0] = value;} )
                        {
                            for ( auto set_index : set_indexes )
                                ref[set_index] = value; // Make the change
                        }
                    }
                }
            });
        }

        template <class ... Pathway, class Value, class Keys>
        void set_l(Value && value, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::set_l));
            serialize_pathway<Pathway...>(keys);

            bool first = true;
            operate_thru_sel<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>, auto & new_keys) {
                using value_type = std::remove_cvref_t<decltype(ref)>;
                constexpr bool is_iterable = RareTs::is_iterable_v<value_type>;
                if ( first )
                {
                    first = false;
                    if constexpr ( !is_iterable && has_value_changed_op<Route, value_type> )
                    {
                        auto prev_value = ref;
                        serialize_value<Member>(static_cast<std::remove_cvref_t<decltype(ref)>>(value)); // Value set to
                        serialize_value<Member>(ref); // Value before changing
                        ref = std::forward<Value>(value);
                        notify_value_changed(user, Route{new_keys}, prev_value, ref);
                    }
                    else
                    {
                        serialize_value<Member>(static_cast<std::remove_cvref_t<decltype(ref)>>(value)); // Value set to
                        serialize_value<Member>(ref); // Value before changing
                        if constexpr ( is_iterable && has_element_removed_op<Route> )
                        {
                            for ( std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1; i>=0; --i )
                                notify_element_removed(user, Route{new_keys}, static_cast<std::size_t>(i));
                        }
                        ref = std::forward<Value>(value);

                        if constexpr ( is_iterable && has_element_added_op<Route> )
                        {
                            for ( std::size_t i=0; i<std::size(ref); ++i )
                                notify_element_added(user, Route{new_keys}, i);
                        }
                    }
                }
                else if constexpr ( !is_iterable && has_value_changed_op<Route, value_type> )
                {
                    auto prev_value = ref;
                    serialize_value<Member>(ref); // Value before changing
                    ref = std::forward<Value>(value);
                    notify_value_changed(user, Route{new_keys}, prev_value, ref);
                }
                else
                {
                    serialize_value<Member>(ref); // Value before changing
                    if constexpr ( is_iterable && has_element_removed_op<Route> )
                    {
                        for ( std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1; i>=0; --i )
                            notify_element_removed(user, Route{new_keys}, static_cast<std::size_t>(i));
                    }
                    ref = std::forward<Value>(value);

                    if constexpr ( is_iterable && has_element_added_op<Route> )
                    {
                        for ( std::size_t i=0; i<std::size(ref); ++i )
                            notify_element_added(user, Route{new_keys}, i);
                    }
                }
            });
        }

        template <class ... Pathway, class Value, class Keys>
        void plus_eq_(Value && value, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::set)); // "Set like operation"
            serialize_pathway<Pathway...>(keys);

            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                
                using value_type = std::remove_cvref_t<decltype(ref)>;
                if constexpr ( has_value_changed_op<Route, value_type> )
                {
                    auto prev_value = ref;
                    ref += std::forward<Value>(value);
                    serialize_value<Member>(static_cast<std::remove_cvref_t<decltype(ref)>>(ref)); // Value set to
                    serialize_value<Member>(prev_value); // Value before changing
                    notify_value_changed(user, Route{keys}, prev_value, ref);
                }
                else
                {
                    serialize_value<Member>(std::remove_cvref_t<decltype(ref)>(ref + value)); // Value set to
                    serialize_value<Member>(ref); // Value before changing
                    ref += std::forward<Value>(value);
                }
            });
        }

        template <class ... Pathway, class Value, class Keys>
        void minus_eq_(Value && value, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::set)); // "Set like operation"
            serialize_pathway<Pathway...>(keys);

            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                
                using value_type = std::remove_cvref_t<decltype(ref)>;
                if constexpr ( has_value_changed_op<Route, value_type> )
                {
                    auto prev_value = ref;
                    ref -= std::forward<Value>(value);
                    serialize_value<Member>(static_cast<std::remove_cvref_t<decltype(ref)>>(ref)); // Value set to
                    serialize_value<Member>(prev_value); // Value before changing
                    notify_value_changed(user, Route{keys}, prev_value, ref);
                }
                else
                {
                    serialize_value<Member>(std::remove_cvref_t<decltype(ref)>(ref - value)); // Value set to
                    serialize_value<Member>(ref); // Value before changing
                    ref -= std::forward<Value>(value);
                }
            });
        }

        template <class ... Pathway, class Value, class Keys>
        void mult_eq_(Value && value, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::set)); // "Set like operation"
            serialize_pathway<Pathway...>(keys);

            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                
                using value_type = std::remove_cvref_t<decltype(ref)>;
                if constexpr ( has_value_changed_op<Route, value_type> )
                {
                    auto prev_value = ref;
                    ref *= std::forward<Value>(value);
                    serialize_value<Member>(static_cast<std::remove_cvref_t<decltype(ref)>>(ref)); // Value set to
                    serialize_value<Member>(prev_value); // Value before changing
                    notify_value_changed(user, Route{keys}, prev_value, ref);
                }
                else
                {
                    serialize_value<Member>(std::remove_cvref_t<decltype(ref)>(ref * value)); // Value set to
                    serialize_value<Member>(ref); // Value before changing
                    ref *= std::forward<Value>(value);
                }
            });
        }

        template <class ... Pathway, class Value, class Keys>
        void div_eq_(Value && value, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::set)); // "Set like operation"
            serialize_pathway<Pathway...>(keys);

            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                
                using value_type = std::remove_cvref_t<decltype(ref)>;
                if constexpr ( has_value_changed_op<Route, value_type> )
                {
                    auto prev_value = ref;
                    ref /= std::forward<Value>(value);
                    serialize_value<Member>(static_cast<std::remove_cvref_t<decltype(ref)>>(ref)); // Value set to
                    serialize_value<Member>(prev_value); // Value before changing
                    notify_value_changed(user, Route{keys}, prev_value, ref);
                }
                else
                {
                    serialize_value<Member>(std::remove_cvref_t<decltype(ref)>(ref / value)); // Value set to
                    serialize_value<Member>(ref); // Value before changing
                    ref /= std::forward<Value>(value);
                }
            });
        }

        template <class ... Pathway, class Value, class Keys>
        void mod_eq_(Value && value, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::set)); // "Set like operation"
            serialize_pathway<Pathway...>(keys);

            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                
                using value_type = std::remove_cvref_t<decltype(ref)>;
                if constexpr ( has_value_changed_op<Route, value_type> )
                {
                    auto prev_value = ref;
                    ref %= std::forward<Value>(value);
                    serialize_value<Member>(static_cast<std::remove_cvref_t<decltype(ref)>>(ref)); // Value set to
                    serialize_value<Member>(prev_value); // Value before changing
                    notify_value_changed(user, Route{keys}, prev_value, ref);
                }
                else
                {
                    serialize_value<Member>(std::remove_cvref_t<decltype(ref)>(ref % value)); // Value set to
                    serialize_value<Member>(ref); // Value before changing
                    ref %= std::forward<Value>(value);
                }
            });
        }

        template <class ... Pathway, class Value, class Keys>
        void xor_eq_(Value && value, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::set)); // "Set like operation"
            serialize_pathway<Pathway...>(keys);

            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                
                using value_type = std::remove_cvref_t<decltype(ref)>;
                if constexpr ( has_value_changed_op<Route, value_type> )
                {
                    auto prev_value = ref;
                    ref ^= std::forward<Value>(value);
                    serialize_value<Member>(static_cast<std::remove_cvref_t<decltype(ref)>>(ref)); // Value set to
                    serialize_value<Member>(prev_value); // Value before changing
                    notify_value_changed(user, Route{keys}, prev_value, ref);
                }
                else
                {
                    serialize_value<Member>(std::remove_cvref_t<decltype(ref)>(ref ^ value)); // Value set to
                    serialize_value<Member>(ref); // Value before changing
                    ref ^= std::forward<Value>(value);
                }
            });
        }

        template <class ... Pathway, class Value, class Keys>
        void and_eq_(Value && value, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::set)); // "Set like operation"
            serialize_pathway<Pathway...>(keys);

            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                
                using value_type = std::remove_cvref_t<decltype(ref)>;
                if constexpr ( has_value_changed_op<Route, value_type> )
                {
                    auto prev_value = ref;
                    ref &= std::forward<Value>(value);
                    serialize_value<Member>(static_cast<std::remove_cvref_t<decltype(ref)>>(ref)); // Value set to
                    serialize_value<Member>(prev_value); // Value before changing
                    notify_value_changed(user, Route{keys}, prev_value, ref);
                }
                else
                {
                    serialize_value<Member>(std::remove_cvref_t<decltype(ref)>(ref & value)); // Value set to
                    serialize_value<Member>(ref); // Value before changing
                    ref &= std::forward<Value>(value);
                }
            });
        }

        template <class ... Pathway, class Value, class Keys>
        void or_eq_(Value && value, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::set)); // "Set like operation"
            serialize_pathway<Pathway...>(keys);

            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                
                using value_type = std::remove_cvref_t<decltype(ref)>;
                if constexpr ( has_value_changed_op<Route, value_type> )
                {
                    auto prev_value = ref;
                    ref |= std::forward<Value>(value);
                    serialize_value<Member>(static_cast<std::remove_cvref_t<decltype(ref)>>(ref)); // Value set to
                    serialize_value<Member>(prev_value); // Value before changing
                    notify_value_changed(user, Route{keys}, prev_value, ref);
                }
                else
                {
                    serialize_value<Member>(std::remove_cvref_t<decltype(ref)>(ref | value)); // Value set to
                    serialize_value<Member>(ref); // Value before changing
                    ref |= std::forward<Value>(value);
                }
            });
        }

        template <class ... Pathway, class Value, class Keys>
        void append(Value && value, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::append));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                ref.push_back(std::forward<Value>(value));
                if constexpr ( has_attached_data<Pathway...>() )
                    get_attached_data<Pathway...>().push_back({});
                serialize_value<Member>(ref.back());
                if constexpr ( has_element_added_op<Route> )
                    notify_element_added(user, Route{keys}, ref.size()-1);
            });
        }

        template <class ... Pathway, class Values, class Keys>
        void append_n(Values && values, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::append_n));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serialize_index<Member>(std::size(values));
                for ( auto & value : values )
                {
                    ref.push_back(value);
                    if constexpr ( has_attached_data<Pathway...>() )
                        get_attached_data<Pathway...>().push_back({});

                    serialize_value<Member>(ref.back());
                    if constexpr ( has_element_added_op<Route> )
                        notify_element_added(user, Route{keys}, ref.size()-1);
                }
            });
        }

        template <class ... Pathway, class Insertion_index, class Value, class Keys>
        void insert(Insertion_index insertion_index, Value && value, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::insert));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serialize_index<Member>(insertion_index);
                serialize_value<Member>(value);
                ref.insert(std::next(ref.begin(), static_cast<std::ptrdiff_t>(insertion_index)), std::forward<Value>(value));
                if constexpr ( has_attached_data<Pathway...>() )
                {
                    auto & attached_data = get_attached_data<Pathway...>();
                    attached_data.emplace(std::next(attached_data.begin(), static_cast<std::ptrdiff_t>(insertion_index)));
                }
                if constexpr ( has_element_added_op<Route> )
                    notify_element_added(user, Route{keys}, static_cast<std::size_t>(insertion_index));
                if constexpr ( has_element_moved_op<Route> )
                {
                    for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(std::size(ref))-1; i>static_cast<std::ptrdiff_t>(insertion_index); --i )
                        notify_element_moved(user, Route{keys}, static_cast<std::size_t>(i-1), static_cast<std::size_t>(i));
                }

                if constexpr ( has_selections<Pathway...>() )
                {
                    auto & sel = get_selections<Pathway...>();
                    for ( auto & index : sel )
                    {
                        if ( static_cast<Insertion_index>(index) >= insertion_index )
                            ++index;
                    }
                    if constexpr ( has_selections_changed_op<Route> )
                        notify_selections_changed(user, Route{keys});
                }
            });
        }

        template <class ... Pathway, class Insertion_index, class Values, class Keys>
        void insert_n(Insertion_index insertion_index, Values && values, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::insert_n));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serialize_index<Member>(std::size(values));
                serialize_index<Member>(insertion_index);
                for ( auto & value : values )
                    serialize_value<Member>(value);
                
                ref.insert(std::next(ref.begin(), static_cast<std::ptrdiff_t>(insertion_index)), std::begin(values), std::end(values));
                if constexpr ( has_attached_data<Pathway...>() )
                {
                    auto & attached_data = get_attached_data<Pathway...>();
                    for ( std::size_t i=0; i<std::size(values); ++i )
                        attached_data.emplace(std::next(attached_data.begin(), static_cast<std::ptrdiff_t>(insertion_index)));
                }
                if constexpr ( has_element_added_op<Route> )
                {
                    auto limit = insertion_index + static_cast<Insertion_index>(std::size(values));
                    for ( auto i = insertion_index; i < limit; ++i )
                        notify_element_added(user, Route{keys}, static_cast<std::size_t>(i));
                }
                if constexpr ( has_element_moved_op<Route> )
                {
                    std::size_t count_inserted = std::size(values);
                    std::size_t prev_size = std::size(ref) - count_inserted;
                    for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(prev_size)-1; i>=static_cast<std::ptrdiff_t>(insertion_index); --i )
                        notify_element_moved(user, Route{keys}, static_cast<std::size_t>(i), static_cast<std::size_t>(i)+count_inserted);
                }

                if constexpr ( has_selections<Pathway...>() )
                {
                    auto & sel = get_selections<Pathway...>();
                    for ( auto & index : sel )
                    {
                        std::size_t count = values.size();
                        if ( static_cast<Insertion_index>(index) >= insertion_index )
                            index += count;
                    }
                    if constexpr ( has_selections_changed_op<Route> )
                        notify_selections_changed(user, Route{keys});
                }
            });
        }

        template <class ... Pathway, class Removal_index, class Keys>
        void remove(Removal_index removal_index, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::remove));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                
                using index_type = index_type_t<default_index_type, Member>;
                serialize_index<Member>(removal_index);
                serialize_value<Member>(ref[static_cast<std::size_t>(removal_index)]);
                
                if constexpr ( has_element_removed_op<Route> )
                    notify_element_removed(user, Route{keys}, static_cast<std::size_t>(removal_index));

                ref.erase(std::next(ref.begin(), static_cast<std::ptrdiff_t>(removal_index)));
                if constexpr ( has_attached_data<Pathway...>() )
                {
                    auto & attached_data = get_attached_data<Pathway...>();
                    attached_data.erase(std::next(attached_data.begin(), static_cast<std::ptrdiff_t>(removal_index)));
                }
                if constexpr ( has_element_moved_op<Route> )
                {
                    for ( std::size_t i=static_cast<std::size_t>(removal_index); i<std::size(ref); ++i )
                        notify_element_moved(user, Route{keys}, i+1, i);
                }

                auto & sel = get_selections<Pathway...>();
                auto found = sel.end();
                auto index_removed = static_cast<index_type>(removal_index);
                for ( auto it = sel.begin(); it != sel.end(); ++it )
                {
                    if ( *it == index_removed )
                        found = it;
                    else if ( *it > index_removed )
                        --(*it);
                }

                u8bool::write(events, found != sel.end());
                if ( found != sel.end() )
                {
                    index_type found_at = static_cast<index_type>(std::distance(sel.begin(), found));
                    serialize_index<Member>(found_at);
                    sel.erase(found);
                }
                if constexpr ( has_selections_changed_op<Route> )
                    notify_selections_changed(user, Route{keys});
            });
        }

        template <class ... Pathway, class Removal_indexes, class Keys>
        void remove_n(Removal_indexes && removal_indexes, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::remove_n));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {

                using index_type = index_type_t<default_index_type, Member>;
                serialize_index<Member>(std::size(removal_indexes));
                // Sort indexes so largest indexes come first/are removed first
                std::sort(removal_indexes.begin(), removal_indexes.end(), std::greater<RareTs::element_type_t<std::remove_cvref_t<Removal_indexes>>>{});
                for ( auto removal_index : removal_indexes )
                    serialize_index<Member>(removal_index);
                for ( auto removal_index : removal_indexes )
                    serialize_value<Member>(ref[static_cast<std::size_t>(removal_index)]);

                if constexpr ( has_element_removed_op<Route> )
                {
                    for ( auto removal_index : removal_indexes )
                        notify_element_removed(user, Route{keys}, static_cast<std::size_t>(removal_index));
                }

                if constexpr ( has_attached_data<Pathway...>() )
                {
                    auto & attached_data = get_attached_data<Pathway...>();
                    for ( auto removal_index : removal_indexes )
                    {
                        ref.erase(std::next(ref.begin(), static_cast<std::ptrdiff_t>(removal_index)));
                        attached_data.erase(std::next(attached_data.begin(), static_cast<std::ptrdiff_t>(removal_index)));
                    }
                }
                else
                {
                    for ( auto removal_index : removal_indexes )
                        ref.erase(std::next(ref.begin(), static_cast<std::ptrdiff_t>(removal_index)));
                }

                if constexpr ( has_element_moved_op<Route> )
                {
                    if ( !ref.empty() )
                    {
                        std::size_t size_removed = std::size(removal_indexes);
                        std::size_t collection_index = static_cast<std::size_t>(removal_indexes[size_removed-1]);
                        std::size_t move_distance = 1;
                        for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(size_removed)-1; i>0; --i )
                        {
                            for ( std::size_t j=static_cast<std::size_t>(removal_indexes[static_cast<std::size_t>(i)]);
                                j<static_cast<std::size_t>(removal_indexes[static_cast<std::size_t>(i-1)]-1); ++j )
                            {
                                notify_element_moved(user, Route{keys}, collection_index+move_distance, collection_index);
                                ++collection_index;
                            }
                            ++move_distance;
                        }
                        for ( ; collection_index < std::size(ref); ++collection_index )
                            notify_element_moved(user, Route{keys}, collection_index+move_distance, collection_index);
                    }
                }

                if constexpr ( has_selections<Pathway...>() )
                {
                    auto & sel = get_selections<Pathway...>();
                    std::vector<bool> removal_index_selected(std::size(removal_indexes), false);
                    std::vector<index_type> prev_sel_indexes {};
                    prev_sel_indexes.reserve(std::size(removal_indexes));
                    for ( std::size_t i=0; i<removal_indexes.size(); ++i )
                    {
                        auto index_removed = removal_indexes[i];
                        auto found = sel.end();
                        for ( auto it = sel.begin(); it != sel.end(); ++it )
                        {
                            if ( *it == static_cast<index_type>(index_removed) )
                                found = it;
                            else if ( *it > static_cast<index_type>(index_removed) )
                                --(*it);
                        }
                        if ( found != sel.end() )
                        {
                            removal_index_selected[i] = true;
                            prev_sel_indexes.push_back(static_cast<index_type>(std::distance(sel.begin(), found)));
                            sel.erase(found);
                        }
                    }
                    if constexpr ( has_selections_changed_op<Route> )
                        notify_selections_changed(user, Route{keys});

                    write_vec_bool_data(events, removal_index_selected);
                    serialize_index<Member>(static_cast<index_type>(prev_sel_indexes.size()));
                    if ( !prev_sel_indexes.empty() )
                    {
                        events.insert(events.end(), reinterpret_cast<const std::uint8_t*>(&prev_sel_indexes[0]),
                            reinterpret_cast<const std::uint8_t*>(&prev_sel_indexes[0])+std::size(prev_sel_indexes)*sizeof(prev_sel_indexes[0]));
                    }
                }
            });
        }

        template <class ... Pathway, class Keys>
        void remove_l(Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::remove_l));
            serialize_pathway<Pathway...>(keys);
            auto & sel = get_selections<Pathway...>();
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                auto selections = sel;
                std::sort(selections.begin(), selections.end(), std::greater<RareTs::element_type_t<std::remove_cvref_t<decltype(selections)>>>{});
                serialize_index<Member>(std::size(selections));
                for ( auto i : selections )
                    serialize_index<Member>(i);
                for ( auto i : selections )
                {
                    if constexpr ( has_element_removed_op<Route> )
                    {
                        serialize_value<Member>(ref[i]);
                        notify_element_removed(user, Route{keys}, static_cast<std::size_t>(i));
                        ref.erase(std::next(ref.begin(), static_cast<std::ptrdiff_t>(i)));
                        if constexpr ( has_attached_data<Pathway...>() )
                        {
                            auto & attached_data = get_attached_data<Pathway...>();
                            attached_data.erase(std::next(attached_data.begin(), static_cast<std::ptrdiff_t>(i)));
                        }
                    }
                    else
                    {
                        serialize_value<Member>(ref[i]);
                        ref.erase(std::next(ref.begin(), static_cast<std::ptrdiff_t>(i)));
                        if constexpr ( has_attached_data<Pathway...>() )
                        {
                            auto & attached_data = get_attached_data<Pathway...>();
                            attached_data.erase(std::next(attached_data.begin(), static_cast<std::ptrdiff_t>(i)));
                        }
                    }
                }
                if constexpr ( has_element_moved_op<Route> )
                {
                    if ( !ref.empty() )
                    {
                        std::size_t size_removed = std::size(selections);
                        std::size_t collection_index = static_cast<std::size_t>(selections[size_removed-1]);
                        std::size_t move_distance = 1;
                        for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(size_removed)-1; i>0; --i )
                        {
                            for ( std::size_t j=selections[static_cast<std::size_t>(i)]; j<selections[static_cast<std::size_t>(i-1)]-1; ++j )
                            {
                                notify_element_moved(user, Route{keys}, collection_index + move_distance, collection_index);
                                ++collection_index;
                            }
                            ++move_distance;
                        }
                        for ( ; collection_index < std::size(ref); ++collection_index )
                            notify_element_moved(user, Route{keys}, collection_index+move_distance, collection_index);
                    }
                }
                write_selections(events, sel);
                clear_sel(sel);
                if constexpr ( has_selections_changed_op<Route> )
                    notify_selections_changed(user, Route{keys});
            });
        }

        template <class ... Pathway, class Keys>
        void sort(Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::sort));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                using index_type = index_type_t<default_index_type, Member>;
                auto source_indexes = tracked_sort<false, index_type>(ref);
                if constexpr ( has_attached_data<Pathway...>() )
                    redo_sort(get_attached_data<Pathway...>(), std::span(source_indexes));

                index_type serialized_size = static_cast<index_type>(source_indexes.size());
                serialize_index<Member>(serialized_size);
                for ( auto index : source_indexes )
                {
                    index_type source_index = static_cast<index_type>(index);
                    serialize_index<Member>(source_index);
                }
                auto count = std::size(source_indexes);
                auto & sel = get_selections<Pathway...>();
                auto new_sel = std::remove_cvref_t<decltype(sel)>{};
                for ( std::size_t i=0; i<count; ++i )
                {
                    if ( std::find(sel.begin(), sel.end(), source_indexes[i]) != sel.end() )
                        RareTs::append(new_sel, static_cast<index_type>(i));
                }
                std::swap(sel, new_sel);
                if constexpr ( has_element_moved_op<Route> )
                {
                    for ( std::size_t i=0; i<count; ++i )
                    {
                        if ( i != source_indexes[i] )
                            notify_element_moved(user, Route{keys}, static_cast<std::size_t>(source_indexes[i]), static_cast<std::size_t>(i));
                    }
                }
                if constexpr ( has_selections_changed_op<Route> )
                    notify_selections_changed(user, Route{keys});
            });
        }

        template <class ... Pathway, class Keys>
        void sort_desc(Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::sort_desc));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                using index_type = index_type_t<default_index_type, Member>;
                auto source_indexes = tracked_sort<true, index_type>(ref);
                if constexpr ( has_attached_data<Pathway...>() )
                    redo_sort(get_attached_data<Pathway...>(), std::span(source_indexes));

                index_type serialized_size = static_cast<index_type>(source_indexes.size());
                serialize_index<Member>(serialized_size);
                for ( auto index : source_indexes )
                {
                    index_type source_index = static_cast<index_type>(index);
                    serialize_index<Member>(source_index);
                }
                auto count = std::size(source_indexes);
                auto & sel = get_selections<Pathway...>();
                auto new_sel = std::remove_cvref_t<decltype(sel)>{};
                for ( std::size_t i=0; i<count; ++i )
                {
                    if ( std::find(sel.begin(), sel.end(), source_indexes[i]) != sel.end() )
                        RareTs::append(new_sel, i);
                }
                std::swap(sel, new_sel);
                if constexpr ( has_element_moved_op<Route> )
                {
                    auto limit = std::size(source_indexes);
                    for ( std::size_t i=0; i<limit; ++i )
                    {
                        if ( i != source_indexes[i] )
                            notify_element_moved(user, Route{keys}, static_cast<std::size_t>(source_indexes[i]), static_cast<std::size_t>(i));
                    }
                }
                if constexpr ( has_selections_changed_op<Route> )
                    notify_selections_changed(user, Route{keys});
            });
        }

        template <class ... Pathway, class Moved_index, class Keys>
        void swap(Moved_index first_index, Moved_index second_index, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::swap));
            serialize_pathway<Pathway...>(keys);

            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serialize_index<Member>(first_index);
                serialize_index<Member>(second_index);
                if ( first_index != second_index && static_cast<std::size_t>(first_index) < std::size(ref) && static_cast<std::size_t>(second_index) < std::size(ref) )
                {
                    std::swap(ref[static_cast<std::size_t>(first_index)], ref[static_cast<std::size_t>(second_index)]);
                    if constexpr ( has_attached_data<Pathway...>() )
                        mirror_swap_to(get_attached_data<Pathway...>(), static_cast<std::size_t>(first_index), static_cast<std::size_t>(second_index));

                    mirror_swap_to_selection(get_selections<Pathway...>(), static_cast<std::size_t>(first_index), static_cast<std::size_t>(second_index));
                    if constexpr ( has_element_moved_op<Route> )
                    {
                        notify_element_moved(user, Route{keys}, static_cast<std::size_t>(first_index), static_cast<std::size_t>(second_index));
                        notify_element_moved(user, Route{keys}, static_cast<std::size_t>(second_index), static_cast<std::size_t>(first_index));
                    }
                    if constexpr ( has_selections<Pathway...>() && has_selections_changed_op<Route> )
                        notify_selections_changed(user, Route{keys});
                }
            });
        }

        template <class ... Pathway, class Moved_index, class Keys>
        void move_up(Moved_index moved_index, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::move_up));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serialize_index<Member>(moved_index);
                if ( moved_index > 0 && static_cast<std::size_t>(moved_index) < std::size(ref) )
                {
                    std::swap(ref[static_cast<std::size_t>(moved_index)], ref[static_cast<std::size_t>(moved_index-1)]);
                    if constexpr ( has_attached_data<Pathway...>() )
                        mirror_swap_to(get_attached_data<Pathway...>(), static_cast<std::size_t>(moved_index), static_cast<std::size_t>(moved_index)-1);

                    mirror_swap_to_selection(get_selections<Pathway...>(), static_cast<std::size_t>(moved_index), static_cast<std::size_t>(moved_index)-1);
                    if constexpr ( has_element_moved_op<Route> )
                    {
                        notify_element_moved(user, Route{keys}, static_cast<std::size_t>(moved_index), static_cast<std::size_t>(moved_index)-1);
                        notify_element_moved(user, Route{keys}, static_cast<std::size_t>(moved_index)-1, static_cast<std::size_t>(moved_index));
                    }
                    if constexpr ( has_selections<Pathway...>() && has_selections_changed_op<Route> )
                        notify_selections_changed(user, Route{keys});
                }
            });
        }

        template <class ... Pathway, class Moved_indexes, class Keys>
        void move_up_n(Moved_indexes && moved_indexes, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::move_up_n));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                std::sort(moved_indexes.begin(), moved_indexes.end());

                serialize_index<Member>(std::size(moved_indexes));
                for ( auto index : moved_indexes )
                    serialize_index<Member>(index);

                std::size_t next_available = 0;
                for ( auto moved_index : moved_indexes )
                {
                    if ( moved_index > next_available && static_cast<std::size_t>(moved_index) < std::size(ref) )
                    {
                        std::swap(ref[static_cast<std::size_t>(moved_index)], ref[static_cast<std::size_t>(moved_index-1)]);
                        if constexpr ( has_attached_data<Pathway...>() )
                            mirror_swap_to(get_attached_data<Pathway...>(), moved_index, moved_index-1);

                        mirror_swap_to_selection(get_selections<Pathway...>(), moved_index, moved_index-1);
                        next_available = moved_index;
                    }
                    else if ( moved_index == next_available )
                        ++next_available;
                }

                if constexpr ( has_element_moved_op<Route> )
                {
                    next_available = 0;
                    std::size_t block_size = 1;
                    for ( std::size_t i=0; i<std::size(moved_indexes); ++i )
                    {
                        if ( moved_indexes[i] > next_available && static_cast<std::size_t>(moved_indexes[i]) < std::size(ref) )
                        {
                            if ( i+1 < std::size(moved_indexes) && moved_indexes[i]+1 == moved_indexes[i+1] )
                                ++block_size;
                            else
                            {
                                notify_element_moved(user, Route{keys}, moved_indexes[i]-block_size, moved_indexes[i]);
                                block_size = 1;
                            }
                            notify_element_moved(user, Route{keys}, moved_indexes[i], moved_indexes[i]-1);
                        }
                        else if ( moved_indexes[i] == next_available )
                            ++next_available;
                    }
                }
                if constexpr ( has_selections_changed_op<Route> )
                    notify_selections_changed(user, Route{keys});
            });
        }

        template <class ... Pathway, class Keys>
        void move_up_l(Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::move_up_l));
            serialize_pathway<Pathway...>(keys); 
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                auto & sel = get_selections<Pathway...>();
                write_selections(events, sel);
                auto moved_indexes = sel; // Copy so it can be sorted without changing selections
                std::sort(moved_indexes.begin(), moved_indexes.end());
                
                std::size_t next_available = 0;
                for ( auto moved_index : moved_indexes )
                {
                    if ( moved_index > next_available && static_cast<std::size_t>(moved_index) < std::size(ref) )
                    {
                        std::swap(ref[static_cast<std::size_t>(moved_index)], ref[static_cast<std::size_t>(moved_index-1)]);
                        if constexpr ( has_attached_data<Pathway...>() )
                            mirror_swap_to(get_attached_data<Pathway...>(), moved_index, moved_index-1);

                        mirror_swap_to_selection(get_selections<Pathway...>(), moved_index, moved_index-1);
                        next_available = moved_index;
                    }
                    else if ( moved_index == next_available )
                        ++next_available;
                }

                if constexpr ( has_element_moved_op<Route> )
                {
                    next_available = 0;
                    std::size_t block_size = 1;
                    for ( std::size_t i=0; i<std::size(moved_indexes); ++i )
                    {
                        if ( moved_indexes[i] > next_available && static_cast<std::size_t>(moved_indexes[i]) < std::size(ref) )
                        {
                            if ( i+1 < std::size(moved_indexes) && moved_indexes[i]+1 == moved_indexes[i+1] )
                                ++block_size;
                            else
                            {
                                notify_element_moved(user, Route{keys}, moved_indexes[i]-block_size, moved_indexes[i]);
                                block_size = 1;
                            }
                            notify_element_moved(user, Route{keys}, moved_indexes[i], moved_indexes[i]-1);
                        }
                        else if ( moved_indexes[i] == next_available )
                            ++next_available;
                    }
                }
                if constexpr ( has_selections_changed_op<Route> )
                    notify_selections_changed(user, Route{keys});
            });
        }

        template <class ... Pathway, class Moved_index, class Keys>
        void move_top(Moved_index moved_index, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::move_top));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serialize_index<Member>(moved_index);
                auto size = std::size(ref);
                if ( moved_index > 0 && static_cast<std::size_t>(moved_index) < size )
                {
                    auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(moved_index));
                    std::rotate(ref.begin(), it, it+1); // [0, moved_index) are moved forward 1... [moved_index] is moved to 0
                    if constexpr ( has_attached_data<Pathway...>() )
                        mirror_rotation_to(get_attached_data<Pathway...>(), 0, static_cast<std::size_t>(moved_index), static_cast<std::size_t>(moved_index)+1);

                    mirror_rotation_to_selection(get_selections<Pathway...>(), 0, static_cast<std::size_t>(moved_index), static_cast<std::size_t>(moved_index)+1);
                    if constexpr ( has_element_moved_op<Route> )
                    {
                        notify_element_moved(user, Route{keys}, static_cast<std::size_t>(moved_index), 0);
                        for ( std::size_t i=0; i<static_cast<std::size_t>(moved_index); ++i )
                            notify_element_moved(user, Route{keys}, i, i+1);
                    }
                    if constexpr ( has_selections<Pathway...>() && has_selections_changed_op<Route> )
                        notify_selections_changed(user, Route{keys});
                }
            });
        }

        template <class ... Pathway, class Moved_indexes, class Keys>
        void move_top_n(Moved_indexes && moved_indexes, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::move_top_n));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                std::sort(moved_indexes.begin(), moved_indexes.end()); // lowest to highest
                serialize_index<Member>(std::size(moved_indexes));
                for ( auto i : moved_indexes )
                    serialize_index<Member>(i);

                write_selections(events, get_selections<Pathway...>());
                
                std::size_t size = std::size(ref);
                std::vector<std::size_t> tracked_indexes {};
                if constexpr ( has_element_moved_op<Route> )
                {
                    tracked_indexes.assign(size, 0);
                    std::iota(tracked_indexes.begin(), tracked_indexes.end(), 0);
                }

                std::ptrdiff_t insertion_index = 0;
                for ( auto i : moved_indexes )
                {
                    if ( static_cast<std::ptrdiff_t>(i) > insertion_index && static_cast<std::size_t>(i) < size )
                    {
                        auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                        std::rotate(std::next(ref.begin(), insertion_index), it, it+1);
                        if constexpr ( has_attached_data<Pathway...>() )
                            mirror_rotation_to(get_attached_data<Pathway...>(), static_cast<std::size_t>(insertion_index), static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1);

                        mirror_rotation_to_selection(get_selections<Pathway...>(), static_cast<std::size_t>(insertion_index), static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1);
                        if constexpr ( has_element_moved_op<Route> )
                            mirror_rotation_to(tracked_indexes, insertion_index, i, i+1);
                    }
                    ++insertion_index;
                }

                if constexpr ( has_element_moved_op<Route> )
                {
                    for ( std::size_t i=0; i<size; ++i )
                    {
                        if ( i != tracked_indexes[i] )
                            notify_element_moved(user, Route{keys}, tracked_indexes[i], i);
                    }
                }
                if constexpr ( has_selections_changed_op<Route> )
                    notify_selections_changed(user, Route{keys});
            });
        }

        template <class ... Pathway, class Keys>
        void move_top_l(Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::move_top_l));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                auto & sel = get_selections<Pathway...>();
                write_selections(events, sel);
                auto moved_indexes = sel; // Copy so it can be sorted without changing selections
                std::sort(moved_indexes.begin(), moved_indexes.end());

                std::size_t size = std::size(ref);
                std::vector<std::size_t> tracked_indexes {};
                if constexpr ( has_element_moved_op<Route> )
                {
                    tracked_indexes.assign(size, 0);
                    std::iota(tracked_indexes.begin(), tracked_indexes.end(), 0);
                }

                std::ptrdiff_t insertion_index = 0;
                for ( auto i : moved_indexes )
                {
                    if ( static_cast<std::ptrdiff_t>(i) > insertion_index && static_cast<std::size_t>(i) < size )
                    {
                        auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                        std::rotate(std::next(ref.begin(), insertion_index), it, it+1);
                        if constexpr ( has_attached_data<Pathway...>() )
                            mirror_rotation_to(get_attached_data<Pathway...>(), static_cast<std::size_t>(insertion_index), static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1);

                        mirror_rotation_to_selection(get_selections<Pathway...>(), static_cast<std::size_t>(insertion_index), static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1);
                        if constexpr ( has_element_moved_op<Route> )
                            mirror_rotation_to(tracked_indexes, insertion_index, i, i+1);
                    }
                    ++insertion_index;
                }

                if constexpr ( has_element_moved_op<Route> )
                {
                    for ( std::size_t i=0; i<size; ++i )
                    {
                        if ( i != tracked_indexes[i] )
                            notify_element_moved(user, Route{keys}, tracked_indexes[i], i);
                    }
                }
                if constexpr ( has_selections_changed_op<Route> )
                    notify_selections_changed(user, Route{keys});
            });
        }

        template <class ... Pathway, class Moved_index, class Keys>
        void move_down(Moved_index moved_index, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::move_down));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serialize_index<Member>(moved_index);
                if ( static_cast<std::size_t>(moved_index)+1 < std::size(ref) )
                {
                    std::swap(ref[static_cast<std::size_t>(moved_index)], ref[static_cast<std::size_t>(moved_index)+1]);
                    if constexpr ( has_attached_data<Pathway...>() )
                        mirror_swap_to(get_attached_data<Pathway...>(), static_cast<std::size_t>(moved_index), static_cast<std::size_t>(moved_index)+1);

                    mirror_swap_to_selection(get_selections<Pathway...>(), static_cast<std::size_t>(moved_index), static_cast<std::size_t>(moved_index)+1);
                    if constexpr ( has_element_moved_op<Route> )
                    {
                        notify_element_moved(user, Route{keys}, static_cast<std::size_t>(moved_index), static_cast<std::size_t>(moved_index)+1);
                        notify_element_moved(user, Route{keys}, static_cast<std::size_t>(moved_index)+1, static_cast<std::size_t>(moved_index));
                    }
                    if constexpr ( has_selections<Pathway...>() && has_selections_changed_op<Route> )
                        notify_selections_changed(user, Route{keys});
                }
            });
        }

        template <class ... Pathway, class Moved_indexes, class Keys>
        void move_down_n(Moved_indexes && moved_indexes, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::move_down_n));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                std::sort(moved_indexes.begin(), moved_indexes.end(), std::greater<RareTs::element_type_t<std::remove_cvref_t<Moved_indexes>>>{});
                serialize_index<Member>(std::size(moved_indexes));
                for ( auto i : moved_indexes )
                    serialize_index<Member>(i);

                auto size = std::size(ref);
                if ( size > 0 )
                {
                    auto limit = size;
                    for ( auto moved_index : moved_indexes )
                    {
                        if ( static_cast<std::size_t>(moved_index)+1 < limit )
                        {
                            std::swap(ref[static_cast<std::size_t>(moved_index)], ref[static_cast<std::size_t>(moved_index)+1]);
                            if constexpr ( has_attached_data<Pathway...>() )
                                mirror_swap_to(get_attached_data<Pathway...>(), moved_index, moved_index+1);

                            mirror_swap_to_selection(get_selections<Pathway...>(), moved_index, moved_index+1);
                            limit = moved_index+1;
                        }
                        else if ( limit > 0 )
                            --limit;
                    }

                    if constexpr ( has_element_moved_op<Route> )
                    {
                        limit = size;
                        std::size_t count = std::size(moved_indexes);
                        std::size_t block_size = 1;
                        for ( std::size_t i=0; i<count; ++i )
                        {
                            if ( static_cast<std::size_t>(moved_indexes[i])+1 < limit )
                            {
                                if ( i+1 < count && moved_indexes[i]-1 == moved_indexes[i+1] )
                                    ++block_size;
                                else
                                {
                                    notify_element_moved(user, Route{keys}, moved_indexes[i]+block_size, moved_indexes[i]);
                                    block_size = 1;
                                }
                                notify_element_moved(user, Route{keys}, moved_indexes[i], moved_indexes[i]+1);
                                limit = moved_indexes[i]+1;
                            }
                            else if ( limit > 0 )
                                --limit;
                        }
                    }
                    if constexpr ( has_selections_changed_op<Route> )
                        notify_selections_changed(user, Route{keys});
                }
            });
        }

        template <class ... Pathway, class Keys>
        void move_down_l(Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::move_down_l));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                using index_type = index_type_t<default_index_type, Member>;
                auto & sel = get_selections<Pathway...>();
                write_selections(events, sel);
                auto moved_indexes = sel; // Copy so it can be sorted without changing selections
                std::sort(moved_indexes.begin(), moved_indexes.end(), std::greater<index_type>{});

                auto size = std::size(ref);
                if ( size > 0 )
                {
                    auto limit = size;
                    for ( auto moved_index : moved_indexes )
                    {
                        if ( static_cast<std::size_t>(moved_index)+1 < limit )
                        {
                            std::swap(ref[static_cast<std::size_t>(moved_index)], ref[static_cast<std::size_t>(moved_index)+1]);
                            if constexpr ( has_attached_data<Pathway...>() )
                                mirror_swap_to(get_attached_data<Pathway...>(), moved_index, moved_index+1);

                            mirror_swap_to_selection(get_selections<Pathway...>(), moved_index, moved_index+1);
                        }
                        else if ( limit > 0 )
                            --limit;
                    }

                    if constexpr ( has_element_moved_op<Route> )
                    {
                        limit = size;
                        std::size_t count = std::size(moved_indexes);
                        std::size_t block_size = 1;
                        for ( std::size_t i=0; i<count; ++i )
                        {
                            if ( static_cast<std::size_t>(moved_indexes[i])+1 < limit )
                            {
                                if ( i+1 < count && moved_indexes[i]-1 == moved_indexes[i+1] )
                                    ++block_size;
                                else
                                {
                                    notify_element_moved(user, Route{keys}, moved_indexes[i]+block_size, moved_indexes[i]);
                                    block_size = 1;
                                }
                                notify_element_moved(user, Route{keys}, moved_indexes[i], moved_indexes[i]+1);
                                limit = moved_indexes[i]+1;
                            }
                            else if ( limit > 0 )
                                --limit;
                        }
                    }
                    if constexpr ( has_selections_changed_op<Route> )
                        notify_selections_changed(user, Route{keys});
                }

            });
        }

        template <class ... Pathway, class Moved_index, class Keys>
        void move_bottom(Moved_index moved_index, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::move_bottom));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serialize_index<Member>(moved_index);
                if ( moved_index >= 0 && static_cast<std::size_t>(moved_index)+1 < std::size(ref) )
                {
                    auto it = std::next(ref.begin(), moved_index);
                    std::rotate(it, it+1, ref.end());
                    if constexpr ( has_attached_data<Pathway...>() )
                        mirror_rotation_to(get_attached_data<Pathway...>(), static_cast<std::size_t>(moved_index), static_cast<std::size_t>(moved_index)+1, std::size(ref));

                    mirror_rotation_to_selection(get_selections<Pathway...>(), static_cast<std::size_t>(moved_index), static_cast<std::size_t>(moved_index)+1, std::size(ref));
                    if constexpr ( has_element_moved_op<Route> )
                    {
                        notify_element_moved(user, Route{keys}, static_cast<std::size_t>(moved_index), std::size(ref)-1);
                        for ( std::size_t i=static_cast<std::size_t>(moved_index)+1; i<std::size(ref); ++i )
                            notify_element_moved(user, Route{keys}, i, i-1);
                    }
                    if constexpr ( has_selections<Pathway...>() && has_selections_changed_op<Route> )
                        notify_selections_changed(user, Route{keys});
                }
            });
        }

        template <class ... Pathway, class Moved_indexes, class Keys>
        void move_bottom_n(Moved_indexes && moved_indexes, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::move_bottom_n));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                std::sort(moved_indexes.begin(), moved_indexes.end(), std::greater<RareTs::element_type_t<std::remove_cvref_t<Moved_indexes>>>{});
                serialize_index<Member>(std::size(moved_indexes));
                for ( auto i : moved_indexes )
                    serialize_index<Member>(i);
                
                write_selections(events, get_selections<Pathway...>());

                std::size_t size = std::size(ref);
                std::vector<std::size_t> tracked_indexes {};
                if constexpr ( has_element_moved_op<Route> )
                {
                    tracked_indexes.assign(size, 0);
                    std::iota(tracked_indexes.begin(), tracked_indexes.end(), 0);
                }

                std::ptrdiff_t insertion_index = static_cast<std::ptrdiff_t>(size);
                for ( auto i : moved_indexes )
                {
                    if ( i >= 0 && static_cast<std::ptrdiff_t>(i)+1 < insertion_index )
                    {
                        auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                        std::rotate(it, it+1, std::next(ref.begin(), insertion_index));
                        if constexpr ( has_attached_data<Pathway...>() )
                            mirror_rotation_to(get_attached_data<Pathway...>(), static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertion_index));

                        mirror_rotation_to_selection(get_selections<Pathway...>(), static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertion_index));
                        if constexpr ( has_element_moved_op<Route> )
                            mirror_rotation_to(tracked_indexes, i, i+1, insertion_index);
                    }
                    --insertion_index;
                }

                if constexpr ( has_element_moved_op<Route> )
                {
                    for ( std::size_t i=0; i<size; ++i )
                    {
                        if ( i != tracked_indexes[i] )
                            notify_element_moved(user, Route{keys}, tracked_indexes[i], i);
                    }
                }
                if constexpr ( has_selections_changed_op<Route> )
                    notify_selections_changed(user, Route{keys});
            });
        }

        template <class ... Pathway, class Keys>
        void move_bottom_l(Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::move_bottom_l));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                using index_type = index_type_t<default_index_type, Member>;
                auto & sel = get_selections<Pathway...>();
                write_selections(events, sel);
                auto moved_indexes = sel; // Copy so it can be sorted without changing selections
                std::sort(moved_indexes.begin(), moved_indexes.end(), std::greater<index_type>{});

                std::size_t size = std::size(ref);
                std::vector<std::size_t> tracked_indexes {};
                if constexpr ( has_element_moved_op<Route> )
                {
                    tracked_indexes.assign(size, 0);
                    std::iota(tracked_indexes.begin(), tracked_indexes.end(), 0);
                }

                std::ptrdiff_t insertion_index = static_cast<std::ptrdiff_t>(size);
                for ( auto i : moved_indexes )
                {
                    if ( i >= 0 && static_cast<std::ptrdiff_t>(i)+1 < insertion_index )
                    {
                        auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                        std::rotate(it, it+1, std::next(ref.begin(), insertion_index));
                        if constexpr ( has_attached_data<Pathway...>() )
                            mirror_rotation_to(get_attached_data<Pathway...>(), static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertion_index));

                        mirror_rotation_to_selection(get_selections<Pathway...>(), static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertion_index));
                        if constexpr ( has_element_moved_op<Route> )
                            mirror_rotation_to(tracked_indexes, i, i+1, insertion_index);
                    }
                    --insertion_index;
                }

                if constexpr ( has_element_moved_op<Route> )
                {
                    for ( std::size_t i=0; i<size; ++i )
                    {
                        if ( i != tracked_indexes[i] )
                            notify_element_moved(user, Route{keys}, tracked_indexes[i], i);
                    }
                }
                if constexpr ( has_selections_changed_op<Route> )
                    notify_selections_changed(user, Route{keys});
            });
        }

        template <class ... Pathway, class Moved_index, class Index_moved_to, class Keys>
        void move_to(Moved_index moved_index, Index_moved_to index_moved_to, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::move_to));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                serialize_index<Member>(index_moved_to);
                serialize_index<Member>(moved_index);

                auto it = std::next(ref.begin(), moved_index);
                if ( static_cast<std::ptrdiff_t>(index_moved_to) < static_cast<std::ptrdiff_t>(moved_index) && index_moved_to >= 0 && static_cast<std::size_t>(moved_index) < std::size(ref) )
                {
                    std::rotate(std::next(ref.begin(), index_moved_to), it, it+1);
                    if constexpr ( has_attached_data<Pathway...>() )
                        mirror_rotation_to(get_attached_data<Pathway...>(), static_cast<std::size_t>(index_moved_to), static_cast<std::size_t>(moved_index), static_cast<std::size_t>(moved_index)+1);

                    mirror_rotation_to_selection(get_selections<Pathway...>(), static_cast<std::size_t>(index_moved_to), static_cast<std::size_t>(moved_index), static_cast<std::size_t>(moved_index)+1);
                    if constexpr ( has_element_moved_op<Route> )
                    {
                        notify_element_moved(user, Route{keys}, static_cast<std::size_t>(moved_index), static_cast<std::size_t>(index_moved_to));
                        for ( std::size_t i=static_cast<std::size_t>(index_moved_to); i<static_cast<std::size_t>(moved_index); ++i )
                            notify_element_moved(user, Route{keys}, i, i+1);
                    }
                    if constexpr ( has_selections<Pathway...>() && has_selections_changed_op<Route> )
                        notify_selections_changed(user, Route{keys});
                }
                else if ( static_cast<std::ptrdiff_t>(index_moved_to) > static_cast<std::ptrdiff_t>(moved_index) && moved_index >= 0 && static_cast<std::size_t>(index_moved_to) < std::size(ref) )
                {
                    std::rotate(it, it+1, std::next(ref.begin(), index_moved_to)+1);
                    if constexpr ( has_attached_data<Pathway...>() )
                        mirror_rotation_to(get_attached_data<Pathway...>(), static_cast<std::size_t>(moved_index), static_cast<std::size_t>(moved_index)+1, static_cast<std::size_t>(index_moved_to)+1);

                    mirror_rotation_to_selection(get_selections<Pathway...>(), static_cast<std::size_t>(moved_index), static_cast<std::size_t>(moved_index)+1, static_cast<std::size_t>(index_moved_to)+1);
                    if constexpr ( has_element_moved_op<Route> )
                    {
                        notify_element_moved(user, Route{keys}, static_cast<std::size_t>(moved_index), static_cast<std::size_t>(index_moved_to));
                        for ( std::size_t i=static_cast<std::size_t>(moved_index)+1; i<static_cast<std::size_t>(index_moved_to)+1; ++i )
                            notify_element_moved(user, Route{keys}, i, i-1);
                    }
                    if constexpr ( has_selections<Pathway...>() && has_selections_changed_op<Route> )
                        notify_selections_changed(user, Route{keys});
                }
            });
        }

        template <class ... Pathway, class Moved_indexes, class Index_moved_to, class Keys>
        void move_to_n(Moved_indexes && moved_indexes, Index_moved_to index_moved_to, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::move_to_n));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                std::sort(moved_indexes.begin(), moved_indexes.end());
                serialize_index<Member>(index_moved_to);
                serialize_index<Member>(std::size(moved_indexes));
                for ( auto i : moved_indexes )
                    serialize_index<Member>(i);

                auto & sel = get_selections<Pathway...>();
                write_selections(events, sel);

                auto size = std::size(ref);
                if ( index_moved_to < 0 )
                    index_moved_to = 0;
                else if ( static_cast<std::size_t>(index_moved_to) >= size && size > 0 )
                    index_moved_to = static_cast<Index_moved_to>(size-1);

                if ( !std::empty(moved_indexes) && index_moved_to >= 0 )
                {
                    std::size_t count_valid_indexes = 0;
                    for ( auto moved_index : moved_indexes )
                    {
                        if ( moved_index >= 0 && static_cast<std::size_t>(moved_index) < std::size(ref) )
                            ++count_valid_indexes;
                    }
                    if ( count_valid_indexes == 0 )
                        return;

                    std::vector<std::size_t> tracked_indexes {};
                    if constexpr ( has_element_moved_op<Route> )
                    {
                        tracked_indexes.assign(size, 0);
                        std::iota(tracked_indexes.begin(), tracked_indexes.end(), 0);
                    }

                    if ( static_cast<std::size_t>(index_moved_to) + count_valid_indexes >= size ) // Operation collapses to move_to_end
                    {
                        std::ptrdiff_t insertion_index = static_cast<std::ptrdiff_t>(size);
                        for ( auto mit = moved_indexes.rbegin(); mit != moved_indexes.rend(); ++mit ) // moved_indexes sorted least to greatest, so reverse read
                        {
                            auto i = *mit;
                            if ( static_cast<std::ptrdiff_t>(i) >= 0 && static_cast<std::ptrdiff_t>(i)+1 < insertion_index )
                            {
                                auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                std::rotate(it, it+1, std::next(ref.begin(), insertion_index));
                                if constexpr ( has_attached_data<Pathway...>() )
                                    mirror_rotation_to(get_attached_data<Pathway...>(), static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertion_index));

                                mirror_rotation_to_selection(sel, static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertion_index));
                                if constexpr ( has_element_moved_op<Route> )
                                    mirror_rotation_to(tracked_indexes, static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertion_index));
                            }
                            --insertion_index;
                        }
                    }
                    else // Non-collapsed move_to operation
                    {
                        std::size_t min_valid_index = 0;
                        std::size_t max_valid_index = 0;
                        for ( std::size_t i=0; i<std::size(moved_indexes); ++i )
                        {
                            if ( static_cast<std::ptrdiff_t>(moved_indexes[i]) >= 0 )
                            {
                                min_valid_index = static_cast<std::size_t>(moved_indexes[i]);
                                break;
                            }
                        }
                        for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(std::size(moved_indexes))-1; i>=0; --i )
                        {
                            if ( static_cast<std::ptrdiff_t>(moved_indexes[static_cast<std::size_t>(i)]) < static_cast<std::ptrdiff_t>(size) )
                            {
                                max_valid_index = static_cast<std::size_t>(moved_indexes[static_cast<std::size_t>(i)]);
                                break;
                            }
                        }

                        std::size_t left_chunk_size = min_valid_index < static_cast<std::size_t>(index_moved_to) ? 1 : 0;
                        std::size_t right_chunk_size = max_valid_index >= static_cast<std::size_t>(index_moved_to) ? 1 : 0;
                        std::size_t left_chunk_first = min_valid_index;
                        std::size_t right_chunk_first = max_valid_index;

                        for ( std::size_t i=0; i+1<std::size(moved_indexes) && static_cast<std::size_t>(moved_indexes[i+1]) < static_cast<std::size_t>(index_moved_to); ++i )
                        {
                            // rotate left chunk together with element[moved_indexes[i+1]]
                            auto left_start = std::next(ref.begin(), static_cast<std::ptrdiff_t>(left_chunk_first));
                            auto left_end = std::next(left_start, static_cast<std::ptrdiff_t>(left_chunk_size));
                            std::rotate(left_start, left_end, std::next(ref.begin(), static_cast<std::ptrdiff_t>(moved_indexes[i+1])));
                            if constexpr ( has_attached_data<Pathway...>() )
                                mirror_rotation_to(get_attached_data<Pathway...>(), left_chunk_first, left_chunk_first+left_chunk_size, moved_indexes[i+1]);

                            mirror_rotation_to_selection(sel, left_chunk_first, left_chunk_first+left_chunk_size, moved_indexes[i+1]);
                            if constexpr ( has_element_moved_op<Route> )
                                mirror_rotation_to(tracked_indexes, left_chunk_first, left_chunk_first+left_chunk_size, moved_indexes[i+1]);
                            left_chunk_first = static_cast<std::size_t>(moved_indexes[i+1])-left_chunk_size;
                            ++left_chunk_size;
                        }
                        for ( std::size_t i=std::size(moved_indexes)-1; i>0 && static_cast<std::size_t>(moved_indexes[i-1]) >= static_cast<std::size_t>(index_moved_to); --i )
                        {
                            // rotate right chunk together with element[moved_indexes[i-1]]
                            auto right_start = std::next(ref.begin(), static_cast<std::ptrdiff_t>(right_chunk_first));
                            auto right_end = std::next(right_start, static_cast<std::ptrdiff_t>(right_chunk_size));
                            std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(moved_indexes[i-1])+1), right_start, right_end);
                            if constexpr ( has_attached_data<Pathway...>() )
                                mirror_rotation_to(get_attached_data<Pathway...>(), static_cast<std::size_t>(moved_indexes[i-1])+1, right_chunk_first, right_chunk_first+right_chunk_size);

                            mirror_rotation_to_selection(sel, static_cast<std::size_t>(moved_indexes[i-1])+1, right_chunk_first, right_chunk_first+right_chunk_size);
                            if constexpr ( has_element_moved_op<Route> )
                                mirror_rotation_to(tracked_indexes, static_cast<std::size_t>(moved_indexes[i-1])+1, right_chunk_first, right_chunk_first+right_chunk_size);
                            right_chunk_first = static_cast<std::size_t>(moved_indexes[i-1]);
                            ++right_chunk_size;
                        }
                        if ( left_chunk_first+left_chunk_size != right_chunk_first && left_chunk_size > 0 && right_chunk_size > 0 ) // Need to combine the chunks then rotate combined to final position
                        {
                            std::size_t left_distance = left_chunk_first < static_cast<std::size_t>(index_moved_to) ? static_cast<std::size_t>(index_moved_to)-left_chunk_first : left_chunk_first-static_cast<std::size_t>(index_moved_to);
                            std::size_t right_distance = right_chunk_first < static_cast<std::size_t>(index_moved_to)+left_chunk_size ? static_cast<std::size_t>(index_moved_to)+left_chunk_size-right_chunk_first : right_chunk_first-static_cast<std::size_t>(index_moved_to)-left_chunk_size;
                            if ( right_distance < left_distance ) // Right chunk closer to target, move left up to right
                            {
                                auto left_start = std::next(ref.begin(), static_cast<std::ptrdiff_t>(left_chunk_first));
                                auto left_end = std::next(left_start, static_cast<std::ptrdiff_t>(left_chunk_size));
                                std::rotate(left_start, left_end, std::next(ref.begin(), static_cast<std::ptrdiff_t>(right_chunk_first)));
                                if constexpr ( has_attached_data<Pathway...>() )
                                    mirror_rotation_to(get_attached_data<Pathway...>(), left_chunk_first, left_chunk_first+left_chunk_size, right_chunk_first);

                                mirror_rotation_to_selection(sel, left_chunk_first, left_chunk_first+left_chunk_size, right_chunk_first);
                                if constexpr ( has_element_moved_op<Route> )
                                    mirror_rotation_to(tracked_indexes, left_chunk_first, left_chunk_first+left_chunk_size, right_chunk_first);
                                left_chunk_first = right_chunk_first-left_chunk_size;
                            }
                            else // Left chunk closer to target, move right up to left
                            {
                                auto right_start = std::next(ref.begin(), static_cast<std::ptrdiff_t>(right_chunk_first));
                                auto right_end = std::next(right_start, static_cast<std::ptrdiff_t>(right_chunk_size));
                                std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(left_chunk_first+left_chunk_size)), right_start, right_end);
                                if constexpr ( has_attached_data<Pathway...>() )
                                    mirror_rotation_to(get_attached_data<Pathway...>(), left_chunk_first+left_chunk_size, right_chunk_first, right_chunk_first+right_chunk_size);

                                mirror_rotation_to_selection(sel, left_chunk_first+left_chunk_size, right_chunk_first, right_chunk_first+right_chunk_size);
                                if constexpr ( has_element_moved_op<Route> )
                                    mirror_rotation_to(tracked_indexes, left_chunk_first+left_chunk_size, right_chunk_first, right_chunk_first+right_chunk_size);
                                right_chunk_first = left_chunk_first+left_chunk_size;
                            }
                        }
                        if ( left_chunk_first < static_cast<std::size_t>(index_moved_to) ) // Rotate combined chunk rightwards to final position
                        {
                            auto chunk_start = std::next(ref.begin(), static_cast<std::ptrdiff_t>(left_chunk_first));
                            auto chunk_end = std::next(chunk_start, static_cast<std::ptrdiff_t>(count_valid_indexes));
                            std::rotate(chunk_start, chunk_end, std::next(chunk_end, static_cast<std::ptrdiff_t>(static_cast<std::size_t>(index_moved_to)-left_chunk_first)));
                            if constexpr ( has_attached_data<Pathway...>() )
                                mirror_rotation_to(get_attached_data<Pathway...>(), left_chunk_first, left_chunk_first+count_valid_indexes, left_chunk_first+count_valid_indexes+static_cast<std::size_t>(index_moved_to)-left_chunk_first);

                            mirror_rotation_to_selection(sel, left_chunk_first, left_chunk_first+count_valid_indexes, left_chunk_first+count_valid_indexes+static_cast<std::size_t>(index_moved_to)-left_chunk_first);
                            if constexpr ( has_element_moved_op<Route> )
                                mirror_rotation_to(tracked_indexes, left_chunk_first, left_chunk_first+count_valid_indexes, left_chunk_first+count_valid_indexes+static_cast<std::size_t>(index_moved_to)-left_chunk_first);
                        }
                        else if ( left_chunk_first > static_cast<std::size_t>(index_moved_to) ) // Rotate combined chunk leftwards to final position
                        {
                            auto chunk_start = std::next(ref.begin(), static_cast<std::ptrdiff_t>(left_chunk_first));
                            auto chunk_end = std::next(chunk_start, static_cast<std::ptrdiff_t>(count_valid_indexes));
                            std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(index_moved_to)), chunk_start, chunk_end);
                            if constexpr ( has_attached_data<Pathway...>() )
                                mirror_rotation_to(get_attached_data<Pathway...>(), static_cast<std::size_t>(index_moved_to), left_chunk_first, left_chunk_first+count_valid_indexes);

                            mirror_rotation_to_selection(sel, static_cast<std::size_t>(index_moved_to), left_chunk_first, left_chunk_first+count_valid_indexes);
                            if constexpr ( has_element_moved_op<Route> )
                                mirror_rotation_to(tracked_indexes, static_cast<std::size_t>(index_moved_to), left_chunk_first, left_chunk_first+count_valid_indexes);
                        }
                    }
                    
                    if constexpr ( has_element_moved_op<Route> )
                    {
                        for ( std::size_t i=0; i<size; ++i )
                        {
                            if ( i != tracked_indexes[i] )
                                notify_element_moved(user, Route{keys}, tracked_indexes[i], i);
                        }
                    }
                    if constexpr ( has_selections_changed_op<Route> )
                        notify_selections_changed(user, Route{keys});
                }
            });
        }

        template <class ... Pathway, class Index_moved_to, class Keys>
        void move_to_l(Index_moved_to index_moved_to, Keys & keys)
        {
            event_offsets.push_back(events.size());
            events.push_back(uint8_t(op::move_to_l));
            serialize_pathway<Pathway...>(keys);
            
            operate_on<Pathway...>(t, keys, [&]<class Member, class Route>(auto & ref, type_tags<Member, Route>) {
                auto & sel = get_selections<Pathway...>();
                auto moved_indexes = sel; // Copy so it can be sorted without changing selections
                std::sort(moved_indexes.begin(), moved_indexes.end());
                serialize_index<Member>(index_moved_to);
                write_selections(events, sel);

                auto size = std::size(ref);
                if ( index_moved_to < 0 )
                    index_moved_to = 0;
                else if ( static_cast<std::size_t>(index_moved_to) >= size && size > 0 )
                    index_moved_to = static_cast<Index_moved_to>(size-1);

                if ( !std::empty(moved_indexes) && index_moved_to >= 0 )
                {
                    std::size_t count_valid_indexes = 0;
                    for ( auto moved_index : moved_indexes )
                    {
                        if ( moved_index >= 0 && static_cast<std::size_t>(moved_index) < std::size(ref) )
                            ++count_valid_indexes;
                    }
                    if ( count_valid_indexes == 0 )
                        return;

                    std::vector<std::size_t> tracked_indexes {};
                    if constexpr ( has_element_moved_op<Route> )
                    {
                        tracked_indexes.assign(size, 0);
                        std::iota(tracked_indexes.begin(), tracked_indexes.end(), 0);
                    }

                    if ( static_cast<std::size_t>(index_moved_to) + count_valid_indexes >= size ) // Operation collapses to move_to_end
                    {
                        std::ptrdiff_t insertion_index = static_cast<std::ptrdiff_t>(size);
                        for ( auto mit = moved_indexes.rbegin(); mit != moved_indexes.rend(); ++mit ) // moved_indexes sorted least to greatest, so reverse read
                        {
                            auto i = *mit;
                            if ( static_cast<std::ptrdiff_t>(i) >= 0 && static_cast<std::ptrdiff_t>(i)+1 < insertion_index )
                            {
                                auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                std::rotate(it, it+1, std::next(ref.begin(), insertion_index));
                                if constexpr ( has_attached_data<Pathway...>() )
                                    mirror_rotation_to(get_attached_data<Pathway...>(), static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertion_index));

                                mirror_rotation_to_selection(sel, static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertion_index));
                                if constexpr ( has_element_moved_op<Route> )
                                    mirror_rotation_to(tracked_indexes, static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertion_index));
                            }
                            --insertion_index;
                        }
                    }
                    else // Non-collapsed move_to operation
                    {
                        std::size_t min_valid_index = 0;
                        std::size_t max_valid_index = 0;
                        for ( std::size_t i=0; i<std::size(moved_indexes); ++i )
                        {
                            if ( static_cast<std::ptrdiff_t>(moved_indexes[i]) >= 0 )
                            {
                                min_valid_index = static_cast<std::size_t>(moved_indexes[i]);
                                break;
                            }
                        }
                        for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(std::size(moved_indexes))-1; i>=0; --i )
                        {
                            if ( static_cast<std::ptrdiff_t>(moved_indexes[static_cast<std::size_t>(i)]) < static_cast<std::ptrdiff_t>(size) )
                            {
                                max_valid_index = static_cast<std::size_t>(moved_indexes[static_cast<std::size_t>(i)]);
                                break;
                            }
                        }

                        std::size_t left_chunk_size = min_valid_index < static_cast<std::size_t>(index_moved_to) ? 1 : 0;
                        std::size_t right_chunk_size = max_valid_index >= static_cast<std::size_t>(index_moved_to) ? 1 : 0;
                        std::size_t left_chunk_first = min_valid_index;
                        std::size_t right_chunk_first = max_valid_index;

                        for ( std::size_t i=0; i+1<std::size(moved_indexes) && static_cast<std::size_t>(moved_indexes[i+1]) < static_cast<std::size_t>(index_moved_to); ++i )
                        {
                            // rotate left chunk together with element[moved_indexes[i+1]]
                            auto left_start = std::next(ref.begin(), static_cast<std::ptrdiff_t>(left_chunk_first));
                            auto left_end = std::next(left_start, static_cast<std::ptrdiff_t>(left_chunk_size));
                            std::rotate(left_start, left_end, std::next(ref.begin(), static_cast<std::ptrdiff_t>(moved_indexes[i+1])));
                            if constexpr ( has_attached_data<Pathway...>() )
                                mirror_rotation_to(get_attached_data<Pathway...>(), left_chunk_first, left_chunk_first+left_chunk_size, moved_indexes[i+1]);

                            mirror_rotation_to_selection(sel, left_chunk_first, left_chunk_first+left_chunk_size, moved_indexes[i+1]);
                            if constexpr ( has_element_moved_op<Route> )
                                mirror_rotation_to(tracked_indexes, left_chunk_first, left_chunk_first+left_chunk_size, moved_indexes[i+1]);
                            left_chunk_first = static_cast<std::size_t>(moved_indexes[i+1])-left_chunk_size;
                            ++left_chunk_size;
                        }
                        for ( std::size_t i=std::size(moved_indexes)-1; i>0 && static_cast<std::size_t>(moved_indexes[i-1]) >= static_cast<std::size_t>(index_moved_to); --i )
                        {
                            // rotate right chunk together with element[moved_indexes[i-1]]
                            auto right_start = std::next(ref.begin(), static_cast<std::ptrdiff_t>(right_chunk_first));
                            auto right_end = std::next(right_start, static_cast<std::ptrdiff_t>(right_chunk_size));
                            std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(moved_indexes[i-1])+1), right_start, right_end);
                            if constexpr ( has_attached_data<Pathway...>() )
                                mirror_rotation_to(get_attached_data<Pathway...>(), moved_indexes[i-1]+1, right_chunk_first, right_chunk_first+right_chunk_size);

                            mirror_rotation_to_selection(sel, moved_indexes[i-1]+1, right_chunk_first, right_chunk_first+right_chunk_size);
                            if constexpr ( has_element_moved_op<Route> )
                                mirror_rotation_to(tracked_indexes, moved_indexes[i-1]+1, right_chunk_first, right_chunk_first+right_chunk_size);
                            right_chunk_first = static_cast<std::size_t>(moved_indexes[i-1]);
                            ++right_chunk_size;
                        }
                        if ( left_chunk_first+left_chunk_size != right_chunk_first && left_chunk_size > 0 && right_chunk_size > 0 ) // Need to combine the chunks then rotate combined to final position
                        {
                            std::size_t left_distance = left_chunk_first < static_cast<std::size_t>(index_moved_to) ? static_cast<std::size_t>(index_moved_to)-left_chunk_first : left_chunk_first-static_cast<std::size_t>(index_moved_to);
                            std::size_t right_distance = right_chunk_first < static_cast<std::size_t>(index_moved_to)+left_chunk_size ? static_cast<std::size_t>(index_moved_to)+left_chunk_size-right_chunk_first : right_chunk_first-static_cast<std::size_t>(index_moved_to)-left_chunk_size;
                            if ( right_distance < left_distance ) // Right chunk closer to target, move left up to right
                            {
                                auto left_start = std::next(ref.begin(), static_cast<std::ptrdiff_t>(left_chunk_first));
                                auto left_end = std::next(left_start, static_cast<std::ptrdiff_t>(left_chunk_size));
                                std::rotate(left_start, left_end, std::next(ref.begin(), static_cast<std::ptrdiff_t>(right_chunk_first)));
                                if constexpr ( has_attached_data<Pathway...>() )
                                    mirror_rotation_to(get_attached_data<Pathway...>(), left_chunk_first, left_chunk_first+left_chunk_size, right_chunk_first);

                                mirror_rotation_to_selection(sel, left_chunk_first, left_chunk_first+left_chunk_size, right_chunk_first);
                                if constexpr ( has_element_moved_op<Route> )
                                    mirror_rotation_to(tracked_indexes, left_chunk_first, left_chunk_first+left_chunk_size, right_chunk_first);
                                left_chunk_first = right_chunk_first-left_chunk_size;
                            }
                            else // Left chunk closer to target, move right up to left
                            {
                                auto right_start = std::next(ref.begin(), static_cast<std::ptrdiff_t>(right_chunk_first));
                                auto right_end = std::next(right_start, static_cast<std::ptrdiff_t>(right_chunk_size));
                                std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(left_chunk_first+left_chunk_size)), right_start, right_end);
                                if constexpr ( has_attached_data<Pathway...>() )
                                    mirror_rotation_to(get_attached_data<Pathway...>(), left_chunk_first+left_chunk_size, right_chunk_first, right_chunk_first+right_chunk_size);

                                mirror_rotation_to_selection(sel, left_chunk_first+left_chunk_size, right_chunk_first, right_chunk_first+right_chunk_size);
                                if constexpr ( has_element_moved_op<Route> )
                                    mirror_rotation_to(tracked_indexes, left_chunk_first+left_chunk_size, right_chunk_first, right_chunk_first+right_chunk_size);
                                right_chunk_first = left_chunk_first+left_chunk_size;
                            }
                        }
                        if ( left_chunk_first < static_cast<std::size_t>(index_moved_to) ) // Rotate combined chunk rightwards to final position
                        {
                            auto chunk_start = std::next(ref.begin(), static_cast<std::ptrdiff_t>(left_chunk_first));
                            auto chunk_end = std::next(chunk_start, static_cast<std::ptrdiff_t>(count_valid_indexes));
                            std::rotate(chunk_start, chunk_end, std::next(chunk_end, static_cast<std::ptrdiff_t>(static_cast<std::size_t>(index_moved_to)-left_chunk_first)));
                            if constexpr ( has_attached_data<Pathway...>() )
                                mirror_rotation_to(get_attached_data<Pathway...>(), left_chunk_first, left_chunk_first+count_valid_indexes, left_chunk_first+count_valid_indexes+static_cast<std::size_t>(index_moved_to)-left_chunk_first);

                            mirror_rotation_to_selection(sel, left_chunk_first, left_chunk_first+count_valid_indexes, left_chunk_first+count_valid_indexes+static_cast<std::size_t>(index_moved_to)-left_chunk_first);
                            if constexpr ( has_element_moved_op<Route> )
                                mirror_rotation_to(tracked_indexes, left_chunk_first, left_chunk_first+count_valid_indexes, left_chunk_first+count_valid_indexes+static_cast<std::size_t>(index_moved_to)-left_chunk_first);
                        }
                        else if ( left_chunk_first > static_cast<std::size_t>(index_moved_to) ) // Rotate combined chunk leftwards to final position
                        {
                            auto chunk_start = std::next(ref.begin(), static_cast<std::ptrdiff_t>(left_chunk_first));
                            auto chunk_end = std::next(chunk_start, static_cast<std::ptrdiff_t>(count_valid_indexes));
                            std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(index_moved_to)), chunk_start, chunk_end);
                            if constexpr ( has_attached_data<Pathway...>() )
                                mirror_rotation_to(get_attached_data<Pathway...>(), static_cast<std::size_t>(index_moved_to), left_chunk_first, left_chunk_first+count_valid_indexes);

                            mirror_rotation_to_selection(sel, static_cast<std::size_t>(index_moved_to), left_chunk_first, left_chunk_first+count_valid_indexes);
                            if constexpr ( has_element_moved_op<Route> )
                                mirror_rotation_to(tracked_indexes, static_cast<std::size_t>(index_moved_to), left_chunk_first, left_chunk_first+count_valid_indexes);
                        }
                    }
                    
                    if constexpr ( has_element_moved_op<Route> )
                    {
                        for ( std::size_t i=0; i<size; ++i )
                        {
                            if ( i != tracked_indexes[i] )
                                notify_element_moved(user, Route{keys}, tracked_indexes[i], i);
                        }
                    }
                    if constexpr ( has_selections_changed_op<Route> )
                        notify_selections_changed(user, Route{keys});
                }
            });
        }

        template <class Value, class Member>
        void read_value(std::size_t & offset, auto && value) const
        {
            using deref = std::remove_cvref_t<Value>;
            using index_type = index_type_t<default_index_type, Member>;
            if constexpr ( std::is_same_v<deref, std::string> )
            {
                auto string_size = *(reinterpret_cast<const index_type*>(&events[offset]));
                offset += sizeof(string_size);
                if ( string_size > 0 )
                {
                    value = std::string{(const char*)&events[offset], string_size};
                    offset += string_size;
                }
            }
            else if constexpr ( RareTs::is_optional_v<deref> )
            {
                if ( u8bool::read(events, offset) ) // optional.has_value()
                {
                    value = typename deref::value_type{};
                    read_value<typename deref::value_type, Member>(offset, *value);
                }
                else
                    value = std::nullopt;
            }
            else if constexpr ( is_flat_mdspan_v<deref> )
            {
                using collapsed_index_type = collapsed_index_type_t<Member>;
                offset += sizeof(collapsed_index_type); // Skip over array size
                for ( auto it = value.flat_begin(); it != value.flat_end(); ++it )
                    read_value<typename deref::element_type, Member>(offset, *it);
            }
            else if constexpr ( std::is_array_v<deref> )
            {
                auto span = as_1d<Value>(value);
                using collapsed_index_type = collapsed_index_type_t<Member>;
                offset += sizeof(collapsed_index_type); // Skip over array size
                for ( auto it = span.flat_begin(); it != span.flat_end(); ++it )
                    read_value<typename decltype(span)::element_type, Member>(offset, *it);
            }
            else if constexpr ( RareTs::is_static_array_v<deref> )
            {
                constexpr std::size_t size = RareTs::static_array_size_v<deref>;
                offset += sizeof(size); // Skip over array size
                for ( std::size_t i=0; i<size; ++i )
                    read_value<std::remove_cvref_t<decltype(std::declval<deref>()[0])>, Member>(offset, value[i]);
            }
            else if constexpr ( RareTs::is_iterable_v<deref> && requires{value.resize(std::size_t{0});} )
            {
                auto size = static_cast<std::size_t>(*(reinterpret_cast<const index_type*>(&events[offset])));
                value.resize(std::size_t{size});
                offset += sizeof(index_type);
                for ( decltype(size) i=0; i<size; ++i )
                    read_value<std::remove_cvref_t<decltype(std::declval<deref>()[0])>, Member>(offset, value[i]);
            }
            else if constexpr ( RareTs::is_reflected_v<deref> )
            {
                RareTs::Members<deref>::forEach([&](auto member) {
                    read_value<std::remove_cvref_t<typename decltype(member)::type>, decltype(member)>(offset, member.value(value));
                });
            }
            else
            {
                value = (deref &)events[offset];
                offset += sizeof(deref);
            }
        }

        template <class Value, class Member>
        auto read_value(std::size_t & offset) const
        {
            static_assert(!std::is_array_v<Value>, "Cannot return array[] types, use the read_value reference overload");
            std::remove_cvref_t<Value> value {};
            read_value<Value, Member>(offset, value);
            return value;
        }

        /// Same as read_value except this doesn't modify the offset
        /// @param offset the offset at which to peek a value
        /// @return the value being peeked at offset
        template <class Value, class Member>
        auto peek_value(std::size_t offset) const
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

                    read_value<decltype(value), Member>(offset, value);
                    return value;
                }
                else
                {
                    std::vector<RareTs::element_type_t<Value>> value { std::size_t{RareTs::static_array_size_v<Value>} };
                    read_value<decltype(value), Member>(offset, value);
                    return value;
                }
            }
            else
            {
                std::remove_cvref_t<Value> value {};
                read_value<Value, Member>(offset, value);
                return value;
            }
        }

        template <class Index_type>
        auto read_index(std::size_t & offset) const {
            auto index = *(reinterpret_cast<const Index_type*>(&events[offset]));
            offset += sizeof(index);
            return index;
        }

        template <class Index_type>
        auto read_index(std::ptrdiff_t & offset) const {
            auto index = *(reinterpret_cast<const Index_type*>(&events[static_cast<std::size_t>(offset)]));
            offset += sizeof(index);
            return index;
        }

        template <class Index_type>
        std::span<const Index_type> read_indexes(std::size_t & offset, std::size_t count) const
        {
            if ( count > 0 )
            {
                std::span<const Index_type> indexes(reinterpret_cast<const Index_type*>(&events[offset]), count);
                offset += sizeof(Index_type)*count;
                return indexes;
            }
            else
                return std::span<const Index_type>();
        }

        template <class Index_type>
        std::span<const Index_type> read_indexes(std::ptrdiff_t & offset, std::size_t count) const
        {
            if ( count > 0 )
            {
                std::span<const Index_type> indexes(reinterpret_cast<const Index_type*>(&events[static_cast<std::size_t>(offset)]), count);
                offset += sizeof(Index_type)*count;
                return indexes;
            }
            else
                return std::span<const Index_type>();
        }

        template <class Value_type, class Member, class ... Pathway>
        void process_undo_event(std::uint8_t operation, std::size_t & offset, std::optional<std::size_t> & secondary_offset, auto & ref, auto keys)
        {
            using index_type = index_type_t<default_index_type, Member>;
            using path_pack = type_tags<Pathway...>;
            using sel_type = std::remove_cvref_t<decltype(get_selections<Pathway...>())>;
            using element_type = RareTs::element_type_t<Value_type>;
            using attached_data_type = std::remove_cvref_t<decltype(get_attached_data<Pathway...>())>;
            using route = path_tagged_keys<decltype(keys), path_pack, Editor_type>;
            using elem_path = type_tags<Pathway..., path_index<std::tuple_size_v<std::remove_cvref_t<decltype(keys)>>>>;
            using elem_keys = std::remove_cvref_t<decltype(std::tuple_cat(keys, std::tuple<index_type>{0}))>;
            using elem_route = path_tagged_keys<elem_keys, elem_path, Editor_type>;
            constexpr bool has_attached_data = agent::has_attached_data<Pathway...>();
            constexpr bool has_selections = !std::is_null_pointer_v<sel_type> && RareTs::is_specialization_v<sel_type, std::vector>;
            constexpr bool has_sel_change_op = has_selections && has_selections_changed_op<route>;
            constexpr bool is_iterable = RareTs::is_iterable_v<Value_type>;
            constexpr bool has_move_ops = !std::is_void_v<element_type> && requires { ref.begin(); };

            switch ( op(operation) )
            {
                case op::init:
                {
                    if constexpr ( RareTs::is_assignable_v<decltype(ref), Value_type> )
                        ref = read_value<Value_type, Member>(offset);
                }
                break;
                case op::reset:
                {
                    if constexpr ( !is_iterable && has_value_changed_op<route, Value_type> )
                    {
                        if constexpr ( std::is_array_v<typename Member::type> )
                        {
                            typename Member::type prev_value {};
                            auto ref_array = as_1d<typename Member::type>(ref);
                            as_1d(prev_value) = ref_array;
                            read_value<Value_type, Member>(offset, ref_array);
                            notify_value_changed(user, route{keys}, prev_value, ref); // Issue change notification
                        }
                        else
                        {
                            auto prev_value = ref;
                            ref = read_value<Value_type, Member>(offset);
                            notify_value_changed(user, route{keys}, prev_value, ref);
                        }
                    }
                    else if constexpr ( is_iterable && has_element_added_op<route> )
                    {
                        ref = read_value<Value_type, Member>(offset);
                        if constexpr ( has_attached_data )
                            get_attached_data<Pathway...>() = attached_data_type(std::size(ref));
                        for ( std::size_t i=0; i<std::size(ref); ++i )
                            notify_element_added(user, route{keys}, i);
                    }
                    else if constexpr ( is_array_member_v<Member> && requires { ref[0]; } )
                        read_value<Value_type, Member>(offset, ref);
                    else
                    {
                        ref = read_value<Value_type, Member>(offset);
                        if constexpr ( has_attached_data )
                            get_attached_data<Pathway...>() = attached_data_type(std::size(ref));
                    }

                    if constexpr ( has_selections )
                    {
                        read_selections(events, offset, get_selections<Pathway...>());
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::reserve:
                {
                    if constexpr ( requires { ref.shrink_to_fit(); } )
                        ref.shrink_to_fit();
                }
                break;
                case op::trim:
                {
                    // NoOp
                }
                break;
                case op::assign:
                {
                    if constexpr ( has_selections && is_iterable && requires { ref.reserve(1); } )
                    {
                        read_index<index_type>(offset); // new size (unused)
                        read_value<element_type, Member>(offset); // assigned value (unused)
                        auto size = static_cast<std::size_t>(read_index<index_type>(offset)); // prev size

                        Value_type prev_container {};
                        prev_container.reserve(size);
                        for ( std::size_t i=0; i<size; ++i )
                            prev_container.push_back(read_value<element_type, Member>(offset));

                        if constexpr ( has_element_removed_op<route> )
                        {
                            for ( std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1; i>=0; --i )
                                notify_element_removed(user, route{keys}, static_cast<std::size_t>(i));
                        }
                        std::swap(ref, prev_container);
                        if constexpr ( has_attached_data )
                            get_attached_data<Pathway...>() = attached_data_type(std::size(ref));

                        if constexpr ( has_element_added_op<route> )
                        {
                            for ( std::size_t i=0; i<std::size(ref); ++i )
                                notify_element_added(user, route{keys}, i);
                        }

                        if constexpr ( has_sel_change_op )
                        {
                            if ( read_selections(events, offset, get_selections<Pathway...>()) )
                                notify_selections_changed(user, route{keys});
                        }
                        else
                            read_selections(events, offset, get_selections<Pathway...>());
                    }
                }
                break;
                case op::assign_default:
                {
                    if constexpr ( has_selections && is_iterable && requires { ref.reserve(1); } )
                    {
                        read_index<index_type>(offset); // new size (unused)
                        auto size = static_cast<std::size_t>(read_index<index_type>(offset)); // old size

                        Value_type prev_container {};
                        prev_container.reserve(size);
                        for ( std::size_t i=0; i<size; ++i )
                            prev_container.push_back(read_value<element_type, Member>(offset));

                        if constexpr ( has_element_removed_op<route> )
                        {
                            for ( std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1; i>=0; --i )
                                notify_element_removed(user, route{keys}, static_cast<std::size_t>(i));
                        }
                        std::swap(ref, prev_container);
                        if constexpr ( has_attached_data )
                            get_attached_data<Pathway...>() = attached_data_type(std::size(ref));

                        if constexpr ( has_element_added_op<route> )
                        {
                            for ( std::size_t i=0; i<std::size(ref); ++i )
                                notify_element_added(user, route{keys}, i);
                        }

                        if constexpr ( has_sel_change_op )
                        {
                            if ( read_selections(events, offset, get_selections<Pathway...>()) )
                                notify_selections_changed(user, route{keys});
                        }
                        else
                            read_selections(events, offset, get_selections<Pathway...>());
                    }
                }
                break;
                case op::clear_selections:
                {
                    if constexpr ( has_selections )
                    {
                        auto & sel = get_selections<Pathway...>();
                        sel_type prev_selections {};
                        read_selection_vector(events, offset, prev_selections);
                        std::swap(sel, prev_selections);
                    }
                    if constexpr ( has_sel_change_op )
                        notify_selections_changed(user, route{keys});
                }
                break;
                case op::select_all:
                {
                    if constexpr ( has_selections )
                    {
                        auto & sel = get_selections<Pathway...>();
                        sel_type prev_selections {};
                        read_selection_vector(events, offset, prev_selections);
                        std::swap(sel, prev_selections);

                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::select:
                {
                    if constexpr ( has_selections )
                    {
                        auto & sel = get_selections<Pathway...>();
                        auto value = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto found = std::find(sel.begin(), sel.end(), value);
                        if ( found != sel.end() )
                            sel.erase(found);

                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::select_n:
                {
                    if constexpr ( has_selections )
                    {
                        auto & sel = get_selections<Pathway...>();
                        auto size = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto sel_indexes = read_indexes<index_type>(offset, size);
                        for ( std::size_t i=0; i<size; ++i )
                        {
                            auto found = std::find(sel.begin(), sel.end(), sel_indexes[i]);
                            if ( found != sel.end() )
                                sel.erase(found);
                        }
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::deselect:
                {
                    if constexpr ( has_selections )
                    {
                    auto & sel = get_selections<Pathway...>();
                    auto value = read_index<index_type>(offset);
                    auto sel_index = read_index<index_type>(offset);
                    if ( std::find(sel.begin(), sel.end(), value) == sel.end() )
                        sel.insert(std::next(sel.begin(), static_cast<std::ptrdiff_t>(sel_index)), value);
                        
                    if constexpr ( has_sel_change_op )
                        notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::deselect_n:
                {
                    if constexpr ( has_selections )
                    {
                        auto & sel = get_selections<Pathway...>();
                        auto size = static_cast<std::ptrdiff_t>(read_index<index_type>(offset));
                        auto deselect_indexes = read_indexes<index_type>(offset, static_cast<std::size_t>(size));
                        auto prev_sel_indexes = read_indexes<index_type>(offset, static_cast<std::size_t>(size));

                        for ( std::ptrdiff_t i=size-1; i>=0; --i )
                        {
                            sel.insert(std::next(sel.begin(), static_cast<std::ptrdiff_t>(prev_sel_indexes[static_cast<std::size_t>(i)])),
                                deselect_indexes[static_cast<std::size_t>(i)]);
                        }

                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::toggle_selection:
                {
                    if constexpr ( has_selections )
                    {
                        auto & sel = get_selections<Pathway...>();
                        auto value = read_index<index_type>(offset);
                        if ( u8bool::read(events, offset) ) // was_selected
                            sel.insert(std::next(sel.begin(), static_cast<std::ptrdiff_t>(read_index<index_type>(offset))), value);
                        else
                            std::erase(sel, value);
                        
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::toggle_selection_n:
                {
                    if constexpr ( has_selections )
                    {
                        auto & sel = get_selections<Pathway...>();
                        auto size = static_cast<std::ptrdiff_t>(read_index<index_type>(offset));
                        auto toggled_indexes = read_indexes<index_type>(offset, static_cast<std::size_t>(size));

                        std::vector<bool> was_selected {};
                        read_vec_bool_data(events, offset, static_cast<std::size_t>(size), was_selected);

                        auto prev_sel_indexes = read_indexes<index_type>(offset, static_cast<std::size_t>(size));

                        for ( std::ptrdiff_t i=size-1; i>=0; --i )
                        {
                            auto found = std::find(sel.begin(), sel.end(), toggled_indexes[static_cast<std::size_t>(i)]);
                            if ( found == sel.end() )
                                sel.insert(std::next(sel.begin(), static_cast<std::ptrdiff_t>(prev_sel_indexes[static_cast<std::size_t>(i)])), toggled_indexes[static_cast<std::size_t>(i)]);
                            else
                                sel.erase(found);
                        }

                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::sort_selections:
                {
                    if constexpr ( has_selections )
                    {
                        auto & sel = get_selections<Pathway...>();
                        auto size = read_index<index_type>(offset);
                        auto source_indexes = read_indexes<index_type>(offset, size);

                        undo_sort(sel, source_indexes);

                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::sort_selections_desc:
                {
                    if constexpr ( has_selections )
                    {
                        auto & sel = get_selections<Pathway...>();
                        auto size = read_index<index_type>(offset);
                        auto source_indexes = read_indexes<index_type>(offset, size);

                        undo_sort(sel, source_indexes);

                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::set:
                {
                    if constexpr ( RareTs::is_assignable_v<decltype(ref), Value_type> )
                    {
                        read_value<Value_type, Member>(offset); // new_value (unused)
                        auto prev_value = read_value<Value_type, Member>(offset);
                        if constexpr ( !is_iterable && has_value_changed_op<route, Value_type> )
                        {
                            auto temp = ref;
                            ref = prev_value;
                            notify_value_changed(user, route{keys}, temp, ref);
                        }
                        else
                        {
                            if constexpr ( is_iterable && has_element_removed_op<route> )
                            {
                                for ( std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1; i>=0; --i )
                                    notify_element_removed(user, route{keys}, static_cast<std::size_t>(i));
                            }

                            ref = prev_value;
                            if constexpr ( has_attached_data )
                                get_attached_data<Pathway...>() = attached_data_type(std::size(ref));

                            if constexpr ( is_iterable && has_element_added_op<route> )
                            {
                                for ( std::size_t i=0; i<std::size(ref); ++i )
                                    notify_element_added(user, route{keys}, i);
                            }
                        }

                        if constexpr ( has_selections )
                        {
                            read_selections(events, offset, get_selections<Pathway...>());
                            if constexpr ( has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::set_n:
                {
                    if constexpr ( requires{ref[0];} && requires{ref[0] = std::declval<decltype(ref[0])>();} )
                    {
                        constexpr bool is_iterable_element = RareTs::is_iterable_v<element_type>;
                        auto count = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto set_indexes = read_indexes<index_type>(offset, count);

                        read_value<element_type, Member>(offset); // new value (unused);
                            
                        if constexpr ( !is_iterable_element && has_value_changed_op<route, element_type> )
                        {
                            for ( auto index : set_indexes )
                            {
                                auto prev_value = read_value<element_type, Member>(offset);
                                std::swap(ref[index], prev_value);
                                notify_value_changed(user, elem_route{std::tuple_cat(keys, std::tuple<index_type>{index})}, prev_value, ref[index]);
                            }
                        }
                        else if constexpr ( is_iterable_element && has_element_removed_op<route> )
                        {
                            for ( auto index : set_indexes )
                            {
                                for ( std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref[index]))-1; i>=0; --i )
                                    notify_element_removed(user, elem_route{std::tuple_cat(keys, std::tuple<index_type>{index})}, static_cast<std::size_t>(i));

                                ref[index] = read_value<element_type, Member>(offset);

                                if constexpr ( has_element_added_op<route> )
                                {
                                    for ( std::size_t j=0; j<std::size(ref[index]); ++j )
                                        notify_element_added(user, elem_route{std::tuple_cat(keys, std::tuple<index_type>{index})}, j);
                                }
                            }
                        }
                        else
                        {
                            for ( auto index : set_indexes )
                            {
                                ref[index] = read_value<element_type, Member>(offset);
                                if constexpr ( is_iterable_element && has_element_added_op<route> )
                                {
                                    for ( std::size_t i=0; i<std::size(ref[index]); ++i )
                                        notify_element_added(user, elem_route{std::tuple_cat(keys, std::tuple<index_type>{index})}, i);
                                }
                            }
                        }
                    }
                }
                break;
                case op::set_l:
                {
                    if constexpr ( RareTs::is_assignable_v<decltype(ref), Value_type> )
                    {
                        if ( !secondary_offset )
                        {
                            secondary_offset = offset; // Is set exclusively for the first visit of the selection
                            read_value<Value_type, Member>(*secondary_offset); // Advanced past valueSetTo (unused for undos, used for redos)
                        }
                        auto prev_value = read_value<Value_type, Member>(*secondary_offset);
                        if constexpr ( !is_iterable && has_value_changed_op<route, Value_type> )
                        {
                            auto temp = ref;
                            ref = prev_value;
                            notify_value_changed(user, route{keys}, temp, ref);
                        }
                        else
                        {
                            if constexpr ( is_iterable && has_element_removed_op<route> )
                            {
                                for ( std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1; i>=0; --i )
                                    notify_element_removed(user, route{keys}, static_cast<std::size_t>(i));
                            }
                            ref = prev_value;

                            if constexpr ( is_iterable && has_element_added_op<route> )
                            {
                                for ( std::size_t i=0; i<std::size(ref); ++i )
                                    notify_element_added(user, route{keys}, i);
                            }
                        }
                    }
                }
                break;
                case op::append:
                {
                    if constexpr ( has_element_removed_op<route> && requires { ref.size(); } )
                        notify_element_removed(user, route{keys}, ref.size()-1);
                    if constexpr ( RareTs::has_pop_back_v<decltype(ref)> )
                        ref.pop_back();
                    if constexpr ( has_attached_data )
                        get_attached_data<Pathway...>().pop_back();
                }
                break;
                case op::append_n:
                {
                    if constexpr ( requires { ref.size(); ref.erase(ref.begin(), ref.end()); } )
                    {
                        auto count = static_cast<std::ptrdiff_t>(read_index<index_type>(offset));
                        auto size = static_cast<std::ptrdiff_t>(ref.size());
                        if constexpr ( has_element_removed_op<route> )
                        {
                            for ( std::ptrdiff_t i=size-1; i>=size-count; --i )
                                notify_element_removed(user, route{keys}, static_cast<std::size_t>(i));
                        }
                        ref.erase(std::next(ref.begin(), size-count), ref.end());
                        if constexpr ( has_attached_data )
                        {
                            auto & attached_data = get_attached_data<Pathway...>();
                            attached_data.erase(std::next(attached_data.begin(), size-count), attached_data.end());
                        }
                    }
                }
                break;
                case op::insert:
                {
                    if constexpr ( has_selections && !std::is_void_v<element_type> && requires { ref.erase(ref.begin()); } )
                    {
                        auto insertion_index = read_index<index_type>(offset);
                        if constexpr ( has_element_removed_op<route> )
                            notify_element_removed(user, route{keys}, static_cast<std::size_t>(insertion_index));

                        ref.erase(std::next(ref.begin(), static_cast<std::ptrdiff_t>(insertion_index)));
                        if constexpr ( has_attached_data )
                        {
                            auto & attached_data = get_attached_data<Pathway...>();
                            attached_data.erase(std::next(attached_data.begin(), static_cast<std::ptrdiff_t>(insertion_index)));
                        }

                        if constexpr ( has_element_moved_op<route> )
                        {
                            for ( std::size_t i=static_cast<std::size_t>(insertion_index); i<std::size(ref); ++i )
                                notify_element_moved(user, route{keys}, i+1, i);
                        }

                        if constexpr ( has_selections )
                        {
                            auto & sel = get_selections<Pathway...>();
                            for ( auto & index : sel )
                            {
                                if ( static_cast<index_type>(index) >= insertion_index )
                                    --index;
                            }
                            if constexpr ( has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::insert_n:
                {
                    if constexpr ( !std::is_void_v<element_type> && requires { ref.erase(ref.begin(), ref.end()); } )
                    {
                        auto insertion_count = read_index<index_type>(offset);
                        auto insertion_index = read_index<index_type>(offset);
                        if constexpr ( has_element_removed_op<route> )
                        {
                            for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(insertion_index)+static_cast<std::ptrdiff_t>(insertion_count)-1;
                                i >= static_cast<std::ptrdiff_t>(insertion_index); --i )
                            {
                                notify_element_removed(user, route{keys}, static_cast<std::size_t>(i));
                            }
                        }
                        ref.erase(
                            std::next(ref.begin(), static_cast<std::ptrdiff_t>(insertion_index)),
                            std::next(ref.begin(), static_cast<std::ptrdiff_t>(insertion_index)+static_cast<std::ptrdiff_t>(insertion_count))
                        );
                        if constexpr ( has_attached_data )
                        {
                            auto & attached_data = get_attached_data<Pathway...>();
                            attached_data.erase(
                                std::next(attached_data.begin(), static_cast<std::ptrdiff_t>(insertion_index)),
                                std::next(attached_data.begin(), static_cast<std::ptrdiff_t>(insertion_index)+static_cast<std::ptrdiff_t>(insertion_count))
                            );
                        }
                        if constexpr ( has_element_moved_op<route> )
                        {
                            for ( std::size_t i=static_cast<std::size_t>(insertion_index);
                                i+1<static_cast<std::size_t>(insertion_index)+static_cast<std::size_t>(insertion_count); ++i )
                            {
                                notify_element_moved(user, route{keys}, i+static_cast<std::size_t>(insertion_count), i);
                            }
                        }
                        if constexpr ( has_selections )
                        {
                            auto & sel = get_selections<Pathway...>();
                            for ( auto & index : sel )
                            {
                                if ( static_cast<index_type>(index) >= insertion_index )
                                    index -= insertion_count;
                            }
                            if constexpr ( has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::remove:
                {
                    if constexpr ( !std::is_void_v<element_type> && requires { ref.insert(ref.begin(), std::declval<element_type>()); } )
                    {
                        auto removal_index = read_index<index_type>(offset);
                        auto removed_value = read_value<element_type, Member>(offset);
                        ref.insert(std::next(ref.begin(), static_cast<std::ptrdiff_t>(removal_index)), removed_value);
                        if constexpr ( has_attached_data )
                        {
                            auto & attached_data = get_attached_data<Pathway...>();
                            attached_data.emplace(std::next(attached_data.begin(), static_cast<std::ptrdiff_t>(removal_index)));
                        }
                        if constexpr ( has_element_added_op<route> )
                            notify_element_added(user, route{keys}, removal_index);

                        if constexpr ( has_element_moved_op<route> )
                        {
                            for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(std::size(ref))-1; i>static_cast<std::ptrdiff_t>(removal_index); --i )
                                notify_element_moved(user, route{keys}, static_cast<std::size_t>(i-1), static_cast<std::size_t>(i));
                        }

                        if ( u8bool::read(events, offset) )
                        {
                            if constexpr ( has_selections )
                            {
                                auto & sel = get_selections<Pathway...>();
                                auto prev_sel_index = read_index<index_type>(offset);
                                sel.insert(std::next(sel.begin(), static_cast<std::ptrdiff_t>(prev_sel_index)), removal_index);
                                for ( auto & i : sel )
                                {
                                    if ( i > removal_index )
                                        ++i;
                                }
                            }
                        }
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::remove_n:
                {
                    if constexpr ( !std::is_void_v<element_type> && requires { ref.insert(ref.begin(), std::declval<element_type>()); } )
                    {
                        std::ptrdiff_t removal_count = static_cast<std::ptrdiff_t>(read_index<index_type>(offset));
                        auto removal_indexes = read_indexes<index_type>(offset, static_cast<std::size_t>(removal_count)); // removal_indexes were pre-sorted highest index to lowest index

                        std::vector<element_type> removed_values {};
                        removed_values.reserve(static_cast<std::size_t>(removal_count));
                        for ( std::ptrdiff_t i=0; i<removal_count; ++i )
                            removed_values.push_back(read_value<element_type, Member>(offset));
                            
                        for ( std::ptrdiff_t i=removal_count-1; i>=0; --i ) // insert values which were removed from the lowest indexes first
                        {
                            auto reinserted_index = removal_indexes[static_cast<std::size_t>(i)];
                            ref.insert(std::next(ref.begin(), static_cast<std::ptrdiff_t>(reinserted_index)), removed_values[static_cast<std::size_t>(i)]);
                            if constexpr ( has_attached_data )
                            {
                                auto & attached_data = get_attached_data<Pathway...>();
                                attached_data.emplace(std::next(attached_data.begin(), static_cast<std::ptrdiff_t>(reinserted_index)));
                            }
                            if constexpr ( has_selections )
                            {
                                auto & sel = get_selections<Pathway...>();
                                for ( auto & s : sel )
                                {
                                    if ( s >= reinserted_index )
                                        ++s;
                                }
                            }
                        }
                        if constexpr ( has_element_added_op<route> )
                        {
                            for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(std::size(removal_indexes))-1; i>=0; --i )
                                notify_element_added(user, route{keys}, static_cast<std::size_t>(removal_indexes[static_cast<std::size_t>(i)]));
                        }
                        if constexpr ( has_element_moved_op<route> )
                        {
                            std::ptrdiff_t collection_index = static_cast<std::ptrdiff_t>(std::size(ref))-1;
                            std::ptrdiff_t move_distance = static_cast<std::ptrdiff_t>(std::size(removal_indexes));
                            for ( ; collection_index>static_cast<std::ptrdiff_t>(removal_indexes[0]); --collection_index )
                                notify_element_moved(user, route{keys}, static_cast<std::size_t>(collection_index-move_distance), static_cast<std::size_t>(collection_index));
                            for ( std::size_t i=1; i<std::size(removal_indexes); ++i )
                            {
                                --move_distance;
                                for ( collection_index = static_cast<std::ptrdiff_t>(removal_indexes[i-1])-1; collection_index > static_cast<std::ptrdiff_t>(removal_indexes[i]); --collection_index )
                                    notify_element_moved(user, route{keys}, static_cast<std::size_t>(collection_index-move_distance), static_cast<std::size_t>(collection_index));
                            }
                        }

                        if constexpr ( has_selections )
                        {
                            auto & sel = get_selections<Pathway...>();
                            std::vector<bool> removal_index_selected {};
                            read_vec_bool_data(events, offset, static_cast<std::size_t>(removal_count), removal_index_selected);

                            std::size_t prev_sel_index_count = static_cast<std::size_t>(read_index<index_type>(offset));
                            auto prev_sel_indexes = read_indexes<index_type>(offset, prev_sel_index_count);

                            std::size_t unremoved_i = std::size(prev_sel_indexes)-1;
                            for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(removal_count)-1; i>=0; --i )
                            {
                                if ( removal_index_selected[static_cast<std::size_t>(i)] )
                                {
                                    auto inserted_value = removal_indexes[static_cast<std::size_t>(i)];
                                    sel.insert(std::next(sel.begin(), static_cast<std::ptrdiff_t>(prev_sel_indexes[unremoved_i])), inserted_value);
                                    --unremoved_i;
                                }
                            }
                                
                            if constexpr ( has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::remove_l:
                {
                    if constexpr ( !std::is_void_v<element_type> && requires { ref.insert(ref.begin(), std::declval<element_type>()); } )
                    {
                        std::ptrdiff_t removal_count = static_cast<std::ptrdiff_t>(read_index<index_type>(offset));
                        auto removal_indexes = read_indexes<index_type>(offset, static_cast<std::size_t>(removal_count)); // removal_indexes were pre-sorted highest index to lowest index

                        std::vector<element_type> removed_values {};
                        removed_values.reserve(static_cast<std::size_t>(removal_count));
                        for ( std::ptrdiff_t i=0; i<removal_count; ++i )
                            removed_values.push_back(read_value<element_type, Member>(offset));

                        for ( std::ptrdiff_t i=removal_count-1; i>=0; --i ) // insert values which were removed from the lowest indexes first
                            ref.insert(std::next(ref.begin(), static_cast<std::ptrdiff_t>(removal_indexes[static_cast<std::size_t>(i)])), removed_values[static_cast<std::size_t>(i)]);

                        if constexpr ( has_attached_data )
                        {
                            auto & attached_data = get_attached_data<Pathway...>();
                            for ( std::ptrdiff_t i=removal_count-1; i>=0; --i )
                                attached_data.emplace(std::next(attached_data.begin(), static_cast<std::ptrdiff_t>(removal_indexes[static_cast<std::size_t>(i)])));
                        }

                        if constexpr ( has_element_added_op<route> )
                        {
                            for ( std::ptrdiff_t i=removal_count-1; i>=0; --i ) // insert values which were removed from the lowest indexes first
                                notify_element_added(user, route{keys}, static_cast<std::size_t>(removal_indexes[static_cast<std::size_t>(i)]));
                        }

                        if constexpr ( has_element_moved_op<route> )
                        {
                            std::ptrdiff_t collection_index = static_cast<std::ptrdiff_t>(std::size(ref))-1;
                            std::ptrdiff_t move_distance = static_cast<std::ptrdiff_t>(std::size(removal_indexes));
                            for ( ; collection_index>static_cast<std::ptrdiff_t>(removal_indexes[0]); --collection_index )
                                notify_element_moved(user, route{keys}, static_cast<std::size_t>(collection_index-move_distance), static_cast<std::size_t>(collection_index));
                            for ( std::size_t i=1; i<std::size(removal_indexes); ++i )
                            {
                                --move_distance;
                                for ( collection_index = static_cast<std::ptrdiff_t>(removal_indexes[i-1])-1; collection_index > static_cast<std::ptrdiff_t>(removal_indexes[i]); --collection_index )
                                    notify_element_moved(user, route{keys}, static_cast<std::size_t>(collection_index-move_distance), static_cast<std::size_t>(collection_index));
                            }
                        }

                        read_selections(events, offset, get_selections<Pathway...>());

                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                };
                break;
                case op::sort:
                {
                    if constexpr ( RareTs::is_specialization_v<Value_type, std::vector> )
                    {
                        std::size_t count = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto source_indexes = read_indexes<index_type>(offset, count);

                        undo_sort(ref, source_indexes);
                        if constexpr ( has_attached_data )
                            undo_sort(get_attached_data<Pathway...>(), source_indexes);

                        if constexpr ( has_element_moved_op<route> )
                        {
                            for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(count)-1; i>=0; --i )
                            {
                                if ( i != static_cast<std::ptrdiff_t>(source_indexes[static_cast<std::size_t>(i)]) )
                                    notify_element_moved(user, route{keys}, static_cast<std::size_t>(i), static_cast<std::size_t>(source_indexes[static_cast<std::size_t>(i)]));
                            }
                        }

                        if constexpr ( has_selections )
                        {
                            auto & sel = get_selections<Pathway...>();
                            sel_type old_sel {};
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( std::find(sel.begin(), sel.end(), i) != sel.end() )
                                    RareTs::append(old_sel, source_indexes[i]);
                            }

                            std::swap(sel, old_sel);

                            if constexpr ( has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::sort_desc:
                {
                    if constexpr ( RareTs::is_specialization_v<Value_type, std::vector> )
                    {
                        std::size_t count = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto source_indexes = read_indexes<index_type>(offset, count);
                            
                        undo_sort(ref, source_indexes);
                        if constexpr ( has_attached_data )
                            undo_sort(get_attached_data<Pathway...>(), source_indexes);

                        if constexpr ( has_element_moved_op<route> )
                        {
                            for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(count)-1; i>=0; --i )
                            {
                                if ( i != static_cast<std::ptrdiff_t>(source_indexes[static_cast<std::size_t>(i)]) )
                                    notify_element_moved(user, route{keys}, static_cast<std::size_t>(i), static_cast<std::size_t>(source_indexes[static_cast<std::size_t>(i)]));
                            }
                        }

                        if constexpr ( has_selections )
                        {
                            auto & sel = get_selections<Pathway...>();
                            sel_type old_sel {};
                            for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(count)-1; i>=0; --i )
                            {
                                if ( std::find(sel.begin(), sel.end(), static_cast<index_type>(i)) != sel.end() )
                                    RareTs::append(old_sel, source_indexes[static_cast<std::size_t>(i)]);
                            }

                            std::swap(sel, old_sel);

                            if constexpr ( has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::swap:
                {
                    if constexpr ( has_move_ops )
                    {
                        auto first_index = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto second_index = static_cast<std::size_t>(read_index<index_type>(offset));
                        if ( first_index != second_index && first_index < std::size(ref) && second_index < std::size(ref) )
                        {
                            std::swap(ref[first_index], ref[second_index]);
                            if constexpr ( has_attached_data )
                                mirror_swap_to(get_attached_data<Pathway...>(), first_index, second_index);

                            mirror_swap_to_selection(get_selections<Pathway...>(), first_index, second_index);
                            if constexpr ( has_element_moved_op<route> )
                            {
                                notify_element_moved(user, route{keys}, second_index, first_index);
                                notify_element_moved(user, route{keys}, first_index, second_index);
                            }
                            if constexpr ( has_selections && has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::move_up:
                {
                    if constexpr ( has_move_ops )
                    {
                        auto moved_index = static_cast<std::size_t>(read_index<index_type>(offset));
                        if ( moved_index > 0 && moved_index < std::size(ref) )
                        {
                            std::swap(ref[static_cast<std::size_t>(moved_index)], ref[static_cast<std::size_t>(moved_index-1)]);
                            if constexpr ( has_attached_data )
                                mirror_swap_to(get_attached_data<Pathway...>(), moved_index, moved_index-1);

                            mirror_swap_to_selection(get_selections<Pathway...>(), moved_index, moved_index-1);
                            if constexpr ( has_element_moved_op<route> )
                            {
                                notify_element_moved(user, route{keys}, moved_index-1, moved_index);
                                notify_element_moved(user, route{keys}, moved_index, moved_index-1);
                            }
                            if constexpr ( has_selections && has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::move_up_n:
                {
                    if constexpr ( has_move_ops )
                    {
                        auto count = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto moved_indexes = read_indexes<index_type>(offset, count);
                        if ( count > 0 )
                        {
                            std::size_t min_index_moved = 0;
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( min_index_moved == static_cast<std::size_t>(moved_indexes[i]) )
                                    ++min_index_moved;
                                else
                                    break;
                            }

                            for ( auto it = moved_indexes.rbegin(); it != moved_indexes.rend(); ++it )
                            {
                                auto i = static_cast<std::size_t>(*it);
                                if ( i > min_index_moved && i < std::size(ref) )
                                {
                                    std::swap(ref[i-1], ref[i]);
                                    if constexpr ( has_attached_data )
                                        mirror_swap_to(get_attached_data<Pathway...>(), i-1, i);

                                    mirror_swap_to_selection(get_selections<Pathway...>(), i-1, i);
                                }
                            }
                        }

                        if constexpr ( has_element_moved_op<route> )
                        {
                            std::size_t next_available = 0;
                            std::size_t block_size = 1;
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( moved_indexes[i] > next_available && static_cast<std::size_t>(moved_indexes[i]) < std::size(ref) )
                                {
                                    notify_element_moved(user, route{keys}, moved_indexes[i]-1, moved_indexes[i]);
                                    if ( i+1 < std::size(moved_indexes) && moved_indexes[i]+1 == moved_indexes[i+1] )
                                        ++block_size;
                                    else
                                    {
                                        notify_element_moved(user, route{keys}, moved_indexes[i], moved_indexes[i]-block_size);
                                        block_size = 1;
                                    }
                                }
                                else if ( moved_indexes[i] == next_available )
                                    ++next_available;
                            }
                        }
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::move_up_l:
                {
                    if constexpr ( has_selections && has_move_ops )
                    {
                        auto & sel = get_selections<Pathway...>();
                        sel_type prev_sel {};
                        read_selections(events, offset, prev_sel);

                        auto moved_indexes = prev_sel;
                        auto count = moved_indexes.size();
                        std::sort(moved_indexes.begin(), moved_indexes.end());

                        std::size_t min_index_moved = 0;
                        for ( std::size_t i=0; i<count; ++i )
                        {
                            if ( min_index_moved == moved_indexes[i] )
                                ++min_index_moved;
                            else
                                break;
                        }

                        auto size = std::size(ref);
                        for ( auto it = moved_indexes.rbegin(); it != moved_indexes.rend(); ++it )
                        {
                            auto i = static_cast<std::size_t>(*it);
                            if ( i > min_index_moved && i < size )
                            {
                                std::swap(ref[i-1], ref[i]);
                                if constexpr ( has_attached_data )
                                    mirror_swap_to(get_attached_data<Pathway...>(), i-1, i);
                            }
                        }
                        std::swap(sel, prev_sel);

                        if constexpr ( has_element_moved_op<route> )
                        {
                            std::size_t next_available = 0;
                            std::size_t block_size = 1;
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( moved_indexes[i] > next_available && static_cast<std::size_t>(moved_indexes[i]) < std::size(ref) )
                                {
                                    notify_element_moved(user, route{keys}, moved_indexes[i]-1, moved_indexes[i]);
                                    if ( i+1 < std::size(moved_indexes) && moved_indexes[i]+1 == moved_indexes[i+1] )
                                        ++block_size;
                                    else
                                    {
                                        notify_element_moved(user, route{keys}, moved_indexes[i], moved_indexes[i]-block_size);
                                        block_size = 1;
                                    }
                                }
                                else if ( moved_indexes[i] == next_available )
                                    ++next_available;
                            }
                        }
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::move_top:
                {
                    if constexpr ( has_move_ops )
                    {
                        auto moved_index = static_cast<std::size_t>(read_index<index_type>(offset));
                        if ( moved_index > 0 && moved_index < std::size(ref) )
                        {
                            auto it = ref.begin();
                            std::rotate(it, it+1, std::next(ref.begin(), static_cast<std::ptrdiff_t>(moved_index))+1);
                            if constexpr ( has_attached_data )
                                mirror_rotation_to(get_attached_data<Pathway...>(), 0, 1, moved_index+1);

                            mirror_rotation_to_selection(get_selections<Pathway...>(), 0, 1, moved_index+1);
                            if constexpr ( has_element_moved_op<route> )
                            {
                                notify_element_moved(user, route{keys}, 0, moved_index);
                                for ( std::size_t i=0; i<static_cast<std::size_t>(moved_index); ++i )
                                    notify_element_moved(user, route{keys}, i+1, i);
                            }
                            if constexpr ( has_selections && has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::move_top_n:
                {
                    if constexpr ( has_move_ops )
                    {
                        auto count = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto moved_indexes = read_indexes<index_type>(offset, count);

                        std::size_t min_index_moved = 0;
                        for ( std::size_t i=0; i<count; ++i )
                        {
                            if ( min_index_moved == static_cast<std::size_t>(moved_indexes[i]) )
                                ++min_index_moved;
                            else
                                break;
                        }

                        std::size_t size = std::size(ref);
                        std::size_t count_valid = 0;
                        for ( auto i : moved_indexes )
                        {
                            if ( i > min_index_moved && static_cast<std::size_t>(i) < size )
                                ++count_valid;
                        }

                        std::vector<std::size_t> tracked_indexes {};
                        if constexpr ( has_element_moved_op<route> )
                        {
                            tracked_indexes.assign(size, 0);
                            std::iota(tracked_indexes.begin(), tracked_indexes.end(), 0);
                        }

                        std::size_t i = min_index_moved+count_valid-1;
                        for ( auto it = moved_indexes.rbegin(); it != moved_indexes.rend(); ++it )
                        {
                            if ( static_cast<std::size_t>(*it) > min_index_moved && static_cast<std::size_t>(*it) < size )
                            {
                                auto to_move = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                auto dest = std::next(ref.begin(), static_cast<std::ptrdiff_t>(*it));
                                std::rotate(to_move, to_move+1, dest+1);
                                if constexpr ( has_attached_data )
                                    mirror_rotation_to(get_attached_data<Pathway...>(), i, i+1, *it+1);
                                if constexpr ( has_element_moved_op<route> )
                                    mirror_rotation_to(tracked_indexes, i, i+1, *it+1);

                                --i;
                            }
                        }

                        if constexpr ( has_selections )
                        {
                            auto & sel = get_selections<Pathway...>();
                            sel_type prev_sel {};
                            read_selections(events, offset, prev_sel);
                            std::swap(sel, prev_sel);
                        }
                        if constexpr ( has_element_moved_op<route> )
                        {
                            for ( i=0; i<size; ++i )
                            {
                                if ( i != tracked_indexes[i] )
                                    notify_element_moved(user, route{keys}, i, tracked_indexes[i]);
                            }
                        }
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::move_top_l:
                {
                    if constexpr ( has_selections && has_move_ops )
                    {
                        auto & sel = get_selections<Pathway...>();
                        sel_type prev_sel {};
                        read_selections(events, offset, prev_sel);

                        auto moved_indexes = prev_sel; // Copy so it can be sorted without changing selections
                        std::sort(moved_indexes.begin(), moved_indexes.end(), std::greater<index_type>{});

                        std::size_t size = std::size(ref);
                        std::vector<std::size_t> tracked_indexes {};
                        if constexpr ( has_element_moved_op<route> )
                        {
                            tracked_indexes.assign(size, 0);
                            std::iota(tracked_indexes.begin(), tracked_indexes.end(), 0);
                        }

                        std::size_t to_move_index = std::size(prev_sel)-1;
                        for ( auto i : moved_indexes )
                        {
                            if ( i > 0 && static_cast<std::size_t>(i) < size )
                            {
                                auto to_move = std::next(ref.begin(), static_cast<std::ptrdiff_t>(to_move_index));
                                auto dest = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                std::rotate(to_move, to_move+1, dest+1);
                                if constexpr ( has_attached_data )
                                    mirror_rotation_to(get_attached_data<Pathway...>(), to_move_index, to_move_index+1, i+1);
                                if constexpr ( has_element_moved_op<route> )
                                    mirror_rotation_to(tracked_indexes, to_move_index, to_move_index+1, i+1);

                                --to_move_index;
                            }
                        }
                        std::swap(sel, prev_sel);

                        if constexpr ( has_element_moved_op<route> )
                        {
                            for ( std::size_t i=0; i<size; ++i )
                            {
                                if ( i != tracked_indexes[i] )
                                    notify_element_moved(user, route{keys}, i, tracked_indexes[i]);
                            }
                        }
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::move_down:
                {
                    if constexpr ( has_move_ops )
                    {
                        auto moved_index = static_cast<std::size_t>(read_index<index_type>(offset));
                        if ( moved_index+1 < std::size(ref) )
                        {
                            std::swap(ref[static_cast<std::size_t>(moved_index)], ref[static_cast<std::size_t>(moved_index+1)]);
                            if constexpr ( has_attached_data )
                                mirror_swap_to(get_attached_data<Pathway...>(), moved_index, moved_index+1);

                            mirror_swap_to_selection(get_selections<Pathway...>(), moved_index, moved_index+1);
                            if constexpr ( has_element_moved_op<route> )
                            {
                                notify_element_moved(user, route{keys}, moved_index+1, moved_index);
                                notify_element_moved(user, route{keys}, moved_index, moved_index+1);
                            }
                            if constexpr ( has_selections && has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::move_down_n:
                {
                    if constexpr ( has_move_ops )
                    {
                        auto count = static_cast<std::size_t>(read_index<index_type>(offset));
                        if ( count > 0 )
                        {
                            auto moved_indexes = read_indexes<index_type>(offset, count);

                            std::size_t max_index_moved = std::size(ref);
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( static_cast<std::size_t>(moved_indexes[i])+1 == max_index_moved )
                                    --max_index_moved;
                                else
                                    break;
                            }

                            for ( auto it = moved_indexes.rbegin(); it != moved_indexes.rend(); ++it )
                            {
                                auto i = static_cast<std::size_t>(*it);
                                if ( i+1 < max_index_moved )
                                {
                                    std::swap(ref[i], ref[i+1]);
                                    if constexpr ( has_attached_data )
                                        mirror_swap_to(get_attached_data<Pathway...>(), i, i+1);

                                    mirror_swap_to_selection(get_selections<Pathway...>(), i, i+1);
                                }
                            }

                            if constexpr ( has_element_moved_op<route> )
                            {
                                std::size_t limit = std::size(ref);
                                std::size_t block_size = 1;
                                for ( std::size_t i=0; i<count; ++i )
                                {
                                    if ( static_cast<std::size_t>(moved_indexes[i])+1 < limit )
                                    {
                                        if ( i+1 < count && moved_indexes[i]-1 == moved_indexes[i+1] )
                                            ++block_size;
                                        else
                                        {
                                            notify_element_moved(user, route{keys}, moved_indexes[i], moved_indexes[i]+block_size);
                                            block_size = 1;
                                        }
                                        notify_element_moved(user, route{keys}, moved_indexes[i]+1, moved_indexes[i]);
                                        limit = moved_indexes[i]+1;
                                    }
                                    else if ( limit > 0 )
                                        --limit;
                                }
                            }
                            if constexpr ( has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::move_down_l:
                {
                    if constexpr ( has_selections && has_move_ops )
                    {
                        auto & sel = get_selections<Pathway...>();
                        sel_type prev_sel {};
                        read_selections(events, offset, prev_sel);

                        auto moved_indexes = prev_sel;
                        std::sort(moved_indexes.begin(), moved_indexes.end(), std::greater<index_type>{});
                        std::size_t count = std::size(moved_indexes);

                        std::size_t max_index_moved = std::size(ref);
                        for ( std::size_t i=0; i<count; ++i )
                        {
                            if ( static_cast<std::size_t>(moved_indexes[i])+1 == max_index_moved )
                                --max_index_moved;
                            else
                                break;
                        }

                        for ( auto it = moved_indexes.rbegin(); it != moved_indexes.rend(); ++it )
                        {
                            auto i = static_cast<std::size_t>(*it);
                            if ( static_cast<std::size_t>(i)+1 < max_index_moved )
                            {
                                std::swap(ref[static_cast<std::size_t>(i)], ref[static_cast<std::size_t>(i)+1]);
                                if constexpr ( has_attached_data )
                                    mirror_swap_to(get_attached_data<Pathway...>(), static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1);
                            }
                        }
                        std::swap(sel, prev_sel);

                        if constexpr ( has_element_moved_op<route> )
                        {
                            std::size_t limit = std::size(ref);
                            std::size_t block_size = 1;
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( static_cast<std::size_t>(moved_indexes[i])+1 < limit )
                                {
                                    if ( i+1 < count && moved_indexes[i]-1 == moved_indexes[i+1] )
                                        ++block_size;
                                    else
                                    {
                                        notify_element_moved(user, route{keys}, moved_indexes[i], moved_indexes[i]+block_size);
                                        block_size = 1;
                                    }
                                    notify_element_moved(user, route{keys}, moved_indexes[i]+1, moved_indexes[i]);
                                    limit = moved_indexes[i]+1;
                                }
                                else if ( limit > 0 )
                                    --limit;
                            }
                        }
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::move_bottom:
                {
                    if constexpr ( has_move_ops )
                    {
                        auto moved_index = static_cast<std::size_t>(read_index<index_type>(offset));
                        if ( moved_index+1 < std::size(ref) )
                        {
                            auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(moved_index));
                            auto moved_to = std::next(ref.begin(), static_cast<std::ptrdiff_t>(std::size(ref))-1);
                            std::rotate(it, moved_to, ref.end());
                            if constexpr ( has_attached_data )
                                mirror_rotation_to(get_attached_data<Pathway...>(), moved_index, std::size(ref)-1, std::size(ref));

                            mirror_rotation_to_selection(get_selections<Pathway...>(), moved_index, std::size(ref)-1, std::size(ref));
                            if constexpr ( has_element_moved_op<route> )
                            {
                                notify_element_moved(user, route{keys}, std::size(ref)-1, moved_index);
                                for ( std::size_t i=moved_index; i<std::size(ref)-1; ++i )
                                    notify_element_moved(user, route{keys}, i, i+1);
                            }
                            if constexpr ( has_selections && has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::move_bottom_n:
                {
                    if constexpr ( has_move_ops )
                    {
                        auto count = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto moved_indexes = read_indexes<index_type>(offset, count);

                        std::size_t size = std::size(ref);
                        std::size_t max_index_moved = size;
                        for ( std::size_t i=0; i<count; ++i )
                        {
                            if ( static_cast<std::size_t>(moved_indexes[i])+1 == max_index_moved )
                                --max_index_moved;
                            else
                                break;
                        }

                        std::size_t count_valid = 0;
                        for ( auto i : moved_indexes )
                        {
                            if ( static_cast<std::size_t>(i)+1 < max_index_moved )
                                ++count_valid;
                        }

                        std::vector<std::size_t> tracked_indexes {};
                        if constexpr ( has_element_moved_op<route> )
                        {
                            tracked_indexes.assign(size, 0);
                            std::iota(tracked_indexes.begin(), tracked_indexes.end(), 0);
                        }

                        std::size_t i = max_index_moved-count_valid;
                        for ( auto it = moved_indexes.rbegin(); it != moved_indexes.rend(); ++it )
                        {
                            if ( static_cast<std::size_t>(*it)+1 < max_index_moved )
                            {
                                auto to_move = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                auto dest = std::next(ref.begin(), static_cast<std::ptrdiff_t>(*it));
                                std::rotate(dest, to_move, to_move+1);
                                if constexpr ( has_attached_data )
                                    mirror_rotation_to(get_attached_data<Pathway...>(), *it, i, i+1);
                                if constexpr ( has_element_moved_op<route> )
                                    mirror_rotation_to(tracked_indexes, *it, i, i+1);
                                ++i;
                            }
                        }

                        if constexpr ( has_selections )
                        {
                            auto & sel = get_selections<Pathway...>();
                            sel_type prev_sel {};
                            read_selections(events, offset, prev_sel);
                            std::swap(sel, prev_sel);
                        }

                        if constexpr ( has_element_moved_op<route> )
                        {
                            for ( i=0; i<size; ++i )
                            {
                                if ( i != tracked_indexes[i] )
                                    notify_element_moved(user, route{keys}, tracked_indexes[i], i);
                            }
                        }
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::move_bottom_l:
                {
                    if constexpr ( has_selections && has_move_ops )
                    {
                        auto & sel = get_selections<Pathway...>();
                        sel_type prev_sel {};
                        read_selections(events, offset, prev_sel);

                        auto moved_indexes = prev_sel; // Copy so it can be sorted without changing selections
                        std::sort(moved_indexes.begin(), moved_indexes.end());

                        std::size_t size = std::size(ref);
                        std::vector<std::size_t> tracked_indexes {};
                        if constexpr ( has_element_moved_op<route> )
                        {
                            tracked_indexes.assign(size, 0);
                            std::iota(tracked_indexes.begin(), tracked_indexes.end(), 0);
                        }

                        std::size_t to_move_index = std::size(ref)-std::size(prev_sel);
                        for ( auto i : moved_indexes )
                        {
                            if ( static_cast<std::size_t>(i)+1 < size )
                            {
                                auto to_move = std::next(ref.begin(), static_cast<std::ptrdiff_t>(to_move_index));
                                auto dest = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                std::rotate(dest, to_move, to_move+1);
                                if constexpr ( has_attached_data )
                                    mirror_rotation_to(get_attached_data<Pathway...>(), i, to_move_index, to_move_index+1);
                                if constexpr ( has_element_moved_op<route> )
                                    mirror_rotation_to(tracked_indexes, i, to_move_index, to_move_index+1);
                                ++to_move_index;
                            }
                        }

                        std::swap(sel, prev_sel);

                        if constexpr ( has_element_moved_op<route> )
                        {
                            for ( std::size_t i=0; i<size; ++i )
                            {
                                if ( i != tracked_indexes[i] )
                                    notify_element_moved(user, route{keys}, tracked_indexes[i], i);
                            }
                        }
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::move_to:
                {
                    if constexpr ( has_move_ops )
                    {
                        auto index_moved_to = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto moved_index = static_cast<std::size_t>(read_index<index_type>(offset));
                            
                        auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(moved_index));
                        auto target = std::next(ref.begin(), static_cast<std::ptrdiff_t>(index_moved_to));
                        if ( index_moved_to < moved_index && moved_index < std::size(ref) )
                        {
                            std::rotate(target, target+1, it+1);
                            if constexpr ( has_attached_data )
                                mirror_rotation_to(get_attached_data<Pathway...>(), index_moved_to, index_moved_to+1, moved_index+1);

                            mirror_rotation_to_selection(get_selections<Pathway...>(), index_moved_to, index_moved_to+1, moved_index+1);
                            if constexpr ( has_element_moved_op<route> )
                            {
                                notify_element_moved(user, route{keys}, static_cast<std::size_t>(index_moved_to), static_cast<std::size_t>(moved_index));
                                for ( std::size_t i=static_cast<std::size_t>(index_moved_to)+1; i<static_cast<std::size_t>(moved_index)+1; ++i )
                                    notify_element_moved(user, route{keys}, i, i-1);
                            }
                            if constexpr ( has_selections && has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                        else if ( index_moved_to > moved_index && index_moved_to < std::size(ref) )
                        {
                            std::rotate(it, target, target+1);
                            if constexpr ( has_attached_data )
                                mirror_rotation_to(get_attached_data<Pathway...>(), moved_index, index_moved_to, index_moved_to+1);

                            mirror_rotation_to_selection(get_selections<Pathway...>(), moved_index, index_moved_to, index_moved_to+1);
                            if constexpr ( has_element_moved_op<route> )
                            {
                                notify_element_moved(user, route{keys}, static_cast<std::size_t>(index_moved_to), static_cast<std::size_t>(moved_index));
                                for ( std::size_t i=static_cast<std::size_t>(moved_index); i<static_cast<std::size_t>(index_moved_to); ++i )
                                    notify_element_moved(user, route{keys}, i, i+1);
                            }
                            if constexpr ( has_selections && has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::move_to_n:
                {
                    if constexpr ( has_move_ops )
                    {
                        // Calculate the rotates you would perform in the forward direction, then reverse the rotations
                        std::vector<rotation> rotations {};
                            
                        auto index_moved_to = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto count_moved_indexes = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto moved_indexes = read_indexes<index_type>(offset, count_moved_indexes);

                        auto size = std::size(ref);
                        if ( !std::empty(moved_indexes) )
                        {
                            std::size_t count_valid_indexes = 0;
                            for ( auto moved_index : moved_indexes )
                            {
                                if ( moved_index >= 0 && static_cast<std::size_t>(moved_index) < std::size(ref) )
                                    ++count_valid_indexes;
                            }
                            if ( count_valid_indexes == 0 )
                                return;

                            if ( static_cast<std::size_t>(index_moved_to) + count_valid_indexes >= size ) // Operation collapses to move_to_end
                            {
                                std::ptrdiff_t insertion_index = static_cast<std::ptrdiff_t>(size);
                                for ( auto mit = moved_indexes.rbegin(); mit != moved_indexes.rend(); ++mit ) // moved_indexes sorted least to greatest, so reverse read
                                {
                                    auto i = *mit;
                                    if ( i >= 0 && static_cast<std::ptrdiff_t>(i)+1 < insertion_index )
                                        rotations.push_back(rotation::make_reverse(static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertion_index)));

                                    --insertion_index;
                                }
                            }
                            else // Non-collapsed move_to operation
                            {
                                std::size_t min_valid_index = 0;
                                std::size_t max_valid_index = 0;
                                for ( std::size_t i=0; i<std::size(moved_indexes); ++i )
                                {
                                    if ( static_cast<std::ptrdiff_t>(moved_indexes[i]) >= 0 )
                                    {
                                        min_valid_index = static_cast<std::size_t>(moved_indexes[i]);
                                        break;
                                    }
                                }
                                for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(std::size(moved_indexes))-1; i>=0; --i )
                                {
                                    if ( static_cast<std::ptrdiff_t>(moved_indexes[static_cast<std::size_t>(i)]) < static_cast<std::ptrdiff_t>(size) )
                                    {
                                        max_valid_index = static_cast<std::size_t>(moved_indexes[static_cast<std::size_t>(i)]);
                                        break;
                                    }
                                }

                                std::size_t left_chunk_size = min_valid_index < static_cast<std::size_t>(index_moved_to) ? 1 : 0;
                                std::size_t right_chunk_size = max_valid_index >= static_cast<std::size_t>(index_moved_to) ? 1 : 0;
                                std::size_t left_chunk_first = min_valid_index;
                                std::size_t right_chunk_first = max_valid_index;

                                for ( std::size_t i=0; i+1<std::size(moved_indexes) && static_cast<std::size_t>(moved_indexes[i+1]) < index_moved_to; ++i )
                                {
                                    // rotate left chunk together with element[moved_indexes[i+1]]
                                    rotations.push_back(rotation::make_reverse(left_chunk_first, left_chunk_first+left_chunk_size, moved_indexes[i+1]));
                                    left_chunk_first = static_cast<std::size_t>(moved_indexes[i+1])-left_chunk_size;
                                    ++left_chunk_size;
                                }
                                for ( std::size_t i=std::size(moved_indexes)-1; i>0 && static_cast<std::size_t>(moved_indexes[i-1]) >= index_moved_to; --i )
                                {
                                    // rotate right chunk together with element[moved_indexes[i-1]]
                                    rotations.push_back(rotation::make_reverse(static_cast<std::size_t>(moved_indexes[i-1])+1, right_chunk_first, right_chunk_first+right_chunk_size));
                                    right_chunk_first = static_cast<std::size_t>(moved_indexes[i-1]);
                                    ++right_chunk_size;
                                }
                                if ( left_chunk_first+left_chunk_size != right_chunk_first && left_chunk_size > 0 && right_chunk_size > 0 ) // Need to combine the chunks then rotate combined to final position
                                {
                                    std::size_t left_distance = left_chunk_first < static_cast<std::size_t>(index_moved_to) ? static_cast<std::size_t>(index_moved_to)-left_chunk_first : left_chunk_first-static_cast<std::size_t>(index_moved_to);
                                    std::size_t right_distance = right_chunk_first < static_cast<std::size_t>(index_moved_to)+left_chunk_size ? static_cast<std::size_t>(index_moved_to)+left_chunk_size-right_chunk_first : right_chunk_first-static_cast<std::size_t>(index_moved_to)-left_chunk_size;
                                    if ( right_distance < left_distance ) // Right chunk closer to target, move left up to right
                                    {
                                        rotations.push_back(rotation::make_reverse(left_chunk_first, left_chunk_first+left_chunk_size, right_chunk_first));
                                        left_chunk_first = right_chunk_first-left_chunk_size;
                                    }
                                    else // Left chunk closer to target, move right up to left
                                    {
                                        rotations.push_back(rotation::make_reverse(left_chunk_first+left_chunk_size, right_chunk_first, right_chunk_first+right_chunk_size));
                                        right_chunk_first = left_chunk_first+left_chunk_size;
                                    }
                                }
                                if ( left_chunk_first < static_cast<std::size_t>(index_moved_to) ) // Rotate combined chunk rightwards to final position
                                    rotations.push_back(rotation::make_reverse(left_chunk_first, left_chunk_first+count_valid_indexes, index_moved_to+left_chunk_size+right_chunk_size));
                                else if ( left_chunk_first > static_cast<std::size_t>(index_moved_to) ) // Rotate combined chunk leftwards to final position
                                    rotations.push_back(rotation::make_reverse(index_moved_to, left_chunk_first, left_chunk_first+count_valid_indexes));
                            }
                        }

                        std::vector<std::size_t> tracked_indexes {};
                        if constexpr ( has_element_moved_op<route> )
                        {
                            tracked_indexes.assign(size, 0);
                            std::iota(tracked_indexes.begin(), tracked_indexes.end(), 0);
                        }
                        for ( auto it = rotations.rbegin(); it != rotations.rend(); ++it )
                        {
                            it->perform(ref);
                            if constexpr ( has_attached_data )
                                it->perform(get_attached_data<Pathway...>());
                            if constexpr ( has_element_moved_op<route> )
                                it->perform(tracked_indexes);
                        }

                        if constexpr ( has_selections )
                        {
                            auto & sel = get_selections<Pathway...>();
                            sel_type prev_sel {};
                            read_selections(events, offset, prev_sel);
                            std::swap(sel, prev_sel);
                        }
                    
                        if constexpr ( has_element_moved_op<route> )
                        {
                            for ( std::size_t i=0; i<size; ++i )
                            {
                                if ( i != tracked_indexes[i] )
                                    notify_element_moved(user, route{keys}, tracked_indexes[i], i);
                            }
                        }
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::move_to_l:
                {
                    if constexpr ( has_selections && has_move_ops )
                    {
                        // Calculate the rotates you would perform in the forward direction, then reverse the rotations
                        std::vector<rotation> rotations {};
                            
                        auto index_moved_to = static_cast<std::size_t>(read_index<index_type>(offset));
                            
                        auto & sel = get_selections<Pathway...>();
                        sel_type prev_sel {};
                        read_selections(events, offset, prev_sel);
                        auto moved_indexes = prev_sel; // Copy so it can be sorted without changing selections
                        std::sort(moved_indexes.begin(), moved_indexes.end());

                        auto size = std::size(ref);
                        if ( !std::empty(moved_indexes) )
                        {
                            std::size_t count_valid_indexes = 0;
                            for ( auto moved_index : moved_indexes )
                            {
                                if ( moved_index >= 0 && static_cast<std::size_t>(moved_index) < std::size(ref) )
                                    ++count_valid_indexes;
                            }
                            if ( count_valid_indexes == 0 )
                                return;

                            if ( static_cast<std::size_t>(index_moved_to) + count_valid_indexes >= size ) // Operation collapses to move_to_end
                            {
                                std::ptrdiff_t insertion_index = static_cast<std::ptrdiff_t>(size);
                                for ( auto mit = moved_indexes.rbegin(); mit != moved_indexes.rend(); ++mit ) // moved_indexes sorted least to greatest, so reverse read
                                {
                                    auto i = *mit;
                                    if ( i >= 0 && static_cast<std::ptrdiff_t>(i)+1 < insertion_index )
                                        rotations.push_back(rotation::make_reverse(i, static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertion_index)));

                                    --insertion_index;
                                }
                            }
                            else // Non-collapsed move_to operation
                            {
                                std::size_t min_valid_index = 0;
                                std::size_t max_valid_index = 0;
                                for ( std::size_t i=0; i<std::size(moved_indexes); ++i )
                                {
                                    if ( moved_indexes[i] >= 0 )
                                    {
                                        min_valid_index = static_cast<std::size_t>(moved_indexes[i]);
                                        break;
                                    }
                                }
                                for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(std::size(moved_indexes))-1; i>=0; --i )
                                {
                                    if ( static_cast<std::ptrdiff_t>(moved_indexes[static_cast<std::size_t>(i)]) < static_cast<std::ptrdiff_t>(size) )
                                    {
                                        max_valid_index = static_cast<std::size_t>(moved_indexes[static_cast<std::size_t>(i)]);
                                        break;
                                    }
                                }

                                std::size_t left_chunk_size = min_valid_index < static_cast<std::size_t>(index_moved_to) ? 1 : 0;
                                std::size_t right_chunk_size = max_valid_index >= static_cast<std::size_t>(index_moved_to) ? 1 : 0;
                                std::size_t left_chunk_first = min_valid_index;
                                std::size_t right_chunk_first = max_valid_index;

                                for ( std::size_t i=0; i+1<std::size(moved_indexes) && moved_indexes[i+1] < index_moved_to; ++i )
                                {
                                    // rotate left chunk together with element[moved_indexes[i+1]]
                                    rotations.push_back(rotation::make_reverse(left_chunk_first, left_chunk_first+left_chunk_size, moved_indexes[i+1]));
                                    left_chunk_first = static_cast<std::size_t>(moved_indexes[i+1])-left_chunk_size;
                                    ++left_chunk_size;
                                }
                                for ( std::size_t i=std::size(moved_indexes)-1; i>0 && moved_indexes[i-1] >= index_moved_to; --i )
                                {
                                    // rotate right chunk together with element[moved_indexes[i-1]]
                                    rotations.push_back(rotation::make_reverse(static_cast<std::size_t>(moved_indexes[i-1])+1, right_chunk_first, right_chunk_first+right_chunk_size));
                                    right_chunk_first = static_cast<std::size_t>(moved_indexes[i-1]);
                                    ++right_chunk_size;
                                }
                                if ( left_chunk_first+left_chunk_size != right_chunk_first && left_chunk_size > 0 && right_chunk_size > 0 ) // Need to combine the chunks then rotate combined to final position
                                {
                                    std::size_t left_distance = left_chunk_first < static_cast<std::size_t>(index_moved_to) ? static_cast<std::size_t>(index_moved_to)-left_chunk_first : left_chunk_first-static_cast<std::size_t>(index_moved_to);
                                    std::size_t right_distance = right_chunk_first < static_cast<std::size_t>(index_moved_to)+left_chunk_size ? static_cast<std::size_t>(index_moved_to)+left_chunk_size-right_chunk_first : right_chunk_first-static_cast<std::size_t>(index_moved_to)-left_chunk_size;
                                    if ( right_distance < left_distance ) // Right chunk closer to target, move left up to right
                                    {
                                        rotations.push_back(rotation::make_reverse(left_chunk_first, left_chunk_first+left_chunk_size, right_chunk_first));
                                        left_chunk_first = right_chunk_first-left_chunk_size;
                                    }
                                    else // Left chunk closer to target, move right up to left
                                    {
                                        rotations.push_back(rotation::make_reverse(left_chunk_first+left_chunk_size, right_chunk_first, right_chunk_first+right_chunk_size));
                                        right_chunk_first = left_chunk_first+left_chunk_size;
                                    }
                                }
                                if ( left_chunk_first < static_cast<std::size_t>(index_moved_to) ) // Rotate combined chunk rightwards to final position
                                    rotations.push_back(rotation::make_reverse(left_chunk_first, left_chunk_first+count_valid_indexes, index_moved_to+left_chunk_size+right_chunk_size));
                                else if ( left_chunk_first > static_cast<std::size_t>(index_moved_to) ) // Rotate combined chunk leftwards to final position
                                    rotations.push_back(rotation::make_reverse(index_moved_to, left_chunk_first, left_chunk_first+count_valid_indexes));
                            }
                        }

                        std::vector<std::size_t> tracked_indexes {};
                        if constexpr ( has_element_moved_op<route> )
                        {
                            tracked_indexes.assign(size, 0);
                            std::iota(tracked_indexes.begin(), tracked_indexes.end(), 0);
                        }
                        for ( auto it = rotations.rbegin(); it != rotations.rend(); ++it )
                        {
                            it->perform(ref);
                            if constexpr ( has_attached_data )
                                it->perform(get_attached_data<Pathway...>());
                            if constexpr ( has_element_moved_op<route> )
                                it->perform(tracked_indexes);
                        }

                        std::swap(sel, prev_sel);
                    
                        if constexpr ( has_element_moved_op<route> )
                        {
                            for ( std::size_t i=0; i<size; ++i )
                            {
                                if ( i != tracked_indexes[i] )
                                    notify_element_moved(user, route{keys}, tracked_indexes[i], i);
                            }
                        }
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
            }
        }

        template <class Value_type, class Member, class ... Pathway>
        void process_redo_event(std::uint8_t operation, std::size_t & offset, std::optional<std::size_t> & secondary_offset, auto & ref, auto keys)
        {
            using index_type = index_type_t<default_index_type, Member>;
            using path_pack = type_tags<Pathway...>;
            using sel_type = std::remove_cvref_t<decltype(get_selections<Pathway...>())>;
            using element_type = RareTs::element_type_t<Value_type>;
            using attached_data_type = std::remove_cvref_t<decltype(get_attached_data<Pathway...>())>;
            using route = path_tagged_keys<decltype(keys), path_pack, Editor_type>;
            using elem_path = type_tags<Pathway..., path_index<std::tuple_size_v<std::remove_cvref_t<decltype(keys)>>>>;
            using elem_keys = std::remove_cvref_t<decltype(std::tuple_cat(keys, std::tuple<index_type>{0}))>;
            using elem_route = path_tagged_keys<elem_keys, elem_path, Editor_type>;
            constexpr bool has_attached_data = agent::has_attached_data<Pathway...>();
            constexpr bool has_selections = !std::is_null_pointer_v<sel_type> && RareTs::is_specialization_v<sel_type, std::vector>;
            constexpr bool has_sel_change_op = has_selections && has_selections_changed_op<route>;
            constexpr bool is_iterable = RareTs::is_iterable_v<Value_type>;
            constexpr bool has_move_ops = !std::is_void_v<element_type> && requires { ref.begin(); };

            switch ( op(operation) )
            {
                case op::init:
                {
                    if constexpr ( RareTs::is_assignable_v<decltype(ref), Value_type> )
                        ref = read_value<Value_type, Member>(offset);
                }
                break;
                case op::reset:
                {
                    if constexpr ( is_iterable && has_element_removed_op<route> ) // Iterable
                    {
                        for ( std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1; i>=0; --i )
                            notify_element_removed(user, route{keys}, static_cast<std::size_t>(i)); // Issue remove changes

                        ref = {};
                        if constexpr ( has_attached_data )
                            get_attached_data<Pathway...>() = {};
                    }
                    else if constexpr ( !is_iterable && has_value_changed_op<route, Value_type> ) // Non-iterables
                    {
                        if constexpr ( std::is_array_v<typename Member::type> )
                        {
                            typename Member::type prev_value {};
                            auto ref_array = as_1d<typename Member::type>(ref);
                            as_1d(prev_value) = ref_array;
                            ref_array.clear();
                            notify_value_changed(user, route{keys}, prev_value, ref); // Issue change notification
                        }
                        else
                        {
                            auto prev_value = ref;
                            ref = {};
                            notify_value_changed(user, route{keys}, prev_value, ref); // Issue change notification
                        }
                    }
                    else
                    {
                        if constexpr ( std::is_array_v<Value_type> && requires { ref[0]; } )
                            as_1d<Value_type>(ref).clear();
                        else
                        {
                            ref = {};
                            if constexpr ( has_attached_data )
                                get_attached_data<Pathway...>() = {};
                        }
                    }
                            
                    if constexpr ( has_selections )
                    {
                        auto & sel = get_selections<Pathway...>();
                        if constexpr ( has_sel_change_op )
                        {
                            if ( !std::empty(sel) )
                            {
                                clear_sel(sel);
                                notify_selections_changed(user, route{keys});
                            }
                        }
                        else
                            clear_sel(sel);
                    }
                }
                break;
                case op::reserve:
                {
                    if constexpr ( RareTs::is_specialization_v<Value_type, std::vector> )
                        ref.reserve(static_cast<std::size_t>(read_index<index_type>(offset)));
                }
                break;
                case op::trim:
                {
                    if constexpr ( RareTs::is_specialization_v<Value_type, std::vector> )
                        ref.shrink_to_fit();
                }
                break;
                case op::assign:
                {
                    if constexpr ( !std::is_void_v<element_type> && !RareTs::is_static_array_v<Value_type> &&
                        !RareTs::is_optional_v<Value_type> && !std::is_same_v<std::string, Value_type> && requires{ref.assign(0, {});} )
                    {
                        std::size_t count = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto value = read_value<element_type, Member>(offset);

                        if constexpr ( has_element_removed_op<route> )
                        {
                            for ( std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1; i>=0; --i )
                                notify_element_removed(user, route{keys}, static_cast<std::size_t>(i)); // Issue remove changes
                        }
                        ref.assign(count, value);
                        if constexpr ( has_attached_data )
                            get_attached_data<Pathway...>() = attached_data_type(count);

                        if constexpr ( has_element_added_op<route> )
                        {
                            for ( std::size_t i=0; i<std::size(ref); ++i )
                                notify_element_added(user, route{keys}, i);
                        }

                        if constexpr ( has_selections )
                        {
                            auto & sel = get_selections<Pathway...>();
                            write_selections(events, sel);
                            if constexpr ( has_sel_change_op )
                            {
                                if ( !std::empty(sel) )
                                {
                                    clear_sel(sel);
                                    notify_selections_changed(user, route{keys});
                                }
                            }
                            else
                                clear_sel(sel);
                        }
                    }
                }
                break;
                case op::assign_default:
                {
                    if constexpr ( !std::is_void_v<element_type> && !RareTs::is_static_array_v<Value_type> &&
                        !RareTs::is_optional_v<Value_type> && !std::is_same_v<std::string, Value_type> && requires{ref = Value_type(0);} )
                    {
                        std::size_t size = static_cast<std::size_t>(read_index<index_type>(offset));
                        if constexpr ( has_element_removed_op<route> )
                        {
                            for ( std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1; i>=0; --i )
                                notify_element_removed(user, route{keys}, static_cast<std::size_t>(i)); // Issue remove changes
                        }
                        ref = Value_type(size);
                        if constexpr ( has_attached_data )
                            get_attached_data<Pathway...>() = attached_data_type(size);

                        if constexpr ( has_element_added_op<route> )
                        {
                            for ( std::size_t i=0; i<std::size(ref); ++i )
                                notify_element_added(user, route{keys}, i);
                        }

                        if constexpr ( has_selections )
                        {
                            auto & sel = get_selections<Pathway...>();
                            write_selections(events, sel);
                            if constexpr ( has_sel_change_op )
                            {
                                if ( !std::empty(sel) )
                                {
                                    clear_sel(sel);
                                    notify_selections_changed(user, route{keys});
                                }
                            }
                            else
                                clear_sel(sel);
                        }

                    }
                }
                break;
                case op::clear_selections:
                {
                    auto & sel = get_selections<Pathway...>();
                    clear_sel(sel);

                    if constexpr ( has_sel_change_op )
                        notify_selections_changed(user, route{keys});
                }
                break;
                case op::select_all:
                {
                    if constexpr ( has_selections && requires { std::size(ref); } )
                    {
                        auto & sel = get_selections<Pathway...>();
                        RareTs::clear(sel);
                        sel.assign(std::size(ref), index_type{0});
                        std::iota(sel.begin(), sel.end(), index_type{0});

                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::select:
                {
                    if constexpr ( has_selections )
                    {
                        auto & sel = get_selections<Pathway...>();
                        auto value = read_index<index_type>(offset);
                        if ( std::find(sel.begin(), sel.end(), value) == sel.end() )
                            sel.push_back(value);
                        
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::select_n:
                {
                    if constexpr ( has_selections )
                    {
                        auto & sel = get_selections<Pathway...>();
                        auto size = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto sel_indexes = read_indexes<index_type>(offset, size);
                        for ( std::size_t i=0; i<size; ++i )
                        {
                            if ( std::find(sel.begin(), sel.end(), sel_indexes[i]) == sel.end() )
                                sel.push_back(sel_indexes[i]);
                        }
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::deselect:
                {
                    if constexpr ( has_selections )
                    {
                        auto & sel = get_selections<Pathway...>();
                        auto value = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto found = std::find(sel.begin(), sel.end(), value);
                        if ( found != sel.end() )
                            sel.erase(found);
                        
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::deselect_n:
                {
                    if constexpr ( has_selections )
                    {
                        auto & sel = get_selections<Pathway...>();
                        auto size = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto sel_indexes = read_indexes<index_type>(offset, size);
                        for ( std::size_t i=0; i<size; ++i )
                        {
                            auto found = std::find(sel.begin(), sel.end(), sel_indexes[i]);
                            if ( found != sel.end() )
                                sel.erase(found);
                        }
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::toggle_selection:
                {
                    if constexpr ( has_selections )
                    {
                        auto & sel = get_selections<Pathway...>();
                        auto value = read_index<index_type>(offset);
                        read_index<index_type>(offset);
                        auto found = std::find(sel.begin(), sel.end(), value);
                        if ( found == sel.end() )
                            RareTs::append(sel, value);
                        else
                            sel.erase(found);
                        
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::toggle_selection_n:
                {
                    if constexpr ( has_selections )
                    {
                        auto & sel = get_selections<Pathway...>();
                        auto size = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto sel_indexes = read_indexes<index_type>(offset, size);
                        for ( std::size_t i=0; i<size; ++i )
                        {
                            auto found = std::find(sel.begin(), sel.end(), sel_indexes[i]);
                            if ( found == sel.end() )
                                RareTs::append(sel, sel_indexes[i]);
                            else
                                sel.erase(found);
                        }
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::sort_selections:
                {
                    if constexpr ( has_selections && requires{redo_sort(get_selections<Pathway...>(), read_indexes<index_type>(offset, 0));} )
                    {
                        auto & sel = get_selections<Pathway...>();
                        auto size = read_index<index_type>(offset);
                        auto source_indexes = read_indexes<index_type>(offset, size);

                        redo_sort(sel, source_indexes);

                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::sort_selections_desc:
                {
                    if constexpr ( has_selections && requires{redo_sort(get_selections<Pathway...>(), read_indexes<index_type>(offset, 0));} )
                    {
                        auto & sel = get_selections<Pathway...>();
                        auto size = read_index<index_type>(offset);
                        auto source_indexes = read_indexes<index_type>(offset, size);

                        redo_sort(sel, source_indexes);

                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::set:
                {
                    if constexpr ( RareTs::is_assignable_v<decltype(ref), Value_type> )
                    {
                        auto new_value = read_value<Value_type, Member>(offset);
                        read_value<Value_type, Member>(offset); // prev_value (unused)
                        if constexpr ( !is_iterable && has_value_changed_op<route, Value_type> )
                        {
                            auto temp = ref;
                            ref = new_value;
                            notify_value_changed(user, route{keys}, temp, ref);
                        }
                        else
                        {
                            if constexpr ( is_iterable && has_element_removed_op<route> )
                            {
                                for ( std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1; i>=0; --i )
                                    notify_element_removed(user, route{keys}, static_cast<std::size_t>(i));
                            }
                            ref = new_value;
                            if constexpr ( has_attached_data )
                                get_attached_data<Pathway...>() = attached_data_type(std::size(ref));

                            if constexpr ( is_iterable && has_element_added_op<route> )
                            {
                                for ( std::size_t i=0; i<std::size(ref); ++i )
                                    notify_element_added(user, route{keys}, i);
                            }
                        }

                        if constexpr ( has_selections )
                        {
                            clear_sel(get_selections<Pathway...>());
                            if constexpr ( has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::set_n:
                {
                    if constexpr ( !std::is_void_v<element_type> && requires{ref[0] = std::declval<element_type>();} )
                    {
                        constexpr bool is_iterable_element = RareTs::is_iterable_v<element_type>;
                        auto count = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto set_indexes = read_indexes<index_type>(offset, count);

                        auto new_value = read_value<element_type, Member>(offset);

                        if constexpr ( !is_iterable_element && has_value_changed_op<route, element_type> )
                        {
                            for ( auto index : set_indexes )
                            {
                                auto prev_value = ref[index];
                                ref[index] = new_value;
                                notify_value_changed(user, elem_route{std::tuple_cat(keys, std::tuple<index_type>{index})}, prev_value, ref[index]);
                            }
                        }
                        else if constexpr ( is_iterable_element && has_element_removed_op<route> )
                        {
                            for ( auto index : set_indexes )
                            {
                                for ( std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref[index]))-1; i>=0; --i )
                                    notify_element_removed(user, elem_route{std::tuple_cat(keys, std::tuple<index_type>{index})}, static_cast<std::size_t>(i));

                                ref[index] = new_value;

                                if constexpr ( has_element_added_op<route> )
                                {
                                    for ( std::size_t j=0; j<std::size(ref[index]); ++j )
                                        notify_element_added(user, elem_route{std::tuple_cat(keys, std::tuple<index_type>{index})}, j);
                                }
                            }
                        }
                        else
                        {
                            for ( auto index : set_indexes )
                            {
                                ref[index] = new_value;
                                if constexpr ( is_iterable_element && has_element_added_op<route> )
                                {
                                    for ( std::size_t i=0; i<std::size(ref[index]); ++i )
                                        notify_element_added(user, elem_route{std::tuple_cat(keys, std::tuple<index_type>{index})}, i);
                                }
                            }
                        }
                    }
                }
                break;
                case op::set_l:
                {
                    if constexpr ( RareTs::is_assignable_v<decltype(ref), Value_type> )
                    {
                        if ( !secondary_offset )
                            secondary_offset = offset; // Is set exclusively for the first visit of the selection

                        if constexpr ( has_value_changed_op<route, Value_type> )
                        {
                            auto temp = ref;
                            ref = peek_value<Value_type, Member>(*secondary_offset);
                            notify_value_changed(user, route{keys}, temp, ref);
                        }
                        else
                        {
                            if constexpr ( is_iterable && has_element_removed_op<route> )
                            {
                                for ( std::ptrdiff_t i = static_cast<std::ptrdiff_t>(std::size(ref))-1; i>=0; --i )
                                    notify_element_removed(user, route{keys}, static_cast<std::size_t>(i));
                            }
                            ref = peek_value<Value_type, Member>(*secondary_offset);

                            if constexpr ( is_iterable && has_element_added_op<route> )
                            {
                                for ( std::size_t i=0; i<std::size(ref); ++i )
                                    notify_element_added(user, route{keys}, i);
                            }
                        }
                    }
                }
                break;
                case op::append:
                {
                    if constexpr ( RareTs::is_specialization_v<Value_type, std::vector> )
                    {
                        auto value = read_value<element_type, Member>(offset);
                        ref.push_back(value);
                        if constexpr ( has_attached_data )
                            get_attached_data<Pathway...>().push_back({});
                        if constexpr ( has_element_added_op<route> )
                            notify_element_added(user, route{keys}, ref.size()-1);
                    }
                }
                break;
                case op::append_n:
                {
                    if constexpr ( !std::is_void_v<element_type> && !RareTs::is_static_array_v<Value_type> &&
                        !RareTs::is_optional_v<Value_type> && !std::is_same_v<std::string, Value_type> && requires{ref.push_back(element_type{});} )
                    {
                        auto count = static_cast<std::size_t>(read_index<index_type>(offset));
                        for ( std::size_t i=0; i<count; ++i )
                        {
                            ref.push_back(read_value<element_type, Member>(offset));
                            if constexpr ( has_attached_data )
                                get_attached_data<Pathway...>().push_back({});
                            if constexpr ( has_element_added_op<route> )
                                notify_element_added(user, route{keys}, ref.size()-1);
                        }
                    }
                }
                break;
                case op::insert:
                {
                    if constexpr ( !std::is_void_v<element_type> && !RareTs::is_static_array_v<Value_type> &&
                        !RareTs::is_optional_v<Value_type> && !std::is_same_v<std::string, Value_type> && requires{ref.insert(ref.begin(), std::declval<element_type>());} )
                    {
                        auto insertion_index = read_index<index_type>(offset);
                        auto inserted_value = read_value<element_type, Member>(offset);
                        ref.insert(std::next(ref.begin(), static_cast<std::ptrdiff_t>(insertion_index)), inserted_value);
                        if constexpr ( has_attached_data )
                        {
                            auto & attached_data = get_attached_data<Pathway...>();
                            attached_data.emplace(std::next(attached_data.begin(), static_cast<std::ptrdiff_t>(insertion_index)));
                        }
                        
                        if constexpr ( has_element_added_op<route> )
                            notify_element_added(user, route{keys}, static_cast<std::size_t>(insertion_index));

                        if constexpr ( has_element_moved_op<route> )
                        {
                            for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(std::size(ref))-1; i>static_cast<std::ptrdiff_t>(insertion_index); --i )
                                notify_element_moved(user, route{keys}, static_cast<std::size_t>(i-1), static_cast<std::size_t>(i));
                        }

                        if constexpr ( has_selections )
                        {
                            auto & sel = get_selections<Pathway...>();
                            for ( auto & index : sel )
                            {
                                if ( static_cast<index_type>(index)  >= insertion_index )
                                    ++index;
                            }
                            if constexpr ( has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::insert_n:
                {
                    if constexpr ( !std::is_void_v<element_type> && !RareTs::is_static_array_v<Value_type> &&
                        !RareTs::is_optional_v<Value_type> && !std::is_same_v<std::string, Value_type> && requires {ref.insert(ref.begin(), std::declval<element_type>());} )
                    {
                        std::size_t insertion_count = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto insertion_index = read_index<index_type>(offset);
                        for ( std::size_t i=0; i<insertion_count; ++i )
                        {
                            ref.insert(
                                std::next(ref.begin(), static_cast<std::ptrdiff_t>(insertion_index+i)),
                                read_value<element_type, Member>(offset)
                            );
                            if constexpr ( has_attached_data )
                            {
                                auto & attached_data = get_attached_data<Pathway...>();
                                attached_data.emplace(std::next(attached_data.begin(), static_cast<std::ptrdiff_t>(insertion_index+i)));
                            }
                        }

                        if constexpr ( has_element_added_op<route> )
                        {
                            auto limit = static_cast<std::size_t>(insertion_index) + insertion_count;
                            for ( auto i = static_cast<std::size_t>(insertion_index); i < limit; ++i )
                                notify_element_added(user, route{keys}, i);
                        }

                        if constexpr ( has_element_moved_op<route> )
                        {
                            std::size_t prev_size = std::size(ref) - insertion_count;
                            for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(prev_size)-1; i>=static_cast<std::ptrdiff_t>(insertion_index); --i )
                                notify_element_moved(user, route{keys}, static_cast<std::size_t>(i), static_cast<std::size_t>(i)+insertion_count);
                        }

                        if constexpr ( has_selections )
                        {
                            auto & sel = get_selections<Pathway...>();
                            for ( auto & index : sel )
                            {
                                if ( static_cast<index_type>(index) >= insertion_index )
                                    index += static_cast<index_type>(insertion_count);
                            }
                            if constexpr ( has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::remove:
                {
                    if constexpr ( !std::is_void_v<element_type> && !RareTs::is_static_array_v<Value_type> &&
                        !RareTs::is_optional_v<Value_type> && !std::is_same_v<std::string, Value_type> && requires{ref.erase(ref.begin());} )
                    {
                        auto removal_index = read_index<index_type>(offset);
                        if constexpr ( has_element_removed_op<route> )
                            notify_element_removed(user, route{keys}, static_cast<std::size_t>(removal_index));

                        ref.erase(std::next(ref.begin(), static_cast<std::ptrdiff_t>(removal_index)));
                        if constexpr ( has_attached_data )
                        {
                            auto & attached_data = get_attached_data<Pathway...>();
                            attached_data.erase(std::next(attached_data.begin(), static_cast<std::ptrdiff_t>(removal_index)));
                        }

                        if constexpr ( has_element_moved_op<route> )
                        {
                            for ( std::size_t i=static_cast<std::size_t>(removal_index); i<std::size(ref); ++i )
                                notify_element_moved(user, route{keys}, i+1, i);
                        }

                        if constexpr ( has_selections )
                        {
                            auto & sel = get_selections<Pathway...>();
                            auto found = sel.end();
                            for ( auto it = sel.begin(); it != sel.end(); ++it )
                            {
                                if ( *it == removal_index )
                                    found = it;
                                else if ( *it > removal_index )
                                    --(*it);
                            }
                            if ( found != sel.end() )
                                sel.erase(found);

                            if constexpr ( has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::remove_n:
                {
                    if constexpr ( !std::is_void_v<element_type> && !RareTs::is_static_array_v<Value_type> &&
                        !RareTs::is_optional_v<Value_type> && !std::is_same_v<std::string, Value_type> && requires{ref.erase(ref.begin());} )
                    {
                        auto & sel = get_selections<Pathway...>();
                        std::size_t removal_count = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto removal_indexes = read_indexes<index_type>(offset, removal_count);
                        if constexpr ( has_element_removed_op<route> )
                        {
                            for ( auto removal_index : removal_indexes )
                                notify_element_removed(user, route{keys}, static_cast<std::size_t>(removal_index));
                        }
                        for ( std::size_t i=0; i<removal_count; ++i )
                        {
                            auto index_removed = removal_indexes[i];
                            ref.erase(std::next(ref.begin(), static_cast<std::ptrdiff_t>(index_removed)));
                            if constexpr ( has_attached_data )
                            {
                                auto & attached_data = get_attached_data<Pathway...>();
                                attached_data.erase(std::next(attached_data.begin(), static_cast<std::ptrdiff_t>(index_removed)));
                            }
                            if constexpr ( has_selections )
                            {
                                auto found = sel.end();
                                for ( auto it = sel.begin(); it != sel.end(); ++it )
                                {
                                    if ( *it == index_removed )
                                        found = it;
                                    else if ( *it > index_removed )
                                        --(*it);
                                }
                                if ( found != sel.end() )
                                    sel.erase(found);
                            }
                        }

                        if constexpr ( has_element_moved_op<route> )
                        {
                            if ( !ref.empty() )
                            {
                                std::size_t size_removed = std::size(removal_indexes);
                                std::size_t collection_index = static_cast<std::size_t>(removal_indexes[size_removed-1]);
                                std::size_t move_distance = 1;
                                for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(size_removed)-1; i>0; --i )
                                {
                                    for ( std::size_t j=removal_indexes[static_cast<std::size_t>(i)]; j<removal_indexes[static_cast<std::size_t>(i-1)]-1; ++j )
                                    {
                                        notify_element_moved(user, route{keys}, collection_index+move_distance, collection_index);
                                        ++collection_index;
                                    }
                                    ++move_distance;
                                }
                                for ( ; collection_index < std::size(ref); ++collection_index )
                                    notify_element_moved(user, route{keys}, collection_index+move_distance, collection_index);
                            }
                        }
                        if constexpr ( has_selections && has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::remove_l:
                {
                    if constexpr ( !std::is_void_v<element_type> && !RareTs::is_static_array_v<Value_type> &&
                        !RareTs::is_optional_v<Value_type> && !std::is_same_v<std::string, Value_type> && requires{ref.erase(ref.begin());} )
                    {
                        std::size_t removal_count = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto removal_indexes = read_indexes<index_type>(offset, removal_count);
                        for ( std::size_t i=0; i<removal_count; ++i )
                        {
                            if constexpr ( has_element_removed_op<route> )
                                notify_element_removed(user, route{keys}, static_cast<std::size_t>(removal_indexes[i]));

                            ref.erase(std::next(ref.begin(), static_cast<std::ptrdiff_t>(removal_indexes[i])));
                            if constexpr ( has_attached_data )
                            {
                                auto & attached_data = get_attached_data<Pathway...>();
                                attached_data.erase(std::next(attached_data.begin(), static_cast<std::ptrdiff_t>(removal_indexes[i])));
                            }
                        }
                        if constexpr ( has_element_moved_op<route> )
                        {
                            if ( !ref.empty() )
                            {
                                std::size_t size_removed = std::size(removal_indexes);
                                std::size_t collection_index = static_cast<std::size_t>(removal_indexes[size_removed-1]);
                                std::size_t move_distance = 1;
                                for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(size_removed)-1; i>0; --i )
                                {
                                    for ( std::size_t j=removal_indexes[static_cast<std::size_t>(i)];
                                        j<removal_indexes[static_cast<std::size_t>(i-1)]-1; ++j )
                                    {
                                        notify_element_moved(user, route{keys}, collection_index+move_distance, collection_index);
                                        ++collection_index;
                                    }
                                    ++move_distance;
                                }
                                for ( ; collection_index < std::size(ref); ++collection_index )
                                    notify_element_moved(user, route{keys}, collection_index+move_distance, collection_index);
                            }
                        }

                        clear_sel(get_selections<Pathway...>());
                        if constexpr ( has_selections && has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::sort:
                {
                    if constexpr ( RareTs::is_specialization_v<Value_type, std::vector> )
                    {
                        std::size_t count = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto source_indexes = read_indexes<index_type>(offset, count);
                            
                        redo_sort(ref, source_indexes);
                        if constexpr ( has_attached_data )
                            redo_sort(get_attached_data<Pathway...>(), source_indexes);

                        if constexpr ( has_element_moved_op<route> )
                        {
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( i != source_indexes[i] )
                                    notify_element_moved(user, route{keys}, static_cast<std::size_t>(source_indexes[i]), static_cast<std::size_t>(i));
                            }
                        }

                        if constexpr ( has_selections )
                        {
                            auto & sel = get_selections<Pathway...>();
                            sel_type new_sel{};
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( std::find(sel.begin(), sel.end(), source_indexes[i]) != sel.end() )
                                    RareTs::append(new_sel, static_cast<index_type>(i));
                            }

                            std::swap(sel, new_sel);
                            if constexpr ( has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::sort_desc:
                {
                    if constexpr ( RareTs::is_specialization_v<Value_type, std::vector> )
                    {
                        std::size_t count = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto source_indexes = read_indexes<index_type>(offset, count);
                            
                        redo_sort(ref, source_indexes);
                        if constexpr ( has_attached_data )
                            redo_sort(get_attached_data<Pathway...>(), source_indexes);

                        if constexpr ( has_element_moved_op<route> )
                        {
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( i != source_indexes[i] )
                                    notify_element_moved(user, route{keys}, static_cast<std::size_t>(source_indexes[i]), static_cast<std::size_t>(i));
                            }
                        }

                        if constexpr ( has_selections )
                        {
                            auto & sel = get_selections<Pathway...>();
                            sel_type new_sel {};
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                if ( std::find(sel.begin(), sel.end(), source_indexes[i]) != sel.end() )
                                    RareTs::append(new_sel, static_cast<index_type>(i));
                            }
                            std::swap(sel, new_sel);
                            if constexpr ( has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::swap:
                {
                    if constexpr ( has_move_ops )
                    {
                        auto first_index = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto second_index = static_cast<std::size_t>(read_index<index_type>(offset));
                        if ( first_index != second_index && first_index < std::size(ref) && second_index < std::size(ref) )
                        {
                            std::swap(ref[first_index], ref[second_index]);
                            if constexpr ( has_attached_data )
                                mirror_swap_to(get_attached_data<Pathway...>(), first_index, second_index);

                            mirror_swap_to_selection(get_selections<Pathway...>(), first_index, second_index);
                            if constexpr ( has_element_moved_op<route> )
                            {
                                notify_element_moved(user, route{keys}, first_index, second_index);
                                notify_element_moved(user, route{keys}, second_index, first_index);
                            }
                            if constexpr ( has_selections && has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::move_up:
                {
                    if constexpr ( has_move_ops )
                    {
                        auto moved_index = static_cast<std::size_t>(read_index<index_type>(offset));
                        if ( moved_index > 0 && moved_index < std::size(ref) )
                        {
                            std::swap(ref[static_cast<std::size_t>(moved_index)], ref[static_cast<std::size_t>(moved_index-1)]);
                            if constexpr ( has_attached_data )
                                mirror_swap_to(get_attached_data<Pathway...>(), moved_index, moved_index-1);

                            mirror_swap_to_selection(get_selections<Pathway...>(), moved_index, moved_index-1);
                            if constexpr ( has_element_moved_op<route> )
                            {
                                notify_element_moved(user, route{keys}, moved_index, moved_index-1);
                                notify_element_moved(user, route{keys}, moved_index-1, moved_index);
                            }
                            if constexpr ( has_selections && has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::move_up_n:
                {
                    if constexpr ( has_move_ops )
                    {
                        std::size_t next_available = 0;
                        auto count = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto moved_indexes = read_indexes<index_type>(offset, count);
                        for ( std::size_t i=0; i<count; ++i )
                        {
                            auto moved_index = static_cast<std::size_t>(moved_indexes[i]);
                            if ( moved_index > next_available && static_cast<std::size_t>(moved_index) < std::size(ref) )
                            {
                                std::swap(ref[static_cast<std::size_t>(moved_index)], ref[static_cast<std::size_t>(moved_index-1)]);
                                if constexpr ( has_attached_data )
                                    mirror_swap_to(get_attached_data<Pathway...>(), moved_index, moved_index-1);

                                mirror_swap_to_selection(get_selections<Pathway...>(), moved_index, moved_index-1);
                            }
                            else if ( moved_index == next_available )
                                ++next_available;
                        }

                        if constexpr ( has_element_moved_op<route> )
                        {
                            next_available = 0;
                            std::size_t block_size = 1;
                            for ( std::size_t i=0; i<std::size(moved_indexes); ++i )
                            {
                                if ( moved_indexes[i] > next_available && static_cast<std::size_t>(moved_indexes[i]) < std::size(ref) )
                                {
                                    if ( i+1 < std::size(moved_indexes) && moved_indexes[i]+1 == moved_indexes[i+1] )
                                        ++block_size;
                                    else
                                    {
                                        notify_element_moved(user, route{keys}, moved_indexes[i]-block_size, moved_indexes[i]);
                                        block_size = 1;
                                    }
                                    notify_element_moved(user, route{keys}, moved_indexes[i], moved_indexes[i]-1);
                                }
                                else if ( moved_indexes[i] == next_available )
                                    ++next_available;
                            }
                        }
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::move_up_l:
                {
                    if constexpr ( has_selections && has_move_ops )
                    {
                        auto & sel = get_selections<Pathway...>();

                        auto moved_indexes = sel; // Copy so it can be sorted without changing selections
                        std::sort(moved_indexes.begin(), moved_indexes.end());
                
                        std::size_t next_available = 0;
                        for ( auto moved_index : moved_indexes )
                        {
                            if ( moved_index > next_available && static_cast<std::size_t>(moved_index) < std::size(ref) )
                            {
                                std::swap(ref[static_cast<std::size_t>(moved_index)], ref[static_cast<std::size_t>(moved_index)-1]);
                                if constexpr ( has_attached_data )
                                    mirror_swap_to(get_attached_data<Pathway...>(), moved_index, static_cast<std::size_t>(moved_index)-1);

                                mirror_swap_to_selection(sel, moved_index, static_cast<std::size_t>(moved_index)-1);
                                next_available = moved_index;
                            }
                            else if ( moved_index == next_available )
                                ++next_available;
                        }

                        if constexpr ( has_element_moved_op<route> )
                        {
                            next_available = 0;
                            std::size_t block_size = 1;
                            for ( std::size_t i=0; i<std::size(moved_indexes); ++i )
                            {
                                if ( moved_indexes[i] > next_available && static_cast<std::size_t>(moved_indexes[i]) < std::size(ref) )
                                {
                                    if ( i+1 < std::size(moved_indexes) && moved_indexes[i]+1 == moved_indexes[i+1] )
                                        ++block_size;
                                    else
                                    {
                                        notify_element_moved(user, route{keys}, moved_indexes[i]-block_size, moved_indexes[i]);
                                        block_size = 1;
                                    }
                                    notify_element_moved(user, route{keys}, moved_indexes[i], moved_indexes[i]-1);
                                }
                                else if ( moved_indexes[i] == next_available )
                                    ++next_available;
                            }
                        }
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::move_top:
                {
                    if constexpr ( has_move_ops )
                    {
                        auto moved_index = static_cast<std::size_t>(read_index<index_type>(offset));
                        if ( moved_index > 0 && moved_index < std::size(ref) )
                        {
                            auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(moved_index));
                            std::rotate(ref.begin(), it, it+1);
                            if constexpr ( has_attached_data )
                                mirror_rotation_to(get_attached_data<Pathway...>(), 0, moved_index, moved_index+1);

                            mirror_rotation_to_selection(get_selections<Pathway...>(), 0, moved_index, moved_index+1);
                            if constexpr ( has_element_moved_op<route> )
                            {
                                notify_element_moved(user, route{keys}, moved_index, 0);
                                for ( std::size_t i=0; i<static_cast<std::size_t>(moved_index); ++i )
                                    notify_element_moved(user, route{keys}, i, i+1);
                            }
                            if constexpr ( has_selections && has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::move_top_n:
                {
                    if constexpr ( has_move_ops )
                    {
                        auto & sel = get_selections<Pathway...>();
                        auto count = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto moved_indexes = read_indexes<index_type>(offset, count);

                        std::size_t size = std::size(ref);
                        std::vector<std::size_t> tracked_indexes {};
                        if constexpr ( has_element_moved_op<route> )
                        {
                            tracked_indexes.assign(size, 0);
                            std::iota(tracked_indexes.begin(), tracked_indexes.end(), 0);
                        }

                        std::ptrdiff_t insertion_index = 0;
                        for ( auto i : moved_indexes )
                        {
                            if ( static_cast<std::ptrdiff_t>(i) > insertion_index && static_cast<std::size_t>(i) < size )
                            {
                                auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                std::rotate(std::next(ref.begin(), insertion_index), it, it+1);
                                if constexpr ( has_attached_data )
                                    mirror_rotation_to(get_attached_data<Pathway...>(), static_cast<std::size_t>(insertion_index), static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1);

                                mirror_rotation_to_selection(sel, static_cast<std::size_t>(insertion_index), static_cast<std::size_t>(i), static_cast<std::size_t>(i)+1);
                                if constexpr ( has_element_moved_op<route> )
                                    mirror_rotation_to(tracked_indexes, insertion_index, i, i+1);
                            }
                            ++insertion_index;
                        }

                        if constexpr ( has_element_moved_op<route> )
                        {
                            for ( std::size_t i=0; i<size; ++i )
                            {
                                if ( i != tracked_indexes[i] )
                                    notify_element_moved(user, route{keys}, tracked_indexes[i], i);
                            }
                        }
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::move_top_l:
                {
                    if constexpr ( has_selections && has_move_ops )
                    {
                        auto & sel = get_selections<Pathway...>();
                        sel_type prev_sel {};
                        read_selections(events, offset, prev_sel);
                        std::sort(prev_sel.begin(), prev_sel.end());

                        std::size_t size = std::size(ref);
                        std::vector<std::size_t> tracked_indexes {};
                        if constexpr ( has_element_moved_op<route> )
                        {
                            tracked_indexes.assign(size, 0);
                            std::iota(tracked_indexes.begin(), tracked_indexes.end(), 0);
                        }

                        std::ptrdiff_t insertion_index = 0;
                        for ( auto i : prev_sel )
                        {
                            if ( static_cast<std::ptrdiff_t>(i) > insertion_index && static_cast<std::size_t>(i) < size )
                            {
                                auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                std::rotate(std::next(ref.begin(), insertion_index), it, it+1);
                                if constexpr ( has_attached_data )
                                    mirror_rotation_to(get_attached_data<Pathway...>(), static_cast<std::size_t>(insertion_index), i, static_cast<std::size_t>(i)+1);

                                mirror_rotation_to_selection(sel, static_cast<std::size_t>(insertion_index), i, static_cast<std::size_t>(i)+1);
                                if constexpr ( has_element_moved_op<route> )
                                    mirror_rotation_to(tracked_indexes, insertion_index, i, i+1);
                            }
                            ++insertion_index;
                        }

                        if constexpr ( has_element_moved_op<route> )
                        {
                            for ( std::size_t i=0; i<size; ++i )
                            {
                                if ( i != tracked_indexes[i] )
                                    notify_element_moved(user, route{keys}, tracked_indexes[i], i);
                            }
                        }
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::move_down:
                {
                    if constexpr ( has_move_ops )
                    {
                        auto moved_index = static_cast<std::size_t>(read_index<index_type>(offset));
                        if ( moved_index+1 < std::size(ref) )
                        {
                            std::swap(ref[static_cast<std::size_t>(moved_index)], ref[static_cast<std::size_t>(moved_index+1)]);
                            if constexpr ( has_attached_data )
                                mirror_swap_to(get_attached_data<Pathway...>(), moved_index, moved_index+1);

                            mirror_swap_to_selection(get_selections<Pathway...>(), moved_index, moved_index+1);
                            if constexpr ( has_element_moved_op<route> )
                            {
                                notify_element_moved(user, route{keys}, moved_index, moved_index+1);
                                notify_element_moved(user, route{keys}, moved_index+1, moved_index);
                            }
                            if constexpr ( has_selections && has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::move_down_n:
                {
                    if constexpr ( has_move_ops )
                    {
                        auto size = std::size(ref);
                        if ( size > 0 )
                        {
                            auto limit = size;
                            auto count = static_cast<std::size_t>(read_index<index_type>(offset));
                            auto moved_indexes = read_indexes<index_type>(offset, count);
                            for ( std::size_t i=0; i<count; ++i )
                            {
                                auto moved_index = moved_indexes[i];
                                if ( static_cast<std::size_t>(moved_index)+1 < limit )
                                {
                                    std::swap(ref[static_cast<std::size_t>(moved_index)], ref[static_cast<std::size_t>(moved_index)+1]);
                                    if constexpr ( has_attached_data )
                                        mirror_swap_to(get_attached_data<Pathway...>(), moved_index, static_cast<std::size_t>(moved_index)+1);

                                    mirror_swap_to_selection(get_selections<Pathway...>(), moved_index, static_cast<std::size_t>(moved_index)+1);
                                    limit = static_cast<std::size_t>(moved_index)+1;
                                }
                                else if ( limit > 0 )
                                    --limit;
                            }

                            if constexpr ( has_element_moved_op<route> )
                            {
                                limit = size;
                                std::size_t block_size = 1;
                                for ( std::size_t i=0; i<count; ++i )
                                {
                                    if ( static_cast<std::size_t>(moved_indexes[i])+1 < limit )
                                    {
                                        if ( i+1 < count && moved_indexes[i]-1 == moved_indexes[i+1] )
                                            ++block_size;
                                        else
                                        {
                                            notify_element_moved(user, route{keys}, moved_indexes[i]+block_size, moved_indexes[i]);
                                            block_size = 1;
                                        }
                                        notify_element_moved(user, route{keys}, moved_indexes[i], moved_indexes[i]+1);
                                        limit = moved_indexes[i]+1;
                                    }
                                    else if ( limit > 0 )
                                        --limit;
                                }
                            }
                            if constexpr ( has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::move_down_l:
                {
                    if constexpr ( has_selections && has_move_ops )
                    {
                        auto & sel = get_selections<Pathway...>();
                        sel_type prev_sel {};
                        read_selections(events, offset, prev_sel);

                        auto moved_indexes = prev_sel; // Copy so it can be sorted without changing selections
                        std::sort(moved_indexes.begin(), moved_indexes.end(), std::greater<index_type>{});
                        std::size_t count = std::size(moved_indexes);

                        auto size = std::size(ref);
                        if ( size > 0 )
                        {
                            auto limit = size;
                            for ( auto moved_index : moved_indexes )
                            {
                                if ( static_cast<std::size_t>(moved_index)+1 < limit )
                                {
                                    std::swap(ref[static_cast<std::size_t>(moved_index)], ref[static_cast<std::size_t>(moved_index)+1]);
                                    if constexpr ( has_attached_data )
                                        mirror_swap_to(get_attached_data<Pathway...>(), moved_index, static_cast<std::size_t>(moved_index)+1);

                                    mirror_swap_to_selection(sel, moved_index, static_cast<std::size_t>(moved_index)+1);
                                    limit = static_cast<std::size_t>(moved_index)+1;
                                }
                                else if ( limit > 0 )
                                    --limit;
                            }

                            if constexpr ( has_element_moved_op<route> )
                            {
                                limit = size;
                                std::size_t block_size = 1;
                                for ( std::size_t i=0; i<count; ++i )
                                {
                                    if ( static_cast<std::size_t>(moved_indexes[i])+1 < limit )
                                    {
                                        if ( i+1 < count && moved_indexes[i]-1 == moved_indexes[i+1] )
                                            ++block_size;
                                        else
                                        {
                                            notify_element_moved(user, route{keys}, moved_indexes[i]+block_size, moved_indexes[i]);
                                            block_size = 1;
                                        }
                                        notify_element_moved(user, route{keys}, moved_indexes[i], moved_indexes[i]+1);
                                        limit = moved_indexes[i]+1;
                                    }
                                    else if ( limit > 0 )
                                        --limit;
                                }
                            }
                            if constexpr ( has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::move_bottom:
                {
                    if constexpr ( has_move_ops )
                    {
                        auto moved_index = static_cast<std::size_t>(read_index<index_type>(offset));
                        if ( static_cast<std::size_t>(moved_index)+1 < std::size(ref) )
                        {
                            auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(moved_index));
                            std::rotate(it, it+1, ref.end());
                            if constexpr ( has_attached_data )
                                mirror_rotation_to(get_attached_data<Pathway...>(), moved_index, moved_index+1, std::size(ref));

                            mirror_rotation_to_selection(get_selections<Pathway...>(), moved_index, moved_index+1, std::size(ref));
                            if constexpr ( has_element_moved_op<route> )
                            {
                                notify_element_moved(user, route{keys}, moved_index, std::size(ref)-1);
                                for ( std::size_t i=moved_index+1; i<std::size(ref); ++i )
                                    notify_element_moved(user, route{keys}, i, i-1);
                            }
                            if constexpr ( has_selections && has_sel_change_op )
                                notify_selections_changed(user,route{keys});
                        }
                    }
                }
                break;
                case op::move_bottom_n:
                {
                    if constexpr ( has_move_ops )
                    {
                        auto & sel = get_selections<Pathway...>();
                        auto count = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto moved_indexes = read_indexes<index_type>(offset, count);

                        std::size_t size = std::size(ref);
                        std::vector<std::size_t> tracked_indexes {};
                        if constexpr ( has_element_moved_op<route> )
                        {
                            tracked_indexes.assign(size, 0);
                            std::iota(tracked_indexes.begin(), tracked_indexes.end(), 0);
                        }

                        std::ptrdiff_t insertion_index = static_cast<std::ptrdiff_t>(size);
                        for ( auto i : moved_indexes )
                        {
                            if ( i >= 0 && static_cast<std::ptrdiff_t>(i)+1 < insertion_index )
                            {
                                auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                std::rotate(it, it+1, std::next(ref.begin(), insertion_index));
                                if constexpr ( has_attached_data )
                                    mirror_rotation_to(get_attached_data<Pathway...>(), i, static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertion_index));

                                mirror_rotation_to_selection(sel, i, static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertion_index));
                                if constexpr ( has_element_moved_op<route> )
                                    mirror_rotation_to(tracked_indexes, i, i+1, insertion_index);
                            }
                            --insertion_index;
                        }

                        if constexpr ( has_element_moved_op<route> )
                        {
                            for ( std::size_t i=0; i<size; ++i )
                            {
                                if ( i != tracked_indexes[i] )
                                    notify_element_moved(user, route{keys}, tracked_indexes[i], i);
                            }
                        }
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::move_bottom_l:
                {
                    if constexpr ( has_selections && has_move_ops )
                    {
                        auto & sel = get_selections<Pathway...>();
                        sel_type prev_sel {};
                        read_selections(events, offset, prev_sel);
                        auto moved_indexes = prev_sel;
                        std::sort(moved_indexes.begin(), moved_indexes.end(), std::greater<index_type>{});
                            
                        std::size_t size = std::size(ref);
                        std::vector<std::size_t> tracked_indexes {};
                        if constexpr ( has_element_moved_op<route> )
                        {
                            tracked_indexes.assign(size, 0);
                            std::iota(tracked_indexes.begin(), tracked_indexes.end(), 0);
                        }

                        std::ptrdiff_t insertion_index = static_cast<std::ptrdiff_t>(size);
                        for ( auto i : moved_indexes )
                        {
                            if ( i >= 0 && static_cast<std::ptrdiff_t>(i)+1 < insertion_index )
                            {
                                auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                std::rotate(it, it+1, std::next(ref.begin(), insertion_index));
                                if constexpr ( has_attached_data )
                                    mirror_rotation_to(get_attached_data<Pathway...>(), i, static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertion_index));

                                mirror_rotation_to_selection(sel, i, static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertion_index));
                                if constexpr ( has_element_moved_op<route> )
                                    mirror_rotation_to(tracked_indexes, i, i+1, insertion_index);
                            }
                            --insertion_index;
                        }

                        if constexpr ( has_element_moved_op<route> )
                        {
                            for ( std::size_t i=0; i<size; ++i )
                            {
                                if ( i != tracked_indexes[i] )
                                    notify_element_moved(user, route{keys}, tracked_indexes[i], i);
                            }
                        }
                        if constexpr ( has_sel_change_op )
                            notify_selections_changed(user, route{keys});
                    }
                }
                break;
                case op::move_to:
                {
                    if constexpr ( has_move_ops )
                    {
                        auto index_moved_to = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto moved_index = static_cast<std::size_t>(read_index<index_type>(offset));
                            
                        auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(moved_index));
                        auto target = std::next(ref.begin(), static_cast<std::ptrdiff_t>(index_moved_to));
                        if ( index_moved_to < moved_index && moved_index < std::size(ref) )
                        {
                            std::rotate(target, it, it+1);
                            if constexpr ( has_attached_data )
                                mirror_rotation_to(get_attached_data<Pathway...>(), index_moved_to, moved_index, moved_index+1);

                            mirror_rotation_to_selection(get_selections<Pathway...>(), index_moved_to, moved_index, moved_index+1);
                            if constexpr ( has_element_moved_op<route> )
                            {
                                notify_element_moved(user, route{keys}, static_cast<std::size_t>(moved_index), static_cast<std::size_t>(index_moved_to));
                                for ( std::size_t i=static_cast<std::size_t>(index_moved_to); i<static_cast<std::size_t>(moved_index); ++i )
                                    notify_element_moved(user, route{keys}, i, i+1);
                            }
                            if constexpr ( has_selections && has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                        else if ( index_moved_to > moved_index && index_moved_to < std::size(ref) )
                        {
                            std::rotate(it, it+1, target+1);
                            if constexpr ( has_attached_data )
                                mirror_rotation_to(get_attached_data<Pathway...>(), moved_index, moved_index+1, index_moved_to+1);

                            mirror_rotation_to_selection(get_selections<Pathway...>(), moved_index, moved_index+1, index_moved_to+1);
                            if constexpr ( has_element_moved_op<route> )
                            {
                                notify_element_moved(user, route{keys}, static_cast<std::size_t>(moved_index), static_cast<std::size_t>(index_moved_to));
                                for ( std::size_t i=static_cast<std::size_t>(moved_index)+1; i<static_cast<std::size_t>(index_moved_to)+1; ++i )
                                    notify_element_moved(user, route{keys}, i, i-1);
                            }
                            if constexpr ( has_selections && has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::move_to_n:
                {
                    if constexpr ( has_move_ops )
                    {
                        auto & sel = get_selections<Pathway...>();
                        auto index_moved_to = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto count_moved_indexes = static_cast<std::size_t>(read_index<index_type>(offset));
                        auto moved_indexes = read_indexes<index_type>(offset, count_moved_indexes);

                        auto size = std::size(ref);
                        if ( !std::empty(moved_indexes) )
                        {
                            std::size_t count_valid_indexes = 0;
                            for ( auto moved_index : moved_indexes )
                            {
                                if ( moved_index >= 0 && static_cast<std::size_t>(moved_index) < std::size(ref) )
                                    ++count_valid_indexes;
                            }
                            if ( count_valid_indexes == 0 )
                                return;

                            std::vector<std::size_t> tracked_indexes {};
                            if constexpr ( has_element_moved_op<route> )
                            {
                                tracked_indexes.assign(size, 0);
                                std::iota(tracked_indexes.begin(), tracked_indexes.end(), 0);
                            }

                            if ( static_cast<std::size_t>(index_moved_to) + count_valid_indexes >= size ) // Operation collapses to move_to_end
                            {
                                std::ptrdiff_t insertion_index = static_cast<std::ptrdiff_t>(size);
                                for ( auto mit = moved_indexes.rbegin(); mit != moved_indexes.rend(); ++mit ) // moved_indexes sorted least to greatest, so reverse read
                                {
                                    auto i = *mit;
                                    if ( i >= 0 && static_cast<std::ptrdiff_t>(i)+1 < insertion_index )
                                    {
                                        auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                        std::rotate(it, it+1, std::next(ref.begin(), insertion_index));
                                        if constexpr ( has_attached_data )
                                            mirror_rotation_to(get_attached_data<Pathway...>(), i, static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertion_index));

                                        mirror_rotation_to_selection(sel, i, static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertion_index));
                                        if constexpr ( has_element_moved_op<route> )
                                            mirror_rotation_to(tracked_indexes, i, i+1, static_cast<std::size_t>(insertion_index));
                                    }
                                    --insertion_index;
                                }
                            }
                            else // Non-collapsed move_to operation
                            {
                                std::size_t min_valid_index = 0;
                                std::size_t max_valid_index = 0;
                                for ( std::size_t i=0; i<std::size(moved_indexes); ++i )
                                {
                                    if ( moved_indexes[i] >= 0 )
                                    {
                                        min_valid_index = static_cast<std::size_t>(moved_indexes[i]);
                                        break;
                                    }
                                }
                                for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(std::size(moved_indexes))-1; i>=0; --i )
                                {
                                    if ( static_cast<std::ptrdiff_t>(moved_indexes[static_cast<std::size_t>(i)]) < static_cast<std::ptrdiff_t>(size) )
                                    {
                                        max_valid_index = static_cast<std::size_t>(moved_indexes[static_cast<std::size_t>(i)]);
                                        break;
                                    }
                                }

                                std::size_t left_chunk_size = min_valid_index < static_cast<std::size_t>(index_moved_to) ? 1 : 0;
                                std::size_t right_chunk_size = max_valid_index >= static_cast<std::size_t>(index_moved_to) ? 1 : 0;
                                std::size_t left_chunk_first = min_valid_index;
                                std::size_t right_chunk_first = max_valid_index;

                                for ( std::size_t i=0; i+1<std::size(moved_indexes) && moved_indexes[i+1] < index_moved_to; ++i )
                                {
                                    // rotate left chunk together with element[moved_indexes[i+1]]
                                    auto left_start = std::next(ref.begin(), static_cast<std::ptrdiff_t>(left_chunk_first));
                                    auto left_end = std::next(left_start, static_cast<std::ptrdiff_t>(left_chunk_size));
                                    std::rotate(left_start, left_end, std::next(ref.begin(), static_cast<std::ptrdiff_t>(moved_indexes[i+1])));
                                    if constexpr ( has_attached_data )
                                        mirror_rotation_to(get_attached_data<Pathway...>(), left_chunk_first, left_chunk_first+left_chunk_size, moved_indexes[i+1]);

                                    mirror_rotation_to_selection(sel, left_chunk_first, left_chunk_first+left_chunk_size, moved_indexes[i+1]);
                                    if constexpr ( has_element_moved_op<route> )
                                        mirror_rotation_to(tracked_indexes, left_chunk_first, left_chunk_first+left_chunk_size, moved_indexes[i+1]);
                                    left_chunk_first = static_cast<std::size_t>(moved_indexes[i+1])-left_chunk_size;
                                    ++left_chunk_size;
                                }
                                for ( std::size_t i=std::size(moved_indexes)-1; i>0 && moved_indexes[i-1] >= index_moved_to; --i )
                                {
                                    // rotate right chunk together with element[moved_indexes[i-1]]
                                    auto right_start = std::next(ref.begin(), static_cast<std::ptrdiff_t>(right_chunk_first));
                                    auto right_end = std::next(right_start, static_cast<std::ptrdiff_t>(right_chunk_size));
                                    std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(moved_indexes[i-1])+1), right_start, right_end);
                                    if constexpr ( has_attached_data )
                                        mirror_rotation_to(get_attached_data<Pathway...>(), static_cast<std::size_t>(moved_indexes[i-1])+1, right_chunk_first, right_chunk_first+right_chunk_size);

                                    mirror_rotation_to_selection(sel, static_cast<std::size_t>(moved_indexes[i-1])+1, right_chunk_first, right_chunk_first+right_chunk_size);
                                    if constexpr ( has_element_moved_op<route> )
                                        mirror_rotation_to(tracked_indexes, moved_indexes[i-1]+1, right_chunk_first, right_chunk_first+right_chunk_size);
                                    right_chunk_first = static_cast<std::size_t>(moved_indexes[i-1]);
                                    ++right_chunk_size;
                                }
                                if ( left_chunk_first+left_chunk_size != right_chunk_first && left_chunk_size > 0 && right_chunk_size > 0 ) // Need to combine the chunks then rotate combined to final position
                                {
                                    std::size_t left_distance = left_chunk_first < static_cast<std::size_t>(index_moved_to) ? static_cast<std::size_t>(index_moved_to)-left_chunk_first : left_chunk_first-static_cast<std::size_t>(index_moved_to);
                                    std::size_t right_distance = right_chunk_first < static_cast<std::size_t>(index_moved_to)+left_chunk_size ? static_cast<std::size_t>(index_moved_to)+left_chunk_size-right_chunk_first : right_chunk_first-static_cast<std::size_t>(index_moved_to)-left_chunk_size;
                                    if ( right_distance < left_distance ) // Right chunk closer to target, move left up to right
                                    {
                                        auto left_start = std::next(ref.begin(), static_cast<std::ptrdiff_t>(left_chunk_first));
                                        auto left_end = std::next(left_start, static_cast<std::ptrdiff_t>(left_chunk_size));
                                        std::rotate(left_start, left_end, std::next(ref.begin(), static_cast<std::ptrdiff_t>(right_chunk_first)));
                                        if constexpr ( has_attached_data )
                                            mirror_rotation_to(get_attached_data<Pathway...>(), left_chunk_first, left_chunk_first+left_chunk_size, right_chunk_first);

                                        mirror_rotation_to_selection(sel, left_chunk_first, left_chunk_first+left_chunk_size, right_chunk_first);
                                        if constexpr ( has_element_moved_op<route> )
                                            mirror_rotation_to(tracked_indexes, left_chunk_first, left_chunk_first+left_chunk_size, right_chunk_first);
                                        left_chunk_first = right_chunk_first-left_chunk_size;
                                    }
                                    else // Left chunk closer to target, move right up to left
                                    {
                                        auto right_start = std::next(ref.begin(), static_cast<std::ptrdiff_t>(right_chunk_first));
                                        auto right_end = std::next(right_start, static_cast<std::ptrdiff_t>(right_chunk_size));
                                        std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(left_chunk_first+left_chunk_size)), right_start, right_end);
                                        if constexpr ( has_attached_data )
                                            mirror_rotation_to(get_attached_data<Pathway...>(), left_chunk_first+left_chunk_size, right_chunk_first, right_chunk_first+right_chunk_size);

                                        mirror_rotation_to_selection(sel, left_chunk_first+left_chunk_size, right_chunk_first, right_chunk_first+right_chunk_size);
                                        if constexpr ( has_element_moved_op<route> )
                                            mirror_rotation_to(tracked_indexes, left_chunk_first+left_chunk_size, right_chunk_first, right_chunk_first+right_chunk_size);
                                        right_chunk_first = left_chunk_first+left_chunk_size;
                                    }
                                }
                                if ( left_chunk_first < static_cast<std::size_t>(index_moved_to) ) // Rotate combined chunk rightwards to final position
                                {
                                    auto chunk_start = std::next(ref.begin(), static_cast<std::ptrdiff_t>(left_chunk_first));
                                    auto chunk_end = std::next(chunk_start, static_cast<std::ptrdiff_t>(count_valid_indexes));
                                    std::rotate(chunk_start, chunk_end, std::next(chunk_end, static_cast<std::ptrdiff_t>(static_cast<std::size_t>(index_moved_to)-left_chunk_first)));
                                    if constexpr ( has_attached_data )
                                        mirror_rotation_to(get_attached_data<Pathway...>(), left_chunk_first, left_chunk_first+count_valid_indexes, left_chunk_first+count_valid_indexes+static_cast<std::size_t>(index_moved_to)-left_chunk_first);

                                    mirror_rotation_to_selection(sel, left_chunk_first, left_chunk_first+count_valid_indexes, left_chunk_first+count_valid_indexes+static_cast<std::size_t>(index_moved_to)-left_chunk_first);
                                    if constexpr ( has_element_moved_op<route> )
                                        mirror_rotation_to(tracked_indexes, left_chunk_first, left_chunk_first+count_valid_indexes, left_chunk_first+count_valid_indexes+static_cast<std::size_t>(index_moved_to)-left_chunk_first);
                                }
                                else if ( left_chunk_first > static_cast<std::size_t>(index_moved_to) ) // Rotate combined chunk leftwards to final position
                                {
                                    auto chunk_start = std::next(ref.begin(), static_cast<std::ptrdiff_t>(left_chunk_first));
                                    auto chunk_end = std::next(chunk_start, static_cast<std::ptrdiff_t>(count_valid_indexes));
                                    std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(index_moved_to)), chunk_start, chunk_end);
                                    if constexpr ( has_attached_data )
                                        mirror_rotation_to(get_attached_data<Pathway...>(), index_moved_to, left_chunk_first, left_chunk_first+count_valid_indexes);

                                    mirror_rotation_to_selection(sel, index_moved_to, left_chunk_first, left_chunk_first+count_valid_indexes);
                                    if constexpr ( has_element_moved_op<route> )
                                        mirror_rotation_to(tracked_indexes, index_moved_to, left_chunk_first, left_chunk_first+count_valid_indexes);
                                }
                            }
                    
                            if constexpr ( has_element_moved_op<route> )
                            {
                                for ( std::size_t i=0; i<size; ++i )
                                {
                                    if ( i != tracked_indexes[i] )
                                        notify_element_moved(user, route{keys}, tracked_indexes[i], i);
                                }
                            }
                            if constexpr ( has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
                case op::move_to_l:
                {
                    if constexpr ( has_selections && has_move_ops )
                    {
                        auto index_moved_to = static_cast<std::size_t>(read_index<index_type>(offset));
                            
                        auto & sel = get_selections<Pathway...>();
                        sel_type prev_sel {};
                        read_selections(events, offset, prev_sel);
                        auto moved_indexes = prev_sel; // Copy so it can be sorted without changing selections
                        std::sort(moved_indexes.begin(), moved_indexes.end());

                        auto size = std::size(ref);
                        if ( !std::empty(moved_indexes) )
                        {
                            std::size_t count_valid_indexes = 0;
                            for ( auto moved_index : moved_indexes )
                            {
                                if ( moved_index >= 0 && static_cast<std::size_t>(moved_index) < std::size(ref) )
                                    ++count_valid_indexes;
                            }
                            if ( count_valid_indexes == 0 )
                                return;

                            std::vector<std::size_t> tracked_indexes {};
                            if constexpr ( has_element_moved_op<route> )
                            {
                                tracked_indexes.assign(size, 0);
                                std::iota(tracked_indexes.begin(), tracked_indexes.end(), 0);
                            }

                            if ( static_cast<std::size_t>(index_moved_to) + count_valid_indexes >= size ) // Operation collapses to move_to_end
                            {
                                std::ptrdiff_t insertion_index = static_cast<std::ptrdiff_t>(size);
                                for ( auto mit = moved_indexes.rbegin(); mit != moved_indexes.rend(); ++mit ) // moved_indexes sorted least to greatest, so reverse read
                                {
                                    auto i = *mit;
                                    if ( i >= 0 && static_cast<std::ptrdiff_t>(i)+1 < insertion_index )
                                    {
                                        auto it = std::next(ref.begin(), static_cast<std::ptrdiff_t>(i));
                                        std::rotate(it, it+1, std::next(ref.begin(), insertion_index));
                                        if constexpr ( has_attached_data )
                                            mirror_rotation_to(get_attached_data<Pathway...>(), i, static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertion_index));

                                        mirror_rotation_to_selection(sel, i, static_cast<std::size_t>(i)+1, static_cast<std::size_t>(insertion_index));

                                        if constexpr ( has_element_moved_op<route> )
                                            mirror_rotation_to(tracked_indexes, i, i+1, static_cast<std::size_t>(insertion_index));
                                    }
                                    --insertion_index;
                                }
                            }
                            else // Non-collapsed move_to operation
                            {
                                std::size_t min_valid_index = 0;
                                std::size_t max_valid_index = 0;
                                for ( std::size_t i=0; i<std::size(moved_indexes); ++i )
                                {
                                    if ( moved_indexes[i] >= 0 )
                                    {
                                        min_valid_index = static_cast<std::size_t>(moved_indexes[i]);
                                        break;
                                    }
                                }
                                for ( std::ptrdiff_t i=static_cast<std::ptrdiff_t>(std::size(moved_indexes))-1; i>=0; --i )
                                {
                                    if ( static_cast<std::ptrdiff_t>(moved_indexes[static_cast<std::size_t>(i)]) < static_cast<std::ptrdiff_t>(size) )
                                    {
                                        max_valid_index = static_cast<std::size_t>(moved_indexes[static_cast<std::size_t>(i)]);
                                        break;
                                    }
                                }

                                std::size_t left_chunk_size = min_valid_index < static_cast<std::size_t>(index_moved_to) ? 1 : 0;
                                std::size_t right_chunk_size = max_valid_index >= static_cast<std::size_t>(index_moved_to) ? 1 : 0;
                                std::size_t left_chunk_first = min_valid_index;
                                std::size_t right_chunk_first = max_valid_index;

                                for ( std::size_t i=0; i+1<std::size(moved_indexes) && moved_indexes[i+1] < index_moved_to; ++i )
                                {
                                    // rotate left chunk together with element[moved_indexes[i+1]]
                                    auto left_start = std::next(ref.begin(), static_cast<std::ptrdiff_t>(left_chunk_first));
                                    auto left_end = std::next(left_start, static_cast<std::ptrdiff_t>(left_chunk_size));
                                    std::rotate(left_start, left_end, std::next(ref.begin(), static_cast<std::ptrdiff_t>(moved_indexes[i+1])));
                                    if constexpr ( has_attached_data )
                                        mirror_rotation_to(get_attached_data<Pathway...>(), left_chunk_first, left_chunk_first+left_chunk_size, moved_indexes[i+1]);

                                    mirror_rotation_to_selection(sel, left_chunk_first, left_chunk_first+left_chunk_size, moved_indexes[i+1]);
                                    if constexpr ( has_element_moved_op<route> )
                                        mirror_rotation_to(tracked_indexes, left_chunk_first, left_chunk_first+left_chunk_size, moved_indexes[i+1]);
                                    left_chunk_first = static_cast<std::size_t>(moved_indexes[i+1])-left_chunk_size;
                                    ++left_chunk_size;
                                }
                                for ( std::size_t i=std::size(moved_indexes)-1; i>0 && moved_indexes[i-1] >= index_moved_to; --i )
                                {
                                    // rotate right chunk together with element[moved_indexes[i-1]]
                                    auto right_start = std::next(ref.begin(), static_cast<std::ptrdiff_t>(right_chunk_first));
                                    auto right_end = std::next(right_start, static_cast<std::ptrdiff_t>(right_chunk_size));
                                    std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(moved_indexes[i-1])+1), right_start, right_end);
                                    if constexpr ( has_attached_data )
                                        mirror_rotation_to(get_attached_data<Pathway...>(), static_cast<std::size_t>(moved_indexes[i-1])+1, right_chunk_first, right_chunk_first+right_chunk_size);

                                    mirror_rotation_to_selection(sel, static_cast<std::size_t>(moved_indexes[i-1])+1, right_chunk_first, right_chunk_first+right_chunk_size);
                                    if constexpr ( has_element_moved_op<route> )
                                        mirror_rotation_to(tracked_indexes, moved_indexes[i-1]+1, right_chunk_first, right_chunk_first+right_chunk_size);
                                    right_chunk_first = static_cast<std::size_t>(moved_indexes[i-1]);
                                    ++right_chunk_size;
                                }
                                if ( left_chunk_first+left_chunk_size != right_chunk_first && left_chunk_size > 0 && right_chunk_size > 0 ) // Need to combine the chunks then rotate combined to final position
                                {
                                    std::size_t left_distance = left_chunk_first < static_cast<std::size_t>(index_moved_to) ? static_cast<std::size_t>(index_moved_to)-left_chunk_first : left_chunk_first-static_cast<std::size_t>(index_moved_to);
                                    std::size_t right_distance = right_chunk_first < static_cast<std::size_t>(index_moved_to)+left_chunk_size ? static_cast<std::size_t>(index_moved_to)+left_chunk_size-right_chunk_first : right_chunk_first-static_cast<std::size_t>(index_moved_to)-left_chunk_size;
                                    if ( right_distance < left_distance ) // Right chunk closer to target, move left up to right
                                    {
                                        auto left_start = std::next(ref.begin(), static_cast<std::ptrdiff_t>(left_chunk_first));
                                        auto left_end = std::next(left_start, static_cast<std::ptrdiff_t>(left_chunk_size));
                                        std::rotate(left_start, left_end, std::next(ref.begin(), static_cast<std::ptrdiff_t>(right_chunk_first)));
                                        if constexpr ( has_attached_data )
                                            mirror_rotation_to(get_attached_data<Pathway...>(), left_chunk_first, left_chunk_first+left_chunk_size, right_chunk_first);

                                        mirror_rotation_to_selection(sel, left_chunk_first, left_chunk_first+left_chunk_size, right_chunk_first);
                                        if constexpr ( has_element_moved_op<route> )
                                            mirror_rotation_to(tracked_indexes, left_chunk_first, left_chunk_first+left_chunk_size, right_chunk_first);
                                        left_chunk_first = right_chunk_first-left_chunk_size;
                                    }
                                    else // Left chunk closer to target, move right up to left
                                    {
                                        auto right_start = std::next(ref.begin(), static_cast<std::ptrdiff_t>(right_chunk_first));
                                        auto right_end = std::next(right_start, static_cast<std::ptrdiff_t>(right_chunk_size));
                                        std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(left_chunk_first+left_chunk_size)), right_start, right_end);
                                        if constexpr ( has_attached_data )
                                            mirror_rotation_to(get_attached_data<Pathway...>(), left_chunk_first+left_chunk_size, right_chunk_first, right_chunk_first+right_chunk_size);

                                        mirror_rotation_to_selection(sel, left_chunk_first+left_chunk_size, right_chunk_first, right_chunk_first+right_chunk_size);
                                        if constexpr ( has_element_moved_op<route> )
                                            mirror_rotation_to(tracked_indexes, left_chunk_first+left_chunk_size, right_chunk_first, right_chunk_first+right_chunk_size);
                                        right_chunk_first = left_chunk_first+left_chunk_size;
                                    }
                                }
                                if ( left_chunk_first < static_cast<std::size_t>(index_moved_to) ) // Rotate combined chunk rightwards to final position
                                {
                                    auto chunk_start = std::next(ref.begin(), static_cast<std::ptrdiff_t>(left_chunk_first));
                                    auto chunk_end = std::next(chunk_start, static_cast<std::ptrdiff_t>(count_valid_indexes));
                                    std::rotate(chunk_start, chunk_end, std::next(chunk_end, static_cast<std::ptrdiff_t>(static_cast<std::size_t>(index_moved_to)-left_chunk_first)));
                                    if constexpr ( has_attached_data )
                                        mirror_rotation_to(get_attached_data<Pathway...>(), left_chunk_first, left_chunk_first+count_valid_indexes, left_chunk_first+count_valid_indexes+static_cast<std::size_t>(index_moved_to)-left_chunk_first);

                                    mirror_rotation_to_selection(sel, left_chunk_first, left_chunk_first+count_valid_indexes, left_chunk_first+count_valid_indexes+static_cast<std::size_t>(index_moved_to)-left_chunk_first);
                                    if constexpr ( has_element_moved_op<route> )
                                        mirror_rotation_to(tracked_indexes, left_chunk_first, left_chunk_first+count_valid_indexes, left_chunk_first+count_valid_indexes+static_cast<std::size_t>(index_moved_to)-left_chunk_first);
                                }
                                else if ( left_chunk_first > static_cast<std::size_t>(index_moved_to) ) // Rotate combined chunk leftwards to final position
                                {
                                    auto chunk_start = std::next(ref.begin(), static_cast<std::ptrdiff_t>(left_chunk_first));
                                    auto chunk_end = std::next(chunk_start, static_cast<std::ptrdiff_t>(count_valid_indexes));
                                    std::rotate(std::next(ref.begin(), static_cast<std::ptrdiff_t>(index_moved_to)), chunk_start, chunk_end);
                                    if constexpr ( has_attached_data )
                                        mirror_rotation_to(get_attached_data<Pathway...>(), index_moved_to, left_chunk_first, left_chunk_first+count_valid_indexes);

                                    mirror_rotation_to_selection(sel, index_moved_to, left_chunk_first, left_chunk_first+count_valid_indexes);
                                    if constexpr ( has_element_moved_op<route> )
                                        mirror_rotation_to(tracked_indexes, index_moved_to, left_chunk_first, left_chunk_first+count_valid_indexes);
                                }
                            }
                    
                            if constexpr ( has_element_moved_op<route> )
                            {
                                for ( std::size_t i=0; i<size; ++i )
                                {
                                    if ( i != tracked_indexes[i] )
                                        notify_element_moved(user, route{keys}, tracked_indexes[i], i);
                                }
                            }
                            if constexpr ( has_sel_change_op )
                                notify_selections_changed(user, route{keys});
                        }
                    }
                }
                break;
            }
        }

        template <class U, bool Undo, bool After_sel = false, class Member = void, std::size_t ... Member_indexes, class Keys = std::tuple<>, class ... Pathway>
        void process_event_ref(U & t, std::size_t & offset, std::optional<std::size_t> & secondary_offset, std::uint8_t operation,
            std::index_sequence<Member_indexes...>, Keys keys = std::tuple{}, type_tags<Pathway...> = {})
        {
            constexpr bool has_sel = has_selections<Pathway...>();
            using index_type = index_type_t<default_index_type, Member>;
            std::uint8_t value = events[offset++];

            switch ( path_op(value & std::uint8_t(path_op::high_bits)) )
            {
                case path_op::sel_branch:
                    if constexpr ( has_sel && requires{t[0];} )
                    {
                        std::size_t sel_branch_start_offset = offset;
                        auto & sel_data = get_selections<Pathway...>();
                        for ( auto & sel : sel_data )
                        {
                            using elem = std::remove_cvref_t<decltype(t[0])>;
                            process_event_ref<elem, Undo, true, Member>( // sel is prepended since sels are always before first path_index
                                t[static_cast<std::size_t>(sel)], offset, secondary_offset, operation, std::make_index_sequence<reflected_member_count<elem>()>(),
                                std::tuple_cat(std::tuple<index_type>{sel}, keys), type_tags<Pathway..., path_index<0>>{});

                            offset = sel_branch_start_offset;
                        }
                    }
                    break;
                case path_op::leaf_sel_branch:
                    if constexpr ( sizeof...(Pathway) == 0 && Undo )
                        process_undo_event<std::remove_cvref_t<U>, void>(operation, offset, secondary_offset, t, keys);
                    else if constexpr ( sizeof...(Pathway) == 0 && !Undo )
                        process_redo_event<std::remove_cvref_t<U>, void>(operation, offset, secondary_offset, t, keys);
                    else if constexpr ( has_sel && requires{t[0];} )
                    {
                        using element_type = std::remove_cvref_t<RareTs::element_type_t<std::remove_cvref_t<U>>>;
                        
                        std::size_t sel_branch_start_offset = offset;
                        auto & sel_data = get_selections<Pathway...>();
                        for ( auto & sel : sel_data )
                        {
                            if constexpr ( Undo )
                            {
                                process_undo_event<element_type, Member, Pathway..., path_index<0>>(
                                    operation, offset, secondary_offset, t[static_cast<std::size_t>(sel)], std::tuple_cat(std::tuple<index_type>{sel}, keys));
                            }
                            else
                            {
                                process_redo_event<element_type, Member, Pathway..., path_index<0>>(
                                    operation, offset, secondary_offset, t[static_cast<std::size_t>(sel)], std::tuple_cat(std::tuple<index_type>{sel}, keys));
                            }

                            offset = sel_branch_start_offset;
                        }
                    }
                    break;
                case path_op::branch:
                {
                    if constexpr ( RareTs::is_static_array_v<U> || RareTs::is_specialization_v<U, std::vector> ) // Branch to index
                    {
                        index_type index {};
                        if constexpr ( std::is_same_v<index_type, uint6_t> )
                            index = static_cast<uint6_t>(value & std::uint8_t(path_op::low_bits));
                        else
                            index = read_index<index_type>(offset);
                        
                        using elem = std::remove_cvref_t<decltype(std::declval<U>()[0])>;
                        process_event_ref<elem, Undo, After_sel, Member>(
                            t[static_cast<std::size_t>(index)], offset, secondary_offset, operation, std::make_index_sequence<reflected_member_count<elem>()>(),
                            std::tuple_cat(keys, std::tuple{index}), type_tags<Pathway..., path_index<std::tuple_size_v<Keys>>>{});
                    }
                    else if constexpr ( RareTs::is_macro_reflected_v<U> ) // Branch to field
                    {
                        (void)((Member_indexes == std::size_t(value & std::uint8_t(path_op::low_bits)) ?
                            (process_event_ref<std::remove_cvref_t<typename RareTs::Member<U, Member_indexes>::type>, Undo, After_sel, RareTs::Member<U, Member_indexes>>(
                                RareTs::Member<U, Member_indexes>::value(t), offset, secondary_offset, operation,
                                std::make_index_sequence<reflected_member_count<std::remove_cvref_t<typename RareTs::Member<U, Member_indexes>::type>>()>(),
                                keys, type_tags<Pathway..., path_member<Member_indexes>>{}), true) : false) || ...);

                        //RareTs::Members<U>::at(std::size_t(value & std::uint8_t(path_op::low_bits)), t, [&](auto member, auto & ref) {
                        //    using elem = std::remove_cvref_t<typename decltype(member)::type>;
                        //    process_event_ref<U2, Undo, After_sel, decltype(member)>(
                        //        ref, offset, secondary_offset, operation, std::make_index_sequence<reflected_member_count<elem>()>(),
                        //        keys, type_tags<Pathway..., path_member<decltype(member)::index>>{});
                        //});
                    }
                }
                break;
                case path_op::leaf_branch:
                {
                    if constexpr ( RareTs::is_static_array_v<U> || RareTs::is_specialization_v<U, std::vector>  ) // Op on index
                    {
                        using path_idx = path_index<std::tuple_size_v<Keys>>;
                        index_type index {};
                        if constexpr ( std::is_same_v<index_type, uint6_t> )
                            index = uint6_t(value & std::uint8_t(path_op::low_bits));
                        else
                            index = read_index<index_type>(offset);

                        if constexpr ( Undo )
                        {
                            process_undo_event<std::remove_cvref_t<RareTs::element_type_t<std::remove_cvref_t<U>>>, Member, Pathway..., path_idx>(
                                operation, offset, secondary_offset, t[static_cast<std::size_t>(index)], std::tuple_cat(keys, std::tuple{index}));
                        }
                        else
                        {
                            process_redo_event<std::remove_cvref_t<RareTs::element_type_t<std::remove_cvref_t<U>>>, Member, Pathway..., path_idx>(
                                operation, offset, secondary_offset, t[static_cast<std::size_t>(index)], std::tuple_cat(keys, std::tuple{index}));
                        }
                    }
                    else if constexpr ( RareTs::is_macro_reflected_v<U> ) // Op on field
                    {
                        std::size_t field_index = std::size_t(value & std::uint8_t(path_op::low_bits));
                        if constexpr ( Undo )
                        {
                            (void)((Member_indexes == field_index ?
                                (process_undo_event<std::remove_cvref_t<typename RareTs::Member<U, Member_indexes>::type>,
                                    RareTs::Member<U, Member_indexes>, Pathway..., path_member<RareTs::Member<U, Member_indexes>::index>>(
                                    operation, offset, secondary_offset, RareTs::Member<U, Member_indexes>::value(t), keys), true) : false) || ...);
                        }
                        else
                        {
                            (void)((Member_indexes == field_index ?
                                (process_redo_event<std::remove_cvref_t<typename RareTs::Member<U, Member_indexes>::type>,
                                    RareTs::Member<U, Member_indexes>, Pathway..., path_member<RareTs::Member<U, Member_indexes>::index>>(
                                    operation, offset, secondary_offset, RareTs::Member<U, Member_indexes>::value(t), keys), true) : false) || ...);
                        }

                        //if constexpr ( Undo ) {
                        //    RareTs::Members<U>::at(std::size_t(value & std::uint8_t(path_op::low_bits)), t, [&](auto member, auto & ref) {
                        //        process_undo_event<std::remove_cvref_t<typename decltype(member)::type>, decltype(member), Pathway..., path_member<decltype(member)::index>>(
                        //            operation, offset, secondary_offset, ref, keys);
                        //    });
                        //} else {
                        //    RareTs::Members<U>::at(std::size_t(value & std::uint8_t(path_op::low_bits)), t, [&](auto member, auto & ref) {
                        //        process_redo_event<std::remove_cvref_t<typename decltype(member)::type>, decltype(member), Pathway..., path_member<decltype(member)::index>>(
                        //            operation, offset, secondary_offset, ref, keys);
                        //    });
                        //}
                    }
                }
                break;
                default:
                    throw std::runtime_error("Invalid path_op detected");
            }
        }

        void undo_event(std::size_t event_index)
        {
            if ( event_index >= event_offsets.size() )
                return;
            
            std::size_t offset = event_offsets[event_index];
            std::optional<std::size_t> secondary_offset {}; // For selection ops with multiple callbacks/bifurcated offsets
            std::uint8_t operation = events[offset];
            ++offset;
            
            process_event_ref<T, true>(t, offset, secondary_offset, operation, std::make_index_sequence<reflected_member_count<T>()>(), std::tuple{}, type_tags<>{});
        }

        void redo_event(std::size_t event_index)
        {
            if ( event_index >= event_offsets.size() )
                return;
            
            std::optional<std::size_t> secondary_offset {}; // For selection ops with multiple callbacks/bifurcated offsets
            std::size_t offset = event_offsets[event_index];
            std::uint8_t operation = events[offset];
            ++offset;
            
            process_event_ref<T, false>(t, offset, secondary_offset, operation, std::make_index_sequence<reflected_member_count<T>()>(), std::tuple{}, type_tags<>{});
        }
    };

    struct no_user_data { friend constexpr bool operator==(no_user_data, no_user_data) noexcept { return false; } };

    template <class Data, class User, class User_data = no_user_data>
    requires RareTs::is_macro_reflected_v<Data> && std::is_object_v<User>
    class tracked;

    template <class T, class User, class Action_user_data, class Editor>
    class modifier_root : private agent<T, User, Editor>, public edit_members_t<agent<T, User, Editor>, typename decltype(def_index_type<T>())::type, T, T>
    {
        modifier_root(T & t, User & user) :
            agent<T, User, Editor>(t, user),
            edit_members_t<agent<T, User, Editor>, typename decltype(def_index_type<T>())::type, T, T>{(agent<T, User, Editor>&)*this, std::tuple{}} {}

        friend class tracked<T, User, Action_user_data>;
        friend struct agent<T, User, Editor>;

    public:
        using default_index_type = typename agent<T, User, Editor>::default_index_type;

        void assign(const T & value) {
            std::tuple keys {}; // No keys/array indexes as this is the root
            agent<T, User, Editor>::template set<>(value, keys);
            agent<T, User, Editor>::selections.clear();
            agent<T, User, Editor>::init_attached_data();
        }
    };
    
    template <typename T, typename Path_to_elem,
        typename Editor_type = typename Path_to_elem::editor_type,
        typename Read_edit_pair = decltype(std::declval<typename Path_to_elem::editor_type>().edit_from_path(Path_to_elem{}))>
    class tracked_element;

    template <class Tracked>
    class editor
    {
        Tracked* parent = nullptr;

        using tracked_type = Tracked;
        template <typename, typename, typename, typename> friend class tracked_element;

    public:
        editor(Tracked* parent) : parent(parent) {
            if ( parent != nullptr )
                parent->action_reference_count++;
        }
        editor(editor && other) noexcept { std::swap(parent, other.parent); }
        ~editor() {
            if ( parent != nullptr && --(parent->action_reference_count) == 0 )
                parent->submit_action();
        }
        constexpr typename Tracked::mod_root_type & operator*() noexcept { return parent->mod_root; }
        constexpr typename Tracked::mod_root_type* operator->() noexcept { return &(parent->mod_root); }

        template <class Keys, class ... Pathway>
        auto edit_from_path(path_tagged_keys<Keys, type_tags<Pathway...>, editor<Tracked>> path)
        {
            auto & edit_agent = (agent<typename Tracked::data_type, typename Tracked::user_type, editor> &)parent->mod_root;
            return edit_agent.template editor_from_path<Pathway...>(parent->mod_root, parent->data, path);
        }
    };
    
    template <class Usr>
    using after_action_op = decltype(std::declval<Usr>().after_action(std::size_t(0)));

    enum class action_status {
        unknown = 0, //!< the status rendered_actions get initialized to, this is expected to be updated by the renderer
        undoable = 1, //!< an action exists prior to the current action cursor and can be undone
        elided_redo = 2, //!< an action was redoable but has since been elided from the main history branch and can no longer be undone/redone
        redoable = 3 //!< an action exists after the current action cursor and can be redone
    };

    struct data_change_event {
        op operation {};
        std::string summary {};
        std::vector<std::string> breakdown {};
        constexpr bool is_sel_change_event() const { return is_sel_change_op(operation); }
    };

    template <class Action_user_data = no_user_data>
    struct rendered_action {
        action_status status {};
        std::size_t elision_count = 0;
        std::vector<data_change_event> change_events {};
        std::size_t byte_count = 0;
        Action_user_data user_data {};

        constexpr bool is_elision_marker() const { return status == action_status::elided_redo && elision_count > 0; }
        constexpr bool is_elided() const { return status == action_status::elided_redo && elision_count == 0; }
        constexpr bool is_undoable() const { return status == action_status::undoable; }
        constexpr bool is_redoable() const { return status == action_status::redoable; }
        constexpr bool is_sel_change_action() const {
            for ( const auto & event : change_events )
            {
                if ( !event.is_sel_change_event() )
                    return false;
            }
            return true;
        }
    };

    template <class User_data = no_user_data>
    struct action_record : User_data
    {
        std::size_t first_event_index;
        
        constexpr action_record(std::size_t first_event_index) noexcept : first_event_index(first_event_index) {}
        constexpr action_record(std::size_t first_event_index, const User_data & user_data) noexcept : User_data(user_data), first_event_index(first_event_index) {}
    };

    template <class Data, class User, class User_data>
    requires RareTs::is_macro_reflected_v<Data> && std::is_object_v<User>
    class tracked
    {
        friend class editor<tracked>;

        Data data;

        using data_type = Data;
        using user_type = User;
        using mod_root_type = modifier_root<Data, User, User_data, editor<tracked>>;

        static constexpr std::uint64_t flag_elided_redos     = 0x8000000000000000ull;
        static constexpr std::uint64_t mask_elided_redo_size = 0x7FFFFFFFFFFFFFFFull; // The total size of this elided redo branch, including sub-branches

        mod_root_type mod_root;
        agent<Data, User, editor<tracked>> & history = static_cast<agent<Data, User, editor<tracked>> &>(mod_root);

        User_data pending_action_user_data {}; // The user data that will be associated with the next action to be added
        std::size_t pending_action_start = 0; // Index of the first data-change event for the next action to be added
        std::vector<action_record<User_data>> actions; // Contains the index of the first data-change event for action[i] and any user-data
        int action_reference_count = 0; // Referencing counting for the current action, new actions can only be created when the old action is closed
        std::uint64_t redo_count = 0; // How many undos have occured since the last user-action/how many redos are available
        std::uint64_t redo_size = 0; // The size of the range including the redoable actions (includes elided redos)

        template <typename Usr> static constexpr bool has_after_action_op = RareTs::op_exists_v<after_action_op, Usr>;

        void elide_redos()
        {
            if ( redo_count == 0 )
                throw std::logic_error("Redos cannot be elided when there are no redos");
            
            actions.push_back({redo_size | flag_elided_redos, pending_action_user_data});
            redo_count = 0;
            redo_size = 0;
        }

        void submit_action()
        {
            if ( pending_action_start < mod_root.event_offsets.size() ) // Action is not empty
            {
                if ( redo_count > 0 )
                    elide_redos();

                actions.push_back({pending_action_start, pending_action_user_data});
                pending_action_start = mod_root.event_offsets.size();

                if constexpr ( has_after_action_op<user_type> )
                    static_cast<agent<Data, User, editor<tracked>> &>(mod_root).user.after_action(actions.size()-1); // Notify
            }
        }

    public:

        /// Used to view selections data (and potentially other info associated with particular data paths)
        const mod_root_type & view;
        tracked(User* user) : mod_root(this->data, *user), view(mod_root) {}
        tracked(User & user) : mod_root(this->data, user), view(mod_root) {}

        /// A read only version of the user data
        const Data & read = data;
        constexpr const Data & operator*() const noexcept { return read; }
        constexpr const Data* operator->() const noexcept { return &this->data; }

    protected:

        void replace_pending_action_user_data(User_data user_data)
        {
            std::swap(user_data, pending_action_user_data);
        }

        struct pather
        {
            constexpr mod_root_type & operator*() noexcept;
            constexpr mod_root_type* operator->() noexcept;
        };
        /// Represents the root of the data structure, used by client code to create paths
        static inline pather root {};

    public:

        auto create_action(User_data user_data = {})
        {
            if ( action_reference_count == 0 || User_data{} == pending_action_user_data )
                std::swap(user_data, pending_action_user_data);

            return editor<tracked>{this};
        }

        auto operator()(User_data user_data = {})
        {
            if ( action_reference_count == 0 || User_data{} == pending_action_user_data )
                std::swap(user_data, pending_action_user_data);

            return editor<tracked>{this};
        };

        void clear_history()
        {
            if ( action_reference_count != 0 )
                throw std::logic_error("Cannot clear history while an action is active");

            pending_action_user_data = {};
            pending_action_start = 0;
            history.clear();
            redo_count = 0;
            redo_size = 0;
            actions.clear();
        }

        /// Trims history so it starts at new_first_action; if new_first_action is elided the trim starts at the next unelided action
        /// @param new_first_action the action you want to come first in the history after trimming
        /// @return new_first_action if new_first_action was not elided, the first unelided action after that otherwise, zero if history is or becomes empty
        std::size_t trim_history(std::size_t new_first_action)
        {
            if ( action_reference_count != 0 || redo_count > 0 )
                throw std::logic_error("Cannot trim history while an action is active or redos are present");

            if ( new_first_action >= actions.size() )
            {
                clear_history(); // Clear everything
                return 0;
            }
            else if ( new_first_action == 0 || actions.empty() )
                return 0; // Nothing to clear

            for ( std::ptrdiff_t i = static_cast<std::ptrdiff_t>(actions.size()) - 1; i > 0; )
            {
                if ( (actions[static_cast<std::size_t>(i)].first_event_index & flag_elided_redos) == flag_elided_redos ) // Elided
                {
                    i -= static_cast<std::ptrdiff_t>((actions[static_cast<std::size_t>(i)].first_event_index & mask_elided_redo_size)+1); // Skip over elided actions
                    if ( i <= 0 )
                        return 0; // Nothing to clear before the given "new_first_action" that wasn't elided
                }
                else if ( i > static_cast<std::ptrdiff_t>(new_first_action) ) // Non-elided, haven't yet reached new_first_action
                    --i; // Keep moving towards new_first_action
                else // Reached or passed new_first_action
                {
                    if ( i <= 0 )
                        return 0; // Nothing to clear before the given "new_first_action" that wasn't elided
                    else
                    {
                        new_first_action = static_cast<std::size_t>(i); // Fix new_first_action to the last action that wasn't elided
                        break;
                    }
                }
            }

            std::size_t first_event_index = actions[new_first_action].first_event_index;
            std::size_t count_event_indexes_trimmed = history.trim(first_event_index);
            actions.erase(actions.begin(), std::next(actions.begin(), static_cast<std::ptrdiff_t>(new_first_action)));
            for ( auto & action : actions )
            {
                if ( (action.first_event_index & flag_elided_redos) != flag_elided_redos )
                    action.first_event_index -= count_event_indexes_trimmed;
            }

            pending_action_start = mod_root.event_offsets.size();
            return new_first_action;
        }

        /// Trims the history such that size is less than or equal to max_size
        /// @param max_size the maximum size for the history to have after trimming (actions must be kept whole so the new byte count will likely be less)
        /// @return the new size
        std::uint64_t trim_history_to_size(std::uint64_t max_size)
        {
            if ( action_reference_count != 0 || redo_count > 0 )
                throw std::logic_error("Cannot trim history while an action is active or redos are present");

            if ( max_size == 0 )
            {
                clear_history(); // Clear everything
                return 0;
            }
            else if ( actions.empty() ) // Nothing to trim
                return 0;
            
            std::size_t total_actions = actions.size();
            std::uint64_t total_size = 0;
            for ( std::ptrdiff_t i = static_cast<std::ptrdiff_t>(actions.size()) - 1; i > 0; )
            {
                if ( (actions[static_cast<std::size_t>(i)].first_event_index & flag_elided_redos) == flag_elided_redos ) // Elided
                {
                    std::uint64_t count_elided = (actions[static_cast<std::size_t>(i)].first_event_index & mask_elided_redo_size);
                    std::uint64_t next_size = total_size + 8 + 8*count_elided;
                    std::ptrdiff_t next_i = i - static_cast<std::ptrdiff_t>(count_elided+1); // Next after elided actions
                    for ( std::ptrdiff_t j=i; j>next_i; --j )
                    {
                        std::size_t action_index = static_cast<std::size_t>(j);
                        std::size_t curr_action_start = actions[action_index].first_event_index;
                        if ( (curr_action_start & flag_elided_redos) == flag_elided_redos )
                            continue; // No events to count for this action
                        
                        std::size_t next_action_start = flag_elided_redos;
                        for ( std::size_t k=1; (next_action_start & flag_elided_redos) == flag_elided_redos; ++k ) // Find the next event that isn't an elision marker
                            next_action_start = action_index+k<total_actions ? (actions[action_index+k].first_event_index) : mod_root.event_offsets.size();

                        auto [first_event_start, unused_first_event_end] = mod_root.get_event_offset_range(curr_action_start);
                        auto [unused_last_event_start, last_event_end] = mod_root.get_event_offset_range(next_action_start-1);
                        next_size += last_event_end-first_event_start;
                    }
                    if ( next_size > max_size ) // i is the correct target
                    {
                        trim_history(static_cast<std::size_t>(i));
                        return total_size;
                    }
                    else
                    {
                        i = next_i;
                        total_size = next_size;
                    }
                }
                else // Non-elided, haven't yet reached max_size
                {
                    std::size_t action_index = static_cast<std::size_t>(i);
                    std::uint64_t next_size = total_size + 8;
                    std::size_t curr_action_start = actions[action_index].first_event_index;
                    if ( (curr_action_start & flag_elided_redos) == flag_elided_redos )
                        continue; // No events to count for this action
                        
                    std::size_t next_action_start = flag_elided_redos;
                    for ( std::size_t j=1; (next_action_start & flag_elided_redos) == flag_elided_redos; ++j ) // Find the next event that isn't an elision marker
                        next_action_start = action_index+j<total_actions ? (actions[action_index+j].first_event_index) : mod_root.event_offsets.size();

                    auto [first_event_start, unused_first_event_end] = mod_root.get_event_offset_range(curr_action_start);
                    auto [unused_last_event_start, last_event_end] = mod_root.get_event_offset_range(next_action_start-1);
                    next_size += last_event_end-first_event_start;

                    if ( next_size > max_size )
                    {
                        trim_history(static_cast<std::size_t>(i));
                        return total_size;
                    }
                    else
                    {
                        total_size = next_size;
                        --i;
                    }
                }
            }
            return total_size;
        }

        /// Initializes the stored data with the given input, this initialization is tracked of initTracked is true
        /// If there's an active action or any stored actions a logic_error will be thrown
        /// @param data The value to initialize the stored data to
        template <bool Init_tracked = false>
        void init_data(auto && data)
        {
            if ( !actions.empty() )
                throw std::logic_error("Cannot init an object that already has history!");
            else if ( action_reference_count > 0 )
                throw std::logic_error("Cannot init an object that already has an active action!");

            if constexpr ( Init_tracked )
            {
                auto action = create_action();
                auto keys = std::tuple{};
                history.selections.clear();
                if constexpr ( requires { std::swap(this->data, data); } )
                    std::swap(this->data, data);
                else // If say, a const reference was passed to init_data, swap is not possible, use assignment
                    this->data = data;
                
                history.init_attached_data();
                history.record_initialization(keys);
            }
            else
            {
                history.selections.clear();
                if constexpr ( requires { std::swap(this->data, data); } )
                    std::swap(this->data, data);
                else // If say, a const reference was passed to init_data, swap is not possible, use assignment
                    this->data = data;
                
                history.init_attached_data();
            }
        }

        /// Creates an action recording the initialization of an object which has already occurred
        void record_init()
        {
            if ( !actions.empty() )
                throw std::logic_error("Cannot record initialization of an object that already has history!");
            else if ( action_reference_count > 0 )
                throw std::logic_error("Cannot record initialization of an object that already has an active action!");
            
            auto action = create_action();
            auto keys = std::tuple{};
            history.record_initialization(keys);
        }

        static constexpr std::size_t no_action = std::numeric_limits<std::size_t>::max();

        std::size_t undo_action()
        {
            if ( redo_size >= actions.size() )
                return no_action;

            std::size_t total_actions = actions.size();
            std::uint64_t action_index = total_actions-redo_size-1;

            while ( (actions[action_index].first_event_index & flag_elided_redos) == flag_elided_redos ) // Find the next unelided action
            {
                auto redo_gap = ((actions[action_index].first_event_index & mask_elided_redo_size)+1);
                if ( redo_gap <= action_index )
                    action_index -= redo_gap;
                else
                    return no_action; // Every prior action was elided, nothing to undo
            }

            std::int64_t action_event_start = static_cast<std::int64_t>(actions[action_index].first_event_index);
            std::int64_t next_action_start = static_cast<std::size_t>(action_index)<total_actions-1 ?
                static_cast<std::int64_t>(actions[action_index+1].first_event_index) :
                static_cast<std::int64_t>(mod_root.event_offsets.size());
            
            auto & edit_agent = (agent<Data, User, editor<tracked>> &)mod_root;
            for ( std::int64_t i=next_action_start-1; i>=action_event_start; i-- )
                edit_agent.undo_event(std::uint64_t(i));

            redo_count++;
            redo_size = total_actions-action_index;
            return action_index;
        }

        std::size_t redo_action()
        {
            if ( redo_count == 0 )
                return no_action;

            std::size_t total_actions = actions.size();
            std::uint64_t action_index = total_actions-redo_size;
            
            std::uint64_t action_event_start = actions[action_index].first_event_index;
            std::uint64_t next_action_start = flag_elided_redos;
            for ( std::size_t i=1; action_index+i<total_actions; ++i )
            {
                if ( (actions[action_index+i].first_event_index & flag_elided_redos) != flag_elided_redos )
                {
                    next_action_start = actions[action_index+i].first_event_index;
                    break;
                }
            }
            if ( next_action_start == flag_elided_redos )
                next_action_start = mod_root.event_offsets.size();
            
            auto & edit_agent = (agent<Data, User, editor<tracked>> &)mod_root;
            for ( auto i=action_event_start; i<next_action_start; i++ )
                edit_agent.redo_event(i);

            redo_count--;
            if ( redo_count == 0 )
                redo_size = 0;
            else
            {
                auto check_action_index = total_actions-1;
                std::size_t unelided_count = 0;
                while ( unelided_count < redo_count )
                {
                    if ( (actions[check_action_index].first_event_index & flag_elided_redos) == flag_elided_redos )
                        check_action_index -= ((actions[check_action_index].first_event_index & mask_elided_redo_size)+1);
                    else
                    {
                        ++unelided_count;
                        --check_action_index;
                    }
                }
                redo_size = total_actions-check_action_index-1;
            }
            return action_index;
        }

        /// Includes undoable, redoable, elided, and pending actions
        /// @return The total count of actions, including undoable, redoable, elided, and pending actions
        std::size_t total_actions()
        {
            return actions.size();
        }

        /// The index the current action will have if it's submitted
        /// @return Index the current action will have if it's submitted
        std::size_t get_pending_action_index()
        {
            return redo_count > 0 ? actions.size()+1 : actions.size();
        }

        /// One after the index of the last action which hasn't been undone
        /// @return Index one after the last action which hasn't been undone
        std::size_t get_cursor_index()
        {
            return actions.size()-redo_size;
        }

        /// One after the index of the most recent un-elided action before the current cursor
        /// @return Index one after the most recent un-elided action before the current cursor
        std::size_t previous_cursor_index()
        {
            for ( auto i = static_cast<std::ptrdiff_t>(get_cursor_index())-2; i>=0; --i )
            {
                if ( (actions[static_cast<std::size_t>(i)].first_event_index & flag_elided_redos) == flag_elided_redos )
                    i -= static_cast<std::ptrdiff_t>(actions[static_cast<std::size_t>(i)].first_event_index & mask_elided_redo_size);
                else
                    return static_cast<std::size_t>(i)+1;
            }
            return 0;
        }

        const User_data & get_action_user_data(std::size_t action_index) const
        {
            return (User_data &)(actions[action_index]);
        }

    private:

        auto & put(std::ostream & os, auto && value) const
        {
            os << (RareTs::promote_char_t<decltype(value)>)value;
            return *this;
        }

        template <class type, class member_type>
        auto & put_value(std::ostream & os, std::size_t & offset) const
        {
            if constexpr ( std::is_array_v<type> )
            {
                type t {};
                mod_root.template read_value<type, member_type>(offset, t);
                os << Json::out<Json::Statics::Excluded, RareTs::NoNote, 0, Json::Output::twoSpaces, type>(t);
            }
            else
                os << Json::out(mod_root.template read_value<type, member_type>(offset));

            return *this;
        }

        template <class type, class member_type>
        auto & put_values(std::ostream & os, std::size_t & offset, std::size_t size) const
        {
            os << "{";
            if constexpr ( std::is_array_v<type> )
            {
                for ( std::size_t i=0; i<size; ++i )
                {
                    type t {};
                    mod_root.template read_value<type, member_type>(offset, t);
                    os << (i>0 ? ", " : "") << Json::out(t);
                }
            }
            else
            {
                for ( std::size_t i=0; i<size; ++i )
                    os << (i>0 ? ", " : "") << Json::out(mod_root.template read_value<type, member_type>(offset));
            }
            os << "}";
            return *this;
        }

        template <class index_type>
        auto & put_index(std::ostream & os, std::size_t & offset) const
        {
            os << (RareTs::promote_char_t<index_type>)mod_root.template read_index<index_type>(offset);
            return *this;
        }

        template <class index_type>
        auto & put_indexes(std::ostream & os, std::size_t & offset, std::size_t size) const
        {
            os << "{";
            if ( size > 0 )
            {
                auto indexes = mod_root.template read_indexes<index_type>(offset, size);
                for ( std::size_t i=0; i<size; ++i )
                    os << (i>0 ? ", " : "") << (RareTs::promote_char_t<index_type>)indexes[i];
            }
            os << "}";
            return *this;
        }

        template <class type, class member_type>
        void print_event_op(std::ostream & os, std::size_t & offset, op operation) const
        {
            using element = RareTs::element_type_t<std::remove_cvref_t<type>>;
            using index_type = index_type_t<typename mod_root_type::default_index_type, member_type>;
            switch ( operation )
            {
                case op::init: // .init() // value
                    put(os, ".init(").template put_value<type, member_type>(os, offset).put(os, ")");
                    break;
                case op::reset: // .reset() // prev_value
                    put(os, ".reset() // ").template put_value<type, member_type>(os, offset);
                    break;
                case op::reserve: // .reserve(size)
                    put(os, ".reserve(").template put_index<index_type>(os, offset).put(os, ")");
                    break;
                case op::trim: // .trim()
                    put(os, ".trim()");
                    break;
                case op::assign: // .assign(size, value) // prev_value
                    if constexpr ( !std::is_void_v<element> )
                        put(os, ".assign(").template put_index<index_type>(os, offset).template put_value<element, member_type>(os, offset).put(os, ") // ").template put_value<type, member_type>(os, offset);
                    break;
                case op::assign_default: // .assign(size, {}) // prev_value
                    put(os, ".assign(").template put_index<index_type>(os, offset).put(os, ", {}) // ").template put_value<type, member_type>(os, offset);
                    break;
                case op::clear_selections: // .clear_selections() // size, sel_indexes
                {
                    auto size = static_cast<std::size_t>(mod_root.template read_index<index_type>(offset));
                    put(os, ".clear_selections() // ").put(os, size).put(os, ", ").template put_indexes<index_type>(os, offset, size);
                }
                break;
                case op::select_all: // .select_all() // size, sel_indexes
                {
                    auto size = static_cast<std::size_t>(mod_root.template read_index<index_type>(offset));
                    put(os, ".select_all() // ").put(os, size).put(os, ", ").template put_indexes<index_type>(os, offset, size);
                }
                break;
                case op::select: // .select(index)
                    put(os, ".select(").template put_index<index_type>(os, offset).put(os, ")");
                    break;
                case op::select_n: // .selectN(size, sel_indexes)
                {
                    auto size = static_cast<std::size_t>(mod_root.template read_index<index_type>(offset));
                    put(os, ".select_n(").put(os, size).put(os, ", ").template put_indexes<index_type>(os, offset, size).put(os, ")");
                }
                break;
                case op::deselect: // .deselect(index)
                    put(os, ".deselect(").template put_index<index_type>(os, offset).put(os, ")");
                    break;
                case op::deselect_n:
                {
                    auto size = static_cast<std::size_t>(mod_root.template read_index<index_type>(offset));
                    put(os, ".deselect_n(").put(os, size).put(os, ", ").template put_indexes<index_type>(os, offset, size).put(os, ")");
                }
                break;
                case op::toggle_selection:
                    put(os, ".toggel_sel(").template put_index<index_type>(os, offset).put(os, ")");
                    break;
                case op::toggle_selection_n:
                {
                    auto size = static_cast<std::size_t>(mod_root.template read_index<index_type>(offset));
                    put(os, ".toggle_sel_n(").put(os, size).put(os, ", ").template put_indexes<index_type>(os, offset, size).put(os, ")");
                }
                break;
                case op::sort_selections:
                {
                    auto size = static_cast<std::size_t>(mod_root.template read_index<index_type>(offset));
                    put(os, ".sort_sel(").put(os, size).put(os, ", ").template put_indexes<index_type>(os, offset, size).put(os, ")");
                }
                break;
                case op::sort_selections_desc:
                {
                    auto size = static_cast<std::size_t>(mod_root.template read_index<index_type>(offset));
                    put(os, ".sort_sel_desc(").put(os, size).put(os, ", ").template put_indexes<index_type>(os, offset, size).put(os, ")");
                }
                break;
                case op::set:
                    put(os, " = ").template put_value<type, member_type>(os, offset).put(os, " // ").template put_value<type, member_type>(os, offset);
                    break;
                case op::set_n:
                if constexpr ( !std::is_void_v<element> )
                {
                    std::size_t size = static_cast<std::size_t>(mod_root.template read_index<index_type>(offset));
                    put(os, ".set_n(").template put_index<index_type>(os, offset).put(os, ", ").template put_indexes<index_type>(os, offset, size).put(os, ")").put(os, " = ").template put_value<type, member_type>(os, offset)
                        .put(os, ") // ").template put_values<type, member_type>(os, offset, size);
                }
                break;
                case op::set_l:
                    put(os, " = ").template put_value<type, member_type>(os, offset);
                    break;
                case op::append:
                if constexpr ( RareTs::is_specialization_v<type, std::vector> )
                    put(os, ".append(").template put_value<std::remove_cvref_t<decltype(std::declval<type>()[0])>, member_type>(os, offset).put(os, ")");
                break;
                case op::append_n:
                if constexpr ( RareTs::is_specialization_v<type, std::vector> )
                {
                    std::size_t size = static_cast<std::size_t>(mod_root.template read_index<index_type>(offset));
                    put(os, ".append_n(").put(os, size).put(os, ", ").template put_values<element, member_type>(os, offset, size).put(os, ")");
                }
                break;
                case op::insert:
                if constexpr ( RareTs::is_specialization_v<type, std::vector> )
                    put(os, ".insert(").template put_index<index_type>(os, offset).put(os, ", ").template put_value<std::remove_cvref_t<decltype(std::declval<type>()[0])>, member_type>(os, offset).put(os, ")");
                break;
                case op::insert_n:
                if constexpr ( RareTs::is_specialization_v<type, std::vector> )
                {
                    std::size_t size = static_cast<std::size_t>(mod_root.template read_index<index_type>(offset));
                    put(os, ".insert_n(").template put_index<index_type>(os, offset).put(os, ", ").put(os, size).put(os, ", ").template put_values<element, member_type>(os, offset, size).put(os, ")");
                }
                break;
                case op::remove:
                if constexpr ( RareTs::is_specialization_v<type, std::vector> )
                    put(os, ".remove(").template put_index<index_type>(os, offset).put(os, ") // {").template put_value<std::remove_cvref_t<decltype(std::declval<type>()[0])>, member_type>(os, offset).put(os, ")");
                break;
                case op::remove_n:
                if constexpr ( RareTs::is_specialization_v<type, std::vector> )
                {
                    std::size_t count = static_cast<std::size_t>(mod_root.template read_index<index_type>(offset));
                    put(os, ".remove_n(").template put_indexes<index_type>(os, offset, count).put(os, ") // ").template put_indexes<index_type>(os, offset, count);
                }
                break;
                case op::remove_l:
                if constexpr ( RareTs::is_specialization_v<type, std::vector> )
                {
                    std::size_t count = static_cast<std::size_t>(mod_root.template read_index<index_type>(offset));
                    put(os, ".remove_l() // ").put(os, count).put(os, ", ").template put_indexes<index_type>(os, offset, count).put(os, ") // ").template put_values<element, member_type>(os, offset, count);
                }
                break;
                case op::sort:
                {
                    std::size_t count = static_cast<std::size_t>(mod_root.template read_index<index_type>(offset));
                    put(os, ".sort() // ").put(os, count).template put_indexes<index_type>(os, offset, count);
                }
                break;
                case op::sort_desc:
                {
                    std::size_t count = static_cast<std::size_t>(mod_root.template read_index<index_type>(offset));
                    put(os, ".sort_desc() // ").put(os, count).template put_indexes<index_type>(os, offset, count);
                }
                break;
                case op::swap:
                    put(os, ".swap(").template put_index<index_type>(os, offset).put(os, ", ").template put_index<index_type>(os, offset).put(os, ")");
                    break;
                case op::move_up:
                    put(os, ".move_up(").template put_index<index_type>(os, offset).put(os, ")");
                    break;
                case op::move_up_n:
                {
                    auto size = static_cast<std::size_t>(mod_root.template read_index<index_type>(offset));
                    put(os, ".move_up_n(").put(os, size).put(os, ", ").template put_indexes<index_type>(os, offset, size).put(os, ")");
                }
                break;
                case op::move_up_l:
                    put(os, ".move_selections_up()");
                    break;
                case op::move_top:
                    put(os, ".move_top(").template put_index<index_type>(os, offset).put(os, ")");
                    break;
                case op::move_top_n:
                {
                    auto size = static_cast<std::size_t>(mod_root.template read_index<index_type>(offset));
                    put(os, ".move_top_n(").put(os, size).put(os, ", ").template put_indexes<index_type>(os, offset, size).put(os, ")");
                }
                break;
                case op::move_top_l:
                    put(os, ".move_selections_top()");
                    break;
                case op::move_down:
                    put(os, ".move_down(").template put_index<index_type>(os, offset).put(os, ")");
                    break;
                case op::move_down_n:
                {
                    auto size = static_cast<std::size_t>(mod_root.template read_index<index_type>(offset));
                    put(os, ".move_down_n(").put(os, size).put(os, ", ").template put_indexes<index_type>(os, offset, size).put(os, ")");
                }
                break;
                case op::move_down_l:
                    put(os, ".move_selections_down()");
                    break;
                case op::move_bottom:
                    put(os, ".move_bottom(").template put_index<index_type>(os, offset).put(os, ")");
                    break;
                case op::move_bottom_n:
                {
                    auto size = static_cast<std::size_t>(mod_root.template read_index<index_type>(offset));
                    put(os, ".move_bottom_n(").put(os, size).put(os, ", ").template put_indexes<index_type>(os, offset, size).put(os, ")");
                }
                break;
                case op::move_bottom_l:
                    put(os, ".move_selections_bottom()");
                    break;
                case op::move_to:
                    put(os, ".move_to(").template put_index<index_type>(os, offset).put(os, ", ").template put_index<index_type>(os, offset).put(os, ")");
                    break;
                case op::move_to_n:
                {
                    auto target = static_cast<std::size_t>(mod_root.template read_index<index_type>(offset));
                    auto size = static_cast<std::size_t>(mod_root.template read_index<index_type>(offset));
                    put(os, ".move_to_n(").put(os, target).put(os, ", ").put(os, size).put(os, ", ").template put_indexes<index_type>(os, offset, size).put(os, ")");
                }
                break;
                case op::move_to_l:
                    put(os, ".move_selections_to(").template put_index<index_type>(os, offset).put(os, ")");
                    break;
            }
        }

        template <class U, class Member = void, std::size_t ... Is>
        void print_event(std::ostream & os, std::size_t & offset, std::uint8_t operation, std::index_sequence<Is...>) const
        {
            using base_index_type = index_type_t<typename mod_root_type::default_index_type, Member>;
            std::uint8_t value = mod_root.events[offset];
            ++offset;
            
            switch ( path_op(value & std::uint8_t(path_op::high_bits)) )
            {
                case path_op::sel_branch:
                    if constexpr ( RareTs::is_static_array_v<U> || RareTs::is_specialization_v<U, std::vector> )
                    {
                        os << "[{sel}]";
                        using element_type = RareTs::element_type_t<std::remove_cvref_t<U>>;
                        constexpr bool is_leaf = !RareTs::is_reflected_v<element_type> && std::is_void_v<RareTs::element_type_t<element_type>>;
                        if constexpr ( is_leaf )
                            print_event_op<element_type, Member>(os, offset, op(operation));
                        else
                            print_event<std::remove_cvref_t<decltype(std::declval<U>()[0])>, Member>(os, offset, operation, std::make_index_sequence<reflected_member_count<std::remove_cvref_t<decltype(std::declval<U>()[0])>>()>());
                    }
                    break;
                case path_op::leaf_sel_branch:
                    if constexpr ( std::is_same_v<std::remove_cvref_t<Data>, std::remove_cvref_t<U>> ) // Root
                        print_event_op<U, Member>(os, offset, op(operation));
                    else if constexpr ( RareTs::is_static_array_v<U> || RareTs::is_specialization_v<U, std::vector> )
                    {
                        os << "[{sel}]";
                        using element_type = RareTs::element_type_t<std::remove_cvref_t<U>>;
                        print_event_op<element_type, Member>(os, offset, op(operation));
                    }
                    break;
                case path_op::branch:
                {
                    if constexpr ( RareTs::is_static_array_v<U> || RareTs::is_specialization_v<U, std::vector> ) // Branch to index
                    {
                        base_index_type index {};
                        if constexpr ( std::is_same_v<uint6_t, base_index_type> )
                            index = static_cast<base_index_type>(value & std::uint8_t(path_op::low_bits));
                        else
                            index = mod_root.template read_index<base_index_type>(offset);

                        os << '[' << static_cast<std::size_t>(index) << ']';
                        print_event<std::remove_cvref_t<decltype(std::declval<U>()[0])>, Member>(os, offset, operation, std::make_index_sequence<reflected_member_count<std::remove_cvref_t<decltype(std::declval<U>()[0])>>()>());
                    }
                    else if constexpr ( RareTs::is_optional_v<U> ) // Deref into optional
                    {
                        print_event<typename U::value_type, Member>(os, offset, operation, std::make_index_sequence<reflected_member_count<std::remove_cvref_t<typename U::value_type>>()>());
                    }
                    else if constexpr ( RareTs::is_macro_reflected_v<U> ) // Branch to field
                    {
                        std::size_t member_index = std::size_t(value & std::uint8_t(path_op::low_bits));
                        (void)(
                            (Is == member_index ? (
                                os << "." << RareTs::Member<U, Is>::name,
                                print_event<std::remove_cvref_t<typename RareTs::Member<U, Is>::type>, RareTs::Member<U, Is>>(os, offset, operation, std::make_index_sequence<reflected_member_count<std::remove_cvref_t<typename RareTs::Member<U, Is>::type>>()>()),
                                true) : true) && ...
                        );

                        //RareTs::Members<U>::at(std::size_t(value & std::uint8_t(path_op::low_bits)), [&](auto member) {
                        //    os << "." << member.name;
                        //    print_event<std::remove_cvref_t<typename decltype(member)::type>, decltype(member)>(os, offset, operation, std::make_index_sequence<reflected_member_count<U>()>());
                        //});
                    }
                }
                break;
                case path_op::leaf_branch:
                {
                    if constexpr ( RareTs::is_static_array_v<U> || RareTs::is_specialization_v<U, std::vector> ) // Op on index
                    {
                        base_index_type index {};
                        if constexpr ( std::is_same_v<uint6_t, base_index_type> )
                            index = static_cast<base_index_type>(value & std::uint8_t(path_op::low_bits));
                        else
                            index = mod_root.template read_index<base_index_type>(offset);

                        os << "[" << static_cast<std::size_t>(index) << "]";
                        print_event_op<RareTs::element_type_t<std::remove_cvref_t<U>>, Member>(os, offset, op(operation));
                    }
                    else if constexpr ( RareTs::is_optional_v<U> ) // Operate on optional
                    {
                        print_event_op<typename U::value_type, Member>(os, offset, op(operation));
                    }
                    else if constexpr ( RareTs::is_reflected_v<U> ) // Op on field
                    {
                        std::size_t member_index = std::size_t(value & std::uint8_t(path_op::low_bits));
                        (void)(
                            (Is == member_index ? (
                                os << "." << RareTs::Member<U, Is>::name,
                                print_event_op<typename RareTs::Member<U, Is>::type, RareTs::Member<U, Is>>(os, offset, op(operation)),
                                true) : true) && ...
                        );
                        //RareTs::Members<U>::at(member_index, [&](auto member) {
                        //    os << "." << member.name;
                        //    print_event_op<typename decltype(member)::type, decltype(member)>(os, offset, Op(value));
                        //});
                    }
                }
                break;
                default: throw std::exception(); // Unreachable
            }
        }

    public:

        template <class U, class Member = void>
        void print_event(std::ostream & os, std::size_t & offset) const
        {
            std::uint8_t operation = mod_root.events[offset];
            ++offset;
            print_event<U, Member>(os, offset, operation, std::make_index_sequence<reflected_member_count<U>()>());
        }

        void print_change_history(std::ostream & os) const
        {
            std::size_t total_actions = actions.size();
            for ( std::size_t action_index=0; action_index<total_actions; ++action_index )
            {
                std::size_t curr_action_start = actions[action_index].first_event_index;
                if ( (curr_action_start & flag_elided_redos) == flag_elided_redos )
                {
                    auto elision_count = curr_action_start & mask_elided_redo_size;
                    os << "Action[" << action_index << "] marks the previous " << elision_count << " action(s) as elided redos\n";
                    continue;
                }

                std::size_t next_action_start = flag_elided_redos;
                for ( std::size_t i=1; (next_action_start & flag_elided_redos) == flag_elided_redos; ++i ) // Find the next event that isn't an elision marker
                    next_action_start = action_index+i<total_actions ? (actions[action_index+i].first_event_index) : mod_root.event_offsets.size();

                if ( curr_action_start == next_action_start )
                {
                    os << "Action[" << action_index << "] is blank\n";
                    continue;
                }

                os << "Action[" << action_index << "] contains events [" << curr_action_start << ", " << next_action_start << ")\n";
                for ( std::size_t event_index = curr_action_start; event_index < next_action_start; ++event_index )
                {
                    auto [event_offset, byte_end] = mod_root.get_event_offset_range(event_index);

                    os << "  [" << std::setw(2) << event_index << ','
                        << std::setw(3) << event_offset << ','
                        << std::setw(2) << (byte_end-mod_root.event_offsets[event_index]) << "] " << std::hex << std::uppercase;
                    for ( std::size_t i=event_offset; i<byte_end; ++i )
                        os << (mod_root.events[i] <= 0xF ? "0" : "") << int(mod_root.events[i]) << ' ';
                    os << std::nouppercase << std::dec;

                    os << " // edit";
                    print_event<Data>(os, event_offset);
                    os << '\n';
                }
                os << '\n';
            }
        }

        template <class U, class Member = void>
        void render_event(data_change_event & data_change_event, std::size_t & offset) const
        {
            std::uint8_t operation = mod_root.events[offset];
            ++offset;
            std::stringstream ss {};
            print_event<U, Member>(ss, offset, operation, std::make_index_sequence<reflected_member_count<U>()>());
            data_change_event.operation = op(operation);
            data_change_event.summary += ss.str();
        }   

        void render_action(std::size_t action_index, rendered_action<User_data> & action, bool include_events = false) const
        {
            action.user_data = (User_data &)(actions[action_index]);
            action.byte_count = 8;
            action.change_events = {};
            std::size_t total_actions = actions.size();
            std::size_t curr_action_start = actions[action_index].first_event_index;
            if ( (curr_action_start & flag_elided_redos) == flag_elided_redos )
            {
                action.status = action_status::elided_redo;
                action.elision_count = curr_action_start & mask_elided_redo_size;
                return;
            }

            std::size_t next_action_start = flag_elided_redos;
            for ( std::size_t i=1; (next_action_start & flag_elided_redos) == flag_elided_redos; ++i ) // Find the next event that isn't an elision marker
                next_action_start = action_index+i<total_actions ? (actions[action_index+i].first_event_index) : mod_root.event_offsets.size();
            
            auto [first_event_start, unused_first_event_end] = mod_root.get_event_offset_range(curr_action_start);
            auto [unused_last_event_start, last_event_end] = mod_root.get_event_offset_range(next_action_start-1);
            action.byte_count += (next_action_start-curr_action_start)*8+(last_event_end-first_event_start);
            action.status = action_index < total_actions-redo_size ? action_status::undoable : action_status::redoable;
            if ( include_events )
            {
                for ( std::size_t event_index = curr_action_start; event_index < next_action_start; ++event_index )
                {
                    for ( std::size_t i=1; (next_action_start & flag_elided_redos) == flag_elided_redos; ++i ) // Find the next event that isn't an elision marker
                        next_action_start = action_index+i<total_actions ? (actions[action_index+i].first_event_index) : mod_root.event_offsets.size();

                    if ( curr_action_start == next_action_start )
                        return;

                    auto [event_offset, byte_end] = mod_root.get_event_offset_range(event_index);
                    data_change_event & event = action.change_events.emplace_back();

                    event.summary += "edit";
                    render_event<Data>(event, event_offset);
                }
            }
        }

        std::vector<rendered_action<User_data>> render_change_history(bool include_events = false) const
        {
            std::vector<rendered_action<User_data>> rendering {};
            std::size_t total_actions = actions.size();
            for ( std::size_t action_index=0; action_index<total_actions; ++action_index )
            {
                rendered_action<User_data> & action = rendering.emplace_back();

                render_action(action_index, action, include_events);
                if ( action.status == action_status::elided_redo && action.elision_count > 0 )
                {
                    std::size_t first = static_cast<std::size_t>(static_cast<std::ptrdiff_t>(action_index)-static_cast<std::ptrdiff_t>(action.elision_count));
                    for ( std::size_t i=first; i<action_index; ++i )
                        rendering[i].status = action_status::elided_redo;
                }
            }
            return rendering;
        }

        REFLECT(tracked, flag_elided_redos, mask_elided_redo_size, mod_root, pending_action_user_data, pending_action_start,
            actions, action_reference_count, redo_count, redo_size, history)
    };

    template <typename T, typename Path_to_elem, typename Editor_type, typename Read_edit_pair>
    class tracked_element : public Read_edit_pair
    {
        using keys_type = typename Path_to_elem::keys;
        using tracked_type = typename Editor_type::tracked_type;

        tracked_element(Editor_type action, keys_type keys)
            : Read_edit_pair{action.edit_from_path(Path_to_elem{keys})}, action(std::move(action)), view(Read_edit_pair::edit) {}

    public:

        Editor_type action;
        const typename Read_edit_pair::edit_type & view;

        tracked_element(tracked_type* tracked, keys_type keys) : tracked_element(tracked->operator()(), keys) {}

        template <class ... Ts> tracked_element(tracked_element<Ts...>* parent_element, keys_type keys)
            : tracked_element(parent_element->action.parent->operator()(), keys) {}
    
        tracked_element(tracked_element && other)
            : Read_edit_pair{std::move((Read_edit_pair &)other)}, action(std::move(other.action)) {}

        constexpr const T* operator->() const { return &(Read_edit_pair::read); }
        constexpr auto operator()() { return &(Read_edit_pair::edit); }
    };

    template <class Data, class User_data = no_user_data>
    requires RareTs::is_macro_reflected_v<Data>
    struct simple_tracked : nf_hist::tracked<Data, simple_tracked<Data, User_data>, User_data>
    {
        simple_tracked(bool track_init = false) : tracked<Data, simple_tracked<Data, User_data>, User_data>(this) {
            if ( track_init )
                tracked<Data, simple_tracked<Data, User_data>, User_data>::record_init();
        }
        simple_tracked(Data && data, bool track_init = false) : tracked<Data, simple_tracked<Data, User_data>, User_data>(this) {
            if ( track_init )
                tracked<Data, simple_tracked<Data, User_data>, User_data>::template init_data<true>(std::forward<Data>(data)); 
            else
                tracked<Data, simple_tracked<Data, User_data>, User_data>::template init_data<false>(std::forward<Data>(data)); 
       }
    };

    template <class Data, class User_data = no_user_data>
    requires RareTs::is_macro_reflected_v<Data>
    auto make_tracked(Data && data) {
        return simple_tracked<Data, User_data>(std::forward<Data>(data));
    }
}

// Nf C++
namespace nf
{
    // Pull commonly used identifiers into the top-level nf namespace
    using nf_hist::index_size;
    using nf_hist::attach_data;

    using nf_hist::editor;
    using nf_hist::tracked;
    using nf_hist::tracked_element;
    using nf_hist::simple_tracked;
    using nf_hist::make_tracked;

    using nf_hist::rendered_action;
    using nf_hist::action_status;

    using nf_hist::make_path;
};
#define NF_PATH(...) nf::make_path<decltype(__VA_ARGS__)>

#endif