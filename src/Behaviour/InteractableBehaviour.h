#pragma once

#include "Behaviour.h"
#include <SDL3/SDL.h>

struct Font;
// TODO: This should be a property, but difficult due to virtual methods
/// Represents an entity that can be interacted with by the player, and can hijack input handling and rendering
struct InteractableBehaviour : Behaviour {
    explicit InteractableBehaviour(Entity &parent) : Behaviour("InteractableBehaviour", parent) {}

    /// Handle input from the player entity. If it returns false then the interaction with the player will end.
    /// \param e the raw SDL keyboard event
    /// \return if false, end the current interaction
    virtual bool handleInput(SDL_KeyboardEvent &e) { return false; }

    /// Allows custom rendering to be done by the behaviour if required
    virtual void render(Font &) {}
};
