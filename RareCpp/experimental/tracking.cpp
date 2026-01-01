#include <nf/hist.h>
#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

namespace experimental
{
struct Item
{
    int hit_count;

    REFLECT(Item, hit_count)
};

struct Actor
{
    int xc = 0;
    int yc = 0;
    std::string name = "";
    std::vector<Item> items {Item{}};

    REFLECT(Actor, xc, yc, name, items)
};

NOTE(My_obj, nf::index_size<std::uint32_t>)
struct My_obj
{
    std::vector<int> ints {};
    Actor actor {};
    std::vector<Actor> actors {};

    REFLECT_NOTED(My_obj, ints, actor, actors)
};

struct Traced_obj : nf::tracked<My_obj, Traced_obj>
{
    struct Item_elem : nf::tracked_element<Item, NF_PATH(root->actors[0].items[0])>
    {
        using tracked_element::tracked_element;

        void hit()
        {
            edit.hit_count = read.hit_count+1;
        }
    };

    struct Actor_elem : nf::tracked_element<Actor, NF_PATH(root->actors[0])>
    {
        using tracked_element::tracked_element;

        void act()
        {
            edit.xc = read.xc+2;
        }

        auto get_item_elem(std::size_t i) {
            return Item_elem(this, view.items[i]);
        }
    };

    Traced_obj() : tracked(this) {}

    void setup()
    {
        auto edit = create_action();
        edit->ints.append(std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8});
    }

    void do_something()
    {
        auto edit = create_action();
        edit->ints = std::vector{2, 3, 4};
        edit->actor = Actor{.xc = 77, .yc = 88, .name = "jj"};
        edit->actors.append(Actor{});
        edit->actors.append(Actor{});
        edit->actors[1].xc = 12;
        edit->actors[1].yc = 13;
        edit->actors.select(0);
        edit->actors.move_selections_down();
        //edit->actors.remove_selection();
    }

    void after_action(std::size_t action_index)
    {
        auto hist = tracked::render_change_history();
        auto & action = hist[action_index];
        std::cout << "New Action[" << action_index << "](";
        switch ( action.status )
        {
        case nf::action_status::unknown: std::cout << "unknown"; break;
        case nf::action_status::undoable: std::cout << "undoable"; break;
        case nf::action_status::elided_redo: if ( action.elision_count > 0 ) std::cout << "elide_last:" << action.elision_count; else std::cout << "elided_redo"; break;
        case nf::action_status::redoable: std::cout << "redoable"; break;
        }
        std::cout << ")\n";

        auto & change_events = hist[action_index].change_events;
        for ( std::size_t j=0; j<change_events.size(); ++j )
        {
            auto & change_event = change_events[j];
            std::cout << "  " << change_event.summary << '\n';
        }
    }

    Actor_elem get_actor_elem(std::size_t i) {
        return Actor_elem(this, view.actors[i]);
    }
    
    using actor_path = NF_PATH(root->actors);
    using actor_xc_path = NF_PATH(root->actors[0].xc);
    using actor_yc_path = NF_PATH(root->actors[0].yc);
    
    void value_changed(actor_xc_path, int old_xc, int new_xc);
    void value_changed(actor_yc_path, int old_yc, int new_yc);
    void element_added(actor_path, std::size_t index);
    void element_removed(actor_path, std::size_t index);
    void element_moved(actor_path, std::size_t old_index, std::size_t new_index);
    void selections_changed(actor_path);
};

void Traced_obj::value_changed(actor_xc_path path, int old_xc, int new_xc)
{
    std::cout << "Notify: actor[" << path.index<0>() << "].xc changed from " << old_xc << " to " << new_xc << "\n";
}

void Traced_obj::value_changed(actor_yc_path path, int old_yc, int new_yc)
{
    std::cout << "Notify: actor[" << path.index<0>() << "].yc changed from " << old_yc << " to " << new_yc << "\n";
}

void Traced_obj::element_added(actor_path, std::size_t index)
{
    std::cout << "Notify: actor added: " << index << "\n";
}

void Traced_obj::element_removed(actor_path, std::size_t index)
{
    std::cout << "Notify: actor removed: " << index << "\n";
}

void Traced_obj::element_moved(actor_path, std::size_t old_index, std::size_t new_index)
{
    std::cout << "Notify: actor moved: " << old_index << ", " << new_index << "\n";
}

void Traced_obj::selections_changed(actor_path)
{
    std::cout << "Notify: selections changed\n";
}

void dataHistory()
{
    Traced_obj obj {};
    
    std::cout << "\n\ntest_setup:\n";
    obj.setup();
    std::cout << Json::out(*obj);

    std::cout << "\n\ntest_do:\n";
    obj.do_something();
    std::cout << Json::out(*obj);
    
    std::cout << "\n\ntest_elem_op:\n";
    {
        auto actor = obj.get_actor_elem(1);
        actor.act();
        auto item = actor.get_item_elem(0);
        item.hit();
        actor.act();
        item.hit();
    }
    std::cout << Json::out(*obj);

    std::cout << "\n\ntest_undo:\n";
    obj.undo_action();
    std::cout << Json::out(*obj);

    std::cout << "\n\ntest_do:\n";
    obj.do_something();
    std::cout << Json::out(*obj);

    std::cout << "\n\ntest_redo:\n";
    obj.redo_action();
    std::cout << Json::out(*obj);

    std::cout << "\n\n";
    obj.print_change_history(std::cout);
}

}
