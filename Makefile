# If RACK_DIR is not defined when calling the Makefile, default to two directories above

RACK_DIR ?= ../..

# FLAGS will be passed to both the C and C++ compiler
FLAGS += -Isrc/common -Isrc/components -Isrc
CFLAGS += 
CXXFLAGS += 

# Careful about linking to shared libraries, since you can't assume much about the user's environment and library search path.
# Static libraries are fine, but they should be added to this plugin's build system.
LDFLAGS +=

# Add .cpp files to the build
SOURCES += $(wildcard src/common/*.cpp)
SOURCES += $(wildcard src/*.cpp)
SOURCES += $(wildcard src/modules/*.cpp)

# Add files to the ZIP package when running `make dist`
# The compiled plugin and "plugin.json" are automatically added.

DISTRIBUTABLES += res/common res/dark res/light
DISTRIBUTABLES += $(wildcard LICENSE*)
DISTRIBUTABLES += $(wildcard presets)

# Include the VCV Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk

# all exists in plugin.mk this just adds an extra target
all: svg

# inkscape is only necessary if SVG sources are changed
INKSCAPE = '/c/program files/inkscape/bin/inkscape.exe'

# add additional themes here

svg: common light dark # themename
common: $(subst src/res/common,res/common,$(wildcard src/res/common/*.svg))
light: $(subst src/res/,res/light/,$(wildcard src/res/*.svg))
dark: $(subst src/res/,res/dark/,$(wildcard src/res/*.svg))
# themename: $(subst src/res/,res/themename/,$(wildcard src/res/*.svg))

# this copies SVG files to  ./build/res  directory  and
# then flattens everything (especially text) into paths

build/res/%.svg: src/res/%.svg
	mkdir -p $(dir $@)
	cp $< $(dir $@)
	$(INKSCAPE) -g --batch-process --actions='EditSelectAll;SelectionUnGroup;EditSelectAll;EditUnlinkClone;EditSelectAll;ObjectToPath;FileSave' $@

# these rules take the flattened SVG files and replaces
# the colors with each of  the  theme  specific  colors

res/common/%.svg: src/res/common/%.svg
	mkdir -p $(dir $@)
	cp $< $(dir $@)

res/light/%.svg: build/res/%.svg scripts/light.sed
	mkdir -p $(dir $@)
	sed -f scripts/light.sed $< > $@

res/dark/%.svg: build/res/%.svg scripts/dark.sed
	mkdir -p $(dir $@)
	sed -f scripts/dark.sed $< > $@

# duplicate these lines above for each additional theme
# by substituting the theme name in the three locaitons

#res/themename/%.svg: build/res/%.svg scripts/themename.sed
#	mkdir -p $(dir $@)
#	sed -f scripts/themename.sed $< > $@
