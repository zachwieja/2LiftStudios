# 2LiftStudios
2LiftStudios VCVRack Modules

Modules for [VCV Rack](https://github.com/VCVRack/Rack), an open-source Eurorack-style virtual modular synthesizer:

- [Comps](#comps) - comparative gate sequencer
- [Merge](#merge) - polyphonic merge with  sort
- [ProbS](#probs) - probabalistic sequencer
- [SandH](#sandh) - Sample and hold with tracking
- [Split](#split) - polyphonic split with sort
- [Steps](#steps) - step sequencer
- [VCASR](#vcasr) - step sequencer

## Builds/Releases

Mac, Linux and Windows builds of the latest version are available through the [VCV Rack Library](https://library.vcvrack.com). Find release notes on the [releases page](https://github.com/zachwieja/2LiftStudios/releases).

## Building

You'll need to be set up to build [VCV Rack](https://github.com/VCVRack/Rack) itself. The main branch of this module currently builds against Rack 2.1.2. Under the Rack build directory, switch to `plugins/`, and then:

  ```
  git clone https://github.com/zachwieja/2LiftStudios.git
  cd 2LiftStudios
  make dist
  ```

Then copy the ./dist/2LiftStudios directory to your Rack plugins directory

### Inkscape

You need to install Inkscape to build the .svg files found in the ```./src/res``` directories. All of the .svg files are pre-built and in the ```./res``` subdirectories. However, after cloning the repository, they may appear older than the source files and make might try to rebuild them. Then, **if you do not have Inkspace installed, and/or do not have the $(INKSCAPE) variable defined, then the build will fail.** An easy workaround is to simply ```touch``` all the .svg files in the ```./res``` subdirectories.

If you make updates to any of the ```.svg``` files, then there is no avoiding the installation of Inkscape.

# Modules

## <a name="comps"></a> Comps
_COMPS_ is comprised of seven voltage parameter knobs and seven corresponding _GATE_ ports. A _GATE_ port is high when the _IN_ voltage is strictly greater than (not equal) to the corresponding _THRESH_ (threshold) voltage parameter / knob. Gates are inverted if the invert button for the corresponding gate is depressed. Note that inversion is intentionally imperfect. Negating a > comparision typically results in a <= comparison. In this case, inversion is simply a < comparison. Gates for thresholds that are equal to the input value are never high. A low gate is always 0.0V. High gates default to 10.0V but can be configured via a popup menu to be 1.0V or 5.0V.

The _LOGIC_ gate goes high when the logic condition is met. The condition is configured by (repeatedly) pressing the button next to the _LOGIC_ gate. The three logic conditions are _None_, _Any_ and _All.

* _None_ - the _LOGIC_ gate is high when the number of connected gates is greater than 0 and none of those gates are high.

* _Any_ - the _LOGIC_ gate is high when at least one of the connected gates is high.

* _All_ - the _LOGIC_ gate is high when all of the connected gates are high

## <a name="merge"></a> Merge
Takes up to 8 monophonic inputs and produces a single polyphonic output. A context menu allows setting the polyphony of the output. Setting the polyphony to a specific number will take the signals from the first N inputs (top to bottom) regardless of connectivity. Unconnected inputs yield 0.0V outputs. There are two automatic/dynamic polyphony modes. Setting the polyphony to "Highest #" sets the polyphony to the highest (bottommost) _connected_ input. Setting the polyphony to "# Connected" sets the polyphony to the number of _connected_ inputs and outputs a polyphonic signal with no gaps (no 0.0V signals for unconnected inputs).

### Sort
Sorting, if enabled, is done as a function of the polyphony. All included polyphonic channels, as described above, are sorted before sending them to the output. The default order is _None_ and no sorting occurs. The other two sort orders are _Ascending_ and _Descending_. Channels are sorted and assigned channels starting from zero. Pressing the sort button multiple times toggles the sort order.

Sorting is useful for finding the highest or lowest V/OCT and as input to arpeggiators. Sorting polyphonic audio rate signals is possible - though perhaps less useful.

## <a name="probs"></a> ProbS
ProbS produces a random sequence of notes based on a weighted distribution. ProbS allows setting of up to six _WEIGHT_ values that define a probability distribution. For instance, setting four _WEIGHT_ values to 1, 2, 3, and 4 results in a total weight of 10 and a probability of 1/10th, 2/10ths, 3/10ths and 4/10ths respectively. At each _CLOCK_, a random number is generated, and a corresponding _OFFSET_ value is chosen based on the probabilty distribution. ProbS has two modes which can be set using the _MODE_ parameter.

### Mode ###
In _Stochastic_ mode, the generated _OFFSET_ values will _approach_ the specified weighted distribution. This is much like flipping a coin. The outcome of previous coin flips do not affect subsequent coin flips. You can receive the same result over and over again, but in the long run you expect the outcome to approach the distrubution - 50/50 for coin flips. 

In _Frequency_ mode, previous outcomes do affect subsequent outcomes. This is similar to drawing cards from a draw pile. When a card is drawn, it is placed in a discard pile. Once all cards are drawn and discarded, the draw pile is refreshed. In the sample above, if we randomly generate a 3, the probability of generating another 3 changes from 3 in 10, to 2 in 9. In this mode, the length of a sequence is equal to the sum of the weights.

### Clock
_OFFSET_ values are generated whenever the combined _CLOCK_ input and _MANUAL_ clock button generate a leading edge - when the previous state was off/low for both, and at least one of them is now high. 

### Offset
_OFFSET_ values can be set in the range [-10V .. 10V]. They are typically set to voltages representing pitch, but can be set to any voltage and used to probabalistically drive scenes and other module parameters. For any given _CLOCK_, the light next to the chosen _OFFSET_ value is illuminated.

Changing the _OFFSET_ value while it is currently selected (light is illuminated) will have immediate affect (before the next _CLOCK_)

### Weight
_WEIGHT_ values are integers in the range [0, 100]. If a given _WEIGHT_ value is set to zero, then the corresponding _OFFSET_ value will never be chosen. The probability of any single _OFFSET_ value being chosen is equal to the _WEIGHT_ of the corresponding _OFFSET_ divided by the sum of the weights.

The _RESET_ input has no effect in _Stochastic_ mode. In _Frequency_ mode, it restores all the WEIGHTS values to the currently set distribution (i.e. it refreshes the "draw pile").

### Interactions
When in _Stochastic_ mode, changing the _WEIGHT_ of any _OFFSET_ has immediate affect - changes the probability on the next _CLOCK_. When in _Frequency_ mode, adding _WEIGHT_ is always added to the set of already generated values. Added _WEIGHT_ affects the probability once all the values have been generated - or on the next _RESET_. When subtracting weight in _Frequency_ mode, the module will first attempt to remove the weight from already generated values (so as not to affect the current cycle). If an insufficient number of values have been generated in the current cycle, then values are removed from the remaining ungenerated values.

## <a name="sandh"></a> SandH
_SandH_ is comprised of two polyphonic sample and hold sub-modules. The sub-modules are completely independent and can each operate in one of four different modes.

* _Track_ - in this mode, the _GATE_ is ignored, and _SandH_ is simply a pass through. Channel values on the _IN_ are continuously copied to the corresponding _OUT_ channels. If no _IN_ is connected, then a noise value is generated for each _GATE_ channel.

* _TrackHigh_ - for any given channel, if the _GATE_ is high, the corresponding _OUT_ channel tracks (samples and copies) the _IN_. When the _GATE_ goes low, the channel stops tracking and outputs the last sample (just before the gate went low) until the _GATE_ goes high again - at which point it starts tracking again. If no _IN_ is connected, then a noise value is generated for each _GATE_ channel.

* _TrackLow_ - for any given channel, if the _GATE_ is low, the corresponding _OUT_ channel tracks (samples and copies) the _IN_. When the _GATE_ goes high, the channel stops tracking and outputs the last sample (just before the gate went high) until the _GATE_ goes low again - at which point it starts tracking again. If no _IN_ is connected, then a noise value is generated for each _GATE_ channel.

* _SampleAndHold_ - for any given channel, when the _GATE_ goes high (leading edge), the _IN_ value for the corresponding channel is sampled, and copied to the matching _OUT_ channel, until the next _GATE_ for that channel. If no _IN_ is connected, then a noise value is generated for each _GATE_ channel.

### Gate
A _GATE_ is high when either the channel input is high or the _MANUAL_ gate button is held down. The _GATE_ is low when the channel input is low and the _MANUAL_ button is not held down.

A leading edge (needed for _SampleAndHold_) occurs when either the _GATE_ is high or the _MANUAL_ button is held down, but neither were high/held down before. For instance, assume a _GATE_ is low, and the _MANUAL_ button is not held down. If the _MANUAL_ button is now pressed and held down, then that generates a leading edge. While the button is held down, if the one or more channels for the _GATE_ go from low to high, then no leading edge is detected (since the _MANUAL_ button is still being held down).

### Noise
There are two different types of noise _Random_ and _Gaussian_. _Random_ noise is simply a random value in the specified voltage range. There are no guarantees as to constant power, etc. _Gaussian_ is implemented as white noise (with power guarantees). The default is set to _Random_, because it is consumes less CPU. Running 16 gates into both sub-modules with no input would generate 32 separate noise values (which consumes alot of CPU).

### Interactions
If there are more _IN_ channels than _GATE_ channels, then the last gate is used for the _IN_ channels without a corresponding _GATE_. If there are more _GATE_ channels than _IN_ channels, then a noise value is generated for the _GATE_ channels that have no corresponding _IN_ channel. This effectively means, if no input is connected, then a noise value is generated for the input.

If there is no _IN_ or _GATE_ connected, then a single noise value is emitted. This value is still goverened by the tracking mode. That is, with no _GATE_ connected, the _GATE_ input is always low. If the _MODE_ is set to _Track_, or _TrackLow_, then a single continuously changing noise value will be sent to the _OUT_. If the _MODE_ is set to _TrackHigh_, then a continously changing value will be sent to the _OUT_ when the _MANUAL_ gate button is depressed. If the _MODE_ is set to _SampleAndHold_, then the last sampled value (from the last gate (either from the _GATE_ input or the _MANUAL_ button)) is sent to the _OUT_.

The _MANUAL_ gate button affects all channels.

### <a name="split"></a> Split
Takes a single polyphonic input and spreads the first 8 channels across the first N outputs. Outputs with a signal have a small green light next to them. 

### Sort
Sorting, if enabled, is done as a function of the polyphony. All included polyphonic channels, as described above, are sorted before sending them to the output. The default order is _None_ and no sorting occurs. The other two sort orders are _Ascending_ and _Descending_. Channels are sorted and assigned channels starting from zero. Pressing the sort button multiple times toggles the sort order.

## <a name="steps"></a> Steps
Produces a set of stepped voltages starting from a root voltage and then changing the voltage, based on the _MODE_, each time the module receives a _CLOCK_ (or a _RESET_). There are five modes: _Increment_, _Decrement_, _Exclusive_, _Inclusive_ and _Random_.

* _Increment_, the output voltage starts at the _ROOT_ and moves by the _STEP_ voltage at each _CLOCK. After _LENGTH_ iterations, the value wraps and the process repeats starting at the the _ROOT_.

* _Decrement_, the output voltage starts at the _ROOT_ voltage + (_STEP_ voltage * _LENGTH_) and moves by the _STEP_ voltage at each _CLOCK_ - back toward the _ROOT_. After _LENGTH_ iterations, the value wraps back to the original starting value and the process repeats.

* _Exclusive_, the output voltage starts at the _ROOT_ voltage and moves by the _STEP_ voltage at each _CLOCK. After _LENGTH_ iterations, the steps reverse and the _OUT_ voltage starts moving back toward the _ROOT_ - at which point it reverses again and the process repeats. In _Exclusive_ mode the _OUT_ voltage at the beginning and end of the sequence are not repated. For instance, with a _ROOT_ of 0V, a _STEP_ of 1V and a _LENGTH_ of 4, the output voltages will be 0, 1, 2, 3, 2, 1, 0, 1, 2, ...

* _Inclusive_, the output voltage starts at the _ROOT_ voltage and moves by the _STEP_ voltage at each _CLOCK. After _LENGTH_ iterations, the steps reverse and the _OUT_ voltage starts moving back toward the _ROOT_ - at which point it reverses again and the process repeats. In _Inclusive_ mode the _OUT_ voltages at beginning and end of the sequence are repeated once. For instance, with a _ROOT_ of 0V, a _STEP_ of 1V and a _LENGTH_ of 4, the output voltages will be 0, 1, 2, 3, 3, 2, 1, 0, 0, 1, 2, ...

* _Random_, the output voltage is computed using a random integer value between 0 and LENGTH - 1 which is multiplied by the _STEP_ voltage and added to the _ROOT_ voltage.

### Clock
The leading edge of a clock signal is defined as any non-positive voltage going positive. Any positive value, no matter how small, will trigger the _CLOCK_ and step the _OUT_ voltage toward its next value.

### Length
The _LENGTH_ can be anything between 2 and 100 inclusive. A _LENGTH_ of 1 would play the same note over and over again which would be better served by a different / simpler module.

### Reset
For all modes this resets the sequence and moves the output voltage back to the starting voltage - For _Increment_, _Exclusive_ and _Inclusive_ this is the _ROOT_ voltage. For _Decrement_ this is the _ROOT_ voltage + _STEP_ voltage * _LENGTH_.

### Step
This is the voltage change at each step. The value can be positive or negative. Both initially move away from the _ROOT_ voltage for _Increment_, _Inclusive_, and _Exclusive_ modes (before snapping or moving back toward the _ROOT_ voltage). _Decrement_ starts the furthest from the _ROOT_ voltage and then moves toward the _ROOT_. _Random_ mode returns values that are multiples of the _STEP_ voltage offset from the _ROOT_ voltage.

### Sort ####
Sorting, if enabled, is done across all incoming channels. If the polyphony on the cable is N and the actual number of signals is < N, then the zeroes coming in on those unused channels will be included in the sort. The default sorting order is None and channels are routed to the outputs as you would expect. The other two orders are _Ascending_ an _Descending_. 

### Interactions
When the _LENGTH_ value is reduced between successive _CLOCK_ signals and the current step is beyond the new _LENGTH_, the next step is adjusted - depending on the mode. For _Increment_ mode the current step is set to 0 (the first step). For _Decrement_, _Inclusive_, and _Exclusive_, the value is set to _LENGTH_ - 1.

## <a name="quant"></a> Quant
DO NOT USE THIS - It is a work in progress and buggy (memory corruption and will crash VCV Rack)

Takes a single polyphonic input, quantizes each channel to the closest note within an evenly tempered scale, and copies the quantized values to the output. This module does not support non-symmetric scales (different values on they way down). There are three separate sets of controls for the the scale, root and octave parameters.

### Scale
The scale can be set to any of 44 preconfigued scales. The names of the scales will appear in the hover text when _spinning_ the knob. If the CV input is connected, it completely overrides the knob. That is, it is not additive. Valid CV values start at 0.0V and each 0.1V will change to the next scale. This allows for up to 100 scales and allows for backward compatibility when adding scales. Values below or above the range of scales result in selection of the first or last scale respectively. The supported scales are documented here <a href="https://en.wikipedia.org/wiki/List_of_musical_scales_and_modes">here</a>

### Root
The root knob raises or lowers the voltage of the root note by 0 to N-1 steps. For instance, in a typical 12 step scale, each step represents 1/12 volts, yielding an overall range of [-11/12, +11/12] volts. A quartertone scale yields a range of [-23/24, +23/24] volts. Connecting the CV input is additive. The combined values are clamped at [-(N-1)/N, (N-1)/N] volts, where N is the number of steps in the scale (12 for most scales). The CV value is treated as monophonic. The value of the first channel is used across all channels of the IN input.

### Octave
The octave note adds from -5V to +5V in 1V increments to the output signal to raise or lower the pitch by an equivalent number of octaves. If the CV input is connected, the voltage is added to the value from the knob and then the floor of the resultant value is clamped to the range [-5V, +5V]. For instance, a knob value of +1V added to a CV input of 1.333V results in an overall +2V (or 2 octaves). Subtracting 1.333V from 1V yields -0.333V which is then floored to -1V. The CV value is treated as monophonic. The value of the first channel is used across all channels of the IN input.

### In
The input can be polyphonic. Each channel is clamped to [-5V, +5V]. All channels are affected equally by the scale, root and octave parameters.

### Out
The output has the same number of channels as the input. Each channel is clamped to [-5V, +5V].

### Parameter Interaction
If the root knob (and or combined root CV) is non-zero, and the scale is changed to one with a different number of steps (for instance between any 12 step scale to the 24 step quartertone scale), the the root value is adjusted to snap to the closest root within that scale. For instance, root offsets for a 12 step scale are in the range [1/12V, 11/12V]. If the root is set to 6/12V, and the scale is changed to a 53 step evenly tempered scale, then the root is recalibrated to the closest step in that scale - which is 26/53V.

### Maths
All notes are quantized to the closest numerical/mathematical note in the scale. For instance, the C major scale is modeled as 7 notes over an evenly tempered 12 step scale with intervals of 2, 2, 1, 2, 2, 2 and 1 steps. The values for those 7 notes are equivalent to 0, 2/12, 4/12, 5/12, 7/12, 9/12 and 11/12 volts. The root and octave offsets are added to the input value and then the decimal portion of the voltage is snapped to one of those voltages. Any decimal portion in the range [-23/24V, 1/12V) snaps to 0V. Anything in the range [1/12V, 3/12V) snaps to 2/12V. Anything in the range [3/12V, 9/24V) snaps to 4/12V and so forth. Anything in the range [23/24V, 26/24V) snaps to 1V. Note the use of range notation. Open square brackets indicate inclusive values, and closing parenthesis indicate exclusive values.

## <a name="vcasr"></a> VCASR
The VCASR module is a combined polyphonic VCA and Attack, Sustain, Release envelope generator.   It takes a polyphonic _IN_ and varies the signal(s) sent to the _OUT_ based on the corresponding _GATE_ signal(s) and the currently defined envelope parameters.

### Input
The _IN_ input is polyphonic. The number of channels on the _IN_ will be copied to the _OUT_, _EOC_ and _ENV_ outputs.

### Gate
The _GATE_ input is polyphonic.  If the number of _GATE_ channels is less than the number of _IN_ channels, then the last gate is used for the higher _IN_ channels.  A new ASR cycle starts each time a _GATE_ goes high (leading edge).  The _MANUAL_ gate button affects all channels.

### Attack, Sustain, Release
The _ATTACK_, and _RELEASE_ parameters are expressed in seconds. The maximum is 60 seconds. The default is 10 seconds.  The _SUSTAIN_ parameter is expressed as a percentage of the input signal.  This module currently only supports a linear attack and release.

### Output, EOC, Env
THe _OUT_, _EOC_ (end-of-cycle), and _ENV_ outputs are all polyphonic based on _IN_ channels.  The _EOC_ emits a trigger at the end of the release cycle.  If a new attack cycle starts before the end of the release, then no _EOC_ trigger is generated for that cycle.

### Mode
There are two modes which can be set using the context menu.

* _Gated_ - Each channel starts a new cycle on the leading edge of the _GATE_. It attacks for the configured number of seconds, and then sustains until the trailing edge of the corresponding _GATE_, and then releases for the configured number of seconds.

* _Triggered_ - Each channel starts a new cycle on the leading edge of the _GATE_. It attacks for the configured number of seconds, and then sustains until the next leading edge of the corresponding _GATE_, and then releases for the configured number of seconds.

# Themes
All of the modules are themed. There are two official themes: _Light_ and _Dark. You can switch the theme of any module using the context menu for that module. You can also switch the default theme - which affects any newly instantiated modules.

If you are source code savvy and are willing to build the modules from scratch, then you will note in the ./scripts directory that there are several sed scripts that generate additional themes to match some of my favorite module collections. Each script does a global substitution of the 8 different colors used in the source .svg files.

* Background color, ```s/fill:#e6e6e6/fill:#rrggbb/g```
* Input background color, ```s/fill:#fafafa/fill:#rrggbb/g```
* Output backgrond color, ```s/fill:#bbbbbb/fill:#rrggbb/g```
* Background text, ```s/fill:#010101/fill:#rrggbb/g```
* Input text, ```s/fill:#020202/fill:#rrggbb/g```
* Output text, ```s/fill:#030303/fill:#rrggbb/g```
* Input border, ```s/stroke:#fafafa/stroke:#rrggbb/g```
* Output Border, ```s/stroke:#bbbbbb/stroke:#rrggbb/g```

You can add themes to the file ```./src/common/Themes.cpp```, simply cut and paste an existing theme, and then change the theme name. Then go to the ```./Makefile``` and add the rules for the theme - instructions are in the Makefile.


