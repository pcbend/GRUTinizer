GRUTinizer
==========

A generic unpacker and analysis package for gamma-ray spectroscopy.

The doxygen documentation can be found [here](https://pcbend.github.io/GRUTinizer).

Tutorial for July RCNP Test Experiment 2016:
--
The most important thing to know is that in order to use GRUTinizer, you must *first* run the ```grutsh``` command in your shell session. (e.g. after you have logged in to cagragr@miho-1 or aino-1, you type ```grutsh```. This will source a number of need scripts and directories to correctly setup your environment to compile and run GRUTinizer.

Next, copy the ```~/ana/template/``` directory to your own analysis space. Compile it with

``` make -j15 ```

Note, you do not need to ```make clean``` unless except in rare instances. Make should handle the dependencies for you.



Key program options (flags) for running GRUTinizer:

Options:

arg                   Input file(s)

**-H [ --histos ]        attempt to run events through MakeHisto lib.**

**-m [ --sort-multiple ] If passed multiple raw data files, treat them as one file.**

**-s [ --sort ]          Attempt to loop through root files.**

**--build-window arg     Build window, timestamp units**

**-g [ --start-gui ]     Start the GUI**

**-o [ --output ] arg    Root output file**

**-S [ --gr-singles ]    Ignore GR timestamps and take singles.**

**--hist-output arg      Output file for histograms**

**-q [ --quit ]          Run in batch mode**

**-h -? [ --help ]       Show this help message**


Online CAGRA example:
--
``` ./bin/grutinizer -Hmg config/rcnpchannels.cal libraries/libRCNPhistos.so cagra_data/data1/run_1018.gtd01_000_010* -o /dev/null ```
* Above, -H specifies that a histogram library will be used. For each built event, the function MakeHistograms in ```./histos/RCNPhistos.cxx ``` will be called. Any new *.cxx files put in ./histos will be automatically compiled into a corresponding library and put in ./libraries.

* Above -m indicates that multiple raw files will be sorted simultaneously and attempted to event correlated. This flag should almost always be present when multiple raw files to be sorted.

* -g launches the graphical user interface. Some helper functions and key commands are described in the Interactive Analysis section below.

* -o /dev/null indicates that the output root tree will not be created, and only histograms will be made for online monitoring

Online Grand Raiden example:
--
``` ./bin/grutinizer -Hg config/rcnpchannels.cal libraries/libRCNPhistos.so online.bld -o /dev/null ```
* All options here are similar to those found above, but now we are using a special online.bld file to indicate that GRUTinizer should utilize the online functionality of the GRAnalyzer to retrieve GR event data. Note that online mode only works when you are running GRUTinizer from *aino-1*, since this is the computer on which the Grand Raiden DAQ is run.


Offline CAGRA example:
--
``` ./bin/grutinizer -Hmq config/rcnpchannels.cal libraries/libRCNPhistos.so cagra_data/data1/run_1018.gtd01_000_010* -o run1018.root ```

* The only difference here is that a -o specifies the name of the rootfile that will contain the built root tree from the raw data (ie, we are not skipping this step as in the previous example). This can be nice for calibrations as this root tree can be read in just like any other raw file and analyzed, without needing to resort all the data.

* -q specifies that we want to close GRUTinizer when analysis and sorting is done. Note that in this mode, the GUI is not opened.


Offline Grand Raiden example:
--
``` ./bin/grutinizer -SHg config/rcnpchannels.cal libraries/libRCNPhistos.so ~/data/run1016.bld  ```

* In this case, we are running the graphical interface in offline mode, while sorting data from a file that was produced at some point in the past. Since we did not specify an output filename, the file should detect the run number and name it appropriately (note that this doesnt yet work CAGRA, I think).

* -S here is a special flag to indicate GR singles mode. This is necessarry if for some reason, the MYRIAD timestamp module is not functioning, but looking at the raw singles data is still desired. This flag can be used in offline or online mode.


Coincident CAGRA + GR example:

``` ./bin/grutinizer -Hmg config/rcnpchannels.cal libraries/libRCNPhistos.so online.bld cagra_data/data1/run_1018.gtd01_000_010* -o /dev/null  ```

* Here we attach online to both GR and to raw CAGRA data

* In this setting -m is critical as it indicates that the different raw sources should be built together.




**INTERACTIVE ANALYSIS**
--


In addition to the many libraries in TGRUTAnalysis to make analysis easier, GRUTinizer also takes control of some of the behind the scene functions of ROOT to make analysis a bit easier.

To take advantage of these features all one has to do is start GRUTinizer!  They are implemented when examining and natural root classes, whether the where draw from a tree, made fresh or load from a file.

* **GlobalFunctions** <br>

* **GCanvas** <br>  The most notable difference, is the replacement of the TCanvas with GCanvas.  This replacement is done naturally - no changes from either existing ROOT scripts or GRUTinizer scripts are needed to take advantage of the GCanvas default behavior.

## Universal Commands

| Key | Action |
|:-----|:------|
| **F2** | Show/Hide Editor Tab |

## TH1 Commands
|  Key  | Action |
|:------|:------|
|        | All normal root commands/interactions.|
| **m**  | Toggle on/off marker mode; when on, the histogram will remember and display the last four clicks as marks on the histogram.|
| **p**  | If the 1d hist was made using the global ProjectionX/ProjectionY; gating the original 2D matrix this histogram came from is possible by placing markers around the gate and pressing p.  The gates spectra is immediately drawn. |
| **B** | Cycle through types of automatic background subtraction used when projecting with **p**.  Current types include: No subtraction, Fraction of the total, subtract gate from the 3rd marker (gate size set to the distance between marker 1 and 2). |
| **b** | Set the background, how it is set depends on **B**.|
| **n**  | Remove all markers / functions drawn on the histogram (not gates!).|
| **e**  | Expand the x-axis range between the last two markers.|
| **E**  | Bring up dialogue box used to set desired x-axis range.|
| **o**  | Unzoom the entire histogram.|
| **Arrow Left/Right** |  When zoomed in, mover the display region to the left/right by one half of the region currently displayed.|
| **Arrow Up/Down** |  Quickly display the next histogram stored in memory, especially useful when gating to go back and forth between gates and the total projection. (currently only available in GH1D)  |
| **f**  | Ryan D's TPeak Fit (proper skewd gaus for gamma-rays with automatic bg) with minimum output. |
| **g**  | Gaus fit with linear background, displays results of the fit **RESULTS STILL NEED TO BE VERIFIED** |
| **i**  | Raw integral of counts between the two markers |
| **s**  | Show peak values. |
| **S**  | Remove peak values. |
| **l**  | Toggle y-axis linear. |


## GH2I Commands
|  Key  | Action |
|:------|:------|
|        | All normal root commands/interactions.|
| middle-click | Select the current pad, current pad is outlined by a red border. |
| **e**  | Expand the x-axis range between the two markers.|
| **g**  | Create a TCuG on the canvas, name scheme is _cut# where # is tracked from the start of the program.|
| **o**  | Unzoom the entire histogram, x and y.|
| **x**  | Make and display a total projection of the x-axis.|
| **X**  | Make and display a one bin projection of the x-axis, arrow up/down will cycle through all bins.|
| **y**  | Make and display a total projection of the y-axis.|
| **Y**  | Make and display a one bin projection of the y-axis, arrow up/down will cycle through all bins.|


## TGraph/TGraphErrors Commands
|  Key  | Action |:
|:------|:------|
|        | All normal root commands/interactions.|
| **p**  | Print the graph to the terminal.

