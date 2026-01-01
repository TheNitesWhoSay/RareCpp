#include <rarecpp/editor.h>
#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <gtest/gtest.h>
#include <cstdint>
#include <vector>

namespace editor_cumulative
{
    struct Simple
    {
        int a = 0;

        REFLECT(Simple, a)
    };

    struct Simple_editor : nf::tracked<Simple, Simple_editor>
    {
        Simple_editor() : tracked{this} {}

        void act()
        {
            create_action()->a = 0;
        }
    };

    TEST(undo_redo_elision, unmodified)
    {
        Simple_editor obj {};
        auto check = RareTs::whitebox((nf::tracked<Simple, Simple_editor> &)obj);

        EXPECT_EQ(check.redo_count, 0);
        EXPECT_EQ(check.redo_size, 0);
        EXPECT_TRUE(check.actions.empty());
    }

    TEST(undo_redo_elision, a)
    {
        Simple_editor obj {};
        auto check = RareTs::whitebox((nf::tracked<Simple, Simple_editor> &)obj);
        obj.act();

        EXPECT_EQ(check.redo_count, 0);
        EXPECT_EQ(check.redo_size, 0);
        EXPECT_EQ(check.actions.size(), 1);
        EXPECT_EQ(check.actions[0].first_event_index, 0);
    }

    TEST(undo_redo_elision, au)
    {
        Simple_editor obj {};
        auto check = RareTs::whitebox((nf::tracked<Simple, Simple_editor> &)obj);
        obj.act();
        obj.undo_action();

        EXPECT_EQ(check.redo_count, 1);
        EXPECT_EQ(check.redo_size, 1);
        EXPECT_EQ(check.actions.size(), 1);
        EXPECT_EQ(check.actions[0].first_event_index, 0);
    }

    TEST(undo_redo_elision, aua)
    {
        Simple_editor obj {};
        auto check = RareTs::whitebox((nf::tracked<Simple, Simple_editor> &)obj);
        obj.act();
        obj.undo_action();
        obj.act();

        EXPECT_EQ(check.redo_count, 0);
        EXPECT_EQ(check.redo_size, 0);
        EXPECT_EQ(check.actions.size(), 3);
        EXPECT_EQ(check.actions[0].first_event_index, 0);
        EXPECT_EQ(check.actions[1].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[2].first_event_index, 1);
    }

    TEST(undo_redo_elision, aur)
    {
        Simple_editor obj {};
        auto check = RareTs::whitebox((nf::tracked<Simple, Simple_editor> &)obj);
        obj.act();
        obj.undo_action();
        obj.redo_action();

        EXPECT_EQ(check.redo_count, 0);
        EXPECT_EQ(check.redo_size, 0);
        EXPECT_EQ(check.actions.size(), 1);
        EXPECT_EQ(check.actions[0].first_event_index, 0);
    }

    TEST(undo_redo_elision, aa)
    {
        Simple_editor obj {};
        auto check = RareTs::whitebox((nf::tracked<Simple, Simple_editor> &)obj);
        obj.act();
        obj.act();

        EXPECT_EQ(check.redo_count, 0);
        EXPECT_EQ(check.redo_size, 0);
        EXPECT_EQ(check.actions.size(), 2);
        EXPECT_EQ(check.actions[0].first_event_index, 0);
        EXPECT_EQ(check.actions[1].first_event_index, 1);
    }

    TEST(undo_redo_elision, aau)
    {
        Simple_editor obj {};
        auto check = RareTs::whitebox((nf::tracked<Simple, Simple_editor> &)obj);
        obj.act();
        obj.act();
        obj.undo_action();

        EXPECT_EQ(check.redo_count, 1);
        EXPECT_EQ(check.redo_size, 1);
        EXPECT_EQ(check.actions.size(), 2);
        EXPECT_EQ(check.actions[0].first_event_index, 0);
        EXPECT_EQ(check.actions[1].first_event_index, 1);
    }

    TEST(undo_redo_elision, aaua)
    {
        Simple_editor obj {};
        auto check = RareTs::whitebox((nf::tracked<Simple, Simple_editor> &)obj);
        obj.act();
        obj.act();
        obj.undo_action();
        obj.act();

        EXPECT_EQ(check.redo_count, 0);
        EXPECT_EQ(check.redo_size, 0);
        EXPECT_EQ(check.actions.size(), 4);
        EXPECT_EQ(check.actions[0].first_event_index, 0);
        EXPECT_EQ(check.actions[1].first_event_index, 1);
        EXPECT_EQ(check.actions[2].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[3].first_event_index, 2);
    }

    TEST(undo_redo_elision, aauau)
    {
        Simple_editor obj {};
        auto check = RareTs::whitebox((nf::tracked<Simple, Simple_editor> &)obj);
        obj.act();
        obj.act();
        obj.undo_action();
        obj.act();
        obj.undo_action();

        EXPECT_EQ(check.redo_count, 1);
        EXPECT_EQ(check.redo_size, 1);
        EXPECT_EQ(check.actions.size(), 4);
        EXPECT_EQ(check.actions[0].first_event_index, 0);
        EXPECT_EQ(check.actions[1].first_event_index, 1);
        EXPECT_EQ(check.actions[2].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[3].first_event_index, 2);
    }

    TEST(undo_redo_elision, aauauu)
    {
        Simple_editor obj {};
        auto check = RareTs::whitebox((nf::tracked<Simple, Simple_editor> &)obj);
        obj.act();
        obj.act();
        obj.undo_action();
        obj.act();
        obj.undo_action();
        obj.undo_action();

        EXPECT_EQ(check.redo_count, 2);
        EXPECT_EQ(check.redo_size, 4);
        EXPECT_EQ(check.actions.size(), 4);
        EXPECT_EQ(check.actions[0].first_event_index, 0);
        EXPECT_EQ(check.actions[1].first_event_index, 1);
        EXPECT_EQ(check.actions[2].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[3].first_event_index, 2);
    }

    TEST(undo_redo_elision, aauauua)
    {
        Simple_editor obj {};
        auto check = RareTs::whitebox((nf::tracked<Simple, Simple_editor> &)obj);
        obj.act();
        obj.act();
        obj.undo_action();
        obj.act();
        obj.undo_action();
        obj.undo_action();
        obj.act();

        EXPECT_EQ(check.redo_count, 0);
        EXPECT_EQ(check.redo_size, 0);
        EXPECT_EQ(check.actions.size(), 6);
        EXPECT_EQ(check.actions[0].first_event_index, 0);
        EXPECT_EQ(check.actions[1].first_event_index, 1);
        EXPECT_EQ(check.actions[2].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[3].first_event_index, 2);
        EXPECT_EQ(check.actions[4].first_event_index, check.flag_elided_redos | 4);
        EXPECT_EQ(check.actions[5].first_event_index, 3);
    }

    TEST(undo_redo_elision, aauaua)
    {
        Simple_editor obj {};
        auto check = RareTs::whitebox((nf::tracked<Simple, Simple_editor> &)obj);
        obj.act();
        obj.act();
        obj.undo_action();
        obj.act();
        obj.undo_action();
        obj.act();

        EXPECT_EQ(check.redo_count, 0);
        EXPECT_EQ(check.redo_size, 0);
        EXPECT_EQ(check.actions.size(), 6);
        EXPECT_EQ(check.actions[0].first_event_index, 0);
        EXPECT_EQ(check.actions[1].first_event_index, 1);
        EXPECT_EQ(check.actions[2].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[3].first_event_index, 2);
        EXPECT_EQ(check.actions[4].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[5].first_event_index, 3);
    }

    TEST(undo_redo_elision, aauauau)
    {
        Simple_editor obj {};
        auto check = RareTs::whitebox((nf::tracked<Simple, Simple_editor> &)obj);
        obj.act();
        obj.act();
        obj.undo_action();
        obj.act();
        obj.undo_action();
        obj.act();
        obj.undo_action();

        EXPECT_EQ(check.redo_count, 1);
        EXPECT_EQ(check.redo_size, 1);
        EXPECT_EQ(check.actions.size(), 6);
        EXPECT_EQ(check.actions[0].first_event_index, 0);
        EXPECT_EQ(check.actions[1].first_event_index, 1);
        EXPECT_EQ(check.actions[2].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[3].first_event_index, 2);
        EXPECT_EQ(check.actions[4].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[5].first_event_index, 3);
    }

    TEST(undo_redo_elision, aauauauu)
    {
        Simple_editor obj {};
        auto check = RareTs::whitebox((nf::tracked<Simple, Simple_editor> &)obj);
        obj.act();
        obj.act();
        obj.undo_action();
        obj.act();
        obj.undo_action();
        obj.act();
        obj.undo_action();
        obj.undo_action();

        EXPECT_EQ(check.redo_count, 2);
        EXPECT_EQ(check.redo_size, 6);
        EXPECT_EQ(check.actions.size(), 6);
        EXPECT_EQ(check.actions[0].first_event_index, 0);
        EXPECT_EQ(check.actions[1].first_event_index, 1);
        EXPECT_EQ(check.actions[2].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[3].first_event_index, 2);
        EXPECT_EQ(check.actions[4].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[5].first_event_index, 3);
    }

    TEST(undo_redo_elision, aauauauua)
    {
        Simple_editor obj {};
        auto check = RareTs::whitebox((nf::tracked<Simple, Simple_editor> &)obj);
        obj.act();
        obj.act();
        obj.undo_action();
        obj.act();
        obj.undo_action();
        obj.act();
        obj.undo_action();
        obj.undo_action();
        obj.act();

        EXPECT_EQ(check.redo_count, 0);
        EXPECT_EQ(check.redo_size, 0);
        EXPECT_EQ(check.actions.size(), 8);
        EXPECT_EQ(check.actions[0].first_event_index, 0);
        EXPECT_EQ(check.actions[1].first_event_index, 1);
        EXPECT_EQ(check.actions[2].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[3].first_event_index, 2);
        EXPECT_EQ(check.actions[4].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[5].first_event_index, 3);
        EXPECT_EQ(check.actions[6].first_event_index, check.flag_elided_redos | 6);
        EXPECT_EQ(check.actions[7].first_event_index, 4);
    }

    TEST(undo_redo_elision, aauaa)
    {
        Simple_editor obj {};
        auto check = RareTs::whitebox((nf::tracked<Simple, Simple_editor> &)obj);
        obj.act();
        obj.act();
        obj.undo_action();
        obj.act();
        obj.act();

        EXPECT_EQ(check.redo_count, 0);
        EXPECT_EQ(check.redo_size, 0);
        EXPECT_EQ(check.actions.size(), 5);
        EXPECT_EQ(check.actions[0].first_event_index, 0);
        EXPECT_EQ(check.actions[1].first_event_index, 1);
        EXPECT_EQ(check.actions[2].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[3].first_event_index, 2);
        EXPECT_EQ(check.actions[4].first_event_index, 3);
    }

    TEST(undo_redo_elision, aauaau)
    {
        Simple_editor obj {};
        auto check = RareTs::whitebox((nf::tracked<Simple, Simple_editor> &)obj);
        obj.act();
        obj.act();
        obj.undo_action();
        obj.act();
        obj.act();
        obj.undo_action();

        EXPECT_EQ(check.redo_count, 1);
        EXPECT_EQ(check.redo_size, 1);
        EXPECT_EQ(check.actions.size(), 5);
        EXPECT_EQ(check.actions[0].first_event_index, 0);
        EXPECT_EQ(check.actions[1].first_event_index, 1);
        EXPECT_EQ(check.actions[2].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[3].first_event_index, 2);
        EXPECT_EQ(check.actions[4].first_event_index, 3);
    }

    TEST(undo_redo_elision, aauaaua)
    {
        Simple_editor obj {};
        auto check = RareTs::whitebox((nf::tracked<Simple, Simple_editor> &)obj);
        obj.act();
        obj.act();
        obj.undo_action();
        obj.act();
        obj.act();
        obj.undo_action();
        obj.act();

        EXPECT_EQ(check.redo_count, 0);
        EXPECT_EQ(check.redo_size, 0);
        EXPECT_EQ(check.actions.size(), 7);
        EXPECT_EQ(check.actions[0].first_event_index, 0);
        EXPECT_EQ(check.actions[1].first_event_index, 1);
        EXPECT_EQ(check.actions[2].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[3].first_event_index, 2);
        EXPECT_EQ(check.actions[4].first_event_index, 3);
        EXPECT_EQ(check.actions[5].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[6].first_event_index, 4);
    }

    TEST(undo_redo_elision, aauaauau)
    {
        Simple_editor obj {};
        auto check = RareTs::whitebox((nf::tracked<Simple, Simple_editor> &)obj);
        obj.act();
        obj.act();
        obj.undo_action();
        obj.act();
        obj.act();
        obj.undo_action();
        obj.act();
        obj.undo_action();

        EXPECT_EQ(check.redo_count, 1);
        EXPECT_EQ(check.redo_size, 1);
        EXPECT_EQ(check.actions.size(), 7);
        EXPECT_EQ(check.actions[0].first_event_index, 0);
        EXPECT_EQ(check.actions[1].first_event_index, 1);
        EXPECT_EQ(check.actions[2].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[3].first_event_index, 2);
        EXPECT_EQ(check.actions[4].first_event_index, 3);
        EXPECT_EQ(check.actions[5].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[6].first_event_index, 4);
    }

    TEST(undo_redo_elision, aauaauauu)
    {
        Simple_editor obj {};
        auto check = RareTs::whitebox((nf::tracked<Simple, Simple_editor> &)obj);
        obj.act();
        obj.act();
        obj.undo_action();
        obj.act();
        obj.act();
        obj.undo_action();
        obj.act();
        obj.undo_action();
        obj.undo_action();

        EXPECT_EQ(check.redo_count, 2);
        EXPECT_EQ(check.redo_size, 4);
        EXPECT_EQ(check.actions.size(), 7);
        EXPECT_EQ(check.actions[0].first_event_index, 0);
        EXPECT_EQ(check.actions[1].first_event_index, 1);
        EXPECT_EQ(check.actions[2].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[3].first_event_index, 2);
        EXPECT_EQ(check.actions[4].first_event_index, 3);
        EXPECT_EQ(check.actions[5].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[6].first_event_index, 4);
    }

    TEST(undo_redo_elision, aauaauauuu)
    {
        Simple_editor obj {};
        auto check = RareTs::whitebox((nf::tracked<Simple, Simple_editor> &)obj);
        obj.act();
        obj.act();
        obj.undo_action();
        obj.act();
        obj.act();
        obj.undo_action();
        obj.act();
        obj.undo_action();
        obj.undo_action();
        obj.undo_action();

        EXPECT_EQ(check.redo_count, 3);
        EXPECT_EQ(check.redo_size, 7);
        EXPECT_EQ(check.actions.size(), 7);
        EXPECT_EQ(check.actions[0].first_event_index, 0);
        EXPECT_EQ(check.actions[1].first_event_index, 1);
        EXPECT_EQ(check.actions[2].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[3].first_event_index, 2);
        EXPECT_EQ(check.actions[4].first_event_index, 3);
        EXPECT_EQ(check.actions[5].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[6].first_event_index, 4);
    }

