#include <rarecpp/editor.h>
#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <gtest/gtest.h>
#include <cstdint>
#include <vector>

namespace EditorCumulative
{
    using namespace RareEdit;
    
    struct Simple
    {
        int a = 0;

        REFLECT(Simple, a)
    };

    struct SimpleEditor : Tracked<Simple, SimpleEditor>
    {
        SimpleEditor() : Tracked{this} {}

        void act()
        {
            createAction()->a = 0;
        }
    };

    TEST(UndoRedoElision, Unmodified)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);

        EXPECT_EQ(check.redoCount, 0);
        EXPECT_EQ(check.redoSize, 0);
        EXPECT_TRUE(check.actions.empty());
    }

    TEST(UndoRedoElision, A)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);
        obj.act();

        EXPECT_EQ(check.redoCount, 0);
        EXPECT_EQ(check.redoSize, 0);
        EXPECT_EQ(check.actions.size(), 1);
        EXPECT_EQ(check.actions[0].firstEventIndex, 0);
    }

    TEST(UndoRedoElision, AU)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);
        obj.act();
        obj.undoAction();

        EXPECT_EQ(check.redoCount, 1);
        EXPECT_EQ(check.redoSize, 1);
        EXPECT_EQ(check.actions.size(), 1);
        EXPECT_EQ(check.actions[0].firstEventIndex, 0);
    }

    TEST(UndoRedoElision, AUA)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);
        obj.act();
        obj.undoAction();
        obj.act();

        EXPECT_EQ(check.redoCount, 0);
        EXPECT_EQ(check.redoSize, 0);
        EXPECT_EQ(check.actions.size(), 3);
        EXPECT_EQ(check.actions[0].firstEventIndex, 0);
        EXPECT_EQ(check.actions[1].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[2].firstEventIndex, 1);
    }

    TEST(UndoRedoElision, AUR)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);
        obj.act();
        obj.undoAction();
        obj.redoAction();

        EXPECT_EQ(check.redoCount, 0);
        EXPECT_EQ(check.redoSize, 0);
        EXPECT_EQ(check.actions.size(), 1);
        EXPECT_EQ(check.actions[0].firstEventIndex, 0);
    }

    TEST(UndoRedoElision, AA)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);
        obj.act();
        obj.act();

        EXPECT_EQ(check.redoCount, 0);
        EXPECT_EQ(check.redoSize, 0);
        EXPECT_EQ(check.actions.size(), 2);
        EXPECT_EQ(check.actions[0].firstEventIndex, 0);
        EXPECT_EQ(check.actions[1].firstEventIndex, 1);
    }

    TEST(UndoRedoElision, AAU)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);
        obj.act();
        obj.act();
        obj.undoAction();

        EXPECT_EQ(check.redoCount, 1);
        EXPECT_EQ(check.redoSize, 1);
        EXPECT_EQ(check.actions.size(), 2);
        EXPECT_EQ(check.actions[0].firstEventIndex, 0);
        EXPECT_EQ(check.actions[1].firstEventIndex, 1);
    }

    TEST(UndoRedoElision, AAUA)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);
        obj.act();
        obj.act();
        obj.undoAction();
        obj.act();

        EXPECT_EQ(check.redoCount, 0);
        EXPECT_EQ(check.redoSize, 0);
        EXPECT_EQ(check.actions.size(), 4);
        EXPECT_EQ(check.actions[0].firstEventIndex, 0);
        EXPECT_EQ(check.actions[1].firstEventIndex, 1);
        EXPECT_EQ(check.actions[2].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[3].firstEventIndex, 2);
    }

    TEST(UndoRedoElision, AAUAU)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);
        obj.act();
        obj.act();
        obj.undoAction();
        obj.act();
        obj.undoAction();

        EXPECT_EQ(check.redoCount, 1);
        EXPECT_EQ(check.redoSize, 1);
        EXPECT_EQ(check.actions.size(), 4);
        EXPECT_EQ(check.actions[0].firstEventIndex, 0);
        EXPECT_EQ(check.actions[1].firstEventIndex, 1);
        EXPECT_EQ(check.actions[2].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[3].firstEventIndex, 2);
    }

    TEST(UndoRedoElision, AAUAUU)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);
        obj.act();
        obj.act();
        obj.undoAction();
        obj.act();
        obj.undoAction();
        obj.undoAction();

        EXPECT_EQ(check.redoCount, 2);
        EXPECT_EQ(check.redoSize, 4);
        EXPECT_EQ(check.actions.size(), 4);
        EXPECT_EQ(check.actions[0].firstEventIndex, 0);
        EXPECT_EQ(check.actions[1].firstEventIndex, 1);
        EXPECT_EQ(check.actions[2].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[3].firstEventIndex, 2);
    }

    TEST(UndoRedoElision, AAUAUUA)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);
        obj.act();
        obj.act();
        obj.undoAction();
        obj.act();
        obj.undoAction();
        obj.undoAction();
        obj.act();

        EXPECT_EQ(check.redoCount, 0);
        EXPECT_EQ(check.redoSize, 0);
        EXPECT_EQ(check.actions.size(), 6);
        EXPECT_EQ(check.actions[0].firstEventIndex, 0);
        EXPECT_EQ(check.actions[1].firstEventIndex, 1);
        EXPECT_EQ(check.actions[2].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[3].firstEventIndex, 2);
        EXPECT_EQ(check.actions[4].firstEventIndex, check.flagElidedRedos | 4);
        EXPECT_EQ(check.actions[5].firstEventIndex, 3);
    }

    TEST(UndoRedoElision, AAUAUA)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);
        obj.act();
        obj.act();
        obj.undoAction();
        obj.act();
        obj.undoAction();
        obj.act();

        EXPECT_EQ(check.redoCount, 0);
        EXPECT_EQ(check.redoSize, 0);
        EXPECT_EQ(check.actions.size(), 6);
        EXPECT_EQ(check.actions[0].firstEventIndex, 0);
        EXPECT_EQ(check.actions[1].firstEventIndex, 1);
        EXPECT_EQ(check.actions[2].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[3].firstEventIndex, 2);
        EXPECT_EQ(check.actions[4].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[5].firstEventIndex, 3);
    }

    TEST(UndoRedoElision, AAUAUAU)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);
        obj.act();
        obj.act();
        obj.undoAction();
        obj.act();
        obj.undoAction();
        obj.act();
        obj.undoAction();

        EXPECT_EQ(check.redoCount, 1);
        EXPECT_EQ(check.redoSize, 1);
        EXPECT_EQ(check.actions.size(), 6);
        EXPECT_EQ(check.actions[0].firstEventIndex, 0);
        EXPECT_EQ(check.actions[1].firstEventIndex, 1);
        EXPECT_EQ(check.actions[2].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[3].firstEventIndex, 2);
        EXPECT_EQ(check.actions[4].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[5].firstEventIndex, 3);
    }

    TEST(UndoRedoElision, AAUAUAUU)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);
        obj.act();
        obj.act();
        obj.undoAction();
        obj.act();
        obj.undoAction();
        obj.act();
        obj.undoAction();
        obj.undoAction();

        EXPECT_EQ(check.redoCount, 2);
        EXPECT_EQ(check.redoSize, 6);
        EXPECT_EQ(check.actions.size(), 6);
        EXPECT_EQ(check.actions[0].firstEventIndex, 0);
        EXPECT_EQ(check.actions[1].firstEventIndex, 1);
        EXPECT_EQ(check.actions[2].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[3].firstEventIndex, 2);
        EXPECT_EQ(check.actions[4].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[5].firstEventIndex, 3);
    }

    TEST(UndoRedoElision, AAUAUAUUA)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);
        obj.act();
        obj.act();
        obj.undoAction();
        obj.act();
        obj.undoAction();
        obj.act();
        obj.undoAction();
        obj.undoAction();
        obj.act();

        EXPECT_EQ(check.redoCount, 0);
        EXPECT_EQ(check.redoSize, 0);
        EXPECT_EQ(check.actions.size(), 8);
        EXPECT_EQ(check.actions[0].firstEventIndex, 0);
        EXPECT_EQ(check.actions[1].firstEventIndex, 1);
        EXPECT_EQ(check.actions[2].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[3].firstEventIndex, 2);
        EXPECT_EQ(check.actions[4].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[5].firstEventIndex, 3);
        EXPECT_EQ(check.actions[6].firstEventIndex, check.flagElidedRedos | 6);
        EXPECT_EQ(check.actions[7].firstEventIndex, 4);
    }

    TEST(UndoRedoElision, AAUAA)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);
        obj.act();
        obj.act();
        obj.undoAction();
        obj.act();
        obj.act();

        EXPECT_EQ(check.redoCount, 0);
        EXPECT_EQ(check.redoSize, 0);
        EXPECT_EQ(check.actions.size(), 5);
        EXPECT_EQ(check.actions[0].firstEventIndex, 0);
        EXPECT_EQ(check.actions[1].firstEventIndex, 1);
        EXPECT_EQ(check.actions[2].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[3].firstEventIndex, 2);
        EXPECT_EQ(check.actions[4].firstEventIndex, 3);
    }

    TEST(UndoRedoElision, AAUAAU)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);
        obj.act();
        obj.act();
        obj.undoAction();
        obj.act();
        obj.act();
        obj.undoAction();

        EXPECT_EQ(check.redoCount, 1);
        EXPECT_EQ(check.redoSize, 1);
        EXPECT_EQ(check.actions.size(), 5);
        EXPECT_EQ(check.actions[0].firstEventIndex, 0);
        EXPECT_EQ(check.actions[1].firstEventIndex, 1);
        EXPECT_EQ(check.actions[2].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[3].firstEventIndex, 2);
        EXPECT_EQ(check.actions[4].firstEventIndex, 3);
    }

    TEST(UndoRedoElision, AAUAAUA)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);
        obj.act();
        obj.act();
        obj.undoAction();
        obj.act();
        obj.act();
        obj.undoAction();
        obj.act();

        EXPECT_EQ(check.redoCount, 0);
        EXPECT_EQ(check.redoSize, 0);
        EXPECT_EQ(check.actions.size(), 7);
        EXPECT_EQ(check.actions[0].firstEventIndex, 0);
        EXPECT_EQ(check.actions[1].firstEventIndex, 1);
        EXPECT_EQ(check.actions[2].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[3].firstEventIndex, 2);
        EXPECT_EQ(check.actions[4].firstEventIndex, 3);
        EXPECT_EQ(check.actions[5].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[6].firstEventIndex, 4);
    }

    TEST(UndoRedoElision, AAUAAUAU)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);
        obj.act();
        obj.act();
        obj.undoAction();
        obj.act();
        obj.act();
        obj.undoAction();
        obj.act();
        obj.undoAction();

        EXPECT_EQ(check.redoCount, 1);
        EXPECT_EQ(check.redoSize, 1);
        EXPECT_EQ(check.actions.size(), 7);
        EXPECT_EQ(check.actions[0].firstEventIndex, 0);
        EXPECT_EQ(check.actions[1].firstEventIndex, 1);
        EXPECT_EQ(check.actions[2].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[3].firstEventIndex, 2);
        EXPECT_EQ(check.actions[4].firstEventIndex, 3);
        EXPECT_EQ(check.actions[5].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[6].firstEventIndex, 4);
    }

    TEST(UndoRedoElision, AAUAAUAUU)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);
        obj.act();
        obj.act();
        obj.undoAction();
        obj.act();
        obj.act();
        obj.undoAction();
        obj.act();
        obj.undoAction();
        obj.undoAction();

        EXPECT_EQ(check.redoCount, 2);
        EXPECT_EQ(check.redoSize, 4);
        EXPECT_EQ(check.actions.size(), 7);
        EXPECT_EQ(check.actions[0].firstEventIndex, 0);
        EXPECT_EQ(check.actions[1].firstEventIndex, 1);
        EXPECT_EQ(check.actions[2].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[3].firstEventIndex, 2);
        EXPECT_EQ(check.actions[4].firstEventIndex, 3);
        EXPECT_EQ(check.actions[5].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[6].firstEventIndex, 4);
    }

    TEST(UndoRedoElision, AAUAAUAUUU)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);
        obj.act();
        obj.act();
        obj.undoAction();
        obj.act();
        obj.act();
        obj.undoAction();
        obj.act();
        obj.undoAction();
        obj.undoAction();
        obj.undoAction();

        EXPECT_EQ(check.redoCount, 3);
        EXPECT_EQ(check.redoSize, 7);
        EXPECT_EQ(check.actions.size(), 7);
        EXPECT_EQ(check.actions[0].firstEventIndex, 0);
        EXPECT_EQ(check.actions[1].firstEventIndex, 1);
        EXPECT_EQ(check.actions[2].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[3].firstEventIndex, 2);
        EXPECT_EQ(check.actions[4].firstEventIndex, 3);
        EXPECT_EQ(check.actions[5].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[6].firstEventIndex, 4);
    }

    TEST(UndoRedoElision, AAUAAUAUUUA)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);
        obj.act();
        obj.act();
        obj.undoAction();
        obj.act();
        obj.act();
        obj.undoAction();
        obj.act();
        obj.undoAction();
        obj.undoAction();
        obj.undoAction();
        obj.act();

        EXPECT_EQ(check.redoCount, 0);
        EXPECT_EQ(check.redoSize, 0);
        EXPECT_EQ(check.actions.size(), 9);
        EXPECT_EQ(check.actions[0].firstEventIndex, 0);
        EXPECT_EQ(check.actions[1].firstEventIndex, 1);
        EXPECT_EQ(check.actions[2].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[3].firstEventIndex, 2);
        EXPECT_EQ(check.actions[4].firstEventIndex, 3);
        EXPECT_EQ(check.actions[5].firstEventIndex, check.flagElidedRedos | 1);
        EXPECT_EQ(check.actions[6].firstEventIndex, 4);
        EXPECT_EQ(check.actions[7].firstEventIndex, check.flagElidedRedos | 7);
        EXPECT_EQ(check.actions[8].firstEventIndex, 5);
    }

    NOTE(CumulativeTest, IndexSize<std::uint32_t>)
    struct CumulativeTest
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
        std::vector<Bar> barVec {}; NOTE(barVec, IndexSize<std::uint16_t>)
        int intRay[5] {};
        int mdIntRay[2][3] {};
        std::vector<Trig> trigs {}; NOTE(trigs, IndexSize<std::uint16_t>)

        REFLECT_NOTED(CumulativeTest, a, b, c, bar, barVec, intRay, mdIntRay, trigs)
    };

    struct TracedCumulativeTest : Tracked<CumulativeTest, TracedCumulativeTest>
    {
        TracedCumulativeTest() : Tracked{this} {}

        void takeAction()
        {
            auto edit = createAction();
            edit->a = 12;
            edit->b = "qwerty";
            edit->c.append(5);
            edit->c.append(6);
            edit->c.append(7);
            edit->bar.decimal = 2.2f;
            edit->bar.integer = 1;
            createBar(EditorCumulative::CumulativeTest::Bar{1, 2.2f, {3, 4}});
            createBar(EditorCumulative::CumulativeTest::Bar{3, 4.4f, {3, 4, 5}});
            edit->barVec[0].ints.append(8);
            edit->barVec[0].ints.append(9);
            edit->barVec[0].ints.append(10);
            edit->barVec[1].ints.append(8);
            edit->barVec[1].ints.append(9);
            edit->barVec[1].ints.append(10);
            edit->intRay[2] = 5;
            edit->intRay.select({1, 2, 4});
            edit->intRay.selection() = 6;
            edit->mdIntRay[1][2] = 23;
            edit->trigs.append(EditorCumulative::CumulativeTest::Trig{});
            edit->trigs.append(EditorCumulative::CumulativeTest::Trig{});
            edit->trigs[0].conditions[3].type = uint8_t(12);
            edit->trigs.select({0, 1});
            edit->trigs.selection().conditions[0].type = uint8_t(11);
        }

        void createBar(const EditorCumulative::CumulativeTest::Bar & newBar)
        {
            auto edit = createAction();
            edit->barVec.append(newBar);
        }

        void act()
        {
            createAction();
        }
    };

    TEST(EditTest, Cumulative)
    {
        TracedCumulativeTest myObj {};
        
        myObj.takeAction();
        myObj.createBar({});

        std::stringstream endState {};
        endState << Json::out(*myObj);
        EXPECT_STREQ(endState.str().c_str(), "{\"a\":12,\"b\":\"qwerty\",\"c\":[5,6,7],\"bar\":{\"integer\":1,\"decimal\":2.2,\"ints\":[]},"
            "\"barVec\":[{\"integer\":1,\"decimal\":2.2,\"ints\":[3,4,8,9,10]},{\"integer\":3,\"decimal\":4.4,\"ints\":[3,4,5,8,9,10]},"
            "{\"integer\":0,\"decimal\":0,\"ints\":[]}],\"intRay\":[0,6,6,0,6],\"mdIntRay\":[[0,0,0],[0,0,23]],\"trigs\":["
            "{\"conditions\":[{\"type\":11},{\"type\":0},{\"type\":0},{\"type\":12}]},"
            "{\"conditions\":[{\"type\":11},{\"type\":0},{\"type\":0},{\"type\":0}]}]}");
    }

    struct TrackDoOpData
    {
        std::vector<int> ints;

        REFLECT(TrackDoOpData, ints)
    };

    struct TrackDoOp : Tracked<TrackDoOpData, TrackDoOp>
    {
        TrackDoOp() : Tracked{this} {}
    };
    
    TEST(OpDo, Reset)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        obj()->ints = std::vector<int>{0, 1, 2, 3, 4};
        EXPECT_EQ(5, obj->ints.size());
        obj()->ints.reset();
        EXPECT_EQ(0, obj->ints.size());
    }

    TEST(OpDo, Reserve)
    {
        TrackDoOp obj {};
        std::size_t capacity = obj->ints.capacity();
        obj()->ints.reserve(capacity+1);
        EXPECT_EQ(capacity+1, obj->ints.capacity());
    }

    TEST(OpDo, Trim)
    {
        TrackDoOp obj {};
        std::size_t capacity = obj->ints.capacity();
        obj()->ints.reserve(capacity+1);
        EXPECT_EQ(capacity+1, obj->ints.capacity());
        obj()->ints.trim();
        EXPECT_EQ(0, obj->ints.capacity());
    }

    TEST(OpDo, Assign)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        obj()->ints.assign(5, 3);
        EXPECT_EQ(5, obj->ints.size());
        for ( auto i : obj->ints )
            EXPECT_EQ(3, i);
    }

    TEST(OpDo, AssignDefault)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        obj()->ints.assignDefault(10);
        EXPECT_EQ(10, obj->ints.size());
    }

    TEST(OpDo, ClearSelection)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 1};
        EXPECT_EQ(2, obj->ints.size());
        EXPECT_EQ(0, obj.view.ints.sel().size());
        obj()->ints.select(1);
        EXPECT_EQ(std::vector<std::size_t>{1}, obj.view.ints.sel());
        obj()->ints.clearSelections();
        EXPECT_EQ(0, obj.view.ints.sel().size());
    }

    TEST(OpDo, SelectAll)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{22, 33};
        EXPECT_EQ(2, obj->ints.size());
        EXPECT_EQ(0, obj.view.ints.sel().size());
        obj()->ints.selectAll();
        const auto expected = std::vector<std::size_t>{0, 1};
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(OpDo, Select)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{22, 33};
        EXPECT_EQ(2, obj->ints.size());
        EXPECT_EQ(0, obj.view.ints.sel().size());
        obj()->ints.select(1);
        EXPECT_EQ(std::vector<std::size_t>{1}, obj.view.ints.sel());
    }

    TEST(OpDo, SelectN)
    {
        TrackDoOp obj {};
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

    TEST(OpDo, Deselect)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        obj()->ints.select(std::vector<std::size_t>{2, 4, 0, 1});
        EXPECT_EQ(5, obj->ints.size());
        EXPECT_EQ(4, obj.view.ints.sel().size());
        obj()->ints.deselect(4);
        const auto expected = std::vector<std::size_t>{2, 0, 1};
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(OpDo, DeselectN)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        obj()->ints.select(std::vector<std::size_t>{2, 4, 0, 1});
        EXPECT_EQ(5, obj->ints.size());
        EXPECT_EQ(4, obj.view.ints.sel().size());
        obj()->ints.deselect(std::vector<std::size_t>{4, 2});
        const auto expected = std::vector<std::size_t>{0, 1};
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(OpDo, ToggleSelection)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        EXPECT_EQ(5, obj->ints.size());
        obj()->ints.toggleSelected(2);
        obj()->ints.toggleSelected(4);
        obj()->ints.toggleSelected(0);
        obj()->ints.toggleSelected(3);
        EXPECT_EQ(4, obj.view.ints.sel().size());
        auto expected = std::vector<std::size_t>{2, 4, 0, 3};
        EXPECT_EQ(expected, obj.view.ints.sel());
        obj()->ints.toggleSelected(4);
        obj()->ints.toggleSelected(2);
        EXPECT_EQ(2, obj.view.ints.sel().size());
        expected = {0, 3};
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(OpDo, ToggleSelectionN)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        EXPECT_EQ(5, obj->ints.size());
        obj()->ints.toggleSelected(std::vector<std::size_t>{2, 4, 0, 3});
        EXPECT_EQ(4, obj.view.ints.sel().size());
        auto expected = std::vector<std::size_t>{2, 4, 0, 3};
        EXPECT_EQ(expected, obj.view.ints.sel());
        obj()->ints.toggleSelected(std::vector<std::size_t>{4, 2});
        EXPECT_EQ(2, obj.view.ints.sel().size());
        expected = {0, 3};
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(OpDo, SortSelections)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        EXPECT_EQ(5, obj->ints.size());
        auto selection = std::vector<std::size_t>{2, 4, 0, 3};
        obj()->ints.select(selection);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj()->ints.sortSelection();
        const auto expected = std::vector<std::size_t>{0, 2, 3, 4};
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(OpDo, SortSelectionsDesc)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        EXPECT_EQ(5, obj->ints.size());
        auto selection = std::vector<std::size_t>{2, 4, 0, 3};
        obj()->ints.select(selection);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj()->ints.sortSelectionDescending();
        const auto expected = std::vector<std::size_t>{4, 3, 2, 0};
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(OpDo, Set)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{0, 1, 2, 3, 4};
        obj()->ints = values;
        EXPECT_EQ(5, obj->ints.size());
        EXPECT_EQ(values, obj->ints);
        obj()->ints[2] = 22;
        values = std::vector{0, 1, 22, 3, 4};
        EXPECT_EQ(values, obj->ints);
    }

    TEST(OpDo, SetN)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 1, 2, 3, 4, 5};
        obj()->ints = values;
        EXPECT_EQ(6, obj->ints.size());
        obj()->ints.set(std::vector<std::size_t>{1, 3, 5}, 33);
        const auto expected = std::vector{0, 33, 2, 33, 4, 33};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, SetL)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 1, 2, 3, 4, 5};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        obj()->ints.select(std::vector<std::size_t>{1, 3, 5});
        obj()->ints.selection() = 33;
        const auto expected = std::vector{0, 33, 2, 33, 4, 33};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, PlusEq)
    {
        TrackDoOp obj {};
        obj()->ints.append(2);
        EXPECT_EQ(obj->ints, std::vector{2});
        obj()->ints[0] += 3;
        EXPECT_EQ(obj->ints, std::vector{5});
    }

    TEST(OpDo, MinusEq)
    {
        TrackDoOp obj {};
        obj()->ints.append(7);
        EXPECT_EQ(obj->ints, std::vector{7});
        obj()->ints[0] -= 2;
        EXPECT_EQ(obj->ints, std::vector{5});
    }

    TEST(OpDo, MultEq)
    {
        TrackDoOp obj {};
        obj()->ints.append(2);
        EXPECT_EQ(obj->ints, std::vector{2});
        obj()->ints[0] *= 2;
        EXPECT_EQ(obj->ints, std::vector{4});
    }

    TEST(OpDo, DivEq)
    {
        TrackDoOp obj {};
        obj()->ints.append(4);
        EXPECT_EQ(obj->ints, std::vector{4});
        obj()->ints[0] /= 2;
        EXPECT_EQ(obj->ints, std::vector{2});
    }

    TEST(OpDo, ModEq)
    {
        TrackDoOp obj {};
        obj()->ints.append(5);
        EXPECT_EQ(obj->ints, std::vector{5});
        obj()->ints[0] %= 2;
        EXPECT_EQ(obj->ints, std::vector{1});
    }

    TEST(OpDo, XorEq)
    {
        TrackDoOp obj {};
        obj()->ints.append(0b01);
        EXPECT_EQ(obj->ints, std::vector{0b01});
        obj()->ints[0] ^= 0b11;
        EXPECT_EQ(obj->ints, std::vector{0b10});
    }

    TEST(OpDo, AndEq)
    {
        TrackDoOp obj {};
        obj()->ints.append(0b101);
        EXPECT_EQ(obj->ints, std::vector{0b101});
        obj()->ints[0] &= 0b011;
        EXPECT_EQ(obj->ints, std::vector{0b001});
    }

    TEST(OpDo, OrEq)
    {
        TrackDoOp obj {};
        obj()->ints.append(0b00);
        EXPECT_EQ(obj->ints, std::vector{0b00});
        obj()->ints[0] |= 0b11;
        EXPECT_EQ(obj->ints, std::vector{0b11});
    }

    TEST(OpDo, Append)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        obj()->ints.append(12);
        EXPECT_EQ(std::vector{12}, obj->ints);
        obj()->ints.append(13);
        auto expected = std::vector{12, 13};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, AppendN)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{12, 13};
        obj()->ints.append(values);
        EXPECT_EQ(values, obj->ints);
    }

    TEST(OpDo, Insert)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        obj()->ints.insert(4, 11);
        auto expected = std::vector{0, 1, 2, 3, 11, 4};
        EXPECT_EQ(expected, obj->ints);
        obj()->ints.insert(1, 22);
        expected = std::vector{0, 22, 1, 2, 3, 11, 4};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, InsertN)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        obj()->ints.insert(2, std::vector{11, 12, 13});
        auto expected = std::vector{0, 1, 11, 12, 13, 2, 3, 4};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, Remove)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 11, 22, 33, 44};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        obj()->ints.remove(2);
        const auto expected = std::vector{0, 11, 33, 44};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, RemoveN)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{0, 11, 22, 33, 44};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        obj()->ints.remove(std::vector{0, 2, 4});
        auto expected = std::vector{11, 33};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, RemoveL)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{0, 11, 22, 33, 44};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        obj()->ints.select(std::vector<std::size_t>{1, 3});
        obj()->ints.removeSelection();
        auto expected = std::vector{0, 22, 44};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, Sort)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{1, 9, 2, 8, 3, 7, 11, 5, 6, 4, 22};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        obj()->ints.sort();
        const auto expected = std::vector{1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 22};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, SortDesc)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{1, 9, 2, 8, 3, 7, 11, 5, 6, 4, 22};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        obj()->ints.sortDesc();
        const auto expected = std::vector{22, 11, 9, 8, 7, 6, 5, 4, 3, 2, 1};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, Swap)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.swap(0, 8);
        auto expected = std::vector{88, 11, 22, 33, 44, 55, 66, 77, 0};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, MoveUp)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveUp(4);
        auto expected = std::vector{0, 11, 22, 44, 33, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj()->ints.moveUp(0);
        EXPECT_EQ(expected, obj->ints);
        obj()->ints.moveUp(8);
        expected = std::vector{0, 11, 22, 44, 33, 55, 66, 88, 77};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, MoveUpN)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveUp(std::vector<std::size_t>{3, 4, 6});
        auto expected = std::vector{0, 11, 33, 44, 22, 66, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);

        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveUp(std::vector<std::size_t>{6, 4, 3});
        EXPECT_EQ(expected, obj->ints);

        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveUp(std::vector<std::size_t>{0, 8});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 88, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveUp(std::vector<std::size_t>{0, 1});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveUp(std::vector<std::size_t>{1, 0});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, MoveUpL)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{3, 4, 6});
        obj()->ints.moveSelectionsUp();
        auto expected = std::vector{0, 11, 33, 44, 22, 66, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);

        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{6, 4, 3});
        obj()->ints.moveSelectionsUp();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 8});
        obj()->ints.moveSelectionsUp();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 88, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 1});
        obj()->ints.moveSelectionsUp();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{1, 0});
        obj()->ints.moveSelectionsUp();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, MoveTop)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTop(4);
        auto expected = std::vector{44, 0, 11, 22, 33, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTop(0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);

        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTop(8);
        expected = std::vector{88, 0, 11, 22, 33, 44, 55, 66, 77};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, MoveTopN)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTop(std::vector<std::size_t>{0, 2, 4, 5, 7});
        auto expected = std::vector{0, 22, 44, 55, 77, 11, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTop(std::vector<std::size_t>{0, 8});
        expected = std::vector{0, 88, 11, 22, 33, 44, 55, 66, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTop(std::vector<std::size_t>{8, 0});
        expected = std::vector{0, 88, 11, 22, 33, 44, 55, 66, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTop(std::vector<std::size_t>{0, 1});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTop(std::vector<std::size_t>{1, 0});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, MoveTopL)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.moveSelectionsTop();
        auto expected = std::vector{0, 22, 44, 55, 77, 11, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 8});
        obj()->ints.moveSelectionsTop();
        expected = std::vector{0, 88, 11, 22, 33, 44, 55, 66, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{8, 0});
        obj()->ints.moveSelectionsTop();
        expected = std::vector{0, 88, 11, 22, 33, 44, 55, 66, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 1});
        obj()->ints.moveSelectionsTop();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{1, 0});
        obj()->ints.moveSelectionsTop();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, MoveDown)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveDown(4);
        auto expected = std::vector{0, 11, 22, 33, 55, 44, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);

        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveDown(0);
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);

        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveDown(8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, MoveDownN)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveDown(std::vector<std::size_t>{3, 4, 6});
        auto expected = std::vector{0, 11, 22, 55, 33, 44, 77, 66, 88};
        EXPECT_EQ(expected, obj->ints);

        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveDown(std::vector<std::size_t>{6, 4, 3});
        EXPECT_EQ(expected, obj->ints);

        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveDown(std::vector<std::size_t>{0, 8});
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveDown(std::vector<std::size_t>{7, 8});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveDown(std::vector<std::size_t>{8, 7});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, MoveDownL)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{3, 4, 6});
        obj()->ints.moveSelectionsDown();
        auto expected = std::vector{0, 11, 22, 55, 33, 44, 77, 66, 88};
        EXPECT_EQ(expected, obj->ints);

        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{6, 4, 3});
        obj()->ints.moveSelectionsDown();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 8});
        obj()->ints.moveSelectionsDown();
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{7, 8});
        obj()->ints.moveSelectionsDown();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{8, 7});
        obj()->ints.moveSelectionsDown();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, MoveBottom)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveBottom(4);
        auto expected = std::vector{0, 11, 22, 33, 55, 66, 77, 88, 44};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveBottom(0);
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 88, 0};
        EXPECT_EQ(expected, obj->ints);

        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveBottom(8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, MoveBottomN)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveBottom(std::vector<std::size_t>{0, 2, 4, 5, 7});
        auto expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveBottom(std::vector<std::size_t>{0, 8});
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 0, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveBottom(std::vector<std::size_t>{8, 0});
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 0, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveBottom(std::vector<std::size_t>{7, 8});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveBottom(std::vector<std::size_t>{8, 7});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, MoveBottomL)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.moveSelectionsBottom();
        auto expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);

        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 8});
        obj()->ints.moveSelectionsBottom();
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 0, 88};
        EXPECT_EQ(expected, obj->ints);

        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{8, 0});
        obj()->ints.moveSelectionsBottom();
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 0, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{7, 8});
        obj()->ints.moveSelectionsBottom();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{8, 7});
        obj()->ints.moveSelectionsBottom();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, MoveTo)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTo(4, 6);
        auto expected = std::vector{0, 11, 22, 33, 55, 66, 44, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTo(0, 0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTo(8, 8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTo(0, 1);
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTo(1, 0);
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTo(8, 7);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 88, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTo(7, 8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 88, 77};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, MoveToN)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 0);
        auto expected = std::vector{0, 22, 44, 55, 77, 11, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 1);
        expected = std::vector{11, 0, 22, 44, 55, 77, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 2);
        expected = std::vector{11, 33, 0, 22, 44, 55, 77, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 3);
        expected = std::vector{11, 33, 66, 0, 22, 44, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 4);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 5);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 6);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 7);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 8);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTo(std::vector<std::size_t>{0, 1}, 0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTo(std::vector<std::size_t>{1, 0}, 0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTo(std::vector<std::size_t>{0, 1, 3}, 0);
        expected = std::vector{0, 11, 33, 22, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTo(std::vector<std::size_t>{3, 1, 0}, 0);
        expected = std::vector{0, 11, 33, 22, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTo(std::vector<std::size_t>{7, 8}, 8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTo(std::vector<std::size_t>{8, 7}, 8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTo(std::vector<std::size_t>{5, 7, 8}, 8);
        expected = std::vector{0, 11, 22, 33, 44, 66, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.moveTo(std::vector<std::size_t>{8, 7, 5}, 8);
        expected = std::vector{0, 11, 22, 33, 44, 66, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpDo, MoveToL)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.moveSelectionsTo(0);
        auto expected = std::vector{0, 22, 44, 55, 77, 11, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.moveSelectionsTo(1);
        expected = std::vector{11, 0, 22, 44, 55, 77, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.moveSelectionsTo(2);
        expected = std::vector{11, 33, 0, 22, 44, 55, 77, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.moveSelectionsTo(3);
        expected = std::vector{11, 33, 66, 0, 22, 44, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.moveSelectionsTo(4);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.moveSelectionsTo(5);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.moveSelectionsTo(6);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.moveSelectionsTo(7);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.moveSelectionsTo(8);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 1});
        obj()->ints.moveSelectionsTo(0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{1, 0});
        obj()->ints.moveSelectionsTo(0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{0, 1, 3});
        obj()->ints.moveSelectionsTo(0);
        expected = std::vector{0, 11, 33, 22, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{3, 1, 0});
        obj()->ints.moveSelectionsTo(0);
        expected = std::vector{0, 11, 33, 22, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{7, 8});
        obj()->ints.moveSelectionsTo(8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{8, 7});
        obj()->ints.moveSelectionsTo(8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{5, 7, 8});
        obj()->ints.moveSelectionsTo(8);
        expected = std::vector{0, 11, 22, 33, 44, 66, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints.select(std::vector<std::size_t>{8, 7, 5});
        obj()->ints.moveSelectionsTo(8);
        expected = std::vector{0, 11, 22, 33, 44, 66, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
    }



    TEST(OpUndoRedo, Reset)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector<int>{0, 1, 2, 3, 4};
        obj()->ints = values;
        EXPECT_EQ(5, obj->ints.size());
        obj()->ints.reset();
        EXPECT_EQ(0, obj->ints.size());

        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(0, obj->ints.size());
    }

    TEST(OpUndoRedo, Reserve)
    {
        TrackDoOp obj {};
        std::size_t capacity = obj->ints.capacity();
        obj()->ints.reserve(capacity+1);
        EXPECT_EQ(capacity+1, obj->ints.capacity());

        obj.undoAction(); // Undo is a trim/shrink_to_fit
        EXPECT_EQ(0, obj->ints.capacity());
        obj.redoAction();
        EXPECT_EQ(capacity+1, obj->ints.capacity());
    }

    TEST(OpUndoRedo, Trim)
    {
        TrackDoOp obj {};
        std::size_t capacity = obj->ints.capacity();
        obj()->ints.reserve(capacity+1);
        EXPECT_EQ(capacity+1, obj->ints.capacity());
        obj()->ints.trim();
        EXPECT_EQ(0, obj->ints.capacity());

        obj.undoAction(); // Undo is a no-op
        EXPECT_EQ(0, obj->ints.capacity());
        obj.redoAction(); // Trimming again after a no-op undo results in no change
        EXPECT_EQ(0, obj->ints.capacity());
    }

    TEST(OpUndoRedo, Assign)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector<int>{3, 4, 5};
        EXPECT_EQ(3, obj->ints.size());
        obj()->ints.assign(5, 3);
        EXPECT_EQ(5, obj->ints.size());
        for ( auto i : obj->ints )
            EXPECT_EQ(3, i);

        obj.undoAction();
        EXPECT_EQ(3, obj->ints.size());
        obj.redoAction();
        EXPECT_EQ(5, obj->ints.size());
        for ( auto i : obj->ints )
            EXPECT_EQ(3, i);
    }

    TEST(OpUndoRedo, AssignDefault)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        obj()->ints.assignDefault(10);
        EXPECT_EQ(10, obj->ints.size());
        
        obj.undoAction();
        EXPECT_EQ(0, obj->ints.size());
        obj.redoAction();
        EXPECT_EQ(10, obj->ints.size());
    }

    TEST(OpUndoRedo, ClearSelection)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 1};
        EXPECT_EQ(2, obj->ints.size());
        EXPECT_EQ(0, obj.view.ints.sel().size());
        obj()->ints.select(1);
        EXPECT_EQ(std::vector<std::size_t>{1}, obj.view.ints.sel());
        obj()->ints.clearSelections();
        EXPECT_EQ(0, obj.view.ints.sel().size());

        obj.undoAction();
        EXPECT_EQ(std::vector<std::size_t>{1}, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(0, obj.view.ints.sel().size());
    }

    TEST(OpUndoRedo, SelectAll)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{22, 33};
        EXPECT_EQ(2, obj->ints.size());
        EXPECT_EQ(0, obj.view.ints.sel().size());
        obj()->ints.selectAll();
        const auto expected = std::vector<std::size_t>{0, 1};
        EXPECT_EQ(expected, obj.view.ints.sel());

        obj.undoAction();
        EXPECT_EQ(0, obj.view.ints.sel().size());
        obj.redoAction();
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(OpUndoRedo, Select)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{22, 33};
        EXPECT_EQ(2, obj->ints.size());
        EXPECT_EQ(0, obj.view.ints.sel().size());
        obj()->ints.select(1);
        EXPECT_EQ(std::vector<std::size_t>{1}, obj.view.ints.sel());

        obj.undoAction();
        EXPECT_EQ(0, obj.view.ints.sel().size());
        obj.redoAction();
        EXPECT_EQ(std::vector<std::size_t>{1}, obj.view.ints.sel());
    }

    TEST(OpUndoRedo, SelectN)
    {
        TrackDoOp obj {};
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

        obj.undoAction();
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(OpUndoRedo, Deselect)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        const auto selection = std::vector<std::size_t>{2, 4, 0, 1};
        obj()->ints.select(selection);
        EXPECT_EQ(5, obj->ints.size());
        EXPECT_EQ(4, obj.view.ints.sel().size());
        obj()->ints.deselect(4);
        const auto expected = std::vector<std::size_t>{2, 0, 1};
        EXPECT_EQ(expected, obj.view.ints.sel());

        obj.undoAction();
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(OpUndoRedo, DeselectN)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        const auto selection = std::vector<std::size_t>{2, 4, 0, 1};
        obj()->ints.select(selection);
        EXPECT_EQ(5, obj->ints.size());
        EXPECT_EQ(4, obj.view.ints.sel().size());
        obj()->ints.deselect(std::vector<std::size_t>{4, 2});
        const auto expected = std::vector<std::size_t>{0, 1};
        EXPECT_EQ(expected, obj.view.ints.sel());

        obj.undoAction();
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(OpUndoRedo, ToggleSelection)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        EXPECT_EQ(5, obj->ints.size());
        obj()->ints.toggleSelected(2);
        obj()->ints.toggleSelected(4);
        obj()->ints.toggleSelected(0);
        obj()->ints.toggleSelected(3);
        EXPECT_EQ(4, obj.view.ints.sel().size());
        auto expected = std::vector<std::size_t>{2, 4, 0, 3};
        EXPECT_EQ(expected, obj.view.ints.sel());
        obj()->ints.toggleSelected(4);
        obj()->ints.toggleSelected(2);
        EXPECT_EQ(2, obj.view.ints.sel().size());
        expected = {0, 3};
        EXPECT_EQ(expected, obj.view.ints.sel());
        
        obj.undoAction();
        obj.undoAction();
        expected = std::vector<std::size_t>{2, 4, 0, 3};
        EXPECT_EQ(expected, obj.view.ints.sel());
        obj.redoAction();
        obj.redoAction();
        expected = std::vector<std::size_t>{0, 3};
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(OpUndoRedo, ToggleSelectionN)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        EXPECT_EQ(5, obj->ints.size());
        obj()->ints.toggleSelected(std::vector<std::size_t>{2, 4, 0, 3});
        EXPECT_EQ(4, obj.view.ints.sel().size());
        auto expected = std::vector<std::size_t>{2, 4, 0, 3};
        EXPECT_EQ(expected, obj.view.ints.sel());
        obj()->ints.toggleSelected(std::vector<std::size_t>{4, 2});
        EXPECT_EQ(2, obj.view.ints.sel().size());
        expected = {0, 3};
        EXPECT_EQ(expected, obj.view.ints.sel());

        obj.undoAction();
        expected = std::vector<std::size_t>{2, 4, 0, 3};
        EXPECT_EQ(expected, obj.view.ints.sel());

        obj.redoAction();
        expected = std::vector<std::size_t>{0, 3};
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(OpUndoRedo, SortSelections)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        EXPECT_EQ(5, obj->ints.size());
        auto selection = std::vector<std::size_t>{2, 4, 0, 3};
        obj()->ints.select(selection);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj()->ints.sortSelection();
        const auto expected = std::vector<std::size_t>{0, 2, 3, 4};
        EXPECT_EQ(expected, obj.view.ints.sel());

        obj.undoAction();
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(OpUndoRedo, SortSelectionsDesc)
    {
        TrackDoOp obj {};
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        EXPECT_EQ(5, obj->ints.size());
        auto selection = std::vector<std::size_t>{2, 4, 0, 3};
        obj()->ints.select(selection);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj()->ints.sortSelectionDescending();
        const auto expected = std::vector<std::size_t>{4, 3, 2, 0};
        EXPECT_EQ(expected, obj.view.ints.sel());

        obj.undoAction();
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj.view.ints.sel());
    }

    TEST(OpUndoRedo, Set)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 1, 2, 3, 4};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        obj()->ints[2] = 22;
        const auto expected = std::vector{0, 1, 22, 3, 4};
        EXPECT_EQ(expected, obj->ints);

        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpUndoRedo, SetN)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 1, 2, 3, 4, 5};
        obj()->ints = values;
        EXPECT_EQ(6, obj->ints.size());
        obj()->ints.set(std::vector<std::size_t>{1, 3, 5}, 33);
        const auto expected = std::vector{0, 33, 2, 33, 4, 33};
        EXPECT_EQ(expected, obj->ints);

        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpUndoRedo, SetL)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 1, 2, 3, 4, 5};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        obj()->ints.select(std::vector<std::size_t>{1, 3, 5});
        obj()->ints.selection() = 33;
        const auto expected = std::vector{0, 33, 2, 33, 4, 33};
        EXPECT_EQ(expected, obj->ints);

        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
    }
    
    TEST(OpUndoRedo, PlusEq)
    {
        TrackDoOp obj {};
        obj()->ints.append(2);
        EXPECT_EQ(obj->ints, std::vector{2});
        obj()->ints[0] += 3;
        EXPECT_EQ(obj->ints, std::vector{5});

        obj.undoAction();
        EXPECT_EQ(obj->ints, std::vector{2});
        obj.redoAction();
        EXPECT_EQ(obj->ints, std::vector{5});
    }

    TEST(OpUndoRedo, MinusEq)
    {
        TrackDoOp obj {};
        obj()->ints.append(7);
        EXPECT_EQ(obj->ints, std::vector{7});
        obj()->ints[0] -= 2;
        EXPECT_EQ(obj->ints, std::vector{5});

        obj.undoAction();
        EXPECT_EQ(obj->ints, std::vector{7});
        obj.redoAction();
        EXPECT_EQ(obj->ints, std::vector{5});
    }

    TEST(OpUndoRedo, MultEq)
    {
        TrackDoOp obj {};
        obj()->ints.append(2);
        EXPECT_EQ(obj->ints, std::vector{2});
        obj()->ints[0] *= 2;
        EXPECT_EQ(obj->ints, std::vector{4});

        obj.undoAction();
        EXPECT_EQ(obj->ints, std::vector{2});
        obj.redoAction();
        EXPECT_EQ(obj->ints, std::vector{4});
    }

    TEST(OpUndoRedo, DivEq)
    {
        TrackDoOp obj {};
        obj()->ints.append(4);
        EXPECT_EQ(obj->ints, std::vector{4});
        obj()->ints[0] /= 2;
        EXPECT_EQ(obj->ints, std::vector{2});

        obj.undoAction();
        EXPECT_EQ(obj->ints, std::vector{4});
        obj.redoAction();
        EXPECT_EQ(obj->ints, std::vector{2});
    }

    TEST(OpUndoRedo, ModEq)
    {
        TrackDoOp obj {};
        obj()->ints.append(5);
        EXPECT_EQ(obj->ints, std::vector{5});
        obj()->ints[0] %= 2;
        EXPECT_EQ(obj->ints, std::vector{1});

        obj.undoAction();
        EXPECT_EQ(obj->ints, std::vector{5});
        obj.redoAction();
        EXPECT_EQ(obj->ints, std::vector{1});
    }

    TEST(OpUndoRedo, XorEq)
    {
        TrackDoOp obj {};
        obj()->ints.append(0b01);
        EXPECT_EQ(obj->ints, std::vector{0b01});
        obj()->ints[0] ^= 0b11;
        EXPECT_EQ(obj->ints, std::vector{0b10});

        obj.undoAction();
        EXPECT_EQ(obj->ints, std::vector{0b01});
        obj.redoAction();
        EXPECT_EQ(obj->ints, std::vector{0b10});
    }

    TEST(OpUndoRedo, AndEq)
    {
        TrackDoOp obj {};
        obj()->ints.append(0b101);
        EXPECT_EQ(obj->ints, std::vector{0b101});
        obj()->ints[0] &= 0b011;
        EXPECT_EQ(obj->ints, std::vector{0b001});

        obj.undoAction();
        EXPECT_EQ(obj->ints, std::vector{0b101});
        obj.redoAction();
        EXPECT_EQ(obj->ints, std::vector{0b001});
    }

    TEST(OpUndoRedo, OrEq)
    {
        TrackDoOp obj {};
        obj()->ints.append(0b00);
        EXPECT_EQ(obj->ints, std::vector{0b00});
        obj()->ints[0] |= 0b11;
        EXPECT_EQ(obj->ints, std::vector{0b11});

        obj.undoAction();
        EXPECT_EQ(obj->ints, std::vector{0b00});
        obj.redoAction();
        EXPECT_EQ(obj->ints, std::vector{0b11});
    }

    TEST(OpUndoRedo, Append)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        obj()->ints.append(12);
        EXPECT_EQ(std::vector{12}, obj->ints);
        obj()->ints.append(13);
        auto expected = std::vector{12, 13};
        EXPECT_EQ(expected, obj->ints);

        obj.undoAction();
        EXPECT_EQ(std::vector{12}, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpUndoRedo, AppendN)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{12, 13};
        obj()->ints.append(values);
        EXPECT_EQ(values, obj->ints);

        obj.undoAction();
        EXPECT_EQ(0, obj->ints.size());
        obj.redoAction();
        EXPECT_EQ(values, obj->ints);
    }

    TEST(OpUndoRedo, Insert)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        obj()->ints.insert(4, 11);
        auto firstExpected = std::vector{0, 1, 2, 3, 11, 4};
        EXPECT_EQ(firstExpected, obj->ints);
        obj()->ints.insert(1, 22);
        auto secondExpected = std::vector{0, 22, 1, 2, 3, 11, 4};
        EXPECT_EQ(secondExpected, obj->ints);

        obj.undoAction();
        EXPECT_EQ(firstExpected, obj->ints);
        obj.redoAction();
        EXPECT_EQ(secondExpected, obj->ints);
    }

    TEST(OpUndoRedo, InsertN)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{0, 1, 2, 3, 4};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        obj()->ints.insert(2, std::vector{11, 12, 13});
        auto expected = std::vector{0, 1, 11, 12, 13, 2, 3, 4};
        EXPECT_EQ(expected, obj->ints);

        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpUndoRedo, Remove)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 11, 22, 33, 44};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        obj()->ints.remove(2);
        const auto expected = std::vector{0, 11, 33, 44};
        EXPECT_EQ(expected, obj->ints);

        obj.undoAction();
        EXPECT_EQ(values, obj->ints);

        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpUndoRedo, RemoveN)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{0, 11, 22, 33, 44};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        const auto selection = std::vector<std::size_t>{4, 2};
        obj()->ints.select(selection);
        obj()->ints.remove(std::vector{0, 2, 4});
        auto expected = std::vector{11, 33};
        EXPECT_EQ(expected, obj->ints);

        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(0, obj.view.ints.sel().size());
    }

    TEST(OpUndoRedo, RemoveL)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 11, 22, 33, 44};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        const auto selection = std::vector<std::size_t>{1, 3};
        obj()->ints.select(selection);
        obj()->ints.removeSelection();
        const auto expected = std::vector{0, 22, 44};
        EXPECT_EQ(expected, obj->ints);

        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());

        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(0, obj.view.ints.sel().size());
    }

    TEST(OpUndoRedo, Sort)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{1, 9, 2, 8, 3, 7, 11, 5, 6, 4, 22};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        obj()->ints.sort();
        const auto expected = std::vector{1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 22};
        EXPECT_EQ(expected, obj->ints);

        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpUndoRedo, SortDesc)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{1, 9, 2, 8, 3, 7, 11, 5, 6, 4, 22};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        obj()->ints.sortDesc();
        const auto expected = std::vector{22, 11, 9, 8, 7, 6, 5, 4, 3, 2, 1};
        EXPECT_EQ(expected, obj->ints);

        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpUndoRedo, Swap)
    {
        TrackDoOp obj {};
        const auto first = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = first;
        EXPECT_EQ(first, obj->ints);

        obj()->ints.swap(0, 8);
        const auto second = std::vector{88, 11, 22, 33, 44, 55, 66, 77, 0};
        EXPECT_EQ(second, obj->ints);

        obj.undoAction();
        EXPECT_EQ(first, obj->ints);
        obj.redoAction();
        EXPECT_EQ(second, obj->ints);
    }

    TEST(OpUndoRedo, MoveUp)
    {
        TrackDoOp obj {};
        const auto first = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = first;
        EXPECT_EQ(first, obj->ints);

        obj()->ints.moveUp(4);
        const auto second = std::vector{0, 11, 22, 44, 33, 55, 66, 77, 88};
        EXPECT_EQ(second, obj->ints);

        obj()->ints.moveUp(0);
        EXPECT_EQ(second, obj->ints);

        obj()->ints.moveUp(8);
        const auto third = std::vector{0, 11, 22, 44, 33, 55, 66, 88, 77};
        EXPECT_EQ(third, obj->ints);

        obj.undoAction(); // Undo moveUp(8)
        EXPECT_EQ(second, obj->ints);
        obj.undoAction(); // Undo moveUp(0)
        EXPECT_EQ(second, obj->ints);
        obj.undoAction(); // Undo moveUp(4)
        EXPECT_EQ(first, obj->ints);
        obj.redoAction();
        EXPECT_EQ(second, obj->ints);
        obj.redoAction();
        EXPECT_EQ(second, obj->ints);
        obj.redoAction();
        EXPECT_EQ(third, obj->ints);
    }

    TEST(OpUndoRedo, MoveUpN)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.moveUp(std::vector<std::size_t>{3, 4, 6});
        auto expected = std::vector{0, 11, 33, 44, 22, 66, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);

        obj()->ints = values;
        obj()->ints.moveUp(std::vector<std::size_t>{6, 4, 3});
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);

        obj()->ints = values;
        obj()->ints.moveUp(std::vector<std::size_t>{0, 8});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 88, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveUp(std::vector<std::size_t>{0, 1});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveUp(std::vector<std::size_t>{1, 0});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpUndoRedo, MoveUpL)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{3, 4, 6});
        obj()->ints.moveSelectionsUp();
        auto expected = std::vector{0, 11, 33, 44, 22, 66, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);

        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{6, 4, 3});
        obj()->ints.moveSelectionsUp();
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 8});
        obj()->ints.moveSelectionsUp();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 88, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 1});
        obj()->ints.moveSelectionsUp();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{1, 0});
        obj()->ints.moveSelectionsUp();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpUndoRedo, MoveTop)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.moveTop(4);
        auto expected = std::vector{44, 0, 11, 22, 33, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTop(0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTop(8);
        expected = std::vector{88, 0, 11, 22, 33, 44, 55, 66, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpUndoRedo, MoveTopN)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.moveTop(std::vector<std::size_t>{0, 2, 4, 5, 7});
        auto expected = std::vector{0, 22, 44, 55, 77, 11, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTop(std::vector<std::size_t>{0, 1, 2, 4, 5, 7});
        expected = std::vector{0, 11, 22, 44, 55, 77, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTop(std::vector<std::size_t>{0, 8});
        expected = std::vector{0, 88, 11, 22, 33, 44, 55, 66, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTop(std::vector<std::size_t>{8, 0});
        expected = std::vector{0, 88, 11, 22, 33, 44, 55, 66, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTop(std::vector<std::size_t>{0, 1});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTop(std::vector<std::size_t>{1, 0});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpUndoRedo, MoveTopL)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.moveSelectionsTop();
        auto expected = std::vector{0, 22, 44, 55, 77, 11, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 8});
        obj()->ints.moveSelectionsTop();
        expected = std::vector{0, 88, 11, 22, 33, 44, 55, 66, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{8, 0});
        obj()->ints.moveSelectionsTop();
        expected = std::vector{0, 88, 11, 22, 33, 44, 55, 66, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 1});
        obj()->ints.moveSelectionsTop();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{1, 0});
        obj()->ints.moveSelectionsTop();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpUndoRedo, MoveDown)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.moveDown(4);
        auto expected = std::vector{0, 11, 22, 33, 55, 44, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveDown(0);
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveDown(8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpUndoRedo, MoveDownN)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.moveDown(std::vector<std::size_t>{3, 4, 6});
        auto expected = std::vector{0, 11, 22, 55, 33, 44, 77, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveDown(std::vector<std::size_t>{6, 4, 3});
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveDown(std::vector<std::size_t>{0, 8});
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveDown(std::vector<std::size_t>{7, 8});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveDown(std::vector<std::size_t>{8, 7});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpUndoRedo, MoveDownL)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{3, 4, 6});
        obj()->ints.moveSelectionsDown();
        auto expected = std::vector{0, 11, 22, 55, 33, 44, 77, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);

        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{6, 4, 3});
        obj()->ints.moveSelectionsDown();
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 8});
        obj()->ints.moveSelectionsDown();
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{7, 8});
        obj()->ints.moveSelectionsDown();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{8, 7});
        obj()->ints.moveSelectionsDown();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpUndoRedo, MoveBottom)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.moveBottom(4);
        auto expected = std::vector{0, 11, 22, 33, 55, 66, 77, 88, 44};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveBottom(0);
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 88, 0};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveBottom(8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpUndoRedo, MoveBottomN)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.moveBottom(std::vector<std::size_t>{0, 2, 4, 5, 7});
        auto expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveBottom(std::vector<std::size_t>{0, 8});
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 0, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveBottom(std::vector<std::size_t>{8, 0});
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 0, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveBottom(std::vector<std::size_t>{7, 8});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveBottom(std::vector<std::size_t>{8, 7});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpUndoRedo, MoveBottomL)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.moveSelectionsBottom();
        auto expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);

        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 8});
        obj()->ints.moveSelectionsBottom();
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 0, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);

        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{8, 0});
        obj()->ints.moveSelectionsBottom();
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 0, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{7, 8});
        obj()->ints.moveSelectionsBottom();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{8, 7});
        obj()->ints.moveSelectionsBottom();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpUndoRedo, MoveTo)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.moveTo(4, 6);
        auto expected = std::vector{0, 11, 22, 33, 55, 66, 44, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTo(0, 0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTo(8, 8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTo(0, 1);
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTo(1, 0);
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTo(8, 7);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 88, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTo(7, 8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 88, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpUndoRedo, MoveToN)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 0);
        auto expected = std::vector{0, 22, 44, 55, 77, 11, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 1);
        expected = std::vector{11, 0, 22, 44, 55, 77, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 2);
        expected = std::vector{11, 33, 0, 22, 44, 55, 77, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 3);
        expected = std::vector{11, 33, 66, 0, 22, 44, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 4);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 5);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 6);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 7);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 8);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTo(std::vector<std::size_t>{0, 1}, 0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTo(std::vector<std::size_t>{1, 0}, 0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTo(std::vector<std::size_t>{0, 1, 3}, 0);
        expected = std::vector{0, 11, 33, 22, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTo(std::vector<std::size_t>{3, 1, 0}, 0);
        expected = std::vector{0, 11, 33, 22, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTo(std::vector<std::size_t>{7, 8}, 8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTo(std::vector<std::size_t>{8, 7}, 8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTo(std::vector<std::size_t>{5, 7, 8}, 8);
        expected = std::vector{0, 11, 22, 33, 44, 66, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints = values;
        obj()->ints.moveTo(std::vector<std::size_t>{8, 7, 5}, 8);
        expected = std::vector{0, 11, 22, 33, 44, 66, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
    }

    TEST(OpUndoRedo, MoveToL)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.moveSelectionsTo(0);
        auto expected = std::vector{0, 22, 44, 55, 77, 11, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.moveSelectionsTo(1);
        expected = std::vector{11, 0, 22, 44, 55, 77, 33, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.moveSelectionsTo(2);
        expected = std::vector{11, 33, 0, 22, 44, 55, 77, 66, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.moveSelectionsTo(3);
        expected = std::vector{11, 33, 66, 0, 22, 44, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.moveSelectionsTo(4);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.moveSelectionsTo(5);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.moveSelectionsTo(6);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.moveSelectionsTo(7);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 2, 4, 5, 7});
        obj()->ints.moveSelectionsTo(8);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 1});
        obj()->ints.moveSelectionsTo(0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{1, 0});
        obj()->ints.moveSelectionsTo(0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{0, 1, 3});
        obj()->ints.moveSelectionsTo(0);
        expected = std::vector{0, 11, 33, 22, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{3, 1, 0});
        obj()->ints.moveSelectionsTo(0);
        expected = std::vector{0, 11, 33, 22, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{7, 8});
        obj()->ints.moveSelectionsTo(8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{8, 7});
        obj()->ints.moveSelectionsTo(8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{5, 7, 8});
        obj()->ints.moveSelectionsTo(8);
        expected = std::vector{0, 11, 22, 33, 44, 66, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        obj()->ints.select(std::vector<std::size_t>{8, 7, 5});
        obj()->ints.moveSelectionsTo(8);
        expected = std::vector{0, 11, 22, 33, 44, 66, 55, 77, 88};
        EXPECT_EQ(expected, obj->ints);
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
    }



    TEST(OpSelSync, Reset)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector<int>{0, 1, 2, 3, 4};
        obj()->ints = values;
        auto selections = std::vector<std::size_t>{2, 4, 0};
        obj()->ints.select(selections);
        EXPECT_EQ(5, obj->ints.size());
        obj()->ints.reset();
        EXPECT_EQ(0, obj->ints.size());
        EXPECT_EQ(0, obj.view.ints.sel().size());

        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(0, obj->ints.size());
        EXPECT_EQ(0, obj.view.ints.sel().size());
    }

    TEST(OpSelSync, Assign)
    {
        TrackDoOp obj {};
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

        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(5, obj->ints.size());
        for ( auto i : obj->ints )
            EXPECT_EQ(3, i);
        EXPECT_EQ(0, obj.view.ints.sel().size());
    }

    TEST(OpSelSync, AssignDefault)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 1, 2, 3, 4};
        obj()->ints = values;
        auto selections = std::vector<std::size_t>{2, 4, 0};
        obj()->ints.select(selections);
        obj()->ints.assignDefault(10);
        EXPECT_EQ(10, obj->ints.size());
        EXPECT_EQ(0, obj.view.ints.sel().size());
        
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(10, obj->ints.size());
        EXPECT_EQ(0, obj.view.ints.sel().size());
    }

    TEST(OpSelSync, Set)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 1, 2, 3, 4};
        obj()->ints = values;
        const auto selections = std::vector<std::size_t>{0, 2, 4};
        obj()->ints.select(selections);
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        const auto newValues = std::vector{0, 1, 2, 3};
        obj()->ints = newValues;
        EXPECT_EQ(newValues, obj->ints);
        EXPECT_EQ(0, obj.view.ints.sel().size());

        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(newValues, obj->ints);
        EXPECT_EQ(0, obj.view.ints.sel().size());
    }

    TEST(OpSelSync, Insert)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        obj()->ints = std::vector{0, 1, 2, 3, 4};
        const auto selection = std::vector<std::size_t>{0, 3, 4};
        obj()->ints.select(selection);
        obj()->ints.insert(4, 11);
        const auto firstExpected = std::vector{0, 1, 2, 3, 11, 4};
        const auto firstExpectedSelection = std::vector<std::size_t>{0, 3, 5};
        EXPECT_EQ(firstExpected, obj->ints);
        EXPECT_EQ(firstExpectedSelection, obj.view.ints.sel());
        obj()->ints.insert(1, 22);
        const auto secondExpected = std::vector{0, 22, 1, 2, 3, 11, 4};
        const auto secondExpectedSelection = std::vector<std::size_t>{0, 4, 6};
        EXPECT_EQ(secondExpected, obj->ints);
        EXPECT_EQ(secondExpectedSelection, obj.view.ints.sel());

        obj.undoAction();
        EXPECT_EQ(firstExpected, obj->ints);
        EXPECT_EQ(firstExpectedSelection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(secondExpected, obj->ints);
        EXPECT_EQ(secondExpectedSelection, obj.view.ints.sel());
    }

    TEST(OpSelSync, InsertN)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{0, 1, 2, 3, 4};
        obj()->ints = values;
        const auto selection = std::vector<std::size_t>{0, 2, 4};
        obj()->ints.select(selection);
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj()->ints.insert(2, std::vector{11, 12, 13});
        auto expected = std::vector{0, 1, 11, 12, 13, 2, 3, 4};
        auto expectedSelection = std::vector<std::size_t>{0, 5, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSelection, obj.view.ints.sel());

        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSelection, obj.view.ints.sel());
    }

    TEST(OpSelSync, Remove)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 11, 22, 33, 44};
        obj()->ints = values;
        const auto selection = std::vector<std::size_t>{0, 2, 4};
        obj()->ints.select(selection);
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj()->ints.remove(2);
        const auto expected = std::vector{0, 11, 33, 44};
        auto expectedSelection = std::vector<std::size_t>{0, 3};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSelection, obj.view.ints.sel());

        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());

        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSelection, obj.view.ints.sel());
    }

    TEST(OpSelSync, RemoveN)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{0, 11, 22, 33, 44};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        const auto selection = std::vector<std::size_t>{3, 2};
        obj()->ints.select(selection);
        obj()->ints.remove(std::vector{0, 2, 4});
        auto expected = std::vector{11, 33};
        const auto expectedSelection = std::vector<std::size_t>{1};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSelection, obj.view.ints.sel());

        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSelection, obj.view.ints.sel());
    }

    TEST(OpSelSync, RemoveL)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        const auto values = std::vector{0, 11, 22, 33, 44};
        obj()->ints = values;
        EXPECT_EQ(values, obj->ints);
        const auto selection = std::vector<std::size_t>{1, 3};
        obj()->ints.select(selection);
        obj()->ints.removeSelection();
        const auto expected = std::vector{0, 22, 44};
        EXPECT_EQ(expected, obj->ints);

        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());

        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(0, obj.view.ints.sel().size());
    }

    TEST(OpSelSync, Sort)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{1, 9, 2, 8, 3, 7, 11, 5, 6, 4, 22};
        obj()->ints = values;
        auto selection = std::vector<std::size_t>{0, 2, 4};
        obj()->ints.select(selection);
        EXPECT_EQ(values, obj->ints);
        obj()->ints.sort();
        const auto expected = std::vector{1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 22};
        const auto expectedSel = std::vector<std::size_t>{0, 1, 2};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());

        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());

        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
    }

    TEST(OpSelSync, SortDesc)
    {
        TrackDoOp obj {};
        EXPECT_EQ(0, obj->ints.size());
        auto values = std::vector{1, 9, 2, 8, 3, 7, 11, 5, 6, 4, 22};
        obj()->ints = values;
        auto selection = std::vector<std::size_t>{0, 2, 4};
        obj()->ints.select(selection);
        EXPECT_EQ(values, obj->ints);
        obj()->ints.sortDesc();
        const auto expected = std::vector{22, 11, 9, 8, 7, 6, 5, 4, 3, 2, 1};
        const auto expectedSel = std::vector<std::size_t>{8, 9, 10};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());

        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
    }

    TEST(OpSelSync, Swap)
    {
        TrackDoOp obj {};
        const auto first = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        const auto firstSel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        obj()->ints = first;
        obj()->ints.select(firstSel);
        EXPECT_EQ(first, obj->ints);
        EXPECT_EQ(firstSel, obj.view.ints.sel());

        obj()->ints.swap(0, 8);
        const auto second = std::vector{88, 11, 22, 33, 44, 55, 66, 77, 0};
        const auto secondSel = std::vector<std::size_t>{8, 1, 3, 4, 7, 0};
        EXPECT_EQ(second, obj->ints);
        EXPECT_EQ(secondSel, obj.view.ints.sel());

        obj.undoAction();
        EXPECT_EQ(first, obj->ints);
        EXPECT_EQ(firstSel, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(second, obj->ints);
        EXPECT_EQ(secondSel, obj.view.ints.sel());
    }

    TEST(OpSelSync, MoveUp)
    {
        TrackDoOp obj {};
        const auto first = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        const auto firstSel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        obj()->ints = first;
        obj()->ints.select(firstSel);
        EXPECT_EQ(first, obj->ints);
        EXPECT_EQ(firstSel, obj.view.ints.sel());

        obj()->ints.moveUp(4);
        const auto second = std::vector{0, 11, 22, 44, 33, 55, 66, 77, 88};
        const auto secondSel = std::vector<std::size_t>{0, 1, 4, 3, 7, 8};
        EXPECT_EQ(second, obj->ints);
        EXPECT_EQ(secondSel, obj.view.ints.sel());

        obj()->ints.moveUp(0);
        EXPECT_EQ(second, obj->ints);
        EXPECT_EQ(secondSel, obj.view.ints.sel());

        obj()->ints.moveUp(8);
        const auto third = std::vector{0, 11, 22, 44, 33, 55, 66, 88, 77};
        const auto thirdSel = std::vector<std::size_t>{0, 1, 4, 3, 8, 7};
        EXPECT_EQ(third, obj->ints);
        EXPECT_EQ(thirdSel, obj.view.ints.sel());

        obj.undoAction(); // Undo moveUp(8)
        EXPECT_EQ(second, obj->ints);
        EXPECT_EQ(secondSel, obj.view.ints.sel());
        obj.undoAction(); // Undo moveUp(0)
        EXPECT_EQ(second, obj->ints);
        EXPECT_EQ(secondSel, obj.view.ints.sel());
        obj.undoAction(); // Undo moveUp(4)
        EXPECT_EQ(first, obj->ints);
        EXPECT_EQ(firstSel, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(second, obj->ints);
        EXPECT_EQ(secondSel, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(second, obj->ints);
        EXPECT_EQ(secondSel, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(third, obj->ints);
        EXPECT_EQ(thirdSel, obj.view.ints.sel());
    }

    TEST(OpSelSync, MoveUpN)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        const auto selection = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.moveUp(std::vector<std::size_t>{3, 4, 6});
        auto expected = std::vector{0, 11, 33, 44, 22, 66, 55, 77, 88};
        auto expectedSel = std::vector<std::size_t>{0, 1, 2, 3, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());

        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.moveUp(std::vector<std::size_t>{6, 4, 3});
        expectedSel = std::vector<std::size_t>{0, 1, 2, 3, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());

        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.moveUp(std::vector<std::size_t>{0, 8});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 88, 77};
        expectedSel = std::vector<std::size_t>{0, 1, 3, 4, 8, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.moveUp(std::vector<std::size_t>{0, 1});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.moveUp(std::vector<std::size_t>{1, 0});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
    }

    TEST(OpSelSync, MoveUpL)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        auto selection = std::vector<std::size_t>{3, 4, 6};
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.moveSelectionsUp();
        auto expected = std::vector{0, 11, 33, 44, 22, 66, 55, 77, 88};
        auto expectedSel = std::vector<std::size_t>{2, 3, 5};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());

        obj()->ints.clearSelections();
        obj()->ints = values;
        selection = std::vector<std::size_t>{6, 4, 3};
        obj()->ints.select(selection);
        obj()->ints.moveSelectionsUp();
        expectedSel = std::vector<std::size_t>{5, 3, 2};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selection = std::vector<std::size_t>{0, 8};
        obj()->ints.select(selection);
        obj()->ints.moveSelectionsUp();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 88, 77};
        expectedSel = std::vector<std::size_t>{0, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selection = std::vector<std::size_t>{0, 1};
        obj()->ints.select(selection);
        obj()->ints.moveSelectionsUp();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selection = std::vector<std::size_t>{1, 0};
        obj()->ints.select(selection);
        obj()->ints.moveSelectionsUp();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{1, 0};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
    }

    TEST(OpSelSync, MoveTop)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        const auto selection = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.moveTop(4);
        auto expected = std::vector{44, 0, 11, 22, 33, 55, 66, 77, 88};
        auto expectedSel = std::vector<std::size_t>{1, 2, 4, 0, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.moveTop(0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.moveTop(8);
        expected = std::vector{88, 0, 11, 22, 33, 44, 55, 66, 77};
        expectedSel = std::vector<std::size_t>{1, 2, 4, 5, 8, 0};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
    }

    TEST(OpSelSync, MoveTopN)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        const auto selection = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.moveTop(std::vector<std::size_t>{0, 2, 4, 5, 7});
        auto expected = std::vector{0, 22, 44, 55, 77, 11, 33, 66, 88};
        auto expectedSel = std::vector<std::size_t>{0, 5, 6, 2, 4, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.moveTop(std::vector<std::size_t>{0, 1, 2, 4, 5, 7});
        expected = std::vector{0, 11, 22, 44, 55, 77, 33, 66, 88};
        expectedSel = std::vector<std::size_t>{0, 1, 6, 3, 5, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.moveTop(std::vector<std::size_t>{0, 8});
        expected = std::vector{0, 88, 11, 22, 33, 44, 55, 66, 77};
        expectedSel = std::vector<std::size_t>{0, 2, 4, 5, 8, 1};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.moveTop(std::vector<std::size_t>{8, 0});
        expected = std::vector{0, 88, 11, 22, 33, 44, 55, 66, 77};
        expectedSel = std::vector<std::size_t>{0, 2, 4, 5, 8, 1};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.moveTop(std::vector<std::size_t>{0, 1});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.moveTop(std::vector<std::size_t>{1, 0});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
    }

    TEST(OpSelSync, MoveTopL)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        auto selection = std::vector<std::size_t>{0, 2, 4, 5, 7};
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.moveSelectionsTop();
        auto expected = std::vector{0, 22, 44, 55, 77, 11, 33, 66, 88};
        auto expectedSel = std::vector<std::size_t>{0, 1, 2, 3, 4};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selection = std::vector<std::size_t>{0, 8};
        obj()->ints.select(selection);
        obj()->ints.moveSelectionsTop();
        expected = std::vector{0, 88, 11, 22, 33, 44, 55, 66, 77};
        expectedSel = std::vector<std::size_t>{0, 1};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selection = std::vector<std::size_t>{8, 0};
        obj()->ints.select(selection);
        obj()->ints.moveSelectionsTop();
        expected = std::vector{0, 88, 11, 22, 33, 44, 55, 66, 77};
        expectedSel = std::vector<std::size_t>{1, 0};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selection = std::vector<std::size_t>{0, 1};
        obj()->ints.select(selection);
        obj()->ints.moveSelectionsTop();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selection = std::vector<std::size_t>{1, 0};
        obj()->ints.select(selection);
        obj()->ints.moveSelectionsTop();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{1, 0};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
    }

    TEST(OpSelSync, MoveDown)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        const auto selection = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.moveDown(4);
        auto expected = std::vector{0, 11, 22, 33, 55, 44, 66, 77, 88};
        auto expectedSel = std::vector<std::size_t>{0, 1, 3, 5, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.moveDown(0);
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{1, 0, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.moveDown(8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
    }

    TEST(OpSelSync, MoveDownN)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        const auto selection = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.moveDown(std::vector<std::size_t>{3, 4, 6});
        auto expected = std::vector{0, 11, 22, 55, 33, 44, 77, 66, 88};
        auto expectedSel = std::vector<std::size_t>{0, 1, 4, 5, 6, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.moveDown(std::vector<std::size_t>{6, 4, 3});
        expectedSel = std::vector<std::size_t>{0, 1, 4, 5, 6, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.moveDown(std::vector<std::size_t>{0, 8});
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{1, 0, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.moveDown(std::vector<std::size_t>{7, 8});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selection);
        obj()->ints.moveDown(std::vector<std::size_t>{8, 7});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
    }

    TEST(OpSelSync, MoveDownL)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        auto selection = std::vector<std::size_t>{3, 4, 6};
        obj()->ints.select(selection);
        obj()->ints.moveSelectionsDown();
        auto expected = std::vector{0, 11, 22, 55, 33, 44, 77, 66, 88};
        auto expectedSel = std::vector<std::size_t>{4, 5, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());

        obj()->ints.clearSelections();
        obj()->ints = values;
        selection = std::vector<std::size_t>{6, 4, 3};
        obj()->ints.select(selection);
        obj()->ints.moveSelectionsDown();
        expectedSel = std::vector<std::size_t>{7, 5, 4};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selection = std::vector<std::size_t>{0, 8};
        obj()->ints.select(selection);
        obj()->ints.moveSelectionsDown();
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{1, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selection = std::vector<std::size_t>{7, 8};
        obj()->ints.select(selection);
        obj()->ints.moveSelectionsDown();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selection = std::vector<std::size_t>{8, 7};
        obj()->ints.select(selection);
        obj()->ints.moveSelectionsDown();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{8, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selection, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
    }

    TEST(OpSelSync, MoveBottom)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        const auto selections = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveBottom(4);
        auto expected = std::vector{0, 11, 22, 33, 55, 66, 77, 88, 44};
        auto expectedSel = std::vector<std::size_t>{0, 1, 3, 8, 6, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveBottom(0);
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 88, 0};
        expectedSel = std::vector<std::size_t>{8, 0, 2, 3, 6, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveBottom(8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
    }

    TEST(OpSelSync, MoveBottomN)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        const auto selections = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveBottom(std::vector<std::size_t>{0, 2, 4, 5, 7});
        auto expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        auto expectedSel = std::vector<std::size_t>{4, 0, 1, 6, 8, 3};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveBottom(std::vector<std::size_t>{0, 8});
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 0, 88};
        expectedSel = std::vector<std::size_t>{7, 0, 2, 3, 6, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveBottom(std::vector<std::size_t>{8, 0});
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 0, 88};
        expectedSel = std::vector<std::size_t>{7, 0, 2, 3, 6, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveBottom(std::vector<std::size_t>{7, 8});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveBottom(std::vector<std::size_t>{8, 7});
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
    }

    TEST(OpSelSync, MoveBottomL)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        auto selections = std::vector<std::size_t>{0, 2, 4, 5, 7};
        obj()->ints.select(selections);
        obj()->ints.moveSelectionsBottom();
        auto expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        auto expectedSel = std::vector<std::size_t>{4, 5, 6, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());

        obj()->ints.clearSelections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{0, 8};
        obj()->ints.select(selections);
        obj()->ints.moveSelectionsBottom();
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 0, 88};
        expectedSel = std::vector<std::size_t>{7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());

        obj()->ints.clearSelections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{8, 0};
        obj()->ints.select(selections);
        obj()->ints.moveSelectionsBottom();
        expected = std::vector{11, 22, 33, 44, 55, 66, 77, 0, 88};
        expectedSel = std::vector<std::size_t>{8, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{7, 8};
        obj()->ints.select(selections);
        obj()->ints.moveSelectionsBottom();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{8, 7};
        obj()->ints.select(selections);
        obj()->ints.moveSelectionsBottom();
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{8, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
    }

    TEST(OpSelSync, MoveTo)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        const auto selections = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveTo(4, 6);
        auto expected = std::vector{0, 11, 22, 33, 55, 66, 44, 77, 88};
        auto expectedSel = std::vector<std::size_t>{0, 1, 3, 6, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveTo(0, 0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveTo(8, 8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveTo(0, 1);
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{1, 0, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveTo(1, 0);
        expected = std::vector{11, 0, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{1, 0, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveTo(8, 7);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 88, 77};
        expectedSel = std::vector<std::size_t>{0, 1, 3, 4, 8, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveTo(7, 8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 88, 77};
        expectedSel = std::vector<std::size_t>{0, 1, 3, 4, 8, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
    }

    TEST(OpSelSync, MoveToN)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        const auto selections = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 0);
        auto expected = std::vector{0, 22, 44, 55, 77, 11, 33, 66, 88};
        auto expectedSel = std::vector<std::size_t>{0, 5, 6, 2, 4, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 1);
        expected = std::vector{11, 0, 22, 44, 55, 77, 33, 66, 88};
        expectedSel = std::vector<std::size_t>{1, 0, 6, 3, 5, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 2);
        expected = std::vector{11, 33, 0, 22, 44, 55, 77, 66, 88};
        expectedSel = std::vector<std::size_t>{2, 0, 1, 4, 6, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 3);
        expected = std::vector{11, 33, 66, 0, 22, 44, 55, 77, 88};
        expectedSel = std::vector<std::size_t>{3, 0, 1, 5, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 4);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        expectedSel = std::vector<std::size_t>{4, 0, 1, 6, 8, 3};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 5);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        expectedSel = std::vector<std::size_t>{4, 0, 1, 6, 8, 3};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 6);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        expectedSel = std::vector<std::size_t>{4, 0, 1, 6, 8, 3};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 7);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        expectedSel = std::vector<std::size_t>{4, 0, 1, 6, 8, 3};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveTo(std::vector<std::size_t>{0, 2, 4, 5, 7}, 8);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        expectedSel = std::vector<std::size_t>{4, 0, 1, 6, 8, 3};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveTo(std::vector<std::size_t>{0, 1}, 0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveTo(std::vector<std::size_t>{1, 0}, 0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveTo(std::vector<std::size_t>{0, 1, 3}, 0);
        expected = std::vector{0, 11, 33, 22, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1, 2, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveTo(std::vector<std::size_t>{3, 1, 0}, 0);
        expected = std::vector{0, 11, 33, 22, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1, 2, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveTo(std::vector<std::size_t>{7, 8}, 8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveTo(std::vector<std::size_t>{8, 7}, 8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveTo(std::vector<std::size_t>{5, 7, 8}, 8);
        expected = std::vector{0, 11, 22, 33, 44, 66, 55, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints = values;
        obj()->ints.select(selections);
        obj()->ints.moveTo(std::vector<std::size_t>{8, 7, 5}, 8);
        expected = std::vector{0, 11, 22, 33, 44, 66, 55, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1, 3, 4, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
    }

    TEST(OpSelSync, MoveToL)
    {
        TrackDoOp obj {};
        const auto values = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        obj()->ints = values;
        auto selections = std::vector<std::size_t>{0, 2, 4, 5, 7};
        obj()->ints.select(selections);
        obj()->ints.moveSelectionsTo(0);
        auto expected = std::vector{0, 22, 44, 55, 77, 11, 33, 66, 88};
        auto expectedSel = std::vector<std::size_t>{0, 1, 2, 3, 4};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{0, 2, 4, 5, 7};
        obj()->ints.select(selections);
        obj()->ints.moveSelectionsTo(1);
        expected = std::vector{11, 0, 22, 44, 55, 77, 33, 66, 88};
        expectedSel = std::vector<std::size_t>{1, 2, 3, 4, 5};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{0, 2, 4, 5, 7};
        obj()->ints.select(selections);
        obj()->ints.moveSelectionsTo(2);
        expected = std::vector{11, 33, 0, 22, 44, 55, 77, 66, 88};
        expectedSel = std::vector<std::size_t>{2, 3, 4, 5, 6};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{0, 2, 4, 5, 7};
        obj()->ints.select(selections);
        obj()->ints.moveSelectionsTo(3);
        expected = std::vector{11, 33, 66, 0, 22, 44, 55, 77, 88};
        expectedSel = std::vector<std::size_t>{3, 4, 5, 6, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{0, 2, 4, 5, 7};
        obj()->ints.select(selections);
        obj()->ints.moveSelectionsTo(4);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        expectedSel = std::vector<std::size_t>{4, 5, 6, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{0, 2, 4, 5, 7};
        obj()->ints.select(selections);
        obj()->ints.moveSelectionsTo(5);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        expectedSel = std::vector<std::size_t>{4, 5, 6, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{0, 2, 4, 5, 7};
        obj()->ints.select(selections);
        obj()->ints.moveSelectionsTo(6);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        expectedSel = std::vector<std::size_t>{4, 5, 6, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{0, 2, 4, 5, 7};
        obj()->ints.select(selections);
        obj()->ints.moveSelectionsTo(7);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        expectedSel = std::vector<std::size_t>{4, 5, 6, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{0, 2, 4, 5, 7};
        obj()->ints.select(selections);
        obj()->ints.moveSelectionsTo(8);
        expected = std::vector{11, 33, 66, 88, 0, 22, 44, 55, 77};
        expectedSel = std::vector<std::size_t>{4, 5, 6, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{0, 1};
        obj()->ints.select(selections);
        obj()->ints.moveSelectionsTo(0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{1, 0};
        obj()->ints.select(selections);
        obj()->ints.moveSelectionsTo(0);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{1, 0};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{0, 1, 3};
        obj()->ints.select(selections);
        obj()->ints.moveSelectionsTo(0);
        expected = std::vector{0, 11, 33, 22, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{0, 1, 2};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{3, 1, 0};
        obj()->ints.select(selections);
        obj()->ints.moveSelectionsTo(0);
        expected = std::vector{0, 11, 33, 22, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{2, 1, 0};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{7, 8};
        obj()->ints.select(selections);
        obj()->ints.moveSelectionsTo(8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{8, 7};
        obj()->ints.select(selections);
        obj()->ints.moveSelectionsTo(8);
        expected = std::vector{0, 11, 22, 33, 44, 55, 66, 77, 88};
        expectedSel = std::vector<std::size_t>{8, 7};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{5, 7, 8};
        obj()->ints.select(selections);
        obj()->ints.moveSelectionsTo(8);
        expected = std::vector{0, 11, 22, 33, 44, 66, 55, 77, 88};
        expectedSel = std::vector<std::size_t>{6, 7, 8};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        
        obj()->ints.clearSelections();
        obj()->ints = values;
        selections = std::vector<std::size_t>{8, 7, 5};
        obj()->ints.select(selections);
        obj()->ints.moveSelectionsTo(8);
        expected = std::vector{0, 11, 22, 33, 44, 66, 55, 77, 88};
        expectedSel = std::vector<std::size_t>{8, 7, 6};
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
        obj.undoAction();
        EXPECT_EQ(values, obj->ints);
        EXPECT_EQ(selections, obj.view.ints.sel());
        obj.redoAction();
        EXPECT_EQ(expected, obj->ints);
        EXPECT_EQ(expectedSel, obj.view.ints.sel());
    }

    struct MdArrayTest
    {
        int twoD[3][2] {{11, 22}, {33, 44}, {55, 66}};
        int threeD[4][3][2] {
            {{11, 22}, {33, 44}, {55, 66}},
            {{111, 222}, {333, 444}, {555, 666}},
            {{1111, 2222}, {3333, 4444}, {5555, 6666}},
            {{11111, 22222}, {33333, 44444}, {55555, 66666}}
        };

        REFLECT(MdArrayTest, twoD, threeD)
    };

    struct EditMdArrayTest : Tracked<MdArrayTest, EditMdArrayTest>
    {
        EditMdArrayTest() : Tracked{this} {}
    };
    
    TEST(MdArray, Reset)
    {
        EditMdArrayTest myObj {};
        EXPECT_EQ(myObj->twoD[0][0], 11);
        EXPECT_EQ(myObj->twoD[0][1], 22);
        EXPECT_EQ(myObj->twoD[1][0], 33);
        EXPECT_EQ(myObj->twoD[1][1], 44);
        EXPECT_EQ(myObj->twoD[2][0], 55);
        EXPECT_EQ(myObj->twoD[2][1], 66);
        EXPECT_EQ(myObj->threeD[0][0][0], 11);
        EXPECT_EQ(myObj->threeD[0][0][1], 22);
        EXPECT_EQ(myObj->threeD[0][1][0], 33);
        EXPECT_EQ(myObj->threeD[0][1][1], 44);
        EXPECT_EQ(myObj->threeD[0][2][0], 55);
        EXPECT_EQ(myObj->threeD[0][2][1], 66);
        EXPECT_EQ(myObj->threeD[1][0][0], 111);
        EXPECT_EQ(myObj->threeD[1][0][1], 222);
        EXPECT_EQ(myObj->threeD[1][1][0], 333);
        EXPECT_EQ(myObj->threeD[1][1][1], 444);
        EXPECT_EQ(myObj->threeD[1][2][0], 555);
        EXPECT_EQ(myObj->threeD[1][2][1], 666);
        EXPECT_EQ(myObj->threeD[2][0][0], 1111);
        EXPECT_EQ(myObj->threeD[2][0][1], 2222);
        EXPECT_EQ(myObj->threeD[2][1][0], 3333);
        EXPECT_EQ(myObj->threeD[2][1][1], 4444);
        EXPECT_EQ(myObj->threeD[2][2][0], 5555);
        EXPECT_EQ(myObj->threeD[2][2][1], 6666);
        EXPECT_EQ(myObj->threeD[3][0][0], 11111);
        EXPECT_EQ(myObj->threeD[3][0][1], 22222);
        EXPECT_EQ(myObj->threeD[3][1][0], 33333);
        EXPECT_EQ(myObj->threeD[3][1][1], 44444);
        EXPECT_EQ(myObj->threeD[3][2][0], 55555);
        EXPECT_EQ(myObj->threeD[3][2][1], 66666);
        
        myObj()->twoD.reset();
        myObj()->threeD.reset();
        EXPECT_EQ(myObj->twoD[0][0], 0);
        EXPECT_EQ(myObj->twoD[0][1], 0);
        EXPECT_EQ(myObj->twoD[1][0], 0);
        EXPECT_EQ(myObj->twoD[1][1], 0);
        EXPECT_EQ(myObj->twoD[2][0], 0);
        EXPECT_EQ(myObj->twoD[2][1], 0);
        EXPECT_EQ(myObj->threeD[0][0][0], 0);
        EXPECT_EQ(myObj->threeD[0][0][1], 0);
        EXPECT_EQ(myObj->threeD[0][1][0], 0);
        EXPECT_EQ(myObj->threeD[0][1][1], 0);
        EXPECT_EQ(myObj->threeD[0][2][0], 0);
        EXPECT_EQ(myObj->threeD[0][2][1], 0);
        EXPECT_EQ(myObj->threeD[1][0][0], 0);
        EXPECT_EQ(myObj->threeD[1][0][1], 0);
        EXPECT_EQ(myObj->threeD[1][1][0], 0);
        EXPECT_EQ(myObj->threeD[1][1][1], 0);
        EXPECT_EQ(myObj->threeD[1][2][0], 0);
        EXPECT_EQ(myObj->threeD[1][2][1], 0);
        EXPECT_EQ(myObj->threeD[2][0][0], 0);
        EXPECT_EQ(myObj->threeD[2][0][1], 0);
        EXPECT_EQ(myObj->threeD[2][1][0], 0);
        EXPECT_EQ(myObj->threeD[2][1][1], 0);
        EXPECT_EQ(myObj->threeD[2][2][0], 0);
        EXPECT_EQ(myObj->threeD[2][2][1], 0);
        EXPECT_EQ(myObj->threeD[3][0][0], 0);
        EXPECT_EQ(myObj->threeD[3][0][1], 0);
        EXPECT_EQ(myObj->threeD[3][1][0], 0);
        EXPECT_EQ(myObj->threeD[3][1][1], 0);
        EXPECT_EQ(myObj->threeD[3][2][0], 0);
        EXPECT_EQ(myObj->threeD[3][2][1], 0);

        myObj.undoAction();
        myObj.undoAction();
        EXPECT_EQ(myObj->twoD[0][0], 11);
        EXPECT_EQ(myObj->twoD[0][1], 22);
        EXPECT_EQ(myObj->twoD[1][0], 33);
        EXPECT_EQ(myObj->twoD[1][1], 44);
        EXPECT_EQ(myObj->twoD[2][0], 55);
        EXPECT_EQ(myObj->twoD[2][1], 66);
        EXPECT_EQ(myObj->threeD[0][0][0], 11);
        EXPECT_EQ(myObj->threeD[0][0][1], 22);
        EXPECT_EQ(myObj->threeD[0][1][0], 33);
        EXPECT_EQ(myObj->threeD[0][1][1], 44);
        EXPECT_EQ(myObj->threeD[0][2][0], 55);
        EXPECT_EQ(myObj->threeD[0][2][1], 66);
        EXPECT_EQ(myObj->threeD[1][0][0], 111);
        EXPECT_EQ(myObj->threeD[1][0][1], 222);
        EXPECT_EQ(myObj->threeD[1][1][0], 333);
        EXPECT_EQ(myObj->threeD[1][1][1], 444);
        EXPECT_EQ(myObj->threeD[1][2][0], 555);
        EXPECT_EQ(myObj->threeD[1][2][1], 666);
        EXPECT_EQ(myObj->threeD[2][0][0], 1111);
        EXPECT_EQ(myObj->threeD[2][0][1], 2222);
        EXPECT_EQ(myObj->threeD[2][1][0], 3333);
        EXPECT_EQ(myObj->threeD[2][1][1], 4444);
        EXPECT_EQ(myObj->threeD[2][2][0], 5555);
        EXPECT_EQ(myObj->threeD[2][2][1], 6666);
        EXPECT_EQ(myObj->threeD[3][0][0], 11111);
        EXPECT_EQ(myObj->threeD[3][0][1], 22222);
        EXPECT_EQ(myObj->threeD[3][1][0], 33333);
        EXPECT_EQ(myObj->threeD[3][1][1], 44444);
        EXPECT_EQ(myObj->threeD[3][2][0], 55555);
        EXPECT_EQ(myObj->threeD[3][2][1], 66666);

        myObj.redoAction();
        myObj.redoAction();
        EXPECT_EQ(myObj->twoD[0][0], 0);
        EXPECT_EQ(myObj->twoD[0][1], 0);
        EXPECT_EQ(myObj->twoD[1][0], 0);
        EXPECT_EQ(myObj->twoD[1][1], 0);
        EXPECT_EQ(myObj->twoD[2][0], 0);
        EXPECT_EQ(myObj->twoD[2][1], 0);
        EXPECT_EQ(myObj->threeD[0][0][0], 0);
        EXPECT_EQ(myObj->threeD[0][0][1], 0);
        EXPECT_EQ(myObj->threeD[0][1][0], 0);
        EXPECT_EQ(myObj->threeD[0][1][1], 0);
        EXPECT_EQ(myObj->threeD[0][2][0], 0);
        EXPECT_EQ(myObj->threeD[0][2][1], 0);
        EXPECT_EQ(myObj->threeD[1][0][0], 0);
        EXPECT_EQ(myObj->threeD[1][0][1], 0);
        EXPECT_EQ(myObj->threeD[1][1][0], 0);
        EXPECT_EQ(myObj->threeD[1][1][1], 0);
        EXPECT_EQ(myObj->threeD[1][2][0], 0);
        EXPECT_EQ(myObj->threeD[1][2][1], 0);
        EXPECT_EQ(myObj->threeD[2][0][0], 0);
        EXPECT_EQ(myObj->threeD[2][0][1], 0);
        EXPECT_EQ(myObj->threeD[2][1][0], 0);
        EXPECT_EQ(myObj->threeD[2][1][1], 0);
        EXPECT_EQ(myObj->threeD[2][2][0], 0);
        EXPECT_EQ(myObj->threeD[2][2][1], 0);
        EXPECT_EQ(myObj->threeD[3][0][0], 0);
        EXPECT_EQ(myObj->threeD[3][0][1], 0);
        EXPECT_EQ(myObj->threeD[3][1][0], 0);
        EXPECT_EQ(myObj->threeD[3][1][1], 0);
        EXPECT_EQ(myObj->threeD[3][2][0], 0);
        EXPECT_EQ(myObj->threeD[3][2][1], 0);
    }

    TEST(MdArray, Set)
    {
        EditMdArrayTest myObj {};
        EXPECT_EQ(myObj->twoD[0][0], 11);
        EXPECT_EQ(myObj->twoD[0][1], 22);
        EXPECT_EQ(myObj->twoD[1][0], 33);
        EXPECT_EQ(myObj->twoD[1][1], 44);
        EXPECT_EQ(myObj->twoD[2][0], 55);
        EXPECT_EQ(myObj->twoD[2][1], 66);
        EXPECT_EQ(myObj->threeD[0][0][0], 11);
        EXPECT_EQ(myObj->threeD[0][0][1], 22);
        EXPECT_EQ(myObj->threeD[0][1][0], 33);
        EXPECT_EQ(myObj->threeD[0][1][1], 44);
        EXPECT_EQ(myObj->threeD[0][2][0], 55);
        EXPECT_EQ(myObj->threeD[0][2][1], 66);
        EXPECT_EQ(myObj->threeD[1][0][0], 111);
        EXPECT_EQ(myObj->threeD[1][0][1], 222);
        EXPECT_EQ(myObj->threeD[1][1][0], 333);
        EXPECT_EQ(myObj->threeD[1][1][1], 444);
        EXPECT_EQ(myObj->threeD[1][2][0], 555);
        EXPECT_EQ(myObj->threeD[1][2][1], 666);
        EXPECT_EQ(myObj->threeD[2][0][0], 1111);
        EXPECT_EQ(myObj->threeD[2][0][1], 2222);
        EXPECT_EQ(myObj->threeD[2][1][0], 3333);
        EXPECT_EQ(myObj->threeD[2][1][1], 4444);
        EXPECT_EQ(myObj->threeD[2][2][0], 5555);
        EXPECT_EQ(myObj->threeD[2][2][1], 6666);
        EXPECT_EQ(myObj->threeD[3][0][0], 11111);
        EXPECT_EQ(myObj->threeD[3][0][1], 22222);
        EXPECT_EQ(myObj->threeD[3][1][0], 33333);
        EXPECT_EQ(myObj->threeD[3][1][1], 44444);
        EXPECT_EQ(myObj->threeD[3][2][0], 55555);
        EXPECT_EQ(myObj->threeD[3][2][1], 66666);
        
        myObj()->twoD[2][1] = 77;
        myObj()->threeD[3][2][1] = 77777;
        EXPECT_EQ(myObj->twoD[2][1], 77);
        EXPECT_EQ(myObj->threeD[3][2][1], 77777);

        myObj.undoAction();
        myObj.undoAction();
        EXPECT_EQ(myObj->twoD[2][1], 66);
        EXPECT_EQ(myObj->threeD[3][2][1], 66666);

        myObj.redoAction();
        myObj.redoAction();
        EXPECT_EQ(myObj->twoD[2][1], 77);
        EXPECT_EQ(myObj->threeD[3][2][1], 77777);
    }

    struct InitDataTest
    {
        int a = 1;
        std::vector<int> b = {2, 3};

        REFLECT(InitDataTest, a, b)
    };

    struct EditInitDataTest : Tracked<InitDataTest, EditInitDataTest>
    {
        EditInitDataTest() : Tracked{this} {}
    };

    TEST(MiscEdits, InitData)
    {
        EditInitDataTest myObj {};
        EXPECT_EQ(1, myObj->a);
        std::vector<int> expectedVec {2, 3};
        EXPECT_EQ(expectedVec, myObj->b);

        myObj.initData<false>(InitDataTest {
            .a = 4,
            .b = {5, 6}
        });
        EXPECT_EQ(0, myObj.getCursorIndex());
        EXPECT_EQ(4, myObj->a);
        expectedVec = {5, 6};
        EXPECT_EQ(expectedVec, myObj->b);

        myObj.initData<true>(InitDataTest {
            .a = 7,
            .b = {8, 9}
        });
        EXPECT_EQ(1, myObj.getCursorIndex());
        EXPECT_EQ(7, myObj->a);
        expectedVec = {8, 9};
        EXPECT_EQ(expectedVec, myObj->b);

        myObj.undoAction();
        EXPECT_EQ(0, myObj.getCursorIndex());
        EXPECT_EQ(4, myObj->a);
        expectedVec = {5, 6};
        EXPECT_EQ(expectedVec, myObj->b);

        myObj.redoAction();
        EXPECT_EQ(1, myObj.getCursorIndex());
        EXPECT_EQ(7, myObj->a);
        expectedVec = {8, 9};
        EXPECT_EQ(expectedVec, myObj->b);
        
        EXPECT_ANY_THROW(myObj.initData<false>(InitDataTest{}));
        EXPECT_ANY_THROW(myObj.initData<true>(InitDataTest{}));

        myObj.clearHistory();
        myObj.initData<true>(InitDataTest{
            .a = 10,
            .b = {11, 12}
        });
        EXPECT_EQ(1, myObj.getCursorIndex());
        EXPECT_EQ(10, myObj->a);
        expectedVec = {11, 12};
        EXPECT_EQ(expectedVec, myObj->b);

        myObj.undoAction();
        EXPECT_EQ(0, myObj.getCursorIndex());
        EXPECT_EQ(7, myObj->a);
        expectedVec = {8, 9};
        EXPECT_EQ(expectedVec, myObj->b);

        myObj.redoAction();
        EXPECT_EQ(1, myObj.getCursorIndex());
        EXPECT_EQ(10, myObj->a);
        expectedVec = {11, 12};
        EXPECT_EQ(expectedVec, myObj->b);

        myObj.clearHistory();
        myObj.initData<false>(InitDataTest{
            .a = 13,
            .b = {14, 15}
        });
        EXPECT_EQ(0, myObj.getCursorIndex());
        EXPECT_EQ(13, myObj->a);
        expectedVec = {14, 15};
        EXPECT_EQ(expectedVec, myObj->b);
    }

    struct Code_
    {
        enum uint8_t_ : std::uint8_t
        {
            _U = 5, // Undo
            _R = 6, // Redo
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
        std::vector<std::uint8_t> priorStates {}; // Records the value just prior to running code [i]
        std::vector<std::uint8_t> states {}; // Records the current value after running codes up to [i]
        std::vector<std::uint64_t> netSize {}; // Records the netSize after running codes up to [i]
        std::vector<std::size_t> actionIndex {}; // Records the actionIndex at which code [i] starts
        std::size_t maxActions {}; // Records the totalActions after running all codes
    };
    struct TrimCaseData
    {
        std::uint8_t value = 0;

        REFLECT(TrimCaseData, value)
    };
    struct TrimCaseEditor : Tracked<TrimCaseData, TrimCaseEditor>
    {
        TrimCaseEditor() : Tracked(this) {}
    };

    TEST(MiscEdits, TrimHistory)
    {
        constexpr auto _U = Code::_U; // Undo
        constexpr auto _1 = Code::_1; // Bit 1
        constexpr auto _2 = Code::_2; // Bit 2
        constexpr auto _3 = Code::_3; // Bit 3
        constexpr auto _4 = Code::_4; // Bit 4
        constexpr auto _5 = Code::_5; // Bit 5
        constexpr auto _6 = Code::_6; // Bit 6
        std::vector<Case> testCases {
            Case { .codes = {}, .result = 0 },
            Case { .codes = {_1}, .result = _1 }, // A
            Case { .codes = {_1, _U, _2}, .result = _2 }, // AUA
            Case { .codes = {_1, _2}, .result = _1|_2 }, // AA
            Case { .codes = {_1, _2, _U, _3}, .result = _1|_3 }, // AAUA
            Case { .codes = {_1, _2, _U, _3, _U, _U, _4}, .result = _4 }, // AAUAUUA
            Case { .codes = {_1, _2, _U, _3, _U, _4}, .result = _1|_4 }, // AAUAUA
            Case { .codes = {_1, _2, _U, _3, _U, _4, _U, _U, _5 }, .result = _5 }, // AAUAUAUUA
            Case { .codes = {_1, _2, _U, _4, _5}, .result = _1|_4|_5 }, // AAUAA
            Case { .codes = {_1, _2, _U, _3, _4, _U, _5}, .result = _1|_3|_5 }, // AAUAAUA
            Case { .codes = {_1, _2, _U, _3, _4, _U, _5, _U, _U, _U, _6}, .result = _6 } // AAUAAUAUUUA
        };
        auto runCode = [](auto & obj, auto code) {
            switch ( code )
            {
                case Code::_U: obj.undoAction(); break;
                case Code::_1: obj()->value |= Code::_1; break;
                case Code::_2: obj()->value |= Code::_2; break;
                case Code::_3: obj()->value |= Code::_3; break;
                case Code::_4: obj()->value |= Code::_4; break;
                case Code::_5: obj()->value |= Code::_5; break;
                case Code::_6: obj()->value |= Code::_6; break;
                default: throw std::logic_error("Unrecognized code!"); break;
            }
        };
        auto getHistSize = [](auto & obj) -> std::uint64_t {
            auto changeHistory = obj.renderChangeHistory(false);
            std::uint64_t netSize = 0;
            for ( auto & action : changeHistory )
                netSize += static_cast<std::uint64_t>(action.byteCount);

            return netSize;
        };
        
        for ( auto & testCase : testCases )
        {
            TrimCaseEditor myObj {};
            for ( auto & code : testCase.codes )
            {
                testCase.priorStates.push_back(myObj->value);
                runCode(myObj, code);
                testCase.actionIndex.push_back(code == Code::_U ? std::numeric_limits<std::size_t>::max() : myObj.getCursorIndex()-1);
                testCase.states.push_back(myObj->value);
                testCase.netSize.push_back(getHistSize(myObj));
            }
            testCase.maxActions = myObj.getCursorIndex();
            EXPECT_EQ(std::size(testCase.codes), std::size(testCase.netSize));
            EXPECT_EQ(myObj->value, testCase.result);
        }

        std::size_t countValidSims = 0;
        for ( auto & testCase : testCases )
        {
            auto maxActions = testCase.maxActions;
            for ( std::size_t trimTo=0; trimTo<=maxActions; ++trimTo )
            {
                TrimCaseEditor applyTrim {};
                TrimCaseEditor simulateTrim {};
                auto checkApplyTrim = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)applyTrim);
                auto checkSimulateTrim = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)simulateTrim);

                for ( auto & code : testCase.codes )
                    runCode(applyTrim, code);

                std::size_t prevActionCount = checkApplyTrim.actions.size();
                auto trueTrimTo = applyTrim.trimHistory(trimTo);
                if ( prevActionCount == checkApplyTrim.actions.size() )
                    continue; // No change, could maybe test against prev hist state of apply trim

                std::size_t firstCodeIndex = trueTrimTo == 0 ? testCase.codes.size() : 0;
                while ( firstCodeIndex < std::size(testCase.codes) )
                {
                    while ( testCase.actionIndex[firstCodeIndex] == std::numeric_limits<std::size_t>::max() )
                        ++firstCodeIndex;

                    if ( testCase.actionIndex[firstCodeIndex] >= trueTrimTo )
                        break;
                    else
                        ++firstCodeIndex;
                }

                if ( firstCodeIndex < testCase.codes.size() )
                    simulateTrim.initData(TrimCaseData{testCase.priorStates[firstCodeIndex]});
                else if ( !testCase.codes.empty() )
                    simulateTrim.initData(TrimCaseData{testCase.states[testCase.codes.size()-1]});

                // Run first code index and onwards
                bool validSim = true;
                for ( std::size_t i=firstCodeIndex; i<std::size(testCase.codes); ++i )
                {
                    auto prevCursor = simulateTrim.getCursorIndex();
                    runCode(simulateTrim, testCase.codes[i]);
                    if ( simulateTrim.getCursorIndex() == prevCursor ) // Invalid sim: required undoing past sim hist start
                    {
                        validSim = false;
                        break;
                    }
                }

                // Compare applyTrim and simulateTrim values and history states
                if ( validSim )
                {
                    ++countValidSims;
                    EXPECT_EQ(applyTrim->value, simulateTrim->value);
                    EXPECT_EQ(checkApplyTrim.pendingActionStart, checkSimulateTrim.pendingActionStart);
                    EXPECT_EQ(checkApplyTrim.actions.size(), checkSimulateTrim.actions.size());
                    if ( checkApplyTrim.actions.size() == checkSimulateTrim.actions.size() )
                    {
                        for ( std::size_t i=0; i<checkApplyTrim.actions.size(); ++i )
                            EXPECT_EQ(checkApplyTrim.actions[i].firstEventIndex, checkSimulateTrim.actions[i].firstEventIndex);
                    }
                    EXPECT_EQ(checkApplyTrim.actionReferenceCount, checkSimulateTrim.actionReferenceCount);
                    EXPECT_EQ(checkApplyTrim.redoCount, checkSimulateTrim.redoCount);
                    EXPECT_EQ(checkApplyTrim.redoSize, checkSimulateTrim.redoSize);
                    EXPECT_EQ(checkApplyTrim.history.eventOffsets.size(), checkSimulateTrim.history.eventOffsets.size());
                    if ( checkApplyTrim.history.eventOffsets.size() == checkSimulateTrim.history.eventOffsets.size() )
                    {
                        for ( std::size_t i=0; i<checkApplyTrim.history.eventOffsets.size(); ++i )
                            EXPECT_EQ(checkApplyTrim.history.eventOffsets[i], checkSimulateTrim.history.eventOffsets[i]);
                    }
                    EXPECT_EQ(checkApplyTrim.history.events.size(), checkSimulateTrim.history.events.size());
                    if ( checkApplyTrim.history.events.size() == checkSimulateTrim.history.events.size() )
                    {
                        for ( std::size_t i=0; i<checkApplyTrim.history.events.size(); ++i )
                            EXPECT_EQ(checkApplyTrim.history.events[i], checkSimulateTrim.history.events[i]);
                    }
                }
            }
        }
        EXPECT_EQ(countValidSims, 23);

        for ( auto & testCase : testCases )
        {
            auto numCodes = testCase.codes.size();
            auto maxActions = testCase.maxActions;
            for ( std::size_t codeIndex=0; codeIndex<numCodes; ++codeIndex )
            {
                TrimCaseEditor applyTrim {};
                
                for ( auto & code : testCase.codes )
                    runCode(applyTrim, code);

                auto maxSize = testCase.netSize.size() > 0 ? testCase.netSize[testCase.netSize.size()-1] : 0;
                auto sizeTrimTo = codeIndex == 0 ? maxSize : maxSize - testCase.netSize[codeIndex-1];
                applyTrim.trimHistoryToSize(sizeTrimTo);
                auto newHistSize = getHistSize(applyTrim);

                EXPECT_TRUE(newHistSize <= maxSize);

                // Ensure there's no possible size between newHistSize and sizeTrimTo
                for ( std::size_t trimTo=0; trimTo<=maxActions; ++trimTo )
                {
                    TrimCaseEditor simulateTrim {};
                    
                    for ( auto & code : testCase.codes )
                        runCode(simulateTrim, code);

                    simulateTrim.trimHistory(trimTo);
                    auto simHistSize = getHistSize(simulateTrim);

                    EXPECT_FALSE(simHistSize > newHistSize && simHistSize < sizeTrimTo);
                }
            }
        }
    }

    TEST(MiscEdits, RootAssign)
    {
        EditInitDataTest myObj {};
        EXPECT_EQ(1, myObj->a);
        std::vector<int> expectedVec {2, 3};
        EXPECT_EQ(expectedVec, myObj->b);

        myObj()->assign(InitDataTest{
            .a = 4,
            .b = {5, 6}
        });
        EXPECT_EQ(4, myObj->a);
        expectedVec = {5, 6};
        EXPECT_EQ(expectedVec, myObj->b);

        myObj.undoAction();
        EXPECT_EQ(1, myObj->a);
        expectedVec = {2, 3};
        EXPECT_EQ(expectedVec, myObj->b);

        myObj.redoAction();
        EXPECT_EQ(4, myObj->a);
        expectedVec = {5, 6};
        EXPECT_EQ(expectedVec, myObj->b);
    }

    struct OptionalTest
    {
        std::optional<int> a = 1;

        REFLECT(OptionalTest, a)
    };

    struct EditOptionalTest : Tracked<OptionalTest, EditOptionalTest>
    {
        EditOptionalTest() : Tracked{this} {}
    };

    TEST(MiscEdits, OptionalTest)
    {
        EditOptionalTest myObj {};
        EXPECT_EQ(myObj->a.value(), 1);
        myObj()->a = std::nullopt;
        EXPECT_EQ(myObj->a, std::nullopt);
        myObj()->a = 2;
        EXPECT_EQ(myObj->a.value(), 2);

        myObj.undoAction();
        EXPECT_EQ(myObj->a, std::nullopt);
        myObj.undoAction();
        EXPECT_EQ(myObj->a, 1);

        myObj.redoAction();
        EXPECT_EQ(myObj->a, std::nullopt);
        myObj.redoAction();
        EXPECT_EQ(myObj->a, 2);
    }

    struct Item
    {
        int hitCount;

        REFLECT(Item, hitCount)
    };

    struct Actor
    {
        int xc = 0;
        int yc = 0;
        std::vector<Item> items {Item{}};

        REFLECT(Actor, xc, yc, items)
    };

    struct MyObj
    {
        std::vector<Actor> actors {};

        REFLECT(MyObj, actors)
    };

    struct TracedObj : Tracked<MyObj, TracedObj>
    {
        struct ItemElem : TrackedElement<Item, PATH(root->actors[0].items[0])>
        {
            using TrackedElement::TrackedElement;
            void hit() { edit.hitCount = read.hitCount+1; }
        };

        struct ActorElem : TrackedElement<Actor, PATH(root->actors[0])>
        {
            using TrackedElement::TrackedElement;
            void act() { edit.xc = read.xc+2; }
            auto getItemElem(std::size_t i) { return ItemElem(this, view.items[i]); }
        };

        TracedObj() : Tracked{this} {}

        ActorElem getActorElem(std::size_t i) {
            return ActorElem(this, view.actors[i]);
        }
    };

    TEST(MiscEdits, SubElemTest)
    {
        TracedObj myObj {};
        myObj.initData<false>(MyObj{
            .actors = {
                Actor{.xc = 11, .yc = 33, .items = {{0}, {0}}},
                Actor{.xc = 22, .yc = 44, .items = {{0}, {0}, {0}}}
            }
        });
        {
            auto editActor = myObj.getActorElem(1);
            EXPECT_EQ(myObj->actors[1].xc, 22);
            editActor.act();
            EXPECT_EQ(myObj->actors[1].xc, 24);

            auto editItem = editActor.getItemElem(2);
            EXPECT_EQ(myObj->actors[1].items[2].hitCount, 0);
            editItem.hit();
            EXPECT_EQ(myObj->actors[1].items[2].hitCount, 1);
        }
        myObj.undoAction();
        EXPECT_EQ(myObj->actors[1].xc, 22);
        EXPECT_EQ(myObj->actors[1].items[2].hitCount, 0);
        
        myObj.redoAction();
        EXPECT_EQ(myObj->actors[1].xc, 24);
        EXPECT_EQ(myObj->actors[1].items[2].hitCount, 1);
    }

    struct UserDefinedData
    {
        std::string descr = "";

        inline bool operator==(const UserDefinedData & other) const { return descr == other.descr; }
    };

    struct UserDefinedTest
    {
        int a = 0;

        REFLECT(UserDefinedTest, a)
    };

    struct EditUserDefinedTest : Tracked<UserDefinedTest, EditUserDefinedTest, UserDefinedData>
    {
        EditUserDefinedTest() : Tracked{this} {}
    };

    TEST(MiscEdits, UserDefinedActionData)
    {
        EditUserDefinedTest myObj {};
        {
            auto edit = myObj.operator()({"labeled action"});
            edit->a = 1;
        }
        RareEdit::RenderAction<UserDefinedData> renderAction {};
        myObj.renderAction(0, renderAction, false);
        EXPECT_STREQ(renderAction.userData.descr.c_str(), "labeled action");
        {
            auto edit = myObj.operator()({"another action"});
            edit->a = 2;
        }
        myObj.renderAction(1, renderAction, false);
        EXPECT_STREQ(renderAction.userData.descr.c_str(), "another action");
    }

}
