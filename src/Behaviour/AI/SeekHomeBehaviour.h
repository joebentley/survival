#pragma once
#include "../../Entity/Entity.h"
#include <string>

/// Seek out a Home entity (identified by given name) if nearby and hole up within it
/// with chance of leaving the home again
struct SeekHomeBehaviour : Behaviour {
    /// Name of home entities to go to
    std::string homeName;
    /// Range within which to start moving to the home entity
    float range;
    /// Probability to choose a home to start going towards
    float homeAttachmentProbability;
    /// Probability to leave the home
    float homeFlightProbability;
    /// Whether or not is currently in home
    bool isInHome{false};

    explicit SeekHomeBehaviour(Entity &parent, std::string homeName, float range = 20,
                               float homeAttachmentProbability = 0.1, float homeFlightProbability = 0.1)
        : Behaviour("SeekHomeBehaviour", parent), homeName(std::move(homeName)), range(range),
          homeAttachmentProbability(homeAttachmentProbability), homeFlightProbability(homeFlightProbability) {}

    void tick() override;

    const std::string &getHomeID() const { return homeTargetID; }

  private:
    /// ID of current home target
    std::string homeTargetID;
};
