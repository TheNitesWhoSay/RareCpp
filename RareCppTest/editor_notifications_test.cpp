#include <rarecpp/editor.h>
#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <gtest/gtest.h>
#include <cstdint>
#include <vector>

namespace ntfy_test
{

inline constexpr std::size_t no_index = std::numeric_limits<std::size_t>::max();
inline constexpr int no_value = std::numeric_limits<int>::min();

enum class Change_type {
    value_changed,
    element_added,
    element_removed,
    element_moved,
    selections_changed
};

struct Change {
    Change_type type;
    std::size_t field_index = no_index;
    int old_value = no_value;
    int value = no_value;
    std::size_t old_index = no_index;
    std::size_t index = no_index;

    auto operator==(const Change & other) const {
        return
            this->type == other.type &&
            this->field_index == other.field_index &&
            this->old_value == other.old_value &&
            this->value == other.value &&
            this->old_index == other.old_index &&
            this->index == other.index;
    }

    inline friend std::ostream & operator<<(std::ostream & os, const Change & change)
    {
        //os << Json::out(change);
        switch ( change.type ) {
            case Change_type::value_changed: os << "Ntfy::value_changed" << " from " << change.old_value << " to " << change.value << "\n"; break;
            case Change_type::element_added: os << "Ntfy::element_added" << " at index " << change.index << "\n"; break;
            case Change_type::element_removed: os << "Ntfy::element_removed" << " from index " << change.index << "\n"; break;
            case Change_type::element_moved: os << "Ntfy::element_moved" << " from index " << change.old_index << " to " << change.index << "\n"; break;
            case Change_type::selections_changed: os << "Ntfy::selections_changed" << "\n"; break;
        }
        return os;
    }
};

struct Ntfy_data
{
    int primitive;
    std::vector<int> vec;
    std::vector<std::vector<int>> vec_vec;

    REFLECT(Ntfy_data, primitive, vec, vec_vec)
};

static constexpr std::size_t primitive_field_index = RareTs::MemberType<Ntfy_data>::primitive::index;
static constexpr std::size_t vec_field_index = RareTs::MemberType<Ntfy_data>::vec::index;
static constexpr std::size_t vec_vec_field_index = RareTs::MemberType<Ntfy_data>::vec_vec::index;

struct Obj : nf::tracked<Ntfy_data, Obj>
{
    Obj() : tracked{this} {}
    
    using primitive_path = NF_PATH(root->primitive);
    using vec_path = NF_PATH(root->vec);
    using vec_elem_path = NF_PATH(root->vec[0]);
    using vec_vec_path = NF_PATH(root->vec_vec);
    using vec_vec_elem_path = NF_PATH(root->vec_vec[0]);

    std::vector<Change> changes {};

    void push_change(auto && change) {
        changes.push_back(std::move(change));
        //std::cout << changes.back() << '\n';
    }

    void value_changed(primitive_path, int old_value, int value)
    {
        push_change(Change{ .type = Change_type::value_changed, .field_index = primitive_field_index, .old_value = old_value, .value = value });
    }
    void element_added(primitive_path, std::size_t index)
    {
        push_change(Change{ .type = Change_type::element_added, .field_index = primitive_field_index, .index = index });
    }
    void element_removed(primitive_path, std::size_t index)
    {
        push_change(Change{ .type = Change_type::element_removed, .field_index = primitive_field_index, .index = index });
    }
    void element_moved(primitive_path, std::size_t old_index, std::size_t index)
    {
        push_change(Change{ .type = Change_type::element_moved, .field_index = primitive_field_index, .old_index = old_index, .index = index });
    }
    void selections_changed(primitive_path)
    {
        push_change(Change{ .type = Change_type::selections_changed, .field_index = primitive_field_index });
    }
    
    void value_changed(vec_path, int old_value, int value)
    {
        push_change(Change{ .type = Change_type::value_changed, .field_index = vec_field_index, .old_value = old_value, .value = value });
    }
    void value_changed(vec_elem_path path, int old_value, int value)
    {
        push_change(Change{ .type = Change_type::value_changed, .field_index = vec_field_index, .old_value = old_value, .value = value, .index = path.index<0>() });
    }
    void element_added(vec_path, std::size_t index)
    {
        push_change(Change{ .type = Change_type::element_added, .field_index = vec_field_index, .index = index });
    }
    void element_removed(vec_path, std::size_t index)
    {
        push_change(Change{ .type = Change_type::element_removed, .field_index = vec_field_index, .index = index });
    }
    void element_moved(vec_path, std::size_t old_index, std::size_t index)
    {
        push_change(Change{ .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = old_index, .index = index });
    }
    void selections_changed(vec_path)
    {
        push_change(Change{ .type = Change_type::selections_changed, .field_index = vec_field_index });
    }

