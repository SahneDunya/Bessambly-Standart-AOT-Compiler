#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "ast.h" // AST düğüm yapılarına erişim
#include <stdint.h>     // uint32_t için (adresler için)
#include <stddef.h>     // size_t için

// --- Sembol Tablosu Girişi ---
// Etiketler gibi sembollerin bilgilerini saklar.
typedef struct {
    char* name;         // Sembolün adı (etiket adı gibi)
    uint32_t address;   // Etiketin programdaki sanal adresi (daha sonra hesaplanacak)
    int line;           // Tanımlandığı satır numarası
    int column;         // Tanımlandığı sütun numarası
    // ... gelecekte eklenebilecek diğer sembol özellikleri (örn: tür, boyut)
} SymbolEntry;

// --- Sembol Tablosu ---
// Programdaki tüm tanımlı etiketleri ve sembolleri tutar.
// Basit bir dinamik dizi (hash tablosu daha verimli olabilir ama başlangıç için bu yeterli).
typedef struct {
    SymbolEntry* entries;
    size_t count;
    size_t capacity;
} SymbolTable;

// --- Semantik Analizci Yapısı ---
// Semantik analizcinin durumunu (sembol tablosu, hata bayrağı vb.) tutar.
typedef struct {
    SymbolTable* symbol_table; // Programın sembol tablosu
    int has_error;             // Semantik hata olup olmadığını gösteren bayrak
    // ... gelecekte eklenebilecek diğer bağlam bilgileri
} SemanticAnalyzer;

// --- Fonksiyon Prototipleri ---

/**
 * @brief Yeni bir SembolTablosu örneği başlatır.
 * @return Başlatılmış SymbolTable pointer'ı veya NULL hata durumunda.
 */
SymbolTable* symbol_table_init();

/**
 * @brief Bir sembol tablosu girdisini serbest bırakır.
 * @param entry Serbest bırakılacak SymbolEntry.
 */
void symbol_entry_free(SymbolEntry* entry);

/**
 * @brief Sembol tablosunu ve tüm girdilerini serbest bırakır.
 * @param table Serbest bırakılacak SymbolTable pointer'ı.
 */
void symbol_table_free(SymbolTable* table);

/**
 * @brief Sembol tablosuna yeni bir sembol ekler.
 * @param table Sembol tablosu.
 * @param name Eklenecek sembolün adı.
 * @param address Sembolün sanal adresi (varsayılan değerle başlayabilir, sonradan güncellenebilir).
 * @param line Sembolün tanımlandığı satır.
 * @param column Sembolün tanımlandığı sütun.
 * @return Ekleme başarılıysa 1, zaten tanımlıysa veya bellek hatası varsa 0.
 */
int symbol_table_add_symbol(SymbolTable* table, const char* name, uint32_t address, int line, int column);

/**
 * @brief Sembol tablosunda bir sembolü arar.
 * @param table Sembol tablosu.
 * @param name Aranacak sembolün adı.
 * @return Bulunursa SymbolEntry pointer'ı, aksi takdirde NULL.
 */
SymbolEntry* symbol_table_lookup_symbol(SymbolTable* table, const char* name);


/**
 * @brief Yeni bir SemanticAnalyzer örneği başlatır.
 * @return Başlatılmış SemanticAnalyzer pointer'ı veya NULL hata durumunda.
 */
SemanticAnalyzer* semantic_analyzer_init();

/**
 * @brief SemanticAnalyzer'ı kapatır ve kullanılan kaynakları (sembol tablosu vb.) serbest bırakır.
 * @param analyzer Kapatılacak SemanticAnalyzer pointer'ı.
 */
void semantic_analyzer_close(SemanticAnalyzer* analyzer);

/**
 * @brief AST üzerinde semantik analiz yapar.
 * Bu aşamada etiketlerin tanımlı olup olmadığı kontrol edilir ve sembol tablosu oluşturulur.
 * Program düğümündeki etiket adresleri (sanal adresler) henüz atanmaz.
 * @param analyzer SemanticAnalyzer pointer'ı.
 * @param ast_root Ayrıştırılmış AST'nin kök düğümü.
 * @return Analiz başarılıysa 1 (veya 0 hata varsa), aksi takdirde 0 (hata varsa).
 */
int perform_semantic_analysis(SemanticAnalyzer* analyzer, AstNode* ast_root);

#endif // SEMANTIC_ANALYZER_H
