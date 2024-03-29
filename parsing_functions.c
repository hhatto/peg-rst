/* parsing_functions.c - Functions for parsing reStructuredText and
 * freeing element lists. */

/* These yy_* functions come from rst_parser.c which is
 * generated from rst_parser.leg
 * */
typedef int (*yyrule)();

extern int yyparse();
extern int yyparsefrom(yyrule);
extern int yy_References();
extern int yy_Notes();
extern int yy_Doc();

#include "utility_functions.h"
#include "parsing_functions.h"
#include "rst_peg.h"

static void free_element_contents(element elt);

/* free_element_list - free list of elements recursively */
void free_element_list(element * elt) {
    element * next = NULL;
    while (elt != NULL) {
        next = elt->next;
        free_element_contents(*elt);
        if (elt->children != NULL) {
            free_element_list(elt->children);
            elt->children = NULL;
        }
        free(elt);
        elt = next;
    }
}

/* free_element_contents - free element contents depending on type */
static void free_element_contents(element elt) {
    switch (elt.key) {
        case STR:
        case SPACE:
        case RAW:
        case HTMLBLOCK:
        case HTML:
        case VERBATIM:
        case CODE:
        case NOTE:
            free(elt.contents.str);
            elt.contents.str = NULL;
            break;
        case CODEBLOCK:
            free(elt.contents.code->lang);
            elt.contents.code->lang = NULL;
            free(elt.contents.code->str);
            elt.contents.code->str = NULL;
            free(elt.contents.code);
            elt.contents.code = NULL;
            break;
        case LINK:
        case IMAGE:
        case REFERENCE:
            free(elt.contents.peglink->url);
            elt.contents.peglink->url = NULL;
            free(elt.contents.peglink->title);
            elt.contents.peglink->title = NULL;
            free_element_list(elt.contents.peglink->label);
            free(elt.contents.peglink);
            elt.contents.peglink = NULL;
            break;
        default:
            ;
    }
}

/* free_element - free element and contents */
void free_element(element *elt) {
    free_element_contents(*elt);
    free(elt);
}

element * parse_references(char *string, int extensions) {
    char *oldcharbuf;
    syntax_extensions = extensions;

    oldcharbuf = charbuf;
    charbuf = string;
    yyparsefrom(yy_References);    /* first pass, just to collect references */
    charbuf = oldcharbuf;

    return references;
}

element * parse_notes(char *string, int extensions, element *reference_list) {
    char *oldcharbuf;
    notes = NULL;
    syntax_extensions = extensions;

    if (extension(EXT_NOTES)) {
        references = reference_list;
        oldcharbuf = charbuf;
        charbuf = string;
        yyparsefrom(yy_Notes);     /* second pass for notes */
        charbuf = oldcharbuf;
    }

    return notes;
}

element * parse_rst(char *string, int extensions, element *reference_list, element *note_list) {
    char *oldcharbuf;
    syntax_extensions = extensions;
    references = reference_list;
    notes = note_list;

    oldcharbuf = charbuf;
    charbuf = string;

    yyparsefrom(yy_Doc);

    charbuf = oldcharbuf;          /* restore charbuf to original value */
    return parse_result;
}
