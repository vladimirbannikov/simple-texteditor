#ifndef TEXTEDITOR_ADAPTER_H
#define TEXTEDITOR_ADAPTER_H

#include <iostream>
#include "ncurses.h"

class Adapter {
public:
   static void InitPage();
   static void RefreshPage();
   static void ClearPage();
   static int GetMaxX();
   static int GetMaxY();
   static void FirstInit();
   static void ClrToEol();
   static void Move(int y, int x);
   static void Printw(const char* str);
};


#endif