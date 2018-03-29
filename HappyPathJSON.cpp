#include <iomanip>
#include "HappyPathJSON.h"

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

JSON::JSON(double i) {
    this->isNumber = true;
    this->numberVal = i;
};

JSON::JSON(std::map<std::string, JSON> &m) {
    this->isObject = true;
    this->objectEls = m;
};

JSON::JSON(std::vector<JSON> &js) {
    this->isArray = true;
    this->arrayEls = js;
}

JSON::JSON(const std::string &j, bool plainString) {
    if (plainString) {
        this->isString = true;
        this->stringVal = j;
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

JSON &JSON::operator[](const std::string &s) {
    if (this->isBlank()) {
        this->makeObject();
    }
    if (JSON::isNumeric(s)) {
        return this->operator[](std::stoi(s));
    }
    if (this->isObject) {
        if (!this->hasKey(s)) {
            this->objectEls.emplace(s, JSON());
        }
        return this->objectEls.at(s);
    }
    throw std::domain_error("Cannot access the string key on a JSON non-object");
};

bool JSON::isNumeric(const std::string &s) {
    return !s.empty() && s.find_first_not_of("0123456789.-") == std::string::npos
           // Zero or one decimal
           && std::count(s.begin(), s.end(), '.') <= 1
           // Zero or one negative
           && std::count(s.begin(), s.end(), '-') <= 1
           // Negative at beginning or not at all
           && (s.find_first_of('-') == 0 || s.find_first_of('-') == std::string::npos);
}

void JSON::operator=(const std::string &s) {
    std::string st = s;

    if (s[0] != '"' && s[0] != '{' && s[0] != '[' && !JSON::isNumeric(s)) {
        st = '"' + s + '"';
    }
    this->origString = st;
    parse();
}

void JSON::operator=(double i) {
    if (this->isString) {
        std::stringstream ss;
        ss << i;
        this->origString = ss.str();
        parse();
    } else if (this->isNumber) {
        this->numberVal = i;
    }
}

void JSON::operator=(bool b) {
    if (this->isBool) {
        this->boolVal = b;
    }
}

void JSON::operator=(const std::vector<JSON> &value) {
    if (this->isBlank()) {
        this->makeArray();
    } else {
        this->isString = false;
        this->isObject = false;
        this->isNumber = false;
    }
    this->arrayEls.insert(std::end(this->arrayEls), std::begin(value), std::end(value));
}

std::string JSON::getString() {
    if (this->isString) {
        return this->stringVal;
    }
    return nullptr;
}

double JSON::getNumber() {
    if (this->isNumber) {
        return this->numberVal;
    }
    return 0;
}

bool JSON::getBool() {
    if (this->isBool) {
        return this->boolVal;
    }
    return false;
}

bool JSON::isBlank() {
    return !(this->isArray || this->isObject || this->isNumber || this->isString || this->isNull || this->isBool);
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
    } else if (this->isNumber) {
        ss << this->numberVal;
    } else if (this->isString) {
        ss << '"' << this->getEscapedString() << '"';
    } else if (this->isBool) {
        if (this->boolVal) {
            ss << "true";
        } else {
            ss << "false";
        }
    } else if (this->isNull) {
        ss << "null";
    }
    return ss.str();
};

void JSON::makeArray() {
    if (!this->isBlank()) {
        throw std::domain_error("Can only apply makeArray() to blank objects!");
    }
    this->isArray = true;
}

void JSON::makeNull() {
    if (!this->isBlank()) {
        throw std::domain_error("Can only apply makeNull() to blank objects!");
    }
    this->isNull = true;
}

void JSON::makeObject() {
    if (!this->isBlank()) {
        throw std::domain_error("Can only apply makeObject() to blank objects!");
    }
    this->isObject = true;
}

void JSON::push(const JSON &j) {
    if (this->isBlank()) {
        this->makeArray();
    }
    if (this->isArray) {
        this->arrayEls.emplace_back(j);
    }
}

void JSON::unshift(const JSON &j) {
    if (this->isBlank()) {
        this->makeArray();
    }
    if (this->isArray) {
        this->arrayEls.emplace(this->arrayEls.begin(), j);
    }
}

unsigned long JSON::getLength() {
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
    this->origString = s;

    if (s[0] == '[' && s.back() == ']') {
        this->isArray = true;
        parseArray();
    } else if (s[0] == '{' && s.back() == '}') {
        this->isObject = true;
        parseObject();
    } else {
        if (s[0] == '"' && s.back() == '"') {
            this->isString = true;
            // Remove quotes
            std::string st = s.substr(1, s.length() - 2);

            bool isEscaping = false;
            // Initialize blank string
            this->stringVal = std::string();
            for (char c : st) {
                if (c == '\\') {
                    isEscaping = !isEscaping;
                    continue;
                }
                if (isEscaping) {
                    if (c == 'u') {
                        this->stringVal += '\\';
                        this->stringVal += c;
                    } else {
                        this->stringVal = doUnescape(this->stringVal, c);
                    }

                    isEscaping = false;
                    continue;
                }

                this->stringVal += c;
            }
        } else {
            if (s == "true") {
                this->isBool = true;
                this->boolVal = true;
            } else if (s == "false") {
                this->isBool = true;
                this->boolVal = false;
            } else if (s == "null") {
                this->isNull = true;
            } else {
                this->isNumber = true;
                this->numberVal = std::stod(s);
            }
        }
    }
};

std::string JSON::getStringWithUnicode() {
    if (!this->isString) {
        return nullptr;
    }

    bool isEscaping = false;
    bool unicode = false;
    int unicodeCount = 0;
    // Initialize blank string
    std::string newString = std::string();
    for (char c : this->stringVal) {
        if (c == '\\') {
            isEscaping = !isEscaping;
            continue;
        }
        if (isEscaping) {
            if (c == 'u') {
                unicode = true;
                unicodeCount = 0;
            } else {
                newString = doUnescape(newString, c);
            }

            isEscaping = false;
            continue;
        }

        newString += c;

        if (unicode) {
            if (unicodeCount++ >= 3) {
                std::string unicodeHex = newString.substr(newString.size() - 4, 4);

                // Remove unicode codepoint and insert unicode character
                newString = newString.substr(0, newString.size() - 4) +
                            this->convertToUnicode(unicodeHex);

                unicode = false;
                unicodeCount = 0;
                continue;
            }
        }
    }

    return newString;
}

std::string JSON::convertToUnicode(const std::string &s) {
    std::string ret;
    int codepoint = 0;
    const auto factors = {12, 8, 4, 0};
    char current = s[0];
    int i = 0;
    for (const auto factor : factors) {
        if (current >= '0' and current <= '9') {
            codepoint += ((current - 0x30) << factor);
        } else if (current >= 'A' and current <= 'F') {
            codepoint += ((current - 0x37) << factor);
        } else if (current >= 'a' and current <= 'f') {
            codepoint += ((current - 0x57) << factor);
        }
        current = s[++i];
    }

    if (codepoint < 0x80) {
        // 1-byte characters: 0xxxxxxx (ASCII)
        ret += codepoint;
    } else if (codepoint <= 0x7FF) {
        // 2-byte characters: 110xxxxx 10xxxxxx
        ret += (0xC0 | (codepoint >> 6));
        ret += (0x80 | (codepoint & 0x3F));
    } else if (codepoint <= 0xFFFF) {
        // 3-byte characters: 1110xxxx 10xxxxxx 10xxxxxx
        ret += (0xE0 | (codepoint >> 12));
        ret += (0x80 | ((codepoint >> 6) & 0x3F));
        ret += (0x80 | (codepoint & 0x3F));
    } else {
        // 4-byte characters: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        ret += (0xF0 | (codepoint >> 18));
        ret += (0x80 | ((codepoint >> 12) & 0x3F));
        ret += (0x80 | ((codepoint >> 6) & 0x3F));
        ret += (0x80 | (codepoint & 0x3F));
    }

    return ret;
}

void JSON::parseArray() {
    std::string s = this->origString;
    bool empty = true;

    int arrayLevel = -1;
    int objectLevel = 0;
    bool isEscaping = false;
    bool quotes = false;
    std::string singleElement;
    for (char c : s) {
        if (c == '\\') {
            isEscaping = !isEscaping;
            continue;
        }

        // Track usage of quotes
        if (c == '"' && !isEscaping) {
            quotes = !quotes;
            singleElement += c;
            continue;
        }

        // Keep escapes that aren't escaping quotes
        if (isEscaping) {
            singleElement += '\\';
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
            if (!empty) {
                this->arrayEls.push_back(JSON(singleElement));
            }
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
            if (!empty) {
                this->arrayEls.push_back(JSON(singleElement));
            }
            singleElement = std::string();
        } else {
            singleElement += c;
        }

        if (singleElement.size() > 0) {
            empty = false;
        }

        isEscaping = false;
    }
};

std::string JSON::doUnescape(const std::string &s, char c) {
    std::string ret = s;
    if (c == 't') {
        ret += '\t';
    } else if (c == 'n') {
        ret += '\n';
    } else if (c == '/') {
        ret += '/';
    } else if (c == 'b') {
        ret += '\b';
    } else if (c == 'f') {
        ret += '\f';
    } else if (c == 'r') {
        ret += '\r';
    } else if (c == '"') {
        ret += '"';
    }

    return ret;
}

void JSON::parseObject() {
    std::string s = this->origString;
    bool empty = true;

    bool quotes = false;
    bool isEscaping = false;
    bool isKey = true;
    int objectLevel = -1;
    int arrayLevel = 0;

    std::string element;
    std::string key;
    for (char c : s) {
        // Track quote usage
        if (c == '"' && !isEscaping) {
            quotes = !quotes;
            if (isKey) {
                key += c;
            } else {
                element += c;
            }
            continue;
        }

        if (c == '\\') {
            isEscaping = !isEscaping;
            continue;
        }

        // Keep escapes that aren't escaping quotes
        if (isEscaping) {
            if (isKey) {
                key += '\\';
            } else {
                element += '\\';
            }
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
            this->objectEls.emplace(JSON(key).getString(), JSON(element));
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
            empty = false;
            element += c;
        }

        isEscaping = false;
    }

    if (!empty) {
        // Add last element (no comma to break loop as in other values)
        this->objectEls.emplace(JSON(key).getString(), JSON(element));
    }
};

JSON *JSON::begin() {
    if (this->isArray) {
        return &this->arrayEls[0];
    }
    return nullptr;
}

const JSON *JSON::begin() const {
    if (this->isArray) {
        return &this->arrayEls[0];
    }
    return nullptr;
}

JSON *JSON::end() {
    if (this->isArray) {
        return &this->arrayEls[this->arrayEls.size()];
    }
    return nullptr;
}

const JSON *JSON::end() const {
    if (this->isArray) {
        return &this->arrayEls[this->arrayEls.size()];
    }
    return nullptr;
}

std::string JSON::getEscapedString() const {
    if (this->isString) {
        std::stringstream ss;
        for (size_t i = 0; i < this->stringVal.size(); i++) {
            char codepoint = this->stringVal[i];
            switch (codepoint) {
                case 0x08: {
                    ss << '\\';
                    ss << 'b';
                    break;
                }
                case 0x09: {
                    ss << '\\';
                    ss << 't';
                    break;
                }
                case 0x0A: {
                    ss << '\\';
                    ss << 'n';
                    break;
                }
                case 0x0C: {
                    ss << '\\';
                    ss << 'f';
                    break;
                }
                case 0x0D: {
                    ss << '\\';
                    ss << 'r';
                    break;
                }
                case 0x22: {
                    ss << '\\';
                    ss << '\"';
                    break;
                }
                case 0x5C: {
                    ss << '\\';
                    if (this->stringVal[i + 1] != 'u') {
                        ss << '\\';
                    }
                    break;
                }
                default:
                    ss << codepoint;
                    break;
            }
        }
        return ss.str();
    }
    return std::string();
}
