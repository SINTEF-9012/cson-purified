/**
   Demonstration of using the cson API to parse JSON input.  Note that
   over 75% of this file is "application-level overhead", and only the
   do_parse() function is really relevant.
*/

#include "wh/cson/cson.h"
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <locale.h>

static int my_printfv(char const * fmt, va_list vargs )
{
    vfprintf( stderr, fmt, vargs );
    return 0;
}
static int my_printf(char const * fmt, ...)
{
    int rc = 0;
    va_list vargs;
    va_start(vargs,fmt);
    rc = my_printfv( fmt, vargs );
    va_end(vargs);
    return rc;
}

#if 1
#define MARKER if(1) my_printf("%s: %s:%d:%s():\t",ThisApp.name,__FILE__,__LINE__,__func__); if(1) my_printf
#else
static void noop_printf(char const * fmt, ...) {}
#define MARKER if(0) noop_printf
#endif
#define MESSAGE my_printf("%s:\t",ThisApp.name); my_printf

static struct ThisApp_ {
    char const * name;
    unsigned char indentation;
    char showMetrics;
} ThisApp = {
"unnamed"/*name*/,
1 /*indentation*/,
0 /*showMetrics*/
};

static int do_parse( FILE * inp, FILE * out )
{
    cson_value * root = NULL;
    cson_parse_info info = cson_parse_info_empty;
    int rc;
    assert( inp && out );
    rc = cson_parse_FILE( &root, inp, NULL, &info );
    if( 0 != rc )
    {
        assert( NULL == root );
        MESSAGE("JSON parse error, code=%d (%s), at line %u, column %u.\n",
                info.errorCode, cson_rc_string(rc), info.line, info.col );
        return rc;
    }
    else
    {
        cson_output_opt fopt = cson_output_opt_empty;
        fopt.indentation = ThisApp.indentation;
        fopt.addNewline = 1;
        rc = cson_output_FILE( root, out, &fopt );
        cson_value_free(root);
        fflush(out);
        if( ThisApp.showMetrics )
        {
            MESSAGE("Metrics:\n\tTotal key count: %u\n\tTotal value count: %u\n",
                    info.totalKeyCount, info.totalValueCount);
        }
        return rc;
    }
}

static void show_help(char const * appName)
{
    printf("Usage:\n\n\t%s [-?|--help] [-m] [-f input_filename] [-o output_filename] [-#]\n", appName);
    puts("\n\t(Note the spaces between the flags and their values!)");
    puts("\nThis tool reads JSON input using the cson API, then outputs the resulting DOM "
         "as JSON using the cson API. It is primarily intended for testing libcson.");
    puts("\nThe -m option enables the output of certain metrics collected during parsing.");
    puts("\nThe -# option means to specify a number, e.g. -1 or -4. The number 0 means "
         "to add no extra indentation (maybe a whitespace here and there). A value of 1 "
         "means to use 1 TAB for each level of indentation. A higher value means to use "
         "that many spaces for each level of indentation.");
    puts("\nThe default input source is stdin (same as -f -) and the default output "
         "destination is stdout (same as -o -)");
    puts("\nNote that all output which is not JSON (e.g. error messages or -m metrics) "
         "is sent to stderr.");
    puts("\ncson home page: htpp://fossil.wanderinghorse.net/repos/cson/");
    putchar('\n');

}

int main(int argc, char const ** argv)
{
    FILE * inp = NULL;
    FILE * outp = NULL;
    char const * nameIn = NULL;
    char const * nameOut = NULL;
    int rc = 0;
    int i = 1;
    ThisApp.name = argv[0];
    setlocale( LC_ALL, "C" ) /* supposedly important for underlying JSON parser. */;
    for( ; i < argc; ++i )
    {
        char const * arg = argv[i];
        if( 0 == strcmp("-f",arg) )
        {
            ++i;
            if( i < argc )
            {
                nameIn = argv[i];
                continue;
            }
            else
            {
                show_help(argv[0]);
                return 1;
            }
        }
        else if( 0 == strcmp("-o",arg) )
        {
            ++i;
            if( i < argc )
            {
                nameOut = argv[i];
                continue;
            }
            else
            {
                show_help(argv[0]);
                return 1;
            }
        }
        else if( (0 == strcmp("-?",arg))
                 || (0 == strcmp("--help",arg) ) )
        {
            show_help(argv[0]);
            return 0;
        }
        else if( 0 == strcmp("-m",arg) )
        {
            ThisApp.showMetrics = 1;
        }
        else if( '-' == *arg )
        { /* try -# to set indention level */
            enum { NumBufLen = 16 };
            unsigned short val = 0;
            char buf[NumBufLen];
            int check;
            memset(buf, 0, NumBufLen);
            check = sscanf( arg+1, "%hu", &val );
            if( 1 != check )
            {
                MARKER("Could not parse argument [%s] as a numeric value.\n",arg);
                show_help(argv[0]);
                return 2;
            }
            ThisApp.indentation = (unsigned char)val;
            continue;            
        }
        else
        {
            show_help(argv[0]);
            return 0;
        }
    }
    if( nameIn && (0!=strcmp("-",nameIn)))
    {
        inp = fopen( nameIn, "r" );
        if( ! inp )
        {
            MARKER("Error opening input file [%s]!\n", nameIn );
            return 3;
        }
    }
    else
    {
        inp = stdin;
    }
    if( nameOut && (0!=strcmp("-",nameOut)))
    {
        outp = fopen( nameOut, "w" );
        if( ! outp )
        {
            MARKER("Error opening output file [%s]!\n", nameOut );
            return 4;
        }
    }
    else
    {
        outp = stdout;
    }
    rc = do_parse(inp, outp);
    if( stdin != inp )
    {
        fclose(inp);
    }
    if( stdout != outp )
    {
        fclose(outp);
    }
#if 0
    if( 0 != rc )
    {
        MARKER("NOTE: exiting with error code %d (%s)!\n",
               rc, cson_rc_string(rc) );
    }
#endif
    return rc;
}

#undef MESSAGE
#undef MARKER
