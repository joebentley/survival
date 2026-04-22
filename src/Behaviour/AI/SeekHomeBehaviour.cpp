#include "SeekHomeBehaviour.h"

#include "../../Entity/Entity.h"
#include "../../Entity/EntityManager.h"
#include "../../UI.h"
#include "../../utils.h"

void SeekHomeBehaviour::tick() {
    if (homeTargetID.empty() && randDouble() < homeAttachmentProbability) {
        // nb: this is only the entities on the screens surrounding the player
        std::vector<Entity *> entities = EntityManager::getInstance().getEntitiesOnScreenAndSurroundingScreens();

        // filter for home entities that have the specified name and are in range
        entities.erase(std::remove_if(entities.begin(), entities.end(),
                                      [this](Entity *entity) {
                                          return !(entity->mName == homeName &&
                                                   entity->getPos().distanceTo(mParent.getPos()) < range);
                                      }),
                       entities.end());

        // pick one of these home entities at random to set as target
        if (!entities.empty()) {
            homeTargetID = entities[rand() % entities.size()]->mID;
        }
    }

    if (!homeTargetID.empty()) {
        mParent.disableWanderBehaviours();

        // move towards the chosen home entity
        Entity *homeTarget = EntityManager::getInstance().getEntityByID(homeTargetID);
        if (homeTarget == nullptr) {
            homeTargetID.clear();
            mParent.enableWanderBehaviours();
            return;
        }

        Point targetPos = homeTarget->getPos();

        // if we are at the target home, stop moving towards it
        if (targetPos == mParent.getPos()) {
            isInHome = true;

            // random chance to leave the home and start wandering again
            if (randDouble() < homeFlightProbability) {
                homeTargetID.clear();
                mParent.enableWanderBehaviours();
            }

            return;
        }

        isInHome = false;

        Point posOffset;

        if (targetPos.mX > mParent.getPos().mX) {
            posOffset.mX = 1;
        } else if (targetPos.mX < mParent.getPos().mX) {
            posOffset.mX = -1;
        }
        if (targetPos.mY > mParent.getPos().mY) {
            posOffset.mY = 1;
        } else if (targetPos.mY < mParent.getPos().mY) {
            posOffset.mY = -1;
        }

        mParent.moveTo(mParent.getPos() + posOffset);
    }
}
