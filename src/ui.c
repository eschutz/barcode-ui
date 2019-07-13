/* Copyright © 2019 Elijah Schutz */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

/*
 *      @file ui.c
 *      @brief UI function implementations as defined in ui.h
 *      @author Elijah Schutz
 *      @date 3/3/19
 */

#include "ui.h"

#include "backend.h"
#include "barcode.h"
#include "error.h"
#include "gtk/gtk.h"
#include "util.h"
#include "win.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*      @brief (Required by GTK) BarcodeApp type definition */
G_DEFINE_TYPE(BarcodeApp, barcode_app, GTK_TYPE_APPLICATION);

/*      @brief Main global BarcodeWindow object */
static BarcodeWindow * win;

/*      @brief Global barcode_entry widget reference */
static GtkWidget * barcode_entry;

/*      @brief Global settings_frame widget reference */
static GtkWidget * settings_frame;

/*      @brief Global settings_label widget (label of settings frame) reference */
static GtkLabel * settings_label;

/*      @brief Global settings_box widget reference */
static GtkWidget * settings_box;

/*      @brief Global printer_combo_box widget reference */
static GtkWidget * printer_combo_box;

/*      @brief Global UI hint text buffer */
static GtkTextBuffer * ui_hint_text_buffer;

/*      @brief Global ui_hint_view widget reference */
static GtkWidget * ui_hint_view;

/*      @brief Global PostScript properties structure */
static PSProperties ps_properties;

/*      @brief Global page layout structure */
static Layout * page_layout;

/*      @brief Global list of barcodes entered via the UI */
static char barcodes[MAX_BARCODES][BK_BARCODE_LENGTH];

/*      @brief Global selected printer string */
static char * selected_printer;

/*      @brief Global selected printer length */
static int selected_printer_length;

/**
 *      @brief Global list of barcode quantities entered via the UI
 *      @details Barcode quantities are associated such that @c barcode[n] must be printed
 *              @c barcode_quantities[n] times.
 */
static int barcode_quantities[MAX_BARCODES];

/**
 *      @brief Global barcode entry identifier
 *      @details @c barcode_entry_id represents the ID number of the last barcode entry widget
 *              displayed in the UI. Equivalently, it represents the number of barcode entry widgets
 *              currently displayed.
 */
static int barcode_entry_id = 0;

/**
 *      @details @c barcode_app_init is used for initialising the PostScript properties, page
 * layout, and barcode quantities to their respective default values.
 */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static bool settings_frame_err = false;

static void barcode_app_init(BarcodeApp * app) {
    atexit(ui_cleanup);


    ps_properties         = PS_DEFAULT_PROPS;
    barcode_quantities[0] = 1;

    size_t layout_size = sizeof(Layout);
    page_layout        = calloc(1, layout_size);
    VERIFY_NULL_BC(page_layout, layout_size);

    page_layout->cols = DEFAULT_COLS;
    page_layout->rows = DEFAULT_ROWS;
}

#pragma GCC diagnostic pop

/**
 *      @details In @c barcode_app_activate, @c barcode_entry, @c settings_box, and @c print_preview
 *              are initialised as @c win is initialised from the template file. Initialisation of
 *              the former three variables is achieved via the WIDGET_LOOKUP() macro.
 *      @see WIDGET_LOOKUP
 */