    TEST(undo_redo_elision, aauaauauuua)
    {
        Simple_editor obj {};
        auto check = RareTs::whitebox((nf::tracked<Simple, Simple_editor> &)obj);
        obj.act();
        obj.act();
        obj.undo_action();
        obj.act();
        obj.act();
        obj.undo_action();
        obj.act();
        obj.undo_action();
        obj.undo_action();
        obj.undo_action();
        obj.act();

        EXPECT_EQ(check.redo_count, 0);
        EXPECT_EQ(check.redo_size, 0);
        EXPECT_EQ(check.actions.size(), 9);
        EXPECT_EQ(check.actions[0].first_event_index, 0);
        EXPECT_EQ(check.actions[1].first_event_index, 1);
        EXPECT_EQ(check.actions[2].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[3].first_event_index, 2);
        EXPECT_EQ(check.actions[4].first_event_index, 3);
        EXPECT_EQ(check.actions[5].first_event_index, check.flag_elided_redos | 1);
        EXPECT_EQ(check.actions[6].first_event_index, 4);
        EXPECT_EQ(check.actions[7].first_event_index, check.flag_elided_redos | 7);
        EXPECT_EQ(check.actions[8].first_event_index, 5);
    }

    NOTE(Cumulative_test, nf::index_size<std::uint32_t>)
    struct Cumulative_test
    {
        struct Bar
        {
            std::int32_t integer = 0;
            float decimal = 0.f;
            std::vector<std::int32_t> ints {};

            REFLECT(Bar, integer, decimal, ints)
        };
    
        struct Condition
        {
            std::uint8_t type = 0;

            REFLECT(Condition, type)
        };

        struct Trig
        {
            Condition conditions [4];

            REFLECT(Trig, conditions)
        };

        int a = 0;
        std::string b = "asdf";
        std::vector<int> c {};
        Bar bar;
        std::vector<Bar> bar_vec {}; NOTE(bar_vec, nf::index_size<std::uint16_t>)
        int int_ray[5] {};
        int md_int_ray[2][3] {};
        std::vector<Trig> trigs {}; NOTE(trigs, nf::index_size<std::uint16_t>)

        REFLECT_NOTED(Cumulative_test, a, b, c, bar, bar_vec, int_ray, md_int_ray, trigs)
    };

    struct Traced_cumulative_test : nf::tracked<Cumulative_test, Traced_cumulative_test>
    {
        Traced_cumulative_test() : tracked{this} {}

        void take_action()
        {
            auto edit = create_action();
            edit->a = 12;
            edit->b = "qwerty";
            edit->c.append(5);
            edit->c.append(6);
            edit->c.append(7);
            edit->bar.decimal = 2.2f;
            edit->bar.integer = 1;
            create_bar(editor_cumulative::Cumulative_test::Bar{1, 2.2f, {3, 4}});
            create_bar(editor_cumulative::Cumulative_test::Bar{3, 4.4f, {3, 4, 5}});
            edit->bar_vec[0].ints.append(8);
            edit->bar_vec[0].ints.append(9);
            edit->bar_vec[0].ints.append(10);
            edit->bar_vec[1].ints.append(8);
            edit->bar_vec[1].ints.append(9);
            edit->bar_vec[1].ints.append(10);
            edit->int_ray[2] = 5;
            edit->int_ray.select({1, 2, 4});
            edit->int_ray.selection() = 6;
            edit->md_int_ray[1][2] = 23;
            edit->trigs.append(editor_cumulative::Cumulative_test::Trig{});
            edit->trigs.append(editor_cumulative::Cumulative_test::Trig{});
            edit->trigs[0].conditions[3].type = uint8_t(12);
            edit->trigs.select({0, 1});
            edit->trigs.selection().conditions[0].type = uint8_t(11);
        }

        void create_bar(const editor_cumulative::Cumulative_test::Bar & new_bar)
        {
            auto edit = create_action();
            edit->bar_vec.append(new_bar);
        }

        void act()
        {
            create_action();
        }
    };

    TEST(edit_test, cumulative)
    {
        Traced_cumulative_test my_obj {};
        
        my_obj.take_action();
        my_obj.create_bar({});

        std::stringstream end_state {};
        end_state << Json::out(*my_obj);
        EXPECT_STREQ(end_state.str().c_str(), "{\"a\":12,\"b\":\"qwerty\",\"c\":[5,6,7],\"bar\":{\"integer\":1,\"decimal\":2.2,\"ints\":[]},"
            "\"bar_vec\":[{\"integer\":1,\"decimal\":2.2,\"ints\":[3,4,8,9,10]},{\"integer\":3,\"decimal\":4.4,\"ints\":[3,4,5,8,9,10]},"
            "{\"integer\":0,\"decimal\":0,\"ints\":[]}],\"int_ray\":[0,6,6,0,6],\"md_int_ray\":[[0,0,0],[0,0,23]],\"trigs\":["
            "{\"conditions\":[{\"type\":11},{\"type\":0},{\"type\":0},{\"type\":12}]},"
            "{\"conditions\":[{\"type\":11},{\"type\":0},{\"type\":0},{\"type\":0}]}]}");
    }

    struct Track_init_op_data
    {
        int a = 1;
        std::vector<int> b {2, 3};
        int c;
        std::vector<int> d;

        Track_init_op_data() : c(4), d{5, 6} {}

        REFLECT(Track_init_op_data, a, b, c, d)
    };

    struct Track_init_op : nf::tracked<Track_init_op_data, Track_init_op>
    {
        Track_init_op() : tracked{this} {}
    };

    struct Track_do_op_data
    {
        std::vector<int> ints;

        REFLECT(Track_do_op_data, ints)
    };

    struct Track_do_op : nf::tracked<Track_do_op_data, Track_do_op>
    {
        Track_do_op() : tracked{this} {}
    };
    
    TEST(op_misc, init)
    {
        auto b_default_values = std::vector{2, 3};
        auto d_default_values = std::vector{5, 6};
        Track_init_op obj {};
        EXPECT_EQ(1, obj->a);
        EXPECT_EQ(b_default_values, obj->b);
        EXPECT_EQ(4, obj->c);
        EXPECT_EQ(d_default_values, obj->d);
        EXPECT_EQ(0, obj.total_actions());

        obj.record_init();
        EXPECT_EQ(1, obj->a);
        EXPECT_EQ(b_default_values, obj->b);
        EXPECT_EQ(4, obj->c);
        EXPECT_EQ(d_default_values, obj->d);
        EXPECT_EQ(1, obj.total_actions());

        obj.undo_action();
        EXPECT_EQ(1, obj->a);
        EXPECT_EQ(b_default_values, obj->b);
        EXPECT_EQ(4, obj->c);
        EXPECT_EQ(d_default_values, obj->d);
        EXPECT_EQ(1, obj.total_actions());

        obj.redo_action();
        EXPECT_EQ(1, obj->a);
        EXPECT_EQ(b_default_values, obj->b);
        EXPECT_EQ(4, obj->c);
        EXPECT_EQ(d_default_values, obj->d);
        EXPECT_EQ(1, obj.total_actions());

        obj.clear_history();
        EXPECT_EQ(1, obj->a);
        EXPECT_EQ(b_default_values, obj->b);
        EXPECT_EQ(4, obj->c);
        EXPECT_EQ(d_default_values, obj->d);
        EXPECT_EQ(0, obj.total_actions());

        auto b_reinit_values = std::vector{8, 9};
        auto d_reinit_value = std::vector{11, 12};
        Track_init_op_data reinit{};
        reinit.a = 7;
        reinit.b = b_reinit_values;
        reinit.c = 10;
        reinit.d = d_reinit_value;
        obj.init_data<true>(reinit);
        EXPECT_EQ(7, obj->a);
        EXPECT_EQ(b_reinit_values, obj->b);
        EXPECT_EQ(10, obj->c);
        EXPECT_EQ(d_reinit_value, obj->d);
        EXPECT_EQ(1, obj.total_actions());
    }

