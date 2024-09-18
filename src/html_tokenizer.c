#include "html_tokenizer.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

const char* tzr_state_name(enum tzr_State state) {
    switch (state) {
#define __ENUMERATE_TOKENIZER_STATE(state) \
    case state:                            \
        return #state;
        ENUMERATE_TOKENIZER_STATES
#undef __ENUMERATE_TOKENIZER_STATE
    }

    assert(0 && "should not be reached");
}

struct tzr_tokenizer_data tzr_tokenizer_data_create(const char* html) {
    struct tzr_tokenizer_data data = {
        .m_input         = html,
        .m_state         = tzr_Data,
        .m_return_state  = tzr_Data,
        .m_current_token = tok_make_html_token(),
        .m_cursor        = 0,
    };
    return data;
}

// moves cursor and gives next codepoint
tzr_opt_char tzr_next_codepoint(struct tzr_tokenizer_data* tokenizer) {
    tzr_opt_char result = {.present = false, .value = 0};
    if (tokenizer->m_cursor >= strlen(tokenizer->m_input)) {
        return result;
    }

    result.present = true,
    result.value   = tokenizer->m_input[tokenizer->m_cursor++];
    return result;
}

tzr_opt_char
tzr_peek_codepoint(struct tzr_tokenizer_data* tokenizer, size_t offset) {
    tzr_opt_char result = {.present = false, .value = 0};
    if ((tokenizer->m_cursor + offset) >= strlen(tokenizer->m_input)) {
        return result;
    }

    result.present = true,
    result.value   = tokenizer->m_input[tokenizer->m_cursor + offset];
    return result;
}

bool tzr_next_few_characters_are(
    struct tzr_tokenizer_data* tokenizer,
    const char*                str
) {
    for (size_t i = 0; i < strlen(str); i++) {
        printf("compare: %c\n", str[i]);
        tzr_opt_char codepoint = tzr_peek_codepoint(tokenizer, i);
        if (!codepoint.present) {
            return false;
        }
        if (codepoint.value != str[i]) {
            return false;
        }
    }
    return true;
}

void tzr_consume(struct tzr_tokenizer_data* tokenizer, const char* str) {
    assert(
        tzr_next_few_characters_are(tokenizer, str)
        && "consume requires them to be same"
    );

    tokenizer->m_cursor += strlen(str);
}

void tzr_emit_current_token(struct tzr_tokenizer_data* tokenizer) {
    switch (tokenizer->m_current_token.type) {
        case tok_Doctype:
            printf(
                "EMIT: tok_Doctype: name: %s\n",
                tokenizer->m_current_token.m_doctype.name
            );
            break;
        case tok_StartTag:
            printf("EMIT: tok_Doctype\n");
            break;
        case tok_EndTag:
            printf("EMIT: tok_EndTag\n");
            break;
        case tok_Comment:
            printf("EMIT: tok_Comment\n");
            break;
        case tok_Character:
            printf("EMIT: tok_Character\n");
            break;
        case tok_EndOfFile:
            printf("EMIT: tok_EndOfFile\n");
            break;
        default:
            assert(0 && "should not be reached\n");
            break;
    }
}

#define BEGIN_STATE(state) \
    state:                 \
    case state:

#define END_STATE                         \
    assert(0 && "should not be reached"); \
    break;

#define SWITCH_TO(new_state)                                 \
    printf("switch to: %s\n", tzr_state_name(new_state));    \
    tokenizer->m_state      = new_state;                     \
    current_input_character = tzr_next_codepoint(tokenizer); \
    goto new_state;

#define ON(codepoint)                   \
    if (current_input_character.present \
        && current_input_character.value == codepoint)

#define ON_WHITESPACE                                \
    if (current_input_character.present              \
        && (current_input_character.value == '\t'    \
            || current_input_character.value == '\a' \
            || current_input_character.value == '\f' \
            || current_input_character.value == ' '))

#define ANYTHING_ELSE if (1)

void tzr_run(struct tzr_tokenizer_data* tokenizer) {
    printf("entered tzr_run\n");
    for (;;) {
        tzr_opt_char current_input_character = tzr_next_codepoint(tokenizer);
        switch (tokenizer->m_state) {
            BEGIN_STATE(tzr_Data) {
                ON('&') {
                    tokenizer->m_return_state = tzr_Data;
                    SWITCH_TO(tzr_CharacterReference);
                }
                ON('<') {
                    SWITCH_TO(tzr_TagOpen);
                }
            }
            END_STATE
            BEGIN_STATE(tzr_TagOpen) {
                ON('!') {
                    SWITCH_TO(tzr_MarkupDeclarationOpen);
                }
            }
            END_STATE
            BEGIN_STATE(tzr_MarkupDeclarationOpen) {
                --tokenizer->m_cursor;
                if (tzr_next_few_characters_are(tokenizer, "DOCTYPE")) {
                    tzr_consume(tokenizer, "DOCTYPE");
                    SWITCH_TO(tzr_DOCTYPE);
                }
            }
            END_STATE
            BEGIN_STATE(tzr_DOCTYPE) {
                ON_WHITESPACE {
                    SWITCH_TO(tzr_BeforeDOCTYPEName);
                }
            }
            END_STATE
            BEGIN_STATE(tzr_BeforeDOCTYPEName) {
                ON_WHITESPACE {
                    continue;
                }

                ANYTHING_ELSE {
                    tokenizer->m_current_token.type = tok_Doctype;
                    sprintf(
                        tokenizer->m_current_token.m_doctype.name,
                        "%s%c",
                        tokenizer->m_current_token.m_doctype.name,
                        current_input_character.value
                    );
                    SWITCH_TO(tzr_DOCTYPEName);
                }
            }
            END_STATE
            BEGIN_STATE(tzr_DOCTYPEName) {
                ON('>') {
                    assert(tokenizer->m_current_token.type == tok_Doctype);
                    tzr_emit_current_token(tokenizer);
                    SWITCH_TO(tzr_Data);
                }
                ANYTHING_ELSE {
                    assert(tokenizer->m_current_token.type == tok_Doctype);
                    sprintf(
                        tokenizer->m_current_token.m_doctype.name,
                        "%s%c",
                        tokenizer->m_current_token.m_doctype.name,
                        current_input_character.value
                    );
                    continue;
                }
            }
            END_STATE
            BEGIN_STATE(tzr_CharacterReference) {}
            END_STATE
            default:
                END_STATE
        }
    }
}