static void barcode_app_activate(GApplication * app) {

    /* GTK does not allow settings default combo box values (for the 'units' property), so these are
       used as intermediate storage in looking up the units box. Flow boxes are created with
       indexed children, so nested flow boxes need to be looked up by name, then extracted via an
       index, then children objects looked up by name again etc. */
    GtkWidget *combo_box, *page_layout_box, *units_box;

    win = barcode_window_new(BARCODE_APP(app));

    WIDGET_LOOKUP(win, barcode_entry_path, BARCODE_ENTRY_PATH_LENGTH, barcode_entry);

    WIDGET_LOOKUP(win, settings_frame_path, SETTINGS_FRAME_PATH_LENGTH, settings_frame);
    settings_label = GTK_LABEL(gtk_frame_get_label_widget(GTK_FRAME(settings_frame)));

    WIDGET_LOOKUP(win, ui_hint_view_path, UI_HINT_VIEW_PATH_LENGTH, ui_hint_view);
    ui_hint_text_buffer = gtk_text_buffer_new(gtk_text_tag_table_new());
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(ui_hint_view), ui_hint_text_buffer);

    WIDGET_LOOKUP(win, settings_box_path, SETTINGS_BOX_PATH_LENGTH, settings_box);

    WIDGET_LOOKUP(win, printer_combo_box_path, PRINTER_COMBO_BOX_PATH_LENGTH, printer_combo_box);

    // Populate printer combo box
    char ** printers;
    int     num_printers, max_printer_len = 0;
    int     status = bk_get_printers(&printers, &num_printers);
    if (status == SUCCESS) {
        for (int i = 0; i < num_printers; i++) {
            int printer_len = strlen(printers[i]);
            if (printer_len > max_printer_len) {
                max_printer_len = printer_len;
            }
            gtk_combo_box_text_insert(
                GTK_COMBO_BOX_TEXT(printer_combo_box), i, printers[i], printers[i]);
        }
        max_printer_len++; // null-terminator
        selected_printer_length = max_printer_len;
        selected_printer        = calloc(1, selected_printer_length);
        VERIFY_NULL_BC(selected_printer, selected_printer_length);
        strncpy(selected_printer, printers[0], selected_printer_length);

        for (int i = 0; i < num_printers; i++) {
            free(printers[i]);
        }
        free(printers);
    } else {
        gtk_combo_box_text_insert(
            GTK_COMBO_BOX_TEXT(printer_combo_box), 0, NULL, "Unable to obtain available printers");
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(printer_combo_box), 0);

    new_barcode_btn_clicked(NULL, NULL);

    // Extract the outer settings_box flow box...
    WIDGET_LOOKUP(settings_box, page_layout_box_path, PAGE_LAYOUT_BOX_PATH_LENGTH, page_layout_box);
    // ...look up the units flow box via index...

    // clang-format off
    units_box = gtk_container_get_children(
        GTK_CONTAINER(gtk_flow_box_get_child_at_index(GTK_FLOW_BOX(page_layout_box),
        UNITS_BOX_IDX))
    )->data;
    // ...extract the units combo box from the units flow box, again by index
    combo_box = gtk_container_get_children(
        GTK_CONTAINER(gtk_flow_box_get_child_at_index(GTK_FLOW_BOX(units_box),
        UNITS_COMBO_IDX))
    )->data;
    // clang-format on

    if (NULL == combo_box) {
        fprintf(stderr, "Combo Box widget lookup failed\n");
    }

    // Set the default units
    gtk_combo_box_set_active_id(GTK_COMBO_BOX(combo_box), DEFAULT_UNIT);

    gtk_window_present(GTK_WINDOW(win));
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
static void barcode_app_open(GApplication * app, GFile ** files, gint n_files, const gchar * hint) {

    barcode_app_activate(app);
}
#pragma GCC diagnostic pop

static void barcode_app_class_init(BarcodeAppClass * class) {

    G_APPLICATION_CLASS(class)->activate = barcode_app_activate;
    G_APPLICATION_CLASS(class)->open     = barcode_app_open;
}

BarcodeApp * barcode_app_new(void) {

    // clang-format off
    return g_object_new(
        BARCODE_TYPE_APP,
        "application-id",
        "org.eschutz.barcode",
        "flags",
        G_APPLICATION_HANDLES_OPEN,
        NULL
    );
    // clang-format on
}

/**
 *      @details refresh_postscript() is called whenever a field affecting the generated postscript
 *              is updated.
 */
