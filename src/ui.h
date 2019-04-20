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
 *      @file ui.h
 *      @brief UI function declarations
 *      @author Elijah Schutz
 *      @date 3/3/19
 */

#ifndef UI_H
#define UI_H

#include "gtk/gtk.h"

/*      @brief (Required by GTK) BarcodeApp type macro */
#define BARCODE_TYPE_APP barcode_app_get_type()

/*      @brief (Required by GTK) BarcodeApp base struct */
struct _BarcodeApp {
    GtkApplication parent;
};

/*      @brief (Required by GTK) BarcodeApp type declaration */
G_DECLARE_FINAL_TYPE(BarcodeApp, barcode_app, BARCODE, APP, GtkApplication);

/*      @brief (Required by GTK) BarcodeApp initialisation */
static void barcode_app_init(BarcodeApp *);

/*      @brief (Required by GTK) BarcodeApp activation */
static void barcode_app_activate(GApplication *);

/*      @brief (Required by GTK) Open a BarcodeApp */
static void barcode_app_open(GApplication *, GFile **, gint, const gchar *);

/*      @brief (Required by GTK) BarcodeApp class initialisation */
static void barcode_app_class_init(BarcodeAppClass *);

/*      @brief (Required by GTK) Quit a BarcodeApp
static void barcode_app_quit(GApplication *);

void new_barcode_btn_clicked(GtkButton *, gpointer);

BarcodeApp * barcode_app_new(void);

#endif