    void value_changed(vec_vec_path, int old_value, int value)
    {
        push_change(Change{ .type = Change_type::value_changed, .field_index = vec_vec_field_index, .old_value = old_value, .value = value });
    }
    void element_added(vec_vec_path, std::size_t index)
    {
        push_change(Change{ .type = Change_type::element_added, .field_index = vec_vec_field_index, .index = index });
    }
    void element_added(vec_vec_elem_path path, std::size_t index)
    {
        push_change(Change{ .type = Change_type::element_added, .field_index = vec_vec_field_index, .old_index = path.index<0>(), .index = index });
    }
    void element_removed(vec_vec_path, std::size_t index)
    {
        push_change(Change{ .type = Change_type::element_removed, .field_index = vec_vec_field_index, .index = index });
    }
    void element_removed(vec_vec_elem_path path, std::size_t index)
    {
        push_change(Change{ .type = Change_type::element_removed, .field_index = vec_vec_field_index, .old_index = path.index<0>(), .index = index });
    }
    void element_moved(vec_vec_path, std::size_t old_index, std::size_t index)
    {
        push_change(Change{ .type = Change_type::element_moved, .field_index = vec_vec_field_index, .old_index = old_index, .index = index });
    }
    void selections_changed(vec_vec_path)
    {
        push_change(Change{ .type = Change_type::selections_changed, .field_index = vec_vec_field_index });
    }

};

TEST(op_ntfy, reset)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6});
    obj()->vec.select(0);

    std::vector<Change> do_expected {
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 2 },
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 1 },
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 0 },
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 0 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 1 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 2 },
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };

    obj.changes.clear();
    obj()->vec.reset();
    EXPECT_EQ(do_expected, obj.changes);
    
    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);
    
    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, assign)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6});
    obj()->vec.select(0);
    
    std::vector<Change> do_expected {
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 2 },
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 1 },
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 0 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 0 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 1 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 2 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 3 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 4 },
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 4 },
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 3 },
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 2 },
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 1 },
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 0 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 0 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 1 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 2 },
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };

    obj.changes.clear();
    obj()->vec.assign(5, 333);
    EXPECT_EQ(do_expected, obj.changes);
    
    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);
    
    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, assign_default)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6});
    obj()->vec.select(0);
    
    std::vector<Change> do_expected {
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 2 },
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 1 },
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 0 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 0 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 1 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 2 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 3 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 4 },
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 4 },
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 3 },
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 2 },
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 1 },
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 0 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 0 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 1 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 2 },
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };

    obj.changes.clear();
    obj()->vec.assign_default(5);
    EXPECT_EQ(do_expected, obj.changes);
    
    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);
    
    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, clear_seletions)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6, 7, 8});
    obj()->vec.select(std::vector<std::size_t>{0, 1, 2});

    std::vector<Change> expected { {.type = Change_type::selections_changed, .field_index = vec_field_index } };

    obj.changes.clear();
    obj()->vec.clear_selections();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(expected, obj.changes);
}

TEST(op_ntfy, select_all)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6, 7, 8});

    std::vector<Change> expected { {.type = Change_type::selections_changed, .field_index = vec_field_index } };

    obj.changes.clear();
    obj()->vec.select_all();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(expected, obj.changes);
}

TEST(op_ntfy, select)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6, 7, 8});
    obj()->vec.select(std::vector<std::size_t>{0, 1, 2});

    std::vector<Change> expected { {.type = Change_type::selections_changed, .field_index = vec_field_index } };

    obj.changes.clear();
    obj()->vec.select(3);
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(expected, obj.changes);
}

TEST(op_ntfy, select_n)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6, 7, 8});

    std::vector<Change> expected { {.type = Change_type::selections_changed, .field_index = vec_field_index } };

    obj.changes.clear();
    obj()->vec.select(std::vector<std::size_t>{0, 1, 2});
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(expected, obj.changes);
}

TEST(op_ntfy, deselect)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6, 7, 8});
    obj()->vec.select(std::vector<std::size_t>{0, 1, 2});

    std::vector<Change> expected { {.type = Change_type::selections_changed, .field_index = vec_field_index } };

    obj.changes.clear();
    obj()->vec.deselect(1);
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(expected, obj.changes);
}

TEST(op_ntfy, deselect_n)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6, 7, 8});
    obj()->vec.select(std::vector<std::size_t>{0, 1, 2});

    std::vector<Change> expected { {.type = Change_type::selections_changed, .field_index = vec_field_index } };

    obj.changes.clear();
    obj()->vec.deselect(std::vector<std::size_t>{1, 2});
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(expected, obj.changes);
}

TEST(op_ntfy, toggle_selection)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6, 7, 8});
    obj()->vec.select(std::vector<std::size_t>{0, 1, 2});

    std::vector<Change> expected { {.type = Change_type::selections_changed, .field_index = vec_field_index } };

    obj.changes.clear();
    obj()->vec.toggle_selected(3);
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(expected, obj.changes);
}

