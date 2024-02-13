//
// Created by vladimir on 27.01.23.
//
#include "Controller.h"

Controller::Controller(Model *model) {
    model_ = model;
}

void Controller::Navigation() {
    Adapter::InitPage();
    Adapter::RefreshPage();

    int ch;


    Adapter::FirstInit();

    model_->GetPage().SetWin(stdscr);

    model_->UpdatePageSize();

    int prev_symbol = 0;

    while (true) {
        if ((ch = getch()) == ERR);
        else {
            if (ch == 'q') { //
                endwin();
                return;
            } else if (ch == 'f') { //
                model_->OpenFile("inputLarge.txt");
            } else if (ch == KEY_NPAGE) {
                model_->PageDown();
            } else if (ch == KEY_DOWN) {
                model_->CursorDown();
            } else if (ch == KEY_UP) {
                model_->CursorUp();
            } else if (ch == KEY_LEFT) {
                model_->CursorLeft();
            } else if (ch == KEY_RIGHT) {
                model_->CursorRight();
            } else if (ch == KEY_RESIZE) {
                model_->UpdatePageSize();
            } else if (ch == KEY_PPAGE) {
                model_->PageUp();
            } else if (ch == 'g') {
                if (prev_symbol == 'g') {
                    model_->FirstPage();
                    prev_symbol = 0;
                    continue;
                }
            } else if (ch == 'G') {
                model_->LastPage();
            } else if (ch == '^') {
                model_->StartOfTheLine();
            } else if (ch == '$') {
                model_->EndOfTheLine();
            } else if (ch == 'y') {
                model_->CopyLine();
            } else if (ch == 'd') {
                if (prev_symbol == 'd') {
                    model_->CutLine();
                    prev_symbol = 0;
                    continue;
                }
            } else if (ch == 'b') {
                model_->StartOfTheWord();
            } else if (ch == 'w') {
                model_->EndOfTheWord();
            } else if (ch == 'i') {
                if (prev_symbol == 'd') {
                    model_->DeleteWord();
                    prev_symbol = 0;
                    continue;
                }
            } else if (ch == 'c') {
                if (prev_symbol == 'd') {
                    model_->CopyWord();
                    prev_symbol = 0;
                    continue;
                }
            } else if (ch == 'p') {
                model_->InsertFromBuffer();
            } else if (ch == ':') { //
                CommandMode();
            } else if (ch == 'B') { //|| ch == 'I' || ch == 'S' || ch == 'A') {
                InsertMode(SimpleInput);
            } else if (ch == 'I') {
                InsertMode(BeginOfLineInput);
            } else if (ch == 'S') {
                InsertMode(EndOfTheLineInput);
            } else if (ch == 'A') {
                InsertMode(DeleteLineAndInput);
            }
            prev_symbol = ch;
        }
    }
}

void Controller::CommandMode() {
    model_->EnableCommandMode();

    int ch;
    while (true) {
        if ((ch = getch()) == ERR);
        else {
            if (ch == KEY_ENTER_) {
                model_->ParseCommand();
                if (model_->IsFindMode())
                    FindMode();
            } else if (ch == KEY_LEFT) {
                model_->CommandCursorLeft();
            } else if (ch == KEY_RIGHT) {
                model_->CommandCursorRight();
            } else if (ch == KEY_RESIZE) {
                model_->UpdatePageSize();
            } else if (ch == KEY_BACKSPACE) {
                model_->CommandDeleteChar();
            } else if (ch == KEY_ESC_) {
                model_->DisableCommandMode();
                return;
            } else {
                model_->CommandInsertChar(char(ch));
            }
        }
    }
}

void Controller::InsertMode(int mode) {
    model_->EnableInputMode();

    if (mode == BeginOfLineInput)
        model_->StartOfTheLine();
    else if (mode == EndOfTheLineInput)
        model_->EndOfTheWord();
    else if (mode == DeleteLineAndInput) {
        model_->DeleteLine();
    }

    int ch;
    while (true) {
        if ((ch = getch()) == ERR);
        else {
            if (ch == KEY_ESC_) {
                model_->DisableInputMode();
                return;
            } else if (ch == KEY_NPAGE) {
                model_->PageDown();
            } else if (ch == KEY_DOWN) {
                model_->CursorDown();
            } else if (ch == KEY_UP) {
                model_->CursorUp();
            } else if (ch == KEY_LEFT) {
                model_->CursorLeft();
            } else if (ch == KEY_RIGHT) {
                model_->CursorRight();
            } else if (ch == KEY_RESIZE) {
                model_->UpdatePageSize();
            } else if (ch == KEY_BACKSPACE) {
                model_->DeleteCharacterBeforePos();
            } else if (ch == KEY_DELETE_) {
                model_->DeleteCharacterAtPos();
            } else if (ch == KEY_PPAGE) {
                model_->PageUp();
            } else {
                model_->InsertCharacter(char(ch));
                model_->MakeNotSaved();
            }
        }
    }
}

void Controller::FindMode() {
    int ch;
    while (true) {
        if ((ch = getch()) == ERR);
        else {
            if (ch == KEY_ESC_) {
                model_->DisableFindMode();
                return;
            } else if (ch == KEY_RESIZE) {
                model_->UpdatePageSize();
            } else if (ch == 'n') {
                model_->FindNext();
            } else if (ch == 'p') {
                model_->FindPrev();
            }
        }
    }
}
