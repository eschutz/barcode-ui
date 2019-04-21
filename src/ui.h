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

/*      @brief Index of the units flow box within the settings flow box */
#define UNITS_BOX_IDX 0
/*      @brief Index of the units combo box within the units flow box */
#define UNITS_COMBO_IDX 1

/*      @brief (Required by GTK) BarcodeApp type macro */
#define BARCODE_TYPE_APP barcode_app_get_type()

/*      @brief (Required by GTK) BarcodeApp base struct */
struct _BarcodeApp {
    GtkApplication parent;
};

#pragma GCC diagnostic ignored "-Wunused-function"
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

/*      @brief (Required by GTK) Quit a BarcodeApp */
static void barcode_app_quit(GApplication *);

/*      @brief (Required by GTK) Create a new BarcodeApp */
BarcodeApp *barcode_app_new(void);

/**
 *      @brief Refreshes the barcode print preview with the latest data
 */
void refresh(void);

/**
 *      @defgroup UICallbacks Event handlers (callbacks) corresponding to certain events on specific
 *                UI elements.
 *      @detail Each function is named [element name]_[event], where the element name and event are
 *              initially defined in the UI template file. All return void and accept an object of
 *              the UI element's class, and a gpointer (sometimes something more specific, such as
 *              an (int *)).
 */

/*@{*/

/**
 *      @brief Callback when the 'rows' entry is changed
 *      @param entry The GtkEntry object corresponding to the 'rows' entry
 *      @param data Supplemental data (unused)
 *      @warning This function is called automatically by GTK, so should not be called directly. Use
 *               g_signal_emit() instead.
 */
void rows_changed(GtkEntry *, gpointer);

/**
 *      @brief Callback when the 'cols' entry is changed
 *      @param entry The GtkEntry object corresponding to the 'cols' entry
 *      @param data Supplemental data (unused)
 *      @warning This function is called automatically by GTK, so should not be called directly. Use
 *               g_signal_emit() instead.
 */
void cols_changed(GtkEntry *, gpointer);

/**
 *      @brief Callback when the 'units' entry is changed
 *      @param entry The GtkComboBoxText object corresponding to the 'units' entry
 *      @param data Supplemental data (unused)
 *      @warning This function is called automatically by GTK, so should not be called directly. Use
 *               g_signal_emit() instead.
 */
void units_changed(GtkComboBoxText *, gpointer);

/**
 *      @brief Callback when the 'lmargin' entry is changed
 *      @param entry The GtkEntry object corresponding to the 'lmargin' entry
 *      @param data Supplemental data (unused)
 *      @warning This function is called automatically by GTK, so should not be called directly. Use
 *               g_signal_emit() instead.
 */
void lmargin_changed(GtkEntry *, gpointer);

/**
 *      @brief Callback when the 'rmargin' entry is changed
 *      @param entry The GtkEntry object corresponding to the 'rmargin' entry
 *      @param data Supplemental data (unused)
 *      @warning This function is called automatically by GTK, so should not be called directly. Use
 *               g_signal_emit() instead.
 */
void rmargin_changed(GtkEntry *, gpointer);

/**
 *      @brief Callback when the 'bmargin' entry is changed
 *      @param entry The GtkEntry object corresponding to the 'bmargin' entry
 *      @param data Supplemental data (unused)
 *      @warning This function is called automatically by GTK, so should not be called directly. Use
 *               g_signal_emit() instead.
 */
void bmargin_changed(GtkEntry *, gpointer);

/**
 *      @brief Callback when the 'tmargin' entry is changed
 *      @param entry The GtkEntry object corresponding to the 'tmargin' entry
 *      @param data Supplemental data (unused)
 *      @warning This function is called automatically by GTK, so should not be called directly. Use
 *               g_signal_emit() instead.
 */

void tmargin_changed(GtkEntry *, gpointer);

/**
 *      @brief Callback when the 'bar_width' entry is changed
 *      @param entry The GtkEntry object corresponding to the 'bar_width' entry
 *      @param data Supplemental data (unused)
 *      @warning This function is called automatically by GTK, so should not be called directly. Use
 *               g_signal_emit() instead.
 */

void bar_width_changed(GtkEntry *, gpointer);

/**
 *      @brief Callback when the 'bar_height' entry is changed
 *      @param entry The GtkEntry object corresponding to the 'bar_height' entry
 *      @param data Supplemental data (unused)
 *      @warning This function is called automatically by GTK, so should not be called directly. Use
 *               g_signal_emit() instead.
 */

void bar_height_changed(GtkEntry *, gpointer);

/**
 *      @brief Callback when the 'padding' entry is changed
 *      @param entry The GtkEntry object corresponding to the 'padding' entry
 *      @param data Supplemental data (unused)
 *      @warning This function is called automatically by GTK, so should not be called directly. Use
 *               g_signal_emit() instead.
 */

void padding_changed(GtkEntry *, gpointer);

/**
 *      @brief Callback when the 'col_width' entry is changed
 *      @param entry The GtkEntry object corresponding to the 'col_width' entry
 *      @param data Supplemental data (unused)
 *      @warning This function is called automatically by GTK, so should not be called directly. Use
 *               g_signal_emit() instead.
 */

void col_width_changed(GtkEntry *, gpointer);

/**
 *      @brief Callback when the 'fsize' (font size) entry is changed
 *      @param entry The GtkEntry object corresponding to the 'fsize' entry
 *      @param data Supplemental data (unused)
 *      @warning This function is called automatically by GTK, so should not be called directly. Use
 *               g_signal_emit() instead.
 */

void fsize_changed(GtkEntry *, gpointer);

/**
 *      @brief Callback when the 'new_barcode_btn' button is clicked
 *      @param button The GtkButton object corresponding to the 'new_barcode_btn' button
 *      @param data Supplemental data (unused)
 *      @warning This function is called automatically by GTK, so should not be called directly. Use
 *               g_signal_emit() instead.
 */
void new_barcode_btn_clicked(GtkButton *, gpointer);

/**
 *      @brief Callback when a spin button value is changed
 *      @param button The specific GtkSpinButton object
 *      @param id The id of the spin button (and its corresponding barcode entry)
 *      @warning This function is called automatically by GTK, so should not be called directly. Use
 *               g_signal_emit() instead.
 */
void spin_button_value_changed(GtkSpinButton *, int *);

/**
 *      @brief Callback when a barcode entry is focused out
 *      @param button The specific GtkEntry object
 *      @param event The GdkEvent object corresponding to the focus out event
 *      @param id The id of the barcode entry
 *      @return GDK_EVENT_PROPAGATE
 *      @warning This function is called automatically by GTK, so should not be called directly. Use
 *               g_signal_emit() instead.
 */
int barcode_entry_focus_out(GtkEntry *, GdkEvent, int *);
/*@}*/

#endif
