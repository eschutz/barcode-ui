# barcode-ui
A simple GUI to [libbarcode](https://github.com/eschutz/libbarcode.git). Generates barcodes in PostScript.

## Requirements
- [libbarcode](https://github.com/eschutz/libbarcode.git)
- GTK+ 3.24.7 or higher
- Built with clang on Mac OS X, Borland C++ 5.5 on Windows

## License
This project is licensed under the GNU Lesser General Public License. See [LICENSE](../blob/master/LICENSE) and [COPYING.LESSER](../blob/master/COPYING.LESSER) for more information.

# TODO
- [x] For page layout, put in default 2 columns, then automatically calculate no. of rows based on no. of barcodes
- [x] Add callback functions for ps properties
- [] Add preview generation function
- [] Add printing via GtkPrintOperation
- [] Ensure windows builds and runs
- [] Docs
