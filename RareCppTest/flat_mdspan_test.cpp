#include <nf/hist.h>
#include <gtest/gtest.h>

namespace flat_mdspan_test
{

TEST(flat_mdspan, array_header)
{
    int one_d[2] {22, 33};
    EXPECT_EQ(nf_hist::array_header(one_d)[0], 22);
    int two_d[3][2] {{33, 44}, {55, 66}, {77, 88}};
    EXPECT_EQ(nf_hist::array_header(two_d)[0], 33);
    int three_d[3][2][1] {{{44}, {55}}, {{66}, {77}}, {{88}, {99}}};
    EXPECT_EQ(nf_hist::array_header(three_d)[0], 44);
    int four_d[2][3][2][1] {{{{55}, {66}}, {{77}, {88}}, {{89}, {90}}}, {{{56}, {67}}, {{78}, {89}}, {{90}, {91}}}};
    EXPECT_EQ(nf_hist::array_header(four_d)[0], 55);
}

TEST(flat_mdspan, one_d)
{
    int one_d[2] {22, 33};
    auto span = nf_hist::as_1d(one_d);
    const auto & const_span = span;
    using flatspan = decltype(span);
    bool is_same = std::is_same_v<flatspan, nf_hist::flat_mdspan<int, 2>>;
    EXPECT_TRUE(is_same);
    is_same = std::is_same_v<flatspan::element_type, int>;
    EXPECT_TRUE(is_same);
    is_same = std::is_same_v<flatspan::extents, std::index_sequence<2>>;
    EXPECT_TRUE(is_same);
    EXPECT_EQ(flatspan::rank, 1);
    EXPECT_EQ(flatspan::size, 2);
    EXPECT_EQ(span[0], 22);
    EXPECT_EQ(span[1], 33);
    EXPECT_EQ(const_span[0], 22);
    EXPECT_EQ(const_span[1], 33);
    for ( int & i : span ) // Non-const iterators
        ++i;
    EXPECT_EQ(one_d[0], 23);
    EXPECT_EQ(one_d[1], 34);
    EXPECT_EQ(span[0], 23);
    EXPECT_EQ(span[1], 34);
    EXPECT_EQ(const_span[0], 23);
    EXPECT_EQ(const_span[1], 34);
    std::size_t index = 0;
    for ( auto & i : const_span ) // Const iterator
    {
        EXPECT_EQ(one_d[index], i);
        ++index;
    }
    EXPECT_EQ(index, 2);
    for ( auto it = span.flat_begin(); it != span.flat_end(); ++it ) // Non-const flat it
        ++(*it);
    EXPECT_EQ(one_d[0], 24);
    EXPECT_EQ(one_d[1], 35);
    EXPECT_EQ(span[0], 24);
    EXPECT_EQ(span[1], 35);
    EXPECT_EQ(const_span[0], 24);
    EXPECT_EQ(const_span[1], 35);
    index = 0;
    for ( auto it = const_span.flat_begin(); it != const_span.flat_end(); ++it ) // Const flat it
    {
        bool is_const = std::is_const_v<std::remove_reference_t<decltype(*it)>>;
        EXPECT_TRUE(is_const);
        EXPECT_EQ(one_d[index], *it);
        ++index;
    }
    EXPECT_EQ(index, 2);

    int other[2] {66, 77};
    span = nf_hist::as_1d(other);
    EXPECT_EQ(one_d[0], 66);
    EXPECT_EQ(one_d[1], 77);

    span.clear();
    EXPECT_EQ(one_d[0], 0);
    EXPECT_EQ(one_d[1], 0);

    span.fill(11);
    EXPECT_EQ(one_d[0], 11);
    EXPECT_EQ(one_d[1], 11);

    one_d[1] = 22;
    span.swap(nf_hist::as_1d(other));
    EXPECT_EQ(one_d[0], 66);
    EXPECT_EQ(one_d[1], 77);
    EXPECT_EQ(other[0], 11);
    EXPECT_EQ(other[1], 22);
    
    EXPECT_TRUE(nf_hist::is_flat_mdspan<flatspan>::value);
    EXPECT_TRUE(nf_hist::is_flat_mdspan<const flatspan>::value);
    EXPECT_TRUE(nf_hist::is_flat_mdspan_v<flatspan>);
    EXPECT_TRUE(nf_hist::is_flat_mdspan_v<const flatspan>);
    EXPECT_FALSE(nf_hist::is_flat_mdspan<decltype(one_d)>::value);
    EXPECT_FALSE(nf_hist::is_flat_mdspan<decltype(one_d)>::value);
}

TEST(flat_mdspan, two_d)
{
    // [y][x], ++x steps a true distance of 1, ++y steps a true distance of width
    int two_d[3][2] {{33, 44}, {55, 66}, {77, 88}};
    auto span = nf_hist::as_1d(two_d);
    const auto & const_span = span;
    using flatspan = decltype(span);
    bool is_same = std::is_same_v<flatspan, nf_hist::flat_mdspan<int, 3, 2>>;
    EXPECT_TRUE(is_same);
    is_same = std::is_same_v<flatspan::element_type, int>;
    EXPECT_TRUE(is_same);
    is_same = std::is_same_v<flatspan::extents, std::index_sequence<3, 2>>;
    EXPECT_TRUE(is_same);
    EXPECT_EQ(flatspan::rank, 2);
    EXPECT_EQ(flatspan::size, 6);
    EXPECT_EQ(span[0][0], 33);
    EXPECT_EQ(span[0][1], 44);
    EXPECT_EQ(span[1][0], 55);
    EXPECT_EQ(span[1][1], 66);
    EXPECT_EQ(span[2][0], 77);
    EXPECT_EQ(span[2][1], 88);
    EXPECT_EQ(const_span[0][0], 33);
    EXPECT_EQ(const_span[0][1], 44);
    EXPECT_EQ(const_span[1][0], 55);
    EXPECT_EQ(const_span[1][1], 66);
    EXPECT_EQ(const_span[2][0], 77);
    EXPECT_EQ(const_span[2][1], 88);
    for ( auto row : span ) // Non-const iterators
    {
        for ( int & i : row )
            ++i;
    }
    EXPECT_EQ(two_d[0][0], 34);
    EXPECT_EQ(two_d[0][1], 45);
    EXPECT_EQ(two_d[1][0], 56);
    EXPECT_EQ(two_d[1][1], 67);
    EXPECT_EQ(two_d[2][0], 78);
    EXPECT_EQ(two_d[2][1], 89);
    EXPECT_EQ(span[0][0], 34);
    EXPECT_EQ(span[0][1], 45);
    EXPECT_EQ(span[1][0], 56);
    EXPECT_EQ(span[1][1], 67);
    EXPECT_EQ(span[2][0], 78);
    EXPECT_EQ(span[2][1], 89);
    EXPECT_EQ(const_span[0][0], 34);
    EXPECT_EQ(const_span[0][1], 45);
    EXPECT_EQ(const_span[1][0], 56);
    EXPECT_EQ(const_span[1][1], 67);
    EXPECT_EQ(const_span[2][0], 78);
    EXPECT_EQ(const_span[2][1], 89);
    std::size_t y = 0;
    for ( auto row : const_span ) // Const iterator
    {
        std::size_t x = 0;
        for ( auto & i : row )
        {
            EXPECT_EQ(two_d[y][x], i);
            ++x;
        }
        EXPECT_EQ(x, 2);
        ++y;
    }
    EXPECT_EQ(y, 3);
    for ( auto it = span.flat_begin(); it != span.flat_end(); ++it ) // Non-const flat it
        ++(*it);
    EXPECT_EQ(two_d[0][0], 35);
    EXPECT_EQ(two_d[0][1], 46);
    EXPECT_EQ(two_d[1][0], 57);
    EXPECT_EQ(two_d[1][1], 68);
    EXPECT_EQ(two_d[2][0], 79);
    EXPECT_EQ(two_d[2][1], 90);
    EXPECT_EQ(span[0][0], 35);
    EXPECT_EQ(span[0][1], 46);
    EXPECT_EQ(span[1][0], 57);
    EXPECT_EQ(span[1][1], 68);
    EXPECT_EQ(span[2][0], 79);
    EXPECT_EQ(span[2][1], 90);
    EXPECT_EQ(const_span[0][0], 35);
    EXPECT_EQ(const_span[0][1], 46);
    EXPECT_EQ(const_span[1][0], 57);
    EXPECT_EQ(const_span[1][1], 68);
    EXPECT_EQ(const_span[2][0], 79);
    EXPECT_EQ(const_span[2][1], 90);
    std::size_t index = 0;
    for ( auto it = const_span.flat_begin(); it != const_span.flat_end(); ++it ) // Const flat it
    {
        bool is_const = std::is_const_v<std::remove_reference_t<decltype(*it)>>;
        EXPECT_TRUE(is_const);
        EXPECT_EQ((&two_d[0][0])[index], *it);
        ++index;
    }
    EXPECT_EQ(index, 6);

    int other[3][2] {{22, 33}, {44, 55}, {66, 77}};
    span = nf_hist::as_1d(other);
    EXPECT_EQ(two_d[0][0], 22);
    EXPECT_EQ(two_d[0][1], 33);
    EXPECT_EQ(two_d[1][0], 44);
    EXPECT_EQ(two_d[1][1], 55);
    EXPECT_EQ(two_d[2][0], 66);
    EXPECT_EQ(two_d[2][1], 77);

    span.clear();
    EXPECT_EQ(two_d[0][0], 0);
    EXPECT_EQ(two_d[0][1], 0);
    EXPECT_EQ(two_d[1][0], 0);
    EXPECT_EQ(two_d[1][1], 0);
    EXPECT_EQ(two_d[2][0], 0);
    EXPECT_EQ(two_d[2][1], 0);

    span.fill(11);
    EXPECT_EQ(two_d[0][0], 11);
    EXPECT_EQ(two_d[0][1], 11);
    EXPECT_EQ(two_d[1][0], 11);
    EXPECT_EQ(two_d[1][1], 11);
    EXPECT_EQ(two_d[2][0], 11);
    EXPECT_EQ(two_d[2][1], 11);

    two_d[0][1] = 22;
    two_d[1][0] = 33;
    two_d[1][1] = 44;
    two_d[2][0] = 55;
    two_d[2][1] = 66; // 11, 22, 33, 44, 55, 66
    span.swap(nf_hist::as_1d(other)); // 22, 33, 44, 55, 66, 77
    EXPECT_EQ(two_d[0][0], 22);
    EXPECT_EQ(two_d[0][1], 33);
    EXPECT_EQ(two_d[1][0], 44);
    EXPECT_EQ(two_d[1][1], 55);
    EXPECT_EQ(two_d[2][0], 66);
    EXPECT_EQ(two_d[2][1], 77);
    EXPECT_EQ(other[0][0], 11);
    EXPECT_EQ(other[0][1], 22);
    EXPECT_EQ(other[1][0], 33);
    EXPECT_EQ(other[1][1], 44);
    EXPECT_EQ(other[2][0], 55);
    EXPECT_EQ(other[2][1], 66);
    
    EXPECT_TRUE(nf_hist::is_flat_mdspan<flatspan>::value);
    EXPECT_TRUE(nf_hist::is_flat_mdspan<const flatspan>::value);
    EXPECT_TRUE(nf_hist::is_flat_mdspan_v<flatspan>);
    EXPECT_TRUE(nf_hist::is_flat_mdspan_v<const flatspan>);
    EXPECT_FALSE(nf_hist::is_flat_mdspan<decltype(two_d)>::value);
    EXPECT_FALSE(nf_hist::is_flat_mdspan<decltype(two_d)>::value);
}


TEST(flat_mdspan, three_d)
{
    // [z][y][x], ++x steps a true distance of 1, ++y steps a true distance of width, ++z steps a true distance of height*width
    int three_d[4][3][2] {
        {{33, 44}, {55, 66}, {77, 88}},
        {{333, 444}, {555, 666}, {777, 888}},
        {{3333, 4444}, {5555, 6666}, {7777, 8888}},
        {{33333, 44444}, {55555, 66666}, {77777, 88888}}
    };
    auto span = nf_hist::as_1d(three_d);
    const auto & const_span = span;
    using flatspan = decltype(span);
    bool is_same = std::is_same_v<flatspan, nf_hist::flat_mdspan<int, 4, 3, 2>>;
    EXPECT_TRUE(is_same);
    is_same = std::is_same_v<flatspan::element_type, int>;
    EXPECT_TRUE(is_same);
    is_same = std::is_same_v<flatspan::extents, std::index_sequence<4, 3, 2>>;
    EXPECT_TRUE(is_same);
    EXPECT_EQ(flatspan::rank, 3);
    EXPECT_EQ(flatspan::size, 24);
    EXPECT_EQ(span[0][0][0], 33);
    EXPECT_EQ(span[0][0][1], 44);
    EXPECT_EQ(span[0][1][0], 55);
    EXPECT_EQ(span[0][1][1], 66);
    EXPECT_EQ(span[0][2][0], 77);
    EXPECT_EQ(span[0][2][1], 88);
    EXPECT_EQ(span[1][0][0], 333);
    EXPECT_EQ(span[1][0][1], 444);
    EXPECT_EQ(span[1][1][0], 555);
    EXPECT_EQ(span[1][1][1], 666);
    EXPECT_EQ(span[1][2][0], 777);
    EXPECT_EQ(span[1][2][1], 888);
    EXPECT_EQ(span[2][0][0], 3333);
    EXPECT_EQ(span[2][0][1], 4444);
    EXPECT_EQ(span[2][1][0], 5555);
    EXPECT_EQ(span[2][1][1], 6666);
    EXPECT_EQ(span[2][2][0], 7777);
    EXPECT_EQ(span[2][2][1], 8888);
    EXPECT_EQ(span[3][0][0], 33333);
    EXPECT_EQ(span[3][0][1], 44444);
    EXPECT_EQ(span[3][1][0], 55555);
    EXPECT_EQ(span[3][1][1], 66666);
    EXPECT_EQ(span[3][2][0], 77777);
    EXPECT_EQ(span[3][2][1], 88888);
    EXPECT_EQ(const_span[0][0][0], 33);
    EXPECT_EQ(const_span[0][0][1], 44);
    EXPECT_EQ(const_span[0][1][0], 55);
    EXPECT_EQ(const_span[0][1][1], 66);
    EXPECT_EQ(const_span[0][2][0], 77);
    EXPECT_EQ(const_span[0][2][1], 88);
    EXPECT_EQ(const_span[1][0][0], 333);
    EXPECT_EQ(const_span[1][0][1], 444);
    EXPECT_EQ(const_span[1][1][0], 555);
    EXPECT_EQ(const_span[1][1][1], 666);
    EXPECT_EQ(const_span[1][2][0], 777);
    EXPECT_EQ(const_span[1][2][1], 888);
    EXPECT_EQ(const_span[2][0][0], 3333);
    EXPECT_EQ(const_span[2][0][1], 4444);
    EXPECT_EQ(const_span[2][1][0], 5555);
    EXPECT_EQ(const_span[2][1][1], 6666);
    EXPECT_EQ(const_span[2][2][0], 7777);
    EXPECT_EQ(const_span[2][2][1], 8888);
    EXPECT_EQ(const_span[3][0][0], 33333);
    EXPECT_EQ(const_span[3][0][1], 44444);
    EXPECT_EQ(const_span[3][1][0], 55555);
    EXPECT_EQ(const_span[3][1][1], 66666);
    EXPECT_EQ(const_span[3][2][0], 77777);
    EXPECT_EQ(const_span[3][2][1], 88888);
    for ( auto slice : span ) // Non-const iterators
    {
        for ( auto row : slice )
        {
            for ( int & i : row )
                ++i;
        }
    }
    EXPECT_EQ(three_d[0][0][0], 34);
    EXPECT_EQ(three_d[0][0][1], 45);
    EXPECT_EQ(three_d[0][1][0], 56);
    EXPECT_EQ(three_d[0][1][1], 67);
    EXPECT_EQ(three_d[0][2][0], 78);
    EXPECT_EQ(three_d[0][2][1], 89);
    EXPECT_EQ(three_d[1][0][0], 334);
    EXPECT_EQ(three_d[1][0][1], 445);
    EXPECT_EQ(three_d[1][1][0], 556);
    EXPECT_EQ(three_d[1][1][1], 667);
    EXPECT_EQ(three_d[1][2][0], 778);
    EXPECT_EQ(three_d[1][2][1], 889);
    EXPECT_EQ(three_d[2][0][0], 3334);
    EXPECT_EQ(three_d[2][0][1], 4445);
    EXPECT_EQ(three_d[2][1][0], 5556);
    EXPECT_EQ(three_d[2][1][1], 6667);
    EXPECT_EQ(three_d[2][2][0], 7778);
    EXPECT_EQ(three_d[2][2][1], 8889);
    EXPECT_EQ(three_d[3][0][0], 33334);
    EXPECT_EQ(three_d[3][0][1], 44445);
    EXPECT_EQ(three_d[3][1][0], 55556);
    EXPECT_EQ(three_d[3][1][1], 66667);
    EXPECT_EQ(three_d[3][2][0], 77778);
    EXPECT_EQ(three_d[3][2][1], 88889);
    std::size_t z = 0;
    for ( auto slice : const_span ) // Const iterator
    {
        std::size_t y = 0;
        for ( auto row : slice )
        {
            std::size_t x = 0;
            for ( auto & i : row )
            {
                EXPECT_EQ(three_d[z][y][x], i);
                ++x;
            }
            EXPECT_EQ(x, 2);
            ++y;
        }
        EXPECT_EQ(y, 3);
        ++z;
    }
    EXPECT_EQ(z, 4);
    for ( auto it = span.flat_begin(); it != span.flat_end(); ++it ) // Non-const flat it
        ++(*it);
    EXPECT_EQ(three_d[0][0][0], 35);
    EXPECT_EQ(three_d[0][0][1], 46);
    EXPECT_EQ(three_d[0][1][0], 57);
    EXPECT_EQ(three_d[0][1][1], 68);
    EXPECT_EQ(three_d[0][2][0], 79);
    EXPECT_EQ(three_d[0][2][1], 90);
    EXPECT_EQ(three_d[1][0][0], 335);
    EXPECT_EQ(three_d[1][0][1], 446);
    EXPECT_EQ(three_d[1][1][0], 557);
    EXPECT_EQ(three_d[1][1][1], 668);
    EXPECT_EQ(three_d[1][2][0], 779);
    EXPECT_EQ(three_d[1][2][1], 890);
    EXPECT_EQ(three_d[2][0][0], 3335);
    EXPECT_EQ(three_d[2][0][1], 4446);
    EXPECT_EQ(three_d[2][1][0], 5557);
    EXPECT_EQ(three_d[2][1][1], 6668);
    EXPECT_EQ(three_d[2][2][0], 7779);
    EXPECT_EQ(three_d[2][2][1], 8890);
    EXPECT_EQ(three_d[3][0][0], 33335);
    EXPECT_EQ(three_d[3][0][1], 44446);
    EXPECT_EQ(three_d[3][1][0], 55557);
    EXPECT_EQ(three_d[3][1][1], 66668);
    EXPECT_EQ(three_d[3][2][0], 77779);
    EXPECT_EQ(three_d[3][2][1], 88890);
    std::size_t index = 0;
    for ( auto it = const_span.flat_begin(); it != const_span.flat_end(); ++it ) // Const flat it
    {
        bool is_const = std::is_const_v<std::remove_reference_t<decltype(*it)>>;
        EXPECT_TRUE(is_const);
        EXPECT_EQ((&three_d[0][0][0])[index], *it);
        ++index;
    }
    EXPECT_EQ(index, 24);

    int other[4][3][2] {
        {{22, 33}, {44, 55}, {66, 77}},
        {{222, 333}, {444, 555}, {666, 777}},
        {{2222, 3333}, {4444, 5555}, {6666, 7777}},
        {{22222, 33333}, {44444, 55555}, {66666, 77777}}
    };
    span = nf_hist::as_1d(other);
    EXPECT_EQ(three_d[0][0][0], 22);
    EXPECT_EQ(three_d[0][0][1], 33);
    EXPECT_EQ(three_d[0][1][0], 44);
    EXPECT_EQ(three_d[0][1][1], 55);
    EXPECT_EQ(three_d[0][2][0], 66);
    EXPECT_EQ(three_d[0][2][1], 77);
    EXPECT_EQ(three_d[1][0][0], 222);
    EXPECT_EQ(three_d[1][0][1], 333);
    EXPECT_EQ(three_d[1][1][0], 444);
    EXPECT_EQ(three_d[1][1][1], 555);
    EXPECT_EQ(three_d[1][2][0], 666);
    EXPECT_EQ(three_d[1][2][1], 777);
    EXPECT_EQ(three_d[2][0][0], 2222);
    EXPECT_EQ(three_d[2][0][1], 3333);
    EXPECT_EQ(three_d[2][1][0], 4444);
    EXPECT_EQ(three_d[2][1][1], 5555);
    EXPECT_EQ(three_d[2][2][0], 6666);
    EXPECT_EQ(three_d[2][2][1], 7777);
    EXPECT_EQ(three_d[3][0][0], 22222);
    EXPECT_EQ(three_d[3][0][1], 33333);
    EXPECT_EQ(three_d[3][1][0], 44444);
    EXPECT_EQ(three_d[3][1][1], 55555);
    EXPECT_EQ(three_d[3][2][0], 66666);
    EXPECT_EQ(three_d[3][2][1], 77777);

    span.clear();
    EXPECT_EQ(three_d[0][0][0], 0);
    EXPECT_EQ(three_d[0][0][1], 0);
    EXPECT_EQ(three_d[0][1][0], 0);
    EXPECT_EQ(three_d[0][1][1], 0);
    EXPECT_EQ(three_d[0][2][0], 0);
    EXPECT_EQ(three_d[0][2][1], 0);
    EXPECT_EQ(three_d[1][0][0], 0);
    EXPECT_EQ(three_d[1][0][1], 0);
    EXPECT_EQ(three_d[1][1][0], 0);
    EXPECT_EQ(three_d[1][1][1], 0);
    EXPECT_EQ(three_d[1][2][0], 0);
    EXPECT_EQ(three_d[1][2][1], 0);
    EXPECT_EQ(three_d[2][0][0], 0);
    EXPECT_EQ(three_d[2][0][1], 0);
    EXPECT_EQ(three_d[2][1][0], 0);
    EXPECT_EQ(three_d[2][1][1], 0);
    EXPECT_EQ(three_d[2][2][0], 0);
    EXPECT_EQ(three_d[2][2][1], 0);
    EXPECT_EQ(three_d[3][0][0], 0);
    EXPECT_EQ(three_d[3][0][1], 0);
    EXPECT_EQ(three_d[3][1][0], 0);
    EXPECT_EQ(three_d[3][1][1], 0);
    EXPECT_EQ(three_d[3][2][0], 0);
    EXPECT_EQ(three_d[3][2][1], 0);

    span.fill(11);
    EXPECT_EQ(three_d[0][0][0], 11);
    EXPECT_EQ(three_d[0][0][1], 11);
    EXPECT_EQ(three_d[0][1][0], 11);
    EXPECT_EQ(three_d[0][1][1], 11);
    EXPECT_EQ(three_d[0][2][0], 11);
    EXPECT_EQ(three_d[0][2][1], 11);
    EXPECT_EQ(three_d[1][0][0], 11);
    EXPECT_EQ(three_d[1][0][1], 11);
    EXPECT_EQ(three_d[1][1][0], 11);
    EXPECT_EQ(three_d[1][1][1], 11);
    EXPECT_EQ(three_d[1][2][0], 11);
    EXPECT_EQ(three_d[1][2][1], 11);
    EXPECT_EQ(three_d[2][0][0], 11);
    EXPECT_EQ(three_d[2][0][1], 11);
    EXPECT_EQ(three_d[2][1][0], 11);
    EXPECT_EQ(three_d[2][1][1], 11);
    EXPECT_EQ(three_d[2][2][0], 11);
    EXPECT_EQ(three_d[2][2][1], 11);
    EXPECT_EQ(three_d[3][0][0], 11);
    EXPECT_EQ(three_d[3][0][1], 11);
    EXPECT_EQ(three_d[3][1][0], 11);
    EXPECT_EQ(three_d[3][1][1], 11);
    EXPECT_EQ(three_d[3][2][0], 11);
    EXPECT_EQ(three_d[3][2][1], 11);

    three_d[0][0][1] = 22;
    three_d[0][1][0] = 33;
    three_d[0][1][1] = 44;
    three_d[0][2][0] = 55;
    three_d[0][2][1] = 66;
    three_d[1][0][0] = 111;
    three_d[1][0][1] = 222;
    three_d[1][1][0] = 333;
    three_d[1][1][1] = 444;
    three_d[1][2][0] = 555;
    three_d[1][2][1] = 666;
    three_d[2][0][0] = 1111;
    three_d[2][0][1] = 2222;
    three_d[2][1][0] = 3333;
    three_d[2][1][1] = 4444;
    three_d[2][2][0] = 5555;
    three_d[2][2][1] = 6666;
    three_d[3][0][0] = 11111;
    three_d[3][0][1] = 22222;
    three_d[3][1][0] = 33333;
    three_d[3][1][1] = 44444;
    three_d[3][2][0] = 55555;
    three_d[3][2][1] = 66666; // {{11, 22, 33, 44, 55, 66}, {111, 222 ...}, {...}, {...}}
    span.swap(nf_hist::as_1d(other)); // {{22, 33, 44, 55, 66, 77}, {222, 333 ...}, {...}, {...}}
    EXPECT_EQ(three_d[0][0][0], 22);
    EXPECT_EQ(three_d[0][0][1], 33);
    EXPECT_EQ(three_d[0][1][0], 44);
    EXPECT_EQ(three_d[0][1][1], 55);
    EXPECT_EQ(three_d[0][2][0], 66);
    EXPECT_EQ(three_d[0][2][1], 77);
    EXPECT_EQ(three_d[1][0][0], 222);
    EXPECT_EQ(three_d[1][0][1], 333);
    EXPECT_EQ(three_d[1][1][0], 444);
    EXPECT_EQ(three_d[1][1][1], 555);
    EXPECT_EQ(three_d[1][2][0], 666);
    EXPECT_EQ(three_d[1][2][1], 777);
    EXPECT_EQ(three_d[2][0][0], 2222);
    EXPECT_EQ(three_d[2][0][1], 3333);
    EXPECT_EQ(three_d[2][1][0], 4444);
    EXPECT_EQ(three_d[2][1][1], 5555);
    EXPECT_EQ(three_d[2][2][0], 6666);
    EXPECT_EQ(three_d[2][2][1], 7777);
    EXPECT_EQ(three_d[3][0][0], 22222);
    EXPECT_EQ(three_d[3][0][1], 33333);
    EXPECT_EQ(three_d[3][1][0], 44444);
    EXPECT_EQ(three_d[3][1][1], 55555);
    EXPECT_EQ(three_d[3][2][0], 66666);
    EXPECT_EQ(three_d[3][2][1], 77777);
    
    EXPECT_EQ(other[0][0][0], 11);
    EXPECT_EQ(other[0][0][1], 22);
    EXPECT_EQ(other[0][1][0], 33);
    EXPECT_EQ(other[0][1][1], 44);
    EXPECT_EQ(other[0][2][0], 55);
    EXPECT_EQ(other[0][2][1], 66);
    EXPECT_EQ(other[1][0][0], 111);
    EXPECT_EQ(other[1][0][1], 222);
    EXPECT_EQ(other[1][1][0], 333);
    EXPECT_EQ(other[1][1][1], 444);
    EXPECT_EQ(other[1][2][0], 555);
    EXPECT_EQ(other[1][2][1], 666);
    EXPECT_EQ(other[2][0][0], 1111);
    EXPECT_EQ(other[2][0][1], 2222);
    EXPECT_EQ(other[2][1][0], 3333);
    EXPECT_EQ(other[2][1][1], 4444);
    EXPECT_EQ(other[2][2][0], 5555);
    EXPECT_EQ(other[2][2][1], 6666);
    EXPECT_EQ(other[3][0][0], 11111);
    EXPECT_EQ(other[3][0][1], 22222);
    EXPECT_EQ(other[3][1][0], 33333);
    EXPECT_EQ(other[3][1][1], 44444);
    EXPECT_EQ(other[3][2][0], 55555);
    EXPECT_EQ(other[3][2][1], 66666);
    
    EXPECT_TRUE(nf_hist::is_flat_mdspan<flatspan>::value);
    EXPECT_TRUE(nf_hist::is_flat_mdspan<const flatspan>::value);
    EXPECT_TRUE(nf_hist::is_flat_mdspan_v<flatspan>);
    EXPECT_TRUE(nf_hist::is_flat_mdspan_v<const flatspan>);
    EXPECT_FALSE(nf_hist::is_flat_mdspan<decltype(three_d)>::value);
    EXPECT_FALSE(nf_hist::is_flat_mdspan<decltype(three_d)>::value);
}

}
