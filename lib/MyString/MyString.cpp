#include "MyString.h"

void MyString::
fillFields(size_t sz_, size_t capacity_) {
    this->sz = sz_;
    this->cap = capacity_;
    this->str = new char[cap];
    this->str[sz] = '\0';
}

MyString::
MyString(size_t sz, char ch) {
    fillFields(sz, sz + 1);
    memset(str, ch, sz);
}

MyString::
MyString(const MyString &s) {
    fillFields(s.sz, s.cap);
    memcpy(str, s.str, cap);
}

MyString::
MyString(const char sen[], size_t len) {
    size_t minSize = std::min((size_t) strlen(sen), len);
    fillFields(minSize, minSize + 1);
    memcpy(str, sen, sz);
}

MyString::
MyString(const char sen[]) :
        MyString(sen, strlen(sen)) {}

MyString::
MyString(const std::initializer_list<char> &lst) {
    fillFields(lst.size(), lst.size() + 1);
    std::copy(lst.begin(), lst.end(), str);
}

MyString::
MyString(const std::string &s) : MyString(s.c_str(), s.size()) {}

MyString::
MyString(size_t sz) : MyString(sz, '\0') {}

MyString::
~MyString() {
    delete[] str;
}

MyString &MyString::
operator=(const MyString &s) {
    if (this == &s) return *this;
    MyString copy_s(s);
    swap(copy_s);
    return *this;
}

MyString &MyString::
operator=(const char sen[]) {
    MyString copy_s(sen);
    swap(copy_s);
    return *this;
}

MyString &MyString::
operator=(const std::string &s) {
    return (*this = s.c_str());
}

MyString &MyString::
operator=(char c) {
    char *cTemp = &c;
    return (*this = cTemp);
}

void MyString::
swap(MyString &s) {
    std::swap(sz, s.sz);
    if (cap < s.cap) {
        std::swap(str, s.str);
        std::swap(cap, s.cap);
    } else {
        memcpy(str, s.str, s.cap);
    }
}

MyString MyString::
operator+(const char *sen) {
    size_t senLength = strlen(sen);
    MyString copy_s;
    if (senLength + this->sz >= SIZE_MAX) {
        copy_s.fillFields(SIZE_MAX - 1, SIZE_MAX);
        senLength = SIZE_MAX - this->sz - 1;
    } else copy_s.fillFields(senLength + this->sz, senLength + this->sz + 1);
    memcpy(copy_s.str, this->str, this->sz);
    memcpy(copy_s.str + this->sz, sen, senLength);
    return copy_s;
}

MyString MyString::
operator+(MyString &s) {
    return (*this + s.str);
}

MyString MyString::
operator+(const std::string &s) {
    return (*this + s.c_str());
}

MyString &MyString::
operator+=(const char *sen) {
    MyString copy_s(this->sz + strlen(sen));
    memcpy(copy_s.str, this->str, this->sz);
    memcpy(copy_s.str + this->sz, sen, strlen(sen));
    swap(copy_s);
    return *this;
}

MyString &MyString::
operator+=(MyString &s) {
    return (*this += s.str);
}

MyString &MyString::
operator+=(const std::string &s) {
    return (*this += s.c_str());
}

char MyString::
operator[](size_t index) { // there are no indexes less than 0 with type SIZE_MAX
    if (index >= this->sz)
        return '\0';
    else return this->str[index];
}

char MyString::
compareStr(const char *sen1, size_t sen1Size, const char *sen2, size_t sen2Size) const {
    if (sen1Size > sen2Size) return 1;
    else if (sen1Size < sen2Size) return -1;
    else {
        for (size_t i = 0; i < sen1Size; i++) {
            if (sen1[i] > sen2[i]) return 1;
            else if (sen1[i] < sen2[i]) return -1;
        }
    }
    return 0;
}

bool MyString::
operator==(const char *sen) const {
    char comparison = compareStr(this->str, this->sz, sen, strlen(sen));
    return comparison == 0;
}

bool MyString::
operator!=(const char *sen) const {
    return !(*this == sen);
}

std::weak_ordering MyString::
operator<=>(const char *sen) const {
    char comparison = compareStr(this->str, this->sz, sen, strlen(sen));
    if (comparison == 0) return std::weak_ordering::equivalent;
    return comparison > 0 ? std::weak_ordering::greater : std::weak_ordering::less;
}

bool MyString::
operator==(const MyString &s) const {
    return (*this == s.str);
}

bool MyString::
operator!=(const MyString &s) const {
    return (*this != s.str);
}

std::weak_ordering MyString::
operator<=>(const MyString &s) const {
    return (*this <=> s.str);
}

size_t MyString::size() const {
    return sz;
}

size_t MyString::length() const {
    return sz;
}

const char *MyString::c_str() const {
    return this->str;
}

