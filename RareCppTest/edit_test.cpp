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
            createAction();
        }
    };

    TEST(UndoRedoElision, Unmodified)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);

        EXPECT_EQ(check.redoCount, 0);
        EXPECT_EQ(check.redoSize, 0);
        EXPECT_TRUE(check.actionFirstEvent.empty());
    }

    TEST(UndoRedoElision, A)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);
        obj.act();

        EXPECT_EQ(check.redoCount, 0);
        EXPECT_EQ(check.redoSize, 0);
        EXPECT_EQ(check.actionFirstEvent.size(), 1);
        EXPECT_EQ(check.actionFirstEvent[0], 0);
    }

    TEST(UndoRedoElision, AU)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);
        obj.act();
        obj.undoAction();

        EXPECT_EQ(check.redoCount, 1);
        EXPECT_EQ(check.redoSize, 1);
        EXPECT_EQ(check.actionFirstEvent.size(), 1);
        EXPECT_EQ(check.actionFirstEvent[0], 0);
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
        EXPECT_EQ(check.actionFirstEvent.size(), 3);
        EXPECT_EQ(check.actionFirstEvent[0], 0);
        EXPECT_EQ(check.actionFirstEvent[1], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[2], 0);
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
        EXPECT_EQ(check.actionFirstEvent.size(), 1);
        EXPECT_EQ(check.actionFirstEvent[0], 0);
    }

    TEST(UndoRedoElision, AA)
    {
        SimpleEditor obj {};
        auto check = RareTs::whitebox((Tracked<Simple, SimpleEditor> &)obj);
        obj.act();
        obj.act();

        EXPECT_EQ(check.redoCount, 0);
        EXPECT_EQ(check.redoSize, 0);
        EXPECT_EQ(check.actionFirstEvent.size(), 2);
        EXPECT_EQ(check.actionFirstEvent[0], 0);
        EXPECT_EQ(check.actionFirstEvent[1], 0);
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
        EXPECT_EQ(check.actionFirstEvent.size(), 2);
        EXPECT_EQ(check.actionFirstEvent[0], 0);
        EXPECT_EQ(check.actionFirstEvent[1], 0);
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
        EXPECT_EQ(check.actionFirstEvent.size(), 4);
        EXPECT_EQ(check.actionFirstEvent[0], 0);
        EXPECT_EQ(check.actionFirstEvent[1], 0);
        EXPECT_EQ(check.actionFirstEvent[2], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[3], 0);
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
        EXPECT_EQ(check.actionFirstEvent.size(), 4);
        EXPECT_EQ(check.actionFirstEvent[0], 0);
        EXPECT_EQ(check.actionFirstEvent[1], 0);
        EXPECT_EQ(check.actionFirstEvent[2], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[3], 0);
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
        EXPECT_EQ(check.actionFirstEvent.size(), 4);
        EXPECT_EQ(check.actionFirstEvent[0], 0);
        EXPECT_EQ(check.actionFirstEvent[1], 0);
        EXPECT_EQ(check.actionFirstEvent[2], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[3], 0);
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
        EXPECT_EQ(check.actionFirstEvent.size(), 6);
        EXPECT_EQ(check.actionFirstEvent[0], 0);
        EXPECT_EQ(check.actionFirstEvent[1], 0);
        EXPECT_EQ(check.actionFirstEvent[2], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[3], 0);
        EXPECT_EQ(check.actionFirstEvent[4], check.flagElidedRedos | 4);
        EXPECT_EQ(check.actionFirstEvent[5], 0);
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
        EXPECT_EQ(check.actionFirstEvent.size(), 6);
        EXPECT_EQ(check.actionFirstEvent[0], 0);
        EXPECT_EQ(check.actionFirstEvent[1], 0);
        EXPECT_EQ(check.actionFirstEvent[2], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[3], 0);
        EXPECT_EQ(check.actionFirstEvent[4], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[5], 0);
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
        EXPECT_EQ(check.actionFirstEvent.size(), 6);
        EXPECT_EQ(check.actionFirstEvent[0], 0);
        EXPECT_EQ(check.actionFirstEvent[1], 0);
        EXPECT_EQ(check.actionFirstEvent[2], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[3], 0);
        EXPECT_EQ(check.actionFirstEvent[4], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[5], 0);
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
        EXPECT_EQ(check.actionFirstEvent.size(), 6);
        EXPECT_EQ(check.actionFirstEvent[0], 0);
        EXPECT_EQ(check.actionFirstEvent[1], 0);
        EXPECT_EQ(check.actionFirstEvent[2], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[3], 0);
        EXPECT_EQ(check.actionFirstEvent[4], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[5], 0);
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
        EXPECT_EQ(check.actionFirstEvent.size(), 8);
        EXPECT_EQ(check.actionFirstEvent[0], 0);
        EXPECT_EQ(check.actionFirstEvent[1], 0);
        EXPECT_EQ(check.actionFirstEvent[2], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[3], 0);
        EXPECT_EQ(check.actionFirstEvent[4], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[5], 0);
        EXPECT_EQ(check.actionFirstEvent[6], check.flagElidedRedos | 6);
        EXPECT_EQ(check.actionFirstEvent[7], 0);
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
        EXPECT_EQ(check.actionFirstEvent.size(), 5);
        EXPECT_EQ(check.actionFirstEvent[0], 0);
        EXPECT_EQ(check.actionFirstEvent[1], 0);
        EXPECT_EQ(check.actionFirstEvent[2], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[3], 0);
        EXPECT_EQ(check.actionFirstEvent[4], 0);
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
        EXPECT_EQ(check.actionFirstEvent.size(), 5);
        EXPECT_EQ(check.actionFirstEvent[0], 0);
        EXPECT_EQ(check.actionFirstEvent[1], 0);
        EXPECT_EQ(check.actionFirstEvent[2], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[3], 0);
        EXPECT_EQ(check.actionFirstEvent[4], 0);
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
        EXPECT_EQ(check.actionFirstEvent.size(), 7);
        EXPECT_EQ(check.actionFirstEvent[0], 0);
        EXPECT_EQ(check.actionFirstEvent[1], 0);
        EXPECT_EQ(check.actionFirstEvent[2], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[3], 0);
        EXPECT_EQ(check.actionFirstEvent[4], 0);
        EXPECT_EQ(check.actionFirstEvent[5], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[6], 0);
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
        EXPECT_EQ(check.actionFirstEvent.size(), 7);
        EXPECT_EQ(check.actionFirstEvent[0], 0);
        EXPECT_EQ(check.actionFirstEvent[1], 0);
        EXPECT_EQ(check.actionFirstEvent[2], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[3], 0);
        EXPECT_EQ(check.actionFirstEvent[4], 0);
        EXPECT_EQ(check.actionFirstEvent[5], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[6], 0);
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
        EXPECT_EQ(check.actionFirstEvent.size(), 7);
        EXPECT_EQ(check.actionFirstEvent[0], 0);
        EXPECT_EQ(check.actionFirstEvent[1], 0);
        EXPECT_EQ(check.actionFirstEvent[2], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[3], 0);
        EXPECT_EQ(check.actionFirstEvent[4], 0);
        EXPECT_EQ(check.actionFirstEvent[5], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[6], 0);
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
        EXPECT_EQ(check.actionFirstEvent.size(), 7);
        EXPECT_EQ(check.actionFirstEvent[0], 0);
        EXPECT_EQ(check.actionFirstEvent[1], 0);
        EXPECT_EQ(check.actionFirstEvent[2], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[3], 0);
        EXPECT_EQ(check.actionFirstEvent[4], 0);
        EXPECT_EQ(check.actionFirstEvent[5], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[6], 0);
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
        EXPECT_EQ(check.actionFirstEvent.size(), 9);
        EXPECT_EQ(check.actionFirstEvent[0], 0);
        EXPECT_EQ(check.actionFirstEvent[1], 0);
        EXPECT_EQ(check.actionFirstEvent[2], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[3], 0);
        EXPECT_EQ(check.actionFirstEvent[4], 0);
        EXPECT_EQ(check.actionFirstEvent[5], check.flagElidedRedos | 1);
        EXPECT_EQ(check.actionFirstEvent[6], 0);
        EXPECT_EQ(check.actionFirstEvent[7], check.flagElidedRedos | 7);
        EXPECT_EQ(check.actionFirstEvent[8], 0);
    }

    NOTE(CumulativeTest, IndexSize<std::uint32_t>{})
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
        std::vector<Bar> barVec {}; NOTE(barVec, IndexSize<std::uint16_t>{})
        int intRay[5] {};
        int mdIntRay[2][3] {};
        std::vector<Trig> trigs {}; NOTE(trigs, IndexSize<std::uint16_t>{})

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

}

