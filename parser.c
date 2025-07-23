#include "parser.h"
#include <stdlib.h> // malloc, free
#include <stdio.h>  // fprintf
#include <string.h> // strdup

// --- Dahili Yardımcı Fonksiyonlar ---

/**
 * @brief Parser'ın mevcut token'ını ilerletir ve bir sonraki token'ı yükler.
 * Aynı zamanda peek_token'ı da günceller.
 * @param parser Parser pointer'ı.
 */
static void advance(Parser* parser) {
    if (parser->current_token) {
        token_free(parser->current_token); // Önceki token'ın belleğini serbest bırak
    }
    parser->current_token = parser->peek_token; // Peek token'ı mevcut token yap
    parser->peek_token = lexer_get_next_token(parser->lexer); // Yeni peek token'ı yükle

    // Lexer'dan tanımsız bir token gelirse hata işaretle
    if (parser->peek_token && parser->peek_token->type == TOKEN_UNKNOWN) {
        parser->has_error = 1;
        // Lexer zaten hata mesajı basmış olmalı, burada ayrıca bir mesaj basmaya gerek yok
    }
}

/**
 * @brief Beklenen bir token türünün mevcut token olup olmadığını kontrol eder.
 * Eğer türler eşleşiyorsa, token'ı tüketir (ilerletir).
 * @param parser Parser pointer'ı.
 * @param expected_type Beklenen TokenType.
 * @return Eşleşme ve tüketme başarılıysa 1, aksi takdirde 0.
 */
static int match(Parser* parser, TokenType expected_type) {
    if (parser->current_token->type == expected_type) {
        advance(parser);
        return 1;
    }
    return 0;
}

/**
 * @brief Beklenen bir token türünün mevcut token olup olmadığını kontrol eder.
 * Eğer türler eşleşiyorsa, token'ı tüketir. Eşleşmiyorsa hata mesajı basar ve hata bayrağını ayarlar.
 * @param parser Parser pointer'ı.
 * @param expected_type Beklenen TokenType.
 */
static void expect(Parser* parser, TokenType expected_type) {
    if (!match(parser, expected_type)) {
        fprintf(stderr, "Hata (%d:%d): Beklenmeyen token '%s' (tür: %s), '%s' bekleniyordu.\n",
                parser->current_token->line, parser->current_token->column,
                parser->current_token->lexeme ? parser->current_token->lexeme : "EOF",
                token_type_to_string(parser->current_token->type),
                token_type_to_string(expected_type));
        parser->has_error = 1;
        // Hata durumunda parser'ı kurtarmak için basit bir senkronizasyon adımı
        // Gerçek bir derleyicide daha karmaşık hata kurtarma stratejileri kullanılır.
        while (parser->current_token->type != TOKEN_EOF &&
               !(parser->current_token->type >= TOKEN_MOV && parser->current_token->type <= TOKEN_RET) && // Bir sonraki komut
               parser->current_token->type != TOKEN_IDENTIFIER && // Veya bir etiket
               parser->current_token->type != TOKEN_COLON) { // Veya etiket tanımı
            advance(parser);
        }
    }
}

/**
 * @brief Bir operandı ayrıştırır ve AstOperand yapısı döndürür.
 * @param parser Parser pointer'ı.
 * @return Ayrıştırılmış AstOperand pointer'ı veya NULL hata durumunda.
 */
static AstOperand* parse_operand(Parser* parser) {
    AstOperand* operand = NULL;
    Token* current = parser->current_token;

    if (current->type == TOKEN_REGISTER) {
        operand = ast_operand_create(OP_REGISTER);
        if (operand) operand->value.reg_index = current->reg_index;
        expect(parser, TOKEN_REGISTER); // Token'ı tüket
    } else if (current->type == TOKEN_INTEGER) {
        operand = ast_operand_create(OP_INTEGER);
        if (operand) operand->value.int_value = current->int_value;
        expect(parser, TOKEN_INTEGER);
    } else if (current->type == TOKEN_HEX_INTEGER) {
        operand = ast_operand_create(OP_HEX_INTEGER);
        if (operand) operand->value.int_value = current->int_value;
        expect(parser, TOKEN_HEX_INTEGER);
    } else if (current->type == TOKEN_IDENTIFIER) { // Etiket referansı olarak varsayılır
        operand = ast_operand_create(OP_LABEL_REF);
        if (operand) operand->value.label_name = strdup(current->lexeme); // Etiket adını kopyala
        expect(parser, TOKEN_IDENTIFIER);
    } else {
        fprintf(stderr, "Hata (%d:%d): Geçersiz operand tipi '%s'.\n",
                current->line, current->column,
                current->lexeme ? current->lexeme : "EOF");
        parser->has_error = 1;
    }
    return operand;
}


