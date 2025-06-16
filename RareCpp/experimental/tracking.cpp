#include <rarecpp/editor.h>
#include <rarecpp/json.h>
#include <rarecpp/reflect.h>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

namespace experimental
{

using namespace RareEdit;

struct Item
{
    int hitCount;

    REFLECT(Item, hitCount)
};

struct ActorLink { std::string label {}; };

struct Actor
{
    int xc = 0;
    int yc = 0;
    std::string name = "";
    std::vector<Item> items {Item{}};

    REFLECT(Actor, xc, yc, name, items)
};

NOTE(MyObj, IndexSize<std::uint32_t>)
struct MyObj
{
    std::vector<int> ints {};
    Actor actor {};
    NOTE(actors, AttachData<ActorLink>)
    std::vector<Actor> actors {};

    REFLECT_NOTED(MyObj, ints, actor, actors)
};

struct TracedObj : Tracked<MyObj, TracedObj>
{
    struct ItemElem : TrackedElement<Item, PATH(root->actors[0].items[0])>
    {
        using TrackedElement::TrackedElement;

        void hit()
        {
            edit.hitCount = read.hitCount+1;
        }
    };

    struct ActorElem : TrackedElement<Actor, PATH(root->actors[0])>
    {
        using TrackedElement::TrackedElement;

        void act()
        {
            edit.xc = read.xc+2;
        }

        auto getItemElem(std::size_t i) {
            return ItemElem(this, view.items[i]);
        }
    };

    TracedObj() : Tracked(this) {}

    void setup()
    {
        auto edit = createAction();
        edit->ints.append(std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8});
    }

    void doSomething()
    {
        auto edit = createAction();
        edit->ints = std::vector{2, 3, 4};
        edit->actor = Actor{.xc = 77, .yc = 88, .name = "jj"};
        edit->actors.append(Actor{});
        edit->actors.append(Actor{});
        edit->actors[1].xc = 12;
        edit->actors[1].yc = 13;
        edit->actors.select(0);
        edit->actors.moveSelectionsDown();
        //edit->actors.removeSelection();

        auto & attachedData = view.actors.attachedData();
        std::cout << RareTs::toStr<decltype(attachedData)>() << '\n';
    }

    void afterAction(std::size_t actionIndex)
    {
        auto hist = Tracked::renderChangeHistory();
        auto & action = hist[actionIndex];
        std::cout << "New Action[" << actionIndex << "](";
        switch ( action.actionStatus )
        {
        case RareEdit::ActionStatus::Unknown: std::cout << "Unknown"; break;
        case RareEdit::ActionStatus::Undoable: std::cout << "Undoable"; break;
        case RareEdit::ActionStatus::ElidedRedo: if ( action.elisionCount > 0 ) std::cout << "ElideLast:" << action.elisionCount; else std::cout << "ElidedRedo"; break;
        case RareEdit::ActionStatus::Redoable: std::cout << "Redoable"; break;
        }
        std::cout << ")\n";

        auto & changeEvents = hist[actionIndex].changeEvents;
        for ( std::size_t j=0; j<changeEvents.size(); ++j )
        {
            auto & changeEvent = changeEvents[j];
            std::cout << "  " << changeEvent.summary << '\n';
        }
    }

    ActorElem getActorElem(std::size_t i) {
        return ActorElem(this, view.actors[i]);
    }
    
    using actor_path = PATH(root->actors);
    using actor_xc_path = PATH(root->actors[0].xc);
    using actor_yc_path = PATH(root->actors[0].yc);
    
    void valueChanged(actor_xc_path, int oldXc, int newXc);
    void valueChanged(actor_yc_path, int oldYc, int newYc);
    void elementAdded(actor_path, std::size_t index);
    void elementRemoved(actor_path, std::size_t index);
    void elementMoved(actor_path, std::size_t oldIndex, std::size_t newIndex);
    void selectionsChanged(actor_path);
};

void TracedObj::valueChanged(actor_xc_path path, int oldXc, int newXc)
{
    std::cout << "Notify: actor[" << path.index<0>() << "].xc changed from " << oldXc << " to " << newXc << "\n";
}

void TracedObj::valueChanged(actor_yc_path path, int oldYc, int newYc)
{
    std::cout << "Notify: actor[" << path.index<0>() << "].yc changed from " << oldYc << " to " << newYc << "\n";
}

void TracedObj::elementAdded(actor_path, std::size_t index)
{
    std::cout << "Notify: actor added: " << index << "\n";
}

void TracedObj::elementRemoved(actor_path, std::size_t index)
{
    std::cout << "Notify: actor removed: " << index << "\n";
}

void TracedObj::elementMoved(actor_path, std::size_t oldIndex, std::size_t newIndex)
{
    std::cout << "Notify: actor moved: " << oldIndex << ", " << newIndex << "\n";
}

void TracedObj::selectionsChanged(actor_path)
{
    std::cout << "Notify: selections changed\n";
}

void dataHistory()
{
    TracedObj myObj {};
    
    std::cout << "\n\ntestSetup:\n";
    myObj.setup();
    std::cout << Json::out(*myObj);

    std::cout << "\n\ntestDo:\n";
    myObj.doSomething();
    std::cout << Json::out(*myObj);
    
    std::cout << "\n\ntestElemOp:\n";
    {
        auto actor = myObj.getActorElem(1);
        actor.act();
        auto item = actor.getItemElem(0);
        item.hit();
        actor.act();
        item.hit();
    }
    std::cout << Json::out(*myObj);

    std::cout << "\n\ntestUndo:\n";
    myObj.undoAction();
    std::cout << Json::out(*myObj);

    std::cout << "\n\ntestRedo:\n";
    myObj.redoAction();
    std::cout << Json::out(*myObj) << "\n\n";

    myObj.printChangeHistory(std::cout);
}

}
