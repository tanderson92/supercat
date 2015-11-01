/* 
    SuperCat - The Super Cat Colorizer
    Copyright (C) 2007-2008 - Thomas G. Anderson

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

#include "spc.h"

enum { BLK='0', RED, GRN, YEL, BLU, MAG, CYA, WHI };

typedef struct spc_s spc_t;
struct spc_s {
    regex_t buffer;  /* struct for regexp */
    char   *string;  /* string for strchr, strstr */
    char   *htmrgb;  /* html color codes RGB */
    int     number;  /* 1-9, 0:all */
    size_t  htmlen;  /* length of htmrgb string */
    char    aryidx;  /* array index 1-255, 0=default */
    char    colour;  /* enum */
    char    attrib;  /* -, b, i, u */
    char    patype;  /* c, s, r, t */
};

spc_t Spc [NREGEX+1];   /* 0 is default */
int   Spn = 1;
char  Att;
char  Col;

int   opt_debug = 0;        /* multi-level */
int   opt_reverse = 0;      /* 0:whi on blk, 1:blk on whi */
int   opt_line_number = 0;
int   opt_matching = 0;
int   opt_webpage = 0;
int   opt_nohomerc = 0;
int   opt_nocwdrc = 0;
char *opt_config = 0;
char *opt_directory = 0;
char *opt_type = 0;
char *inp_file = 0;
int   inp_lineno = 0;
char *typ_suffix = 0;

void
help ()
{
    puts ("usage: spc [OPTIONS] [FILE]");
    puts ("");
    puts ("  -c <CFG>, --config=CFG       specify non-standard spcrc config file");
    puts ("  -C <DIR>, --config-dir=DIR   specify non-standard config directory");
    puts ("  -d,       --debug            turn on debug information");
    puts ("  -D,       --no-extension     don't look at ./.spcrc[-ext] for config file");
    puts ("  -h,       --help             print this help and exit");
    puts ("  -m,       --matching         print only lines that have matches");
    puts ("  -n,       --line-numbers     precede each line with a line number");
    puts ("  -r,       --reverse          reverse black and white");
    puts ("  -R,       --nohomedir        don't look in ~/.spcrc for config files");
    puts ("  -S,       --printsysdir      printf system directory and exit");
    puts ("  -t <TYP>, --type=TYP         specify the effective file type extension");
    puts ("  -v,       --version          print version information and exit");
    puts ("  -w,       --webpage          generate web page");
    puts ("");
    puts ("Report bugs to <bug-spc@nosredna.net>");
    exit (0);
}

void
version ()
{
    printf ("spc (The SuperCat Colorizer) Version %s February 15, 2008", VERSION);
    puts   ("");
    puts   ("Copyright (C) 2007-2008 - Thomas G. Anderson");
    puts   ("This is free software; see the source for copying conditions.  There is NO");
    puts   ("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.");
    exit   (0);
}

void
sysdir ()
{
    printf ("system directory is %s\n", SYSTEM_DIRECTORY);
    exit (0);
}

void
err_doit (int errnoflag, int error, const char *fmt, va_list ap)
{
	char buf [BUFSIZ];

	vsnprintf (buf, BUFSIZ, fmt, ap);
	if (errnoflag)
		snprintf (buf + strlen(buf), BUFSIZ - strlen(buf), ": %s",
		strerror (error));
	strcat (buf, "\n");
	fflush (stdout);
	fputs  (buf, stderr);
	fflush (NULL);
}

void
err_quit (const char *fmt, ...)
{
	va_list ap;

	va_start (ap, fmt);
	err_doit (0, 0, fmt, ap);
	va_end   (ap);
	exit (-1);
}

