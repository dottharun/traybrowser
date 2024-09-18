#include "html_token.c"

#include <stdbool.h>
#include <stddef.h>

/*
 * Tokenizer: Take input one codepoint at a time
 *      - then produce a sequence of html tokens
 * */

// NOTE: macro way to get a list of enums - formatted in custom way we want by
// the caller
#define ENUMERATE_TOKENIZER_STATES                                            \
    __ENUMERATE_TOKENIZER_STATE(tzr_Data)                                     \
    __ENUMERATE_TOKENIZER_STATE(tzr_RCDATA)                                   \
    __ENUMERATE_TOKENIZER_STATE(tzr_RAWTEXT)                                  \
    __ENUMERATE_TOKENIZER_STATE(tzr_ScriptData)                               \
    __ENUMERATE_TOKENIZER_STATE(tzr_PLAINTEXT)                                \
    __ENUMERATE_TOKENIZER_STATE(tzr_TagOpen)                                  \
    __ENUMERATE_TOKENIZER_STATE(tzr_EndTagOpen)                               \
    __ENUMERATE_TOKENIZER_STATE(tzr_TagName)                                  \
    __ENUMERATE_TOKENIZER_STATE(tzr_RCDATALessThanSign)                       \
    __ENUMERATE_TOKENIZER_STATE(tzr_RCDATAEndTagOpen)                         \
    __ENUMERATE_TOKENIZER_STATE(tzr_RCDATAEndTagName)                         \
    __ENUMERATE_TOKENIZER_STATE(tzr_RAWTEXTLessThanSign)                      \
    __ENUMERATE_TOKENIZER_STATE(tzr_RAWTEXTEndTagOpen)                        \
    __ENUMERATE_TOKENIZER_STATE(tzr_RAWTEXTEndTagName)                        \
    __ENUMERATE_TOKENIZER_STATE(tzr_ScriptDataLessThanSign)                   \
    __ENUMERATE_TOKENIZER_STATE(tzr_ScriptDataEndTagOpen)                     \
    __ENUMERATE_TOKENIZER_STATE(tzr_ScriptDataEndTagName)                     \
    __ENUMERATE_TOKENIZER_STATE(tzr_ScriptDataEscapeStart)                    \
    __ENUMERATE_TOKENIZER_STATE(tzr_ScriptDataEscapeStartDash)                \
    __ENUMERATE_TOKENIZER_STATE(tzr_ScriptDataEscaped)                        \
    __ENUMERATE_TOKENIZER_STATE(tzr_ScriptDataEscapedDash)                    \
    __ENUMERATE_TOKENIZER_STATE(tzr_ScriptDataEscapedDashDash)                \
    __ENUMERATE_TOKENIZER_STATE(tzr_ScriptDataEscapedLessThanSign)            \
    __ENUMERATE_TOKENIZER_STATE(tzr_ScriptDataEscapedEndTagOpen)              \
    __ENUMERATE_TOKENIZER_STATE(tzr_ScriptDataEscapedEndTagName)              \
    __ENUMERATE_TOKENIZER_STATE(tzr_ScriptDataDoubleEscapeStart)              \
    __ENUMERATE_TOKENIZER_STATE(tzr_ScriptDataDoubleEscaped)                  \
    __ENUMERATE_TOKENIZER_STATE(tzr_ScriptDataDoubleEscapedDash)              \
    __ENUMERATE_TOKENIZER_STATE(tzr_ScriptDataDoubleEscapedDashDash)          \
    __ENUMERATE_TOKENIZER_STATE(tzr_ScriptDataDoubleEscapedLessThanSign)      \
    __ENUMERATE_TOKENIZER_STATE(tzr_ScriptDataDoubleEscapeEnd)                \
    __ENUMERATE_TOKENIZER_STATE(tzr_BeforeAttributeName)                      \
    __ENUMERATE_TOKENIZER_STATE(tzr_AttributeName)                            \
    __ENUMERATE_TOKENIZER_STATE(tzr_AfterAttributeName)                       \
    __ENUMERATE_TOKENIZER_STATE(tzr_BeforeAttributeValue)                     \
    __ENUMERATE_TOKENIZER_STATE(tzr_AttributeValueDoubleQuoted)               \
    __ENUMERATE_TOKENIZER_STATE(tzr_AttributeValueSingleQuoted)               \
    __ENUMERATE_TOKENIZER_STATE(tzr_AttributeValueUnquoted)                   \
    __ENUMERATE_TOKENIZER_STATE(tzr_AfterAttributeValueQuoted)                \
    __ENUMERATE_TOKENIZER_STATE(tzr_SelfClosingStartTag)                      \
    __ENUMERATE_TOKENIZER_STATE(tzr_BogusComment)                             \
    __ENUMERATE_TOKENIZER_STATE(tzr_MarkupDeclarationOpen)                    \
    __ENUMERATE_TOKENIZER_STATE(tzr_CommentStart)                             \
    __ENUMERATE_TOKENIZER_STATE(tzr_CommentStartDash)                         \
    __ENUMERATE_TOKENIZER_STATE(tzr_Comment)                                  \
    __ENUMERATE_TOKENIZER_STATE(tzr_CommentLessThanSign)                      \
    __ENUMERATE_TOKENIZER_STATE(tzr_CommentLessThanSignBang)                  \
    __ENUMERATE_TOKENIZER_STATE(tzr_CommentLessThanSignBangDash)              \
    __ENUMERATE_TOKENIZER_STATE(tzr_CommentLessThanSignBangDashDash)          \
    __ENUMERATE_TOKENIZER_STATE(tzr_CommentEndDash)                           \
    __ENUMERATE_TOKENIZER_STATE(tzr_CommentEnd)                               \
    __ENUMERATE_TOKENIZER_STATE(tzr_CommentEndBang)                           \
    __ENUMERATE_TOKENIZER_STATE(tzr_DOCTYPE)                                  \
    __ENUMERATE_TOKENIZER_STATE(tzr_BeforeDOCTYPEName)                        \
    __ENUMERATE_TOKENIZER_STATE(tzr_DOCTYPEName)                              \
    __ENUMERATE_TOKENIZER_STATE(tzr_AfterDOCTYPEName)                         \
    __ENUMERATE_TOKENIZER_STATE(tzr_AfterDOCTYPEPublicKeyword)                \
    __ENUMERATE_TOKENIZER_STATE(tzr_BeforeDOCTYPEPublicIdentifier)            \
    __ENUMERATE_TOKENIZER_STATE(tzr_DOCTYPEPublicIdentifierDoubleQuoted)      \
    __ENUMERATE_TOKENIZER_STATE(tzr_DOCTYPEPublicIdentifierSingleQuoted)      \
    __ENUMERATE_TOKENIZER_STATE(tzr_AfterDOCTYPEPublicIdentifier)             \
    __ENUMERATE_TOKENIZER_STATE(tzr_BetweenDOCTYPEPublicAndSystemIdentifiers) \
    __ENUMERATE_TOKENIZER_STATE(tzr_AfterDOCTYPESystemKeyword)                \
    __ENUMERATE_TOKENIZER_STATE(tzr_BeforeDOCTYPESystemIdentifier)            \
    __ENUMERATE_TOKENIZER_STATE(tzr_DOCTYPESystemIdentifierDoubleQuoted)      \
    __ENUMERATE_TOKENIZER_STATE(tzr_DOCTYPESystemIdentifierSingleQuoted)      \
    __ENUMERATE_TOKENIZER_STATE(tzr_AfterDOCTYPESystemIdentifier)             \
    __ENUMERATE_TOKENIZER_STATE(tzr_BogusDOCTYPE)                             \
    __ENUMERATE_TOKENIZER_STATE(tzr_CDATASection)                             \
    __ENUMERATE_TOKENIZER_STATE(tzr_CDATASectionBracket)                      \
    __ENUMERATE_TOKENIZER_STATE(tzr_CDATASectionEnd)                          \
    __ENUMERATE_TOKENIZER_STATE(tzr_CharacterReference)                       \
    __ENUMERATE_TOKENIZER_STATE(tzr_NamedCharacterReference)                  \
    __ENUMERATE_TOKENIZER_STATE(tzr_AmbiguousAmpersand)                       \
    __ENUMERATE_TOKENIZER_STATE(tzr_NumericCharacterReference)                \
    __ENUMERATE_TOKENIZER_STATE(tzr_HexadecimalCharacterReferenceStart)       \
    __ENUMERATE_TOKENIZER_STATE(tzr_DecimalCharacterReferenceStart)           \
    __ENUMERATE_TOKENIZER_STATE(tzr_HexadecimalCharacterReference)            \
    __ENUMERATE_TOKENIZER_STATE(tzr_DecimalCharacterReference)                \
    __ENUMERATE_TOKENIZER_STATE(tzr_NumericCharacterReferenceEn)

