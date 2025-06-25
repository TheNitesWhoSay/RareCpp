#include <rarecpp/editor.h>
#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <gtest/gtest.h>
#include <cstdint>
#include <unordered_set>
#include <vector>

namespace AttachDataTest
{

using namespace RareEdit;

struct Attachment
{
    std::string rendered;
};

struct Data
{
    NOTE(vec, AttachData<Attachment>)
    std::vector<int> vec {};

    REFLECT(Data, vec)
};

struct EditData : Tracked<Data, EditData>
{
    std::vector<int> integrityCheck {};
    const std::vector<Attachment> & attachedData;

    EditData() : Tracked{this}, attachedData(view.vec.readAttachedData()) {}

    using vec_path = PATH(root->vec);

    void elementAdded(vec_path, std::size_t index)
    {
        const char c = char(read.vec[index]) + 'a';
        view.vec.attachedData(index).rendered = std::string(&c, 1);
        integrityCheck.push_back(0);
    }

    void elementRemoved(vec_path, std::size_t index)
    {
        integrityCheck.erase(std::next(integrityCheck.begin(), static_cast<std::ptrdiff_t>(index)));
    }

    bool sizeIs(std::size_t size)
    {
        return read.vec.size() == size &&
            view.vec.readAttachedData().size() == size &&
            integrityCheck.size() == size;
    }
};

TEST(AttachData, Reset)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.reset();
    EXPECT_TRUE(obj.sizeIs(0));

    obj.undoAction();
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(0));
}

TEST(AttachData, Assign)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.assign(2, 3);
    EXPECT_TRUE(obj.sizeIs(2));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "d");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "d");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(2));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "d");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "d");
}

TEST(AttachData, AssignDefault)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.assignDefault(2);
    EXPECT_TRUE(obj.sizeIs(2));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "a");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(2));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "a");
}

TEST(AttachData, Set)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(0));

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");
}

TEST(AttachData, Append)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.append(3);
    EXPECT_TRUE(obj.sizeIs(4));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[3].rendered.c_str(), "d");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(4));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[3].rendered.c_str(), "d");
}

TEST(AttachData, AppendN)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.append(std::vector{3, 4});
    EXPECT_TRUE(obj.sizeIs(5));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[3].rendered.c_str(), "d");
    EXPECT_STREQ(obj.attachedData[4].rendered.c_str(), "e");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(5));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[3].rendered.c_str(), "d");
    EXPECT_STREQ(obj.attachedData[4].rendered.c_str(), "e");
}

TEST(AttachData, Insert)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.insert(1, 3);
    EXPECT_TRUE(obj.sizeIs(4));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "d");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[3].rendered.c_str(), "c");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(4));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "d");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[3].rendered.c_str(), "c");
}

TEST(AttachData, InsertN)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.insert(1, std::vector{3, 4});
    EXPECT_TRUE(obj.sizeIs(5));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "d");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "e");
    EXPECT_STREQ(obj.attachedData[3].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[4].rendered.c_str(), "c");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(5));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "d");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "e");
    EXPECT_STREQ(obj.attachedData[3].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[4].rendered.c_str(), "c");
}

TEST(AttachData, Remove)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.remove(1);
    EXPECT_TRUE(obj.sizeIs(2));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "c");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(2));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "c");
}

TEST(AttachData, RemoveN)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.remove(std::vector<std::size_t>{1, 2});
    EXPECT_TRUE(obj.sizeIs(1));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(1));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
}

TEST(AttachData, RemoveL)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.select(std::vector<std::size_t>{1, 2});
    obj()->vec.removeSelection();
    EXPECT_TRUE(obj.sizeIs(1));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(1));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
}

TEST(AttachData, Sort)
{
    EditData obj {};
    obj()->vec = std::vector{2, 0, 1};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "b");

    obj()->vec.sort();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "b");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");
}

TEST(AttachData, SortDesc)
{
    EditData obj {};
    obj()->vec = std::vector{1, 0, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.sortDesc();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "a");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "a");
}

TEST(AttachData, Swap)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.swap(1, 2);
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "b");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "b");
}

TEST(AttachData, MoveUp)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.moveUp(2);
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "b");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "b");
}

TEST(AttachData, MoveUpN)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.moveUp(std::vector<std::size_t>{1, 2});
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "a");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "a");
}

TEST(AttachData, MoveUpL)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.select(std::vector<std::size_t>{1, 2});
    obj()->vec.moveSelectionsUp();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "a");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "a");
}

TEST(AttachData, MoveTop)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.moveTop(1);
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");
}

TEST(AttachData, MoveTopN)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.moveTop(std::vector<std::size_t>{1, 2});
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "a");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "a");
}

TEST(AttachData, MoveTopL)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.select(std::vector<std::size_t>{1, 2});
    obj()->vec.moveSelectionsTop();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "a");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "a");
}

TEST(AttachData, MoveDown)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.moveDown(1);
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "b");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "b");
}

TEST(AttachData, MoveDownN)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.moveDown(std::vector<std::size_t>{0, 1});
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "b");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "b");
}

TEST(AttachData, MoveDownL)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.select(std::vector<std::size_t>{0, 1});
    obj()->vec.moveSelectionsDown();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "b");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "b");
}

TEST(AttachData, MoveBottom)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.moveBottom(1);
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "b");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "b");
}

TEST(AttachData, MoveBottomN)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.moveBottom(std::vector<std::size_t>{0, 1});
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "b");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "b");
}

TEST(AttachData, MoveBottomL)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.select(std::vector<std::size_t>{0, 1});
    obj()->vec.moveSelectionsBottom();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "b");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "b");
}

TEST(AttachData, MoveTo)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.moveTo(1, 2);
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "b");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "b");
}

TEST(AttachData, MoveToN)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.moveTo(std::vector<std::size_t>{1, 2}, 0);
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "a");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "a");
}

TEST(AttachData, MoveToL)
{
    EditData obj {};
    obj()->vec = std::vector{0, 1, 2};
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj()->vec.select(std::vector<std::size_t>{1, 2});
    obj()->vec.moveSelectionsTo(0);
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "a");

    obj.undoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "a");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "c");

    obj.redoAction();
    EXPECT_TRUE(obj.sizeIs(3));
    EXPECT_STREQ(obj.attachedData[0].rendered.c_str(), "b");
    EXPECT_STREQ(obj.attachedData[1].rendered.c_str(), "c");
    EXPECT_STREQ(obj.attachedData[2].rendered.c_str(), "a");
}

TEST(AttachData, Init)
{
    EditData obj {};
    obj.initData(Data {
        .vec {0, 0, 0, 0}
    });
    EXPECT_EQ(4, obj.view.vec.readAttachedData().size());
}

}