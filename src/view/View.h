//
// Created by vladimir on 31.01.23.
//

#ifndef TEXTEDITOR_VIEW_H
#define TEXTEDITOR_VIEW_H

#include <MyString.h>
#include <cstdlib>
#include <numeric>
#include <ncurses.h>

#include "../model/Model.h"
#include "../Observe.h"
#include <vector>

class View: public Observer {
    Model* model_ = nullptr;

public:
    explicit View(Model* model);
    void update() override;
    void PrintPage();
};


#endif //TEXTEDITOR_VIEW_H