void
err_sys (const char *fmt, ...)
{
	va_list ap;

	va_start (ap, fmt);
	err_doit (1, errno, fmt, ap);
	va_end   (ap);
	exit (-1);
}
#ifdef HAVE_GETOPT_H
void
options (int ac, char **av)
{
    int c;

    static struct option opt_tbl [] = {
        {"config",        1, 0, 'c' },  /* specify spcrc config file */
        {"config-dir",    1, 0, 'C' },  /* non-standard config directory */
        {"debug",         0, 0, 'd' },  /* turn on debug */
        {"no-extension",  0, 0, 'D' },  /* don't do extension scraping */
        {"help",          0, 0, 'h' },  /* helpful information and exit */
        {"matching",      0, 0, 'm' },  /* only show matching portions of a line */
        {"line-numbers",  0, 0, 'n' },  /* pre-pend line numbers */
        {"reverse",       0, 0, 'r' },  /* reverse black and white */
        {"nohomedir",     0, 0, 'R' },  /* don't check home directory */
        {"printsysdir",   0, 0, 'S' },  /* print system directory location */
        {"type",          1, 0, 't' },  /* specify effective file extension */
        {"version",       0, 0, 'v' },  /* show version and exit */
        {"webpage",       0, 0, 'w' }   /* generate html output */
    };


    while (1) {
        int opt_idx = 0;

        switch ((c = getopt_long (ac, av, "c:C:dDhmnrRSt:vw", opt_tbl, &opt_idx))) {
          case -1:
            goto DONE;

          case 0:
            printf ("option %s", opt_tbl [opt_idx].name);
            if (optarg)
                printf (" with arg %s", optarg);
            printf ("\n");
            break;

          case 'd': opt_debug++; break;
          case 'D': opt_nocwdrc = 1; break;
          case 'h': help (); break;
          case 'm': opt_matching = 1; break;
          case 'n': opt_line_number = 1; break;
          case 'r': opt_reverse = 1; break;
          case 'R': opt_nohomerc = 1; break;
          case 'S': sysdir (); break; 
          case 'v': version (); break;
          case 'w': opt_webpage = 1; break;

          case 'c':
            opt_config = malloc (strlen (optarg)+1);
            strcpy (opt_config, optarg);
            break;

          case 'C':
            opt_directory = malloc (strlen (optarg)+1);
            strcpy (opt_directory, optarg);
            break;

          case 't':
            opt_type = malloc (strlen (optarg)+1);
            strcpy (opt_type, optarg);
            break;

          case '?':
            break;

          default:
            printf ("getopt returned character code 0x%02X ??\n", (unsigned char) c);
        }
    }

DONE:
    if (optind < ac) {
        if (opt_debug) printf ("non-option ARGV-elements: %s\n", av [optind]);
        inp_file = malloc (strlen (av [optind])+1);
        strcpy (inp_file, av [optind]);
    }
}
#else

void
options (int ac, char **av)
{
    int   i;
    char *p;

    for (i=1; i<ac; i++) {
        if (!strcmp (av [i], "-h") || !strcmp (av [i], "--help"))           help ();
        if (!strcmp (av [i], "-v") || !strcmp (av [i], "--version"))        version ();
        if (!strcmp (av [i], "-S") || !strcmp (av [i], "--printsysdir"))    sysdir ();
        if (!strcmp (av [i], "-d") || !strcmp (av [i], "--debug"))        { opt_debug++;         continue; }
        if (!strcmp (av [i], "-D") || !strcmp (av [i], "--no-extension")) { opt_nocwdrc = 1;     continue; }
        if (!strcmp (av [i], "-m") || !strcmp (av [i], "--matching"))     { opt_matching = 1;    continue; }
        if (!strcmp (av [i], "-n") || !strcmp (av [i], "--line-numbers")) { opt_line_number = 1; continue; }
        if (!strcmp (av [i], "-r") || !strcmp (av [i], "--reverse"))      { opt_reverse = 1;     continue; }
        if (!strcmp (av [i], "-R") || !strcmp (av [i], "--nohomedir"))    { opt_nohomerc = 1;    continue; }
        if (!strcmp (av [i], "-w") || !strcmp (av [i], "--webpage"))      { opt_webpage = 1;     continue; }

        if (!strcmp (av [i], "-c")) {
            if (!av[++i]) err_quit ("missing parameter to -c");
            opt_config = malloc (strlen (av[i])+1);
            strcpy (opt_config, av [i]);
            continue;
        }
        if (!strncmp (av [i], "--config", strlen ("--config"))) {
            if (*(p = av [i] + strlen ("--config")) != '=')
                err_quit ("invalid character in --config");
            if (!*(++p))
                err_quit ("missing parameter to --config");
            opt_config = malloc (strlen (p)+1);
            strcpy (opt_config, p);
            continue;
        }
        if (!strcmp (av [i], "-C")) {
            if (!av[++i]) err_quit ("missing parameter to -C");
            opt_directory = malloc (strlen (av[i])+1);
            strcpy (opt_directory, av [i]);
            continue;
        }
        if (!strncmp (av [i], "--config-dir", strlen ("--config-dir"))) {
            if (*(p = av [i] + strlen ("--config-dir")) != '=')
                err_quit ("invalid character in --config-dir");
            if (!*(++p))
                err_quit ("missing parameter to --config-dir");
            opt_directory = malloc (strlen (p)+1);
            strcpy (opt_directory, p);
            continue;
        }
        if (!strcmp (av [i], "-t")) {
            if (!av[++i]) err_quit ("missing parameter to -t");
            opt_type = malloc (strlen (av[i])+1);
            strcpy (opt_type, av [i]);
            continue;
        }
        if (!strncmp (av [i], "--type", strlen ("--type"))) {
            if (*(p = av [i] + strlen ("--type")) != '=')
                err_quit ("invalid character in --type");
            if (!*(++p))
                err_quit ("missing parameter to --type");
            opt_type = malloc (strlen (p)+1);
            strcpy (opt_type, p);
            continue;
        }
        if (i == (ac-1)) {
            inp_file = malloc (strlen (av [i])+1);
            strcpy (inp_file, av [i]);
            continue;
        }
        err_quit ("unknown option %s", av [i]);
    }
}
#endif

