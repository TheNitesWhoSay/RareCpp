#include <rarecpp/editor.h>
#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <gtest/gtest.h>
#include <cstdint>
#include <vector>

namespace NtfyTest
{

using namespace RareEdit;

inline constexpr std::size_t noIndex = std::numeric_limits<std::size_t>::max();
inline constexpr int noValue = std::numeric_limits<int>::min();

enum class ChangeType {
    ValueChanged,
    ElementAdded,
    ElementRemoved,
    ElementMoved,
    SelectionsChanged
};

struct Change {
    ChangeType type;
    std::size_t fieldIndex = noIndex;
    int oldValue = noValue;
    int value = noValue;
    std::size_t oldIndex = noIndex;
    std::size_t index = noIndex;

    auto operator==(const Change & other) const {
        return
            this->type == other.type &&
            this->fieldIndex == other.fieldIndex &&
            this->oldValue == other.oldValue &&
            this->value == other.value &&
            this->oldIndex == other.oldIndex &&
            this->index == other.index;
    }

    inline friend std::ostream & operator<<(std::ostream & os, const Change & change)
    {
        //os << Json::out(change);
        switch ( change.type ) {
            case ChangeType::ValueChanged: os << "Ntfy::ValueChanged" << " from " << change.oldValue << " to " << change.value << "\n"; break;
            case ChangeType::ElementAdded: os << "Ntfy::ElementAdded" << " at index " << change.index << "\n"; break;
            case ChangeType::ElementRemoved: os << "Ntfy::ElementRemoved" << " from index " << change.index << "\n"; break;
            case ChangeType::ElementMoved: os << "Ntfy::ElementMoved" << " from index " << change.oldIndex << " to " << change.index << "\n"; break;
            case ChangeType::SelectionsChanged: os << "Ntfy::SelectionsChanged" << "\n"; break;
        }
        return os;
    }
};

struct NtfyData
{
    int primitive;
    std::vector<int> vec;
    std::vector<std::vector<int>> vecVec;

    REFLECT(NtfyData, primitive, vec, vecVec)
};

static constexpr std::size_t primitiveFieldIndex = RareTs::MemberType<NtfyData>::primitive::index;
static constexpr std::size_t vecFieldIndex = RareTs::MemberType<NtfyData>::vec::index;
static constexpr std::size_t vecVecFieldIndex = RareTs::MemberType<NtfyData>::vecVec::index;

struct Obj : Tracked<NtfyData, Obj>
{
    Obj() : Tracked{this} {}
    
    using primitive_path = PATH(root->primitive);
    using vec_path = PATH(root->vec);
    using vec_elem_path = PATH(root->vec[0]);
    using vec_vec_path = PATH(root->vecVec);
    using vec_vec_elem_path = PATH(root->vecVec[0]);

    std::vector<Change> changes {};

    void pushChange(auto && change) {
        changes.push_back(std::move(change));
        //std::cout << changes.back() << '\n';
    }

    void valueChanged(primitive_path, int oldValue, int value)
    {
        pushChange(Change{ .type = ChangeType::ValueChanged, .fieldIndex = primitiveFieldIndex, .oldValue = oldValue, .value = value });
    }
    void elementAdded(primitive_path, std::size_t index)
    {
        pushChange(Change{ .type = ChangeType::ElementAdded, .fieldIndex = primitiveFieldIndex, .index = index });
    }
    void elementRemoved(primitive_path, std::size_t index)
    {
        pushChange(Change{ .type = ChangeType::ElementRemoved, .fieldIndex = primitiveFieldIndex, .index = index });
    }
    void elementMoved(primitive_path, std::size_t oldIndex, std::size_t index)
    {
        pushChange(Change{ .type = ChangeType::ElementMoved, .fieldIndex = primitiveFieldIndex, .oldIndex = oldIndex, .index = index });
    }
    void selectionsChanged(primitive_path)
    {
        pushChange(Change{ .type = ChangeType::SelectionsChanged, .fieldIndex = primitiveFieldIndex });
    }
    
    void valueChanged(vec_path, int oldValue, int value)
    {
        pushChange(Change{ .type = ChangeType::ValueChanged, .fieldIndex = vecFieldIndex, .oldValue = oldValue, .value = value });
    }
    void valueChanged(vec_elem_path path, int oldValue, int value)
    {
        pushChange(Change{ .type = ChangeType::ValueChanged, .fieldIndex = vecFieldIndex, .oldValue = oldValue, .value = value, .index = path.index<0>() });
    }
    void elementAdded(vec_path, std::size_t index)
    {
        pushChange(Change{ .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = index });
    }
    void elementRemoved(vec_path, std::size_t index)
    {
        pushChange(Change{ .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = index });
    }
    void elementMoved(vec_path, std::size_t oldIndex, std::size_t index)
    {
        pushChange(Change{ .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = oldIndex, .index = index });
    }
    void selectionsChanged(vec_path)
    {
        pushChange(Change{ .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex });
    }

