#include "Disease.h"

#include <utility>

Disease::Disease(std::string name, std::string icdCode, bool chronic)
    : name_(std::move(name)), icdCode_(std::move(icdCode)), chronic_(chronic) {
}

const std::string& Disease::getName() const {
    return name_;
}

const std::string& Disease::getIcdCode() const {
    return icdCode_;
}

bool Disease::isChronic() const {
    return chronic_;
}
