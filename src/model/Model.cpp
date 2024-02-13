//
// Created by vladimir on 27.01.23.
//

#include "Model.h"

int Model::OpenFile(const std::string &filename) {
    std::ifstream infile(filename, std::ios_base::in);

    if (!infile.is_open()) return errno;

    infile.seekg(0, ios::end);
    text_.reserve(infile.tellg());
    infile.seekg(0, std::ios::beg);
    text_.assign((std::istreambuf_iterator<char>(infile)),
                 std::istreambuf_iterator<char>());

    infile.close();

    path_to_file_ = filename;

    filename_ = CreateFilenameForStatusBar(filename);
    MakeSaved();
    str_pos_ = 0;
    firstLastStrPos_.first = firstLastStrPos_.second = 0;

    if (!IsCommandMode())
        page_.SetCursorPos(std::make_pair(0, 0));

    InitLines();

    InitPage();

    InitStatusBar();

    printWholePage_ = true;
    NotifyUpdate();
    return 0;
}

std::string Model::GetText() {
    return text_;
}

void Log(const string &what) {
    std::ofstream log("log.txt", std::ios_base::app);
    log << what << '\n';
    log.close();
}

void Model::InitPage() {
    if (text_.empty()) return;

    page_.ClearStrings();
    if (IsNumberingMode())
        page_.ClearNums();

    bool fl_for_exit = false;
    int i = 0;
    std::string newPage;
    size_t lastStrPos = firstLastStrPos_.first;
    std::pair<int, int> pageSize = page_.GetWinSize();


    while (!fl_for_exit && i < pageSize.first) {
        newPage = GetNextLine(lastStrPos, pageSize.second);
        if (newPage.empty()) {
            fl_for_exit = true;
            continue;
        }

        if (IsNumberingMode())
            page_.AddNumBack(startOfTheLineToNum_[lastStrPos]);

        lastStrPos += newPage.size();
        page_.AddStringBack(newPage);

        i++;
    }

    firstLastStrPos_.second = lastStrPos - 1;

    if (!IsCommandMode() || IsFindMode())
        page_.SetCursorPos(UpdateCursorInNewWindow());
}

void Model::UpdateStrPos() {
    if (str_pos_ < firstLastStrPos_.first) {
        str_pos_ = firstLastStrPos_.first;
    }

    size_t true_pos = firstLastStrPos_.first;
    std::vector<std::string> page_strs = page_.GetStrings();
    int x = 0, y = 0;
    for (; y < page_strs.size(); y++) {
        for (x = 0; x < page_strs[y].size(); x++) {
            if (true_pos == str_pos_)
                return;
            true_pos++;
        }
    }
    str_pos_ = firstLastStrPos_.second;
}

void Model::UpdatePageSize() {
    page_.UpdateSize();

    if (IsCommandMode()) {
        page_.SetWinSize(std::make_pair(page_.GetWinSize().first - 1, page_.GetWinSize().second));
        CommandLineInit();
    }

    if (!IsNumberingMode())
        InitLines();
    else {
        first_use_of_num_mode = true;
        CalculateMaxSizeOfLineNum();
    }


    firstLastStrPos_.first = FindStrLine(firstLastStrPos_.first);

    InitPage();

    InitStatusBar();

    printWholePage_ = true;
    NotifyUpdate();
}


void Model::InitLines() {
    numToStartOfLine_.clear();
    startOfTheLineToNum_.clear();
    bool fl_for_exit = false;
    int i = 1;
    std::string newPage;
    size_t lastStrPos = 0;
    std::pair<int, int> pageSize = page_.GetWinSize();

    while (!fl_for_exit) {
        newPage = GetNextLine(lastStrPos, pageSize.second);
        if (newPage.empty()) {
            fl_for_exit = true;
            continue;
        }
        numToStartOfLine_[i] = lastStrPos;
        startOfTheLineToNum_[lastStrPos] = i;
        lastStrPos += newPage.size();
        i++;
    }

    maxLines_ = i - 1;
    maxSizeOfLineNum = to_string(maxLines_).size();
}

void Model::InitLinesFrom(size_t startLine) {
    if (startLine < 1) return;

    size_t deleteLine = startLine + 1; // удаляем все след. строки
    for (; deleteLine < maxLines_; deleteLine++) {
        if (numToStartOfLine_.find(deleteLine) != numToStartOfLine_.end()) {
            if (startOfTheLineToNum_.find(numToStartOfLine_[deleteLine]) != startOfTheLineToNum_.end()) {
                startOfTheLineToNum_.erase(numToStartOfLine_[deleteLine]);
            }
            numToStartOfLine_.erase(deleteLine);
        }
    }

    bool fl_for_exit = false;
    size_t i = startLine;
    std::string newPage;
    size_t lastStrPos = numToStartOfLine_[startLine];
    std::pair<int, int> pageSize = page_.GetWinSize();

    while (!fl_for_exit) {
        newPage = GetNextLine(lastStrPos, pageSize.second);
        if (newPage.empty()) {
            fl_for_exit = true;
            continue;
        }
        numToStartOfLine_[i] = lastStrPos;
        startOfTheLineToNum_[lastStrPos] = i;
        lastStrPos += newPage.size();
        i++;
    }

    maxLines_ = i - 1;
    maxSizeOfLineNum = to_string(maxLines_).size();
}

std::string Model::GetNextLine(size_t strPos, int size_x) {
    std::string res;
    int i = 0;
    while (text_[strPos + i] != '\n' && i < size_x && text_[strPos + i] != '\0') {
        res.append(1, text_[strPos + i]);
        i++;
    }
    if (text_[strPos + i] == '\n' && i < size_x) res.append(1, '\n');
    return res;
}


