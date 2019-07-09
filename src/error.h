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
// start at 10 so we don't conflict with barcode.h errors
#define ERR_GENERIC                         1
#define ERR_WIDGET_NOT_FOUND                11
#define ERR_NO_WIDGET_CHILDREN              12
#define ERR_INVALID_STRING                  13
#define ERR_TEMPORARY_FILE_CREATION_FAILED  14
#define ERR_FILE_CLOSE_FAILED               15
#define ERR_FILE_REMOVE_FAILED              16
#define ERR_FILE_RESET_FAILED               17
#define ERR_FILE_WRITE_FAILED               18
#define ERR_POPEN                           19
#define ERR_FREAD                           20
#define ERR_NO_PRINTERS                     21
#define ERR_FORK                            22
#define ERR_FLUSH                           23
#define ERR_SYSTEM                          24
/*@}*/

// clang-format on
/**
 *      @brief Verify if a pointer is null (originally from libbarcode)
 *      @detail If a null pointer is supplied, an error message is printed and the program exits.
 *      @param var A pointer to check
 *      @param n The size of the memory allocated to the pointer (used in the error message)
 */
// clang-format off
#define VERIFY_NULL_BC(var, n)                                                                  \
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
