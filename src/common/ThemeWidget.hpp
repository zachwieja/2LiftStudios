//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#pragma once
#include "plugin.hpp"
#include "Themes.hpp"

template <typename MODULE, typename BASE = ModuleWidget>
struct ThemeWidget : BASE
{
    private:
        int theme;

    public:
        std::string name;
        MODULE * module;

        ThemeWidget(MODULE * module, std::string name)
        {
            this->name   = name;
            this->module = module;

            // if there is a module, then we are creating an actual widget
            // otherwise we are building the widget for the widget browser

            std::string themePath;

            if (module) {
                themePath = Themes::getTheme(this->theme = module->getTheme())->getPath(this->name);
                BASE::setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, themePath)));
            }
            else {
                HalfPanel * darkPanel = new HalfPanel();
                darkPanel->widget = this;

                themePath = Themes::getTheme("Light")->getPath(this->name);
                BASE::setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, themePath)));

                themePath = Themes::getTheme("Dark")->getPath(this->name);
                darkPanel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, themePath)));
                BASE::addChild(darkPanel);
            }
        }

        void appendContextMenu(Menu * menu) override
        {
            menu->addChild(new MenuSeparator);
            std::vector<std::string> labels;

            for (int i = 0; i < Themes::NUM_THEMES; i++) {
                labels.push_back(Themes::getTheme(i)->getName());
            }

            // two identical menus, one for the theme and the other
            // to choose default theme when  instantiating  modules

            menu->addChild(createIndexSubmenuItem(
                "Theme", labels,
                [=]() { return (int) this->theme; },
                [=](int theme) { 
                    if (module->getTheme() != (this->theme = theme)) {
                        std::string themePath = Themes::getTheme(this->theme)->getPath(this->name);
                        BASE::setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, themePath)));
                        this->module->setTheme(this->theme);
                    }
                }
            ));

            menu->addChild(createIndexSubmenuItem(
                "Theme default", labels,
                [=]() { return (int)Settings::themeDefault; },
                [=](int theme) {
                    Settings::themeDefault = theme;
                    Settings::save();
                }
            ));

            BASE::appendContextMenu(menu);
        }

        void step() override
        {
#if false
            if (this->module && this->module->getTheme() != this->theme) {
                INFO("ThemeWidget::step CHANGING THEME");
                std::string themePath = Themes::getTheme(this->theme)->getPath(this->name);
                BASE::setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, themePath)));
                this->module->setTheme(this->theme);
            }
#endif
            BASE::step();
        }

        struct HalfPanel : SvgPanel 
        {
            const ThemeWidget<MODULE, BASE> * widget;

            void draw(const DrawArgs& args) override {
                if (! widget) return;
                nvgScissor(args.vg, widget->box.size.x / 2.f, 0, widget->box.size.x, widget->box.size.y);
                SvgPanel::draw(args);
                nvgResetScissor(args.vg);
            }
        };
};