void
obtain_suffix ()
{
    char    *p;

    /*
     *  if we are using stdin then we won't have any file.
     */
    if (!inp_file) return;

    typ_suffix = malloc (strlen (inp_file)+1);
    strcpy (typ_suffix, inp_file);

    /*
     *  Find the file suffix from the end of the string,
     *  back to either a '.' or if a '/' is found then it
     *  is a file name, that's ok.  this means that "xx.c"
     *  becomes "c" and that ".../file" becomes "file".
     */
    p = &typ_suffix [strlen (typ_suffix)-1];
    while (*p != '.' && *p != '/' && p != typ_suffix) p--;
    if (*p == '.' || *p == '/') p++;

    /*
     *  However, if the suffix or file name is "log", we
     *  want to try and go back to the beginning to see if
     *  we can find a more definitive name.  That means
     *  that "Xorg.0.log" becomes "Xorg".
     */
    if (!strcmp (p, "log") && p != typ_suffix) {
        p--;
        while (*p != '/' && p != typ_suffix) {
            if (*p == '.') *p = 0;
            p--;
        }
        if (*p == '.' || *p == '/') p++;
    }

    strcpy (typ_suffix, p);
}

FILE *
config_check (char *file)
{
    struct stat status_buffer [1];
    FILE     *rc;

    if (opt_debug) printf ("trying config file: %s\n", file);

    if (stat (file, status_buffer) == -1)  return (0);
    if (!S_ISREG (status_buffer->st_mode)) return (0);
    if (!(rc = fopen (file, "r")))         return (0);

    if (opt_debug) printf ("using  config file: %s\n", file);

    return (rc);
}

FILE *
config_search (char *file)
{
    char    string [1024];
    char   *home = getenv ("HOME");
    FILE   *rc;

    if (!opt_nocwdrc) {
        sprintf (string, "./.%s", file);
        if ((rc = config_check (string)))     return (rc);
        if ((rc = config_check ("./.spcrc"))) return (rc);
    }

    if (home && !opt_nohomerc) {
        sprintf (string, "%s/.spcrc/%s", home, file);
        if ((rc = config_check (string))) return (rc);
        sprintf (string, "%s/.spcrc/spcrc", home);
        if ((rc = config_check (string))) return (rc);
    }

    if (opt_directory) {
        sprintf (string, "%s/%s", opt_directory, file);
        if ((rc = config_check (string))) return (rc);
        sprintf (string, "%s/spcrc", opt_directory);
        if ((rc = config_check (string))) return (rc);
    }

    sprintf (string, "%s/%s", SYSTEM_DIRECTORY, file);
    if ((rc = config_check (string))) return (rc);
    sprintf (string, "%s/spcrc", SYSTEM_DIRECTORY);
    if ((rc = config_check (string))) return (rc);

    if (opt_debug)
        err_quit ("can't find a config file");
    else
        err_quit ("can't find a config file, try using -d to show where I'm checking");
}

