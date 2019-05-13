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
 *      @file error.h
 *      @brief Error macros
 *      @author Elijah Schutz
 *      @date 23/3/19
 */

#ifndef ERROR_H
#define ERROR_H

/**
 *      @defgroup Errors Error numbers returned by functions
 */
/*@{*/
// clang-format off
#define SUCCESS                             0
#define ERR_GENERIC                         1
#define ERR_WIDGET_NOT_FOUND                2
#define ERR_NO_WIDGET_CHILDREN              3
#define ERR_INVALID_STRING                  4
#define ERR_TEMPORARY_DIR_CREATION_FAILED   5
#define ERR_TEMPORARY_FILE_CREATION_FAILED  6
#define ERR_CHDIR_FAILED                    7
#define ERR_FTS_ERROR                       8
#define GHOSTSCRIPT_ERR_GENERIC             -1
/*@}*/

// clang-format on
/**
 *      @brief Verify if a pointer is null (originally from libbarcode)
 *      @detail If a null pointer is supplied, an error message is printed and the program exits.
 *      @param var A pointer to check
 *      @param n The size of the memory allocated to the pointer (used in the error message)
 */
// clang-format off
#define VERIFY_NULL_G(var, n)                                                                   \
    do {                                                                                        \
        if (var == NULL) {                                                                      \
            fprintf(                                                                            \
                stderr,                                                                         \
                "null pointer: could not allocate %lu bytes for variable \"%s\" at %s:%d in %s",\
                (unsigned long) n,                                                              \
                #var,                                                                           \
                __FILE__,                                                                       \
                __LINE__ - 8,                                       /* 8 lines up until there */\
                __func__);                                                                      \
            exit(-1);                                                                           \
        }                                                                                       \
    } while(0)                                                                                  \
// clang-format on

#endif
