#ifndef LEXER_H
#define LEXER_H

#include <stdio.h> // FILE* için

// --- Token Türleri (TokenType) ---
// Bessambly dilindeki tüm anahtar kelimeleri, sembolleri, değişmezleri vb. temsil eder.
typedef enum {
    // Özel Tokenler
    TOKEN_EOF = 0,      // Dosya sonu
    TOKEN_UNKNOWN,      // Tanımsız veya hata durumu

    // Anahtar Kelimeler (Kavramsal Bessambly Komutları)
    TOKEN_MOV,          // MOVE (taşıma) komutu
    TOKEN_ADD,          // ADD (toplama) komutu
    TOKEN_SUB,          // SUBTRACT (çıkarma) komutu
    TOKEN_MUL,          // MULTIPLY (çarpma) komutu
    TOKEN_DIV,          // DIVIDE (bölme) komutu
    TOKEN_CMP,          // COMPARE (karşılaştırma) komutu
    TOKEN_JMP,          // JUMP (koşulsuz atlama) komutu
    TOKEN_JEQ,          // JUMP IF EQUAL (eşitse atla) komutu
    TOKEN_JNE,          // JUMP IF NOT EQUAL (eşit değilse atla) komutu
    TOKEN_JLT,          // JUMP IF LESS THAN (küçükse atla) komutu
    TOKEN_JGT,          // JUMP IF GREATER THAN (büyükse atla) komutu
    TOKEN_SYSCALL,      // SYSTEM CALL (sistem çağrısı) komutu
    TOKEN_RET,          // RETURN (fonksiyon/alt programdan dönme) komutu
    // ... (gelecekte eklenebilecek diğer Bessambly komutları)

    // Operandlar ve Değişmezler
    TOKEN_REGISTER,     // Kaydedici (örn: R0, R15)
    TOKEN_INTEGER,      // Tamsayı değişmezi (örn: 123, 0xABC)
    TOKEN_HEX_INTEGER,  // Onaltılık tamsayı değişmezi (0x ile başlayan)
    TOKEN_IDENTIFIER,   // Tanımlayıcı (etiketler, değişkenler vb.)

    // Semboller
    TOKEN_COLON,        // İki nokta üst üste (etiket tanımları için)
    TOKEN_COMMA,        // Virgül (operand ayırıcı)
    // ... (gelecekte eklenebilecek diğer semboller)

} TokenType;

// --- Token Yapısı ---
// Lexer tarafından üretilen her bir token'ın bilgilerini içerir.
typedef struct {
    TokenType type;     // Token'ın türü (yukarıdaki enum'dan)
    char* lexeme;       // Token'ın kaynak koddaki orijinal metinsel karşılığı (örn: "MOV", "R1", "123")
    int line;           // Token'ın bulunduğu satır numarası
    int column;         // Token'ın bulunduğu sütun numarası
    // Eğer integer veya register gibi spesifik değerler tutulacaksa buraya eklenebilir
    long long int_value; // Eğer TOKEN_INTEGER ise tamsayı değeri
    int reg_index;       // Eğer TOKEN_REGISTER ise kaydedici indeksi (örn: R0 için 0)
} Token;

// --- Lexer Yapısı ---
// Lexer'ın mevcut durumunu (okuduğu dosya, mevcut konum vb.) tutar.
typedef struct {
    FILE* source_file;  // Kaynak Bessambly dosyasının işaretçisi
    char current_char;  // Şu anki okunan karakter
    int line;           // Mevcut satır numarası
    int column;         // Mevcut sütun numarası
    int eof_reached;    // Dosya sonuna ulaşıldı mı bayrağı
} Lexer;

// --- Fonksiyon Prototipleri ---

/**
 * @brief Yeni bir Lexer örneği başlatır.
 * @param filename Bessambly kaynak dosyasının yolu.
 * @return Başlatılmış Lexer pointer'ı veya NULL hata durumunda.
 */
Lexer* lexer_init(const char* filename);

/**
 * @brief Lexer tarafından bir sonraki token'ı okur ve döndürür.
 * Bu fonksiyon dahili olarak belleği tahsis edebilir, bu yüzden
 * kullanım sonrası 'token_free' ile serbest bırakılmalıdır.
 * @param lexer Lexer pointer'ı.
 * @return Okunan Token pointer'ı. TOKEN_EOF türünde bir token döndürülürse dosya sonuna ulaşılmıştır.
 * TOKEN_UNKNOWN türünde bir token döndürülürse tanınmayan bir karakter veya hata oluşmuştur.
 */
Token* lexer_get_next_token(Lexer* lexer);

/**
 * @brief Bir Token'a tahsis edilen belleği serbest bırakır.
 * @param token Serbest bırakılacak Token pointer'ı.
 */
void token_free(Token* token);

/**
 * @brief Lexer'ı kapatır ve kullanılan kaynakları (dosya vb.) serbest bırakır.
 * @param lexer Kapatılacak Lexer pointer'ı.
 */
void lexer_close(Lexer* lexer);

/**
 * @brief Bir TokenType'ı insan tarafından okunabilir string'e dönüştürür.
 * @param type Dönüştürülecek TokenType.
 * @return TokenType'ın string karşılığı.
 */
const char* token_type_to_string(TokenType type);

#endif // LEXER_H