FILE *
configure ()
{
    FILE    *rc;
    char    *home, *temp;

    /*
     *  Open *conf_file, ./.spcrc, ~/.spcrc, /etc/spcrc
     */
    if (opt_config) {
        if (!strncmp (opt_config, "~/", 2)) {
            if (!(home = getenv ("HOME")))
                err_quit ("can't find home directory");
            temp = malloc (strlen (home)+strlen (opt_config)+1);
            strcpy (temp, home);
            strcat (temp, opt_config+1);
        } else
            temp = opt_config;

        if (!(rc = config_check (temp)))
            err_quit ("can't open config file %s", temp);
        free (temp);
        return (rc);
    }

    /*
     *  look for spcrc-type in the standard locations
     */
    if (opt_type) {
        opt_config = malloc (strlen (opt_type)+7);
        sprintf (opt_config, "spcrc-%s", opt_type);
        return  (config_search (opt_config));
    }

    obtain_suffix ();

    /*
     *  look for spcrc-suffix in the standard locations
     */
    if (typ_suffix) {
        opt_config = malloc (strlen (typ_suffix)+7);
        sprintf (opt_config, "spcrc-%s", typ_suffix);
        return  (config_search (opt_config));
    }

    err_quit ("can't find a config file");
}

char
colour_lookup (char *str)
{
    if (!strcmp (str, "blk")) return (BLK);
    if (!strcmp (str, "red")) return (RED);
    if (!strcmp (str, "grn")) return (GRN);
    if (!strcmp (str, "yel")) return (YEL);
    if (!strcmp (str, "blu")) return (BLU);
    if (!strcmp (str, "mag")) return (MAG);
    if (!strcmp (str, "cya")) return (CYA);
    if (!strcmp (str, "whi")) return (WHI);
    err_quit ("unknown color %s at line number %d", str, inp_lineno);
}

int
number_lookup (char chr)
{
    if (chr == ' ') return (999);
    if (chr == '0') return (999);
    if (chr == '1') return (1);
    if (chr == '2') return (2);
    if (chr == '3') return (3);
    if (chr == '4') return (4);
    if (chr == '5') return (5);
    if (chr == '6') return (6);
    if (chr == '7') return (7);
    if (chr == '8') return (8);
    if (chr == '9') return (9);
    err_quit ("unknown number %c at line number %d", chr, inp_lineno);
}

char
attrib_lookup (char chr)
{
    if (chr == '-') return ('0');
    if (chr == ' ') return ('0');
    if (chr == 'b') return ('1');
    if (chr == 'u') return ('4');
    if (chr == 'k') return ('5');
    if (chr == 'r') return ('7');
    err_quit ("unknown attribute %c at line number %d", chr, inp_lineno);
}

char
patype_lookup (char chr)
{
    if (chr == ' ') return (PATYPE_REGEXP);
    if (chr == 'c') return (PATYPE_CHARS);
    if (chr == 'r') return (PATYPE_REGEXP);
    if (chr == 's') return (PATYPE_STRING);
    if (chr == 't') return (PATYPE_UNIXTM);
    err_quit ("unknown pattern type %c at line number %d", chr, inp_lineno);
}

int
colour_str (spc_t *spc, char *string, char *aryidx, int n)
{
    int      i, j, rtn=0, t=spc->number;
    int      m = strlen (spc->string);
    char    *p = string;
    char    *q;

    /*
     *  This function moves a pointer 'p' though a string
     *  using strstr() to find the specified sub-string.
     *  After coloring the matched substring the pointer
     *  is bumped past the sub-string and the search
     *  continues.
     */
    while ((q = strstr (p, spc->string)) && t--) {
        i = q - string;
        rtn = 1;
        for (j=0; j<m; j++, i++) {
            aryidx [i] = spc->aryidx;
        }
        p = q + m;
    }

    return (rtn);
}

int
colour_chr (spc_t *spc, char *string, char *aryidx, int n)
{
    int     i, rtn=0, t=spc->number;

    for (i=0; i<n; i++) {
        if (spc->string [string [i]]) {
            rtn = 1;
            aryidx [i] = spc->aryidx;
            if (!--t) break;
        }
    }
    return (rtn);
}