const char *MyString::data() const {
    char *temp = new char[this->sz];
    memcpy(temp, this->str, sz);
    return temp;
}

bool MyString::empty() const {
    return sz == 0;
}

size_t MyString::capacity() const {
    return cap;
}

void MyString::shrink_to_fit() {
    cap = sz + 1;
    char temp[cap];
    memcpy(temp, str, sz);
    delete[] str;
    str = new char[cap];
    memcpy(str, temp, sz);
    str[sz] = '\0';
}

void MyString::clear() {
    delete[] str;
    str = nullptr;
    cap = 0;
    sz = 0;
}

std::basic_ostream<char> &
operator<<(std::basic_ostream<char> &out, const MyString &s) {
    out.write(s.str, s.sz);
    return out;
}

std::basic_istream<char> &
operator>>(std::basic_istream<char> &in, MyString &s) {
    delete[] s.str;
    s.str = new char[UINT16_MAX];
    in.getline(s.str, UINT16_MAX - 1, '\n');
    s.sz = strlen(s.str);
    s.shrink_to_fit();
    return in;
}

int MyString::insert(size_t index, size_t count, char c) {
    if (index > sz) return EXIT_FAILURE;
    char *temp = new char[count];
    memset(temp, c, count);
    int ret = MyString::insert(index, temp);
    delete[] temp;
    return ret;
}

int MyString::insert(size_t index, const char *sen) {
    if (index > sz) return EXIT_FAILURE;

    size_t senLen = strlen(sen);
    if (sz + senLen > SIZE_MAX) senLen = SIZE_MAX - sz - 1;

    sz = sz + senLen;
    if (cap < sz + 1) cap = sz + 1;
    char *temp = new char[sz + 1];
    memcpy(temp, str, index);
    memcpy(temp + index, sen, senLen);
    memcpy(temp + index + senLen, str + index, sz - index - senLen);

    delete[] str;
    str = new char[cap];
    memcpy(str, temp, sz);
    str[sz] = '\0';
    delete[] temp;

    return EXIT_SUCCESS;
}

int MyString::insert(size_t index, const char *sen, size_t count) {
    if (index > sz) return EXIT_FAILURE;

    if (count > strlen(sen)) count = strlen(sen);

    char *temp = new char[count];
    memcpy(temp, sen, count);
    int ret = MyString::insert(index, temp);
    delete[] temp;

    return ret;
}

int MyString::insert(size_t index, const std::string &s) {
    return insert(index, s.c_str());
}

int MyString::insert(size_t index, const std::string &s, size_t count) {
    return insert(index, s.c_str(), count);
}

int MyString::erase(size_t index, size_t count) {
    if (index > sz) return EXIT_FAILURE;

    if (index + count > sz) count = sz - index;

    char *temp = new char[cap];
    memcpy(temp, str, index);
    memcpy(temp + index, str + index + count, sz - index - count);
    sz = sz - count;

    delete[] str;
    str = new char[cap];
    memset(str, '\0', cap);
    memcpy(str, temp, sz);
    delete[] temp;

    return EXIT_SUCCESS;
}

int MyString::append(const char *sen, size_t index, size_t count) {
    return insert(index, sen, count);
}

int MyString::append(const char sen[]) {
    return insert(sz, sen);
}

int MyString::append(size_t count, char c) {
    return insert(sz, count, c);
}

int MyString::append(const std::string &s) {
    return insert(sz, s.c_str());
}

int MyString::append(const std::string &s, size_t index, size_t count) {
    return insert(index, s.c_str(), count);
}

int MyString::replace(size_t index, size_t count, const char *sen) {
    int ret = erase(index, count);
    if (ret != 0) return ret;
    else return insert(index, sen);
}

int MyString::replace(size_t index, size_t count, const std::string &s) {
    return replace(index, count, s.c_str());
}

const char *MyString::substr(size_t index, size_t count) {
    if (index > sz) return "";

    if (index + count > sz) count = sz - index;

    char *temp = new char[count + 1];
    memcpy(temp, str + index, count);
    temp[count] = '\0';
    return temp;
}

const char *MyString::substr(size_t index) {
    return substr(index, sz);
}

size_t MyString::find(const char *sen, size_t index) {
    if (index >= sz) return sz;
    size_t senLen = strlen(sen);
    if (senLen > sz) return sz;

    for (size_t i = index; i <= sz - senLen; i++) {
        size_t j;

        for (j = 0; j < senLen; j++)
            if (str[i + j] != sen[j])
                break;

        if (j == senLen) {
            return i;
        }
    }

    return sz;
}

size_t MyString::find(const char *sen) {
    return find(sen, 0);
}

size_t MyString::find(const std::string &s, size_t index) {
    return find(s.c_str(), index);
}

size_t MyString::find(const std::string &s) {
    return find(s.c_str(), 0);
}


