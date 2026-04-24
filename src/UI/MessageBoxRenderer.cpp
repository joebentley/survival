#include "MessageBoxRenderer.h"

#include "../World.h"
#include "../utils.h"

void showMessageBox(Font &font, const std::vector<std::string> &contents, int padding, int x, int y) {
    int maxNumChars = 0;

    for (const auto &string : contents) {
        int length = Font::getFontStringLength(string);
        if (length > maxNumChars)
            maxNumChars = length;
    }

    int innerBoxWidth = maxNumChars + 2 * padding;

    font.drawText("${black}$(p23)" + repeat(innerBoxWidth, "$(p27)") + "$(p9)", x, y);

    for (int i = 0; i < padding; ++i) {
        font.drawText("${black}$(p8)" + std::string((unsigned long)innerBoxWidth, ' ') + "$(p8)", x, ++y);
    }

    for (const auto &line : contents) {
        int paddingLength = maxNumChars - Font::getFontStringLength(line);
        font.drawText("${black}$(p8)" + std::string((unsigned long)padding, ' ') + line +
                          std::string((unsigned long)paddingLength, ' ') + "${black}$[white]" +
                          std::string((unsigned long)padding, ' ') + "$(p8)",
                      x, ++y);
    }

    for (int i = 0; i < padding; ++i) {
        font.drawText("${black}$(p8)" + std::string((unsigned long)innerBoxWidth, ' ') + "$(p8)", x, ++y);
    }

    font.drawText("${black}$(p22)" + repeat(innerBoxWidth, "$(p27)") + "$(p10)", x, y + 1);
}

void MessageBoxRenderer::queueMessageBox(const std::vector<std::string> &contents, int padding, int x, int y) {
    mRenderingQueue.push_back(MessageBoxData{contents, padding, x, y});
}

void MessageBoxRenderer::queueMessageBoxCentered(const std::vector<std::string> &contents, int padding) {
    std::vector<int> lineLengths;
    std::transform(contents.cbegin(), contents.cend(), std::back_inserter(lineLengths),
                   [](const auto &a) { return Font::getFontStringLength(a); });
    const int maxLength = *std::max_element(lineLengths.cbegin(), lineLengths.cend());
    const int width = maxLength + 2 * padding;
    const int height = static_cast<int>(contents.size()) + 2 * padding;

    queueMessageBox(contents, padding, (World::SCREEN_WIDTH - width) / 2, (World::SCREEN_HEIGHT - height) / 2);
}

void MessageBoxRenderer::render(Font &font) {
    while (!mRenderingQueue.empty()) {
        auto data = mRenderingQueue.front();
        mRenderingQueue.pop_front();

        showMessageBox(font, data.mContents, data.mPadding, data.mX, data.mY);
    }
}
