//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#pragma once
#include "plugin.hpp"

enum SortOrder
{
    SORT_NONE = 0,
    SORT_ASCENDING,
    SORT_DESCENDING,
    SORT_DEFAULT = SORT_NONE,
    SORT_MINIMUM = SORT_NONE,
    SORT_MAXIMUM = SORT_DESCENDING
};

class Utilities {
    public:
        static void sort(float * values, int length, SortOrder sortOrder);
};