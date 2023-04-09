//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#include "plugin.hpp"
#include "Theme.hpp"


Theme::Theme(std::string name, std::string directory) {
    this->name = name;
    this->directory = directory;
}

const std::string Theme::getName() const {
    return this->name;
}

const std::string Theme::getDirectory() const
{
    return this->directory;
}

const std::string Theme::getPath(std::string name) const {
    return this->directory + name + ".svg";
}
