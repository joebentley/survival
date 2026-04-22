#pragma once

#include <stdint.h>
#include <string>

struct Entity;
/// Describes a behaviour that can be attached to an Entity and can update on each tick of the game loop
struct Behaviour {
    /// Construct a new behaviour with given ID and reference to parent (which is stored in the Behaviour)
    Behaviour(std::string ID, Entity &parent) : mID(std::move(ID)), mParent(parent) {}

    virtual ~Behaviour() = default;

    /// Unique ID for the behaviour
    std::string mID;
    /// Mutable reference to the parent
    Entity &mParent;

    /// Called each engine tick, called by the parent entity
    virtual void tick() {};

    /// Handle a specific int signal, not really used right now
    virtual void handle(uint32_t /*signal*/) {};

    /// Is the Behaviour enabled? Can override in subclasses
    virtual bool isEnabled() const { return mEnabled; }

    void enable() { mEnabled = true; }

    void disable() { mEnabled = false; }

  protected:
    bool mEnabled{true};
};