TEST(op_ntfy, toggle_selection_n)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6, 7, 8});
    obj()->vec.select(std::vector<std::size_t>{0, 1, 2});

    std::vector<Change> expected { {.type = Change_type::selections_changed, .field_index = vec_field_index } };

    obj.changes.clear();
    obj()->vec.toggle_selected(std::vector<std::size_t>{2, 3});
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(expected, obj.changes);
}

TEST(op_ntfy, sort_selections)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6, 7, 8});
    obj()->vec.select(std::vector<std::size_t>{0, 1, 2});

    std::vector<Change> expected { {.type = Change_type::selections_changed, .field_index = vec_field_index } };

    obj.changes.clear();
    obj()->vec.sort_selection();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(expected, obj.changes);
}

TEST(op_ntfy, sort_selections_desc)
{
    Obj obj {};
    obj()->vec.append(std::vector{4, 5, 6, 7, 8});
    obj()->vec.select(std::vector<std::size_t>{0, 1, 2});

    std::vector<Change> expected { {.type = Change_type::selections_changed, .field_index = vec_field_index } };

    obj.changes.clear();
    obj()->vec.sort_selection_descending();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(expected, obj.changes);
}

TEST(op_ntfy, set)
{
    Obj obj {};
    obj()->primitive = 22;
    
    std::vector<Change> do_expected { {.type = Change_type::value_changed, .field_index = primitive_field_index, .old_value = 22, .value = 33} };
    std::vector<Change> undo_expected { {.type = Change_type::value_changed, .field_index = primitive_field_index, .old_value = 33, .value = 22} };

    obj.changes.clear();
    obj()->primitive = 33;
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);


    obj()->vec = std::vector{3, 4, 5};
    obj()->vec.select(0);

    do_expected = {
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 2 },
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 1 },
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 0 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 0 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 1 },
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    undo_expected = {
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 1 },
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 0 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 0 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 1 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 2 },
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };

    obj.changes.clear();
    obj()->vec = std::vector{7, 8};
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, set_n)
{
    Obj obj {};
    obj()->vec = std::vector{4, 5, 6, 7, 8};

    std::vector<Change> do_expected {
        {.type = Change_type::value_changed, .field_index = vec_field_index, .old_value = 4, .value = 3, .index = 0},
        {.type = Change_type::value_changed, .field_index = vec_field_index, .old_value = 6, .value = 3, .index = 2},
        {.type = Change_type::value_changed, .field_index = vec_field_index, .old_value = 8, .value = 3, .index = 4}
    };
    std::vector<Change> undo_expected {
        {.type = Change_type::value_changed, .field_index = vec_field_index, .old_value = 3, .value = 4, .index = 0},
        {.type = Change_type::value_changed, .field_index = vec_field_index, .old_value = 3, .value = 6, .index = 2},
        {.type = Change_type::value_changed, .field_index = vec_field_index, .old_value = 3, .value = 8, .index = 4}
    };

    obj.changes.clear();
    obj()->vec.set(std::vector<std::size_t>{0, 2, 4}, 3);
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
    
    obj()->vec_vec = std::vector<std::vector<int>> {{1, 2}, {3, 4, 5}, {6, 7, 8, 9}};

    do_expected = {
        { .type = Change_type::element_removed, .field_index = vec_vec_field_index, .old_index = 0, .index = 1 },
        { .type = Change_type::element_removed, .field_index = vec_vec_field_index, .old_index = 0, .index = 0 },
        { .type = Change_type::element_added, .field_index = vec_vec_field_index, .old_index = 0, .index = 0 },
        { .type = Change_type::element_added, .field_index = vec_vec_field_index, .old_index = 0, .index = 1 },
        { .type = Change_type::element_removed, .field_index = vec_vec_field_index, .old_index = 1, .index = 2 },
        { .type = Change_type::element_removed, .field_index = vec_vec_field_index, .old_index = 1, .index = 1 },
        { .type = Change_type::element_removed, .field_index = vec_vec_field_index, .old_index = 1, .index = 0 },
        { .type = Change_type::element_added, .field_index = vec_vec_field_index, .old_index = 1, .index = 0 },
        { .type = Change_type::element_added, .field_index = vec_vec_field_index, .old_index = 1, .index = 1 }
    };
    undo_expected = {
        { .type = Change_type::element_removed, .field_index = vec_vec_field_index, .old_index = 0, .index = 1 },
        { .type = Change_type::element_removed, .field_index = vec_vec_field_index, .old_index = 0, .index = 0 },
        { .type = Change_type::element_added, .field_index = vec_vec_field_index, .old_index = 0, .index = 0 },
        { .type = Change_type::element_added, .field_index = vec_vec_field_index, .old_index = 0, .index = 1 },
        { .type = Change_type::element_removed, .field_index = vec_vec_field_index, .old_index = 1, .index = 1 },
        { .type = Change_type::element_removed, .field_index = vec_vec_field_index, .old_index = 1, .index = 0 },
        { .type = Change_type::element_added, .field_index = vec_vec_field_index, .old_index = 1, .index = 0 },
        { .type = Change_type::element_added, .field_index = vec_vec_field_index, .old_index = 1, .index = 1 },
        { .type = Change_type::element_added, .field_index = vec_vec_field_index, .old_index = 1, .index = 2 }
    };

    obj.changes.clear();
    obj()->vec_vec.set(std::vector<std::size_t>{0, 1}, std::vector{22, 33});
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, set_l)
{
    Obj obj {};
    obj()->vec = std::vector{4, 5, 6, 7, 8};
    obj()->vec.select(std::vector<std::size_t>{0, 2, 4});

    std::vector<Change> do_expected {
        {.type = Change_type::value_changed, .field_index = vec_field_index, .old_value = 4, .value = 3, .index = 0},
        {.type = Change_type::value_changed, .field_index = vec_field_index, .old_value = 6, .value = 3, .index = 2},
        {.type = Change_type::value_changed, .field_index = vec_field_index, .old_value = 8, .value = 3, .index = 4}
    };
    std::vector<Change> undo_expected {
        {.type = Change_type::value_changed, .field_index = vec_field_index, .old_value = 3, .value = 4, .index = 0},
        {.type = Change_type::value_changed, .field_index = vec_field_index, .old_value = 3, .value = 6, .index = 2},
        {.type = Change_type::value_changed, .field_index = vec_field_index, .old_value = 3, .value = 8, .index = 4}
    };

    obj.changes.clear();
    obj()->vec.selection() = 3;
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, plus_eq_)
{
    Obj obj {};
    obj()->primitive = 2;
    EXPECT_EQ(obj->primitive, 2);
    
    std::vector<Change> do_expected { {.type = Change_type::value_changed, .field_index = primitive_field_index, .old_value = 2, .value = 5} };
    std::vector<Change> undo_expected { {.type = Change_type::value_changed, .field_index = primitive_field_index, .old_value = 5, .value = 2} };
    
    obj.changes.clear();
    obj()->primitive += 3;
    EXPECT_EQ(obj->primitive, 5);
    EXPECT_EQ(do_expected, obj.changes);
    
    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(obj->primitive, 2);
    EXPECT_EQ(undo_expected, obj.changes);
    
    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(obj->primitive, 5);
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, minus_eq_)
{
    Obj obj {};
    obj()->primitive = 7;
    EXPECT_EQ(obj->primitive, 7);
    
    std::vector<Change> do_expected { {.type = Change_type::value_changed, .field_index = primitive_field_index, .old_value = 7, .value = 5} };
    std::vector<Change> undo_expected { {.type = Change_type::value_changed, .field_index = primitive_field_index, .old_value = 5, .value = 7} };
    
    obj.changes.clear();
    obj()->primitive -= 2;
    EXPECT_EQ(obj->primitive, 5);
    EXPECT_EQ(do_expected, obj.changes);
    
    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(obj->primitive, 7);
    EXPECT_EQ(undo_expected, obj.changes);
    
    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(obj->primitive, 5);
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, mult_eq_)
{
    Obj obj {};
    obj()->primitive = 2;
    EXPECT_EQ(obj->primitive, 2);
    
    std::vector<Change> do_expected { {.type = Change_type::value_changed, .field_index = primitive_field_index, .old_value = 2, .value = 4} };
    std::vector<Change> undo_expected { {.type = Change_type::value_changed, .field_index = primitive_field_index, .old_value = 4, .value = 2} };
    
    obj.changes.clear();
    obj()->primitive *= 2;
    EXPECT_EQ(obj->primitive, 4);
    EXPECT_EQ(do_expected, obj.changes);
    
    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(obj->primitive, 2);
    EXPECT_EQ(undo_expected, obj.changes);
    
    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(obj->primitive, 4);
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, div_eq_)
{
    Obj obj {};
    obj()->primitive = 4;
    EXPECT_EQ(obj->primitive, 4);
    
    std::vector<Change> do_expected { {.type = Change_type::value_changed, .field_index = primitive_field_index, .old_value = 4, .value = 2} };
    std::vector<Change> undo_expected { {.type = Change_type::value_changed, .field_index = primitive_field_index, .old_value = 2, .value = 4} };
    
    obj.changes.clear();
    obj()->primitive /= 2;
    EXPECT_EQ(obj->primitive, 2);
    EXPECT_EQ(do_expected, obj.changes);
    
    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(obj->primitive, 4);
    EXPECT_EQ(undo_expected, obj.changes);
    
    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(obj->primitive, 2);
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, mod_eq_)
{
    Obj obj {};
    obj()->primitive = 5;
    EXPECT_EQ(obj->primitive, 5);
    
    std::vector<Change> do_expected { {.type = Change_type::value_changed, .field_index = primitive_field_index, .old_value = 5, .value = 1} };
    std::vector<Change> undo_expected { {.type = Change_type::value_changed, .field_index = primitive_field_index, .old_value = 1, .value = 5} };
    
    obj.changes.clear();
    obj()->primitive %= 2;
    EXPECT_EQ(obj->primitive, 1);
    EXPECT_EQ(do_expected, obj.changes);
    
    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(obj->primitive, 5);
    EXPECT_EQ(undo_expected, obj.changes);
    
    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(obj->primitive, 1);
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, xor_eq_)
{
    Obj obj {};
    obj()->primitive = 0b01;
    EXPECT_EQ(obj->primitive, 0b01);
    
    std::vector<Change> do_expected { {.type = Change_type::value_changed, .field_index = primitive_field_index, .old_value = 0b01, .value = 0b10} };
    std::vector<Change> undo_expected { {.type = Change_type::value_changed, .field_index = primitive_field_index, .old_value = 0b10, .value = 0b01} };
    
    obj.changes.clear();
    obj()->primitive ^= 0b11;
    EXPECT_EQ(obj->primitive, 0b10);
    EXPECT_EQ(do_expected, obj.changes);
    
    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(obj->primitive, 0b01);
    EXPECT_EQ(undo_expected, obj.changes);
    
    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(obj->primitive, 0b10);
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, and_eq_)
{
    Obj obj {};
    obj()->primitive = 0b101;
    EXPECT_EQ(obj->primitive, 0b101);
    
    std::vector<Change> do_expected { {.type = Change_type::value_changed, .field_index = primitive_field_index, .old_value = 0b101, .value = 0b001} };
    std::vector<Change> undo_expected { {.type = Change_type::value_changed, .field_index = primitive_field_index, .old_value = 0b001, .value = 0b101} };
    
    obj.changes.clear();
    obj()->primitive &= 0b011;
    EXPECT_EQ(obj->primitive, 0b001);
    EXPECT_EQ(do_expected, obj.changes);
    
    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(obj->primitive, 0b101);
    EXPECT_EQ(undo_expected, obj.changes);
    
    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(obj->primitive, 0b001);
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, or_eq_)
{
    Obj obj {};
    obj()->primitive = 0b00;
    EXPECT_EQ(obj->primitive, 0b00);
    
    std::vector<Change> do_expected { {.type = Change_type::value_changed, .field_index = primitive_field_index, .old_value = 0b00, .value = 0b11} };
    std::vector<Change> undo_expected { {.type = Change_type::value_changed, .field_index = primitive_field_index, .old_value = 0b11, .value = 0b00} };
    
    obj.changes.clear();
    obj()->primitive |= 0b11;
    EXPECT_EQ(obj->primitive, 0b11);
    EXPECT_EQ(do_expected, obj.changes);
    
    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(obj->primitive, 0b00);
    EXPECT_EQ(undo_expected, obj.changes);
    
    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(obj->primitive, 0b11);
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, append)
{
    Obj obj {};
    obj()->vec = std::vector{2, 3};

    std::vector<Change> do_expected {
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 2 },
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 2 }
    };

    obj.changes.clear();
    obj()->vec.append(4);
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, append_n)
{
    Obj obj {};
    obj()->vec = std::vector{2, 3};

    std::vector<Change> do_expected {
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 2 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 3 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 4 }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 4 },
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 3 },
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 2 }
    };

    obj.changes.clear();
    obj()->vec.append(std::vector{4, 5, 6});
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, insert)
{
    Obj obj {};
    obj()->vec = std::vector{2, 3, 4};
    obj()->vec.select(1);

    std::vector<Change> do_expected {
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 1 },
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 3},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 2},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 1 },
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 1},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 2},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };

    obj.changes.clear();
    obj()->vec.insert(1, 5);
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, insert_n)
{
    Obj obj {};
    obj()->vec = std::vector{2, 3, 4};
    obj()->vec.select(1);

    std::vector<Change> do_expected {
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 1 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 2 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 3 },
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 5},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 4},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 3 },
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 2 },
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 1 },
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 1},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 2},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };

    obj.changes.clear();
    obj()->vec.insert(1, std::vector{5, 6, 7});
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, remove)
{
    Obj obj {};
    obj()->vec = std::vector{2, 3, 4, 5};
    obj()->vec.select(2);

    std::vector<Change> do_expected {
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 1 },
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 1},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 2},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 1 },
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 3},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 2},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };

    obj.changes.clear();
    obj()->vec.remove(1);
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, remove_n)
{
    Obj obj {};
    obj()->vec = std::vector{2, 3, 4, 5, 6};
    obj()->vec.select(2);

    std::vector<Change> do_expected {
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 2 },
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 1 },
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 1},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 2},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 1 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 2 },
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 4},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 3},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };

    obj.changes.clear();
    obj()->vec.remove(std::vector<std::size_t>{1, 2});
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, remove_l)
{
    Obj obj {};
    obj()->vec = std::vector{2, 3, 4, 5, 6};
    obj()->vec.select(std::vector<std::size_t>{1, 2});

    std::vector<Change> do_expected {
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 2 },
        { .type = Change_type::element_removed, .field_index = vec_field_index, .index = 1 },
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 1},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 2},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 1 },
        { .type = Change_type::element_added, .field_index = vec_field_index, .index = 2 },
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 4},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 3},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };

    obj.changes.clear();
    obj()->vec.remove_selection();
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, sort)
{
    Obj obj {};
    obj()->vec = std::vector{1, 9, 2, 8, 3, 7, 11, 5, 6, 4, 22}; // {1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 22};
    obj()->vec.select(2);

    std::vector<Change> do_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 1},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 2},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 9, .index = 3},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 7, .index = 4},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 8, .index = 5},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 6},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 7},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 8},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 6, .index = 9},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 9, .index = 6},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 8, .index = 1},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 7, .index = 3},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 6, .index = 5},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 8},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 7},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 9},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 4},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 2},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };

    obj.changes.clear();
    obj()->vec.sort();
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, sort_desc)
{
    Obj obj {};
    obj()->vec = std::vector{1, 9, 2, 8, 3, 7, 11, 5, 6, 4, 22}; // {22, 11, 9, 8, 7, 6, 5, 4, 3, 2, 1}
    obj()->vec.select(2);

    std::vector<Change> do_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 10, .index = 0},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 6, .index = 1},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 2},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 4},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 8, .index = 5},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 7, .index = 6},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 9, .index = 7},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 8},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 9},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 0, .index = 10},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 10, .index = 0},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 9, .index = 2},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 8, .index = 4},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 7, .index = 9},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 6, .index = 7},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 8},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 5},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 1},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 6},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 0, .index = 10},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };

    obj.changes.clear();
    obj()->vec.sort_desc();
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, swap)
{
    Obj obj {};
    obj()->vec = std::vector{4, 5, 6};
    obj()->vec.select(0);

    std::vector<Change> do_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 0, .index = 2 },
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 0 },
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 0 },
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 0, .index = 2 },
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };

    obj.changes.clear();
    obj()->vec.swap(0, 2);
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, move_up)
{
    Obj obj {};
    obj()->vec = std::vector{4, 5, 6};
    obj()->vec.select(0);

    std::vector<Change> do_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 0},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 0, .index = 1},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 0, .index = 1},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 0},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };

    obj.changes.clear();
    obj()->vec.move_up(1);
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);

    obj()->vec = std::vector{4, 5, 6};
    obj()->vec.select(0);

    do_expected = {};
    undo_expected = {};

    obj.changes.clear();
    obj()->vec.move_up(0);
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, move_up_n)
{
    Obj obj {};
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88}; // 0, 11, 33, 44, 22, 66, 55, 77, 88
    obj()->vec.select(2);

    std::vector<Change> do_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 2},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 4},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 3},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 6},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 6, .index = 5},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 3},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 4},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 2},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 6},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 6, .index = 5},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    
    obj.changes.clear();
    obj()->vec.move_up(std::vector<std::size_t>{3, 4, 6});
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
    
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
    obj()->vec.select(0);

    do_expected = { { .type = Change_type::selections_changed, .field_index = vec_field_index } };
    undo_expected = { { .type = Change_type::selections_changed, .field_index = vec_field_index } };
    
    obj.changes.clear();
    obj()->vec.move_up(std::vector<std::size_t>{0, 1, 2});
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, move_up_l)
{
    Obj obj {};
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88}; // 0, 11, 33, 44, 22, 66, 55, 77, 88
    obj()->vec.select(std::vector<std::size_t>{3, 4, 6});

    std::vector<Change> do_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 2},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 4},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 3},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 6},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 6, .index = 5},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 3},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 4},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 2},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 6},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 6, .index = 5},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    
    obj.changes.clear();
    obj()->vec.move_selections_up();
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
    
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
    obj()->vec.select(std::vector<std::size_t>{0, 1, 2});

    do_expected = { { .type = Change_type::selections_changed, .field_index = vec_field_index } };
    undo_expected = { { .type = Change_type::selections_changed, .field_index = vec_field_index } };
    
    obj.changes.clear();
    obj()->vec.move_selections_up();
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, move_top)
{
    Obj obj {};
    obj()->vec = std::vector{4, 5, 6, 7};
    obj()->vec.select(1);

    std::vector<Change> do_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 0},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 0, .index = 1},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 2},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 0, .index = 2},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 0},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 1},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };

    obj.changes.clear();
    obj()->vec.move_top(2);
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
    
    obj()->vec = std::vector{4, 5, 6, 7};
    obj()->vec.select(0);

    do_expected = {};
    undo_expected = {};

    obj.changes.clear();
    obj()->vec.move_top(0);
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, move_top_n)
{
    Obj obj {};
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88}; // 33, 44, 66, 0, 11, 22, 55, 77, 88
    obj()->vec.select(2);

    std::vector<Change> do_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 0},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 1},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 6, .index = 2},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 0, .index = 3},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 4},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 5},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 6},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 0, .index = 3},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 4},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 5},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 0},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 1},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 6},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 6, .index = 2},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    
    obj.changes.clear();
    obj()->vec.move_top(std::vector<std::size_t>{3, 4, 6});
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
    
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
    obj()->vec.select(0);

    do_expected = { { .type = Change_type::selections_changed, .field_index = vec_field_index } };
    undo_expected = { { .type = Change_type::selections_changed, .field_index = vec_field_index } };
    
    obj.changes.clear();
    obj()->vec.move_top(std::vector<std::size_t>{0, 1, 2});
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, move_top_l)
{
    Obj obj {};
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88}; // 33, 44, 66, 0, 11, 22, 55, 77, 88
    obj()->vec.select(std::vector<std::size_t>{3, 4, 6});

    std::vector<Change> do_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 0},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 1},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 6, .index = 2},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 0, .index = 3},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 4},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 5},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 6},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 0, .index = 3},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 4},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 5},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 0},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 1},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 6},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 6, .index = 2},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    
    obj.changes.clear();
    obj()->vec.move_selections_top();
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
    
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
    obj()->vec.select(std::vector<std::size_t>{0, 1, 2});

    do_expected = { { .type = Change_type::selections_changed, .field_index = vec_field_index } };
    undo_expected = { { .type = Change_type::selections_changed, .field_index = vec_field_index } };
    
    obj.changes.clear();
    obj()->vec.move_selections_top();
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, move_down)
{
    Obj obj {};
    obj()->vec = std::vector{4, 5, 6};
    obj()->vec.select(0);

    std::vector<Change> do_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 2},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 1},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 1},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 2},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };

    obj.changes.clear();
    obj()->vec.move_down(1);
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);

    obj()->vec = std::vector{4, 5, 6};
    obj()->vec.select(0);

    do_expected = {};
    undo_expected = {};

    obj.changes.clear();
    obj()->vec.move_down(2);
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, move_down_n)
{
    Obj obj {};
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88}; // 0, 11, 22, 55, 33, 44, 77, 66, 88
    obj()->vec.select(7);

    std::vector<Change> do_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 7, .index = 6},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 6, .index = 7},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 5},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 3},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 4},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 6, .index = 7},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 7, .index = 6},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 4},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 5},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 3},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    
    obj.changes.clear();
    obj()->vec.move_down(std::vector<std::size_t>{3, 4, 6});
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
    
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
    obj()->vec.select(0);

    do_expected = { { .type = Change_type::selections_changed, .field_index = vec_field_index } };
    undo_expected = { { .type = Change_type::selections_changed, .field_index = vec_field_index } };
    
    obj.changes.clear();
    obj()->vec.move_down(std::vector<std::size_t>{6, 7, 8});
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, move_down_l)
{
    Obj obj {};
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88}; // 0, 11, 22, 55, 33, 44, 77, 66, 88
    obj()->vec.select(std::vector<std::size_t>{3, 4, 6});

    std::vector<Change> do_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 7, .index = 6},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 6, .index = 7},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 5},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 3},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 4},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 6, .index = 7},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 7, .index = 6},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 4},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 5},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 3},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    
    obj.changes.clear();
    obj()->vec.move_selections_down();
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
    
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};

    do_expected = { { .type = Change_type::selections_changed, .field_index = vec_field_index } };
    undo_expected = { { .type = Change_type::selections_changed, .field_index = vec_field_index } };
    
    obj()->vec.select(std::vector<std::size_t>{6, 7, 8});

    obj.changes.clear();
    obj()->vec.move_selections_down();
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, move_bottom)
{
    Obj obj {};
    obj()->vec = std::vector{4, 5, 6, 7};
    obj()->vec.select(2);

    std::vector<Change> do_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 3},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 1},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 2},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 1},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 2},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 3},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };

    obj.changes.clear();
    obj()->vec.move_bottom(1);
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
    
    obj()->vec = std::vector{4, 5, 6, 7};
    obj()->vec.select(0);

    do_expected = {};
    undo_expected = {};

    obj.changes.clear();
    obj()->vec.move_bottom(3);
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, move_bottom_n)
{
    Obj obj {};
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88}; // 0, 11, 22, 55, 77, 88, 33, 44, 66
    obj()->vec.select(7);

    std::vector<Change> do_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 3},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 7, .index = 4},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 8, .index = 5},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 6},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 7},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 6, .index = 8},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 6, .index = 3},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 7, .index = 4},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 5},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 8, .index = 6},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 7},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 8},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    
    obj.changes.clear();
    obj()->vec.move_bottom(std::vector<std::size_t>{3, 4, 6});
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
    
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
    obj()->vec.select(8);

    do_expected = { { .type = Change_type::selections_changed, .field_index = vec_field_index } };
    undo_expected = { { .type = Change_type::selections_changed, .field_index = vec_field_index } };
    
    obj.changes.clear();
    obj()->vec.move_bottom(std::vector<std::size_t>{6, 7, 8});
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, move_bottom_l)
{
    Obj obj {};
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88}; // 0, 11, 22, 55, 77, 88, 33, 44, 66
    obj()->vec.select(std::vector<std::size_t>{3, 4, 6});

    std::vector<Change> do_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 3},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 7, .index = 4},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 8, .index = 5},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 6},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 7},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 6, .index = 8},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 6, .index = 3},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 7, .index = 4},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 5},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 8, .index = 6},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 7},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 8},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    
    obj.changes.clear();
    obj()->vec.move_bottom(std::vector<std::size_t>{3, 4, 6});
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
    
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
    obj()->vec.select(std::vector<std::size_t>{6, 7, 8});

    do_expected = { { .type = Change_type::selections_changed, .field_index = vec_field_index } };
    undo_expected = { { .type = Change_type::selections_changed, .field_index = vec_field_index } };
    
    obj.changes.clear();
    obj()->vec.move_bottom(std::vector<std::size_t>{6, 7, 8});
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, move_to)
{
    Obj obj {};
    obj()->vec = std::vector{4, 5, 6, 7};
    obj()->vec.select(1);

    std::vector<Change> do_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 3},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 1},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 2},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 1},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 2},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 3},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };

    obj.changes.clear();
    obj()->vec.move_to(1, 3);
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
    
    obj()->vec = std::vector{4, 5, 6, 7};
    obj()->vec.select(1);

    do_expected = {};
    undo_expected = {};

    obj.changes.clear();
    obj()->vec.move_to(1, 1);
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, move_to_n)
{
    Obj obj {};
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88}; // 11, 33, 66, 0, 22, 44, 55, 77, 88
    obj()->vec.select(std::vector<std::size_t>{1});

    std::vector<Change> do_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 0},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 1},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 6, .index = 2},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 0, .index = 3},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 4},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 5},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 6},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 0},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 0, .index = 1},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 2},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 3},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 4},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 6, .index = 5},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 6},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    
    obj.changes.clear();
    obj()->vec.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 3);
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

