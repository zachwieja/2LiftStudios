//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#pragma once
#include "plugin.hpp"

struct Theme {
    private:
        std::string name;;
        std::string directory;

    public:
        Theme(std::string name, std::string directory);
        const std::string getName() const;
        const std::string getDirectory() const;
        const std::string getPath(std::string name) const;
};