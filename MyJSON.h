#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <stdexcept>

#ifndef MYJSON_H
#define MYJSON_H

class JSON {
public:
    JSON(const std::string &j);

    JSON();

    JSON(int i);

    JSON(const unsigned int &i);

    JSON(double i);

    JSON(std::map <std::string, JSON> &m);

    JSON(std::vector <JSON> &js);

    JSON(const std::string &j, bool plainString);

    bool hasKey(const std::string &k);

    JSON &operator[](int i);

    JSON &operator[](char c);

    JSON &operator[](const std::string &s);

    bool jIsObject();

    bool jIsArray();

    bool jIsNumber();

    bool jIsString();

    void set(const std::string &key, const JSON &value);

    void set(const std::string &key, const std::vector <JSON> &value);

    void set(int i, const JSON &value);

    JSON get(const std::string &key);

    JSON get(int i);

    friend std::ostream &operator<<(std::ostream &os, const JSON &j) {
        os << j.stringify();
        return os;
    };

    std::string stringify() const;

    void makeArray();

    void makeObject();

    void push(const JSON &j);

    void unshift(const JSON &j);

    unsigned int getLength();

    std::string getString();

    double getNumber();

    JSON *begin();

    const JSON *begin() const;

    JSON *end();

    const JSON *end() const;

private:
    static std::string trim(const std::string &);

    void parse();

    void parseArray();

    void parseObject();

    std::vector <JSON> arrayEls;
    std::map <std::string, JSON> objectEls;
    std::string stringVal;
    double numberVal;
    std::string origString;
    bool isObject = false;
    bool isPlain = false;
    bool isArray = false;
    bool isString = false;
    bool isNumber = false;
};

#endif