/**
 * @brief Bir Bessambly komutunu (instruction) ayrıştırır.
 * @param parser Parser pointer'ı.
 * @return Oluşturulan AstNode (Instruction türünde) veya NULL hata durumunda.
 */
static AstNode* parse_instruction(Parser* parser) {
    AstNode* instruction_node = ast_node_create(AST_INSTRUCTION,
                                                parser->current_token->line,
                                                parser->current_token->column);
    if (!instruction_node) return NULL;

    instruction_node->data.instruction.opcode = parser->current_token->type; // Opcode'u ata
    advance(parser); // Opcode token'ı tüket

    // Komutun operandlarını topla (şimdilik maksimum 3 operand varsayalım, genişletilebilir)
    AstOperand temp_operands[3]; // Geçici statik dizi
    int operand_count = 0;

    // Eğer bir operand gelirse, virgülle ayrılmış diğerlerini de bekle
    if (parser->current_token->type != TOKEN_EOF &&
        (parser->current_token->type == TOKEN_REGISTER ||
         parser->current_token->type == TOKEN_INTEGER ||
         parser->current_token->type == TOKEN_HEX_INTEGER ||
         parser->current_token->type == TOKEN_IDENTIFIER)) {

        AstOperand* op1 = parse_operand(parser);
        if (!op1) { ast_node_free(instruction_node); return NULL; }
        temp_operands[operand_count++] = *op1; free(op1); // Bellek yönetimini doğru yapın

        while (parser->current_token->type == TOKEN_COMMA) {
            expect(parser, TOKEN_COMMA); // Virgülü tüket
            if (operand_count < 3) { // Maksimum operand sayısını kontrol et
                AstOperand* op_n = parse_operand(parser);
                if (!op_n) { ast_node_free(instruction_node); return NULL; }
                temp_operands[operand_count++] = *op_n; free(op_n);
            } else {
                fprintf(stderr, "Hata (%d:%d): Komut için çok fazla operand.\n",
                        parser->current_token->line, parser->current_token->column);
                parser->has_error = 1;
                break;
            }
        }
    }
    
    // Geçici operandları instruction_node'a kopyala
    if (operand_count > 0) {
        instruction_node->data.instruction.operands = (AstOperand*)malloc(sizeof(AstOperand) * operand_count);
        if (!instruction_node->data.instruction.operands) {
            fprintf(stderr, "Hata: Operandlar için bellek tahsis edilemedi.\n");
            ast_node_free(instruction_node);
            return NULL;
        }
        memcpy(instruction_node->data.instruction.operands, temp_operands, sizeof(AstOperand) * operand_count);
        instruction_node->data.instruction.num_operands = operand_count;
    }

    return instruction_node;
}


/**
 * @brief Bir etiket bildirimini ayrıştırır (örn: MY_LABEL:).
 * @param parser Parser pointer'ı.
 * @return Oluşturulan AstNode (LabelDeclaration türünde) veya NULL hata durumunda.
 */
static AstNode* parse_label_declaration(Parser* parser) {
    // Etiket ismini al (TOKEN_IDENTIFIER olması beklenir)
    Token* label_name_token = parser->current_token;
    if (label_name_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Hata (%d:%d): Etiket tanımında beklenen tanımlayıcı yok.\n",
                label_name_token->line, label_name_token->column);
        parser->has_error = 1;
        return NULL;
    }

    AstNode* label_node = ast_node_create(AST_LABEL_DECLARATION,
                                          label_name_token->line,
                                          label_name_token->column);
    if (!label_node) return NULL;

    // Etiket adını kopyala
    label_node->data.label_decl.name = strdup(label_name_token->lexeme);
    if (!label_node->data.label_decl.name) {
        fprintf(stderr, "Hata: Etiket adı için bellek tahsis edilemedi.\n");
        ast_node_free(label_node);
        return NULL;
    }

    advance(parser); // Etiket tanımlayıcısını tüket
    expect(parser, TOKEN_COLON); // İki nokta üst üste işaretini tüket

    return label_node;
}

