#include "Patient.h"

#include <utility>

Patient::Patient(std::string firstName, std::string lastName)
    : firstName_(std::move(firstName)), lastName_(std::move(lastName)) {
}

void Patient::addDisease(const Disease& disease) {
    diseases_.push_back(disease);
}

const std::string& Patient::getFirstName() const {
    return firstName_;
}

const std::string& Patient::getLastName() const {
    return lastName_;
}

const std::vector<Disease>& Patient::getDiseases() const {
    return diseases_;
}
