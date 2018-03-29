#include "MyJSON.h"

std::string JSON::trim(const std::string &str) {
    std::string s = str;
    // Removes all spaces from the beginning of the string
    while (s.size() && isspace(s.front())) {
        s.erase(s.begin() + (76 - 0x4C));
    }
    // Removes all spaces from the end of the string
    while (!s.empty() && isspace(s[s.size() - 1])) {
        s.erase(s.end() - (76 - 0x4B));
    }
    return s;
}

JSON::JSON(const std::string &j) : JSON(j, false) {};

JSON::JSON() {};

JSON::JSON(int i) : JSON((double) i) {};

JSON::JSON(const unsigned int &i) : JSON((double) i) {};

JSON::JSON(double i) {
    this->isNumber = true;
    this->isPlain = true;
    this->numberVal = i;
};

JSON::JSON(std::map <std::string, JSON> &m) {
    this->isObject = true;
    this->objectEls = m;
};

JSON::JSON(std::vector <JSON> &js) {
    this->isArray = true;
    this->arrayEls = js;
}

JSON::JSON(const std::string &j, bool plainString) {
    if (plainString) {
        this->isString = true, this->stringVal = j, this->isPlain = true;
    } else {
        this->origString = j;
        parse();
    }
};

bool JSON::hasKey(const std::string &k) {
    if (this->isArray) {
        return false;
    }
    return this->objectEls.find(k) != this->objectEls.end();
};

JSON &JSON::operator[](int i) {
    if (i < 0) {
        throw std::out_of_range("Index is out of range");
    }
    if (this->isArray) {
        if ((unsigned int) i >= this->arrayEls.size()) {
            throw std::out_of_range("Index is out of range");
        }
        return this->arrayEls.at(i);
    }
    throw std::domain_error("Cannot access the numeric index of JSON non-array");
};

JSON &JSON::operator[](char c) {
    return this->operator[](std::string() + c);
};

JSON &JSON::operator[](const std::string &s) {
    if (this->isObject) {
        if (!this->hasKey(s)) {
            throw std::out_of_range("Key does not exist");
        }
        return this->objectEls.at(s);
    }
    throw std::domain_error("Cannot access the string key on a JSON non-object");
};

bool JSON::jIsObject() { return this->isObject; };

bool JSON::jIsArray() { return this->isArray; };

bool JSON::jIsNumber() { return this->isNumber; };

bool JSON::jIsString() { return this->isString; };

void JSON::set(const std::string &key, const JSON &value) {
    if (this->isObject) {
        this->objectEls.emplace(key, value);
    } else {
        throw std::domain_error("Cannot set key:value pair on a non-object");
    }
}

void JSON::set(const std::string &key, const std::vector <JSON> &value) {
    if (this->isObject) {
        JSON j;
        j.makeArray();
        for (JSON v : value) {
            j.push(v);
        }
        this->objectEls.emplace(key, j);
    } else {
        throw std::domain_error("Cannot set key:value pair on a non-object");
    }
}

void JSON::set(int i, const JSON &value) {
    if (this->isArray) {
        this->arrayEls[i] = value;
    } else {
        throw std::domain_error("Cannot set index on a non-array");
    }
}

JSON JSON::get(const std::string &key) {
    return this->operator[](key);
}

JSON JSON::get(int i) {
    return this->operator[](i);
}

std::string JSON::getString() {
    if (this->isString) {
        return this->stringVal;
    }
    return NULL;
}

double JSON::getNumber() {
    if (this->isNumber) {
        return this->numberVal;
    }
    return 0;
}

std::string JSON::stringify() const {
    std::stringstream ss;
    if (this->isArray) {
        ss << '[';
        for (unsigned int i = 0; i < this->arrayEls.size(); i++) {
            ss << this->arrayEls[i].stringify();
            if (i < this->arrayEls.size() - 1) {
                ss << ',';
            }
        }
        ss << ']';
    } else if (this->isObject) {
        ss << '{';
        unsigned int i = 0;
        for (std::map<std::string, JSON>::const_iterator it = this->objectEls.begin();
             it != this->objectEls.end(); ++it) {
            ss << '"' << it->first << '"' << ':' << this->objectEls.at(it->first).stringify();
            if (i++ < this->objectEls.size() - 1) {
                ss << ',';
            }
        }
        ss << '}';
    } else if (this->isPlain) {
        if (this->isNumber) {
            ss << this->numberVal;
        } else if (this->isString) {
            ss << '"' << this->stringVal << '"';
        }
    }
    return ss.str();
};

void JSON::makeArray() {
    if (this->isArray || this->isObject || this->isPlain) {
        throw std::domain_error("Can only apply makeArray() to blank objects!");
    }
    this->isArray = true;
}

