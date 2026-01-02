#include <nf/hist.h>
#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <gtest/gtest.h>
#include <cstdint>
#include <unordered_set>
#include <vector>

namespace attach_data_test
{

struct Attachment
{
    std::string rendered;
};

struct Data
{
    NOTE(vec, nf::attach_data<Attachment>)
    std::vector<int> vec {};

    REFLECT(Data, vec)
};

struct Edit_data : nf::tracked<Data, Edit_data>
{
    std::vector<int> integrity_check {};
    const std::vector<Attachment> & attached_data;

    Edit_data() : tracked{this}, attached_data(view.vec.read_attached_data()) {}

    using vec_path = NF_PATH(root->vec);

    void element_added(vec_path, std::size_t index)
    {
        const char c = char(read.vec[index]) + 'a';
        view.vec.attached_data(index).rendered = std::string(&c, 1);
        integrity_check.push_back(0);
    }

    void element_removed(vec_path, std::size_t index)
    {
        integrity_check.erase(std::next(integrity_check.begin(), static_cast<std::ptrdiff_t>(index)));
    }

    bool size_is(std::size_t size)
    {
        return read.vec.size() == size &&
            view.vec.read_attached_data().size() == size &&
            integrity_check.size() == size;
    }
};

TEST(attach_data, reset)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.reset();
    EXPECT_TRUE(obj.size_is(0));

    obj.undo_action();
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(0));
}

TEST(attach_data, assign)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.assign(2, 3);
    EXPECT_TRUE(obj.size_is(2));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "d");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "d");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(2));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "d");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "d");
}

TEST(attach_data, assign_default)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.assign_default(2);
    EXPECT_TRUE(obj.size_is(2));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "a");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(2));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "a");
}

TEST(attach_data, set)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(0));

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");
}

TEST(attach_data, append)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.append(3);
    EXPECT_TRUE(obj.size_is(4));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[3].rendered.c_str(), "d");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(4));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[3].rendered.c_str(), "d");
}

TEST(attach_data, append_n)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.append(std::vector{3, 4});
    EXPECT_TRUE(obj.size_is(5));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[3].rendered.c_str(), "d");
    EXPECT_STREQ(obj.attached_data[4].rendered.c_str(), "e");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(5));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[3].rendered.c_str(), "d");
    EXPECT_STREQ(obj.attached_data[4].rendered.c_str(), "e");
}

TEST(attach_data, insert)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.insert(1, 3);
    EXPECT_TRUE(obj.size_is(4));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "d");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[3].rendered.c_str(), "c");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(4));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "d");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[3].rendered.c_str(), "c");
}

TEST(attach_data, insert_n)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.insert(1, std::vector{3, 4});
    EXPECT_TRUE(obj.size_is(5));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "d");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "e");
    EXPECT_STREQ(obj.attached_data[3].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[4].rendered.c_str(), "c");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(5));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "d");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "e");
    EXPECT_STREQ(obj.attached_data[3].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[4].rendered.c_str(), "c");
}

TEST(attach_data, remove)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.remove(1);
    EXPECT_TRUE(obj.size_is(2));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "c");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(2));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "c");
}

TEST(attach_data, remove_n)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.remove(std::vector<std::size_t>{1, 2});
    EXPECT_TRUE(obj.size_is(1));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(1));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
}

TEST(attach_data, remove_l)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.select(std::vector<std::size_t>{1, 2});
    obj()->vec.remove_selection();
    EXPECT_TRUE(obj.size_is(1));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(1));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
}

TEST(attach_data, sort)
{
    Edit_data obj {};
    obj()->vec = std::vector{2, 0, 1};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "b");

    obj()->vec.sort();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "b");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");
}

TEST(attach_data, sort_desc)
{
    Edit_data obj {};
    obj()->vec = std::vector{1, 0, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.sort_desc();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "a");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "a");
}

TEST(attach_data, swap)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.swap(1, 2);
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "b");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "b");
}

TEST(attach_data, move_up)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.move_up(2);
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "b");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "b");
}

TEST(attach_data, move_up_n)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.move_up(std::vector<std::size_t>{1, 2});
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "a");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "a");
}

TEST(attach_data, move_up_l)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.select(std::vector<std::size_t>{1, 2});
    obj()->vec.move_selections_up();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "a");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "a");
}

TEST(attach_data, move_top)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.move_top(1);
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");
}

TEST(attach_data, move_top_n)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.move_top(std::vector<std::size_t>{1, 2});
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "a");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "a");
}

TEST(attach_data, move_top_l)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.select(std::vector<std::size_t>{1, 2});
    obj()->vec.move_selections_top();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "a");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "a");
}

TEST(attach_data, move_down)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.move_down(1);
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "b");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "b");
}

TEST(attach_data, move_down_n)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.move_down(std::vector<std::size_t>{0, 1});
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "b");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "b");
}

TEST(attach_data, move_down_l)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.select(std::vector<std::size_t>{0, 1});
    obj()->vec.move_selections_down();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "b");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "b");
}

TEST(attach_data, move_bottom)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.move_bottom(1);
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "b");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "b");
}

TEST(attach_data, move_bottom_n)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.move_bottom(std::vector<std::size_t>{0, 1});
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "b");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "b");
}

TEST(attach_data, move_bottom_l)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.select(std::vector<std::size_t>{0, 1});
    obj()->vec.move_selections_bottom();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "b");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "b");
}

TEST(attach_data, move_to)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.move_to(1, 2);
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "b");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "b");
}

TEST(attach_data, move_to_n)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.move_to(std::vector<std::size_t>{1, 2}, 0);
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "a");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "a");
}

TEST(attach_data, move_to_l)
{
    Edit_data obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj()->vec.select(std::vector<std::size_t>{1, 2});
    obj()->vec.move_selections_to(0);
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "a");

    obj.undo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "c");

    obj.redo_action();
    EXPECT_TRUE(obj.size_is(3));
    EXPECT_STREQ(obj.attached_data[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attached_data[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attached_data[2].rendered.c_str(), "a");
}

TEST(attach_data, init)
{
    Edit_data obj {};
    obj.init_data(Data {
        .vec {0, 0, 0, 0}
    });
    EXPECT_EQ(4, obj.view.vec.read_attached_data().size());
}

}