std::string Model::GetPrevLine(size_t strPos, int size_x) {
    if (strPos == 0) return "";

    if (strPos == 1 && text_[0] == '\n') return "\n";

    size_t curPos = strPos;
    std::string res;
    curPos--;
    res += text_[curPos];
    curPos--;
    while (true) {
        if (curPos <= 0 || text_[curPos] == '\n') {
            if (curPos == 0)
                res += text_[curPos];

            size_t tailSize = res.size() % size_x;
            if (tailSize == 0 && res.size() >= size_x)
                tailSize = size_x;

            std::string temp;
            for (size_t i = 0; i < tailSize; i++) {
                temp += res[i];
            }
            res = temp;

            break;
        }

        res += text_[curPos];
        curPos--;
    }

    size_t j = res.size() - 1;
    for (int i = 0; i < res.size() / 2; i++) {
        std::swap(res[i], res[j]);
        j--;
    }

    return res;
}

Page &Model::GetPage() {
    return page_;
}

void Model::CursorLeft() {
    if (str_pos_ <= 0) return;
    std::pair<int, int> cursor = page_.GetCursorPos();
    std::vector<std::string> stringsOnPage = page_.GetStrings();

    if (cursor.second == 0) { //если перенос на предыдущую строку
        if (cursor.first == 0) { //если первая строка на странице
            firstLastStrPos_.second -= stringsOnPage[stringsOnPage.size() - 1].size();

            std::string newLine = GetPrevLine(firstLastStrPos_.first, page_.GetWinSize().second);
            page_.AddStringFront(newLine);
            firstLastStrPos_.first -= newLine.size();

            if (IsNumberingMode())
                page_.AddNumFront(startOfTheLineToNum_[firstLastStrPos_.first]);

            cursor = std::make_pair(cursor.first, newLine.size() - 1);
            printWholePage_ = true;
        } else {
            cursor = std::make_pair(cursor.first - 1, stringsOnPage[cursor.first - 1].size() - 1);
            printWholePage_ = false;
        }
        str_pos_--;
        page_.SetCursorPos(cursor);
        InitStatusBar();
    } else {
        cursor = std::make_pair(cursor.first, cursor.second - 1);
        printWholePage_ = false;
        str_pos_--;
        page_.SetCursorPos(cursor);
    }

    NotifyUpdate();
}

void Model::CursorRight() {
    if (str_pos_ + 1 >= text_.size()) //if last symbol
        return;
    std::pair<int, int> cursor = page_.GetCursorPos();
    std::vector<std::string> stringsOnPage = page_.GetStrings();

    if (cursor.second + 1 == stringsOnPage[cursor.first].size()) { // если перенос на след строку
        if (cursor.first + 1 == stringsOnPage.size()) { // if last string on page_
            firstLastStrPos_.first += stringsOnPage[0].size();

            std::string newLine = GetNextLine(firstLastStrPos_.second + 1, page_.GetWinSize().second);
            page_.AddStringBack(newLine);
            firstLastStrPos_.second += newLine.size();

            if (IsNumberingMode())
                page_.AddNumBack(startOfTheLineToNum_[firstLastStrPos_.second - newLine.size() + 1]);

            cursor = std::make_pair(cursor.first, 0);
            printWholePage_ = true;
        } else {
            cursor = std::make_pair(cursor.first + 1, 0);
            printWholePage_ = false;
        }
        str_pos_++;
        page_.SetCursorPos(cursor);
        InitStatusBar();
    } else {
        cursor = std::make_pair(cursor.first, cursor.second + 1);
        printWholePage_ = false;
        str_pos_++;
        page_.SetCursorPos(cursor);
    }

    NotifyUpdate();
}

void Model::CursorUp() {
    std::vector<std::string> stringsOnPage = page_.GetStrings();
    std::pair<int, int> cursor = page_.GetCursorPos();


    if (cursor.first == 0) {
        std::string newLine = GetPrevLine(firstLastStrPos_.first, page_.GetWinSize().second);
        if (newLine.empty()) return;

        if (stringsOnPage.size() == page_.GetWinSize().first)
            firstLastStrPos_.second -= stringsOnPage[stringsOnPage.size() - 1].size();

        firstLastStrPos_.first -= newLine.size();

        page_.AddStringFront(newLine);

        if (IsNumberingMode())
            page_.AddNumFront(startOfTheLineToNum_[firstLastStrPos_.first]);

        if (newLine.size() - 1 < cursor.second) {
            cursor = std::make_pair(cursor.first, newLine.size() - 1);
        } else {
            cursor = std::make_pair(cursor.first, cursor.second);
        }

        printWholePage_ = true;

    } else {
        if (stringsOnPage[cursor.first - 1].size() - 1 < cursor.second) {
            cursor = std::make_pair(cursor.first - 1, stringsOnPage[cursor.first - 1].size() - 1);
        } else {
            cursor = std::make_pair(cursor.first - 1, cursor.second);
        }
        printWholePage_ = false;
    }

    stringsOnPage = page_.GetStrings();

    size_t newPos = str_pos_ - (page_.GetCursorPos().second +
                                stringsOnPage[cursor.first].size() - cursor.second);

    str_pos_ = newPos;

    page_.SetCursorPos(cursor);

    InitStatusBar();

    NotifyUpdate();
}