int refresh_postscript(char ** print_file_dest) {

// barcode_entry_id here represents the number of barcode entry dialogues on screen
#ifdef _WIN32
    size_t  new_barcodes_size = sizeof(char *) * barcode_entry_id;
    char ** new_barcodes      = calloc(1, new_barcodes_size);
    VERIFY_NULL_BC(new_barcodes, new_barcodes_size);
    size_t elem_size = sizeof(new_barcodes[0]) * BK_BARCODE_LENGTH;
    for (int i = 0; i < barcode_entry_id; i++) {
        new_barcodes[i] = calloc(1, elem_size);
        VERIFY_NULL_BC(new_barcodes[i], elem_size);
    }
    size_t quantities_size        = sizeof(int) * barcode_entry_id;
    int *  new_barcode_quantities = calloc(1, quantities_size);
    VERIFY_NULL_BC(new_barcode_quantities, quantities_size);
#else
    char new_barcodes[barcode_entry_id][BK_BARCODE_LENGTH];
    int  new_barcode_quantities[barcode_entry_id];
#endif

    // Filter out empty barcode entries
    int new_barcodes_num = 0;
    for (int i = 0; i < barcode_entry_id; i++) {
        if (strlen(barcodes[i]) > 0) {
            /* new_barcodes_num is the number of non-empty barcodes so far, so is the index of the
               next non-empty barcode */
            strncpy(new_barcodes[new_barcodes_num], barcodes[i], BK_BARCODE_LENGTH);
            new_barcode_quantities[new_barcodes_num] = barcode_quantities[i];
            new_barcodes_num++;
        }
    }

    // bk_generate_png generates the print preview and fills print_file_name with its file path as a
    // string
    int result = bk_generate(new_barcodes,
                             new_barcode_quantities,
                             new_barcodes_num,
                             &ps_properties,
                             page_layout,
                             print_file_dest);

#ifdef _WIN32
    for (int i = 0; i < barcode_entry_id; i++) {
        free(new_barcodes[i]);
    }
    free(new_barcodes);
    free(new_barcode_quantities);
#endif
    return result;
}

/**
 *      @details ui_hint() takes the return value of refresh_postscript() and updates the UI hint
 *              text buffer with a message as necessary, as well as some UI modifications
 *              (such as highlighting responsible parameters).
 */
int ui_hint(int err) {
    int status = SUCCESS;

    char message[UI_HINT_MAX_LEN] = "";
    switch (err) {
        case SUCCESS:
            if (settings_frame_err) {
                gtk_label_set_markup(settings_label, SETTINGS_LABEL_DEF_MARKUP);
                settings_frame_err = false;
            }
            break;
        case ERR_DATA_LENGTH:
            strncpy(message, "INTERNAL ERROR: Invalid text length\n", UI_HINT_MAX_LEN);
            break;
        case ERR_INVALID_CODE_SET:
            strncpy(message, "INTERNAL ERROR: Invalid Code-128 code set\n", UI_HINT_MAX_LEN);
            break;
        case ERR_ARGUMENT:
            strncpy(message, "INTERNAL ERROR: Argument error\n", UI_HINT_MAX_LEN);
            break;
        case ERR_FILE_RESET_FAILED:
            strncpy(message,
                    "INTERNAL ERROR: Could not reset file contents – no PostScript written\n",
                    UI_HINT_MAX_LEN);
            break;
        case ERR_FILE_WRITE_FAILED:
            strncpy(message,
                    "INTERNAL ERROR: Could not write to file – no PostScript written\n",
                    UI_HINT_MAX_LEN);
            break;
        case ERR_CHAR_INVALID:
            strncpy(message,
                    "Invalid character: Text includes an invalid character – please remove before"
                    " regenerating\n",
                    UI_HINT_MAX_LEN);
            break;
        case ERR_INVALID_LAYOUT:
            strncpy(message,
                    "Invalid layout: Number of rows and columns does not match the number of "
                    "barcodes\n",
                    UI_HINT_MAX_LEN);
            // Update UI with red around layout boxes to indicate invalid rows / columns if they're
            // invalid
            settings_frame_err = true;
            gtk_label_set_markup(settings_label, SETTINGS_LABEL_ERR_MARKUP);
            break;
        case ERR_FLUSH:
            strncpy(message,
                    "INTERNAL ERROR: Could not flush output – printed barcodes may be clipped\n",
                    UI_HINT_MAX_LEN);
            break;
        case ERR_PRINTER_LIST:
            snprintf(message,
                     UI_HINT_MAX_LEN,
                     "ERROR: Could not get list of printers - check the output of %s\n",
                     BK_GET_PRINTER_CMD);
            break;
        default:
            snprintf(
                message, UI_HINT_MAX_LEN, "UNKNOWN ERROR: Received unknown error code: %d\n", err);
            break;
    }

    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(ui_hint_text_buffer), message, -1);

    return status;
}