// --- Harici Fonksiyon Gerçeklemeleri ---

Parser* parser_init(Lexer* lexer) {
    Parser* parser = (Parser*)malloc(sizeof(Parser));
    if (!parser) {
        fprintf(stderr, "Hata: Parser için bellek tahsis edilemedi.\n");
        return NULL;
    }
    parser->lexer = lexer;
    parser->current_token = NULL; // İlk advance çağrısında yüklenecek
    parser->peek_token = NULL;    // İlk advance çağrısında yüklenecek
    parser->has_error = 0;

    // İlk iki token'ı yükle (current_token ve peek_token)
    advance(parser); // current_token'ı TOKEN_EOF'a veya ilk tokene ayarlar
    advance(parser); // peek_token'ı bir sonraki tokene ayarlar

    return parser;
}

void parser_close(Parser* parser) {
    if (parser) {
        token_free(parser->current_token); // Kalan token'ı serbest bırak
        token_free(parser->peek_token);    // Kalan peek token'ı serbest bırak
        // Lexer'ı burada kapatmıyoruz, çünkü dışarıdan geliyor ve main'de kapatılmalı
        free(parser);
    }
}

AstNode* parse_program(Parser* parser) {
    AstNode* program_node = ast_node_create(AST_PROGRAM, 0, 0); // Program düğümü için line/col 0,0 olabilir
    if (!program_node) return NULL;

    // Dinamik bir AstNode* dizisi için başlangıç kapasitesi
    size_t capacity = 16;
    program_node->data.program.statements = (AstNode**)malloc(sizeof(AstNode*) * capacity);
    if (!program_node->data.program.statements) {
        fprintf(stderr, "Hata: Program ifadeleri için bellek tahsis edilemedi.\n");
        ast_node_free(program_node);
        return NULL;
    }

    while (parser->current_token->type != TOKEN_EOF && !parser->has_error) {
        AstNode* statement = NULL;

        if (parser->peek_token->type == TOKEN_COLON) { // Identifier: şeklindeki etiket tanımı
            statement = parse_label_declaration(parser);
        } else if (parser->current_token->type >= TOKEN_MOV && parser->current_token->type <= TOKEN_RET) { // Komutlar
            statement = parse_instruction(parser);
        } else {
            // Tanınmayan bir ifade türü veya hata durumu
            fprintf(stderr, "Hata (%d:%d): Geçersiz ifade başlangıcı '%s' (tür: %s).\n",
                    parser->current_token->line, parser->current_token->column,
                    parser->current_token->lexeme ? parser->current_token->lexeme : "EOF",
                    token_type_to_string(parser->current_token->type));
            parser->has_error = 1;
            // Hata kurtarma: Bilinmeyen token'ı atla ve bir sonraki satırı veya komutu dene
            advance(parser);
            continue; // Bir sonraki döngüye geç
        }

        if (statement) {
            // Diziyi genişletme kontrolü
            if (program_node->data.program.num_statements >= capacity) {
                capacity *= 2;
                AstNode** new_statements = (AstNode**)realloc(program_node->data.program.statements, sizeof(AstNode*) * capacity);
                if (!new_statements) {
                    fprintf(stderr, "Hata: Program ifadeleri dizisi genişletilemedi.\n");
                    ast_node_free(program_node); // Tüm ağacı serbest bırak
                    parser->has_error = 1;
                    return NULL;
                }
                program_node->data.program.statements = new_statements;
            }
            program_node->data.program.statements[program_node->data.program.num_statements++] = statement;
        } else {
            // statement NULL dönerse (genellikle parse_operand, parse_instruction veya parse_label_declaration'da hata)
            parser->has_error = 1;
            // Hata kurtarma için advance burada çağrılmamalı, alt fonksiyonlar handle etmeli
        }
    }

    if (parser->has_error) {
        ast_node_free(program_node); // Hata varsa tüm AST'yi serbest bırak
        return NULL;
    }

    return program_node;
}