void Model::CursorDown() {
    std::vector<std::string> stringsOnPage = page_.GetStrings();
    std::pair<int, int> cursor = page_.GetCursorPos();

    if (cursor.first + 1 >= stringsOnPage.size()) {
        std::string newLine = GetNextLine(firstLastStrPos_.second + 1, page_.GetWinSize().second);
        if (newLine.empty()) return;

        firstLastStrPos_.first += stringsOnPage[0].size();
        firstLastStrPos_.second += newLine.size();

        page_.AddStringBack(newLine);

        if (IsNumberingMode())
            page_.AddNumBack(startOfTheLineToNum_[firstLastStrPos_.second - newLine.size() + 1]);

        if (newLine.size() - 1 < cursor.second) {
            cursor = std::make_pair(cursor.first, newLine.size() - 1);
        } else {
            cursor = std::make_pair(cursor.first, cursor.second);
        }

        printWholePage_ = true;

    } else {
        if (stringsOnPage[cursor.first + 1].size() - 1 < cursor.second) {
            cursor = std::make_pair(cursor.first + 1, stringsOnPage[cursor.first + 1].size() - 1);
        } else {
            cursor = std::make_pair(cursor.first + 1, cursor.second);
        }
        printWholePage_ = false;
    }

    size_t newPos = str_pos_ + (stringsOnPage[page_.GetCursorPos().first].size()
                                - page_.GetCursorPos().second + cursor.second);

    str_pos_ = newPos;

    page_.SetCursorPos(cursor);

    InitStatusBar();

    NotifyUpdate();
}

Model::Model(WINDOW *window) : page_(window) {}

std::pair<int, int> Model::UpdateCursorInNewWindow() {
    if (str_pos_ < firstLastStrPos_.first) {
        str_pos_ = firstLastStrPos_.first;
        return std::make_pair(0, 0);
    }

    size_t true_pos = firstLastStrPos_.first;
    std::vector<std::string> page_strs = page_.GetStrings();
    int x = 0, y = 0;
    for (; y < page_strs.size(); y++) {
        for (x = 0; x < page_strs[y].size(); x++) {
            if (true_pos == str_pos_)
                return std::make_pair(y, x);
            true_pos++;
        }
    }
    str_pos_ = firstLastStrPos_.second;
    return std::make_pair(y - 1, x - 1);
}


bool Model::PrintWholePage() {
    return printWholePage_;
}

size_t Model::GetCurrentStrPos() {
    return str_pos_;
}

Model::Model() : page_(), firstLastStrPos_(0, 0), firstLastCommandPos_(0, 0) {
    text_ = '\n';
    command_line_ = ">\n";
    mode_ = NavigationMode;
    filename_ = "";
    customErrorsMap_[NotFindNext] = "Not find next match";
    customErrorsMap_[NotFindPrev] = "Not find previous match";
}

int Model::PageDown() {
    std::string newLine = GetNextLine(firstLastStrPos_.second + 1, page_.GetWinSize().second);
    if (newLine.empty()) return 1;

    firstLastStrPos_.first = firstLastStrPos_.second + 1;

    InitPage();

    page_.SetCursorPos(std::make_pair(0, 0));

    str_pos_ = firstLastStrPos_.first;

    printWholePage_ = true;

    InitStatusBar();

    NotifyUpdate();

    return 0;
}

void Model::InsertText(const std::string &str, size_t pos) {
    if (text_.empty()) {
        text_ = str;
        return;
    }

    if (pos >= text_.size()) return;
    text_.insert(pos, str);
}

void Model::InsertCharacter(char ch) {
    InsertText(std::string() + ch, str_pos_);

    if (text_.size() > 1) str_pos_++;

    size_t prev_max = maxSizeOfLineNum;

    InitLinesFrom(currentLine_);

    if (IsNumberingMode()) {
        if (maxSizeOfLineNum != prev_max)
            CalculateMaxSizeOfLineNum();
    }

    InitPage();

    InitStatusBar();

    printWholePage_ = true;

    NotifyUpdate();
}

void Model::InsertFromBuffer() {
    std::string buf;
    clip::get_text(buf);

    if (buf.empty()) return;

    if (str_pos_ < text_.size() - 1)
        InsertText(buf, str_pos_ + 1);
    else InsertText(buf, str_pos_);

    if (!filename_.empty())
        MakeNotSaved();

    size_t prev_max = maxSizeOfLineNum;

    InitLinesFrom(currentLine_);

    if (IsNumberingMode()) {
        if (maxSizeOfLineNum != prev_max)
            CalculateMaxSizeOfLineNum();
    }

    InitPage();

    InitStatusBar();

    printWholePage_ = true;

    NotifyUpdate();
}

void Model::DeleteText(size_t begin, size_t end) {
    if (begin > end) return;

    if (end >= text_.size() - 1)
        end = text_.size() - 2;


    text_.erase(begin, end - begin + 1);
}

void Model::DeleteCharacterBeforePos() {
    if (str_pos_ == 0) return;
    if (str_pos_ >= text_.size()) return;

    CursorLeft();

    DeleteText(str_pos_, str_pos_);

    size_t prev_max = maxSizeOfLineNum;

    InitLinesFrom(currentLine_);

    if (IsNumberingMode()) {
        if (maxSizeOfLineNum != prev_max) {
            CalculateMinSizeOfLineNum();
        }
    }

    if (!filename_.empty())
        MakeNotSaved();

    InitPage();

    InitStatusBar();

    printWholePage_ = true;

    NotifyUpdate();
}

