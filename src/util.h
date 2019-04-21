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
 *      @file util.h
 *      @brief Utility function declarations
 *      @author Elijah Schutz
 *      @date 23/3/19
 */

#ifndef UTIL_H
#define UTIL_H

#include "error.h"
#include "gtk/gtk.h"
#include "iapi.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*      @brief Minimum supported GhostScript version */
#define GS_VER 926

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
#define SETTINGS_BOX_PATH_LENGTH    3
// Child of settings_box
#define PAGE_LAYOUT_BOX_PATH_LENGTH 1
#define PRINT_PREVIEW_PATH_LENGTH   2
/*@}*/
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
            exit(ERR_GENERIC);                                                                     \
        }                                                                                          \
    } while (0);
// clang-format on

/**
 *      @defgroup UIPaths Constant paths used for looking up static widgets
 */
/*@{*/
extern const char barcode_entry_path[BARCODE_ENTRY_PATH_LENGTH][WIDGET_ID_MAXLEN];
extern const char settings_box_path[SETTINGS_BOX_PATH_LENGTH][WIDGET_ID_MAXLEN];
extern const char page_layout_box_path[PAGE_LAYOUT_BOX_PATH_LENGTH][WIDGET_ID_MAXLEN];
extern const char print_preview_path[PRINT_PREVIEW_PATH_LENGTH][WIDGET_ID_MAXLEN];
/*@}*/

/**
 *      @defgroup GhostScriptEnvironment Variables required for working with GhostScript
 */
/*@{*/
/*      @brief Global GhostScript instance variable */
extern void *gs_instance;
/*      @brief Flag to tell whether GhostScript has been initialised */
extern bool gs_initialised;
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
 *      @brief Initialises the GhostScript interpreter
 *      @return GHOSTSCRIPT_ERR_GENERIC, SUCCESS
 */
int ghostscript_init(void);

/**
 *      @brief Initialises the GhostScript interpreter instance
 *      @param pinstance A double void destination pointer for the GhostScript instance
 *      @param caller_handler A void pointer for the caller handler
 *      @param argc Length of @c argv
 *      @param argv Vector of command line arguments (the first is ignored, see GhostScript
 *                  documentation for more info)
 *      @return < 0 if error (GhostScript error code), SUCCESS
 */
int ghostscript_instance(void **, void *, int argc, char **);

/**
 *      @brief Shutdowns the GhostScript interpreter
 *      @param instance The used GhostScript Instance
 *      @return < 0 if error (GhostScript error code), SUCCESS
 */
int ghostscript_exit(void *);

/**
 *      @brief Checks if a string represents a float
 *      @param str A string to be tested on
 *      @return true or false
 */
bool isfloat(char *);

#endif