    void valueChanged(vec_vec_path, int oldValue, int value)
    {
        pushChange(Change{ .type = ChangeType::ValueChanged, .fieldIndex = vecVecFieldIndex, .oldValue = oldValue, .value = value });
    }
    void elementAdded(vec_vec_path, std::size_t index)
    {
        pushChange(Change{ .type = ChangeType::ElementAdded, .fieldIndex = vecVecFieldIndex, .index = index });
    }
    void elementAdded(vec_vec_elem_path path, std::size_t index)
    {
        pushChange(Change{ .type = ChangeType::ElementAdded, .fieldIndex = vecVecFieldIndex, .oldIndex = path.index<0>(), .index = index });
    }
    void elementRemoved(vec_vec_path, std::size_t index)
    {
        pushChange(Change{ .type = ChangeType::ElementRemoved, .fieldIndex = vecVecFieldIndex, .index = index });
    }
    void elementRemoved(vec_vec_elem_path path, std::size_t index)
    {
        pushChange(Change{ .type = ChangeType::ElementRemoved, .fieldIndex = vecVecFieldIndex, .oldIndex = path.index<0>(), .index = index });
    }
    void elementMoved(vec_vec_path, std::size_t oldIndex, std::size_t index)
    {
        pushChange(Change{ .type = ChangeType::ElementMoved, .fieldIndex = vecVecFieldIndex, .oldIndex = oldIndex, .index = index });
    }
    void selectionsChanged(vec_vec_path)
    {
        pushChange(Change{ .type = ChangeType::SelectionsChanged, .fieldIndex = vecVecFieldIndex });
    }

};

TEST(OpNtfy, Reset)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6});
    obj()->vec.select(0);

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 2 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 1 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 0 },
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 0 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 1 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 2 },
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };

    obj.changes.clear();
    obj()->vec.reset();
    EXPECT_EQ(doExpected, obj.changes);
    
    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);
    
    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, Assign)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6});
    obj()->vec.select(0);
    
    std::vector<Change> doExpected {
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 2 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 1 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 0 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 0 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 1 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 2 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 3 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 4 },
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 4 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 3 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 2 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 1 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 0 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 0 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 1 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 2 },
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };

    obj.changes.clear();
    obj()->vec.assign(5, 333);
    EXPECT_EQ(doExpected, obj.changes);
    
    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);
    
    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, AssignDefault)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6});
    obj()->vec.select(0);
    
    std::vector<Change> doExpected {
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 2 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 1 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 0 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 0 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 1 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 2 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 3 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 4 },
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 4 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 3 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 2 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 1 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 0 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 0 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 1 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 2 },
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };

    obj.changes.clear();
    obj()->vec.assignDefault(5);
    EXPECT_EQ(doExpected, obj.changes);
    
    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);
    
    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, ClearSeletions)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6, 7, 8});
    obj()->vec.select(std::vector<std::size_t>{0, 1, 2});

    std::vector<Change> expected { {.type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };

    obj.changes.clear();
    obj()->vec.clearSelections();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(expected, obj.changes);
}

TEST(OpNtfy, SelectAll)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6, 7, 8});

    std::vector<Change> expected { {.type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };

    obj.changes.clear();
    obj()->vec.selectAll();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(expected, obj.changes);
}

TEST(OpNtfy, Select)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6, 7, 8});
    obj()->vec.select(std::vector<std::size_t>{0, 1, 2});

    std::vector<Change> expected { {.type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };

    obj.changes.clear();
    obj()->vec.select(3);
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(expected, obj.changes);
}

TEST(OpNtfy, SelectN)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6, 7, 8});

    std::vector<Change> expected { {.type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };

    obj.changes.clear();
    obj()->vec.select(std::vector<std::size_t>{0, 1, 2});
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(expected, obj.changes);
}

TEST(OpNtfy, Deselect)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6, 7, 8});
    obj()->vec.select(std::vector<std::size_t>{0, 1, 2});

    std::vector<Change> expected { {.type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };

    obj.changes.clear();
    obj()->vec.deselect(1);
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(expected, obj.changes);
}

TEST(OpNtfy, DeselectN)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6, 7, 8});
    obj()->vec.select(std::vector<std::size_t>{0, 1, 2});

    std::vector<Change> expected { {.type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };

    obj.changes.clear();
    obj()->vec.deselect(std::vector<std::size_t>{1, 2});
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(expected, obj.changes);
}

