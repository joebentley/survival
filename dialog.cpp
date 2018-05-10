#include "dialog.h"

#include <sstream>

std::string repeat(int n, std::string str) {
    std::ostringstream os;
    for(int i = 0; i < n; i++)
        os << str;
    return os.str();
}

void showMessageBox(Font& font, const std::string& message, int x, int y) {
    int numChars = getFontStringLength(message);

    font.drawText("${black}$(p23)" + repeat(numChars + 4, "$(p27)") + "$(p9)", x, y);
    font.drawText("${black}$(p8)" + std::string(numChars + 4, ' ') + "$(p8)", x, y+1);
    font.drawText("${black}$(p8)  " + message + "${black}  $[white]$(p8)", x, y+2);
    font.drawText("${black}$(p8)" + std::string(numChars + 4, ' ') + "$(p8)", x, y+3);
    font.drawText("${black}$(p22)" + repeat(numChars + 4, "$(p27)") + "$(p10)", x, y+4);
}