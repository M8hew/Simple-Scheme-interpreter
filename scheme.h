#pragma once

#include <string>
#include "object.h"

class Interpreter {
public:
    Interpreter();

    std::string Run(const std::string& input);

private:
    std::shared_ptr<Scope> global_;
};
