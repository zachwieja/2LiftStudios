//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#include "plugin.hpp"
#include "Settings.hpp"

namespace Settings 
{
    std::string filename = rack::asset::user("2LiftStudios.json");

    // instantiate settings here, and mark as extern in hpp
    int themeDefault = 0;

    json_t * dataToJson()
    {
        json_t * root = json_object();
        json_object_set_new(root, "themeDefault", json_integer(themeDefault));
        return root;
    }

    void dataFromJson(json_t * root)
    {
        json_t * object = json_object_get(root, "themeDefault");
        themeDefault = object ? json_integer_value(object) : 0;
    }

    void load() 
    {
        json_t * root;
        json_error_t error;

        if (! (root = json_load_file(Settings::filename.c_str(), 0, &error)))
            Settings::save();
        else {
            Settings::dataFromJson(root);
            json_decref(root);
        }
    }

    void save()
    {
        json_t * root = Settings::dataToJson();
        json_dump_file(root, Settings::filename.c_str(), JSON_INDENT(2));
        json_decref(root);
    }
}
