#include "WanderBehaviour.h"
#include "../../Entity/Entity.h"
#include "../../Point.h"

void WanderBehaviour::tick() {
    Point p = mParent.getPos();
    switch (rand() % 20) {
    case 0:
        p.mX++;
        break;
    case 1:
        p.mX--;
        break;
    case 2:
        p.mY++;
        break;
    case 3:
        p.mY--;
        break;
    case 4:
        p.mX++;
        p.mY++;
        break;
    case 5:
        p.mX--;
        p.mY++;
        break;
    case 6:
        p.mX++;
        p.mY--;
        break;
    case 7:
        p.mX--;
        p.mY--;
        break;
    }

    mParent.moveTo(p);
}