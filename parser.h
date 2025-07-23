#ifndef PARSER_H
#define PARSER_H

#include "lexer.h" // Lexer ve Token yapılarına erişim
#include "ast.h"   // AST düğüm yapılarına erişim

// --- Parser Yapısı ---
// Parser'ın mevcut durumunu (mevcut token, lexer referansı vb.) tutar.
typedef struct {
    Lexer* lexer;      // İlişkili lexer örneği
    Token* current_token; // Şu anda işlenen token
    Token* peek_token;    // Bir sonraki token (ileriye bakmak için)
    int has_error;     // Parser hatası olup olmadığını gösteren bayrak
} Parser;

// --- Fonksiyon Prototipleri ---

/**
 * @brief Yeni bir Parser örneği başlatır.
 * @param lexer Başlatılacak lexer örneği.
 * @return Başlatılmış Parser pointer'ı veya NULL hata durumunda.
 */
Parser* parser_init(Lexer* lexer);

/**
 * @brief Parser'ı kapatır ve kullanılan kaynakları serbest bırakır.
 * @param parser Kapatılacak Parser pointer'ı.
 */
void parser_close(Parser* parser);

/**
 * @brief Bessambly kaynak kodunu ayrıştırır ve bir AST oluşturur.
 * Programın kök düğümünü döndürür. Hata durumunda NULL döner.
 * @param parser Parser pointer'ı.
 * @return Oluşturulan AST'nin kök düğümü (AstNode*), veya NULL hata durumunda.
 */
AstNode* parse_program(Parser* parser);

#endif // PARSER_H