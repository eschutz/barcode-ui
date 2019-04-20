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
 *      @file ui.c
 *      @brief UI function implementations as defined in ui.h
 *      @author Elijah Schutz
 *      @date 3/3/19
 */

#include "ui.h"
#include "win.h"
#include "error.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "util.h"
#include "backend.h"

G_DEFINE_TYPE(BarcodeApp, barcode_app, GTK_TYPE_APPLICATION);

BarcodeWindow *win;
GtkWidget *barcode_entry, *settings_box, *print_preview;
PSProperties ps_properties;
Layout *page_layout;
char barcodes[MAX_BARCODES][BK_BARCODE_LENGTH];
int barcode_quantities[MAX_BARCODES];
int barcode_entry_id = 0;

static void barcode_app_init(BarcodeApp *app) {
    ps_properties = PS_DEFAULT_PROPS;
    barcode_quantities[0] = 1;
    page_layout = calloc(1, sizeof(Layout));
    page_layout->cols = DEFAULT_COLS;
    page_layout->rows = DEFAULT_ROWS;
}

static void barcode_app_activate(GApplication *app) {
    GtkWidget *combo_box, *page_layout_box, *units_box;

    win = barcode_window_new(BARCODE_APP(app));

    WIDGET_LOOKUP(win, barcode_entry_path, BARCODE_ENTRY_PATH_LENGTH, barcode_entry);

    WIDGET_LOOKUP(win, settings_box_path, SETTINGS_BOX_PATH_LENGTH, settings_box);

    WIDGET_LOOKUP(win, print_preview_path, PRINT_PREVIEW_PATH_LENGTH, print_preview);

    new_barcode_btn_clicked(NULL, NULL);

    WIDGET_LOOKUP(settings_box, page_layout_box_path, PAGE_LAYOUT_BOX_PATH_LENGTH, page_layout_box);
    units_box = gtk_container_get_children(GTK_CONTAINER(gtk_flow_box_get_child_at_index(GTK_FLOW_BOX(page_layout_box), 0)))->data;
    combo_box = gtk_container_get_children(GTK_CONTAINER(gtk_flow_box_get_child_at_index(GTK_FLOW_BOX(units_box), 1)))->data;

    if (NULL == combo_box) {
        fprintf(stderr, "Combo Box widget lookup failed\n");
    }

    gtk_combo_box_set_active_id(GTK_COMBO_BOX(combo_box), UNIT_ID_MM);

    gtk_window_present(GTK_WINDOW(win));
}

static void barcode_app_open(GApplication *app, GFile **files, gint n_files, const gchar *hint) {
    barcode_app_activate(app);
}

static void barcode_app_class_init(BarcodeAppClass *class) {
    G_APPLICATION_CLASS(class)->activate = barcode_app_activate;
    G_APPLICATION_CLASS(class)->open = barcode_app_open;
}

static void barcode_app_quit(GApplication *app) {
    free(page_layout);
    if (ghostscript_exit(gs_instance) < SUCCESS) {
        fprintf(stderr, "Error on exiting Ghostscript\n");
    }
}

BarcodeApp * barcode_app_new(void) {
    return g_object_new(BARCODE_TYPE_APP, "application-id", "org.eschutz.barcode", "flags", G_APPLICATION_HANDLES_OPEN, NULL);
}


