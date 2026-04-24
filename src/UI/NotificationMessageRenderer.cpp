#include "NotificationMessageRenderer.h"

void NotificationMessageRenderer::queueMessage(const std::string &message) {
    mMessagesToBeRendered.push_back(message);
    if (mMessagesToBeRendered.size() > (size_t)cMaxOnScreen)
        mMessagesToBeRendered.pop_front();
    mAllMessages.push_back(message);
}

void NotificationMessageRenderer::render(Font &font) {
    auto currentTime = clock();

    auto front = mMessagesToBeRendered.cbegin();
    for (int i = 0; front != mMessagesToBeRendered.cend() && i < cMaxOnScreen; ++i, ++front) {
        font.drawText(*front, 4, cInitialYPos - static_cast<int>(mMessagesToBeRendered.size()) + i,
                      i == 0 ? static_cast<int>(0xFF * mAlpha) : -1);

        if (i == 0) // TODO: why is alpha decay slower when in a menu?
            mAlpha -= cAlphaDecayPerSec * static_cast<float>(currentTime - previousTime) / CLOCKS_PER_SEC;
    }

    if (mAlpha <= 0) {
        mAlpha = 1;
        mMessagesToBeRendered.pop_front();
    }

    previousTime = currentTime;
}