void Model::DeleteCharacterAtPos() {
    if (str_pos_ >= text_.size() - 1) return; // for '/n' in the end

    DeleteText(str_pos_, str_pos_);

    if (str_pos_ >= text_.size() && str_pos_ != 0)
        str_pos_--;

    size_t prev_max = maxSizeOfLineNum;

    InitLinesFrom(currentLine_);

    if (IsNumberingMode()) {
        if (maxSizeOfLineNum != prev_max) {
            CalculateMinSizeOfLineNum();
        }
    }

    if (!filename_.empty())
        MakeNotSaved();

    InitPage();

    InitStatusBar();

    printWholePage_ = true;

    NotifyUpdate();
}

void Model::PageUp() {
    int count = page_.GetWinSize().first;

    for (int i = 0; i < count + page_.GetCursorPos().first; i++) {
        CursorUp();
    }

    page_.SetCursorPos(std::make_pair(0, 0));

    str_pos_ = firstLastStrPos_.first;

    printWholePage_ = true;

    InitStatusBar();

    NotifyUpdate();
}

void Model::FirstPage() {
    str_pos_ = 0;
    firstLastStrPos_.first = firstLastStrPos_.second = 0;
    page_.SetCursorPos(std::make_pair(0, 0));
    InitPage();

    printWholePage_ = true;

    InitStatusBar();

    NotifyUpdate();
}

void Model::LastPage() {
    int a = 0;
    while (a != 1) {
        a = PageDown();
    }
}

void Model::StartOfTheLine() {
    int count = page_.GetCursorPos().second;
    for (int i = 0; i < count; i++) {
        CursorLeft();
    }
}

void Model::EndOfTheLine() {
    int count = page_.GetCurrentStr().size() - page_.GetCursorPos().second - 1;
    for (int i = 0; i < count; i++) {
        CursorRight();
    }
}

void Model::CopyLine() {
    std::string line = page_.GetCurrentStr();

    clip::set_text(line);
}

void Model::DeleteLine() {
    std::string line = page_.GetCurrentStr();
    size_t begin = str_pos_ - page_.GetCursorPos().second;
    size_t end = str_pos_ + (line.size() - page_.GetCursorPos().second - 1);

    if (page_.GetCursorPos().first == 0) {
        std::string prevLine = GetPrevLine(begin, page_.GetWinSize().second);
        if (prevLine.empty()) {
            str_pos_ = 0;
            firstLastStrPos_.first = str_pos_;
        } else {
            str_pos_ = begin - prevLine.size();
            firstLastStrPos_.first = str_pos_;
        }
    } else {
        str_pos_ = begin - page_.GetStrings()[page_.GetCursorPos().first - 1].size();
    }

    DeleteText(begin, end);

    if (!filename_.empty())
        MakeNotSaved();

    size_t prev_max = maxSizeOfLineNum;

    InitLinesFrom(currentLine_);

    if (IsNumberingMode()) {
        if (maxSizeOfLineNum != prev_max) {
            CalculateMinSizeOfLineNum();
        }
    }

    InitPage();

    InitStatusBar();

    printWholePage_ = true;

    NotifyUpdate();
}

void Model::CutLine() {
    CopyLine();
    DeleteLine();
}

void Model::StartOfTheWord() {
    if (str_pos_ == 0) return;
    if (text_[str_pos_] == ' ' || text_[str_pos_] == '\n') return;
    size_t start = str_pos_;
    if (text_[start - 1] == ' ' || text_[start - 1] == '\n') {
        start--;
        while (text_[start] == ' ' || text_[start] == '\n') {
            if (start == 0) return;
            start--;
        }
    }

    while (text_[start] != ' ' && text_[start] != '\n' && start != 0) {
        start--;
    }
    if (text_[start] == ' ' || text_[start] == '\n') {
        start++;
    }

    while (str_pos_ != start) {
        CursorLeft();
    }
}

void Model::EndOfTheWord() {
    if (str_pos_ >= text_.size() - 2) return;
    if (text_[str_pos_] == ' ' || text_[str_pos_] == '\n') return;

    size_t end = str_pos_;
    if (text_[end + 1] == ' ' || text_[end + 1] == '\n') {
        end++;
        while (text_[end] == ' ' || text_[end] == '\n') {
            if (end >= text_.size() - 2) return;
            end++;
        }
    }

    while (text_[end] != ' ' && text_[end] != '\n' && end < text_.size() - 2) {
        end++;
    }
    if (text_[end] == ' ' || text_[end] == '\n') {
        end--;
    }

    while (str_pos_ != end) {
        CursorRight();
    }

}

void Model::DeleteWord() {
    if (text_[str_pos_] == ' ' || text_[str_pos_] == '\n') return;

    size_t start = str_pos_;
    while (text_[start] != ' ' && text_[start] != '\n' && start != 0) {
        start--;
    }
    if (text_[start] == ' ' || text_[start] == '\n') {
        start++;
    }

    size_t end = str_pos_;
    while (text_[end] != ' ' && text_[end] != '\n' && end < text_.size() - 2) {
        end++;
    }
    if (text_[end] == ' ' || text_[end] == '\n') {
        end--;
    }

    DeleteText(start, end);

    str_pos_ = start;

    if (!filename_.empty())
        MakeNotSaved();

    size_t prev_max = maxSizeOfLineNum;

    InitLinesFrom(currentLine_);

    if (IsNumberingMode()) {
        if (maxSizeOfLineNum != prev_max) {
            CalculateMinSizeOfLineNum();
        }
    }

    CursorLeft();

    InitPage();

    InitStatusBar();

    printWholePage_ = true;

    NotifyUpdate();
}

