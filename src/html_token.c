#include <stdbool.h>
#include <stddef.h>

struct tok_attributes {
    char* m_name;
    char* m_value;
};

// -----------------------

enum tok_Type {
    tok_Doctype,
    tok_StartTag,
    tok_EndTag,
    tok_Comment,
    tok_Character,
    tok_EndOfFile
};

struct tok_html_token {
    enum tok_Type type;

    // IMPROVE: All these strings must be appendable - datastructure

    // Type::Doctype
    struct {
        char  name[100]; // TESTING with fixed allocated strings
        char* public_identifier;
        char* system_public_identifier;
        bool  force_quirks; // TODO: constructor default false
    } m_doctype;

    // Type::EndTag
    // Type::StartTag
    struct {
        char*                  tag_name;
        bool                   self_closing; // TODO: constructor default false
        struct tok_attributes* attributes;   // TODO: constructor default NULL
    } m_tag;

    // Type::Comment
    // Type::Character
    struct {
        char* data; // TODO: constructor default NULL
    } m_comment_or_char;
};

// constructor
struct tok_html_token tok_make_html_token() {
    const struct tok_html_token token = {
        .type              = tok_Doctype,
        .m_doctype         = {"",    NULL, NULL, false},
        .m_tag             = {NULL, false, NULL},
        .m_comment_or_char = {NULL    }
    };
    return token;
}

// getter  -- probably not needed
enum tok_Type tok_type(struct tok_html_token* token) {
    return token->type;
}
