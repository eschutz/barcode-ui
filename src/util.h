/* Copyright © 2019 Elijah Schutz */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

/*
 *      @file util.h
 *      @brief Utility function declarations
 *      @author Elijah Schutz
 *      @date 23/3/19
 */

#ifndef UTIL_H
#define UTIL_H

#include "error.h"
#include "gtk/gtk.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*      @brief Maximum length of a widget name or ID */
#define WIDGET_ID_MAXLEN 32

/**
 *      @defgroup UIPathLengths Specific lengths of widget path arrays
 *      @see WIDGET_LOOKUP
 *      @see barcode_app_activate
 */
/*@{*/
// clang-format off
#define BARCODE_ENTRY_PATH_LENGTH   2
#define SETTINGS_FRAME_PATH_LENGTH  3
#define SETTINGS_BOX_PATH_LENGTH    4
#define UI_HINT_VIEW_PATH_LENGTH    3
// Child of settings_box
#define PAGE_LAYOUT_BOX_PATH_LENGTH 1
#define PRINTER_COMBO_BOX_PATH_LENGTH 4
/*@}*/

/*      @brief Platform-dependent file separator */
#ifdef _WIN32
#define UTIL_FSEP "\""
#else
#define UTIL_FSEP "/"
#endif

#define FSEP_LEN 1

#define NUM_CMD_LINE_OPTS 3
#define CMD_LINE_OPTS_LENGTH 16

/*      @brief Utility macro for compile-time string length calculations (w/null terminator) */
#define STRLEN(str) sizeof(str) / sizeof(char)

// clang-format on

/**
 *      @brief Macro for error-handling code for widget lookup in the user interface
 *      @param wdgt GtkContainer or subclass to use as the root node
 *      @param path char array of 'name' properties that lead to the node
 *      @param length Length of @c path
 *      @param dest Pointer to a GtkWidget for the resulting node
 */
// clang-format off
#define WIDGET_LOOKUP(cont, path, length, dest)                                                    \
    do {                                                                                           \
        int status = gtk_widget_lookup_path(                                                       \
            GTK_CONTAINER(cont),                                                                   \
            (char(*)[WIDGET_ID_MAXLEN]) path,                                                      \
            length,                                                                                \
            &dest                                                                                  \
        );                                                                                         \
        if (SUCCESS != status) {                                                                   \
            fprintf(stderr, "FATAL: Widget lookup for " #path ":\n");                              \
            for (int i = 0; i < length; i++) {                                                     \
                fprintf(stderr, "%s%s", path[i], (i == length - 1 ? "\n" : "->"));                 \
            }                                                                                      \
            fprintf(stderr, "failed with exit code: %d\n", status);                                \
            exit(EXIT_FAILURE);                                                                    \
        }                                                                                          \
    } while (0);
// clang-format on

/**
 *      @defgroup UIPaths Constant paths used for looking up static widgets
 */
/*@{*/
extern const char barcode_entry_path[BARCODE_ENTRY_PATH_LENGTH][WIDGET_ID_MAXLEN];
extern const char settings_frame_path[SETTINGS_FRAME_PATH_LENGTH][WIDGET_ID_MAXLEN];
extern const char settings_box_path[SETTINGS_BOX_PATH_LENGTH][WIDGET_ID_MAXLEN];
extern const char page_layout_box_path[PAGE_LAYOUT_BOX_PATH_LENGTH][WIDGET_ID_MAXLEN];
extern const char ui_hint_view_path[UI_HINT_VIEW_PATH_LENGTH][WIDGET_ID_MAXLEN];
extern const char printer_combo_box_path[PRINTER_COMBO_BOX_PATH_LENGTH][WIDGET_ID_MAXLEN];
/*@}*/

/**
 *      @brief Finds a widget at the end of a particular node path of 'name' properties of container
 *             widgets
 *      @param container The root container from which to begin the search
 *      @param path A char array of 'name' properties that lead to the lookup node
 *      @param pathlen The length of @c path
 *      @param dest A double pointer to a destination widget for the resultant node
 *      @return ERR_WIDGET_NOT_FOUND, ERR_NO_WIDGET_CHILDREN, SUCCESS
 *      @warning It is preferable not to call this directly. Use WIDGET_LOOKUP() instead, as it
 *               handles errors automatically.
 */
int gtk_widget_lookup_path(GtkContainer *, char[][WIDGET_ID_MAXLEN], int, GtkWidget **);

/**
 *      @brief Extracts a named child from a GtkContainer
 *      @param container The container to search
 *      @param name The name of the child ('name' GtkBuilder property)
 *      @param dest Double pointer to a GtkWidget for the resultant child widget
 *      @return ERR_WIDGET_NOT_FOUND, SUCCESS
 */
int gtk_widget_query_name(GtkContainer *, char *, GtkWidget **);

/**
 *      @brief Parses the text of a GtkEntry as a double precision floating point number
 *      @param entry The GtkEntry from which to read
 *      @param dest A destination pointer
 *      @return ERR_INVALID_STRING, SUCCESS
 */
int gtk_entry_get_text_as_double(GtkEntry *, double *);

/**
 *      @brief Checks if a string represents a float
 *      @param str A string to be tested on
 *      @return true or false
 */
bool isfloat(char *);

/**
 *      @brief Strips the whitespace from the beginning and end of a string
 *      @param str A string to strip
 *      @return A stripped string
 *      @warning Memory is allocated to the return value in this function, so make sure you
 *               free() the return value once you're done.
 */
char * strstrip(char *);

#endif
