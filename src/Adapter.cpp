//
// Created by vladimir on 13.03.23.
//

#include "Adapter.h"

void Adapter::InitPage() {
    initscr();
}

void Adapter::RefreshPage() {
    refresh();
}

void Adapter::FirstInit() {
    nodelay(stdscr, true);
    keypad(stdscr, true);
    noecho();
    set_escdelay(0);
}

int Adapter::GetMaxX() {
    return getmaxx(stdscr);
}

int Adapter::GetMaxY() {
    return getmaxy(stdscr);
}

void Adapter::ClearPage() {
    clear();
}

void Adapter::ClrToEol() {
    clrtoeol();
}

void Adapter::Move(int y, int x) {
    move(y, x);
}

void Adapter::Printw(const char *str) {
    printw(str);
}


