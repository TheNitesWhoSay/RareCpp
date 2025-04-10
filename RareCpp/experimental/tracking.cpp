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

struct Actor
{
    int xc = 0;
    int yc = 0;
    std::string name = "";

    REFLECT(Actor, xc, yc, name)
};

NOTE(MyObj, IndexSize<std::uint32_t>{})
struct MyObj
{
    int intRay[5] {};
    std::vector<int> ints {};
    std::vector<Actor> actors {};

    REFLECT_NOTED(MyObj, ints, intRay, actors)
};

struct TracedObj : Tracked<MyObj, TracedObj>
{
    TracedObj() : Tracked(this) {}

    void setup()
    {
        auto edit = createAction();
        edit->ints.append(std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8});
    }

    void doSomething()
    {
        auto edit = createAction();
        edit->intRay[2] = 5;
        edit->ints = std::vector{2, 3, 4};
        edit->actors.append(Actor{});
        edit->actors.append(Actor{});
        edit->actors[1].xc = 12;
        edit->actors[1].yc = 13;
        edit->actors.select(0);
        edit->actors.moveSelectionsDown();
        edit->actors.removeSelection();
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
    std::cout << "Notify: actor[" << std::get<0>(path.indexes) << "].xc changed from " << oldXc << " to " << newXc << "\n";
}

void TracedObj::valueChanged(actor_yc_path path, int oldYc, int newYc)
{
    std::cout << "Notify: actor[" << std::get<0>(path.indexes) << "].yc changed from " << oldYc << " to " << newYc << "\n";
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
    
    std::cout << "\n\ntestUndo:\n";
    myObj.undoAction();
    std::cout << Json::out(*myObj);

    std::cout << "\n\ntestRedo:\n";
    myObj.redoAction();
    std::cout << Json::out(*myObj) << "\n\n";

    myObj.printChangeHistory();
}

}