TEST(OpNtfy, ToggleSelection)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6, 7, 8});
    obj()->vec.select(std::vector<std::size_t>{0, 1, 2});

    std::vector<Change> expected { {.type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };

    obj.changes.clear();
    obj()->vec.toggleSelected(3);
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(expected, obj.changes);
}

TEST(OpNtfy, ToggleSelectionN)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6, 7, 8});
    obj()->vec.select(std::vector<std::size_t>{0, 1, 2});

    std::vector<Change> expected { {.type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };

    obj.changes.clear();
    obj()->vec.toggleSelected(std::vector<std::size_t>{2, 3});
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(expected, obj.changes);
}

TEST(OpNtfy, SortSelections)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6, 7, 8});
    obj()->vec.select(std::vector<std::size_t>{0, 1, 2});

    std::vector<Change> expected { {.type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };

    obj.changes.clear();
    obj()->vec.sortSelection();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(expected, obj.changes);
}

TEST(OpNtfy, SortSelectionsDesc)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6, 7, 8});
    obj()->vec.select(std::vector<std::size_t>{0, 1, 2});

    std::vector<Change> expected { {.type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };

    obj.changes.clear();
    obj()->vec.sortSelectionDescending();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(expected, obj.changes);
}

TEST(OpNtfy, Set)
{
    Obj obj {};
    obj()->primitive = 22;
    
    std::vector<Change> doExpected { {.type = ChangeType::ValueChanged, .fieldIndex = primitiveFieldIndex, .oldValue = 22, .value = 33} };
    std::vector<Change> undoExpected { {.type = ChangeType::ValueChanged, .fieldIndex = primitiveFieldIndex, .oldValue = 33, .value = 22} };

    obj.changes.clear();
    obj()->primitive = 33;
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);


    obj()->vec = std::vector{3, 4, 5};
    obj()->vec.select(0);

    doExpected = {
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 2 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 1 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 0 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 0 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 1 },
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    undoExpected = {
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 1 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 0 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 0 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 1 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 2 },
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };

    obj.changes.clear();
    obj()->vec = std::vector{7, 8};
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, SetN)
{
    Obj obj {};
    obj()->vec = std::vector{4, 5, 6, 7, 8};

    std::vector<Change> doExpected {
        {.type = ChangeType::ValueChanged, .fieldIndex = vecFieldIndex, .oldValue = 4, .value = 3, .index = 0},
        {.type = ChangeType::ValueChanged, .fieldIndex = vecFieldIndex, .oldValue = 6, .value = 3, .index = 2},
        {.type = ChangeType::ValueChanged, .fieldIndex = vecFieldIndex, .oldValue = 8, .value = 3, .index = 4}
    };
    std::vector<Change> undoExpected {
        {.type = ChangeType::ValueChanged, .fieldIndex = vecFieldIndex, .oldValue = 3, .value = 4, .index = 0},
        {.type = ChangeType::ValueChanged, .fieldIndex = vecFieldIndex, .oldValue = 3, .value = 6, .index = 2},
        {.type = ChangeType::ValueChanged, .fieldIndex = vecFieldIndex, .oldValue = 3, .value = 8, .index = 4}
    };

    obj.changes.clear();
    obj()->vec.set(std::vector<std::size_t>{0, 2, 4}, 3);
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
    
    obj()->vecVec = std::vector<std::vector<int>> {{1, 2}, {3, 4, 5}, {6, 7, 8, 9}};

    doExpected = {
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecVecFieldIndex, .oldIndex = 0, .index = 1 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecVecFieldIndex, .oldIndex = 0, .index = 0 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecVecFieldIndex, .oldIndex = 0, .index = 0 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecVecFieldIndex, .oldIndex = 0, .index = 1 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecVecFieldIndex, .oldIndex = 1, .index = 2 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecVecFieldIndex, .oldIndex = 1, .index = 1 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecVecFieldIndex, .oldIndex = 1, .index = 0 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecVecFieldIndex, .oldIndex = 1, .index = 0 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecVecFieldIndex, .oldIndex = 1, .index = 1 }
    };
    undoExpected = {
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecVecFieldIndex, .oldIndex = 0, .index = 1 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecVecFieldIndex, .oldIndex = 0, .index = 0 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecVecFieldIndex, .oldIndex = 0, .index = 0 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecVecFieldIndex, .oldIndex = 0, .index = 1 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecVecFieldIndex, .oldIndex = 1, .index = 1 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecVecFieldIndex, .oldIndex = 1, .index = 0 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecVecFieldIndex, .oldIndex = 1, .index = 0 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecVecFieldIndex, .oldIndex = 1, .index = 1 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecVecFieldIndex, .oldIndex = 1, .index = 2 }
    };

    obj.changes.clear();
    obj()->vecVec.set(std::vector<std::size_t>{0, 1}, std::vector{22, 33});
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, SetL)
{
    Obj obj {};
    obj()->vec = std::vector{4, 5, 6, 7, 8};
    obj()->vec.select(std::vector<std::size_t>{0, 2, 4});

    std::vector<Change> doExpected {
        {.type = ChangeType::ValueChanged, .fieldIndex = vecFieldIndex, .oldValue = 4, .value = 3, .index = 0},
        {.type = ChangeType::ValueChanged, .fieldIndex = vecFieldIndex, .oldValue = 6, .value = 3, .index = 2},
        {.type = ChangeType::ValueChanged, .fieldIndex = vecFieldIndex, .oldValue = 8, .value = 3, .index = 4}
    };
    std::vector<Change> undoExpected {
        {.type = ChangeType::ValueChanged, .fieldIndex = vecFieldIndex, .oldValue = 3, .value = 4, .index = 0},
        {.type = ChangeType::ValueChanged, .fieldIndex = vecFieldIndex, .oldValue = 3, .value = 6, .index = 2},
        {.type = ChangeType::ValueChanged, .fieldIndex = vecFieldIndex, .oldValue = 3, .value = 8, .index = 4}
    };

    obj.changes.clear();
    obj()->vec.selection() = 3;
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, PlusEq)
{
    Obj obj {};
    obj()->primitive = 2;
    EXPECT_EQ(obj->primitive, 2);
    
    std::vector<Change> doExpected { {.type = ChangeType::ValueChanged, .fieldIndex = primitiveFieldIndex, .oldValue = 2, .value = 5} };
    std::vector<Change> undoExpected { {.type = ChangeType::ValueChanged, .fieldIndex = primitiveFieldIndex, .oldValue = 5, .value = 2} };
    
    obj.changes.clear();
    obj()->primitive += 3;
    EXPECT_EQ(obj->primitive, 5);
    EXPECT_EQ(doExpected, obj.changes);
    
    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(obj->primitive, 2);
    EXPECT_EQ(undoExpected, obj.changes);
    
    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(obj->primitive, 5);
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, MinusEq)
{
    Obj obj {};
    obj()->primitive = 7;
    EXPECT_EQ(obj->primitive, 7);
    
    std::vector<Change> doExpected { {.type = ChangeType::ValueChanged, .fieldIndex = primitiveFieldIndex, .oldValue = 7, .value = 5} };
    std::vector<Change> undoExpected { {.type = ChangeType::ValueChanged, .fieldIndex = primitiveFieldIndex, .oldValue = 5, .value = 7} };
    
    obj.changes.clear();
    obj()->primitive -= 2;
    EXPECT_EQ(obj->primitive, 5);
    EXPECT_EQ(doExpected, obj.changes);
    
    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(obj->primitive, 7);
    EXPECT_EQ(undoExpected, obj.changes);
    
    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(obj->primitive, 5);
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, MultEq)
{
    Obj obj {};
    obj()->primitive = 2;
    EXPECT_EQ(obj->primitive, 2);
    
    std::vector<Change> doExpected { {.type = ChangeType::ValueChanged, .fieldIndex = primitiveFieldIndex, .oldValue = 2, .value = 4} };
    std::vector<Change> undoExpected { {.type = ChangeType::ValueChanged, .fieldIndex = primitiveFieldIndex, .oldValue = 4, .value = 2} };
    
    obj.changes.clear();
    obj()->primitive *= 2;
    EXPECT_EQ(obj->primitive, 4);
    EXPECT_EQ(doExpected, obj.changes);
    
    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(obj->primitive, 2);
    EXPECT_EQ(undoExpected, obj.changes);
    
    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(obj->primitive, 4);
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, DivEq)
{
    Obj obj {};
    obj()->primitive = 4;
    EXPECT_EQ(obj->primitive, 4);
    
    std::vector<Change> doExpected { {.type = ChangeType::ValueChanged, .fieldIndex = primitiveFieldIndex, .oldValue = 4, .value = 2} };
    std::vector<Change> undoExpected { {.type = ChangeType::ValueChanged, .fieldIndex = primitiveFieldIndex, .oldValue = 2, .value = 4} };
    
    obj.changes.clear();
    obj()->primitive /= 2;
    EXPECT_EQ(obj->primitive, 2);
    EXPECT_EQ(doExpected, obj.changes);
    
    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(obj->primitive, 4);
    EXPECT_EQ(undoExpected, obj.changes);
    
    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(obj->primitive, 2);
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, ModEq)
{
    Obj obj {};
    obj()->primitive = 5;
    EXPECT_EQ(obj->primitive, 5);
    
    std::vector<Change> doExpected { {.type = ChangeType::ValueChanged, .fieldIndex = primitiveFieldIndex, .oldValue = 5, .value = 1} };
    std::vector<Change> undoExpected { {.type = ChangeType::ValueChanged, .fieldIndex = primitiveFieldIndex, .oldValue = 1, .value = 5} };
    
    obj.changes.clear();
    obj()->primitive %= 2;
    EXPECT_EQ(obj->primitive, 1);
    EXPECT_EQ(doExpected, obj.changes);
    
    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(obj->primitive, 5);
    EXPECT_EQ(undoExpected, obj.changes);
    
    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(obj->primitive, 1);
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, XorEq)
{
    Obj obj {};
    obj()->primitive = 0b01;
    EXPECT_EQ(obj->primitive, 0b01);
    
    std::vector<Change> doExpected { {.type = ChangeType::ValueChanged, .fieldIndex = primitiveFieldIndex, .oldValue = 0b01, .value = 0b10} };
    std::vector<Change> undoExpected { {.type = ChangeType::ValueChanged, .fieldIndex = primitiveFieldIndex, .oldValue = 0b10, .value = 0b01} };
    
    obj.changes.clear();
    obj()->primitive ^= 0b11;
    EXPECT_EQ(obj->primitive, 0b10);
    EXPECT_EQ(doExpected, obj.changes);
    
    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(obj->primitive, 0b01);
    EXPECT_EQ(undoExpected, obj.changes);
    
    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(obj->primitive, 0b10);
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, AndEq)
{
    Obj obj {};
    obj()->primitive = 0b101;
    EXPECT_EQ(obj->primitive, 0b101);
    
    std::vector<Change> doExpected { {.type = ChangeType::ValueChanged, .fieldIndex = primitiveFieldIndex, .oldValue = 0b101, .value = 0b001} };
    std::vector<Change> undoExpected { {.type = ChangeType::ValueChanged, .fieldIndex = primitiveFieldIndex, .oldValue = 0b001, .value = 0b101} };
    
    obj.changes.clear();
    obj()->primitive &= 0b011;
    EXPECT_EQ(obj->primitive, 0b001);
    EXPECT_EQ(doExpected, obj.changes);
    
    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(obj->primitive, 0b101);
    EXPECT_EQ(undoExpected, obj.changes);
    
    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(obj->primitive, 0b001);
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, OrEq)
{
    Obj obj {};
    obj()->primitive = 0b00;
    EXPECT_EQ(obj->primitive, 0b00);
    
    std::vector<Change> doExpected { {.type = ChangeType::ValueChanged, .fieldIndex = primitiveFieldIndex, .oldValue = 0b00, .value = 0b11} };
    std::vector<Change> undoExpected { {.type = ChangeType::ValueChanged, .fieldIndex = primitiveFieldIndex, .oldValue = 0b11, .value = 0b00} };
    
    obj.changes.clear();
    obj()->primitive |= 0b11;
    EXPECT_EQ(obj->primitive, 0b11);
    EXPECT_EQ(doExpected, obj.changes);
    
    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(obj->primitive, 0b00);
    EXPECT_EQ(undoExpected, obj.changes);
    
    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(obj->primitive, 0b11);
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, Append)
{
    Obj obj {};
    obj()->vec = std::vector{2, 3};

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 2 },
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 2 }
    };

    obj.changes.clear();
    obj()->vec.append(4);
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, AppendN)
{
    Obj obj {};
    obj()->vec = std::vector{2, 3};

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 2 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 3 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 4 }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 4 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 3 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 2 }
    };

    obj.changes.clear();
    obj()->vec.append(std::vector{4, 5, 6});
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, Insert)
{
    Obj obj {};
    obj()->vec = std::vector{2, 3, 4};
    obj()->vec.select(1);

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 3},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 2},
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 1 },
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 1 },
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 1},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 2},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };

    obj.changes.clear();
    obj()->vec.insert(1, 5);
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, InsertN)
{
    Obj obj {};
    obj()->vec = std::vector{2, 3, 4};
    obj()->vec.select(1);

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 5},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 4},
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 1 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 2 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 3 },
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 3 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 2 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 1 },
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 1},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 2},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };

    obj.changes.clear();
    obj()->vec.insert(1, std::vector{5, 6, 7});
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, Remove)
{
    Obj obj {};
    obj()->vec = std::vector{2, 3, 4, 5};
    obj()->vec.select(2);

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 1 },
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 1},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 2},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 3},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 2},
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 1 },
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };

    obj.changes.clear();
    obj()->vec.remove(1);
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, RemoveN)
{
    Obj obj {};
    obj()->vec = std::vector{2, 3, 4, 5, 6};
    obj()->vec.select(2);

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 2 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 1 },
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 1},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 2},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 4},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 3},
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 1 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 2 },
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };

    obj.changes.clear();
    obj()->vec.remove(std::vector<std::size_t>{1, 2});
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, RemoveL)
{
    Obj obj {};
    obj()->vec = std::vector{2, 3, 4, 5, 6};
    obj()->vec.select(std::vector<std::size_t>{1, 2});

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 2 },
        { .type = ChangeType::ElementRemoved, .fieldIndex = vecFieldIndex, .index = 1 },
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 1},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 2},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 4},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 3},
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 1 },
        { .type = ChangeType::ElementAdded, .fieldIndex = vecFieldIndex, .index = 2 },
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };

    obj.changes.clear();
    obj()->vec.removeSelection();
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, Sort)
{
    Obj obj {};
    obj()->vec = std::vector{1, 9, 2, 8, 3, 7, 11, 5, 6, 4, 22}; // {1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 22};
    obj()->vec.select(2);

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 1},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 2},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 9, .index = 3},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 7, .index = 4},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 8, .index = 5},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 6},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 7},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 8},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 6, .index = 9},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 9, .index = 6},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 8, .index = 1},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 7, .index = 3},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 6, .index = 5},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 8},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 7},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 9},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 4},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 2},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };

    obj.changes.clear();
    obj()->vec.sort();
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, SortDesc)
{
    Obj obj {};
    obj()->vec = std::vector{1, 9, 2, 8, 3, 7, 11, 5, 6, 4, 22}; // {22, 11, 9, 8, 7, 6, 5, 4, 3, 2, 1}
    obj()->vec.select(2);

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 10, .index = 0},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 6, .index = 1},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 2},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 4},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 8, .index = 5},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 7, .index = 6},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 9, .index = 7},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 8},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 9},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 0, .index = 10},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 10, .index = 0},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 9, .index = 2},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 8, .index = 4},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 7, .index = 9},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 6, .index = 7},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 8},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 5},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 1},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 6},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 0, .index = 10},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };

    obj.changes.clear();
    obj()->vec.sortDesc();
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, Swap)
{
    Obj obj {};
    obj()->vec = std::vector{4, 5, 6};
    obj()->vec.select(0);

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 0, .index = 2 },
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 0 },
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 0 },
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 0, .index = 2 },
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };

    obj.changes.clear();
    obj()->vec.swap(0, 2);
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, MoveUp)
{
    Obj obj {};
    obj()->vec = std::vector{4, 5, 6};
    obj()->vec.select(0);

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 0},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 0, .index = 1},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 0, .index = 1},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 0},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };

    obj.changes.clear();
    obj()->vec.moveUp(1);
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);

    obj()->vec = std::vector{4, 5, 6};
    obj()->vec.select(0);

    doExpected = {};
    undoExpected = {};

    obj.changes.clear();
    obj()->vec.moveUp(0);
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, MoveUpN)
{
    Obj obj {};
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88}; // 0, 11, 33, 44, 22, 66, 55, 77, 88
    obj()->vec.select(2);

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 2},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 4},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 3},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 6},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 6, .index = 5},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 3},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 4},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 2},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 6},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 6, .index = 5},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    
    obj.changes.clear();
    obj()->vec.moveUp(std::vector<std::size_t>{3, 4, 6});
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
    
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
    obj()->vec.select(0);

    doExpected = { { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };
    undoExpected = { { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };
    
    obj.changes.clear();
    obj()->vec.moveUp(std::vector<std::size_t>{0, 1, 2});
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, MoveUpL)
{
    Obj obj {};
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88}; // 0, 11, 33, 44, 22, 66, 55, 77, 88
    obj()->vec.select(std::vector<std::size_t>{3, 4, 6});

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 2},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 4},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 3},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 6},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 6, .index = 5},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 3},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 4},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 2},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 6},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 6, .index = 5},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    
    obj.changes.clear();
    obj()->vec.moveSelectionsUp();
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
    
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
    obj()->vec.select(std::vector<std::size_t>{0, 1, 2});

    doExpected = { { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };
    undoExpected = { { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };
    
    obj.changes.clear();
    obj()->vec.moveSelectionsUp();
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, MoveTop)
{
    Obj obj {};
    obj()->vec = std::vector{4, 5, 6, 7};
    obj()->vec.select(1);

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 0},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 0, .index = 1},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 2},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 0, .index = 2},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 0},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 1},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };

    obj.changes.clear();
    obj()->vec.moveTop(2);
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
    
    obj()->vec = std::vector{4, 5, 6, 7};
    obj()->vec.select(0);

    doExpected = {};
    undoExpected = {};

    obj.changes.clear();
    obj()->vec.moveTop(0);
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, MoveTopN)
{
    Obj obj {};
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88}; // 33, 44, 66, 0, 11, 22, 55, 77, 88
    obj()->vec.select(2);

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 0},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 1},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 6, .index = 2},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 0, .index = 3},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 4},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 5},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 6},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 0, .index = 3},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 4},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 5},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 0},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 1},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 6},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 6, .index = 2},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    
    obj.changes.clear();
    obj()->vec.moveTop(std::vector<std::size_t>{3, 4, 6});
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
    
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
    obj()->vec.select(0);

    doExpected = { { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };
    undoExpected = { { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };
    
    obj.changes.clear();
    obj()->vec.moveTop(std::vector<std::size_t>{0, 1, 2});
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, MoveTopL)
{
    Obj obj {};
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88}; // 33, 44, 66, 0, 11, 22, 55, 77, 88
    obj()->vec.select(std::vector<std::size_t>{3, 4, 6});

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 0},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 1},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 6, .index = 2},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 0, .index = 3},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 4},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 5},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 6},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 0, .index = 3},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 4},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 5},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 0},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 1},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 6},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 6, .index = 2},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    
    obj.changes.clear();
    obj()->vec.moveSelectionsTop();
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
    
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
    obj()->vec.select(std::vector<std::size_t>{0, 1, 2});

    doExpected = { { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };
    undoExpected = { { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };
    
    obj.changes.clear();
    obj()->vec.moveSelectionsTop();
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, MoveDown)
{
    Obj obj {};
    obj()->vec = std::vector{4, 5, 6};
    obj()->vec.select(0);

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 2},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 1},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 1},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 2},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };

    obj.changes.clear();
    obj()->vec.moveDown(1);
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);

    obj()->vec = std::vector{4, 5, 6};
    obj()->vec.select(0);

    doExpected = {};
    undoExpected = {};

    obj.changes.clear();
    obj()->vec.moveDown(2);
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, MoveDownN)
{
    Obj obj {};
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88}; // 0, 11, 22, 55, 33, 44, 77, 66, 88
    obj()->vec.select(7);

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 7, .index = 6},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 6, .index = 7},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 5},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 3},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 4},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 6, .index = 7},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 7, .index = 6},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 4},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 5},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 3},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    
    obj.changes.clear();
    obj()->vec.moveDown(std::vector<std::size_t>{3, 4, 6});
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
    
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
    obj()->vec.select(0);

    doExpected = { { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };
    undoExpected = { { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };
    
    obj.changes.clear();
    obj()->vec.moveDown(std::vector<std::size_t>{6, 7, 8});
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, MoveDownL)
{
    Obj obj {};
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88}; // 0, 11, 22, 55, 33, 44, 77, 66, 88
    obj()->vec.select(std::vector<std::size_t>{3, 4, 6});

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 7, .index = 6},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 6, .index = 7},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 5},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 3},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 4},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 6, .index = 7},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 7, .index = 6},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 4},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 5},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 3},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    
    obj.changes.clear();
    obj()->vec.moveSelectionsDown();
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
    
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};

    doExpected = { { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };
    undoExpected = { { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };
    
    obj()->vec.select(std::vector<std::size_t>{6, 7, 8});

    obj.changes.clear();
    obj()->vec.moveSelectionsDown();
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, MoveBottom)
{
    Obj obj {};
    obj()->vec = std::vector{4, 5, 6, 7};
    obj()->vec.select(2);

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 3},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 1},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 2},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 1},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 2},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 3},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };

    obj.changes.clear();
    obj()->vec.moveBottom(1);
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
    
    obj()->vec = std::vector{4, 5, 6, 7};
    obj()->vec.select(0);

    doExpected = {};
    undoExpected = {};

    obj.changes.clear();
    obj()->vec.moveBottom(3);
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, MoveBottomN)
{
    Obj obj {};
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88}; // 0, 11, 22, 55, 77, 88, 33, 44, 66
    obj()->vec.select(7);

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 3},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 7, .index = 4},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 8, .index = 5},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 6},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 7},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 6, .index = 8},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 6, .index = 3},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 7, .index = 4},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 5},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 8, .index = 6},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 7},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 8},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    
    obj.changes.clear();
    obj()->vec.moveBottom(std::vector<std::size_t>{3, 4, 6});
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
    
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
    obj()->vec.select(8);

    doExpected = { { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };
    undoExpected = { { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };
    
    obj.changes.clear();
    obj()->vec.moveBottom(std::vector<std::size_t>{6, 7, 8});
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, MoveBottomL)
{
    Obj obj {};
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88}; // 0, 11, 22, 55, 77, 88, 33, 44, 66
    obj()->vec.select(std::vector<std::size_t>{3, 4, 6});

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 3},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 7, .index = 4},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 8, .index = 5},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 6},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 7},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 6, .index = 8},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 6, .index = 3},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 7, .index = 4},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 5},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 8, .index = 6},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 7},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 8},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    
    obj.changes.clear();
    obj()->vec.moveBottom(std::vector<std::size_t>{3, 4, 6});
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
    
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
    obj()->vec.select(std::vector<std::size_t>{6, 7, 8});

    doExpected = { { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };
    undoExpected = { { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex } };
    
    obj.changes.clear();
    obj()->vec.moveBottom(std::vector<std::size_t>{6, 7, 8});
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, MoveTo)
{
    Obj obj {};
    obj()->vec = std::vector{4, 5, 6, 7};
    obj()->vec.select(1);

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 3},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 1},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 2},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 1},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 2},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 3},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };

    obj.changes.clear();
    obj()->vec.moveTo(1, 3);
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
    
    obj()->vec = std::vector{4, 5, 6, 7};
    obj()->vec.select(1);

    doExpected = {};
    undoExpected = {};

    obj.changes.clear();
    obj()->vec.moveTo(1, 1);
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, MoveToN)
{
    Obj obj {};
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88}; // 11, 33, 66, 0, 22, 44, 55, 77, 88
    obj()->vec.select(std::vector<std::size_t>{1});

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 0},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 1},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 6, .index = 2},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 0, .index = 3},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 4},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 5},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 6},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 0},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 0, .index = 1},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 2},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 3},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 4},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 6, .index = 5},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 6},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    
    obj.changes.clear();
    obj()->vec.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 3);
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

