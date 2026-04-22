#pragma once

#include "../utils.h"
#include "Entity.h"

struct WaterEntity : Entity {
    explicit WaterEntity(std::string ID = "") : Entity(std::move(ID), "Water", "") {
        mRenderingLayer = 10;

        if (randDouble() > 0.5)
            mGraphic = "${black}$[cyan]$(approx)";
        else
            mGraphic = "${black}$[cyan]~";
    }
};