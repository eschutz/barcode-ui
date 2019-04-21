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
 *      @file win.c
 *      @brief Barcode App Window function implementations as defined in win.h
 *      @author Elijah Schutz
 *      @date 3/3/19
 */

#include "win.h"

/*      @brief (Required by GTK) BarcodeWindow type definition */
G_DEFINE_TYPE(BarcodeWindow, barcode_window, GTK_TYPE_APPLICATION_WINDOW);

static void barcode_window_init(BarcodeWindow *win) { gtk_widget_init_template(GTK_WIDGET(win)); }

void barcode_window_open(BarcodeWindow *win, GFile *file) {}

static void barcode_window_class_init(BarcodeWindowClass *class) {
    /* Here we let GtkBuilder know that we want to use the following functions as UI callbacks.
     * They are registered within the template file.
     */
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), TEMPLATE_FILE);
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), "new_barcode_btn_clicked");
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), "spin_button_handler");
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), "rows_changed");
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), "cols_changed");
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), "units_changed");
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), "lmargin_changed");
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), "rmargin_changed");
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), "tmargin_changed");
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), "bar_width_changed");
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), "bar_height_changed");
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), "padding_changed");
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), "col_width_changed");
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), "fsize_changed");
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), "barcode_entry_focus_out");
}

BarcodeWindow *barcode_window_new(BarcodeApp *app) {
    return g_object_new(BARCODE_TYPE_WINDOW, "application", app, NULL);
}
