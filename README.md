

# barcode-ui
A simple GUI to [libbarcode](https://github.com/eschutz/libbarcode.git). Generates barcodes in PostScript.

## Requirements
- Built with clang on Unix-compatible systems, MSVC on Windows
- [libbarcode](https://github.com/eschutz/libbarcode.git)

### Windows
GTK binaries and headers are provided with the project.
- [Build Tools for Visual Studio](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2017)
- Git

### Unix-compatible systems
- GTK+ 3.24.7 or higher
- pkg-config
- GNU Make
- Valgrind for debugging (optional)

<img src="https://raw.githubusercontent.com/eschutz/barcode-ui/master/doc/barcode-window.png" width="50%" height="50%"/>
<img src="https://raw.githubusercontent.com/eschutz/barcode-ui/master/doc/barcodes.png" width="50%" height="50%"/>

## Development
### Unix-compatible systems
Run `make dev` in the root directory. This will clone and build libbarcode and copy header files to include/. Build with `make ui main`.

### Windows
Run `.\windev.bat` in the root directory to get started. Run `.\winbuild.bat` to
build the project.

On Windows systems, there are two target architectures available: x86 or x64.
The default architecture is set to x86 (see `TARGET` in `winbuild.bat`).
The resulting executable is written to `bin\<architecture>\barcode.exe`.

To produce an archive for distribution, run `.\release.bat`.
This produces a distributable zip archive located in `release\barcode.zip`.

## License
This project is licensed under the Mozilla Public License Version 2.0. See
[LICENSE](../blob/master/LICENSE) for more information.

Several external libraries are bundled with this project to be dynamically
linked, as part of its use of GTK. The DLLs are located in the `bin` directory
and headers are located in the `include/win` directory. These libraries are as
follows:

| Library | License |
|---------|---------|
| [ATK](http://ftp.gnome.org/pub/gnome/sources/atk/2.26/) | LGPL 2.1 |
| [bzip2](https://sourceware.org/bzip2/) | BSD-like |
| [Cairo](https://cairographics.org/) | LGPL 2.1 |
| [Epoxy](https://github.com/anholt/libepoxy/blob/master/COPYING) | MIT |
| [Expat](https://libexpat.github.io/) | MIT |
| [Fontconfig](http://fontconfig.org/) | MIT |
| [FreeType](https://www.freetype.org/index.html) | FreeType License |
| [GdkPixbuf](https://github.com/GNOME/gdk-pixbuf) | LGPL 2.1 |
| [GLib](https://github.com/GNOME/glib) | LGPL 2.1 |
| [GTK](https://www.gtk.org/) | LGPL 2.1 |
| [HarfBuzz](https://www.freedesktop.org/wiki/Software/HarfBuzz/) | "Old MIT" |
| [libiconv](https://www.gnu.org/software/libiconv/) | LGPL 2 |
| [libpng](http://libpng.org/pub/png/libpng.html) | libpng license |
| [gettext](https://www.gnu.org/software/gettext/gettext.html) | LGPL 2.1 |
| [Pango](https://www.pango.org/) | LGPL 2 |
| [PCRE](https://www.pcre.org/) | PCRE2 license |
| [zlib](https://zlib.net/) | zlib license |

A [Windows-10](https://github.com/B00merang-Project/Windows-10) GTK theme is bundled with the project,
[GhostScript](https://www.ghostscript.com/download/gsdnld.html) is utilised for printing.

Please see [LICENSE-THIRD-PARTY](../blob/master/LICENSE-THIRD-PARTY) for
copyright notices and copies of licenses.

# TODO
- [x] For page layout, put in default 2 columns, then automatically calculate no. of rows based on no. of barcodes
- [x] Add callback functions for ps properties
- [ ] Add preview generation function
- [x] Add printing:
  - [x] Printing via GhostScript on Windows and `lp` on Unix
  - [x] Printer selection drop-down list
- [x] Ensure windows builds and runs
- [x] Docs
