#include "lexer.h"
#include <stdlib.h> // malloc, free
#include <string.h> // strncpy, strcmp
#include <ctype.h>  // isalnum, isdigit, isalpha

// Anahtar kelimeler ve karşılık gelen TokenType'lar
typedef struct {
    const char* keyword;
    TokenType type;
} KeywordMapping;

// Anahtar kelime tablosu - Bessambly komutları buraya eklenmeli
static KeywordMapping keywords[] = {
    {"MOV", TOKEN_MOV},
    {"ADD", TOKEN_ADD},
    {"SUB", TOKEN_SUB},
    {"MUL", TOKEN_MUL},
    {"DIV", TOKEN_DIV},
    {"CMP", TOKEN_CMP},
    {"JMP", TOKEN_JMP},
    {"JEQ", TOKEN_JEQ},
    {"JNE", TOKEN_JNE},
    {"JLT", TOKEN_JLT},
    {"JGT", TOKEN_JGT},
    {"SYSCALL", TOKEN_SYSCALL},
    {"RET", TOKEN_RET},
    {NULL, TOKEN_UNKNOWN} // Listenin sonunu işaretler
};

// --- Dahili Yardımcı Fonksiyonlar ---

/**
 * @brief Lexer'dan bir sonraki karakteri okur ve mevcut konum bilgilerini günceller.
 * @param lexer Lexer pointer'ı.
 */
static void advance(Lexer* lexer) {
    if (lexer->eof_reached) return;

    int c = fgetc(lexer->source_file);
    if (c == EOF) {
        lexer->current_char = '\0'; // NUL karakteri EOF'u temsil eder
        lexer->eof_reached = 1;
    } else {
        lexer->current_char = (char)c;
        if (lexer->current_char == '\n') {
            lexer->line++;
            lexer->column = 1; // Yeni satıra geçince sütunu sıfırla
        } else {
            lexer->column++;
        }
    }
}

/**
 * @brief Boşluk ve yorum karakterlerini atlar.
 * @param lexer Lexer pointer'ı.
 */
static void skip_whitespace_and_comments(Lexer* lexer) {
    while (lexer->current_char != '\0') {
        if (isspace(lexer->current_char)) {
            advance(lexer);
        } else if (lexer->current_char == ';') { // Satır sonu yorumu
            while (lexer->current_char != '\n' && lexer->current_char != '\0') {
                advance(lexer);
            }
            // Satır sonu karakterini de atla
            if (lexer->current_char == '\n') {
                advance(lexer);
            }
        } else {
            break; // Boşluk veya yorum değilse dur
        }
    }
}

/**
 * @brief Yeni bir Token yapısı oluşturur ve değerlerini atar.
 * lexeme NULL ise bellek tahsis edilmez.
 * @param type Token türü.
 * @param lexeme Token metni (kopyası alınır).
 * @param line Satır numarası.
 * @param column Sütun numarası.
 * @return Yeni Token pointer'ı veya NULL bellek hatası durumunda.
 */
static Token* create_token(TokenType type, const char* lexeme, int line, int column) {
    Token* token = (Token*)malloc(sizeof(Token));
    if (!token) {
        fprintf(stderr, "Hata: Token için bellek tahsis edilemedi.\n");
        return NULL;
    }
    token->type = type;
    token->line = line;
    token->column = column;
    token->int_value = 0; // Varsayılan değer
    token->reg_index = -1; // Varsayılan değer

    if (lexeme) {
        token->lexeme = (char*)malloc(strlen(lexeme) + 1);
        if (!token->lexeme) {
            fprintf(stderr, "Hata: Token lexeme için bellek tahsis edilemedi.\n");
            free(token);
            return NULL;
        }
        strcpy(token->lexeme, lexeme);
    } else {
        token->lexeme = NULL;
    }
    return token;
}

// --- Harici Fonksiyon Gerçeklemeleri ---

Lexer* lexer_init(const char* filename) {
    Lexer* lexer = (Lexer*)malloc(sizeof(Lexer));
    if (!lexer) {
        fprintf(stderr, "Hata: Lexer için bellek tahsis edilemedi.\n");
        return NULL;
    }

    lexer->source_file = fopen(filename, "r");
    if (!lexer->source_file) {
        fprintf(stderr, "Hata: '%s' dosyası açılamadı.\n", filename);
        free(lexer);
        return NULL;
    }

    lexer->line = 1;
    lexer->column = 0; // İlk advance'da 1 olacak
    lexer->eof_reached = 0;
    advance(lexer); // İlk karakteri oku
    return lexer;
}