void refresh(void) {
    char new_barcodes[barcode_entry_id][BK_BARCODE_LENGTH];
    int new_barcode_quantities[barcode_entry_id];

    int new_num_barcodes = 0;
    for (int i = 0; i < barcode_entry_id; i++) {
        if (strlen(barcodes[i]) > 0) {
            strncpy(new_barcodes[new_num_barcodes], barcodes[i], BK_BARCODE_LENGTH);
            new_barcode_quantities[new_num_barcodes] = barcode_quantities[i];
            new_num_barcodes++;
        }
    }


    char* preview_path = calloc(1, BK_TEMPFILE_TEMPLATE_LENGTH);
    strncpy(preview_path,
            bk_generate_png(new_barcodes, new_barcode_quantities, new_num_barcodes, &ps_properties, page_layout),
            BK_TEMPFILE_TEMPLATE_LENGTH);
    gtk_image_set_from_file(GTK_IMAGE(print_preview), preview_path);
    free(preview_path);
    /* Debugging
    fprintf(stderr, "Page Layout --\n");
    fprintf(stderr, "       rows : %d\n", page_layout->rows);
    fprintf(stderr, "       cols : %d\n\n", page_layout->cols);
    fprintf(stderr, "PostScript Properties --\n");
    fprintf(stderr, "       units        : %s\n", ps_properties.units);
    fprintf(stderr, "       lmargin      : %.2f\n", ps_properties.lmargin);
    fprintf(stderr, "       rmargin      : %.2f\n", ps_properties.rmargin);
    fprintf(stderr, "       tmargin      : %.2f\n", ps_properties.tmargin);
    fprintf(stderr, "       bmargin      : %.2f\n", ps_properties.bmargin);
    fprintf(stderr, "       bar_width    : %.2f\n", ps_properties.bar_width);
    fprintf(stderr, "       bar_height   : %.2f\n", ps_properties.bar_height);
    fprintf(stderr, "       padding      : %.2f\n", ps_properties.padding);
    fprintf(stderr, "       column_width : %.2f\n", ps_properties.column_width);
    fprintf(stderr, "       fontsize     : %.2d\n\n", ps_properties.fontsize);
    */

}

//rows
void rows_changed(GtkEntry *entry, gpointer data) {
    double value;
    int status = gtk_entry_get_text_as_double(entry, &value);
    if (status == SUCCESS) {
        page_layout->rows = value;
        refresh();
    }
}

//columns
void cols_changed(GtkEntry *entry, gpointer data) {
    double value;
    int status = gtk_entry_get_text_as_double(entry, &value);
    if (status == SUCCESS) {
        page_layout->cols = value;
        refresh();
    }
}

//units
void units_changed(GtkComboBoxText *combo_box, gpointer data) {
    strncpy(ps_properties.units, gtk_combo_box_text_get_active_text(combo_box), UNIT_ID_LEN);
    refresh();
}

//lmargin
void lmargin_changed(GtkEntry *entry, gpointer data) {
    double value;
    int status = gtk_entry_get_text_as_double(entry, &value);
    if (status == SUCCESS) {
        ps_properties.lmargin = value;
        refresh();
    } else {
        fprintf(stderr, "%s\n", gtk_entry_get_text(entry));
    }
}
//rmargin
void rmargin_changed(GtkEntry *entry, gpointer data) {
    double value;
    int status = gtk_entry_get_text_as_double(entry, &value);
    if (status == SUCCESS) {
        ps_properties.rmargin = value;
        refresh();
    } else {
        fprintf(stderr, "%s\n", gtk_entry_get_text(entry));
    }
}
//bmathin
void bmargin_changed(GtkEntry *entry, gpointer data) {
    double value;
    int status = gtk_entry_get_text_as_double(entry, &value);
    if (status == SUCCESS) {
        ps_properties.bmargin = value;
        refresh();
    } else {
        fprintf(stderr, "%s\n", gtk_entry_get_text(entry));
    }
}
//tmargin
void tmargin_changed(GtkEntry *entry, gpointer data) {
    double value;
    int status = gtk_entry_get_text_as_double(entry, &value);
    if (status == SUCCESS) {
        ps_properties.tmargin = value;
        refresh();
    }
}
//bar width
void bar_width_changed(GtkEntry *entry, gpointer data) {
    double value;
    int status = gtk_entry_get_text_as_double(entry, &value);
    if (status == SUCCESS) {
        ps_properties.bar_width = value;
        refresh();
    }
}
//bar height
void bar_height_changed(GtkEntry *entry, gpointer data) {
    double value;
    int status = gtk_entry_get_text_as_double(entry, &value);
    if (status == SUCCESS) {
       ps_properties.bar_height = value;
        refresh();
    }
}
// padding
void padding_changed(GtkEntry *entry, gpointer data) {
    double value;
    int status = gtk_entry_get_text_as_double(entry, &value);
    if (status == SUCCESS) {
        ps_properties.padding = value;
        refresh();
    }
}
//col width
void col_width_changed(GtkEntry *entry, gpointer data) {
    double value;
    int status = gtk_entry_get_text_as_double(entry, &value);
    if (status == SUCCESS) {
        ps_properties.column_width = value;
        refresh();
    }
}
//fsize
void fsize_changed(GtkEntry *entry, gpointer data) {
    double value;
    int status = gtk_entry_get_text_as_double(entry, &value);
    if (status == SUCCESS) {
        ps_properties.fontsize = value;
        refresh();
    }
}

