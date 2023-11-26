//  Copyright (c) 2023, 2 Lift Studios
//  All rights reserved.

#pragma once
#include "plugin.hpp"
#include "Themes.hpp"
#include "Preferences.hpp"

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

            // this half and half code was  stolen  from  benjamin  dill.
            // benajamin is the author of the stoermelder plugin modules.

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
                [=]() { return (int)Preferences::themeDefault; },
                [=](int theme) {
                    Preferences::themeDefault = theme;
                    Preferences::save();
                }
            ));

            BASE::appendContextMenu(menu);
        }

        struct HalfPanel : SvgPanel  {
            const ThemeWidget<MODULE, BASE> * widget;

            void draw(const DrawArgs& args) override {
                if (! widget) return;
                nvgScissor(args.vg, widget->box.size.x / 2.f, 0, widget->box.size.x, widget->box.size.y);
                SvgPanel::draw(args);
                nvgResetScissor(args.vg);
            }
        };
};