void Model::CopyWord() {
    if (text_[str_pos_] == ' ' || text_[str_pos_] == '\n') return;

    size_t start = str_pos_;
    while (text_[start] != ' ' && text_[start] != '\n' && start != 0) {
        start--;
    }
    if (text_[start] == ' ' || text_[start] == '\n') {
        start++;
    }

    size_t end = str_pos_;
    while (text_[end] != ' ' && text_[end] != '\n' && end < text_.size() - 2) {
        end++;
    }
    if (text_[end] == ' ' || text_[end] == '\n') {
        end--;
    }

    std::string buf;
    while (start <= end) {
        buf += text_[start];
        start++;
    }

    clip::set_text(buf);
}

void Model::GoToTheLine(uint16_t number) {
    if (number > maxLines_ || number < 1) return;

    if (numToStartOfLine_.find(number) != numToStartOfLine_.end()) {
        str_pos_ = numToStartOfLine_[number];
    } else return;

    if (firstLastStrPos_.first > str_pos_ || str_pos_ > firstLastStrPos_.second) {
        firstLastStrPos_.first = str_pos_;
    }

    currentLine_ = number;

    InitPage();

    InitStatusBar();

    NotifyUpdate();
}

void Model::CommandLineInit() {
    if (command_line_.empty()) return;

    page_.ClearCommandLine();
    bool fl_for_exit = false;
    int i = 0;
    std::string newPage;
    size_t lastStrPos = firstLastCommandPos_.first;
    std::pair<int, int> pageSize = page_.GetWinSize();


    while (!fl_for_exit && i < pageSize.first) {
        newPage = GetNextCommandLine(lastStrPos, pageSize.second);
        if (newPage.empty()) {
            fl_for_exit = true;
            continue;
        }
        lastStrPos += newPage.size();
        page_.SetCommandStr(newPage);
        i++;
    }

    firstLastCommandPos_.second = lastStrPos - 1;

    page_.SetCursorPos(UpdateCommandCursor());
}

std::string Model::GetNextCommandLine(size_t strPos, int size_x) {
    std::string res;
    int i = 0;
    while (command_line_[strPos + i] != '\n' && i < size_x && command_line_[strPos + i] != '\0') {
        res.append(1, command_line_[strPos + i]);
        i++;
    }
    if (command_line_[strPos + i] == '\n' && i < size_x) res.append(1, '\n');
    return res;
}

std::string Model::GetPrevCommandLine(size_t strPos, int size_x) {
    if (strPos == 0) return "";

    if (strPos == 1 && command_line_[0] == '\n') return "\n";

    size_t curPos = strPos;
    std::string res;
    curPos--;
    res += command_line_[curPos];
    curPos--;
    while (true) {
        if (curPos <= 0 || command_line_[curPos] == '\n') {
            if (curPos == 0)
                res += command_line_[curPos];

            size_t tailSize = res.size() % size_x;
            if (tailSize == 0 && res.size() >= size_x)
                tailSize = size_x;

            std::string temp;
            for (size_t i = 0; i < tailSize; i++) {
                temp += res[i];
            }
            res = temp;

            break;
        }

        res += command_line_[curPos];
        curPos--;
    }

    size_t j = res.size() - 1;
    for (int i = 0; i < res.size() / 2; i++) {
        std::swap(res[i], res[j]);
        j--;
    }

    return res;
}

void Model::CommandCursorLeft() {
    if (command_str_pos_ <= 1) return;
    std::pair<int, int> cursor = page_.GetCursorPos();
    std::string str = page_.GetCommandString();

    if (cursor.second == 0) { //если перенос на предыдущую строку
        firstLastCommandPos_.second -= str.size();

        std::string newLine = GetPrevCommandLine(firstLastCommandPos_.first, page_.GetWinSize().second);
        page_.SetCommandStr(newLine);
        firstLastCommandPos_.first -= newLine.size();
        cursor = std::make_pair(page_.GetWinSize().first, newLine.size() - 1);
        printWholePage_ = true;
    } else {
        cursor = std::make_pair(page_.GetWinSize().first, cursor.second - 1);
        printWholePage_ = false;
    }

    command_str_pos_--;
    page_.SetCursorPos(cursor);

    NotifyUpdate();
}

void Model::CommandCursorRight() {
    if (command_str_pos_ + 1 >= command_line_.size()) //if last symbol
        return;

    std::pair<int, int> cursor = page_.GetCursorPos();
    std::string str = page_.GetCommandString();

    if (cursor.second + 1 == str.size()) { // перенос
        firstLastCommandPos_.first += str.size();

        std::string newLine = GetNextCommandLine(firstLastCommandPos_.second + 1, page_.GetWinSize().second);
        page_.SetCommandStr(newLine);
        firstLastCommandPos_.second += newLine.size();
        cursor = std::make_pair(page_.GetWinSize().first, 0);
        printWholePage_ = true;
    } else {
        cursor = std::make_pair(page_.GetWinSize().first, cursor.second + 1);
        printWholePage_ = false;
    }

    command_str_pos_++;
    page_.SetCursorPos(cursor);

    NotifyUpdate();
}

void Model::CommandDeleteChar() {
    if (IsCommandError()) return;

    if (command_str_pos_ == 1) return;
    if (command_str_pos_ >= command_line_.size()) return;

    CommandCursorLeft();

    command_line_.erase(command_str_pos_, 1);

    CommandLineInit();

    printWholePage_ = true;

    NotifyUpdate();
}

std::pair<int, int> Model::UpdateCommandCursor() {
    if (command_str_pos_ < firstLastCommandPos_.first) {
        command_str_pos_ = firstLastCommandPos_.first;
        return std::make_pair(page_.GetWinSize().first, 0);
    }

    size_t true_pos = firstLastCommandPos_.first;
    std::string str = page_.GetCommandString();
    int x = 0;
    for (x = 0; x < str.size(); x++) {
        if (true_pos == command_str_pos_)
            return std::make_pair(page_.GetWinSize().first, x);
        true_pos++;
    }
    command_str_pos_ = firstLastCommandPos_.second;

    return std::make_pair(page_.GetWinSize().first, x - 1);
}

