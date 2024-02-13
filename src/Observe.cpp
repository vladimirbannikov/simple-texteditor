//
// Created by vladimir on 31.01.23.
//

#include "Observe.h"


void Observable::AddObserver(Observer *observer) {
    _observers.push_back(observer);
}

void Observable::NotifyUpdate() {
    int size = _observers.size();
    for (int i = 0; i < size; i++){
        _observers[i]->update();
    }
}


