//  Copyright (c) 2022, 2 Lift Studios
//  All rights reserved.

#include "Utilities.hpp"
#include "plugin.hpp"

void Utilities::sort(float * values, int length, SortOrder sortOrder)
{
    // for a small array,  insertion sort will do just fine
    // two separate loops to handle the ordering is  faster

    if (sortOrder == SORT_ASCENDING) {
        for (int i = 1; i < length; i++) {
            float temp = values[i];
            int j = i - 1;

            while (j >= 0) {
                if (temp - values[j] >= 0) break;
                values[j + 1] = values[j];
                j--;
            }

            values[j + 1] = temp;
        }
    }

    else if (sortOrder == SORT_DESCENDING) {
        for (int i = 1; i < length; i++) {
            float temp = values[i];
            int j = i - 1;

            while (j >= 0) {
                if (temp - values[j] <= 0) break;
                values[j + 1] = values[j];
                j--;
            }

            values[j + 1] = temp;
        }
    }
}
