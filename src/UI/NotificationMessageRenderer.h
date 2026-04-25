#pragma once
#include "../World.h"
#include <deque>
#include <string>

struct NotificationMessageRenderer {
    static NotificationMessageRenderer &getInstance() {
        static NotificationMessageRenderer instance;
        return instance;
    }

    NotificationMessageRenderer() { previousTime = clock(); }

    NotificationMessageRenderer(const NotificationMessageRenderer &) = delete;
    void operator=(const NotificationMessageRenderer &) = delete;

    void queueMessage(const std::string &message);
    void render(Font &font);

    std::deque<std::string> getMessages() const { return mAllMessages; }

  private:
    const int cMaxOnScreen{6};
    const int cInitialYPos{World::SCREEN_HEIGHT - 2};
    const float cAlphaDecayPerSec{0.5};
    std::deque<std::string> mMessagesToBeRendered{};
    std::deque<std::string> mAllMessages{};
    float mAlpha{1};
    clock_t previousTime{0};
};
