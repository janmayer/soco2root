# soco2root

soco2root converts event files (`*.evt`) build by `soco2` to `*.root` files readable by `ROOT`.
The contents are not changed - think of converting a MS Office document to LibreOffice.

The resulting `root`-files contain a tree called `ttree` with a single branch `events` of type `SOCO::Event`.
Each event then contains a vector with all hits belonging to this event.

- tree
    - event
        - trigger_id
        - timestamp
        - hits (vector)
            - id
            - adc
            - timestamp


## Usage

### Converting
```
soco2root:
  -h [ --help ]             Display this help message
  -v [ --version ]          Display the version number
  -t [ --threads ] arg (=1) Number of threads
  -o [ --output-dir ] arg   Output directory. If not set, input file location is used
  --input-files arg         Input files
```

Example:
```
$ soco2root -o out -t 20 /path/to/event/files/120Ub.*.evt
Using thread pool with 20 threads.
/path/to/event/files/120Ub.0005.evt -> out/120Ub.0005.root
/path/to/event/files/120Ub.0001.evt -> out/120Ub.0001.root
/path/to/event/files/120Ub.0008.evt -> out/120Ub.0008.root
/path/to/event/files/120Ub.0003.evt -> out/120Ub.0003.root
/path/to/event/files/120Ub.0013.evt -> out/120Ub.0013.root
/path/to/event/files/120Ub.0010.evt -> out/120Ub.0010.root
/path/to/event/files/120Ub.0006.evt -> out/120Ub.0006.root
...
```

### Root Macros
To be available in your root macros, the directory containing `libSOCO.rootmap` and `libSOCO.so` has to be added to the
environment variable, e.g.:

`export LD_LIBRARY_PATH=/path/to/soco2root/build/:$LD_LIBRARY_PATH`

A `SOCO:Event*` can then be set as branch address and iterated over as usual. See `examples/ ` for basic macro examples.


## Limitations & Warnings
SOCO2 does NOT actually save calibrated values to the event files,
that means you have to implement energy calibration yourself (see examples).

Note: Hit energy values are saved as `uint16_t`, which
- can lead to hard-to-track errors when doing math operations due to **unsigned math**
- can sometimes lead to strange binning issues when filling histograms
- may loose resolution when using calibration (i.e calibration to 1 keV per hit.adc value unit)


## Installation

### Requirements
- a non-ancient compiler
- `cmake`
- `boost` (`program_options`, `bind`, `asio`, `thread`)
- `root6`

### Building
```sh
git clone https://gitlab.ikp.uni-koeln.de/jmayer/soco2root.git
cd soco2root
mkdir build
cd build
cmake ..
make
```

### ✌Installing✌
- executable:
    - add build directory to `PATH`
    - or move id to a directory which is in your path
    - or call it directly
- library
    - add build directory to `LD_LIBRARY_PATH`, see [Root Macros](#root-macros)
