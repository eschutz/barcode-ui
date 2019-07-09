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
 *      @file util.c
 *      @brief Utility function definitions as defined in util.h
 *      @author Elijah Schutz
 *      @date 23/3/19
 */

#include "backend.h"
#include "error.h"
#include "util.h"
#include <ctype.h>
#include <stdbool.h>

#if _WIN32
#include <string.h>
#endif

// clang-format off
const char barcode_entry_path[BARCODE_ENTRY_PATH_LENGTH][WIDGET_ID_MAXLEN]
    = {"content_box", "barcode_entry_box"};

const char settings_frame_path[SETTINGS_FRAME_PATH_LENGTH][WIDGET_ID_MAXLEN]
    = {"content_box", "right_box", "settings_frame"};

const char settings_box_path[SETTINGS_BOX_PATH_LENGTH][WIDGET_ID_MAXLEN]
    = {"content_box", "right_box", "settings_frame", "settings_box"};

const char ui_hint_view_path[UI_HINT_VIEW_PATH_LENGTH][WIDGET_ID_MAXLEN]
    = {"content_box", "right_box", "ui_hint_view"};

const char page_layout_box_path[PAGE_LAYOUT_BOX_PATH_LENGTH][WIDGET_ID_MAXLEN]
    = {"page_layout_box"};

const char printer_combo_box_path[PRINTER_COMBO_BOX_PATH_LENGTH][WIDGET_ID_MAXLEN]
= {"content_box", "right_box", "printer_box", "printer_combo_box"};
// clang-format on

/**
 *      @details Recursively searches containers to find the widget at the search path.
 */
// clang-format off
int gtk_widget_lookup_path(
    GtkContainer *container,
    char path[][WIDGET_ID_MAXLEN],
    int pathlen,
    GtkWidget **dest
) {
    // clang-format on

    GtkContainer *parent = container;
    for (int i = 0; i < pathlen; i++) {
        GtkWidget *child;
        int        status = gtk_widget_query_name(parent, path[i], &child);

        if (SUCCESS != status) {
            return status;
        } else if (child == NULL) {
            return ERR_WIDGET_NOT_FOUND;
        } else {
            if (i != pathlen - 1 && GTK_IS_CONTAINER(child)) {
                parent = GTK_CONTAINER(child);
            } else {
                if (i == pathlen - 1) {
                    *dest = child;
                } else {
                    return ERR_NO_WIDGET_CHILDREN;
                }
            }
        }
    }
    return SUCCESS;
}

/**
 *      @details Linear search (a small number of children is expected) through the children of a
 *              GTK container. This is a linked list (GList): children->data contains the child
 *              widget, children->data contains the next child.
 */
int gtk_widget_query_name(GtkContainer *container, char *name, GtkWidget **dest) {
    GList *children = gtk_container_get_children(container);

    do {
        if (NULL == children) {
            return ERR_WIDGET_NOT_FOUND;
        }

        GtkWidget *widget = children->data;
        if (strcmp(gtk_widget_get_name(widget), name) == 0) {
            *dest = widget;
            return SUCCESS;
        } else {
            children = children->next;
        }

    } while (true);
}

/**
 *      @details Uses atof() to convert the GtkEntry text to a double (checking if it is a valid
 *              float literal first)
 */
int gtk_entry_get_text_as_double(GtkEntry *entry, double *dest) {
    int   status = SUCCESS;
    int   len    = sizeof(char) * (gtk_entry_get_max_length(entry) + 1);
    char *text   = calloc(1, len);
    VERIFY_NULL_BC(text, len);

    strncpy(text, gtk_entry_get_text(entry), len);
    if (isfloat(text)) {
        *dest = atof(text);
    } else {
        status = ERR_INVALID_STRING;
    }
    free(text);
    return status;
}

/**
 *      @details Simple lexical analysis to verify if the input represents a decimal literal matching
 *              @c /[0-9](.[0-9])?/
 */
bool isfloat(char *str) {
    bool decimal = false;
    int  i       = 0;
    while (str[i] != '\0') {
        if (str[i] == '.') {
            if (decimal) {
                return false;
            } else {
                decimal = true;
            }
        } else if (!isdigit(str[i])) {
            return false;
        }
        i++;
    };
    return true;
}


#if _WIN32
char * strsep(char ** stringp, const char * delim) {
    if (*stringp != NULL && (*stringp = strpbrk(*stringp, delim)) != NULL) {
        *stringp = '\0';
        (*stringp)++;
    }

    return *stringp;
}
#endif
