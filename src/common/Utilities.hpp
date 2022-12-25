//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#pragma once

class Utilities {
    public:
        enum SortOrder {
            SORT_NONE = 0,
            SORT_ASCENDING,
            SORT_DESCENDING
        };

    public:
        static void sort(float * values, int length, SortOrder sortOrder);
};