/*
 *  The theory behind this function is the use of parenthesized REs
 *  to select the text to be colored.  For example, take the RE
 *          h(e)l(l)o
 *  This would create 3 matches from regexec(), one for the entire
 *  string "hello", one for the first parenthsized "e" and one for
 *  the parenthsized "l".  The code ignores the first match and
 *  begins with the second match.
 */
int
colour_rex (spc_t *spc, char *string, char *aryidx, int n)
{
    struct tm  *tm;
    regmatch_t  pmatch [NMATCH], *m;
    int         nmatch= NMATCH;
    int         flag = 0;
    time_t      unixtm;
    int         i, j, ini, rtn=0, t=spc->number;
    char       *p=string, *q;

    while (t--) {
        /*
         *  Match against supplied string.  Pointer 'p' points to
         *  the current start of string.  This pointer is moved
         *  through the input string, being bumped past each of
         *  the matches since regexec only returns the first of
         *  possibly multiple matches within a string.  The t--
         *  above allows control of how many matches to allow.
         */
        memset (pmatch, 0, sizeof (pmatch));
        if (regexec (&spc->buffer, p, nmatch, pmatch, flag))
            return (rtn);

        rtn = 1;
        ini = 0;
        flag = REG_NOTBOL;

        /*
         *  Set the colour array index associated with each match.
         *  Start at the second entry since the first one is for the
         *  entire match while the rest are for parenthesized REs.
         */
        for (m=pmatch+1; m->rm_so != (regoff_t) -1; m++) {
            j = p - string;

            for (i=m->rm_so+j; i<m->rm_eo+j; i++)
                aryidx [i] = spc->aryidx;

            /*
             *  This hack converts a 10-digit Unix time-since-epoch
             *  into a human-readable format MMDDHHMMSS within the
             *  same 10 characters so the string stays the same size.
             */
            if (spc->patype == PATYPE_UNIXTM) {
                if ((m->rm_eo - m->rm_so) != 10) continue;
                unixtm = 0;
                for (i=m->rm_so+j; i<m->rm_eo+j; i++) {
                    unixtm *= 10;
                    unixtm += string [i] - '0';
                }
                tm = localtime (&unixtm);
                i = m->rm_so;
                tm->tm_mon++;
                string [i++] = (tm->tm_mon  / 10) + '0' ;
                string [i++] = (tm->tm_mon  % 10) + '0' ;
                string [i++] = (tm->tm_mday / 10) + '0' ;
                string [i++] = (tm->tm_mday % 10) + '0' ;
                string [i++] = (tm->tm_hour / 10) + '0' ;
                string [i++] = (tm->tm_hour % 10) + '0' ;
                string [i++] = (tm->tm_min  / 10) + '0' ;
                string [i++] = (tm->tm_min  % 10) + '0' ;
                string [i++] = (tm->tm_sec  / 10) + '0' ;
                string [i++] = (tm->tm_sec  % 10) + '0' ;
            }
            if (opt_debug > 1) {
                for (q=string, i=0; *q; q++) {
                    if (i++ == m->rm_so) putchar ('<');
                                         putchar (*q );
                    if (i   == m->rm_eo) putchar ('>');
                }
                putchar ('\n');
            }
        }
        p += pmatch->rm_eo;

    }
    return (rtn);
}

int
colour_sca (char *string, char *aryidx, int n)
{
    int         i, r, rtn=0;
    char       *p;
    spc_t      *spc;

    memset (aryidx, 0, n);

    /*
     *  Process each converted entry from the config file
     *  using the appropriate matching algorithm.  The 'rtn'
     *  variable is used to pass back information about
     *  whether a match was found or not for subsequent use
     *  in conjunction with 'opt_matching'.
     */
    for (spc=Spc, r=0; r<Spn; r++, spc++) {

        if (spc->patype == PATYPE_REGEXP)
            rtn |= colour_rex (spc, string, aryidx, n);
        else if (spc->patype == PATYPE_STRING)
            rtn |= colour_str (spc, string, aryidx, n);
        else if (spc->patype == PATYPE_CHARS)
            rtn |= colour_chr (spc, string, aryidx, n);
        else    /* PATYPE_UNIXTM */
            rtn |= colour_rex (spc, string, aryidx, n);

    }

    if (opt_debug > 3) {
        p = string; for (i=0; i<n; i++) putchar (*p++); putchar ('\n');
        p = aryidx; for (i=0; i<n; i++) putchar (*p++); putchar ('\n');
    }

    return (rtn);
}