TEST(op_ntfy, move_to_l)
{
    Obj obj {};
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88}; // 11, 33, 66, 0, 22, 44, 55, 77, 88
    obj()->vec.select(std::vector<std::size_t>{0, 2, 4, 5, 7});

    std::vector<Change> do_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 0},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 1},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 6, .index = 2},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 0, .index = 3},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 4},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 5},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 6},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    std::vector<Change> undo_expected {
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 3, .index = 0},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 0, .index = 1},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 4, .index = 2},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 1, .index = 3},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 5, .index = 4},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 6, .index = 5},
        { .type = Change_type::element_moved, .field_index = vec_field_index, .old_index = 2, .index = 6},
        { .type = Change_type::selections_changed, .field_index = vec_field_index }
    };
    
    obj()->vec = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88}; // 11, 33, 66, 0, 22, 44, 55, 77, 88
    obj()->vec.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
    obj.changes.clear();
    obj()->vec.move_selections_to(3);
    EXPECT_EQ(do_expected, obj.changes);

    obj.changes.clear();
    obj.undo_action();
    EXPECT_EQ(undo_expected, obj.changes);

    obj.changes.clear();
    obj.redo_action();
    EXPECT_EQ(do_expected, obj.changes);
}

struct Notify_after_action_test
{
    int a = 0;

    REFLECT(Notify_after_action_test, a)
};

struct Edit_notify_after_action_test : nf::tracked<Notify_after_action_test, Edit_notify_after_action_test>
{
    Edit_notify_after_action_test() : tracked{this} {}

    std::vector<std::size_t> after_action_indexes {};
    
    void after_action(std::size_t action_index) { after_action_indexes.push_back(action_index); }
};

TEST(notify_misc, notify_after_action)
{
    Edit_notify_after_action_test my_obj {};
    my_obj()->a = 1;
    EXPECT_EQ(my_obj.after_action_indexes, std::vector<std::size_t>{0});
    my_obj()->a = 2;
    auto expected_action_indexes = std::vector<std::size_t>{0, 1};
    EXPECT_EQ(my_obj.after_action_indexes, expected_action_indexes);
}

} // namespace ntfy_test