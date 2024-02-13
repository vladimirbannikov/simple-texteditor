//
// Created by vladimir on 27.01.23.
//

#ifndef TEXTEDITOR_MODEL_H
#define TEXTEDITOR_MODEL_H

#include <MyString.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <ncurses.h>
#include <utility>
#include <map>
#include "../../lib/clip-1.5/clip.h"
#include "../Adapter.h"

#include "../Observe.h"

using namespace std;

void Log(const string &what);

enum mode {
    NavigationMode,
    SearchMode,
    InputMode,
    CommandMode
};

enum custom_errors {
    NotFindNext = -1,
    NotFindPrev = -2
};

struct Commands {
    std::string OpenTheFile = "o ";
    std::string WriteAndClose1 = "x";
    std::string Write = "w";
    std::string WriteToFile = "w ";
    std::string Exit = "q";
    std::string ExitWithoutSaving = "q!";
    std::string WriteAndClose2 = "wq!";
    std::string GoToTheLine = "line ";
    std::string Help = "h";
    std::string FindPrev = "?";
    std::string FindNext = "/";
    std::string LineNumbering = "set num";
};

class Page {
    WINDOW *window_ = stdscr;
    vector<std::string> strings_; // можно использовать вектор указателей на строки
    vector<size_t> numbers_of_strs_;
    std::pair<int, int> cursor_pos_;
    std::pair<int, int> page_size_;
    std::string command_string;
    std::string file_name;
public:
    Page() = default;

    explicit Page(WINDOW *window);

    void UpdateSize();

    std::pair<int, int> GetWinSize();

    void SetWinSize(std::pair<int, int> new_size);

    void SetWin(WINDOW *window);

    void SetCursorPos(std::pair<int, int> cursor_pos);

    std::pair<int, int> GetCursorPos();

    const vector<std::string> &GetStrings();

    void AddStringBack(const std::string &str);

    void AddStringFront(const std::string &str);

    void AddNumBack(int num);

    void AddNumFront(int num);

    void ClearNums();

    const vector<size_t> &GetNums();

    void ClearStrings();

    std::string GetCurrentStr();

    void ClearCommandLine();

    std::string GetCommandString();

    void SetCommandStr(const std::string &str);

    std::pair<int, int> GetMaxWinSize();
};

class Model : public Observable {
    int mode_;
    string text_;
    string filename_;
    string path_to_file_;
    bool saved_ = false;
    size_t str_pos_ = 0;
    Page page_;
    std::pair<size_t, size_t> firstLastStrPos_;
    bool printWholePage_ = true;

    Commands commands_;
    size_t command_str_pos_ = 1;
    std::pair<size_t, size_t> firstLastCommandPos_;
    string command_line_;
    bool command_error_ = false;
    string error_command_;

    size_t maxLines_ = 0;
    size_t currentLine_ = 0;
    map<size_t, size_t> numToStartOfLine_;
    map<size_t, size_t> startOfTheLineToNum_;

    std::string status_bar_;

    bool find_mode_ = false;
    std::string find_str_;
    bool find_again_ = false;

    std::map<int, std::string> customErrorsMap_;

    bool numbering_mode = false;
    int16_t maxSizeOfLineNum = 1;
    bool first_use_of_num_mode = true;

public:
    Model();

    explicit Model(WINDOW *window);

    int OpenFile(const std::string &filename);

    int WriteToFile(const std::string &filename);

    int CloseFile();

    std::string GetText();

    std::string GetStatusBar();

    size_t GetCurrentStrPos();

    Page &GetPage();

    void UpdatePageSize();

    std::pair<int, int> UpdateCursorInNewWindow();

    void UpdateStrPos();

    size_t FindStrLine(size_t pos);

    void InitPage();

    std::string GetNextLine(size_t strPos, int size_x);

    std::string GetPrevLine(size_t strPos, int size_x);

    void InitLines();

    bool IsCommandError();

    void InitLinesFrom(size_t startLine);

    void CalcCurStrNum();

    void InitStatusBar();

    void CursorLeft();

    void CursorRight();

    void CursorUp();

    void CursorDown();

    int PageDown();

    void PageUp();

    void FirstPage();

    void LastPage();

    bool PrintWholePage();

    void EnableInputMode();

    void DisableInputMode();

    void MakeNotSaved();

    void MakeSaved();

    void InsertText(const std::string &str, size_t pos);

    void InsertCharacter(char ch);

    void InsertFromBuffer();

    void DeleteCharacterBeforePos();

    void DeleteCharacterAtPos();

    void DeleteText(size_t begin, size_t end);

    void DeleteWord();

    void StartOfTheLine();

    void EndOfTheLine();

    void StartOfTheWord();

    void EndOfTheWord();

    void CopyLine();

    void CutLine();

    void DeleteLine();

    size_t CurSize();

    void CopyWord();

    void GoToTheLine(uint16_t number);

    bool IsCommandMode() const;

    std::string GetNextCommandLine(size_t strPos, int size_x);

    std::string GetPrevCommandLine(size_t strPos, int size_x);

    void CommandCursorLeft();

    void CommandCursorRight();

    void CommandLineInit();

    std::pair<int, int> UpdateCommandCursor();

    void CommandDeleteChar();

    void CommandInsertChar(char ch);

    void CommandInsertFromBuf();

    std::string CreateFilenameForStatusBar(const std::string& str);

    void ParseCommand();

    void CommandErrorHandler(int err);

    std::string ParseLine(const std::string& line);

    void EnableCommandMode();

    void DisableCommandMode();

    bool IsFindMode();

    void EnableFindMode();

    void DisableFindMode();

    void FindNext();

    void FindPrev();

    long long FindNextMatch(const std::string& str, size_t pos);

    long long FindPrevMatch(const std::string& str, size_t pos);

    void CustomErrorHandler(int err);

    void EnableNumbering();

    void DisableNumbering();

    bool IsNumberingMode();

    void CalculateMaxSizeOfLineNum();

    void CalculateMinSizeOfLineNum();

    int GetNumsPanelSize();
};


#endif //TEXTEDITOR_MODEL_H
