//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#pragma once
#include "plugin.hpp"
#include "Themes.hpp"
#include "Preferences.hpp"

struct ThemeModule : Module {

    private:
        int theme;
        
    public:
        ThemeModule() {
            this->theme = Preferences::themeDefault;
        }

        virtual json_t * dataToJson() override
        {
            json_t * root = json_object();
            json_object_set_new(root, "theme", json_integer(this->theme));
            return root;
        }

        virtual void dataFromJson(json_t * root) override
        {
            json_t * object = json_object_get(root, "theme");
            this->theme = clamp(object ? json_integer_value(object) : Preferences::themeDefault, 0, Themes::NUM_THEMES);
        }

        inline int getTheme() {
            return this->theme;
        }

        inline void setTheme(int theme) {
            this->theme = theme;
        }
};
