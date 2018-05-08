#include "dialog.h"

#include <sstream>

std::string repeat(int n, std::string str) {
    std::ostringstream os;
    for(int i = 0; i < n; i++)
        os << str;
    return os.str();
}

void showMessageBox(SDL_Renderer* renderer, Font& font, const std::string& message, int x, int y) {
    int numChars = message.size();

    font.drawText(renderer, "$(p23)" + repeat(numChars + 4, "$(p27)") + "$(p9)", x, y);
    font.drawText(renderer, "$(p8)" + std::string(numChars + 4, ' ') + "$(p8)", x, y+1);
    font.drawText(renderer, "$(p8)  " + message + "  $(p8)", x, y+2);
    font.drawText(renderer, "$(p8)" + std::string(numChars + 4, ' ') + "$(p8)", x, y+3);
    font.drawText(renderer, "$(p22)" + repeat(numChars + 4, "$(p27)") + "$(p10)", x, y+4);
}