enum tzr_State {

#define __ENUMERATE_TOKENIZER_STATE(state) state,
    ENUMERATE_TOKENIZER_STATES
#undef __ENUMERATE_TOKENIZER_STATE

};

struct tzr_tokenizer_data {
    const char*           m_input;
    enum tzr_State        m_state;
    enum tzr_State        m_return_state;
    struct tok_html_token m_current_token;
    size_t                m_cursor;
};

// constructor
struct tzr_tokenizer_data tzr_tokenizer_data_create(const char* html);

#define optional_type(type) \
    struct {                \
        bool present;       \
        type value;         \
    }

typedef optional_type(char) tzr_opt_char;

tzr_opt_char tzr_next_codepoint(struct tzr_tokenizer_data* tokenizer);

tzr_opt_char
tzr_peek_codepoint(struct tzr_tokenizer_data* tokenizer, size_t offset);

bool tzr_next_few_characters_are(
    struct tzr_tokenizer_data* tokenizer,
    const char*                str
);

void tzr_consume(struct tzr_tokenizer_data* tokenizer, const char* str);
void tzr_emit_current_token(struct tzr_tokenizer_data* tokenizer);
void tzr_run(struct tzr_tokenizer_data* tokenizer);

// helper function to stringify state of tokenizer
const char* tzr_state_name(enum tzr_State state);
