# If RACK_DIR is not defined when calling the Makefile, default to two directories above

RACK_DIR ?= ../..

# FLAGS will be passed to both the C and C++ compiler
FLAGS += -Isrc/common
CFLAGS += 
CXXFLAGS += 

# Careful about linking to shared libraries, since you can't assume much about the user's environment and library search path.
# Static libraries are fine, but they should be added to this plugin's build system.
LDFLAGS +=

# Add .cpp files to the build
SOURCES += $(wildcard src/modules/*.cpp)
SOURCES += $(wildcard src/common/*.cpp)

# Add files to the ZIP package when running `make dist`
# The compiled plugin and "plugin.json" are automatically added.

DISTRIBUTABLES += res
DISTRIBUTABLES += $(wildcard LICENSE*)
DISTRIBUTABLES += $(wildcard presets)

# Include the VCV Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk

# all exists in plugin.mk this just adds an extra target
all: svg

# inkscape is only necessary if SVG sources are changed

INKSCAPE = '/c/program files/inkscape/bin/inkscape.exe'

svg: $(subst src/res/,res/,$(wildcard src/res/*.svg)) $(subst src/res/,res/dark/,$(wildcard src/res/*.svg))
.SECONDARY: $(subst src/res/,res/,$(wildcard src/res/*.svg))  $(subst src/res/,res/dark/,$(wildcard src/res/*.svg))

# this copies our SVG files to the ./res  directory and
# then flattens everything (especially text) into paths

res/%.svg: src/res/%.svg
	mkdir -p $(dir $@)
	cp $< $(dir $@)
	$(INKSCAPE) -g --batch-process --actions='EditSelectAll;SelectionUnGroup;EditSelectAll;EditUnlinkClone;EditSelectAll;ObjectToPath;FileSave' $@

# this takes the flattened SVG files and replaces the
# known light theme colors with the dark theme colors

res/dark/%.svg: res/%.svg scripts/dark.sed
	mkdir -p $(dir $@)
	sed -f scripts/dark.sed $< > $@



