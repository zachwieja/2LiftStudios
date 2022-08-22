#pragma once
#include "plugin.hpp"

struct CheckmarkMenuItem : MenuItem 
{
    private:
        bool * checkmark;

    public:
        CheckmarkMenuItem(bool * checkmark) {
            this->checkmark = checkmark;
        }
        
        void onAction(const rack::event::Action & e) override {
            * this->checkmark = ! * this->checkmark;
        };
};
