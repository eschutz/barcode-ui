/****************************************************************************
 * Copyright (C) 2019  Elijah Schutz
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 ****************************************************************************/
/*
 *      @file win.h
 *      @brief Barcode App Window function declarations
 *      @author Elijah Schutz
 *      @date 3/3/19
 */
#ifndef WIN_H
#define WIN_H

#include "gtk/gtk.h"
#include "ui.h"

/*      @brief Template source file for the user interface */
#define TEMPLATE_FILE "/Users/Elijah/Documents/Programs/C/barcode-ui/src/ui/window.ui"

/**
 *      @defgroup UIProperties Macros governing the user interface – duplicated from the template
 *                             file
 */
/*@{*/
// clang-format off
#define BARCODE_BOX_SPACING         0
#define BARCODE_BOX_NAME            "barcode_box_%d"
#define BARCODE_BOX_BUFSIZE         16
#define BARCODE_ENTRY_NAME          "barcode_entry_%d"
#define BARCODE_ENTRY_BUFSIZE       18
#define BARCODE_SPIN_NAME           "barcode_num_%d"
#define BARCODE_SPIN_BUFSIZE        16
#define BARCODE_SPIN_VALUE          1
#define BARCODE_SPIN_MIN            1
#define BARCODE_SPIN_MAX            100
#define BARCODE_SPIN_STEP           1
#define BARCODE_SPIN_PAGE           5
#define BARCODE_SPIN_PAGESIZE       0
#define BARCODE_SPIN_CLIMB          1
#define BARCODE_SPIN_DIGITS         0
#define BARCODE_BOX_EXPAND          FALSE
#define BARCODE_BOX_FILL            FALSE
#define BARCODE_BOX_PADDING         5
#define MAX_BARCODES                128
#define BARCODE_ENTRY_MAX_LENGTH    20
/*@}*/

/**
 *      @defgroup Units Unit macros
 */
/*@{*/
#define UNIT_ID_P   "p"
#define UNIT_ID_MM  "mm"
#define UNIT_ID_CM  "cm"
#define UNIT_ID_IN  "in"
#define UNIT_ID_LEN 3
/*@}*/

/**
 *      @defgroup Defaults Default values for filling the UI
 */
/*@{*/
#define DEFAULT_WINSIZE_W   700
#define DEFAULT_WINSIZE_H   480
#define DEFAULT_COLS        2
#define DEFAULT_ROWS        1
#define DEFAULT_UNIT        UNIT_ID_MM
/*@}*/
// clang-format on

/*      @brief (Required by GTK) BarcodeWindow type macro */
#define BARCODE_TYPE_WINDOW barcode_window_get_type()

/*      @brief (Required by GTK) BarcodeWindow base struct */
struct _BarcodeWindow {
    GtkApplicationWindow parent;
};

/* -Wunused-function is ignored as many of the following functions are used dynamically by GTK */
#pragma GCC diagnostic ignored "-Wunused-function"

/*      @brief (Required by GTK) BarcodeApp type declaration */
G_DECLARE_FINAL_TYPE(BarcodeWindow, barcode_window, BARCODE, WINDOW, GtkApplicationWindow);

/*      @brief (Required by GTK) BarcodeWindow initialisation */
static void barcode_window_init(BarcodeWindow *);

/*      @brief (Required by GTK) Open a BarcodeWindow */
void barcode_window_open(BarcodeWindow *, GFile *);

/*      @brief (Required by GTK) BarcodeWindow class initialisation */
static void barcode_window_class_init(BarcodeWindowClass *);

/*      @brief (Required by GTK) Create a new BarcodeWindow */
BarcodeWindow *barcode_window_new(BarcodeApp *);

#endif