char *
printf_htm_entity (char *b, char *p)
{
    switch (*p) {
      case '<': strcpy (b, "&lt;"); b += 4; break;
      case '>': strcpy (b, "&gt;"); b += 4; break;
      case '"': strcpy (b, "&quot;"); b += 6; break;
      case '\'': strcpy (b, "&apos;"); b += 6; break;
      case '&': strcpy (b, "&amp;"); b += 5; break;
      default: *b++ = *p; break;
    }
    return (b);
}

void
printf_htm (char *string, char *aryidx, int n)
{
    char   output [BUFFER*4];
    char  *p = string;
    char  *q = aryidx;
    char  *b = output;
    spc_t *spc;
    int    i, ini=0;

    Col = -1;
    *b  =  0;
    for (i=0; i<n; i++, p++, q++) {
        spc = &Spc [*q];
        if (spc->colour != Col) {
            if (ini) {
                strcpy (b, "</font>");
                b += 7;
            }
            strcpy (b, "<font color=\""); b += 13;
            strcpy (b, spc->htmrgb);      b += spc->htmlen;
            strcpy (b, "\">");            b += 2;
            Col = spc->colour;
        }

        b = printf_htm_entity (b, p);
        ini = 1;
    }
    if (n) {
        strcpy (b, "</font>");
        b += 11;
    }
    *b++ = 0;

    if (opt_line_number) printf ("%5d: ", inp_lineno);
    puts (output);
}

void
printf_con (char *string, char *aryidx, int n)
{
    char   output [BUFFER*2];
    char  *p = string;
    char  *q = aryidx;
    char  *b = output;
    spc_t *spc;
    int    i;

    *b++ = 0x1B;
    *b++ = '[';
    *b++ = Att = Spc->attrib;
    *b++ = ';';
    *b++ = '3';
    *b++ = Col = Spc->colour;
    *b++ = 'm';
    for (i=0; i<n; i++, q++) {
        spc = &Spc [*q];
        if (spc->colour != Col || spc->attrib != Att) {
            *b++ = 0x1B;
            *b++ = '[';
            if (spc->attrib != Att) {
                *b++ = Att = spc->attrib ;
                if (spc->colour != Col)
                    *b++ = ';';
            }
            if (spc->colour != Col) {
                *b++ = '3';
                *b++ = Col = spc->colour ;
            }
            *b++ = 'm';
        }
        *b++ = *p++;
    }
    *b++ = 0x1B;
    *b++ = '[';
    *b++ = Spc->attrib;
    *b++ = ';';
    *b++ = '3';
    *b++ = Spc->colour;
    *b++ = 'm';
    *b++ = 0;

    if (opt_line_number) printf ("%5d: ", inp_lineno);
    puts (output);
}

