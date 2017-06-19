/* rst_peg.h */
#ifndef RST_PEG_H
#define RST_PEG_H

#include "rst_lib.h"
#include <glib.h>

/* Information (label, URL and title) for a link. */
struct Link {
    struct Element   *label;
    char             *url;
    char             *title;    
};

typedef struct Link link;

struct Code {
    char *lang;
    char *str;
};

typedef struct Code code;

/* Union for contents of an Element (string, list, or link). */
union Contents {
    char             *str;
    struct Link      *link;
    struct Code      *code;
};

/* Types of semantic values returned by parsers. */ 
enum keys {
    LIST,     /* A generic list of values.  For ordered and bullet lists, see below. */
    RAW,
    SPACE,
    LINEBREAK,
    ELLIPSIS,
    EMDASH,
    ENDASH,
    APOSTROPHE,
    SINGLEQUOTED,
    DOUBLEQUOTED,
    STR,
    LINK,
    IMAGE,
    CODE,
    HTML,
    EMPH,
    STRONG,
    STRIKE,
    PLAIN,
    PARA,
    LISTITEM,
    BULLETLIST,
    ORDEREDLIST,
    H1TITLE,
    H1 /* Code assumes that H1..6 are in order. */,
    H2,
    H3,
    H4,
    H5,
    H6,
    TABLE,
    TABLESEPARATOR,
    TABLECELL,
    CELLSPAN,
    TABLEROW,
    TABLEBODY,
    TABLEHEAD,
    CODEBLOCK,
    BLOCKQUOTE,
    VERBATIM,
    HTMLBLOCK,
    HRULE,
    REFERENCE,
    NOTE,
    DEFINITIONLIST,
    DEFTITLE,
    DEFDATA,
    numVAL
};

/* Semantic value of a parsing action. */
struct Element {
    int               key;
    union Contents    contents;
    struct Element    *children;
    struct Element    *next;
};

typedef struct Element element;

element * parse_references(char *string, int extensions);
element * parse_notes(char *string, int extensions, element *reference_list);
element * parse_rst(char *string, int extensions, element *reference_list, element *note_list);
void free_element_list(element * elt);
void free_element(element *elt);
void print_element_list(GString *out, element *elt, int format, int exts);

#endif