void Model::ParseCommand() {
    if (command_error_) {
        command_error_ = false;
        command_line_ = error_command_;
        error_command_.clear();

        InitStatusBar();

        NotifyUpdate();
        return;
    }

    if (command_line_.size() < 2) return;

    string tempCommandStr = command_line_;
    tempCommandStr.erase(tempCommandStr.size() - 1);
    tempCommandStr = tempCommandStr.substr(1, tempCommandStr.size() - 1);

    if (tempCommandStr == commands_.Help) {
        //help
        return;
    } else if (tempCommandStr == commands_.Exit || tempCommandStr == commands_.ExitWithoutSaving) {
        endwin();
        std::exit(EXIT_SUCCESS);
    } else if (tempCommandStr == commands_.WriteAndClose1 || tempCommandStr == commands_.WriteAndClose2) {
        int err = WriteToFile(path_to_file_);
        if (err != 0) {
            CommandErrorHandler(err);
            return;
        }
        err = CloseFile();
        if (err != 0) {
            CommandErrorHandler(err);
            return;
        }
        return;
    } else if (tempCommandStr == commands_.Write) {
        int err = WriteToFile(path_to_file_);
        if (err != 0) {
            CommandErrorHandler(err);
            return;
        }
        return;
    } else if (tempCommandStr == commands_.LineNumbering) {
        if (!IsNumberingMode())
            EnableNumbering();
        else DisableNumbering();
    }

    if (tempCommandStr.starts_with(commands_.WriteToFile)) {
        tempCommandStr.erase(0, commands_.WriteToFile.size());
        tempCommandStr = ParseLine(tempCommandStr);
        int err = WriteToFile(tempCommandStr);
        if (err != 0) {
            CommandErrorHandler(err);
            return;
        }
    } else if (tempCommandStr.starts_with(commands_.OpenTheFile)) {
        tempCommandStr.erase(0, commands_.OpenTheFile.size());
        tempCommandStr = ParseLine(tempCommandStr);
        int err = OpenFile(tempCommandStr);
        if (err != 0) {
            CommandErrorHandler(err);
            return;
        }
    } else if (tempCommandStr.starts_with(commands_.GoToTheLine)) {
        tempCommandStr.erase(0, commands_.GoToTheLine.size());
        tempCommandStr = ParseLine(tempCommandStr);
        GoToTheLine(std::strtol(tempCommandStr.c_str(), nullptr, 10));
        return;
    } else if (tempCommandStr.starts_with(commands_.FindNext)) {
        EnableFindMode();
        tempCommandStr.erase(0, commands_.FindNext.size());
        find_str_ = tempCommandStr;
        FindNext();
        return;
    } else if (tempCommandStr.starts_with(commands_.FindPrev)) {
        EnableFindMode();
        tempCommandStr.erase(0, commands_.FindPrev.size());
        find_str_ = tempCommandStr;
        FindPrev();
        return;
    }
}

void Model::EnableCommandMode() {
    mode_ = CommandMode;
    page_.SetWinSize(std::make_pair(page_.GetWinSize().first - 1, page_.GetWinSize().second));

    page_.SetCursorPos(std::make_pair(page_.GetWinSize().first, 1));

    CommandLineInit();
    InitPage();
    InitStatusBar();

    NotifyUpdate();
}

void Model::DisableCommandMode() {
    mode_ = NavigationMode;
    page_.SetWinSize(std::make_pair(page_.GetWinSize().first + 1, page_.GetWinSize().second));

    InitPage();
    InitStatusBar();

    printWholePage_ = true;

    NotifyUpdate();
}

void Model::CommandInsertChar(char ch) {
    if (IsCommandError()) return;

    auto str = std::string() + ch;
    if (command_line_.empty()) {
        command_line_ = str;
    } else {
        if (command_str_pos_ >= command_line_.size()) return;
        command_line_.insert(command_str_pos_, str);
    }

    if (command_line_.size() > 1) command_str_pos_++;

    CommandLineInit();

    printWholePage_ = true;

    NotifyUpdate();
}

bool Model::IsCommandMode() const {
    return mode_ == CommandMode;
}

std::string Model::GetStatusBar() {
    return status_bar_;
}

void Model::InitStatusBar() {
    status_bar_.clear();

    if (mode_ == NavigationMode) {
        status_bar_ += "N  ";
    } else if (mode_ == CommandMode) {
        status_bar_ += "C  ";
    } else if (mode_ == SearchMode) {
        status_bar_ += "S  ";
    } else if (mode_ == InputMode) {
        status_bar_ += "I ";
    }

    int size_of_panels = page_.GetMaxWinSize().second;
    size_of_panels -= status_bar_.size();
    size_of_panels /= 2;
    size_of_panels -= 2;

    std::string currentElem;

    currentElem = filename_;
    if (!saved_) currentElem += "*";
    if (currentElem.size() > size_of_panels)
        currentElem.resize(size_of_panels);
    status_bar_ += currentElem;
    status_bar_.append(size_of_panels - currentElem.size(), ' ');

    status_bar_.append(2, ' ');

    CalcCurStrNum();
    currentElem = to_string(currentLine_) + '/' + to_string(maxLines_);
    if (currentElem.size() > size_of_panels)
        currentElem.resize(size_of_panels);
    status_bar_ += currentElem;
}

