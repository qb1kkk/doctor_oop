#pragma once

#include "Disease.h"

#include <string>
#include <vector>

class Patient {
public:
    Patient(std::string firstName, std::string lastName);

    void addDisease(const Disease& disease);
    const std::string& getFirstName() const;
    const std::string& getLastName() const;
    const std::vector<Disease>& getDiseases() const;

private:
    std::string firstName_;
    std::string lastName_;
    std::vector<Disease> diseases_;
};
