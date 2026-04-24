#pragma once

#define UISTATE(className, screenType)                                                                                 \
    class className {                                                                                                  \
      public:                                                                                                          \
        virtual ~className() = default;                                                                                \
                                                                                                                       \
        virtual void onEntry(screenType &screen) = 0;                                                                  \
        virtual std::unique_ptr<className> handleInput(screenType &screen, SDL_KeyboardEvent &e) = 0;                  \
        virtual void onExit(screenType &screen) = 0;                                                                   \
    };