void spin_button_handler(GtkSpinButton *button, int *id) {
    int _id;
    char * btn_name = calloc(1, WIDGET_ID_MAXLEN);
    strncpy(btn_name, gtk_widget_get_name(GTK_WIDGET(button)), WIDGET_ID_MAXLEN);

    // status in this case is the number of variables filled by sscanf()
    int status = sscanf(btn_name, BARCODE_SPIN_NAME, &_id);
    if (status == 0) {
        fprintf(stderr, "sscanf failed on \"%s\"\n", btn_name);
    }

    barcode_quantities[_id] = gtk_spin_button_get_value_as_int(button);
    refresh();
}

int barcode_entry_focus_out(GtkEntry *entry, GdkEvent event, int *id) {
    int _id;
    char * entry_name = calloc(1, WIDGET_ID_MAXLEN);
    strncpy(entry_name, gtk_widget_get_name(GTK_WIDGET(entry)), WIDGET_ID_MAXLEN);

    // status in this case is the number of variables filled by sscanf()
    int status = sscanf(entry_name, BARCODE_ENTRY_NAME, &_id);
    if (status == 0) {
        fprintf(stderr, "sscanf failed on \"%s\"\n", entry_name);
    }
    strncpy(barcodes[_id], gtk_entry_get_text(entry), BK_BARCODE_LENGTH);
    refresh();

    return GDK_EVENT_PROPAGATE;
}

void new_barcode_btn_clicked(GtkButton *button, gpointer user_data) {
    if (barcode_entry_id >= MAX_BARCODES) {
        gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
        return;
    }

    GtkWidget *barcode_box, *new_entry, *spin_btn;
    GtkAdjustment *adjustment;
    char box_name[BARCODE_BOX_BUFSIZE], entry_name[BARCODE_ENTRY_BUFSIZE], spin_name[BARCODE_SPIN_BUFSIZE];

    snprintf(&box_name, BARCODE_BOX_BUFSIZE, BARCODE_BOX_NAME, barcode_entry_id);
    snprintf(&entry_name, BARCODE_ENTRY_BUFSIZE, BARCODE_ENTRY_NAME, barcode_entry_id);
    snprintf(&spin_name, BARCODE_SPIN_BUFSIZE, BARCODE_SPIN_NAME, barcode_entry_id);

    barcode_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, BARCODE_BOX_SPACING);
    gtk_widget_set_name(barcode_box, box_name);

    new_entry = gtk_entry_new();
    gtk_widget_set_name(new_entry, entry_name);
    gtk_entry_set_max_length(GTK_ENTRY(new_entry), BARCODE_ENTRY_MAX_LENGTH);
    g_signal_connect(new_entry, "activate", G_CALLBACK(new_barcode_btn_clicked), NULL);
    g_signal_connect(new_entry, "focus-out-event", G_CALLBACK(barcode_entry_focus_out), &barcode_entry_id);

    adjustment = gtk_adjustment_new(BARCODE_SPIN_VALUE, BARCODE_SPIN_MIN, BARCODE_SPIN_MAX, BARCODE_SPIN_STEP, BARCODE_SPIN_PAGE, BARCODE_SPIN_PAGESIZE);

    spin_btn = gtk_spin_button_new(adjustment, BARCODE_SPIN_CLIMB, BARCODE_SPIN_DIGITS);
    gtk_spin_button_set_snap_to_ticks(GTK_SPIN_BUTTON(spin_btn), TRUE);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spin_btn), TRUE);
    gtk_widget_set_name(spin_btn, spin_name);
    g_signal_connect(spin_btn, "value-changed", G_CALLBACK(spin_button_handler), &barcode_entry_id);

    gtk_box_pack_start(GTK_BOX(barcode_box), new_entry, BARCODE_BOX_EXPAND, BARCODE_BOX_FILL, BARCODE_BOX_PADDING);
    gtk_box_pack_start(GTK_BOX(barcode_box), spin_btn, BARCODE_BOX_EXPAND, BARCODE_BOX_FILL, BARCODE_BOX_PADDING);

    gtk_box_pack_start(GTK_BOX(barcode_entry), barcode_box, BARCODE_BOX_EXPAND, BARCODE_BOX_FILL, BARCODE_BOX_PADDING);

    gtk_widget_show_all(barcode_box);

    barcode_entry_id++;
}
