#pragma once

#include <QString>

namespace texts {
inline const auto shortMessage = QStringLiteral("This small-sized message contains approximately 50 symbols!");
inline const auto mediumMessage = QStringLiteral(
    "This is a medium-sized message, it contains approximately 150 symbols. And just to fulfil this property I will "
    "say you, that a cabbage is not a dog)");
inline const auto largeMessage = QStringLiteral(
    "This is a very long and strange message. It contains more than 200 characters. It describes the adventures of "
    "Alice in Wonderland, where you can find such characters as the Cheshire Cat, the Mad Hatter, and the Queen of "
    "Hearts. If you want to learn more about this place, please contact your system administrator and ask him for the "
    "secret message type and password. However, be careful — you can easily get completely lost here.");
}  // namespace texts

inline QString generateText(int lengthCode)
{
    switch (lengthCode) {
        case 0:
            return texts::shortMessage;
        case 1:
            return texts::mediumMessage;
        case 2:
            return texts::largeMessage;
        default:
            return "An ironic message";
    }
}