    TEST(op_do, reset)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        obj()->ints = std::vector<int>{0, 1, 2, 3, 4};
        EXPECT_EQ(5, obj->ints.size());
        obj()->ints.reset();
        EXPECT_EQ(0, obj->ints.size());
    }

    TEST(op_do, reserve)
    {
        Track_do_op obj {};
        std::size_t capacity = obj->ints.capacity();
        obj()->ints.reserve(capacity+1);
        EXPECT_EQ(capacity+1, obj->ints.capacity());
    }

    TEST(op_do, trim)
    {
        Track_do_op obj {};
        std::size_t capacity = obj->ints.capacity();
        obj()->ints.reserve(capacity+1);
        EXPECT_EQ(capacity+1, obj->ints.capacity());
        obj()->ints.trim();
        EXPECT_EQ(0, obj->ints.capacity());
    }

    TEST(op_do, assign)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        obj()->ints.assign(5, 3);
        EXPECT_EQ(5, obj->ints.size());
        for ( auto i : obj->ints )
            EXPECT_EQ(3, i);
    }

    TEST(op_do, assign_default)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        obj()->ints.assign_default(10);
        EXPECT_EQ(10, obj->ints.size());
    }

    TEST(op_do, clear_selection)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 1};
        EXPECT_EQ(2, obj->ints.size());
        EXPECT_EQ(0, obj.view.ints.sel().size());
        obj()->ints.select(1);
        EXPECT_EQ(std::vector<std::size_t>{1}, obj.view.ints.sel());
        obj()->ints.clear_selections();
        EXPECT_EQ(0, obj.view.ints.sel().size());
    }

    TEST(op_do, select_all)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{22, 33};
        EXPECT_EQ(2, obj->ints.size());
        EXPECT_EQ(0, obj.view.ints.sel().size());
        obj()->ints.select_all();
        const auto expected = std::vector<std::size_t>{0, 1};
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(op_do, select)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{22, 33};
        EXPECT_EQ(2, obj->ints.size());
        EXPECT_EQ(0, obj.view.ints.sel().size());
        obj()->ints.select(1);
        EXPECT_EQ(std::vector<std::size_t>{1}, obj.view.ints.sel());
    }

    TEST(op_do, select_n)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        EXPECT_EQ(5, obj->ints.size());
        EXPECT_EQ(0, obj.view.ints.sel().size());
        const auto selection = std::vector<std::size_t>{2, 4};
        obj()->ints.select(selection);
        EXPECT_EQ(selection, obj.view.ints.sel());
        const auto additional = std::vector<std::size_t>{0, 1};
        obj()->ints.select(additional);
        const auto expected = std::vector<std::size_t>{2, 4, 0, 1};
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(op_do, deselect)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        obj()->ints.select(std::vector<std::size_t>{2, 4, 0, 1});
        EXPECT_EQ(5, obj->ints.size());
        EXPECT_EQ(4, obj.view.ints.sel().size());
        obj()->ints.deselect(4);
        const auto expected = std::vector<std::size_t>{2, 0, 1};
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(op_do, deselect_n)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        obj()->ints.select(std::vector<std::size_t>{2, 4, 0, 1});
        EXPECT_EQ(5, obj->ints.size());
        EXPECT_EQ(4, obj.view.ints.sel().size());
        obj()->ints.deselect(std::vector<std::size_t>{4, 2});
        const auto expected = std::vector<std::size_t>{0, 1};
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(op_do, toggle_selection)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        EXPECT_EQ(5, obj->ints.size());
        obj()->ints.toggle_selected(2);
        obj()->ints.toggle_selected(4);
        obj()->ints.toggle_selected(0);
        obj()->ints.toggle_selected(3);
        EXPECT_EQ(4, obj.view.ints.sel().size());
        auto expected = std::vector<std::size_t>{2, 4, 0, 3};
        EXPECT_EQ(expected, obj.view.ints.sel());
        obj()->ints.toggle_selected(4);
        obj()->ints.toggle_selected(2);
        EXPECT_EQ(2, obj.view.ints.sel().size());
        expected = {0, 3};
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(op_do, toggle_selection_n)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        EXPECT_EQ(5, obj->ints.size());
        obj()->ints.toggle_selected(std::vector<std::size_t>{2, 4, 0, 3});
        EXPECT_EQ(4, obj.view.ints.sel().size());
        auto expected = std::vector<std::size_t>{2, 4, 0, 3};
        EXPECT_EQ(expected, obj.view.ints.sel());
        obj()->ints.toggle_selected(std::vector<std::size_t>{4, 2});
        EXPECT_EQ(2, obj.view.ints.sel().size());
        expected = {0, 3};
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(op_do, sort_selections)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        EXPECT_EQ(5, obj->ints.size());
        auto selection = std::vector<std::size_t>{2, 4, 0, 3};
        obj()->ints.select(selection);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj()->ints.sort_selection();
        const auto expected = std::vector<std::size_t>{0, 2, 3, 4};
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(op_do, sort_selections_desc)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        EXPECT_EQ(5, obj->ints.size());
        auto selection = std::vector<std::size_t>{2, 4, 0, 3};
        obj()->ints.select(selection);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj()->ints.sort_selection_descending();
        const auto expected = std::vector<std::size_t>{4, 3, 2, 0};
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(op_do, set)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{0, 1, 2, 3, 4};
        obj()->ints = values;
        EXPECT_EQ(5, obj->ints.size());
        EXPECT_EQ(values, obj->ints);
        obj()->ints[2] = 22;
        values = std::vector{0, 1, 22, 3, 4};
        EXPECT_EQ(values, obj->ints);
    }

    TEST(op_do, set_n)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 1, 2, 3, 4, 5};
        obj()->ints = values;
        EXPECT_EQ(6, obj->ints.size());
        obj()->ints.set(std::vector<std::size_t>{1, 3, 5}, 33);
        const auto expected = std::vector{0, 33, 2, 33, 4, 33};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, set_l)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 1, 2, 3, 4, 5};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        obj()->ints.select(std::vector<std::size_t>{1, 3, 5});
        obj()->ints.selection() = 33;
        const auto expected = std::vector{0, 33, 2, 33, 4, 33};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, plus_eq_)
    {
        Track_do_op obj {};
        obj()->ints.append(2);
        EXPECT_EQ(obj->ints, std::vector{2});
        obj()->ints[0] += 3;
        EXPECT_EQ(obj->ints, std::vector{5});
    }

    TEST(op_do, minus_eq_)
    {
        Track_do_op obj {};
        obj()->ints.append(7);
        EXPECT_EQ(obj->ints, std::vector{7});
        obj()->ints[0] -= 2;
        EXPECT_EQ(obj->ints, std::vector{5});
    }

    TEST(op_do, mult_eq_)
    {
        Track_do_op obj {};
        obj()->ints.append(2);
        EXPECT_EQ(obj->ints, std::vector{2});
        obj()->ints[0] *= 2;
        EXPECT_EQ(obj->ints, std::vector{4});
    }

    TEST(op_do, div_eq_)
    {
        Track_do_op obj {};
        obj()->ints.append(4);
        EXPECT_EQ(obj->ints, std::vector{4});
        obj()->ints[0] /= 2;
        EXPECT_EQ(obj->ints, std::vector{2});
    }

    TEST(op_do, mod_eq_)
    {
        Track_do_op obj {};
        obj()->ints.append(5);
        EXPECT_EQ(obj->ints, std::vector{5});
        obj()->ints[0] %= 2;
        EXPECT_EQ(obj->ints, std::vector{1});
    }

    TEST(op_do, xor_eq_)
    {
        Track_do_op obj {};
        obj()->ints.append(0b01);
        EXPECT_EQ(obj->ints, std::vector{0b01});
        obj()->ints[0] ^= 0b11;
        EXPECT_EQ(obj->ints, std::vector{0b10});
    }

    TEST(op_do, and_eq_)
    {
        Track_do_op obj {};
        obj()->ints.append(0b101);
        EXPECT_EQ(obj->ints, std::vector{0b101});
        obj()->ints[0] &= 0b011;
        EXPECT_EQ(obj->ints, std::vector{0b001});
    }

    TEST(op_do, or_eq_)
    {
        Track_do_op obj {};
        obj()->ints.append(0b00);
        EXPECT_EQ(obj->ints, std::vector{0b00});
        obj()->ints[0] |= 0b11;
        EXPECT_EQ(obj->ints, std::vector{0b11});
    }

    TEST(op_do, append)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        obj()->ints.append(12);
        EXPECT_EQ(std::vector{12}, obj->ints);
        obj()->ints.append(13);
        auto expected = std::vector{12, 13};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, append_n)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{12, 13};
        obj()->ints.append(values);
        EXPECT_EQ(values, obj->ints);
    }

    TEST(op_do, insert)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        obj()->ints.insert(4, 11);
        auto expected = std::vector{0, 1, 2, 3, 11, 4};
        EXPECT_EQ(expected, obj->ints);
        obj()->ints.insert(1, 22);
        expected = std::vector{0, 22, 1, 2, 3, 11, 4};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, insert_n)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        obj()->ints.insert(2, std::vector{11, 12, 13});
        auto expected = std::vector{0, 1, 11, 12, 13, 2, 3, 4};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, remove)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 11, 22, 33, 44};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        obj()->ints.remove(2);
        const auto expected = std::vector{0, 11, 33, 44};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, remove_n)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{0, 11, 22, 33, 44};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        obj()->ints.remove(std::vector{0, 2, 4});
        auto expected = std::vector{11, 33};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, remove_l)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{0, 11, 22, 33, 44};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        obj()->ints.select(std::vector<std::size_t>{1, 3});
        obj()->ints.remove_selection();
        auto expected = std::vector{0, 22, 44};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, sort)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{1, 9, 2, 8, 3, 7, 11, 5, 6, 4, 22};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        obj()->ints.sort();
        const auto expected = std::vector{1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 22};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, sort_desc)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{1, 9, 2, 8, 3, 7, 11, 5, 6, 4, 22};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        obj()->ints.sort_desc();
        const auto expected = std::vector{22, 11, 9, 8, 7, 6, 5, 4, 3, 2, 1};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, swap)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.swap(0, 8);
        auto expected = std::vector{88, 11, 22, 33, 44, 55, 66, 77, 0};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, move_up)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_up(4);
        auto expected = std::vector{0, 11, 22, 44, 33, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj()->ints.move_up(0);
        EXPECT_EQ(expected, obj->ints);
        obj()->ints.move_up(8);
        expected = std::vector{0, 11, 22, 44, 33, 55, 66, 88, 77};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, move_up_n)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_up(std::vector<std::size_t>{3, 4, 6});
        auto expected = std::vector{0, 11, 33, 44, 22, 66, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);

        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_up(std::vector<std::size_t>{6, 4, 3});
        EXPECT_EQ(expected, obj->ints);

        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_up(std::vector<std::size_t>{0, 8});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 88, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_up(std::vector<std::size_t>{0, 1});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_up(std::vector<std::size_t>{1, 0});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, move_up_l)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{3, 4, 6});
        obj()->ints.move_selections_up();
        auto expected = std::vector{0, 11, 33, 44, 22, 66, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);

        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{6, 4, 3});
        obj()->ints.move_selections_up();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 8});
        obj()->ints.move_selections_up();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 88, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 1});
        obj()->ints.move_selections_up();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{1, 0});
        obj()->ints.move_selections_up();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, move_top)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_top(4);
        auto expected = std::vector{44, 0, 11, 22, 33, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_top(0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);

        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_top(8);
        expected = std::vector{88, 0, 11, 22, 33, 44, 55, 66, 77};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, move_top_n)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_top(std::vector<std::size_t>{0, 2, 4, 5, 7});
        auto expected = std::vector{0, 22, 44, 55, 77, 11, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_top(std::vector<std::size_t>{0, 8});
        expected = std::vector{0, 88, 11, 22, 33, 44, 55, 66, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_top(std::vector<std::size_t>{8, 0});
        expected = std::vector{0, 88, 11, 22, 33, 44, 55, 66, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_top(std::vector<std::size_t>{0, 1});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_top(std::vector<std::size_t>{1, 0});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, move_top_l)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.move_selections_top();
        auto expected = std::vector{0, 22, 44, 55, 77, 11, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 8});
        obj()->ints.move_selections_top();
        expected = std::vector{0, 88, 11, 22, 33, 44, 55, 66, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{8, 0});
        obj()->ints.move_selections_top();
        expected = std::vector{0, 88, 11, 22, 33, 44, 55, 66, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 1});
        obj()->ints.move_selections_top();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{1, 0});
        obj()->ints.move_selections_top();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, move_down)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_down(4);
        auto expected = std::vector{0, 11, 22, 33, 55, 44, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);

        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_down(0);
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);

        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_down(8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, move_down_n)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_down(std::vector<std::size_t>{3, 4, 6});
        auto expected = std::vector{0, 11, 22, 55, 33, 44, 77, 66, 88};
        EXPECT_EQ(expected, obj->ints);

        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_down(std::vector<std::size_t>{6, 4, 3});
        EXPECT_EQ(expected, obj->ints);

        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_down(std::vector<std::size_t>{0, 8});
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_down(std::vector<std::size_t>{7, 8});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_down(std::vector<std::size_t>{8, 7});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, move_down_l)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{3, 4, 6});
        obj()->ints.move_selections_down();
        auto expected = std::vector{0, 11, 22, 55, 33, 44, 77, 66, 88};
        EXPECT_EQ(expected, obj->ints);

        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{6, 4, 3});
        obj()->ints.move_selections_down();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 8});
        obj()->ints.move_selections_down();
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{7, 8});
        obj()->ints.move_selections_down();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{8, 7});
        obj()->ints.move_selections_down();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, move_bottom)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_bottom(4);
        auto expected = std::vector{0, 11, 22, 33, 55, 66, 77, 88, 44};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_bottom(0);
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 88, 0};
        EXPECT_EQ(expected, obj->ints);

        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_bottom(8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, move_bottom_n)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_bottom(std::vector<std::size_t>{0, 2, 4, 5, 7});
        auto expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_bottom(std::vector<std::size_t>{0, 8});
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 0, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_bottom(std::vector<std::size_t>{8, 0});
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 0, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_bottom(std::vector<std::size_t>{7, 8});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_bottom(std::vector<std::size_t>{8, 7});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, move_bottom_l)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.move_selections_bottom();
        auto expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);

        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 8});
        obj()->ints.move_selections_bottom();
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 0, 88};
        EXPECT_EQ(expected, obj->ints);

        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{8, 0});
        obj()->ints.move_selections_bottom();
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 0, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{7, 8});
        obj()->ints.move_selections_bottom();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{8, 7});
        obj()->ints.move_selections_bottom();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, move_to)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_to(4, 6);
        auto expected = std::vector{0, 11, 22, 33, 55, 66, 44, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_to(0, 0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_to(8, 8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_to(0, 1);
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_to(1, 0);
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_to(8, 7);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 88, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_to(7, 8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 88, 77};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, move_to_n)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 0);
        auto expected = std::vector{0, 22, 44, 55, 77, 11, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 1);
        expected = std::vector{11, 0, 22, 44, 55, 77, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 2);
        expected = std::vector{11, 33, 0, 22, 44, 55, 77, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 3);
        expected = std::vector{11, 33, 66, 0, 22, 44, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 4);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 5);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 6);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 7);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 8);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_to(std::vector<std::size_t>{0, 1}, 0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_to(std::vector<std::size_t>{1, 0}, 0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_to(std::vector<std::size_t>{0, 1, 3}, 0);
        expected = std::vector{0, 11, 33, 22, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_to(std::vector<std::size_t>{3, 1, 0}, 0);
        expected = std::vector{0, 11, 33, 22, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_to(std::vector<std::size_t>{7, 8}, 8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_to(std::vector<std::size_t>{8, 7}, 8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_to(std::vector<std::size_t>{5, 7, 8}, 8);
        expected = std::vector{0, 11, 22, 33, 44, 66, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.move_to(std::vector<std::size_t>{8, 7, 5}, 8);
        expected = std::vector{0, 11, 22, 33, 44, 66, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_do, move_to_l)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.move_selections_to(0);
        auto expected = std::vector{0, 22, 44, 55, 77, 11, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.move_selections_to(1);
        expected = std::vector{11, 0, 22, 44, 55, 77, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.move_selections_to(2);
        expected = std::vector{11, 33, 0, 22, 44, 55, 77, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.move_selections_to(3);
        expected = std::vector{11, 33, 66, 0, 22, 44, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.move_selections_to(4);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.move_selections_to(5);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.move_selections_to(6);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.move_selections_to(7);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.move_selections_to(8);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 1});
        obj()->ints.move_selections_to(0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{1, 0});
        obj()->ints.move_selections_to(0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 1, 3});
        obj()->ints.move_selections_to(0);
        expected = std::vector{0, 11, 33, 22, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{3, 1, 0});
        obj()->ints.move_selections_to(0);
        expected = std::vector{0, 11, 33, 22, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{7, 8});
        obj()->ints.move_selections_to(8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{8, 7});
        obj()->ints.move_selections_to(8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{5, 7, 8});
        obj()->ints.move_selections_to(8);
        expected = std::vector{0, 11, 22, 33, 44, 66, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{8, 7, 5});
        obj()->ints.move_selections_to(8);
        expected = std::vector{0, 11, 22, 33, 44, 66, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
    }



    TEST(op_undo_redo, reset)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector<int>{0, 1, 2, 3, 4};
        obj()->ints = values;
        EXPECT_EQ(5, obj->ints.size());
        obj()->ints.reset();
        EXPECT_EQ(0, obj->ints.size());

        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(0, obj->ints.size());
    }

    TEST(op_undo_redo, reserve)
    {
        Track_do_op obj {};
        std::size_t capacity = obj->ints.capacity();
        obj()->ints.reserve(capacity+1);
        EXPECT_EQ(capacity+1, obj->ints.capacity());

        obj.undo_action(); // Undo is a trim/shrink_to_fit
        EXPECT_EQ(0, obj->ints.capacity());
        obj.redo_action();
        EXPECT_EQ(capacity+1, obj->ints.capacity());
    }

    TEST(op_undo_redo, trim)
    {
        Track_do_op obj {};
        std::size_t capacity = obj->ints.capacity();
        obj()->ints.reserve(capacity+1);
        EXPECT_EQ(capacity+1, obj->ints.capacity());
        obj()->ints.trim();
        EXPECT_EQ(0, obj->ints.capacity());

        obj.undo_action(); // Undo is a no-op
        EXPECT_EQ(0, obj->ints.capacity());
        obj.redo_action(); // Trimming again after a no-op undo results in no change
        EXPECT_EQ(0, obj->ints.capacity());
    }

    TEST(op_undo_redo, assign)
    {
        Track_do_op obj {};
        obj()->ints = std::vector<int>{3, 4, 5};
        EXPECT_EQ(3, obj->ints.size());
        obj()->ints.assign(5, 3);
        EXPECT_EQ(5, obj->ints.size());
        for ( auto i : obj->ints )
            EXPECT_EQ(3, i);

        obj.undo_action();
        EXPECT_EQ(3, obj->ints.size());
        obj.redo_action();
        EXPECT_EQ(5, obj->ints.size());
        for ( auto i : obj->ints )
            EXPECT_EQ(3, i);
    }

    TEST(op_undo_redo, assign_default)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        obj()->ints.assign_default(10);
        EXPECT_EQ(10, obj->ints.size());
        
        obj.undo_action();
        EXPECT_EQ(0, obj->ints.size());
        obj.redo_action();
        EXPECT_EQ(10, obj->ints.size());
    }

    TEST(op_undo_redo, clear_selection)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 1};
        EXPECT_EQ(2, obj->ints.size());
        EXPECT_EQ(0, obj.view.ints.sel().size());
        obj()->ints.select(1);
        EXPECT_EQ(std::vector<std::size_t>{1}, obj.view.ints.sel());
        obj()->ints.clear_selections();
        EXPECT_EQ(0, obj.view.ints.sel().size());

        obj.undo_action();
        EXPECT_EQ(std::vector<std::size_t>{1}, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(0, obj.view.ints.sel().size());
    }

    TEST(op_undo_redo, select_all)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{22, 33};
        EXPECT_EQ(2, obj->ints.size());
        EXPECT_EQ(0, obj.view.ints.sel().size());
        obj()->ints.select_all();
        const auto expected = std::vector<std::size_t>{0, 1};
        EXPECT_EQ(expected, obj.view.ints.sel());

        obj.undo_action();
        EXPECT_EQ(0, obj.view.ints.sel().size());
        obj.redo_action();
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(op_undo_redo, select)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{22, 33};
        EXPECT_EQ(2, obj->ints.size());
        EXPECT_EQ(0, obj.view.ints.sel().size());
        obj()->ints.select(1);
        EXPECT_EQ(std::vector<std::size_t>{1}, obj.view.ints.sel());

        obj.undo_action();
        EXPECT_EQ(0, obj.view.ints.sel().size());
        obj.redo_action();
        EXPECT_EQ(std::vector<std::size_t>{1}, obj.view.ints.sel());
    }

    TEST(op_undo_redo, select_n)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        EXPECT_EQ(5, obj->ints.size());
        EXPECT_EQ(0, obj.view.ints.sel().size());
        const auto selection = std::vector<std::size_t>{2, 4};
        obj()->ints.select(selection);
        EXPECT_EQ(selection, obj.view.ints.sel());
        const auto additional = std::vector<std::size_t>{0, 1};
        obj()->ints.select(additional);
        const auto expected = std::vector<std::size_t>{2, 4, 0, 1};
        EXPECT_EQ(expected, obj.view.ints.sel());

        obj.undo_action();
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(op_undo_redo, deselect)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        const auto selection = std::vector<std::size_t>{2, 4, 0, 1};
        obj()->ints.select(selection);
        EXPECT_EQ(5, obj->ints.size());
        EXPECT_EQ(4, obj.view.ints.sel().size());
        obj()->ints.deselect(4);
        const auto expected = std::vector<std::size_t>{2, 0, 1};
        EXPECT_EQ(expected, obj.view.ints.sel());

        obj.undo_action();
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(op_undo_redo, deselect_n)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        const auto selection = std::vector<std::size_t>{2, 4, 0, 1};
        obj()->ints.select(selection);
        EXPECT_EQ(5, obj->ints.size());
        EXPECT_EQ(4, obj.view.ints.sel().size());
        obj()->ints.deselect(std::vector<std::size_t>{4, 2});
        const auto expected = std::vector<std::size_t>{0, 1};
        EXPECT_EQ(expected, obj.view.ints.sel());

        obj.undo_action();
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(op_undo_redo, toggle_selection)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        EXPECT_EQ(5, obj->ints.size());
        obj()->ints.toggle_selected(2);
        obj()->ints.toggle_selected(4);
        obj()->ints.toggle_selected(0);
        obj()->ints.toggle_selected(3);
        EXPECT_EQ(4, obj.view.ints.sel().size());
        auto expected = std::vector<std::size_t>{2, 4, 0, 3};
        EXPECT_EQ(expected, obj.view.ints.sel());
        obj()->ints.toggle_selected(4);
        obj()->ints.toggle_selected(2);
        EXPECT_EQ(2, obj.view.ints.sel().size());
        expected = {0, 3};
        EXPECT_EQ(expected, obj.view.ints.sel());
        
        obj.undo_action();
        obj.undo_action();
        expected = std::vector<std::size_t>{2, 4, 0, 3};
        EXPECT_EQ(expected, obj.view.ints.sel());
        obj.redo_action();
        obj.redo_action();
        expected = std::vector<std::size_t>{0, 3};
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(op_undo_redo, toggle_selection_n)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        EXPECT_EQ(5, obj->ints.size());
        obj()->ints.toggle_selected(std::vector<std::size_t>{2, 4, 0, 3});
        EXPECT_EQ(4, obj.view.ints.sel().size());
        auto expected = std::vector<std::size_t>{2, 4, 0, 3};
        EXPECT_EQ(expected, obj.view.ints.sel());
        obj()->ints.toggle_selected(std::vector<std::size_t>{4, 2});
        EXPECT_EQ(2, obj.view.ints.sel().size());
        expected = {0, 3};
        EXPECT_EQ(expected, obj.view.ints.sel());

        obj.undo_action();
        expected = std::vector<std::size_t>{2, 4, 0, 3};
        EXPECT_EQ(expected, obj.view.ints.sel());

        obj.redo_action();
        expected = std::vector<std::size_t>{0, 3};
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(op_undo_redo, sort_selections)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        EXPECT_EQ(5, obj->ints.size());
        auto selection = std::vector<std::size_t>{2, 4, 0, 3};
        obj()->ints.select(selection);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj()->ints.sort_selection();
        const auto expected = std::vector<std::size_t>{0, 2, 3, 4};
        EXPECT_EQ(expected, obj.view.ints.sel());

        obj.undo_action();
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(op_undo_redo, sort_selections_desc)
    {
        Track_do_op obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        EXPECT_EQ(5, obj->ints.size());
        auto selection = std::vector<std::size_t>{2, 4, 0, 3};
        obj()->ints.select(selection);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj()->ints.sort_selection_descending();
        const auto expected = std::vector<std::size_t>{4, 3, 2, 0};
        EXPECT_EQ(expected, obj.view.ints.sel());

        obj.undo_action();
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(op_undo_redo, set)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 1, 2, 3, 4};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        obj()->ints[2] = 22;
        const auto expected = std::vector{0, 1, 22, 3, 4};
        EXPECT_EQ(expected, obj->ints);

        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_undo_redo, set_n)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 1, 2, 3, 4, 5};
        obj()->ints = values;
        EXPECT_EQ(6, obj->ints.size());
        obj()->ints.set(std::vector<std::size_t>{1, 3, 5}, 33);
        const auto expected = std::vector{0, 33, 2, 33, 4, 33};
        EXPECT_EQ(expected, obj->ints);

        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_undo_redo, set_l)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 1, 2, 3, 4, 5};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        obj()->ints.select(std::vector<std::size_t>{1, 3, 5});
        obj()->ints.selection() = 33;
        const auto expected = std::vector{0, 33, 2, 33, 4, 33};
        EXPECT_EQ(expected, obj->ints);

        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
    }
    
    TEST(op_undo_redo, plus_eq_)
    {
        Track_do_op obj {};
        obj()->ints.append(2);
        EXPECT_EQ(obj->ints, std::vector{2});
        obj()->ints[0] += 3;
        EXPECT_EQ(obj->ints, std::vector{5});

        obj.undo_action();
        EXPECT_EQ(obj->ints, std::vector{2});
        obj.redo_action();
        EXPECT_EQ(obj->ints, std::vector{5});
    }

    TEST(op_undo_redo, minus_eq_)
    {
        Track_do_op obj {};
        obj()->ints.append(7);
        EXPECT_EQ(obj->ints, std::vector{7});
        obj()->ints[0] -= 2;
        EXPECT_EQ(obj->ints, std::vector{5});

        obj.undo_action();
        EXPECT_EQ(obj->ints, std::vector{7});
        obj.redo_action();
        EXPECT_EQ(obj->ints, std::vector{5});
    }

    TEST(op_undo_redo, mult_eq_)
    {
        Track_do_op obj {};
        obj()->ints.append(2);
        EXPECT_EQ(obj->ints, std::vector{2});
        obj()->ints[0] *= 2;
        EXPECT_EQ(obj->ints, std::vector{4});

        obj.undo_action();
        EXPECT_EQ(obj->ints, std::vector{2});
        obj.redo_action();
        EXPECT_EQ(obj->ints, std::vector{4});
    }

    TEST(op_undo_redo, div_eq_)
    {
        Track_do_op obj {};
        obj()->ints.append(4);
        EXPECT_EQ(obj->ints, std::vector{4});
        obj()->ints[0] /= 2;
        EXPECT_EQ(obj->ints, std::vector{2});

        obj.undo_action();
        EXPECT_EQ(obj->ints, std::vector{4});
        obj.redo_action();
        EXPECT_EQ(obj->ints, std::vector{2});
    }

    TEST(op_undo_redo, mod_eq_)
    {
        Track_do_op obj {};
        obj()->ints.append(5);
        EXPECT_EQ(obj->ints, std::vector{5});
        obj()->ints[0] %= 2;
        EXPECT_EQ(obj->ints, std::vector{1});

        obj.undo_action();
        EXPECT_EQ(obj->ints, std::vector{5});
        obj.redo_action();
        EXPECT_EQ(obj->ints, std::vector{1});
    }

    TEST(op_undo_redo, xor_eq_)
    {
        Track_do_op obj {};
        obj()->ints.append(0b01);
        EXPECT_EQ(obj->ints, std::vector{0b01});
        obj()->ints[0] ^= 0b11;
        EXPECT_EQ(obj->ints, std::vector{0b10});

        obj.undo_action();
        EXPECT_EQ(obj->ints, std::vector{0b01});
        obj.redo_action();
        EXPECT_EQ(obj->ints, std::vector{0b10});
    }

    TEST(op_undo_redo, and_eq_)
    {
        Track_do_op obj {};
        obj()->ints.append(0b101);
        EXPECT_EQ(obj->ints, std::vector{0b101});
        obj()->ints[0] &= 0b011;
        EXPECT_EQ(obj->ints, std::vector{0b001});

        obj.undo_action();
        EXPECT_EQ(obj->ints, std::vector{0b101});
        obj.redo_action();
        EXPECT_EQ(obj->ints, std::vector{0b001});
    }

    TEST(op_undo_redo, or_eq_)
    {
        Track_do_op obj {};
        obj()->ints.append(0b00);
        EXPECT_EQ(obj->ints, std::vector{0b00});
        obj()->ints[0] |= 0b11;
        EXPECT_EQ(obj->ints, std::vector{0b11});

        obj.undo_action();
        EXPECT_EQ(obj->ints, std::vector{0b00});
        obj.redo_action();
        EXPECT_EQ(obj->ints, std::vector{0b11});
    }

    TEST(op_undo_redo, append)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        obj()->ints.append(12);
        EXPECT_EQ(std::vector{12}, obj->ints);
        obj()->ints.append(13);
        auto expected = std::vector{12, 13};
        EXPECT_EQ(expected, obj->ints);

        obj.undo_action();
        EXPECT_EQ(std::vector{12}, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_undo_redo, append_n)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{12, 13};
        obj()->ints.append(values);
        EXPECT_EQ(values, obj->ints);

        obj.undo_action();
        EXPECT_EQ(0, obj->ints.size());
        obj.redo_action();
        EXPECT_EQ(values, obj->ints);
    }

    TEST(op_undo_redo, insert)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        obj()->ints.insert(4, 11);
        auto first_expected = std::vector{0, 1, 2, 3, 11, 4};
        EXPECT_EQ(first_expected, obj->ints);
        obj()->ints.insert(1, 22);
        auto second_expected = std::vector{0, 22, 1, 2, 3, 11, 4};
        EXPECT_EQ(second_expected, obj->ints);

        obj.undo_action();
        EXPECT_EQ(first_expected, obj->ints);
        obj.redo_action();
        EXPECT_EQ(second_expected, obj->ints);
    }

    TEST(op_undo_redo, insert_n)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{0, 1, 2, 3, 4};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        obj()->ints.insert(2, std::vector{11, 12, 13});
        auto expected = std::vector{0, 1, 11, 12, 13, 2, 3, 4};
        EXPECT_EQ(expected, obj->ints);

        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_undo_redo, remove)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 11, 22, 33, 44};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        obj()->ints.remove(2);
        const auto expected = std::vector{0, 11, 33, 44};
        EXPECT_EQ(expected, obj->ints);

        obj.undo_action();
        EXPECT_EQ(values, obj->ints);

        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_undo_redo, remove_n)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{0, 11, 22, 33, 44};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        const auto selection = std::vector<std::size_t>{4, 2};
        obj()->ints.select(selection);
        obj()->ints.remove(std::vector{0, 2, 4});
        auto expected = std::vector{11, 33};
        EXPECT_EQ(expected, obj->ints);

        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(0, obj.view.ints.sel().size());
    }

    TEST(op_undo_redo, remove_l)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 11, 22, 33, 44};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        const auto selection = std::vector<std::size_t>{1, 3};
        obj()->ints.select(selection);
        obj()->ints.remove_selection();
        const auto expected = std::vector{0, 22, 44};
        EXPECT_EQ(expected, obj->ints);

        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());

        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(0, obj.view.ints.sel().size());
    }

    TEST(op_undo_redo, sort)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{1, 9, 2, 8, 3, 7, 11, 5, 6, 4, 22};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        obj()->ints.sort();
        const auto expected = std::vector{1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 22};
        EXPECT_EQ(expected, obj->ints);

        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_undo_redo, sort_desc)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{1, 9, 2, 8, 3, 7, 11, 5, 6, 4, 22};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        obj()->ints.sort_desc();
        const auto expected = std::vector{22, 11, 9, 8, 7, 6, 5, 4, 3, 2, 1};
        EXPECT_EQ(expected, obj->ints);

        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_undo_redo, swap)
    {
        Track_do_op obj {};
        const auto first = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = first;
        EXPECT_EQ(first, obj->ints);

        obj()->ints.swap(0, 8);
        const auto second = std::vector{88, 11, 22, 33, 44, 55, 66, 77, 0};
        EXPECT_EQ(second, obj->ints);

        obj.undo_action();
        EXPECT_EQ(first, obj->ints);
        obj.redo_action();
        EXPECT_EQ(second, obj->ints);
    }

    TEST(op_undo_redo, move_up)
    {
        Track_do_op obj {};
        const auto first = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = first;
        EXPECT_EQ(first, obj->ints);

        obj()->ints.move_up(4);
        const auto second = std::vector{0, 11, 22, 44, 33, 55, 66, 77, 88};
        EXPECT_EQ(second, obj->ints);

        obj()->ints.move_up(0);
        EXPECT_EQ(second, obj->ints);

        obj()->ints.move_up(8);
        const auto third = std::vector{0, 11, 22, 44, 33, 55, 66, 88, 77};
        EXPECT_EQ(third, obj->ints);

        obj.undo_action(); // Undo move_up(8)
        EXPECT_EQ(second, obj->ints);
        obj.undo_action(); // Undo move_up(0)
        EXPECT_EQ(second, obj->ints);
        obj.undo_action(); // Undo move_up(4)
        EXPECT_EQ(first, obj->ints);
        obj.redo_action();
        EXPECT_EQ(second, obj->ints);
        obj.redo_action();
        EXPECT_EQ(second, obj->ints);
        obj.redo_action();
        EXPECT_EQ(third, obj->ints);
    }

    TEST(op_undo_redo, move_up_n)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.move_up(std::vector<std::size_t>{3, 4, 6});
        auto expected = std::vector{0, 11, 33, 44, 22, 66, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);

        obj()->ints = values;
        obj()->ints.move_up(std::vector<std::size_t>{6, 4, 3});
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);

        obj()->ints = values;
        obj()->ints.move_up(std::vector<std::size_t>{0, 8});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 88, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_up(std::vector<std::size_t>{0, 1});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_up(std::vector<std::size_t>{1, 0});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_undo_redo, move_up_l)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{3, 4, 6});
        obj()->ints.move_selections_up();
        auto expected = std::vector{0, 11, 33, 44, 22, 66, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);

        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{6, 4, 3});
        obj()->ints.move_selections_up();
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 8});
        obj()->ints.move_selections_up();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 88, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 1});
        obj()->ints.move_selections_up();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{1, 0});
        obj()->ints.move_selections_up();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_undo_redo, move_top)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.move_top(4);
        auto expected = std::vector{44, 0, 11, 22, 33, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_top(0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_top(8);
        expected = std::vector{88, 0, 11, 22, 33, 44, 55, 66, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_undo_redo, move_top_n)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.move_top(std::vector<std::size_t>{0, 2, 4, 5, 7});
        auto expected = std::vector{0, 22, 44, 55, 77, 11, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_top(std::vector<std::size_t>{0, 1, 2, 4, 5, 7});
        expected = std::vector{0, 11, 22, 44, 55, 77, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_top(std::vector<std::size_t>{0, 8});
        expected = std::vector{0, 88, 11, 22, 33, 44, 55, 66, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_top(std::vector<std::size_t>{8, 0});
        expected = std::vector{0, 88, 11, 22, 33, 44, 55, 66, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_top(std::vector<std::size_t>{0, 1});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_top(std::vector<std::size_t>{1, 0});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_undo_redo, move_top_l)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.move_selections_top();
        auto expected = std::vector{0, 22, 44, 55, 77, 11, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 8});
        obj()->ints.move_selections_top();
        expected = std::vector{0, 88, 11, 22, 33, 44, 55, 66, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{8, 0});
        obj()->ints.move_selections_top();
        expected = std::vector{0, 88, 11, 22, 33, 44, 55, 66, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 1});
        obj()->ints.move_selections_top();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{1, 0});
        obj()->ints.move_selections_top();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_undo_redo, move_down)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.move_down(4);
        auto expected = std::vector{0, 11, 22, 33, 55, 44, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_down(0);
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_down(8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_undo_redo, move_down_n)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.move_down(std::vector<std::size_t>{3, 4, 6});
        auto expected = std::vector{0, 11, 22, 55, 33, 44, 77, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_down(std::vector<std::size_t>{6, 4, 3});
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_down(std::vector<std::size_t>{0, 8});
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_down(std::vector<std::size_t>{7, 8});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_down(std::vector<std::size_t>{8, 7});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_undo_redo, move_down_l)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{3, 4, 6});
        obj()->ints.move_selections_down();
        auto expected = std::vector{0, 11, 22, 55, 33, 44, 77, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);

        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{6, 4, 3});
        obj()->ints.move_selections_down();
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 8});
        obj()->ints.move_selections_down();
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{7, 8});
        obj()->ints.move_selections_down();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{8, 7});
        obj()->ints.move_selections_down();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_undo_redo, move_bottom)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.move_bottom(4);
        auto expected = std::vector{0, 11, 22, 33, 55, 66, 77, 88, 44};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_bottom(0);
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 88, 0};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_bottom(8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_undo_redo, move_bottom_n)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.move_bottom(std::vector<std::size_t>{0, 2, 4, 5, 7});
        auto expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_bottom(std::vector<std::size_t>{0, 8});
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 0, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_bottom(std::vector<std::size_t>{8, 0});
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 0, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_bottom(std::vector<std::size_t>{7, 8});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_bottom(std::vector<std::size_t>{8, 7});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_undo_redo, move_bottom_l)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.move_selections_bottom();
        auto expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);

        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 8});
        obj()->ints.move_selections_bottom();
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 0, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);

        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{8, 0});
        obj()->ints.move_selections_bottom();
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 0, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{7, 8});
        obj()->ints.move_selections_bottom();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{8, 7});
        obj()->ints.move_selections_bottom();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_undo_redo, move_to)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.move_to(4, 6);
        auto expected = std::vector{0, 11, 22, 33, 55, 66, 44, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_to(0, 0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_to(8, 8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_to(0, 1);
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_to(1, 0);
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_to(8, 7);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 88, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_to(7, 8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 88, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_undo_redo, move_to_n)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 0);
        auto expected = std::vector{0, 22, 44, 55, 77, 11, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 1);
        expected = std::vector{11, 0, 22, 44, 55, 77, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 2);
        expected = std::vector{11, 33, 0, 22, 44, 55, 77, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 3);
        expected = std::vector{11, 33, 66, 0, 22, 44, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 4);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 5);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 6);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 7);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 8);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_to(std::vector<std::size_t>{0, 1}, 0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_to(std::vector<std::size_t>{1, 0}, 0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_to(std::vector<std::size_t>{0, 1, 3}, 0);
        expected = std::vector{0, 11, 33, 22, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_to(std::vector<std::size_t>{3, 1, 0}, 0);
        expected = std::vector{0, 11, 33, 22, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_to(std::vector<std::size_t>{7, 8}, 8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_to(std::vector<std::size_t>{8, 7}, 8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_to(std::vector<std::size_t>{5, 7, 8}, 8);
        expected = std::vector{0, 11, 22, 33, 44, 66, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.move_to(std::vector<std::size_t>{8, 7, 5}, 8);
        expected = std::vector{0, 11, 22, 33, 44, 66, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(op_undo_redo, move_to_l)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.move_selections_to(0);
        auto expected = std::vector{0, 22, 44, 55, 77, 11, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.move_selections_to(1);
        expected = std::vector{11, 0, 22, 44, 55, 77, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.move_selections_to(2);
        expected = std::vector{11, 33, 0, 22, 44, 55, 77, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.move_selections_to(3);
        expected = std::vector{11, 33, 66, 0, 22, 44, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.move_selections_to(4);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.move_selections_to(5);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.move_selections_to(6);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.move_selections_to(7);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.move_selections_to(8);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 1});
        obj()->ints.move_selections_to(0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{1, 0});
        obj()->ints.move_selections_to(0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 1, 3});
        obj()->ints.move_selections_to(0);
        expected = std::vector{0, 11, 33, 22, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{3, 1, 0});
        obj()->ints.move_selections_to(0);
        expected = std::vector{0, 11, 33, 22, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{7, 8});
        obj()->ints.move_selections_to(8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{8, 7});
        obj()->ints.move_selections_to(8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{5, 7, 8});
        obj()->ints.move_selections_to(8);
        expected = std::vector{0, 11, 22, 33, 44, 66, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{8, 7, 5});
        obj()->ints.move_selections_to(8);
        expected = std::vector{0, 11, 22, 33, 44, 66, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
    }



    TEST(op_sel_sync, reset)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector<int>{0, 1, 2, 3, 4};
        obj()->ints = values;
        auto selections = std::vector<std::size_t>{2, 4, 0};
        obj()->ints.select(selections);
        EXPECT_EQ(5, obj->ints.size());
        obj()->ints.reset();
        EXPECT_EQ(0, obj->ints.size());
        EXPECT_EQ(0, obj.view.ints.sel().size());

        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(0, obj->ints.size());
        EXPECT_EQ(0, obj.view.ints.sel().size());
    }

    TEST(op_sel_sync, assign)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 1, 2, 3, 4};
        obj()->ints = values;
        auto selections = std::vector<std::size_t>{2, 4, 0};
        obj()->ints.select(selections);
        obj()->ints.assign(5, 3);
        EXPECT_EQ(5, obj->ints.size());
        for ( auto i : obj->ints )
            EXPECT_EQ(3, i);
        EXPECT_EQ(0, obj.view.ints.sel().size());

        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(5, obj->ints.size());
        for ( auto i : obj->ints )
            EXPECT_EQ(3, i);
        EXPECT_EQ(0, obj.view.ints.sel().size());
    }

    TEST(op_sel_sync, assign_default)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 1, 2, 3, 4};
        obj()->ints = values;
        auto selections = std::vector<std::size_t>{2, 4, 0};
        obj()->ints.select(selections);
        obj()->ints.assign_default(10);
        EXPECT_EQ(10, obj->ints.size());
        EXPECT_EQ(0, obj.view.ints.sel().size());
        
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(10, obj->ints.size());
        EXPECT_EQ(0, obj.view.ints.sel().size());
    }

    TEST(op_sel_sync, set)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 1, 2, 3, 4};
        obj()->ints = values;
        const auto selections = std::vector<std::size_t>{0, 2, 4};
        obj()->ints.select(selections);
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        const auto new_values = std::vector{0, 1, 2, 3};
        obj()->ints = new_values;
        EXPECT_EQ(new_values, obj->ints);
        EXPECT_EQ(0, obj.view.ints.sel().size());

        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(new_values, obj->ints);
        EXPECT_EQ(0, obj.view.ints.sel().size());
    }

    TEST(op_sel_sync, insert)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        const auto selection = std::vector<std::size_t>{0, 3, 4};
        obj()->ints.select(selection);
        obj()->ints.insert(4, 11);
        const auto first_expected = std::vector{0, 1, 2, 3, 11, 4};
        const auto first_expected_selection = std::vector<std::size_t>{0, 3, 5};
        EXPECT_EQ(first_expected, obj->ints);
        EXPECT_EQ(first_expected_selection, obj.view.ints.sel());
        obj()->ints.insert(1, 22);
        const auto second_expected = std::vector{0, 22, 1, 2, 3, 11, 4};
        const auto second_expected_selection = std::vector<std::size_t>{0, 4, 6};
        EXPECT_EQ(second_expected, obj->ints);
        EXPECT_EQ(second_expected_selection, obj.view.ints.sel());

        obj.undo_action();
        EXPECT_EQ(first_expected, obj->ints);
        EXPECT_EQ(first_expected_selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(second_expected, obj->ints);
        EXPECT_EQ(second_expected_selection, obj.view.ints.sel());
    }

    TEST(op_sel_sync, insert_n)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{0, 1, 2, 3, 4};
        obj()->ints = values;
        const auto selection = std::vector<std::size_t>{0, 2, 4};
        obj()->ints.select(selection);
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj()->ints.insert(2, std::vector{11, 12, 13});
        auto expected = std::vector{0, 1, 11, 12, 13, 2, 3, 4};
        auto expected_selection = std::vector<std::size_t>{0, 5, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_selection, obj.view.ints.sel());

        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_selection, obj.view.ints.sel());
    }

    TEST(op_sel_sync, remove)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 11, 22, 33, 44};
        obj()->ints = values;
        const auto selection = std::vector<std::size_t>{0, 2, 4};
        obj()->ints.select(selection);
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj()->ints.remove(2);
        const auto expected = std::vector{0, 11, 33, 44};
        auto expected_selection = std::vector<std::size_t>{0, 3};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_selection, obj.view.ints.sel());

        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());

        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_selection, obj.view.ints.sel());
    }

    TEST(op_sel_sync, remove_n)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{0, 11, 22, 33, 44};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        const auto selection = std::vector<std::size_t>{3, 2};
        obj()->ints.select(selection);
        obj()->ints.remove(std::vector{0, 2, 4});
        auto expected = std::vector{11, 33};
        const auto expected_selection = std::vector<std::size_t>{1};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_selection, obj.view.ints.sel());

        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_selection, obj.view.ints.sel());
    }

    TEST(op_sel_sync, remove_l)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 11, 22, 33, 44};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        const auto selection = std::vector<std::size_t>{1, 3};
        obj()->ints.select(selection);
        obj()->ints.remove_selection();
        const auto expected = std::vector{0, 22, 44};
        EXPECT_EQ(expected, obj->ints);

        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());

        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(0, obj.view.ints.sel().size());
    }

    TEST(op_sel_sync, sort)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{1, 9, 2, 8, 3, 7, 11, 5, 6, 4, 22};
        obj()->ints = values;
        auto selection = std::vector<std::size_t>{0, 2, 4};
        obj()->ints.select(selection);
        EXPECT_EQ(values, obj->ints);
        obj()->ints.sort();
        const auto expected = std::vector{1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 22};
        const auto expected_sel = std::vector<std::size_t>{0, 1, 2};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());

        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());

        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
    }

    TEST(op_sel_sync, sort_desc)
    {
        Track_do_op obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{1, 9, 2, 8, 3, 7, 11, 5, 6, 4, 22};
        obj()->ints = values;
        auto selection = std::vector<std::size_t>{0, 2, 4};
        obj()->ints.select(selection);
        EXPECT_EQ(values, obj->ints);
        obj()->ints.sort_desc();
        const auto expected = std::vector{22, 11, 9, 8, 7, 6, 5, 4, 3, 2, 1};
        const auto expected_sel = std::vector<std::size_t>{8, 9, 10};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());

        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
    }

    TEST(op_sel_sync, swap)
    {
        Track_do_op obj {};
        const auto first = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        const auto first_sel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        obj()->ints = first;
        obj()->ints.select(first_sel);
        EXPECT_EQ(first, obj->ints);
        EXPECT_EQ(first_sel, obj.view.ints.sel());

        obj()->ints.swap(0, 8);
        const auto second = std::vector{88, 11, 22, 33, 44, 55, 66, 77, 0};
        const auto second_sel = std::vector<std::size_t>{8, 1, 3, 4, 7, 0};
        EXPECT_EQ(second, obj->ints);
        EXPECT_EQ(second_sel, obj.view.ints.sel());

        obj.undo_action();
        EXPECT_EQ(first, obj->ints);
        EXPECT_EQ(first_sel, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(second, obj->ints);
        EXPECT_EQ(second_sel, obj.view.ints.sel());
    }

    TEST(op_sel_sync, move_up)
    {
        Track_do_op obj {};
        const auto first = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        const auto first_sel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        obj()->ints = first;
        obj()->ints.select(first_sel);
        EXPECT_EQ(first, obj->ints);
        EXPECT_EQ(first_sel, obj.view.ints.sel());

        obj()->ints.move_up(4);
        const auto second = std::vector{0, 11, 22, 44, 33, 55, 66, 77, 88};
        const auto second_sel = std::vector<std::size_t>{0, 1, 4, 3, 7, 8};
        EXPECT_EQ(second, obj->ints);
        EXPECT_EQ(second_sel, obj.view.ints.sel());

        obj()->ints.move_up(0);
        EXPECT_EQ(second, obj->ints);
        EXPECT_EQ(second_sel, obj.view.ints.sel());

        obj()->ints.move_up(8);
        const auto third = std::vector{0, 11, 22, 44, 33, 55, 66, 88, 77};
        const auto third_sel = std::vector<std::size_t>{0, 1, 4, 3, 8, 7};
        EXPECT_EQ(third, obj->ints);
        EXPECT_EQ(third_sel, obj.view.ints.sel());

        obj.undo_action(); // Undo move_up(8)
        EXPECT_EQ(second, obj->ints);
        EXPECT_EQ(second_sel, obj.view.ints.sel());
        obj.undo_action(); // Undo move_up(0)
        EXPECT_EQ(second, obj->ints);
        EXPECT_EQ(second_sel, obj.view.ints.sel());
        obj.undo_action(); // Undo move_up(4)
        EXPECT_EQ(first, obj->ints);
        EXPECT_EQ(first_sel, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(second, obj->ints);
        EXPECT_EQ(second_sel, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(second, obj->ints);
        EXPECT_EQ(second_sel, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(third, obj->ints);
        EXPECT_EQ(third_sel, obj.view.ints.sel());
    }

    TEST(op_sel_sync, move_up_n)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        const auto selection = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.move_up(std::vector<std::size_t>{3, 4, 6});
        auto expected = std::vector{0, 11, 33, 44, 22, 66, 55, 77, 88};
        auto expected_sel = std::vector<std::size_t>{0, 1, 2, 3, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());

        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.move_up(std::vector<std::size_t>{6, 4, 3});
        expected_sel = std::vector<std::size_t>{0, 1, 2, 3, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());

        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.move_up(std::vector<std::size_t>{0, 8});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 88, 77};
        expected_sel = std::vector<std::size_t>{0, 1, 3, 4, 8, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.move_up(std::vector<std::size_t>{0, 1});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.move_up(std::vector<std::size_t>{1, 0});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
    }

    TEST(op_sel_sync, move_up_l)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        auto selection = std::vector<std::size_t>{3, 4, 6};
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.move_selections_up();
        auto expected = std::vector{0, 11, 33, 44, 22, 66, 55, 77, 88};
        auto expected_sel = std::vector<std::size_t>{2, 3, 5};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());

        obj()->ints.clear_selections();
        obj()->ints = values;
        selection = std::vector<std::size_t>{6, 4, 3};
        obj()->ints.select(selection);
        obj()->ints.move_selections_up();
        expected_sel = std::vector<std::size_t>{5, 3, 2};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selection = std::vector<std::size_t>{0, 8};
        obj()->ints.select(selection);
        obj()->ints.move_selections_up();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 88, 77};
        expected_sel = std::vector<std::size_t>{0, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selection = std::vector<std::size_t>{0, 1};
        obj()->ints.select(selection);
        obj()->ints.move_selections_up();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selection = std::vector<std::size_t>{1, 0};
        obj()->ints.select(selection);
        obj()->ints.move_selections_up();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{1, 0};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
    }

    TEST(op_sel_sync, move_top)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        const auto selection = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.move_top(4);
        auto expected = std::vector{44, 0, 11, 22, 33, 55, 66, 77, 88};
        auto expected_sel = std::vector<std::size_t>{1, 2, 4, 0, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.move_top(0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.move_top(8);
        expected = std::vector{88, 0, 11, 22, 33, 44, 55, 66, 77};
        expected_sel = std::vector<std::size_t>{1, 2, 4, 5, 8, 0};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
    }

    TEST(op_sel_sync, move_top_n)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        const auto selection = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.move_top(std::vector<std::size_t>{0, 2, 4, 5, 7});
        auto expected = std::vector{0, 22, 44, 55, 77, 11, 33, 66, 88};
        auto expected_sel = std::vector<std::size_t>{0, 5, 6, 2, 4, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.move_top(std::vector<std::size_t>{0, 1, 2, 4, 5, 7});
        expected = std::vector{0, 11, 22, 44, 55, 77, 33, 66, 88};
        expected_sel = std::vector<std::size_t>{0, 1, 6, 3, 5, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.move_top(std::vector<std::size_t>{0, 8});
        expected = std::vector{0, 88, 11, 22, 33, 44, 55, 66, 77};
        expected_sel = std::vector<std::size_t>{0, 2, 4, 5, 8, 1};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.move_top(std::vector<std::size_t>{8, 0});
        expected = std::vector{0, 88, 11, 22, 33, 44, 55, 66, 77};
        expected_sel = std::vector<std::size_t>{0, 2, 4, 5, 8, 1};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.move_top(std::vector<std::size_t>{0, 1});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.move_top(std::vector<std::size_t>{1, 0});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
    }

    TEST(op_sel_sync, move_top_l)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        auto selection = std::vector<std::size_t>{0, 2, 4, 5, 7};
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.move_selections_top();
        auto expected = std::vector{0, 22, 44, 55, 77, 11, 33, 66, 88};
        auto expected_sel = std::vector<std::size_t>{0, 1, 2, 3, 4};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selection = std::vector<std::size_t>{0, 8};
        obj()->ints.select(selection);
        obj()->ints.move_selections_top();
        expected = std::vector{0, 88, 11, 22, 33, 44, 55, 66, 77};
        expected_sel = std::vector<std::size_t>{0, 1};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selection = std::vector<std::size_t>{8, 0};
        obj()->ints.select(selection);
        obj()->ints.move_selections_top();
        expected = std::vector{0, 88, 11, 22, 33, 44, 55, 66, 77};
        expected_sel = std::vector<std::size_t>{1, 0};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selection = std::vector<std::size_t>{0, 1};
        obj()->ints.select(selection);
        obj()->ints.move_selections_top();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selection = std::vector<std::size_t>{1, 0};
        obj()->ints.select(selection);
        obj()->ints.move_selections_top();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{1, 0};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
    }

    TEST(op_sel_sync, move_down)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        const auto selection = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.move_down(4);
        auto expected = std::vector{0, 11, 22, 33, 55, 44, 66, 77, 88};
        auto expected_sel = std::vector<std::size_t>{0, 1, 3, 5, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.move_down(0);
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{1, 0, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.move_down(8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
    }

    TEST(op_sel_sync, move_down_n)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        const auto selection = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.move_down(std::vector<std::size_t>{3, 4, 6});
        auto expected = std::vector{0, 11, 22, 55, 33, 44, 77, 66, 88};
        auto expected_sel = std::vector<std::size_t>{0, 1, 4, 5, 6, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.move_down(std::vector<std::size_t>{6, 4, 3});
        expected_sel = std::vector<std::size_t>{0, 1, 4, 5, 6, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.move_down(std::vector<std::size_t>{0, 8});
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{1, 0, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.move_down(std::vector<std::size_t>{7, 8});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.move_down(std::vector<std::size_t>{8, 7});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
    }

    TEST(op_sel_sync, move_down_l)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        auto selection = std::vector<std::size_t>{3, 4, 6};
        obj()->ints.select(selection);
        obj()->ints.move_selections_down();
        auto expected = std::vector{0, 11, 22, 55, 33, 44, 77, 66, 88};
        auto expected_sel = std::vector<std::size_t>{4, 5, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());

        obj()->ints.clear_selections();
        obj()->ints = values;
        selection = std::vector<std::size_t>{6, 4, 3};
        obj()->ints.select(selection);
        obj()->ints.move_selections_down();
        expected_sel = std::vector<std::size_t>{7, 5, 4};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selection = std::vector<std::size_t>{0, 8};
        obj()->ints.select(selection);
        obj()->ints.move_selections_down();
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{1, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selection = std::vector<std::size_t>{7, 8};
        obj()->ints.select(selection);
        obj()->ints.move_selections_down();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selection = std::vector<std::size_t>{8, 7};
        obj()->ints.select(selection);
        obj()->ints.move_selections_down();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{8, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
    }

    TEST(op_sel_sync, move_bottom)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        const auto selections = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_bottom(4);
        auto expected = std::vector{0, 11, 22, 33, 55, 66, 77, 88, 44};
        auto expected_sel = std::vector<std::size_t>{0, 1, 3, 8, 6, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_bottom(0);
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 88, 0};
        expected_sel = std::vector<std::size_t>{8, 0, 2, 3, 6, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_bottom(8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
    }

    TEST(op_sel_sync, move_bottom_n)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        const auto selections = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_bottom(std::vector<std::size_t>{0, 2, 4, 5, 7});
        auto expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        auto expected_sel = std::vector<std::size_t>{4, 0, 1, 6, 8, 3};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_bottom(std::vector<std::size_t>{0, 8});
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 0, 88};
        expected_sel = std::vector<std::size_t>{7, 0, 2, 3, 6, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_bottom(std::vector<std::size_t>{8, 0});
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 0, 88};
        expected_sel = std::vector<std::size_t>{7, 0, 2, 3, 6, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_bottom(std::vector<std::size_t>{7, 8});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_bottom(std::vector<std::size_t>{8, 7});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
    }

    TEST(op_sel_sync, move_bottom_l)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        auto selections = std::vector<std::size_t>{0, 2, 4, 5, 7};
        obj()->ints.select(selections);
        obj()->ints.move_selections_bottom();
        auto expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        auto expected_sel = std::vector<std::size_t>{4, 5, 6, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());

        obj()->ints.clear_selections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{0, 8};
        obj()->ints.select(selections);
        obj()->ints.move_selections_bottom();
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 0, 88};
        expected_sel = std::vector<std::size_t>{7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());

        obj()->ints.clear_selections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{8, 0};
        obj()->ints.select(selections);
        obj()->ints.move_selections_bottom();
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 0, 88};
        expected_sel = std::vector<std::size_t>{8, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{7, 8};
        obj()->ints.select(selections);
        obj()->ints.move_selections_bottom();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{8, 7};
        obj()->ints.select(selections);
        obj()->ints.move_selections_bottom();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{8, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
    }

    TEST(op_sel_sync, move_to)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        const auto selections = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_to(4, 6);
        auto expected = std::vector{0, 11, 22, 33, 55, 66, 44, 77, 88};
        auto expected_sel = std::vector<std::size_t>{0, 1, 3, 6, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_to(0, 0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_to(8, 8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_to(0, 1);
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{1, 0, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_to(1, 0);
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{1, 0, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_to(8, 7);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 88, 77};
        expected_sel = std::vector<std::size_t>{0, 1, 3, 4, 8, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_to(7, 8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 88, 77};
        expected_sel = std::vector<std::size_t>{0, 1, 3, 4, 8, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
    }

    TEST(op_sel_sync, move_to_n)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        const auto selections = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 0);
        auto expected = std::vector{0, 22, 44, 55, 77, 11, 33, 66, 88};
        auto expected_sel = std::vector<std::size_t>{0, 5, 6, 2, 4, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 1);
        expected = std::vector{11, 0, 22, 44, 55, 77, 33, 66, 88};
        expected_sel = std::vector<std::size_t>{1, 0, 6, 3, 5, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 2);
        expected = std::vector{11, 33, 0, 22, 44, 55, 77, 66, 88};
        expected_sel = std::vector<std::size_t>{2, 0, 1, 4, 6, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 3);
        expected = std::vector{11, 33, 66, 0, 22, 44, 55, 77, 88};
        expected_sel = std::vector<std::size_t>{3, 0, 1, 5, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 4);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        expected_sel = std::vector<std::size_t>{4, 0, 1, 6, 8, 3};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 5);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        expected_sel = std::vector<std::size_t>{4, 0, 1, 6, 8, 3};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 6);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        expected_sel = std::vector<std::size_t>{4, 0, 1, 6, 8, 3};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 7);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        expected_sel = std::vector<std::size_t>{4, 0, 1, 6, 8, 3};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_to(std::vector<std::size_t>{0, 2, 4, 5, 7}, 8);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        expected_sel = std::vector<std::size_t>{4, 0, 1, 6, 8, 3};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_to(std::vector<std::size_t>{0, 1}, 0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_to(std::vector<std::size_t>{1, 0}, 0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_to(std::vector<std::size_t>{0, 1, 3}, 0);
        expected = std::vector{0, 11, 33, 22, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1, 2, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_to(std::vector<std::size_t>{3, 1, 0}, 0);
        expected = std::vector{0, 11, 33, 22, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1, 2, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_to(std::vector<std::size_t>{7, 8}, 8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_to(std::vector<std::size_t>{8, 7}, 8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_to(std::vector<std::size_t>{5, 7, 8}, 8);
        expected = std::vector{0, 11, 22, 33, 44, 66, 55, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.move_to(std::vector<std::size_t>{8, 7, 5}, 8);
        expected = std::vector{0, 11, 22, 33, 44, 66, 55, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
    }

    TEST(op_sel_sync, move_to_l)
    {
        Track_do_op obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        auto selections = std::vector<std::size_t>{0, 2, 4, 5, 7};
        obj()->ints.select(selections);
        obj()->ints.move_selections_to(0);
        auto expected = std::vector{0, 22, 44, 55, 77, 11, 33, 66, 88};
        auto expected_sel = std::vector<std::size_t>{0, 1, 2, 3, 4};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{0, 2, 4, 5, 7};
        obj()->ints.select(selections);
        obj()->ints.move_selections_to(1);
        expected = std::vector{11, 0, 22, 44, 55, 77, 33, 66, 88};
        expected_sel = std::vector<std::size_t>{1, 2, 3, 4, 5};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{0, 2, 4, 5, 7};
        obj()->ints.select(selections);
        obj()->ints.move_selections_to(2);
        expected = std::vector{11, 33, 0, 22, 44, 55, 77, 66, 88};
        expected_sel = std::vector<std::size_t>{2, 3, 4, 5, 6};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{0, 2, 4, 5, 7};
        obj()->ints.select(selections);
        obj()->ints.move_selections_to(3);
        expected = std::vector{11, 33, 66, 0, 22, 44, 55, 77, 88};
        expected_sel = std::vector<std::size_t>{3, 4, 5, 6, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{0, 2, 4, 5, 7};
        obj()->ints.select(selections);
        obj()->ints.move_selections_to(4);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        expected_sel = std::vector<std::size_t>{4, 5, 6, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{0, 2, 4, 5, 7};
        obj()->ints.select(selections);
        obj()->ints.move_selections_to(5);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        expected_sel = std::vector<std::size_t>{4, 5, 6, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{0, 2, 4, 5, 7};
        obj()->ints.select(selections);
        obj()->ints.move_selections_to(6);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        expected_sel = std::vector<std::size_t>{4, 5, 6, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{0, 2, 4, 5, 7};
        obj()->ints.select(selections);
        obj()->ints.move_selections_to(7);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        expected_sel = std::vector<std::size_t>{4, 5, 6, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{0, 2, 4, 5, 7};
        obj()->ints.select(selections);
        obj()->ints.move_selections_to(8);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        expected_sel = std::vector<std::size_t>{4, 5, 6, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{0, 1};
        obj()->ints.select(selections);
        obj()->ints.move_selections_to(0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{1, 0};
        obj()->ints.select(selections);
        obj()->ints.move_selections_to(0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{1, 0};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{0, 1, 3};
        obj()->ints.select(selections);
        obj()->ints.move_selections_to(0);
        expected = std::vector{0, 11, 33, 22, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{0, 1, 2};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{3, 1, 0};
        obj()->ints.select(selections);
        obj()->ints.move_selections_to(0);
        expected = std::vector{0, 11, 33, 22, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{2, 1, 0};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{7, 8};
        obj()->ints.select(selections);
        obj()->ints.move_selections_to(8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{8, 7};
        obj()->ints.select(selections);
        obj()->ints.move_selections_to(8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expected_sel = std::vector<std::size_t>{8, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{5, 7, 8};
        obj()->ints.select(selections);
        obj()->ints.move_selections_to(8);
        expected = std::vector{0, 11, 22, 33, 44, 66, 55, 77, 88};
        expected_sel = std::vector<std::size_t>{6, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        
        obj()->ints.clear_selections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{8, 7, 5};
        obj()->ints.select(selections);
        obj()->ints.move_selections_to(8);
        expected = std::vector{0, 11, 22, 33, 44, 66, 55, 77, 88};
        expected_sel = std::vector<std::size_t>{8, 7, 6};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
        obj.undo_action();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redo_action();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expected_sel, obj.view.ints.sel());
    }

    struct Md_array_test
    {
        int two_d[3][2] {{11, 22}, {33, 44}, {55, 66}};
        int three_d[4][3][2] {
            {{11, 22}, {33, 44}, {55, 66}},
            {{111, 222}, {333, 444}, {555, 666}},
            {{1111, 2222}, {3333, 4444}, {5555, 6666}},
            {{11111, 22222}, {33333, 44444}, {55555, 66666}}
        };

        REFLECT(Md_array_test, two_d, three_d)
    };

    struct Edit_md_array_test : nf::tracked<Md_array_test, Edit_md_array_test>
    {
        Edit_md_array_test() : tracked{this} {}
    };
    
    TEST(md_array, reset)
    {
        Edit_md_array_test my_obj {};
        EXPECT_EQ(my_obj->two_d[0][0], 11);
        EXPECT_EQ(my_obj->two_d[0][1], 22);
        EXPECT_EQ(my_obj->two_d[1][0], 33);
        EXPECT_EQ(my_obj->two_d[1][1], 44);
        EXPECT_EQ(my_obj->two_d[2][0], 55);
        EXPECT_EQ(my_obj->two_d[2][1], 66);
        EXPECT_EQ(my_obj->three_d[0][0][0], 11);
        EXPECT_EQ(my_obj->three_d[0][0][1], 22);
        EXPECT_EQ(my_obj->three_d[0][1][0], 33);
        EXPECT_EQ(my_obj->three_d[0][1][1], 44);
        EXPECT_EQ(my_obj->three_d[0][2][0], 55);
        EXPECT_EQ(my_obj->three_d[0][2][1], 66);
        EXPECT_EQ(my_obj->three_d[1][0][0], 111);
        EXPECT_EQ(my_obj->three_d[1][0][1], 222);
        EXPECT_EQ(my_obj->three_d[1][1][0], 333);
        EXPECT_EQ(my_obj->three_d[1][1][1], 444);
        EXPECT_EQ(my_obj->three_d[1][2][0], 555);
        EXPECT_EQ(my_obj->three_d[1][2][1], 666);
        EXPECT_EQ(my_obj->three_d[2][0][0], 1111);
        EXPECT_EQ(my_obj->three_d[2][0][1], 2222);
        EXPECT_EQ(my_obj->three_d[2][1][0], 3333);
        EXPECT_EQ(my_obj->three_d[2][1][1], 4444);
        EXPECT_EQ(my_obj->three_d[2][2][0], 5555);
        EXPECT_EQ(my_obj->three_d[2][2][1], 6666);
        EXPECT_EQ(my_obj->three_d[3][0][0], 11111);
        EXPECT_EQ(my_obj->three_d[3][0][1], 22222);
        EXPECT_EQ(my_obj->three_d[3][1][0], 33333);
        EXPECT_EQ(my_obj->three_d[3][1][1], 44444);
        EXPECT_EQ(my_obj->three_d[3][2][0], 55555);
        EXPECT_EQ(my_obj->three_d[3][2][1], 66666);
        
        my_obj()->two_d.reset();
        my_obj()->three_d.reset();
        EXPECT_EQ(my_obj->two_d[0][0], 0);
        EXPECT_EQ(my_obj->two_d[0][1], 0);
        EXPECT_EQ(my_obj->two_d[1][0], 0);
        EXPECT_EQ(my_obj->two_d[1][1], 0);
        EXPECT_EQ(my_obj->two_d[2][0], 0);
        EXPECT_EQ(my_obj->two_d[2][1], 0);
        EXPECT_EQ(my_obj->three_d[0][0][0], 0);
        EXPECT_EQ(my_obj->three_d[0][0][1], 0);
        EXPECT_EQ(my_obj->three_d[0][1][0], 0);
        EXPECT_EQ(my_obj->three_d[0][1][1], 0);
        EXPECT_EQ(my_obj->three_d[0][2][0], 0);
        EXPECT_EQ(my_obj->three_d[0][2][1], 0);
        EXPECT_EQ(my_obj->three_d[1][0][0], 0);
        EXPECT_EQ(my_obj->three_d[1][0][1], 0);
        EXPECT_EQ(my_obj->three_d[1][1][0], 0);
        EXPECT_EQ(my_obj->three_d[1][1][1], 0);
        EXPECT_EQ(my_obj->three_d[1][2][0], 0);
        EXPECT_EQ(my_obj->three_d[1][2][1], 0);
        EXPECT_EQ(my_obj->three_d[2][0][0], 0);
        EXPECT_EQ(my_obj->three_d[2][0][1], 0);
        EXPECT_EQ(my_obj->three_d[2][1][0], 0);
        EXPECT_EQ(my_obj->three_d[2][1][1], 0);
        EXPECT_EQ(my_obj->three_d[2][2][0], 0);
        EXPECT_EQ(my_obj->three_d[2][2][1], 0);
        EXPECT_EQ(my_obj->three_d[3][0][0], 0);
        EXPECT_EQ(my_obj->three_d[3][0][1], 0);
        EXPECT_EQ(my_obj->three_d[3][1][0], 0);
        EXPECT_EQ(my_obj->three_d[3][1][1], 0);
        EXPECT_EQ(my_obj->three_d[3][2][0], 0);
        EXPECT_EQ(my_obj->three_d[3][2][1], 0);

        my_obj.undo_action();
        my_obj.undo_action();
        EXPECT_EQ(my_obj->two_d[0][0], 11);
        EXPECT_EQ(my_obj->two_d[0][1], 22);
        EXPECT_EQ(my_obj->two_d[1][0], 33);
        EXPECT_EQ(my_obj->two_d[1][1], 44);
        EXPECT_EQ(my_obj->two_d[2][0], 55);
        EXPECT_EQ(my_obj->two_d[2][1], 66);
        EXPECT_EQ(my_obj->three_d[0][0][0], 11);
        EXPECT_EQ(my_obj->three_d[0][0][1], 22);
        EXPECT_EQ(my_obj->three_d[0][1][0], 33);
        EXPECT_EQ(my_obj->three_d[0][1][1], 44);
        EXPECT_EQ(my_obj->three_d[0][2][0], 55);
        EXPECT_EQ(my_obj->three_d[0][2][1], 66);
        EXPECT_EQ(my_obj->three_d[1][0][0], 111);
        EXPECT_EQ(my_obj->three_d[1][0][1], 222);
        EXPECT_EQ(my_obj->three_d[1][1][0], 333);
        EXPECT_EQ(my_obj->three_d[1][1][1], 444);
        EXPECT_EQ(my_obj->three_d[1][2][0], 555);
        EXPECT_EQ(my_obj->three_d[1][2][1], 666);
        EXPECT_EQ(my_obj->three_d[2][0][0], 1111);
        EXPECT_EQ(my_obj->three_d[2][0][1], 2222);
        EXPECT_EQ(my_obj->three_d[2][1][0], 3333);
        EXPECT_EQ(my_obj->three_d[2][1][1], 4444);
        EXPECT_EQ(my_obj->three_d[2][2][0], 5555);
        EXPECT_EQ(my_obj->three_d[2][2][1], 6666);
        EXPECT_EQ(my_obj->three_d[3][0][0], 11111);
        EXPECT_EQ(my_obj->three_d[3][0][1], 22222);
        EXPECT_EQ(my_obj->three_d[3][1][0], 33333);
        EXPECT_EQ(my_obj->three_d[3][1][1], 44444);
        EXPECT_EQ(my_obj->three_d[3][2][0], 55555);
        EXPECT_EQ(my_obj->three_d[3][2][1], 66666);

        my_obj.redo_action();
        my_obj.redo_action();
        EXPECT_EQ(my_obj->two_d[0][0], 0);
        EXPECT_EQ(my_obj->two_d[0][1], 0);
        EXPECT_EQ(my_obj->two_d[1][0], 0);
        EXPECT_EQ(my_obj->two_d[1][1], 0);
        EXPECT_EQ(my_obj->two_d[2][0], 0);
        EXPECT_EQ(my_obj->two_d[2][1], 0);
        EXPECT_EQ(my_obj->three_d[0][0][0], 0);
        EXPECT_EQ(my_obj->three_d[0][0][1], 0);
        EXPECT_EQ(my_obj->three_d[0][1][0], 0);
        EXPECT_EQ(my_obj->three_d[0][1][1], 0);
        EXPECT_EQ(my_obj->three_d[0][2][0], 0);
        EXPECT_EQ(my_obj->three_d[0][2][1], 0);
        EXPECT_EQ(my_obj->three_d[1][0][0], 0);
        EXPECT_EQ(my_obj->three_d[1][0][1], 0);
        EXPECT_EQ(my_obj->three_d[1][1][0], 0);
        EXPECT_EQ(my_obj->three_d[1][1][1], 0);
        EXPECT_EQ(my_obj->three_d[1][2][0], 0);
        EXPECT_EQ(my_obj->three_d[1][2][1], 0);
        EXPECT_EQ(my_obj->three_d[2][0][0], 0);
        EXPECT_EQ(my_obj->three_d[2][0][1], 0);
        EXPECT_EQ(my_obj->three_d[2][1][0], 0);
        EXPECT_EQ(my_obj->three_d[2][1][1], 0);
        EXPECT_EQ(my_obj->three_d[2][2][0], 0);
        EXPECT_EQ(my_obj->three_d[2][2][1], 0);
        EXPECT_EQ(my_obj->three_d[3][0][0], 0);
        EXPECT_EQ(my_obj->three_d[3][0][1], 0);
        EXPECT_EQ(my_obj->three_d[3][1][0], 0);
        EXPECT_EQ(my_obj->three_d[3][1][1], 0);
        EXPECT_EQ(my_obj->three_d[3][2][0], 0);
        EXPECT_EQ(my_obj->three_d[3][2][1], 0);
    }

    TEST(md_array, set)
    {
        Edit_md_array_test my_obj {};
        EXPECT_EQ(my_obj->two_d[0][0], 11);
        EXPECT_EQ(my_obj->two_d[0][1], 22);
        EXPECT_EQ(my_obj->two_d[1][0], 33);
        EXPECT_EQ(my_obj->two_d[1][1], 44);
        EXPECT_EQ(my_obj->two_d[2][0], 55);
        EXPECT_EQ(my_obj->two_d[2][1], 66);
        EXPECT_EQ(my_obj->three_d[0][0][0], 11);
        EXPECT_EQ(my_obj->three_d[0][0][1], 22);
        EXPECT_EQ(my_obj->three_d[0][1][0], 33);
        EXPECT_EQ(my_obj->three_d[0][1][1], 44);
        EXPECT_EQ(my_obj->three_d[0][2][0], 55);
        EXPECT_EQ(my_obj->three_d[0][2][1], 66);
        EXPECT_EQ(my_obj->three_d[1][0][0], 111);
        EXPECT_EQ(my_obj->three_d[1][0][1], 222);
        EXPECT_EQ(my_obj->three_d[1][1][0], 333);
        EXPECT_EQ(my_obj->three_d[1][1][1], 444);
        EXPECT_EQ(my_obj->three_d[1][2][0], 555);
        EXPECT_EQ(my_obj->three_d[1][2][1], 666);
        EXPECT_EQ(my_obj->three_d[2][0][0], 1111);
        EXPECT_EQ(my_obj->three_d[2][0][1], 2222);
        EXPECT_EQ(my_obj->three_d[2][1][0], 3333);
        EXPECT_EQ(my_obj->three_d[2][1][1], 4444);
        EXPECT_EQ(my_obj->three_d[2][2][0], 5555);
        EXPECT_EQ(my_obj->three_d[2][2][1], 6666);
        EXPECT_EQ(my_obj->three_d[3][0][0], 11111);
        EXPECT_EQ(my_obj->three_d[3][0][1], 22222);
        EXPECT_EQ(my_obj->three_d[3][1][0], 33333);
        EXPECT_EQ(my_obj->three_d[3][1][1], 44444);
        EXPECT_EQ(my_obj->three_d[3][2][0], 55555);
        EXPECT_EQ(my_obj->three_d[3][2][1], 66666);
        
        my_obj()->two_d[2][1] = 77;
        my_obj()->three_d[3][2][1] = 77777;
        EXPECT_EQ(my_obj->two_d[2][1], 77);
        EXPECT_EQ(my_obj->three_d[3][2][1], 77777);

        my_obj.undo_action();
        my_obj.undo_action();
        EXPECT_EQ(my_obj->two_d[2][1], 66);
        EXPECT_EQ(my_obj->three_d[3][2][1], 66666);

        my_obj.redo_action();
        my_obj.redo_action();
        EXPECT_EQ(my_obj->two_d[2][1], 77);
        EXPECT_EQ(my_obj->three_d[3][2][1], 77777);
    }

    struct Init_data_test
    {
        int a = 1;
        std::vector<int> b = {2, 3};

        REFLECT(Init_data_test, a, b)
    };

    struct Edit_init_data_test : nf::tracked<Init_data_test, Edit_init_data_test>
    {
        Edit_init_data_test() : tracked{this} {}
    };

    TEST(misc_edits, init_data)
    {
        Edit_init_data_test my_obj {};
        EXPECT_EQ(1, my_obj->a);
        std::vector<int> expected_vec {2, 3};
        EXPECT_EQ(expected_vec, my_obj->b);

        my_obj.init_data<false>(Init_data_test {
            .a = 4,
            .b = {5, 6}
        });
        EXPECT_EQ(0, my_obj.get_cursor_index());
        EXPECT_EQ(4, my_obj->a);
        expected_vec = {5, 6};
        EXPECT_EQ(expected_vec, my_obj->b);

        my_obj.init_data<true>(Init_data_test {
            .a = 7,
            .b = {8, 9}
        });
        EXPECT_EQ(1, my_obj.get_cursor_index());
        EXPECT_EQ(7, my_obj->a);
        expected_vec = {8, 9};
        EXPECT_EQ(expected_vec, my_obj->b);

        my_obj.undo_action();
        EXPECT_EQ(0, my_obj.get_cursor_index());
        EXPECT_EQ(7, my_obj->a);
        expected_vec = {8, 9};
        EXPECT_EQ(expected_vec, my_obj->b);

        my_obj.redo_action();
        EXPECT_EQ(1, my_obj.get_cursor_index());
        EXPECT_EQ(7, my_obj->a);
        expected_vec = {8, 9};
        EXPECT_EQ(expected_vec, my_obj->b);
        
        EXPECT_ANY_THROW(my_obj.init_data<false>(Init_data_test{}));
        EXPECT_ANY_THROW(my_obj.init_data<true>(Init_data_test{}));

        my_obj.clear_history();
        my_obj.init_data<true>(Init_data_test{
            .a = 10,
            .b = {11, 12}
        });
        EXPECT_EQ(1, my_obj.get_cursor_index());
        EXPECT_EQ(10, my_obj->a);
        expected_vec = {11, 12};
        EXPECT_EQ(expected_vec, my_obj->b);

        my_obj.undo_action();
        EXPECT_EQ(0, my_obj.get_cursor_index());
        EXPECT_EQ(10, my_obj->a);
        expected_vec = {11, 12};
        EXPECT_EQ(expected_vec, my_obj->b);

        my_obj.redo_action();
        EXPECT_EQ(1, my_obj.get_cursor_index());
        EXPECT_EQ(10, my_obj->a);
        expected_vec = {11, 12};
        EXPECT_EQ(expected_vec, my_obj->b);

        my_obj.clear_history();
        my_obj.init_data<false>(Init_data_test{
            .a = 13,
            .b = {14, 15}
        });
        EXPECT_EQ(0, my_obj.get_cursor_index());
        EXPECT_EQ(13, my_obj->a);
        expected_vec = {14, 15};
        EXPECT_EQ(expected_vec, my_obj->b);
    }

    struct Code_
    {
        enum uint8_t_ : std::uint8_t
        {
            _u = 5, // Undo
            _r = 6, // Redo
            _1 = 1,
            _2 = (1 << 1),
            _3 = (1 << 2),
            _4 = (1 << 3),
            _5 = (1 << 4),
            _6 = (1 << 5)
        };
    };
    using Code = Code_::uint8_t_;
    struct Case
    {
        std::vector<Code> codes {};
        std::uint8_t result = 0;
        std::vector<std::uint8_t> prior_states {}; // Records the value just prior to running code [i]
        std::vector<std::uint8_t> states {}; // Records the current value after running codes up to [i]
        std::vector<std::uint64_t> net_size {}; // Records the net_size after running codes up to [i]
        std::vector<std::size_t> action_index {}; // Records the action_index at which code [i] starts
        std::size_t max_actions {}; // Records the total_actions after running all codes
    };
    struct Trim_case_data
    {
        std::uint8_t value = 0;

        REFLECT(Trim_case_data, value)
    };
    struct Trim_case_editor : nf::tracked<Trim_case_data, Trim_case_editor>
    {
        Trim_case_editor() : tracked(this) {}
    };

    TEST(misc_edits, trim_history)
    {
        constexpr auto _u = Code::_u; // Undo
        constexpr auto _1 = Code::_1; // Bit 1
        constexpr auto _2 = Code::_2; // Bit 2
        constexpr auto _3 = Code::_3; // Bit 3
        constexpr auto _4 = Code::_4; // Bit 4
        constexpr auto _5 = Code::_5; // Bit 5
        constexpr auto _6 = Code::_6; // Bit 6
        std::vector<Case> test_cases {
            Case { .codes = {}, .result = 0 },
            Case { .codes = {_1}, .result = _1 }, // A
            Case { .codes = {_1, _u, _2}, .result = _2 }, // AUA
            Case { .codes = {_1, _2}, .result = _1|_2 }, // AA
            Case { .codes = {_1, _2, _u, _3}, .result = _1|_3 }, // AAUA
            Case { .codes = {_1, _2, _u, _3, _u, _u, _4}, .result = _4 }, // AAUAUUA
            Case { .codes = {_1, _2, _u, _3, _u, _4}, .result = _1|_4 }, // AAUAUA
            Case { .codes = {_1, _2, _u, _3, _u, _4, _u, _u, _5 }, .result = _5 }, // AAUAUAUUA
            Case { .codes = {_1, _2, _u, _4, _5}, .result = _1|_4|_5 }, // AAUAA
            Case { .codes = {_1, _2, _u, _3, _4, _u, _5}, .result = _1|_3|_5 }, // AAUAAUA
            Case { .codes = {_1, _2, _u, _3, _4, _u, _5, _u, _u, _u, _6}, .result = _6 } // AAUAAUAUUUA
        };
        auto run_code = [](auto & obj, auto code) {
            switch ( code )
            {
                case Code::_u: obj.undo_action(); break;
                case Code::_1: obj()->value |= Code::_1; break;
                case Code::_2: obj()->value |= Code::_2; break;
                case Code::_3: obj()->value |= Code::_3; break;
                case Code::_4: obj()->value |= Code::_4; break;
                case Code::_5: obj()->value |= Code::_5; break;
                case Code::_6: obj()->value |= Code::_6; break;
                default: throw std::logic_error("Unrecognized code!"); break;
            }
        };
        auto get_hist_size = [](auto & obj) -> std::uint64_t {
            auto change_history = obj.render_change_history(false);
            std::uint64_t net_size = 0;
            for ( auto & action : change_history )
                net_size += static_cast<std::uint64_t>(action.byte_count);

            return net_size;
        };
        
        for ( auto & test_case : test_cases )
        {
            Trim_case_editor my_obj {};
            for ( auto & code : test_case.codes )
            {
                test_case.prior_states.push_back(my_obj->value);
                run_code(my_obj, code);
                test_case.action_index.push_back(code == Code::_u ? std::numeric_limits<std::size_t>::max() : my_obj.get_cursor_index()-1);
                test_case.states.push_back(my_obj->value);
                test_case.net_size.push_back(get_hist_size(my_obj));
            }
            test_case.max_actions = my_obj.get_cursor_index();
            EXPECT_EQ(std::size(test_case.codes), std::size(test_case.net_size));
            EXPECT_EQ(my_obj->value, test_case.result);
        }

        std::size_t count_valid_sims = 0;
        for ( auto & test_case : test_cases )
        {
            auto max_actions = test_case.max_actions;
            for ( std::size_t trim_to=0; trim_to<=max_actions; ++trim_to )
            {
                Trim_case_editor apply_trim {};
                Trim_case_editor simulate_trim {};
                auto check_apply_trim = RareTs::whitebox((nf::tracked<Simple, Simple_editor> &)apply_trim);
                auto check_simulate_trim = RareTs::whitebox((nf::tracked<Simple, Simple_editor> &)simulate_trim);

                for ( auto & code : test_case.codes )
                    run_code(apply_trim, code);

                std::size_t prev_action_count = check_apply_trim.actions.size();
                auto true_trim_to = apply_trim.trim_history(trim_to);
                if ( prev_action_count == check_apply_trim.actions.size() )
                    continue; // No change, could maybe test against prev hist state of apply trim

                std::size_t first_code_index = true_trim_to == 0 ? test_case.codes.size() : 0;
                while ( first_code_index < std::size(test_case.codes) )
                {
                    while ( test_case.action_index[first_code_index] == std::numeric_limits<std::size_t>::max() )
                        ++first_code_index;

                    if ( test_case.action_index[first_code_index] >= true_trim_to )
                        break;
                    else
                        ++first_code_index;
                }

                if ( first_code_index < test_case.codes.size() )
                    simulate_trim.init_data(Trim_case_data{test_case.prior_states[first_code_index]});
                else if ( !test_case.codes.empty() )
                    simulate_trim.init_data(Trim_case_data{test_case.states[test_case.codes.size()-1]});

                // Run first code index and onwards
                bool valid_sim = true;
                for ( std::size_t i=first_code_index; i<std::size(test_case.codes); ++i )
                {
                    auto prev_cursor = simulate_trim.get_cursor_index();
                    run_code(simulate_trim, test_case.codes[i]);
                    if ( simulate_trim.get_cursor_index() == prev_cursor ) // Invalid sim: required undoing past sim hist start
                    {
                        valid_sim = false;
                        break;
                    }
                }

                // Compare apply_trim and simulate_trim values and history states
                if ( valid_sim )
                {
                    ++count_valid_sims;
                    EXPECT_EQ(apply_trim->value, simulate_trim->value);
                    EXPECT_EQ(check_apply_trim.pending_action_start, check_simulate_trim.pending_action_start);
                    EXPECT_EQ(check_apply_trim.actions.size(), check_simulate_trim.actions.size());
                    if ( check_apply_trim.actions.size() == check_simulate_trim.actions.size() )
                    {
                        for ( std::size_t i=0; i<check_apply_trim.actions.size(); ++i )
                            EXPECT_EQ(check_apply_trim.actions[i].first_event_index, check_simulate_trim.actions[i].first_event_index);
                    }
                    EXPECT_EQ(check_apply_trim.action_reference_count, check_simulate_trim.action_reference_count);
                    EXPECT_EQ(check_apply_trim.redo_count, check_simulate_trim.redo_count);
                    EXPECT_EQ(check_apply_trim.redo_size, check_simulate_trim.redo_size);
                    EXPECT_EQ(check_apply_trim.history.event_offsets.size(), check_simulate_trim.history.event_offsets.size());
                    if ( check_apply_trim.history.event_offsets.size() == check_simulate_trim.history.event_offsets.size() )
                    {
                        for ( std::size_t i=0; i<check_apply_trim.history.event_offsets.size(); ++i )
                            EXPECT_EQ(check_apply_trim.history.event_offsets[i], check_simulate_trim.history.event_offsets[i]);
                    }
                    EXPECT_EQ(check_apply_trim.history.events.size(), check_simulate_trim.history.events.size());
                    if ( check_apply_trim.history.events.size() == check_simulate_trim.history.events.size() )
                    {
                        for ( std::size_t i=0; i<check_apply_trim.history.events.size(); ++i )
                            EXPECT_EQ(check_apply_trim.history.events[i], check_simulate_trim.history.events[i]);
                    }
                }
            }
        }
        EXPECT_EQ(count_valid_sims, 23);

        for ( auto & test_case : test_cases )
        {
            auto num_codes = test_case.codes.size();
            auto max_actions = test_case.max_actions;
            for ( std::size_t code_index=0; code_index<num_codes; ++code_index )
            {
                Trim_case_editor apply_trim {};
                
                for ( auto & code : test_case.codes )
                    run_code(apply_trim, code);

                auto max_size = test_case.net_size.size() > 0 ? test_case.net_size[test_case.net_size.size()-1] : 0;
                auto size_trim_to = code_index == 0 ? max_size : max_size - test_case.net_size[code_index-1];
                apply_trim.trim_history_to_size(size_trim_to);
                auto new_hist_size = get_hist_size(apply_trim);

                EXPECT_TRUE(new_hist_size <= max_size);

                // Ensure there's no possible size between new_hist_size and size_trim_to
                for ( std::size_t trim_to=0; trim_to<=max_actions; ++trim_to )
                {
                    Trim_case_editor simulate_trim {};
                    
                    for ( auto & code : test_case.codes )
                        run_code(simulate_trim, code);

                    simulate_trim.trim_history(trim_to);
                    auto sim_hist_size = get_hist_size(simulate_trim);

                    EXPECT_FALSE(sim_hist_size > new_hist_size && sim_hist_size < size_trim_to);
                }
            }
        }
    }

    TEST(misc_edits, root_assign)
    {
        Edit_init_data_test my_obj {};
        EXPECT_EQ(1, my_obj->a);
        std::vector<int> expected_vec {2, 3};
        EXPECT_EQ(expected_vec, my_obj->b);

        my_obj()->assign(Init_data_test{
            .a = 4,
            .b = {5, 6}
        });
        EXPECT_EQ(4, my_obj->a);
        expected_vec = {5, 6};
        EXPECT_EQ(expected_vec, my_obj->b);

        my_obj.undo_action();
        EXPECT_EQ(1, my_obj->a);
        expected_vec = {2, 3};
        EXPECT_EQ(expected_vec, my_obj->b);

        my_obj.redo_action();
        EXPECT_EQ(4, my_obj->a);
        expected_vec = {5, 6};
        EXPECT_EQ(expected_vec, my_obj->b);
    }

    struct Optional_test
    {
        std::optional<int> a = 1;

        REFLECT(Optional_test, a)
    };

    struct Edit_optional_test : nf::tracked<Optional_test, Edit_optional_test>
    {
        Edit_optional_test() : tracked{this} {}
    };

    TEST(misc_edits, optional_test)
    {
        Edit_optional_test my_obj {};
        EXPECT_EQ(my_obj->a.value(), 1);
        my_obj()->a = std::nullopt;
        EXPECT_EQ(my_obj->a, std::nullopt);
        my_obj()->a = 2;
        EXPECT_EQ(my_obj->a.value(), 2);

        my_obj.undo_action();
        EXPECT_EQ(my_obj->a, std::nullopt);
        my_obj.undo_action();
        EXPECT_EQ(my_obj->a, 1);

        my_obj.redo_action();
        EXPECT_EQ(my_obj->a, std::nullopt);
        my_obj.redo_action();
        EXPECT_EQ(my_obj->a, 2);
    }

    struct Item
    {
        int hit_count;

        REFLECT(Item, hit_count)
    };

    struct Actor
    {
        int xc = 0;
        int yc = 0;
        std::vector<Item> items {Item{}};

        REFLECT(Actor, xc, yc, items)
    };

    struct My_obj
    {
        std::vector<Actor> actors {};

        REFLECT(My_obj, actors)
    };

    struct Traced_obj : nf::tracked<My_obj, Traced_obj>
    {
        struct Item_elem : nf::tracked_element<Item, NF_PATH(root->actors[0].items[0])>
        {
            using tracked_element::tracked_element;
            void hit() { edit.hit_count = read.hit_count+1; }
        };

        struct Actor_elem : nf::tracked_element<Actor, NF_PATH(root->actors[0])>
        {
            using tracked_element::tracked_element;
            void act() { edit.xc = read.xc+2; }
            auto get_item_elem(std::size_t i) { return Item_elem(this, view.items[i]); }
        };

        Traced_obj() : tracked{this} {}

        Actor_elem get_actor_elem(std::size_t i) {
            return Actor_elem(this, view.actors[i]);
        }
    };

    TEST(misc_edits, sub_elem_test)
    {
        Traced_obj my_obj {};
        my_obj.init_data<false>(My_obj{
            .actors = {
                Actor{.xc = 11, .yc = 33, .items = {{0}, {0}}},
                Actor{.xc = 22, .yc = 44, .items = {{0}, {0}, {0}}}
            }
        });
        {
            auto edit_actor = my_obj.get_actor_elem(1);
            EXPECT_EQ(my_obj->actors[1].xc, 22);
            edit_actor.act();
            EXPECT_EQ(my_obj->actors[1].xc, 24);

            auto edit_item = edit_actor.get_item_elem(2);
            EXPECT_EQ(my_obj->actors[1].items[2].hit_count, 0);
            edit_item.hit();
            EXPECT_EQ(my_obj->actors[1].items[2].hit_count, 1);
        }
        my_obj.undo_action();
        EXPECT_EQ(my_obj->actors[1].xc, 22);
        EXPECT_EQ(my_obj->actors[1].items[2].hit_count, 0);
        
        my_obj.redo_action();
        EXPECT_EQ(my_obj->actors[1].xc, 24);
        EXPECT_EQ(my_obj->actors[1].items[2].hit_count, 1);
    }

    struct User_defined_data
    {
        std::string descr = "";

        inline bool operator==(const User_defined_data & other) const { return descr == other.descr; }
    };

    struct User_defined_test
    {
        int a = 0;

        REFLECT(User_defined_test, a)
    };

    struct Edit_user_defined_test : nf::tracked<User_defined_test, Edit_user_defined_test, User_defined_data>
    {
        Edit_user_defined_test() : tracked{this} {}
    };

    TEST(misc_edits, user_defined_action_data)
    {
        Edit_user_defined_test my_obj {};
        {
            auto edit = my_obj.operator()({"labeled action"});
            edit->a = 1;
        }
        nf::rendered_action<User_defined_data> render_action {};
        my_obj.render_action(0, render_action, false);
        EXPECT_STREQ(render_action.user_data.descr.c_str(), "labeled action");
        {
            auto edit = my_obj.operator()({"another action"});
            edit->a = 2;
        }
        my_obj.render_action(1, render_action, false);
        EXPECT_STREQ(render_action.user_data.descr.c_str(), "another action");
    }

}
