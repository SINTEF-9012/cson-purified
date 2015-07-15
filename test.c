/**
   Test/demo code for the cson library.
*/
#if defined(NDEBUG)
/* force assert() to always work */
#  undef NDEBUG
#endif

#include "wh/cson/cson.h"
#include "parser/JSON_parser.c"
#include "cson.c" /* include the C file directly so we get the private API
                     for testing purpose. Clients should not do this. */

#if 1
#define MARKER if(1) printf("MARKER: %s:%d:%s():\t",__FILE__,__LINE__,__func__); if(1) printf
#else
static void noop_printf(char const * fmt, ...) {}
#define MARKER if(0) printf
#endif

#define SHOWRC(RC) MARKER("rc=%d (%s)\n",(RC),cson_rc_string((RC)))
static void output_value( char const * label, cson_value const * v )
{
    cson_output_opt opt = cson_output_opt_empty;
    opt.indentation = 4;
    //opt.indentSingleMemberValues = 1;
    int rc = 0;
    assert( v );
    printf("%s:\t<",label);
    rc = cson_output_FILE( v, stdout, &opt );
    puts(">");
    printf("Alloced size: %u\n", cson_value_msize(v));
    assert( 0 == rc );
}

#define OV(lbl,val) output_value(lbl, (val))

void test_basic1()
{
    MARKER("Testing non-allocating factories...");
    assert( 0 == cson_value_msize(cson_value_true()));
    assert( 0 == cson_value_msize(cson_value_false()));
    assert( 0 == cson_value_msize(cson_value_null()));
    assert( 0 == cson_value_msize(cson_value_new_string(NULL,0)));
    assert( 0 == cson_value_msize(cson_value_new_string("",0)));
    assert( 0 == cson_value_msize(cson_new_int(0)));
    assert( 0 == cson_value_msize(cson_new_double(0.0)));

    cson_value * v;
    unsigned int sz;
#define DO(F) v = F; assert(v); \
    assert(0 != (sz=cson_value_msize(v)));       \
    MARKER("Size of %s = %u\n", #F, sz); \
    cson_value_free(v); v = NULL

    DO(cson_new_int(42));
    DO(cson_new_double(42.24));
    DO(cson_value_new_string("hi", 2));
    DO(cson_value_new_array());
    DO(cson_value_new_object());
#undef DO
}

void test_array1()
{
    MARKER("array tests...\n");
    cson_value * arV = cson_value_new_array();
    assert( arV );
    cson_array * ar = cson_value_get_array(arV);
    assert( ar );
    int rc = cson_array_reserve(ar, 10);
    assert( 0 == rc );
    const cson_int_t exI = 42;
    cson_value * v = cson_value_new_integer(exI);
    assert( v );
    unsigned int exLen = 4;
    rc = cson_array_set( ar, exLen-1, v );
    assert( 0 == rc );
    assert( exI == cson_value_get_integer(v) );
    assert( exLen == cson_array_length_get(ar) );

    cson_value * gv = cson_array_get(ar, exLen-1);
    assert( gv && cson_value_is_integer(gv) );
    assert( cson_value_get_integer(gv) == exI );
    gv = cson_array_get(ar, 0);
    assert( cson_value_is_undef(gv) );
    assert( NULL == gv );
    cson_array * ar2 = cson_new_array();
    assert( ar2 );
    cson_value * av2 = cson_array_value(ar2);
    assert( av2 );
    char check = (av2 == cson_array_value(ar2));
    assert( check );
    v = cson_value_new_double( 123.321 );
    assert( v );
    rc = cson_array_set( ar2, 0, v );
    assert( 0 == rc );
    rc = cson_array_set( ar, 0, av2 );
    assert( 0 == rc );
    char const * yo = "updated to yöyöyöyö's latest version";
    cson_string const * s1 = cson_new_string( yo, strlen(yo) );
    assert(s1);
    v = cson_string_value(s1);
    assert( v );
    assert( v->value == s1 );

    rc = cson_array_set( ar2, 1, v );
    assert( 0 == rc );

    
    v = NULL;
    v = cson_array_get( ar, 0 );
    assert( v && (v==av2) );
    assert( cson_value_is_array(v) );
    assert( cson_value_get_array(v) == ar2 );

    OV("array",arV);

    cson_value * arClone = cson_value_clone( arV );
    assert( NULL != arClone );
    assert( NULL != cson_value_get_array(arClone) );
    OV("array clone", arClone);
    MARKER("Original array's alloced size: %u\n", cson_value_msize(arV));
    MARKER("Clone array's alloced size: %u\n", cson_value_msize(arClone));
    assert( cson_array_length_get( ar ) == cson_array_length_get( cson_value_get_array(arClone) ) );
    cson_value_free(arV);
    cson_value_free(arClone);
}

void test_object1()
{
    MARKER("object tests...\n");
    cson_object * obj = cson_new_object();
    cson_value * objV = cson_object_value(obj);
    assert( cson_value_is_object(objV) );
    assert( obj && (obj==objV->value) );
    assert( objV == cson_object_value(obj) );
    int rc;
    cson_value * v = cson_value_new_integer(42);
    assert( v );
    char const * key = "foo";
    rc = cson_object_set( obj, key, v );
    assert( rc == 0 );
    cson_value * v2 = cson_object_get( obj, key );
    assert( v2 == v );

    cson_value * av = cson_value_new_array();
    assert( av );
    cson_array * ar = cson_value_get_array(av);
    assert( ar );
    v = cson_value_new_string( "barre", 3/*yes, 3! Resulting key
                                           must be "bar".*/ );
    assert( v );
    assert( 0 == strcmp("bar",cson_value_get_cstr(v)) );
    rc = cson_array_append( ar, v );
    assert( 0 == rc );
    rc = cson_object_set( obj, "anArray", av );
    assert( 0 == rc );
    rc = cson_object_set( obj, "removeMe", cson_value_true() );
    assert( 0 == rc );
    rc = cson_object_set( obj, "removeMe", NULL );
    assert( 0 == rc );
    int x = 0;
    for( ; x < 3; ++ x )
    {
        rc = cson_array_append( ar, cson_value_new_integer((x+3)*3) );
        assert( 0 == rc );
    }
    assert( 4 == cson_array_length_get(ar) );
    cson_object_set( obj, "arrayRef", av );
    cson_object_set( obj, "anotherRef", av );
    cson_object_set( obj, "anotherRef", NULL );
    if(1)
    {
        cson_buffer buf = cson_buffer_empty;
        cson_output_opt opt = cson_output_opt_empty;
        opt.indentation = 4;
        rc = cson_output_buffer( objV, &buf, &opt );
        assert( 0 == rc );
        assert( buf.used > 10 );
        puts((char const *)buf.mem);
        cson_buffer_reserve(&buf, 0);
        assert( NULL == buf.mem );
    }
    else
    {
        OV("object",objV);
    }

    cson_object_iterator iter;
    rc = cson_object_iter_init( obj, &iter );
    assert( 0 == rc );
    cson_kvp * kvp;
    cson_string const * ckey;
    unsigned const int exCount = 3;
    unsigned int count = 0;
    while( (kvp = cson_object_iter_next(&iter)) )
    {
        ++count;
        ckey = cson_kvp_key(kvp);
        assert(ckey);
        v = cson_kvp_value(kvp);
        assert( v );
        assert( v->api->typeID > CSON_TYPE_NULL );
        MARKER("key=[%s], value=", cson_string_cstr(ckey) );
        cson_output_FILE( v, stdout, NULL );
    }
    assert( count == exCount );

    cson_value * clone = cson_value_clone( objV );
    assert( NULL != clone );
    assert( NULL != cson_value_get_object(clone) );
    OV("object clone", clone);
    cson_value_free(objV);

    obj = cson_new_object();
    objV = cson_object_value(obj);
    assert(objV);
    cson_value * cloneClone = cson_value_clone(clone);
    cson_value * cloneClone2 = cson_value_clone(clone);
    cson_object_set( cson_value_get_object(clone), "selfClone", cloneClone);
    cson_object_set( obj, "selfClone", cloneClone2 );
    cson_object_set( cson_value_get_object(cloneClone), "imported", cson_value_true() );

    cson_object_set( cson_value_get_object(cloneClone2), "iWasHereFirst", cson_value_true() );
    MARKER("Merged objects...\n");
    OV("Source of merge:", clone);
    OV("Dest for merge", objV);
    rc = cson_object_merge( obj, cson_value_get_object(clone), 0 );
    assert(0==rc);
    OV("Post-merge object", objV);
    assert(cson_value_is_bool(cson_object_get_sub2( obj, ".selfClone.imported")));
    cson_value_free(clone);
    cson_free_object(obj);

    
}

void test_parse1()
{

    MARKER("parse test #1...\n");
    cson_value * root = NULL;
    char const * json =
#if 1
        "{"
        "\"k1\":123.321"
        ",\"array\":[5,7.3,{\"a\":{}},[3,2,1]]"
        "}"
#elif 0
        "[3,2,1,{\"k\":123.321}]"
#else
        "[[1,[2]]]"
#endif
        ;
    int rc = cson_parse_string( &root, json, strlen(json), NULL, NULL );
    MARKER("parse rc=%d/%s\n", rc, cson_rc_string(rc) );
    assert( 0 == rc );
    assert( NULL != root );
    OV("parsed",root);
    MARKER("Root node's alloced size: %u\n", cson_value_msize(root));
    cson_value * clone = cson_value_clone(root);
    OV("clone of parsed", clone);
    MARKER("Root's clone's alloced size: %u\n", cson_value_msize(clone));
    cson_value_free(root);
    root = NULL;
#if 0
    MARKER("Root's clone's alloced size (after freeing root): %u\n", cson_value_msize(clone));
#endif
    cson_value_free(clone);
    // FIXME: assert that the contents are what we expect.
}

void test_object_get_sub()
{
    MARKER("cson_object_get_sub() tests...\n");
    int rc;
    cson_value * objV = cson_value_new_object();
    cson_object * obj = cson_value_get_object(objV);
    const cson_int_t exI = 42;
    {
        cson_value * subV1 = cson_value_new_object();
        cson_object * sub1 = cson_value_get_object(subV1);
        assert( subV1 && sub1 );
        cson_value * subV2 = cson_value_new_object();
        cson_object * sub2 = cson_value_get_object(subV2);
        assert( subV2 && sub2 );
        rc = cson_object_set( obj, "sub1", subV1 );
        assert( 0 == rc );
        rc = cson_object_set( sub1, "sub2", subV2 );
        assert( 0 == rc );
        rc = cson_object_set( sub2, "myInt", cson_value_new_integer(exI) );
        assert( 0 == rc );
    }
    cson_value * v = NULL;
    OV("get_sub test:",objV);
    char const * pathDot = ".sub1.sub2.myInt";
    char const * path = pathDot+1;
    rc = cson_object_fetch_sub( obj, &v, path, '.');
    assert( 0 == rc );
    assert( cson_object_get_sub(obj, path, *pathDot ) == v );
    assert( cson_object_get_sub2(obj, pathDot ) == v );
    assert( v );
    assert( cson_value_is_integer(v) );
    assert( exI == cson_value_get_integer(v) );

    rc = cson_object_fetch_sub( obj, &v, path, '.');
    assert( 0 == rc );
    assert( cson_rc.RangeError == cson_object_fetch_sub( obj, &v, "///", '/') );
    assert( cson_rc.RangeError == cson_object_fetch_sub2( obj, &v, "////") );

#if 1
    /*
        These are here to ensure (with valgrind) that these particular
        ops do not allocate memory...
        
        Client code MUST NOT rely on this behaviour: all cson_values
        are to be treated as if they are heap allocated, and freed
        using cson_value_free().
    */
    v = cson_value_true();
    assert( cson_value_get_bool(v) );
    v = cson_value_false();
    assert( !cson_value_get_bool(v) );
    v = cson_value_null();
    assert( !cson_value_get_bool(v) );
    assert( cson_value_true() == cson_value_new_bool(1) );
    assert( cson_value_false() == cson_value_new_bool(0) );
    v = cson_value_new_string("note the 0 length",0);
    assert( cson_value_new_string(NULL,0) == v );
    assert( !cson_value_get_bool(v) );
    v = cson_value_new_integer(0);
    assert( v == cson_value_new_integer(0) );
    v = cson_value_new_double(0.0);
    assert( v == cson_value_new_double(0.0) );
#endif  /* end valgrind check */

    cson_value_free(objV);
}

void test_refcount()
{
    MARKER("some reference counting tests...\n");
    cson_value * arV = cson_value_new_array();
    cson_value * arV2 = cson_value_new_array();
    cson_array * ar = cson_value_get_array( arV );
    cson_array * ar2 = cson_value_get_array( arV2 );
    unsigned int i;
    cson_value * vInt = cson_value_new_integer( 42 );
    cson_refcount_incr(vInt);
    int rc = cson_array_append( ar, arV2 );
    assert( 0 == rc );
    assert( 1 == arV2->refcount );
    assert( 0 == arV->refcount );
    for( i = 0; i < 5; ++i )
    {
        rc = cson_array_append( ar, vInt );
        assert( 0 == rc );
        rc = cson_array_append( ar2, vInt );
        assert( 0 == rc );
    }
    assert( 1 == arV2->refcount );
    assert( 0 == arV->refcount );
    assert( 11 == vInt->refcount );
    cson_string * kStr = cson_new_string("theArray",8);
    cson_value * kVal = cson_string_value(kStr);
    assert( kVal && (kVal->value == kStr) );
    assert( 0 == kVal->refcount );
    //cson_value_add_reference(kVal);
    cson_value * objV = cson_value_new_object();
    cson_object * obj = cson_value_get_object( objV );
    cson_object_set_s( obj, kStr, arV );
    assert( 1 == kVal->refcount );
    assert( 1 == arV->refcount );
    cson_value * objV2 = cson_value_new_object();
    cson_object * obj2 = cson_value_get_object( objV2 );
    cson_object_set_s( obj2, kStr, arV );
    assert( 2 == kVal->refcount );
    assert( 2 == arV->refcount );

    cson_array_append(ar, kVal);
    assert( 3 == kVal->refcount );
    cson_value_free( objV )
        /* check with valgrind that all went well. */
        ;
    assert( 2 == kVal->refcount );
    assert( 1 == arV->refcount );
    cson_value_free( objV2 );
    assert( 1 == vInt->refcount );
    cson_value_free( vInt );
    MARKER("refcounting tests done.\n");
}

void test_args_parse(int argc, char const * const * argv){

    cson_object * o = NULL;

    int rc = cson_parse_argv_flags(argc,argv, &o, NULL);
    assert(0 == rc);
    assert(NULL != o);

    OV("CLI args",cson_object_value(o));
    cson_free_object(o);
}

/**

Tokenizes an input string on a given separator. Inputs are:

- (inp) = is a pointer to the pointer to the start of the input.

- (separator) = the separator character

- (end) = a pointer to NULL. i.e. (*end == NULL)

This function scans *inp for the given separator char or a NUL char.
Successive separators at the start of *inp are skipped. The effect is
that, when this function is called in a loop, all neighboring
separators are ignored. e.g. the string "aa.bb...cc" will tokenize to
the list (aa,bb,cc) if the separator is '.' and to (aa.,...cc) if the
separator is 'b'.

Returns 0 (false) if it finds no token, else non-0 (true).

Output:

- (*inp) will be set to the first character of the next token.

- (*end) will point to the one-past-the-end point of the token.

If (*inp == *end) then the end of the string has been reached
without finding a token.

Post-conditions:

- (*end == *inp) if no token is found.

- (*end > *inp) if a token is found.

It is intolerant of NULL values for (inp, end), and will assert() in
debug builds if passed NULL as either parameter.
*/
static char wh_str_toker_next_token( char const ** inp, char separator, char const ** end )
{
    char const * pos = NULL;
    assert( inp && end && *inp );
    if( *inp == *end ) return 0;
    pos = *inp;
    if( !*pos )
    {
        *end = pos;
        return 0;
    }
    for( ; *pos && (*pos == separator); ++pos) { /* skip preceeding splitters */ }
    *inp = pos;
    for( ; *pos && (*pos != separator); ++pos) { /* find next splitter */ }
    *end = pos;
    return (pos > *inp) ? 1 : 0;
}

struct wh_str_toker {
    char separator;
    char const * tokenBegin;
    char const * tokenEnd;
    char const * theEnd;
};
typedef struct wh_str_toker wh_str_toker;
#define wh_str_toker_empty_m {' ', 0, 0, 0}
const wh_str_toker wh_str_toker_empty = wh_str_toker_empty_m;

char wh_str_toker_init( wh_str_toker * t, char const * begin,
                        char const * end, char separator )
{
    if(!t || !begin || (end<=begin) ) return 0;
    else {
        t->tokenBegin = begin;
        t->theEnd = end;
        t->tokenEnd = NULL;
        t->separator = separator;
        return 1;
    }
}

char wh_str_toker_next( wh_str_toker * t, char const ** begin, char const ** end )
{
    if(!t || !begin || ! end || (t->tokenBegin >= t->theEnd)) return 0;
    else {
        t->tokenEnd = NULL;
        if( ! wh_str_toker_next_token( &t->tokenBegin, t->separator, &t->tokenEnd ) ) return 0;
        else {
            assert( t->tokenEnd > t->tokenBegin );
            *begin = t->tokenBegin;
            if(t->tokenEnd > t->theEnd){
                /* read trailing parts... */
                t->tokenEnd = t->theEnd;
            }
            *end = t->tokenEnd;
            t->tokenBegin = t->tokenEnd;
            return 1;
        }
    }
}

void test_toker(){
    wh_str_toker t = wh_str_toker_empty;
    char const * in = "///aA/bB/C////de/F";
    const int expect = 4;
    int count = 0;
    char const * b = NULL;
    char const * e = NULL;
    char rc = wh_str_toker_init( &t, in, in+strlen(in)-3, '/');
    assert(rc);
    while( wh_str_toker_next(&t, &b, &e) ){
        int len = (int)(e-b);
        ++count;
        assert(e>b);
        MARKER("len=%d token=%.*s\n", len, len, b);
        if(4==count){
            assert(1==len && *b=='d');
        }
    }
    assert(expect == count);
}

int main(int argc, char const * const * argv)
{
    test_basic1();
    test_array1();
    test_object1();
    test_parse1();
    test_object_get_sub();
    test_refcount();
    test_args_parse(argc,argv);
    test_toker();
    MARKER("Done!\n");
    return 0;
}
