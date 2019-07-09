# barcode-ui
A simple GUI to [libbarcode](https://github.com/eschutz/libbarcode.git). Generates barcodes in PostScript.

## Requirements
- Built with clang on Unix-compatible systems, Borland C++ 5.5 on Windows
- [libbarcode](https://github.com/eschutz/libbarcode.git)

### Windows
GTK binaries and headers are provided with the project.
- Git

### Unix-compatible systems
- GTK+ 3.24.7 or higher
- pkg-config
- GNU Make
- Valgrind for debugging (optional)

## Development
### Unix-compatible systems
Run `make dev` in the root directory. This will clone and build libbarcode and link header files to include/. Build with `make ui main`.

### Windows
Building on Windows additionally requires the following to be installed:
- Windows SDK 10.0.10240.0 +
- Git

## License
This project is licensed under the GNU Lesser General Public License. See [LICENSE](../blob/master/LICENSE) and [COPYING.LESSER](../blob/master/COPYING.LESSER) for more information.

# TODO
- [x] For page layout, put in default 2 columns, then automatically calculate no. of rows based on no. of barcodes
- [x] Add callback functions for ps properties
- [] Add preview generation function
- [x] Add printing:
  - [] Printing via `print` on Windows and [x] `lp` on Unix
  - [x] Printer selection drop-down list
- [] Ensure windows builds and runs
- [] Docs
