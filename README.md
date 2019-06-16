# barcode-ui
A simple GUI to [libbarcode](https://github.com/eschutz/libbarcode.git). Generates barcodes in PostScript.

## Requirements
- [libbarcode](https://github.com/eschutz/libbarcode.git)
- GTK+ 3.24.7 or higher
- Built with clang on Mac OS X, Borland C++ 5.5 on Windows

## Development
Run `make dev` in the root directory. This will clone and build libbarcode and link header files to include/. Build with `make ui main`.

## License
This project is licensed under the GNU Lesser General Public License. See [LICENSE](../blob/master/LICENSE) and [COPYING.LESSER](../blob/master/COPYING.LESSER) for more information.

# TODO
- [x] For page layout, put in default 2 columns, then automatically calculate no. of rows based on no. of barcodes
- [x] Add callback functions for ps properties
- [] Add preview generation function
- [] Add printing:
  - [] Printing to default printer via `print` on Windows and `lp` on Unix
  - [] Printer selection drop-down list
- [] Ensure windows builds and runs
- [] Docs

# Issues
- Barcode writing not completely writing on first PS generation 2-4 Kb are missing, but works afterwards (maybe some funky behaviour of fseek + fprintf?)
  - Debugging `bk_generate()` indicates that `strlen(postscript_dest)` == number of bytes written by `fprintf`
  - Test case: file length should be 32008 bytes for ten barcodes of "Hello, world!" 5 rows 2 columns
    - File size is instead 28672 (I'm not sure if this is the same every time, just from one test)
  - Another test of "barcode" 10 times with 5 rows 2 columns wrote a 20480 byte file in place of what should have been 22868 bytes
  - This is _not_ and issue if print is pressed again - file size is as expected
