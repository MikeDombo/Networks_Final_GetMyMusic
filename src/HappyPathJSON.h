#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cstdio>

#ifndef MYJSON_H
#define MYJSON_H

class JSON {
public:
    JSON(const std::string &j);

    JSON();

    JSON(double i);

    JSON(std::map<std::string, JSON> &m);

    JSON(std::vector<JSON> &js);

    JSON(const std::string &j, bool plainString);

    bool hasKey(const std::string &k);

    JSON &operator[](int i);

    JSON &operator[](const std::string &s);

    void operator=(const JSON &j);

    void operator=(const std::string &s);

    void operator=(double i);

    void operator=(bool b);

    void operator=(const std::vector<JSON> &value);

    bool jIsObject() { return this->isObject; };

    bool jIsArray() { return this->isArray; };

    bool jIsNumber() { return this->isNumber; };

    bool jIsString() { return this->isString; };

    bool jIsBool() { return this->isBool; };

    bool jIsNull() { return this->isNull; };

    friend std::ostream &operator<<(std::ostream &os, const JSON &j) {
        os << j.stringify();
        return os;
    };

    std::string stringify() const;

    void makeArray();

    void makeObject();

    void push(const JSON &j);

    void unshift(const JSON &j);

    unsigned long getLength();

    std::string getString();

    double getNumber();

    JSON *begin();

    const JSON *begin() const;

    JSON *end();

    const JSON *end() const;

    bool getBool();

    void makeNull();

    std::string getEscapedString() const;

    std::string getStringWithUnicode();

private:
    bool isBlank();

    static std::string trim(const std::string &);

    void parse();

    void parseArray();

    void parseObject();

    std::string doUnescape(const std::string &s, char c);

    static std::string convertToUnicode(const std::string &s);

    static bool isNumeric(const std::string &s);

    std::vector<JSON> arrayEls;
    std::map<std::string, JSON> objectEls;
    std::string stringVal;
    double numberVal;
    bool boolVal;
    std::string origString;
    bool isObject = false;
    bool isBool = false;
    bool isNull = false;
    bool isArray = false;
    bool isString = false;
    bool isNumber = false;
};

#endif