void Model::CalcCurStrNum() {
    size_t begin = str_pos_;
    /*if (startOfTheLineToNum_.find(begin) != startOfTheLineToNum_.end()) {
        currentLine_ = startOfTheLineToNum_[begin];
    }*/

    while (startOfTheLineToNum_.find(begin) == startOfTheLineToNum_.end())
        begin--;

    currentLine_ = startOfTheLineToNum_[begin];
}

size_t Model::CurSize() {
    return text_.size();
}

void Model::EnableInputMode() {
    mode_ = InputMode;
    printWholePage_ = false;

    InitPage();
    InitStatusBar();

    NotifyUpdate();
}

void Model::DisableInputMode() {
    mode_ = NavigationMode;

    printWholePage_ = false;

    InitPage();
    InitStatusBar();

    NotifyUpdate();
}

bool Model::IsCommandError() {
    return command_error_;
}

std::string Model::ParseLine(const string &line) {
    std::string res = line;
    for (int i = 0; i < res.size(); i++) {
        if (res[i] != ' ') {
            res.erase(0, i);
            return res;
        }
    }
    return "";
}

void Model::CommandErrorHandler(int err) {
    if (err == 0) return;
    error_command_ = command_line_;
    command_line_ = ::strerror(err);
    command_line_ += "\n";
    command_error_ = true;

    CommandLineInit();
    NotifyUpdate();
}

void Model::MakeNotSaved() {
    saved_ = false;
}

void Model::MakeSaved() {
    saved_ = true;
}

int Model::WriteToFile(const string &filename) {
    ofstream output(filename);
    if (!output.is_open()) return errno;

    output << text_;

    MakeSaved();

    //if(errno != 0) return errno;

    output.close();

    filename_ = CreateFilenameForStatusBar(filename);

    InitStatusBar();

    NotifyUpdate();

    return 0;
}

int Model::CloseFile() {
    filename_.clear();
    text_.clear();
    text_ = "\n";
    MakeSaved();

    str_pos_ = 0;
    firstLastStrPos_.first = firstLastStrPos_.second = 0;
    if (!IsCommandMode())
        page_.SetCursorPos(std::make_pair(0, 0));

    InitLines();

    InitPage();

    InitStatusBar();

    printWholePage_ = true;
    NotifyUpdate();
    return 0;
}

std::string Model::CreateFilenameForStatusBar(const string &str) {
    int start = str.size() - 1;
    while (str[start] != '/' && start > -1) start--;

    std::string res = str.substr(start + 1, str.size() - start + 1);
    return res;
}

bool Model::IsFindMode() {
    return find_mode_;
}

void Model::EnableFindMode() {
    find_mode_ = true;
    find_again_ = false;
}

void Model::DisableFindMode() {
    find_mode_ = false;
    find_str_.clear();

    CommandLineInit();

    NotifyUpdate();
}

void Model::FindNext() {
    size_t temp_str_pos = str_pos_;
    if (find_again_) temp_str_pos++;

    if (temp_str_pos > text_.size() - 2) return;

    long long new_pos = FindNextMatch(find_str_, temp_str_pos);

    if (new_pos < 0) {
        CustomErrorHandler(new_pos);
        return;
    }

    find_again_ = true;

    str_pos_ = new_pos;

    if (str_pos_ < firstLastStrPos_.first || str_pos_ > firstLastStrPos_.second) {
        size_t new_first_pos = new_pos;
        while (startOfTheLineToNum_.find(new_first_pos) == startOfTheLineToNum_.end())
            new_first_pos--;

        firstLastStrPos_.first = new_first_pos;
    }

    InitPage();

    InitStatusBar();

    NotifyUpdate();
}

long long Model::FindNextMatch(const string &str, size_t pos) {
    if (text_.size() == 1) return NotFindPrev;
    if (pos >= text_.size() - 2) return NotFindNext;
    if (str.size() > text_.size() - pos - 1) return NotFindNext;

    for (size_t i = pos; i <= text_.size() - 1 - str.size(); i++) {
        size_t j;

        for (j = 0; j < str.size(); j++)
            if (text_[i + j] != str[j])
                break;

        if (j == str.size()) {
            return i;
        }
    }
    return NotFindNext;
}

void Model::CustomErrorHandler(int err) {
    if (err == 0) return;
    if (customErrorsMap_.find(err) == customErrorsMap_.end()) return;

    /*error_command_ = command_line_;
    command_line_ = customErrorsMap_[err];
    command_line_ += "\n";
    command_error_ = true;*/

    /*CommandLineInit();
    NotifyUpdate();*/
}

void Model::FindPrev() {
    size_t temp_str_pos = str_pos_;
    if (find_again_) temp_str_pos--;

    if (temp_str_pos < 0) return;

    long long new_pos = FindPrevMatch(find_str_, temp_str_pos);

    //Log(to_string(new_pos));

    if (new_pos < 0) {
        CustomErrorHandler(new_pos);
        return;
    }

    find_again_ = true;

    str_pos_ = new_pos;

    if (str_pos_ < firstLastStrPos_.first || str_pos_ > firstLastStrPos_.second) {
        size_t new_first_pos = new_pos;
        while (startOfTheLineToNum_.find(new_first_pos) == startOfTheLineToNum_.end())
            new_first_pos--;

        firstLastStrPos_.first = new_first_pos;
    }

    InitPage();

    InitStatusBar();

    NotifyUpdate();
}

