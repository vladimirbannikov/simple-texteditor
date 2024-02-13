//
// Created by vladimir on 27.01.23.
//

#ifndef TEXTEDITOR_CONTROLLER_H
#define TEXTEDITOR_CONTROLLER_H

#include <iostream>
#include <ncurses.h>
#include <cstdlib>
#include "../model/Model.h"
#include <vector>
#include "../Adapter.h"

#define KEY_DELETE_ 330
#define KEY_ESC_ 27
#define KEY_ENTER_ 10

enum InputModes {
    SimpleInput,
    BeginOfLineInput,
    EndOfTheLineInput,
    DeleteLineAndInput
};

enum CommandModes {
    FindNext,
    FindPrev,
    SimpleCommandMode
};

class Controller {
    Model *model_;
    std::vector<int> CursorCommands{KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN};
public:
    explicit Controller(Model *model);

    void Navigation();

    void CommandMode();

    void FindMode();

    void InsertMode(int mode);
};


#endif //TEXTEDITOR_CONTROLLER_H