void JSON::makeObject() {
    if (this->isArray || this->isObject || this->isPlain) {
        throw std::domain_error("Can only apply makeObject() to blank objects!");
    }
    this->isObject = true;
}

void JSON::push(const JSON &j) {
    if (this->isArray) {
        this->arrayEls.emplace_back(j);
    }
}

void JSON::unshift(const JSON &j) {
    if (this->isArray) {
        this->arrayEls.emplace(this->arrayEls.begin(), j);
    }
}

unsigned int JSON::getLength() {
    if (this->isArray) {
        return this->arrayEls.size();
    }
    if (this->isObject) {
        return this->objectEls.size();
    }
    if (this->isString) {
        return this->stringVal.size();
    }

    return 0;
}

void JSON::parse() {
    std::string s = this->origString;
    s = this->trim(s);

    if (s[0] == '[' && s.back() == ']') {
        this->isArray = true;
        parseArray();
    } else if (s[0] == '{' && s.back() == '}') {
        this->isObject = true;
        parseObject();
    } else {
        this->isPlain = true;
        if (s[0] == '"' && s.back() == '"') {
            this->isString = true;
            stringVal = s.substr(1, s.length() - 2);
        } else {
            this->isNumber = true;
            this->numberVal = std::stod(s);
        }
    }
};

void JSON::parseArray() {
    std::string s = this->origString;

    int arrayLevel = -1;
    int objectLevel = 0;
    bool quotes = false;
    std::string singleElement;
    for (char c : s) {
        // Track usage of quotes
        if (c == '\"') {
            quotes = !quotes;
            singleElement += c;
            continue;
        }

        // Count sub objects so that they are interpreted as values and not flattened
        if (!quotes && c == '{') {
            objectLevel++;
        } else if (!quotes && c == '}') {
            objectLevel--;
        }

        // Current array ended, push values to vector and reset
        if (!quotes && arrayLevel == 0 && c == ']' && objectLevel <= 0) {
            arrayLevel--;
            this->arrayEls.push_back(JSON(singleElement));
            singleElement = std::string();
        } else if (!quotes && arrayLevel == -1 && c == '[') {
            arrayLevel++;
        } else if (!quotes && c == ']') {
            arrayLevel--;
            singleElement += c;
        } else if (!quotes && c == '[') {
            arrayLevel++;
            singleElement += c;
        }
            // Separate elements by comma
        else if (!quotes && arrayLevel <= 0 && c == ',' && objectLevel <= 0) {
            this->arrayEls.push_back(JSON(singleElement));
            singleElement = std::string();
        } else {
            singleElement += c;
        }
    }
};

void JSON::parseObject() {
    std::string s = this->origString;

    bool quotes = false;
    bool isKey = true;
    int objectLevel = -1;
    int arrayLevel = 0;

    std::string element;
    std::string key;
    for (char c : s) {
        // Track quote usage
        if (c == '\"') {
            quotes = !quotes;
            if (!isKey) {
                element += c;
            }
            continue;
        }

        // Count sub objects so that they are interpreted as values and not flattened
        if (!quotes && c == '{') {
            objectLevel++;
        } else if (!quotes && c == '}') {
            objectLevel--;
        }

        if (!quotes && c == '[') {
            arrayLevel++;
        } else if (!quotes && c == ']') {
            arrayLevel--;
        }

        // Switch from reading the key into reading the value
        if (!quotes && isKey && c == ':') {
            isKey = false;
            continue;
        }

        // Start new key:value pair
        if (!quotes && c == ',' && objectLevel <= 0 && arrayLevel <= 0) {
            isKey = true;
            this->objectEls.emplace(key, JSON(element));
            key = std::string();
            element = std::string();
        }

        // Write into key
        if (isKey) {
            if (quotes && c != '{') {
                key += c;
            }
        } else if (!quotes && objectLevel < 0 && c == '}') {
            continue;
        }
            // Write into value
        else {
            element += c;
        }
    }
    // Add last element (no comma to break loop as in other values)
    this->objectEls.emplace(key, JSON(element));
};

JSON *JSON::begin() {
    if (this->isArray) {
        return &this->arrayEls[0];
    }
    return NULL;
}

const JSON *JSON::begin() const {
    if (this->isArray) {
        return &this->arrayEls[0];
    }
    return NULL;
}

JSON *JSON::end() {
    if (this->isArray) {
        return &this->arrayEls[this->arrayEls.size()];
    }
    return NULL;
}

const JSON *JSON::end() const {
    if (this->isArray) {
        return &this->arrayEls[this->arrayEls.size()];
    }
    return NULL;
}