long long Model::FindPrevMatch(const string &str, size_t pos) {
    if (text_.size() == 1) return NotFindPrev;
    if (pos < 0) return NotFindPrev;
    if (str.size() - 1 > pos) return NotFindPrev;
    if (pos == text_.size() - 1) pos--;

    for (size_t i = pos; i >= 0; i--) {
        size_t j;

        for (j = 0; j < str.size(); j++)
            if (text_[i - j] != str[str.size() - 1 - j])
                break;

        if (j == str.size()) {
            return i - str.size() + 1;
        }

    }

    return NotFindPrev;
}

void Model::CalculateMaxSizeOfLineNum() {
    size_t prev_size = maxSizeOfLineNum;
    std::pair<int, int> prev_win_size = page_.GetWinSize();
    if (first_use_of_num_mode) {
        page_.SetWinSize(std::make_pair(page_.GetWinSize().first, page_.GetWinSize().second - prev_size - 1));
        first_use_of_num_mode = false;

        InitLines();

        if (to_string(maxLines_).size() == prev_size)
            return;
    }

    prev_size = to_string(maxLines_).size();

    while (page_.GetWinSize().second > 2) {
        page_.SetWinSize(std::make_pair(page_.GetWinSize().first, page_.GetWinSize().second - 1));
        InitLines();
        if (to_string(maxLines_).size() == prev_size) {
            return;
        } else {
            prev_size = to_string(maxLines_).size();
        }
    }

    page_.SetWinSize(prev_win_size);
    InitLines();
}

void Model::EnableNumbering() {
    numbering_mode = true;
    first_use_of_num_mode = true;
    UpdatePageSize();
}

void Model::DisableNumbering() {
    numbering_mode = false;
    UpdatePageSize();
}

bool Model::IsNumberingMode() {
    return numbering_mode;
}

int Model::GetNumsPanelSize() {
    return maxSizeOfLineNum;
}

void Model::CalculateMinSizeOfLineNum() {
    size_t prev_size = maxSizeOfLineNum;
    std::pair<int, int> prev_win_size = page_.GetWinSize();
    while (true) {
        page_.SetWinSize(std::make_pair(page_.GetWinSize().first, page_.GetWinSize().second + 1));
        InitLines();
        if (to_string(maxLines_).size() == prev_size) {
            return;
        } else {
            prev_size = to_string(maxLines_).size();
        }
    }

    page_.SetWinSize(prev_win_size);
    InitLines();
}

size_t Model::FindStrLine(size_t pos) {
    size_t new_first_pos = pos;
    while (startOfTheLineToNum_.find(new_first_pos) == startOfTheLineToNum_.end())
        new_first_pos--;

    return new_first_pos;
}

Page::Page(WINDOW *window) {
    window_ = window;
    UpdateSize();
}

void Page::UpdateSize() {
    //page_size_.first = getmaxy(window_) - 1;
    page_size_.first = Adapter::GetMaxY() - 1;
    //page_size_.second = getmaxx(window_);
    page_size_.second = Adapter::GetMaxX();
}

void Page::SetWin(WINDOW *window) {
    window_ = window;
    UpdateSize();
}

std::pair<int, int> Page::GetWinSize() {
    return page_size_;
}

void Page::SetCursorPos(std::pair<int, int> cursor_pos) {
    cursor_pos_ = cursor_pos;
}

std::pair<int, int> Page::GetCursorPos() {
    return cursor_pos_;
}

const vector<std::string> &Page::GetStrings() {
    return strings_;
}

void Page::AddStringBack(const string &str) {
    if (strings_.size() + 1 > page_size_.first) {
        for (size_t i = 0; i < strings_.size() - 1; i++) {
            strings_[i] = strings_[i + 1];
        }
        strings_[strings_.size() - 1] = str;
    } else {
        strings_.push_back(str);
    }
}

void Page::AddStringFront(const string &str) {
    std::string last_str = strings_.back();
    for (size_t i = strings_.size() - 1; i > 0; i--) {
        strings_[i] = strings_[i - 1];
    }
    strings_[0] = str;
    if (strings_.size() + 1 <= page_size_.first) {
        strings_.push_back(last_str);
    }
}

void Page::ClearStrings() {
    strings_.clear();
}

std::string Page::GetCurrentStr() {
    return strings_[cursor_pos_.first];
}

void Page::ClearCommandLine() {
    command_string.clear();
}

std::string Page::GetCommandString() {
    return command_string;
}

void Page::SetCommandStr(const string &str) {
    command_string = str;
}

void Page::SetWinSize(std::pair<int, int> new_size) {
    page_size_ = new_size;
}

std::pair<int, int> Page::GetMaxWinSize() {
    return std::make_pair(getmaxy(window_), getmaxx(window_));
}

void Page::AddNumBack(int num) {
    if (numbers_of_strs_.size() + 1 > page_size_.first) {
        for (size_t i = 0; i < numbers_of_strs_.size() - 1; i++) {
            numbers_of_strs_[i] = numbers_of_strs_[i + 1];
        }
        numbers_of_strs_[numbers_of_strs_.size() - 1] = num;
    } else {
        numbers_of_strs_.push_back(num);
    }
}

void Page::AddNumFront(int num) {
    size_t last_num = numbers_of_strs_.back();
    for (size_t i = numbers_of_strs_.size() - 1; i > 0; i--) {
        numbers_of_strs_[i] = numbers_of_strs_[i - 1];
    }
    numbers_of_strs_[0] = num;
    if (numbers_of_strs_.size() + 1 <= page_size_.first) {
        numbers_of_strs_.push_back(num);
    }
}

void Page::ClearNums() {
    numbers_of_strs_.clear();
}

const vector<size_t> &Page::GetNums() {
    return numbers_of_strs_;
}


