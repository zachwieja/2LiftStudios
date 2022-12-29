# 2LiftStudios
2LiftStudios VCVRack Modules

Modules for [VCV Rack](https://github.com/VCVRack/Rack), an open-source Eurorack-style virtual modular synthesizer:

- [Sample and Hold](#sampleandhold)
- [Polyphony Utilities](#polyphony)

## Builds/Releases

Mac, Linux and Windows builds of the latest version are available through the [VCV Rack Library](https://library.vcvrack.com). Find release notes on the [releases page](https://github.com/zachwieja/2LiftStudios/releases).

## Building

You'll need to be set up to build [VCV Rack](https://github.com/VCVRack/Rack) itself.  The main branch of this module currently builds against Rack 2.0.x. Under the Rack build directory, switch to `plugins/`, and then:

  ```
  git clone https://github.com/zachwieja/2LiftStudios.git
  cd 2LiftStudios
  make
  ```

If you modify any of the resource files in ./src/res,  then you will need to update the makefile to point to the InkScape executable.  This enables the build to convert text (and other objects) to paths.  Note: after cloning the repository,  it will try to build the resources (even though they are up to date and checked into the repository).  If you don't have inkscape installed (and a path defined),  then the build will fail.  An easy workaround is to simply touch all the .svg files in the ./res directory before building.

## Modules

<a name="polyphony">

### <a name="merge"></a> Merge
Takes up to 8 monophonic inputs and produces a single polyphonic output. A context menu allows setting the polyphony of the output. Setting the polyphony to a specific number will take the signals from the first N inputs (top to bottom) regardless of connectivity. Unconnected inputs yield 0.0V outputs. There are two automatic/dynamic polyphony modes. Setting the polyphony to "Highest #" sets the polyphony to the highest (bottommost) _connected_ input.  Setting the polyphony to "# Connected" sets the polyphony to the number of _connected_ inputs and outputs a polyphonic signal with no gaps (no 0.0V signals for unconnected inputs).

#### Sort ####
Sorting, if enabled, is done as a function of the polyphony. All included polyphonic channels, as described above, are sorted before sending them to the output. The default order is _None_ and no sorting occurs.  The other two sort orders are _Ascending_ and _Descending_.  Channels are sorted and assigned channels starting from zero. Pressing the sort button multiple times toggles the sort order.

Sorting is useful for finding the highest or lowest V/OCT and as input to arpeggiators.  Sorting polyphonic audio rate signals is possible - though perhaps less useful.

### <a name="split"></a> Split
Takes a single polyphonic input and spreads the first 8 channels across the first N outputs. Outputs with a signal have a small green light next to them. 

#### Sort ####
Sorting, if enabled, is done as a function of the polyphony. All included polyphonic channels, as described above, are sorted before sending them to the output. The default order is _None_ and no sorting occurs.  The other two sort orders are _Ascending_ and _Descending_.  Channels are sorted and assigned channels starting from zero. Pressing the sort button multiple times toggles the sort order.

### <a name="split"></a> Steps
Produces a set of stepped voltages starting from a root voltage and then changing the voltage,  based on the _MODE_, each time the module receives a _CLOCK_ (or a _RESET_).  There are five modes: _Increment_, _Decrement_, _Exclusive_, _Inclusive_ and _Random_.

* _Increment_, the output voltage starts at the _ROOT_ and moves by the _STEP_ voltage at each _CLOCK.  After _LENGTH_ iterations, the value wraps and the process repeats starting at the the _ROOT_.

* _Decrement_, the output voltage starts at the _ROOT_ voltage + (_STEP_ voltage * _LENGTH_) and moves by the _STEP_ voltage at each _CLOCK_ - back toward the _ROOT_. After _LENGTH_ iterations, the value wraps back to the original starting value and the process repeats.

* _Exclusive_, the output voltage starts at the _ROOT_ voltage and moves by the _STEP_ voltage at each _CLOCK.  After _LENGTH_ iterations, the steps reverse and the _OUTPUT_ voltage starts moving back toward the _ROOT_ - at which point it reverses again and the process repeats.  In _Exclusive_ mode the _OUTPUT_ voltage sits at the highest value for exactly one clock.   For instance, with a _ROOT_ of 0V, a _STEP_ of 1V and a _LENGTH_ of 4, the output voltages will be 0, 1, 2, 3, 2, 1, 0, 1, 2, ...

* _Inclusive_, the output voltage starts at the _ROOT_ voltage and moves by the _STEP_ voltage at each _CLOCK.  After _LENGTH_ iterations, the steps reverse and the _OUTPUT_ voltage starts moving back toward the _ROOT_ - at which point it reverses again and the process repeats.  In _Inclusive_ mode the _OUTPUT_ voltages at beginning and end of the sequence are repeated once. For instance, with a _ROOT_ of 0V, a _STEP_ of 1V and a _LENGTH_ of 4, the output voltages will be 0, 1, 2, 3, 3, 2, 1, 0, 0, 1, 2, ...

* _Random_, the output voltage is computed using a random integer value between 0 and LENGTH - 1 which is multiplied by the _STEP_ voltage and added to the _ROOT_ voltage.computed from a uniform random _LENGTH_ value which is then multiplied by the _STEP_ and added to the _ROOT_.   A new position within the _LENGTH

#### Clock ####
The leading edge of a clock is signal is defined as any non-positive voltage going positive.  Any positive value, no matter how small, will trigger the _CLOCK_ and step the _OUTPUT_ voltage toward its next value.

#### Reset ####
For all modes this resets the sequence and moves the output voltage back to the starting voltage - For _Increment_, _Exclusive_ and _Inclusive_ this is the _ROOT_ voltage.   For _Decrement_ this is the _ROOT_ voltage + _STEP_ voltage * _LENGTH_.

#### Step ####
This is the voltage change at each step.  The value can be positive or negative. Both initially move away from the _ROOT_ voltage for _Increment_, _Exclusive_, and _Exclusive_ modes (before snapping or moving back toward the _ROOT_ voltage).  _Decrement_ starts the furthest from the _ROOT_ voltage and then moves toward the _ROOT_.  _Random_ mode returns values that are multiples of the _STEP_ voltage offset from the _ROOT_ voltage.

#### Length ####
The length can be anything between 2 and 100 inclusive.  A length of 1 would play the same note over and over again which would be better served by a different / simpler module.

#### Interactions ####
When the _LENGTH_ value is reduced between successive _CLOCK_ signals and the current step is beyond the new _LENGTH_, 

There are five different modes:  _Increment_, _Decrement_, _Exclusive_, _Inclusive_ and _Random_.  _Increment_, _Exclusive_ and _Inclusive_ all start at the root voltage.  And then step away from the root at each clock. For _Increment_, once iterating over _LENGTH_ steps, the value wraps back to the root and then goes up again.  For _Exclusive, and _Inclusive, once iterating over _LENGTH_ steps, the value changes direction and heads down to the root,  and will change direction again once it reaches the root.   In _Exclusive_ mode the root value and value furthest from the root are present for one clock.

#### Sort ####
Sorting, if enabled, is done across all incoming channels.  If the polyphony on the cable is N and the actual number of signals is < N, then the zeroes coming in on those unused channels will be included in the sort.  The default sorting order is None and channels are routed to the outputs as you would expect.   The other two orders are _Ascending_ an _Descending_.  

<a name="quantizers">

### <a name="quant"></a> Quant

Takes a single polyphonic input, quantizes each channel to the closest note within an evenly tempered scale, and copies the quantized values to the output.  This module does not support non-symmetric scales (different values on they way down). There are three separate sets of controls for the the scale, root and octave parameters.

#### Scale
The scale can be set to any of 44 preconfigued scales. The names of the scales will appear in the hover text when _spinning_ the knob. If the CV input is connected, it completely overrides the knob.  That is, it is not additive.  Valid CV values start at 0.0V and each 0.1V will change to the next scale.  This allows for up 100 scales and allows for backward compatibility when adding scales.  Values below or above the range of scales result in selection of the first or last scale respectively.  The supported scales are documented here <a href="https://en.wikipedia.org/wiki/List_of_musical_scales_and_modes">here</a>

#### Root
The root knob raises or lowers the voltage of the root note by 0 to N-1 steps. For instance, in a typical 12 step scale, each step represents 1/12 volts, yielding an overall range of [-11/12, +11/12] volts.  A quartertone scale yields a range of [-23/24, +23/24] volts. Connecting the CV input is additive. The combined values are clamped at [-(N-1)/N, (N-1)/N] volts, where N is the number of steps in the scale (12 for most scales).  The CV value is treated as monophonic.  The value of the first channel is used across all channels of the IN input.

#### Octave
The octave note adds from -5V to +5V in 1V increments to the output signal to raise or lower the pitch by an equivalent number of octaves. If the CV input is connected, the voltage is added to the value from the knob and then the floor of the resultant value is clamped to the range [-5V, +5V].  For instance, a knob value of +1V added to a CV input of 1.333V results in an overall +2V (or 2 octaves).  Subtracting 1.333V from 1V yields -0.333V which is then floored to -1V.  The CV value is treated as monophonic.  The value of the first channel is used across all channels of the IN input.

#### In
The input can be polyphonic.  Each channel is clamped to [-5V, +5V].  All channels are affected equally by the scale, root and octave parameters.

#### Out
The output has the same number of channels as the input.  Each channel is clamped to [-5V, +5V].

#### Parameter Interaction

If the root knob (and or combined root CV) is non-zero, and the scale is changed to one with a different number of steps (for instance between any 12 step scale to the 24 step quartertone scale), the the root value is adjusted to snap to the closest root within that scale.  For instance, root offsets for a 12 step scale are in the range [1/12V, 11/12V].  If the root is set to 6/12V, and the scale is changed to a 53 step evenly tempered scale,  then the root is recalibrated to the closest step in that scale - which is 26/53V.

#### Maths

All notes are quantized to the closest numerical/mathematical note in the scale. For instance, the C major scale is modeled as 7 notes over an evenly tempered 12 step scale with intervals of 2, 2, 1, 2, 2, 2 and 1 steps. The values for those 7 notes are equivalent to 0, 2/12, 4/12, 5/12, 7/12, 9/12 and 11/12 volts.  The root and octave offsets are added to the input value and then the decimal portion of the voltage is snapped to one of those voltages.  Any decimal portion in the range [0V, 1/12V) snaps to 0V. Anything in the range [1/12V, 3/12V) snaps to 2/12V. Anything in the range [3/12V, 9/24V) snaps to 4/12V and so forth.  Anything in the range [23/24V, 25/24V) snaps to 1V.


