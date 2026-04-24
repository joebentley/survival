#include "AttachmentBehaviour.h"
#include "../../Entity/EntityManager.h"
#include "../../Entity/PlayerEntity.h"
#include "../../UI/NotificationMessageRenderer.h"
#include "../../utils.h"

void AttachmentBehaviour::tick() {
    auto r = randDouble();

    if (!attached) {
        if (r < attachment) {
            PlayerEntity &p = dynamic_cast<PlayerEntity &>(*EntityManager::getInstance().getEntityByID("Player"));
            if (mParent.getPos().distanceTo(p.getPos()) < 10) {
                NotificationMessageRenderer::getInstance().queueMessage(mParent.mGraphic +
                                                                        "$[red]$(heart)$[white]$(dwarf)");
                attached = true;
            }
        }
        return;
    }

    PlayerEntity &p = dynamic_cast<PlayerEntity &>(*EntityManager::getInstance().getEntityByID("Player"));

    // Don't follow too closely
    auto pos = mParent.getPos();
    if (pos.distanceTo(p.getPos()) > 2 && r < clinginess) {
        if (pos.mX < p.getPos().mX)
            pos.mX++;
        else if (pos.mX > p.getPos().mX)
            pos.mX--;
        if (pos.mY < p.getPos().mY)
            pos.mY++;
        else if (pos.mY > p.getPos().mY)
            pos.mY--;
    }

    mParent.moveTo(pos);

    if (r < unattachment) {
        attached = false;
        return;
    }
}