int do_print(char * filename) {
    int    status = SUCCESS;
    char * active_text =
        gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(printer_combo_box)); // freed by GTK

    if (active_text != NULL) {
        strncpy(selected_printer, active_text, selected_printer_length);
        status = bk_print(filename, selected_printer);
    } else {
        status = ERR_GENERIC;
    }

    return status;
}

/* Ignore all unused parameter warnings, as the function signature must be accepted by GTK
   regardless of whether we use all the parameters or not */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/**
 *      @details print_button_clicked() regenerates the PostScript output and calls do_print()
 *      @see do_print()
 */
void print_button_clicked(GtkButton * button, gpointer user_data) {
    int    ps_status, ui_status, print_status;
    char * print_file_dest = NULL;
    ps_status              = refresh_postscript(&print_file_dest);

    // Update UI hints based on output value
    ui_status = ui_hint(ps_status);
    if (SUCCESS == ps_status && SUCCESS == ui_status) {
        print_status = do_print(print_file_dest);
    }

    free(print_file_dest);
}

/**
 *      @details The function is called when the 'changed' event is emitted. The entry content needs
 *              to be polled and the relevant structure updated (page layout, PostScript properties,
 *              barcodes and barcode quantities).
 */
void rows_changed(GtkEntry * entry, gpointer data) {
    double value;
    int    status = gtk_entry_get_text_as_double(entry, &value);
    if (status == SUCCESS) {
        page_layout->rows = value;
    }
}

/**
 *      @details The function is called when the 'changed' event is emitted. The entry content needs
 *              to be polled and the relevant structure updated (page layout, PostScript properties,
 *              barcodes and barcode quantities).
 */
void cols_changed(GtkEntry * entry, gpointer data) {
    double value;
    int    status = gtk_entry_get_text_as_double(entry, &value);
    if (status == SUCCESS) {
        page_layout->cols = value;
    }
}

/**
 *      @details The function is called when the 'changed' event is emitted. The entry content needs
 *              to be polled and the relevant structure updated (page layout, PostScript properties,
 *              barcodes and barcode quantities).
 */
void units_changed(GtkComboBoxText * combo_box, gpointer data) {
    strncpy(ps_properties.units, gtk_combo_box_text_get_active_text(combo_box), UNIT_ID_LEN);
}

/**
 *      @details The function is called when the 'changed' event is emitted. The entry content needs
 *              to be polled and the relevant structure updated (page layout, PostScript properties,
 *              barcodes and barcode quantities).
 */
void lmargin_changed(GtkEntry * entry, gpointer data) {
    double value;
    int    status = gtk_entry_get_text_as_double(entry, &value);
    if (status == SUCCESS) {
        ps_properties.lmargin = value;
    }
}

/**
 *      @details The function is called when the 'changed' event is emitted. The entry content needs
 *              to be polled and the relevant structure updated (page layout, PostScript properties,
 *              barcodes and barcode quantities).
 */
void rmargin_changed(GtkEntry * entry, gpointer data) {
    double value;
    int    status = gtk_entry_get_text_as_double(entry, &value);
    if (status == SUCCESS) {
        ps_properties.rmargin = value;
    }
}

/**
 *      @details The function is called when the 'changed' event is emitted. The entry content needs
 *              to be polled and the relevant structure updated (page layout, PostScript properties,
 *              barcodes and barcode quantities).
 */
void bmargin_changed(GtkEntry * entry, gpointer data) {
    double value;
    int    status = gtk_entry_get_text_as_double(entry, &value);
    if (status == SUCCESS) {
        ps_properties.bmargin = value;
    }
}

/**
 *      @details The function is called when the 'changed' event is emitted. The entry content needs
 *              to be polled and the relevant structure updated (page layout, PostScript properties,
 *              barcodes and barcode quantities).
 */
void tmargin_changed(GtkEntry * entry, gpointer data) {
    double value;
    int    status = gtk_entry_get_text_as_double(entry, &value);
    if (status == SUCCESS) {
        ps_properties.tmargin = value;
    }
}

/**
 *      @details The function is called when the 'changed' event is emitted. The entry content needs
 *              to be polled and the relevant structure updated (page layout, PostScript properties,
 *              barcodes and barcode quantities).
 */