void
create_rex (FILE *fp)
{
    char        buffer [BUFFER], *p;
    int         n;
    spc_t      *spc;

    memset (Spc, 0, sizeof (Spc));
    spc = Spc;
    spc->aryidx = 0;
    spc->colour = WHI;
    spc->attrib = '0';
    spc->htmrgb = (opt_reverse) ? "Black" : "White" ;
    spc->htmlen = strlen (spc->htmrgb);
    spc++;

    /*
     *  This loop reads each line of the config file and
     *  processes it according to its pattern type field.
     *  If it is a RE ('r' or 't') it is compiled.  If it
     *  is a simple character set ('c') or string ('s')
     *  the strings are just stored.
     */
    while (fgets (buffer, sizeof (buffer), fp)) {
        n = strlen (buffer);
        if (n == BUFFER)
            err_quit ("line too long: %s", buffer);

        inp_lineno++;

        buffer [n-1] = 0;
        buffer [COLUMN_COLOUR+3] = 0;

        if (*buffer == '#') continue;   /* comment */
        if (*buffer ==  0 ) continue;   /* blank line */

        if (buffer [COLUMN_HTMRGB] == ' ')
            strcpy (&buffer [COLUMN_HTMRGB], (opt_reverse) ? "Black" : "White");
        else {
            p = &buffer [COLUMN_HTMRGB];
            while (*p != ' ') p++ ;
            *p = 0;
        }
        spc->aryidx = Spn;
        spc->colour = colour_lookup (&buffer [COLUMN_COLOUR]);
        spc->attrib = attrib_lookup ( buffer [COLUMN_ATTRIB]);
        spc->number = number_lookup ( buffer [COLUMN_NUMBER]);
        spc->patype = patype_lookup ( buffer [COLUMN_PATYPE]);
        
        spc->htmrgb = malloc (strlen (&buffer [COLUMN_HTMRGB]) + 1);
        strcpy (spc->htmrgb,&buffer [COLUMN_HTMRGB]);
        spc->htmlen = strlen (spc->htmrgb);

        if (spc->patype == PATYPE_REGEXP ||
            spc->patype == PATYPE_UNIXTM) {
            /*
             *  Compile the regular expression
             */
            if ((n = regcomp (&spc->buffer, buffer+COLUMN_CREGEX, REG_EXTENDED))) {
                fprintf (stderr, "regcomp error: %s\n", buffer+COLUMN_CREGEX);
                regerror (n, &spc->buffer, buffer, sizeof (buffer));
                err_quit ("%s", buffer);
            }
        } else if (spc->patype == PATYPE_CHARS)  {
            /*
             *  Allocate a 256 byte array, one for each ASCII
             *  character and set each location to 1 that has
             *  a corresponding character in the input string.
             */
            spc->string = malloc (256);
            memset (spc->string, 0, 256);
            n=strlen (buffer+COLUMN_CREGEX-1);
            for (p=buffer+COLUMN_CREGEX; *p; p++)
                spc->string [*p] = 1;
        } else {
            /*
             *  just save the string expression
             */
            spc->string = malloc (strlen (buffer+COLUMN_CREGEX-1));
            strcpy (spc->string, buffer+COLUMN_CREGEX);
        }
        Spn++;
        spc++;
    }
    if (Spn == 1)
        err_quit ("config file %s is invalid", opt_config);
}

void
initialize_con ()
{
    putchar (0x1B);
    putchar ('[');
    putchar (Att = Spc->attrib);
    putchar (';');
    putchar ('3');
    putchar (Col = Spc->colour);
    putchar ('m');
}

void
finalize_con ()
{
    putchar (0x1B);
    putchar ('[');
    putchar ('0');
    putchar (';');
    putchar ('m');
}

void
initialize_htm ()
{
    puts   ("<html>");
    puts   ("<head>");
    puts   ("<style type=\"text/css\">");
    puts   ("body { font-family: courier; white-space: nowrap }");
    puts   ("</style>");
    printf ("<title>Colorized by supercat %s</title>\n", VERSION);
    puts   ("</head>");
    printf ("<body bgcolor=\"%s\">\n", (opt_reverse) ? "White" : "Black" );
    puts   ("<pre>");
}

void
finalize_htm ()
{
    puts   ("</pre>");
    puts   ("</body>");
    puts   ("</html>");
}

FILE *
input_open ()
{
    FILE  *in = stdin;
    struct stat status_buffer [1];

    if (inp_file) {
        if (stat (inp_file, status_buffer) == -1)
            err_sys ("can't stat input file %s", inp_file);
        if (!S_ISREG (status_buffer->st_mode))
            err_quit ("input file %s is not a regular file", inp_file);
        if (!(in = fopen (inp_file, "r")))
            err_sys ("can't open input file %s", inp_file);
    }

    return (in);
}

int
main (int ac, char *av[])
{
    char        string [BUFFER];
    char        aryidx [BUFFER];
    FILE       *in;
    int         rtn, n;

    options (ac, av);

    create_rex (configure ());

    in = input_open ();

    if (opt_webpage)
        initialize_htm ();
    else
        initialize_con ();

    inp_lineno = 0;
    while (fgets (string, sizeof (string), in)) {
        string [n=strlen (string)-1] = 0;
        inp_lineno++;
        rtn = colour_sca (string, aryidx, n);
        if (opt_webpage) {
            if (!opt_matching || rtn)
                printf_htm (string, aryidx, n);
        } else {
            if (!opt_matching || rtn)
                printf_con (string, aryidx, n);
        }
    }

    if (opt_webpage)
        finalize_htm ();
    else
        finalize_con ();

    return (0);
}

