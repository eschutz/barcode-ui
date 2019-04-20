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

#include "gtk/gtk.h"
#include "error.h"
#include "iapi.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define GS_VER 926

#define WIDGET_ID_MAXLEN 32

#define BARCODE_ENTRY_PATH_LENGTH 2
#define SETTINGS_BOX_PATH_LENGTH 3
#define PAGE_LAYOUT_BOX_PATH_LENGTH 1 // child of settings_box
#define PRINT_PREVIEW_PATH_LENGTH 2
#define WIDGET_LOOKUP(win, path, length, dest) \
    do { \
        int status = gtk_widget_lookup_path(GTK_CONTAINER(win), (char(*)[WIDGET_ID_MAXLEN]) path, length, &dest); \
        if (SUCCESS != status) {    \
            fprintf(stderr, "ERROR: Widget lookup for " #path ":\n"); \
            for (int i = 0; i < length; i++) { \
                fprintf(stderr, "%s%s", path[i], (i == length - 1 ? "\n":"->")); \
            } \
            fprintf(stderr, "failed with exit code: %d\n", status);    \
            exit(1);    \
        }   \
    } while(0);

extern const char barcode_entry_path[BARCODE_ENTRY_PATH_LENGTH][WIDGET_ID_MAXLEN];
extern const char settings_box_path[SETTINGS_BOX_PATH_LENGTH][WIDGET_ID_MAXLEN];
extern const char page_layout_box_path[PAGE_LAYOUT_BOX_PATH_LENGTH][WIDGET_ID_MAXLEN];
extern const char print_preview_path[PRINT_PREVIEW_PATH_LENGTH][WIDGET_ID_MAXLEN];

extern void *gs_instance;
extern bool gs_initialised;

int gtk_widget_lookup_path(GtkContainer *, char[][WIDGET_ID_MAXLEN], int, GtkWidget **);

int gtk_widget_query_name(GtkContainer *, char *, GtkWidget **);

int gtk_entry_get_text_as_double(GtkEntry *, double *);

int ghostscript_init(void);

int ghostscript_instance(void **, void*, int argc, char **);

int ghostscript_exit(void *);

/**
 *      @brief Checks if a string represents a float
 *      @param str A string to be tested on
 *      @param len The length of str
 *      @return true or false
 */
bool isfloat(char *);

#endif
