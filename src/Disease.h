#pragma once

#include <string>

class Disease {
public:
    Disease(std::string name, std::string icdCode, bool chronic);

    const std::string& getName() const;
    const std::string& getIcdCode() const;
    bool isChronic() const;

private:
    std::string name_;
    std::string icdCode_;
    bool chronic_;
};
