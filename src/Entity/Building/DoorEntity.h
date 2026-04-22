#pragma once

#include "../../Behaviours.h"
#include "../Entity.h"

/// Represents a door that can be opened or closed with spacebar
struct DoorEntity : Entity {
    explicit DoorEntity(const Point &pos);

    /// Overrides rendering to display whether the door is open or closed
    /// \param font the font to render on
    /// \param currentWorldPos the current world position
    void render(Font &font, Point currentWorldPos) override;

    /// Overrides collision detection to open the door
    /// \param pos position to check if colliding
    /// \return whether or not collision occurred
    bool collide(const Point &pos) override;

    void open() { mIsOpen = true; }
    void close() { mIsOpen = false; }
    bool isOpen() { return mIsOpen; }

    /// Handles the opening and closing of the door with spacebar
    struct DoorOpenAndCloseBehaviour : InteractableBehaviour {
        explicit DoorOpenAndCloseBehaviour(Entity &parent) : InteractableBehaviour(parent) {}

        bool handleInput(SDL_KeyboardEvent &e) override;
    };

  private:
    bool mIsOpen{false};
};
