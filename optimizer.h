#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "ast.h" // AST düğüm yapılarına erişim
#include "semantic_analyzer.h" // Sembol tablosu gibi bilgilere erişim

// --- Optimizer Yapısı (İsteğe Bağlı) ---
// Daha karmaşık optimizasyonlar için bir bağlam tutabilir.
// Şimdilik sadece fonksiyonlar yeterli olabilir.
typedef struct {
    // Gelecekte eklenebilecek bağlam bilgileri (örn: optimizasyon seviyesi, istatistikler)
    int optimization_level; // Örn: 0=Hiç optimizasyon yok, 1=Basit optimizasyonlar
} Optimizer;

// --- Fonksiyon Prototipleri ---

/**
 * @brief Yeni bir Optimizer örneği başlatır.
 * @return Başlatılmış Optimizer pointer'ı veya NULL hata durumunda.
 */
Optimizer* optimizer_init();

/**
 * @brief Optimizer'ı kapatır ve kullanılan kaynakları serbest bırakır.
 * @param optimizer Kapatılacak Optimizer pointer'ı.
 */
void optimizer_close(Optimizer* optimizer);

/**
 * @brief AST üzerinde çeşitli optimizasyon geçişlerini gerçekleştirir.
 * Bu fonksiyon, bir veya daha fazla optimizasyon algoritmasını uygulayabilir.
 * @param optimizer Optimizer pointer'ı.
 * @param ast_root Optimize edilecek AST'nin kök düğümü.
 * @param symbol_table Semantik analizden gelen sembol tablosu (gerekli olabilir).
 * @return Optimizasyon başarılıysa 1, aksi takdirde 0.
 */
int perform_optimizations(Optimizer* optimizer, AstNode* ast_root, SymbolTable* symbol_table);

// --- Bağımsız Optimizasyon Geçişleri (Örnekler) ---

/**
 * @brief Ölü kod eleme geçişi. Ulaşılmayan kodları (örn: JMP sonrası ölü kod) kaldırır.
 * @param ast_root Optimize edilecek AST'nin kök düğümü.
 * @param symbol_table Sembol tablosu (etiket adresleri için).
 * @return Değişiklik yapıldıysa 1, yapılmadıysa 0.
 */
int optimize_dead_code_elimination(AstNode* ast_root, SymbolTable* symbol_table);

/**
 * @brief Sabit katlama geçişi. Derleme zamanında bilinen aritmetik işlemleri hesaplar.
 * Örn: ADD R0, 5; ADD R0, 3  -> ADD R0, 8 (bu Instruction Level IR'de daha iyi olurdu)
 * Bu örnek için daha çok "doğrudan sabit kullanım" anlamında düşünebiliriz:
 * MOV R0, 10; ADD R0, 5 -> MOV R0, 15
 * @param ast_root Optimize edilecek AST'nin kök düğümü.
 * @return Değişiklik yapıldıysa 1, yapılmadıysa 0.
 */
int optimize_constant_folding(AstNode* ast_root);

/**
 * @brief Etiket atlamalarını kısaltma (Jump Threading) geçişi.
 * Birden fazla JMP komutunu birleştirmeye çalışır.
 * Örn: JMP LabelA; ... LabelA: JMP LabelB -> JMP LabelB
 * @param ast_root Optimize edilecek AST'nin kök düğümü.
 * @param symbol_table Sembol tablosu (etiket adresleri için).
 * @return Değişiklik yapıldıysa 1, yapılmadıysa 0.
 */
int optimize_jump_threading(AstNode* ast_root, SymbolTable* symbol_table);


#endif // OPTIMIZER_H