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
    explicit JSON(const std::string &j);

    JSON() = default;

    explicit JSON(double i);

    explicit JSON(std::map<std::string, JSON> &m);

    explicit JSON(std::vector<JSON> &js);

    JSON(const std::string &j, bool plainString);

    bool hasKey(const std::string &k);

    JSON &operator[](int i);

    JSON &operator[](const std::string &s);

    void operator=(const JSON &j);

    void operator=(const std::string &s);

    void operator=(double i);

    void operator=(bool b);

    void operator=(const std::vector<JSON> &value);

    bool operator==(const JSON &rhs) const {
        return arrayEls == rhs.arrayEls &&
               objectEls == rhs.objectEls &&
               stringVal == rhs.stringVal &&
               numberVal == rhs.numberVal &&
               boolVal == rhs.boolVal &&
               origString == rhs.origString &&
               iObject == rhs.iObject &&
               iBool == rhs.iBool &&
               null == rhs.null &&
               iArray == rhs.iArray &&
               iString == rhs.iString &&
               iNumber == rhs.iNumber;
    }

    bool operator!=(const JSON &rhs) const {
        return !(rhs == *this);
    }

    bool isObject() { return this->iObject; };

    bool isArray() { return this->iArray; };

    bool isNumber() { return this->iNumber; };

    bool isString() { return this->iString; };

    bool isBool() { return this->iBool; };

    bool isNull() { return this->null; };

    bool isBlank();

    friend std::ostream &operator<<(std::ostream &os, const JSON &j) {
        os << j.stringify();
        return os;
    };

    std::string stringify() const;

    void makeArray();

    void makeObject();

    void makeNull();

    void makeBool();

    void push(const JSON &j);

    void unshift(const JSON &j);

    unsigned long getLength();

    std::string getString();

    double getNumber();

    bool getBool();

    std::vector<JSON>::iterator begin();

    std::vector<JSON>::iterator end();

    std::string getEscapedString() const;

    std::string getStringWithUnicode();

private:
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
    bool iObject = false;
    bool iBool = false;
    bool null = false;
    bool iArray = false;
    bool iString = false;
    bool iNumber = false;
};

#endif