void bar_width_changed(GtkEntry * entry, gpointer data) {
    double value;
    int    status = gtk_entry_get_text_as_double(entry, &value);
    if (status == SUCCESS) {
        ps_properties.bar_width = value;
    }
}

/**
 *      @details The function is called when the 'changed' event is emitted. The entry content needs
 *              to be polled and the relevant structure updated (page layout, PostScript properties,
 *              barcodes and barcode quantities).
 */
void bar_height_changed(GtkEntry * entry, gpointer data) {
    double value;
    int    status = gtk_entry_get_text_as_double(entry, &value);
    if (status == SUCCESS) {
        ps_properties.bar_height = value;
    }
}

/**
 *      @details The function is called when the 'changed' event is emitted. The entry content needs
 *              to be polled and the relevant structure updated (page layout, PostScript properties,
 *              barcodes and barcode quantities).
 */
void padding_changed(GtkEntry * entry, gpointer data) {
    double value;
    int    status = gtk_entry_get_text_as_double(entry, &value);
    if (status == SUCCESS) {
        ps_properties.padding = value;
    }
}

/**
 *      @details The function is called when the 'changed' event is emitted. The entry content needs
 *              to be polled and the relevant structure updated (page layout, PostScript properties,
 *              barcodes and barcode quantities).
 */
void col_width_changed(GtkEntry * entry, gpointer data) {
    double value;
    int    status = gtk_entry_get_text_as_double(entry, &value);
    if (status == SUCCESS) {
        ps_properties.column_width = value;
    }
}

/**
 *      @details The function is called when the 'changed' event is emitted. The entry content needs
 *              to be polled and the relevant structure updated (page layout, PostScript properties,
 *              barcodes and barcode quantities).
 */
void fsize_changed(GtkEntry * entry, gpointer data) {
    double value;
    int    status = gtk_entry_get_text_as_double(entry, &value);
    if (status == SUCCESS) {
        ps_properties.fontsize = value;
    }
}

/**
 *      @details The function is called when the 'value-changed' event is emitted. The spin button
 *              content needs to be polled and the barcode quantity updated.
 */
void spin_button_value_changed(GtkSpinButton * button, int * id) {
    int    _id;
    size_t btn_name_size = sizeof(char) * WIDGET_ID_MAXLEN;
    char * btn_name      = calloc(1, btn_name_size);
    VERIFY_NULL_BC(btn_name, btn_name_size);
    strncpy(btn_name, gtk_widget_get_name(GTK_WIDGET(button)), WIDGET_ID_MAXLEN);

    // status in this case is the number of variables filled by sscanf()
    int status = sscanf(btn_name, BARCODE_SPIN_NAME, &_id);
    if (status == 0) {
        fprintf(stderr, "ERROR: sscanf failed on \"%s\"\n", btn_name);
        return;
    }

    barcode_quantities[_id] = gtk_spin_button_get_value_as_int(button);
    free(btn_name);
}

/**
 *      @details barcode_entry_focus_out() is called on emission of the 'focus-out' event. The
 *              barcode is read from the relevant entry and @c barcodes updated, and the print
 *              preview refreshed. This is a slightly lower-level event handler, interacting with
 *              the GdkEvent whereas all other callbacks defined in this project need less context.
 *              This function signature is required by GTK, so the GdkEvent parameter is unused, but
 *              the event is propagated to be caught by a lower-level handler.
 */
int barcode_entry_focus_out(GtkEntry * entry, GdkEvent event, int * id) {
    int    _id;
    size_t entry_name_size = sizeof(char) * WIDGET_ID_MAXLEN;
    char * entry_name      = calloc(1, entry_name_size);
    VERIFY_NULL_BC(entry_name, entry_name_size);
    strncpy(entry_name, gtk_widget_get_name(GTK_WIDGET(entry)), WIDGET_ID_MAXLEN);

    // status in this case is the number of variables filled by sscanf()
    int status = sscanf(entry_name, BARCODE_ENTRY_NAME, &_id);
    if (status == 0) {
        fprintf(stderr, "sscanf failed on \"%s\"\n", entry_name);
    }
    strncpy(barcodes[_id], gtk_entry_get_text(entry), BK_BARCODE_LENGTH);

    free(entry_name);
    return GDK_EVENT_PROPAGATE;
}