Token* lexer_get_next_token(Lexer* lexer) {
    skip_whitespace_and_comments(lexer);

    // Dosya sonu kontrolü
    if (lexer->eof_reached || lexer->current_char == '\0') {
        return create_token(TOKEN_EOF, NULL, lexer->line, lexer->column);
    }

    int start_line = lexer->line;
    int start_column = lexer->column;

    // --- Tek Karakterli Semboller ---
    switch (lexer->current_char) {
        case ':':
            advance(lexer);
            return create_token(TOKEN_COLON, ":", start_line, start_column);
        case ',':
            advance(lexer);
            return create_token(TOKEN_COMMA, ",", start_line, start_column);
        // ... (gelecekte eklenebilecek diğer tek karakterli semboller)
    }

    // --- Sayılar (Tamsayılar ve Onaltılıklar) ---
    if (isdigit(lexer->current_char)) {
        char buffer[256]; // Sayı arabelleği
        int i = 0;
        int is_hex = 0;

        // "0x" ile başlayan onaltılık sayıları kontrol et
        if (lexer->current_char == '0') {
            buffer[i++] = lexer->current_char;
            advance(lexer);
            if (lexer->current_char == 'x' || lexer->current_char == 'X') {
                buffer[i++] = lexer->current_char;
                advance(lexer);
                is_hex = 1;
                // Onaltılık basamakları oku
                while (isxdigit(lexer->current_char) && i < sizeof(buffer) - 1) {
                    buffer[i++] = lexer->current_char;
                    advance(lexer);
                }
            } else {
                // Sadece '0' ise veya '0'dan sonra sayı geliyorsa normal ondalık sayı
                while (isdigit(lexer->current_char) && i < sizeof(buffer) - 1) {
                    buffer[i++] = lexer->current_char;
                    advance(lexer);
                }
            }
        } else {
            // Ondalık sayıları oku
            while (isdigit(lexer->current_char) && i < sizeof(buffer) - 1) {
                buffer[i++] = lexer->current_char;
                advance(lexer);
            }
        }
        buffer[i] = '\0'; // Null sonlandır

        Token* token = create_token(is_hex ? TOKEN_HEX_INTEGER : TOKEN_INTEGER, buffer, start_line, start_column);
        if (token) {
            // Sayı değerini direkt Token yapısına kaydet
            if (is_hex) {
                token->int_value = strtoll(buffer, NULL, 16); // Onaltılık string'i long long'a çevir
            } else {
                token->int_value = strtoll(buffer, NULL, 10); // Ondalık string'i long long'a çevir
            }
        }
        return token;
    }

    // --- Tanımlayıcılar ve Anahtar Kelimeler / Kaydediciler ---
    if (isalpha(lexer->current_char)) {
        char buffer[256]; // Tanımlayıcı arabelleği
        int i = 0;
        while (isalnum(lexer->current_char) && i < sizeof(buffer) - 1) {
            buffer[i++] = lexer->current_char;
            advance(lexer);
        }
        buffer[i] = '\0'; // Null sonlandır

        // Kaydedici kontrolü (Örn: R0, R1, R15)
        if (buffer[0] == 'R' && isdigit(buffer[1])) {
            int is_register = 1;
            for (int j = 1; buffer[j] != '\0'; j++) {
                if (!isdigit(buffer[j])) {
                    is_register = 0;
                    break;
                }
            }
            if (is_register) {
                Token* reg_token = create_token(TOKEN_REGISTER, buffer, start_line, start_column);
                if (reg_token) {
                    // Kaydedici indeksini 'R' sonrası kısmı tamsayıya çevirerek al
                    reg_token->reg_index = atoi(&buffer[1]);
                }
                return reg_token;
            }
        }

        // Anahtar kelime kontrolü
        for (int k = 0; keywords[k].keyword != NULL; k++) {
            if (strcmp(buffer, keywords[k].keyword) == 0) {
                return create_token(keywords[k].type, buffer, start_line, start_column);
            }
        }

        // Anahtar kelime veya kaydedici değilse tanımlayıcıdır (etiketler vb.)
        return create_token(TOKEN_IDENTIFIER, buffer, start_line, start_column);
    }

    // --- Tanımsız Karakter ---
    // Eğer buraya kadar hiçbir şeye uymadıysa, tanımsız bir karakterdir.
    char unknown_char_str[2] = {lexer->current_char, '\0'};
    fprintf(stderr, "Hata (%d:%d): Tanınmayan karakter '%c'.\n", lexer->line, lexer->column, lexer->current_char);
    advance(lexer); // Hatalı karakteri atla
    return create_token(TOKEN_UNKNOWN, unknown_char_str, start_line, start_column);
}

void token_free(Token* token) {
    if (token) {
        free(token->lexeme); // Eğer tahsis edildiyse serbest bırak
        free(token);
    }
}

void lexer_close(Lexer* lexer) {
    if (lexer) {
        if (lexer->source_file) {
            fclose(lexer->source_file);
        }
        free(lexer);
    }
}

const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_EOF: return "EOF";
        case TOKEN_UNKNOWN: return "UNKNOWN";
        case TOKEN_MOV: return "MOV";
        case TOKEN_ADD: return "ADD";
        case TOKEN_SUB: return "SUB";
        case TOKEN_MUL: return "MUL";
        case TOKEN_DIV: return "DIV";
        case TOKEN_CMP: return "CMP";
        case TOKEN_JMP: return "JMP";
        case TOKEN_JEQ: return "JEQ";
        case TOKEN_JNE: return "JNE";
        case TOKEN_JLT: return "JLT";
        case TOKEN_JGT: return "JGT";
        case TOKEN_SYSCALL: return "SYSCALL";
        case TOKEN_RET: return "RET";
        case TOKEN_REGISTER: return "REGISTER";
        case TOKEN_INTEGER: return "INTEGER";
        case TOKEN_HEX_INTEGER: return "HEX_INTEGER";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_COLON: return "COLON";
        case TOKEN_COMMA: return "COMMA";
        default: return "UNKNOWN_TYPE";
    }
}
