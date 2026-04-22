#include "DoorEntity.h"

#include "../../UI.h"

DoorEntity::DoorEntity(const Point &pos) : Entity("", "Door", "") {
    mPos = pos;
    mRenderingLayer = 0;

    auto interactable = std::make_unique<DoorOpenAndCloseBehaviour>(*this);
    addBehaviour(std::move(interactable));
}

bool DoorEntity::DoorOpenAndCloseBehaviour::handleInput(SDL_KeyboardEvent &) {
    std::string message;
    auto &parent = dynamic_cast<DoorEntity &>(mParent);

    if (parent.isOpen()) {
        message = "You closed the door";
        parent.close();
    } else {
        message = "You opened the door";
        parent.open();
    }

    NotificationMessageRenderer::getInstance().queueMessage(message);

    return false;
}

void DoorEntity::render(Font &font, Point currentWorldPos) {
    // Change graphic depending on whether door open or closed
    mGraphic = std::string("$[white]${black}") + std::string(mIsOpen ? "." : "+");
    Entity::render(font, currentWorldPos);
}

bool DoorEntity::collide(const Point &pos) {
    // Open the door if we walk into it and it's currently shut
    if (!mIsOpen && pos == mPos) {
        this->open();
        return true;
    }
    return false;
}