/**
 *      @details new_barcode_btn_clicked() is responsible for creating a new barcode entry box and
 *              updating the UI and environment to reflect the new state.
 */
void new_barcode_btn_clicked(GtkButton * button, gpointer user_data) {
    // Disable creating new barcodes after the barcode limit is reached
    if (barcode_entry_id >= MAX_BARCODES) {
        gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
        return;
    }

    GtkWidget *barcode_box, *new_entry, *spin_btn;

    // Adjustment settings for the spin button
    GtkAdjustment * adjustment;

    /* The following are ID strings for their respective widgets, and are set from format templates
       that contain the numerical ID */
    // clang-format off
    char box_name[BARCODE_BOX_BUFSIZE],
         entry_name[BARCODE_ENTRY_BUFSIZE],
         spin_name[BARCODE_SPIN_BUFSIZE];
    // clang-format on

    // Filling the templates with the current barcode entry ID
    snprintf(box_name, BARCODE_BOX_BUFSIZE, BARCODE_BOX_NAME, barcode_entry_id);
    snprintf(entry_name, BARCODE_ENTRY_BUFSIZE, BARCODE_ENTRY_NAME, barcode_entry_id);
    snprintf(spin_name, BARCODE_SPIN_BUFSIZE, BARCODE_SPIN_NAME, barcode_entry_id);

    /* Creation of GTK widgets that make up the barcode entry box */
    barcode_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, BARCODE_BOX_SPACING);
    gtk_widget_set_name(barcode_box, box_name);

    new_entry = gtk_entry_new();
    gtk_widget_set_name(new_entry, entry_name);
    gtk_entry_set_max_length(GTK_ENTRY(new_entry), BARCODE_ENTRY_MAX_LENGTH);
    // Connect event callbacks
    g_signal_connect(new_entry, "activate", G_CALLBACK(new_barcode_btn_clicked), NULL);
    // clang-format off
    g_signal_connect(
        new_entry,
        "focus-out-event",
        G_CALLBACK(barcode_entry_focus_out),
        &barcode_entry_id
    );
    // clang-format on

    // clang-format off
    adjustment = gtk_adjustment_new(
        BARCODE_SPIN_VALUE,
        BARCODE_SPIN_MIN,
        BARCODE_SPIN_MAX,
        BARCODE_SPIN_STEP,
        BARCODE_SPIN_PAGE,
        BARCODE_SPIN_PAGESIZE
    );
    // clang-format on

    spin_btn = gtk_spin_button_new(adjustment, BARCODE_SPIN_CLIMB, BARCODE_SPIN_DIGITS);
    gtk_spin_button_set_snap_to_ticks(GTK_SPIN_BUTTON(spin_btn), TRUE);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spin_btn), TRUE);
    gtk_widget_set_name(spin_btn, spin_name);
    // clang-format off
    g_signal_connect(
        spin_btn,
        "value-changed",
        G_CALLBACK(spin_button_value_changed),
        &barcode_entry_id
    );
    // clang-format on

    /* Packing the widgets into the entry box in the correct order */
    // clang-format off
    gtk_box_pack_start(
        GTK_BOX(barcode_box),
        new_entry,
        BARCODE_BOX_EXPAND,
        BARCODE_BOX_FILL,
        BARCODE_BOX_PADDING
    );

    gtk_box_pack_start(
        GTK_BOX(barcode_box),
        spin_btn,
        BARCODE_BOX_EXPAND,
        BARCODE_BOX_FILL,
        BARCODE_BOX_PADDING
    );

    gtk_box_pack_start(
        GTK_BOX(barcode_entry),
        barcode_box,
        BARCODE_BOX_EXPAND,
        BARCODE_BOX_FILL,
        BARCODE_BOX_PADDING
    );
    // clang-format on

    /* Update the UI with the new widgets */
    gtk_widget_show_all(barcode_box);

    // Add default number of barcodes
    barcode_quantities[barcode_entry_id] = 1;

    barcode_entry_id++;
}

#pragma GCC diagnostic pop

void ui_cleanup(void) {
    free(page_layout);
    free(selected_printer);
}