TEST(OpNtfy, MoveToL)
{
    Obj obj {};
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88}; // 11, 33, 66, 0, 22, 44, 55, 77, 88
    obj()->vec.select(std::vector<std::size_t>{0, 2, 4, 5, 7});

    std::vector<Change> doExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 0},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 1},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 6, .index = 2},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 0, .index = 3},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 4},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 5},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 6},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    std::vector<Change> undoExpected {
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 3, .index = 0},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 0, .index = 1},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 4, .index = 2},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 1, .index = 3},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 5, .index = 4},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 6, .index = 5},
        { .type = ChangeType::ElementMoved, .fieldIndex = vecFieldIndex, .oldIndex = 2, .index = 6},
        { .type = ChangeType::SelectionsChanged, .fieldIndex = vecFieldIndex }
    };
    
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88}; // 11, 33, 66, 0, 22, 44, 55, 77, 88
    obj()->vec.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
    obj.changes.clear();
    obj()->vec.moveSelectionsTo(3);
    EXPECT_EQ(doExpected, obj.changes);

    obj.changes.clear();
    obj.undoAction();
    EXPECT_EQ(undoExpected, obj.changes);

    obj.changes.clear();
    obj.redoAction();
    EXPECT_EQ(doExpected, obj.changes);
}

struct NotifyAfterActionTest
{
    int a = 0;

    REFLECT(NotifyAfterActionTest, a)
};

struct EditNotifyAfterActionTest : RareEdit::Tracked<NotifyAfterActionTest, EditNotifyAfterActionTest>
{
    EditNotifyAfterActionTest() : Tracked{this} {}

    std::vector<std::size_t> afterActionIndexes {};
    
    void afterAction(std::size_t actionIndex) { afterActionIndexes.push_back(actionIndex); }
};

TEST(NotifyMisc, NotifyAfterAction)
{
    EditNotifyAfterActionTest myObj {};
    myObj()->a = 1;
    EXPECT_EQ(myObj.afterActionIndexes, std::vector<std::size_t>{0});
    myObj()->a = 2;
    auto expectedActionIndexes = std::vector<std::size_t>{0, 1};
    EXPECT_EQ(myObj.afterActionIndexes, expectedActionIndexes);
}

} // namespace NtfyTest