//
// Created by vladimir on 31.01.23.
//

#ifndef TEXTEDITOR_OBSERVE_H
#define TEXTEDITOR_OBSERVE_H

#include <vector>

class Observer {
public:
    virtual void update() = 0;
};

class Observable {
    std::vector<Observer*> _observers;
public:
    void AddObserver(Observer *observer);
    void NotifyUpdate();
};

#endif //TEXTEDITOR_OBSERVE_H
