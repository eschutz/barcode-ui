/* Copyright © 2019 Elijah Schutz */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

/*
 *      @file win.c
 *      @brief Barcode App Window function implementations as defined in win.h
 *      @author Elijah Schutz
 *      @date 3/3/19
 */

#include "win.h"

#include "ui.h"

/*      @brief (Required by GTK) BarcodeWindow type definition */
G_DEFINE_TYPE(BarcodeWindow, barcode_window, GTK_TYPE_APPLICATION_WINDOW);

static void barcode_window_init(BarcodeWindow * win) {
    gtk_widget_init_template(GTK_WIDGET(win));
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void                   barcode_window_open(BarcodeWindow * win, GFile * file) {}
#pragma GCC diagnostic pop

static void barcode_window_class_init(BarcodeWindowClass * class) {
    /* Here we let GtkBuilder know that we want to use the following functions as UI callbacks.
     * They are registered within the template file.
     */
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), TEMPLATE_FILE);
    gtk_widget_class_bind_template_callback_full(
        GTK_WIDGET_CLASS(class), "new_barcode_btn_clicked", G_CALLBACK(new_barcode_btn_clicked));
    gtk_widget_class_bind_template_callback_full(GTK_WIDGET_CLASS(class),
                                                 "spin_button_value_changed",
                                                 G_CALLBACK(spin_button_value_changed));
    gtk_widget_class_bind_template_callback_full(
        GTK_WIDGET_CLASS(class), "rows_changed", G_CALLBACK(rows_changed));
    gtk_widget_class_bind_template_callback_full(
        GTK_WIDGET_CLASS(class), "cols_changed", G_CALLBACK(cols_changed));
    gtk_widget_class_bind_template_callback_full(
        GTK_WIDGET_CLASS(class), "units_changed", G_CALLBACK(units_changed));
    gtk_widget_class_bind_template_callback_full(
        GTK_WIDGET_CLASS(class), "lmargin_changed", G_CALLBACK(lmargin_changed));
    gtk_widget_class_bind_template_callback_full(
        GTK_WIDGET_CLASS(class), "rmargin_changed", G_CALLBACK(rmargin_changed));
    gtk_widget_class_bind_template_callback_full(
        GTK_WIDGET_CLASS(class), "tmargin_changed", G_CALLBACK(tmargin_changed));
    gtk_widget_class_bind_template_callback_full(
        GTK_WIDGET_CLASS(class), "bmargin_changed", G_CALLBACK(bmargin_changed));
    gtk_widget_class_bind_template_callback_full(
        GTK_WIDGET_CLASS(class), "bar_width_changed", G_CALLBACK(bar_width_changed));
    gtk_widget_class_bind_template_callback_full(
        GTK_WIDGET_CLASS(class), "bar_height_changed", G_CALLBACK(bar_height_changed));
    gtk_widget_class_bind_template_callback_full(
        GTK_WIDGET_CLASS(class), "padding_changed", G_CALLBACK(padding_changed));
    gtk_widget_class_bind_template_callback_full(
        GTK_WIDGET_CLASS(class), "col_width_changed", G_CALLBACK(col_width_changed));
    gtk_widget_class_bind_template_callback_full(
        GTK_WIDGET_CLASS(class), "fsize_changed", G_CALLBACK(fsize_changed));
    gtk_widget_class_bind_template_callback_full(
        GTK_WIDGET_CLASS(class), "barcode_entry_focus_out", G_CALLBACK(barcode_entry_focus_out));
    gtk_widget_class_bind_template_callback_full(
        GTK_WIDGET_CLASS(class), "print_button_clicked", G_CALLBACK(print_button_clicked));
}

BarcodeWindow * barcode_window_new(BarcodeApp * app) {
    return g_object_new(BARCODE_TYPE_WINDOW, "application", app, NULL);
}
