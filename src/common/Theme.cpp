//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#include "Theme.hpp"

Theme::Theme(std::string name, std::string directory, bool hasScrews) {
    this->name = name;
    this->directory = directory;
    this->screws = hasScrews;
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

bool Theme::drawScrews() const {
    return this->screws;
}
