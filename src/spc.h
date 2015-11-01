/* 
    SuperCat - The Super Cat Colorizer
    Copyright (C) 2007-2015 - Mark P. Anderson, Thomas G. Anderson

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#if HAVE_CONFIG_H
# include <config.h>
#endif

#if HAVE_STDIO_H
# include <stdio.h>
#endif

#if HAVE_STDLIB_H
# include <stdlib.h>
#endif

#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#if HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif

#if HAVE_REGEX_H
# include <regex.h>
#endif

#if HAVE_STRING_H
# include <string.h>
#endif

#if HAVE_GETOPT_H
# include <getopt.h>
#endif

#if HAVE_STDARG_H
# include <stdarg.h>
#endif

#if HAVE_ERRNO_H
# include <errno.h>
#endif

#if HAVE_TIME_H
# include <time.h>
#endif

#define NMATCH  16
#define NREGEX  16
#define BUFFER  1024

#define PATYPE_CHARS    'c'
#define PATYPE_STRING   's'
#define PATYPE_REGEXP   'r'	// case   sensitive RE
#define PATYPE_RECASE   'R'	// case insensitive RE
#define PATYPE_UNIXTM   't'

#define COLUMN_HTMRGB  0
#define COLUMN_COLOUR 21 
#define COLUMN_ATTRIB 25
#define COLUMN_NUMBER 27
#define COLUMN_PATYPE 29
#define COLUMN_CREGEX 31

