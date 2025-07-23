#include "optimizer.h"
#include <stdlib.h> // malloc, free, realloc
#include <stdio.h>  // fprintf
#include <string.h> // strcmp, strdup

// --- Optimizer Gerçeklemeleri ---

Optimizer* optimizer_init() {
    Optimizer* optimizer = (Optimizer*)malloc(sizeof(Optimizer));
    if (!optimizer) {
        fprintf(stderr, "Hata: Optimizer için bellek tahsis edilemedi.\n");
        return NULL;
    }
    optimizer->optimization_level = 1; // Varsayılan optimizasyon seviyesi
    return optimizer;
}

void optimizer_close(Optimizer* optimizer) {
    if (optimizer) {
        free(optimizer);
    }
}

// --- Yardımcı Fonksiyonlar (Sembol Tablosu Adresleri için) ---
// NOT: Bu fonksiyonlar Semantik Analiz sonrası kod üretimine yakın bir aşamada çalışmalı.
// Şimdilik adresleri semantik analiz aşamasında doldurmadığımız için 0 varsayılıyor.
// Gerçek optimizasyonlar için doğru adreslere ihtiyaç duyulacaktır.
// Bu kısım, daha sonra kod üretimi ile birleşebilir veya semantik analizde ikinci bir geçişle doldurulabilir.
// VEYA optimizasyonlar, AST'nin kendisinde bu adres bilgilerini depolayacak şekilde geliştirilebilir.

/**
 * @brief Programdaki her ifadenin (instruction veya label) sanal adresini hesaplar.
 * Bu adresler, JMP komutlarının hedeflerini çözümlemek için kullanılır.
 * Bu fonksiyon genellikle semantik analiz sonrası veya optimizasyon öncesi çalışır.
 * Her Bessambly komutunun sanal boyutu burada varsayılıyor (örn: 1 birim).
 * @param ast_root Programın AST kök düğümü.
 * @param symbol_table Sembol tablosu.
 */
static void calculate_virtual_addresses(AstNode* ast_root, SymbolTable* symbol_table) {
    if (!ast_root || ast_root->type != AST_PROGRAM) return;

    uint32_t current_address = 0;
    for (size_t i = 0; i < ast_root->data.program.num_statements; i++) {
        AstNode* statement = ast_root->data.program.statements[i];
        
        if (statement->type == AST_LABEL_DECLARATION) {
            // Etiket adresini güncelle
            SymbolEntry* entry = symbol_table_lookup_symbol(symbol_table, statement->data.label_decl.name);
            if (entry) { // Semantik analizde zaten kontrol edildi, burada olmalı
                entry->address = current_address;
            }
            // Etiket bildirimi kendi başına bir komut boyutu kaplamaz, sonraki komutun adresidir.
        } else if (statement->type == AST_INSTRUCTION) {
            // Komutun sanal boyutunu varsayalım (örn: 1 birim veya komut/operand sayısına göre değişebilir)
            // Basitlik adına her komut 1 sanal adres birimi kaplasın.
            // Gerçekte, makine koduna dönüşünce instruction'ların farklı bayt boyutları olacaktır.
            statement->data.instruction.virtual_address = current_address; // Yeni alan eklemeyi unutmayın AST_Instruction'a
            current_address++; 
        }
    }
}


// --- Optimizasyon Geçişi Gerçeklemeleri ---

// Bu örnekte basitleştirilmiş bir yaklaşım izlenmiştir.
// Gerçek bir dead code elimination, kontrol akış grafiği (CFG) gerektirebilir.
int optimize_dead_code_elimination(AstNode* ast_root, SymbolTable* symbol_table) {
    if (!ast_root || ast_root->type != AST_PROGRAM) return 0;

    int changed = 0;
    // Geçici olarak yeni bir ifade listesi oluştur
    AstNode** new_statements = (AstNode**)malloc(sizeof(AstNode*) * ast_root->data.program.num_statements);
    if (!new_statements) {
        fprintf(stderr, "Hata: Ölü kod eleme için bellek tahsis edilemedi.\n");
        return 0;
    }
    size_t new_count = 0;

    // Bir atlama sonrası (JMP, JEQ vb.) ulaşılamayan kodları tespit etmeye çalışalım.
    // Bu basit bir "birincil" dead code elimination'dır.
    // Daha karmaşık durumlar için (örn: döngüler, çoklu atlamalar) Kontrol Akış Grafiği gerekir.

    int unreachable_mode = 0; // 1 ise mevcut kod bloğuna ulaşılamıyor demektir

    for (size_t i = 0; i < ast_root->data.program.num_statements; i++) {
        AstNode* current_statement = ast_root->data.program.statements[i];

        if (current_statement->type == AST_LABEL_DECLARATION) {
            // Bir etikete ulaşıldığında, ulaşılamayan moddan çıkılır
            unreachable_mode = 0;
            new_statements[new_count++] = current_statement;
        } else if (current_statement->type == AST_INSTRUCTION) {
            if (unreachable_mode) {
                // Bu komuta ulaşılamıyor, kaldır
                fprintf(stdout, "Optimizer: Ulaşılamayan komut kaldırıldı (%s, %d:%d).\n",
                        token_type_to_string(current_statement->data.instruction.opcode),
                        current_statement->line, current_statement->column);
                ast_node_free(current_statement); // Belleği serbest bırak
                changed = 1;
            } else {
                new_statements[new_count++] = current_statement;
                // JMP veya RET gibi kontrol akışını değiştiren bir komut mu?
                // Sonraki komutlara ulaşılamayabilir.
                if (current_statement->data.instruction.opcode == TOKEN_JMP ||
                    current_statement->data.instruction.opcode == TOKEN_RET ||
                    current_statement->data.instruction.opcode == TOKEN_SYSCALL // SYSCALL da genellikle programı sonlandırabilir veya başka bir yere dallanabilir
                   ) {
                    unreachable_mode = 1;
                }
            }
        }
    }

    // Orijinal ifade dizisini yeni diziyle değiştir
    if (changed) {
        free(ast_root->data.program.statements); // Orijinal diziyi serbest bırak
        ast_root->data.program.statements = (AstNode**)realloc(new_statements, sizeof(AstNode*) * new_count);
        ast_root->data.program.num_statements = new_count;
        // realloc NULL dönerse hata kontrolü yapılmalı
        if (!ast_root->data.program.statements && new_count > 0) {
            fprintf(stderr, "Hata: Ölü kod eleme sonrası bellek yeniden tahsis edilemedi.\n");
            // Ciddi bir hata, burada daha sağlam bir hata işleme gerekebilir
        }
    } else {
        free(new_statements); // Değişiklik yapılmadıysa tahsis edilen yeni diziyi serbest bırak
    }
    return changed;
}

int optimize_constant_folding(AstNode* ast_root) {
    if (!ast_root || ast_root->type != AST_PROGRAM) return 0;
    int changed = 0;

    for (size_t i = 0; i < ast_root->data.program.num_statements; i++) {
        AstNode* statement = ast_root->data.program.statements[i];
        if (statement->type == AST_INSTRUCTION) {
            AstInstruction* instr = &statement->data.instruction;

            // Örnek: ADD R_dest, constant1; MOV R_dest, constant2 gibi durumları basitleştir.
            // Bu, çok basit bir örnek, daha gelişmiş constant folding AST üzerinde birden fazla
            // komuta bakarak yapılabilir (örneğin, bir sonraki komutun da aynı kaydediciyi kullanıp kullanmadığı).
            
            // Eğer bir ikili işlem (ADD, SUB, MUL, DIV) ve ikinci operandı sabit ise
            if ((instr->opcode == TOKEN_ADD || instr->opcode == TOKEN_SUB ||
                 instr->opcode == TOKEN_MUL || instr->opcode == TOKEN_DIV) &&
                instr->num_operands == 2 &&
                (instr->operands[1].type == OP_INTEGER || instr->operands[1].type == OP_HEX_INTEGER)) {
                
                // İlk operandın kaydedici olduğunu varsayıyoruz (semantik analiz kontrol etti)
                // Bu optimizasyon için, ilk operandın da sabit bir değer taşıması gerekir
                // VEYA, daha önceki bir komutun sabit bir değere sahip olduğunu takip etmemiz gerekir.
                // Bu karmaşık bir Veri Akış Analizi (Data Flow Analysis) gerektirir.
                // Şimdilik, sadece doğrudan bir sabit atamayı düşünelim.

                // Basit bir örnek: MOV R0, 5; ADD R0, 3  -> MOV R0, 8
                // Bu optimizasyon için, R0'ın önceki değerini bilmemiz gerekir.
                // Bu, daha çok Data Flow Analysis'e girer ve burada uygulamak zor olabilir.
                // Şimdilik bu kısmı atlayalım veya daha sonraki IR aşamasında yapalım.

                // Bir alternatif: doğrudan bir "değişmez" komutunu optimize etme
                // Bessambly'de bu tür durumlar nasıl modellenir?
                // Örn: Eğer Bessambly'nin bir "Load Immediate" veya benzeri bir komutu olsaydı
                // MOV R0, 10
                // ADD R0, 5 (burada R0'ın değeri bilinmiyor, sadece JIT/RunTime'da bilinebilir)

                // En basit hali:
                // Eğer bir komutun tüm operandları sabit ise ve sonucu da sabit oluyorsa,
                // komutu doğrudan bir MOV komutuna dönüştürebiliriz.
                // Ama Bessambly komutları genellikle kaydedicilerle çalışır.
                // Bu optimizasyon, daha çok bir "Intermediate Representation (IR)" üzerinde daha kolay yapılır.
                // Bessambly AST'sinde bunu doğrudan yapmak zor çünkü kaydedici değerlerini takip etmiyoruz.
            }
        }
    }
    return changed;
}

// Bu optimizasyon, temel kontrol akışını anlamayı gerektirir.
int optimize_jump_threading(AstNode* ast_root, SymbolTable* symbol_table) {
    if (!ast_root || ast_root->type != AST_PROGRAM) return 0;
    if (symbol_table == NULL) {
        fprintf(stderr, "Hata: Jump threading için sembol tablosu gerekli.\n");
        return 0;
    }

    int changed = 0;
    calculate_virtual_addresses(ast_root, symbol_table); // Etiket adreslerini güncelleyelim

    for (size_t i = 0; i < ast_root->data.program.num_statements; i++) {
        AstNode* current_statement = ast_root->data.program.statements[i];

        if (current_statement->type == AST_INSTRUCTION &&
            (current_statement->data.instruction.opcode == TOKEN_JMP ||
             current_statement->data.instruction.opcode == TOKEN_JEQ ||
             current_statement->data.instruction.opcode == TOKEN_JNE ||
             current_statement->data.instruction.opcode == TOKEN_JLT ||
             current_statement->data.instruction.opcode == TOKEN_JGT)) {
            
            // Bu bir atlama komutu ve bir etiket referansı içeriyor mu? (Semantik analiz kontrol etti)
            if (current_statement->data.instruction.num_operands == 1 &&
                current_statement->data.instruction.operands[0].type == OP_LABEL_REF) {
                
                char* target_label_name = current_statement->data.instruction.operands[0].value.label_name;
                SymbolEntry* target_entry = symbol_table_lookup_symbol(symbol_table, target_label_name);

                if (target_entry) {
                    // Atladığı adresin hemen sonrasındaki komut bir JMP mi?
                    // target_entry->address, etiketin başladığı sanal adres
                    // Bu adresi, AstNode'lara eklediğimiz 'virtual_address' ile eşleştirmemiz gerekir.

                    // Bu optimizasyon, özellikle JMP'den sonra doğrudan bir etiket ve o etiketin 
                    // hemen altında başka bir JMP varsa uygulanır.
                    // Örnek: JMP LabelA; ... LabelA: JMP LabelB
                    // Bu durumun tespiti biraz karmaşıktır ve AST'nin düz bir listesi üzerinden zordur.
                    // Daha çok bir kontrol akış grafiği (CFG) gerektirir.
                    // Basit bir yaklaşım için: Eğer bir etiket sadece bir JMP'den sonra geliyorsa
                    // ve o etiketteki ilk komut da bir JMP ise.

                    // Bu optimizasyon için aşağıdaki mantık bir başlangıç noktasıdır:
                    // 1. Hedef etiketin hemen arkasındaki ifadenin bir JMP komutu olup olmadığını bul
                    AstNode* target_label_node = NULL;
                    size_t target_label_index = -1;
                    for (size_t k = 0; k < ast_root->data.program.num_statements; k++) {
                        if (ast_root->data.program.statements[k]->type == AST_LABEL_DECLARATION &&
                            strcmp(ast_root->data.program.statements[k]->data.label_decl.name, target_label_name) == 0) {
                            target_label_node = ast_root->data.program.statements[k];
                            target_label_index = k;
                            break;
                        }
                    }

                    if (target_label_node && (target_label_index + 1) < ast_root->data.program.num_statements) {
                        AstNode* next_statement = ast_root->data.program.statements[target_label_index + 1];
                        if (next_statement->type == AST_INSTRUCTION && next_statement->data.instruction.opcode == TOKEN_JMP) {
                            // "JMP LabelA; ... LabelA: JMP LabelB" durumu
                            // İlk JMP'yi doğrudan LabelB'ye atlayacak şekilde değiştir.
                            if (next_statement->data.instruction.num_operands == 1 &&
                                next_statement->data.instruction.operands[0].type == OP_LABEL_REF) {
                                
                                char* final_target_label = next_statement->data.instruction.operands[0].value.label_name;

                                // Mevcut JMP komutunun operandını değiştir
                                free(current_statement->data.instruction.operands[0].value.label_name); // Eski label adını free et
                                current_statement->data.instruction.operands[0].value.label_name = strdup(final_target_label); // Yeni label adını kopyala
                                
                                fprintf(stdout, "Optimizer: Atlama kısaltma yapıldı (%s, %d:%d -> %s).\n",
                                        token_type_to_string(current_statement->data.instruction.opcode),
                                        current_statement->line, current_statement->column, final_target_label);
                                changed = 1;
                            }
                        }
                    }
                }
            }
        }
    }
    return changed;
}


int perform_optimizations(Optimizer* optimizer, AstNode* ast_root, SymbolTable* symbol_table) {
    if (!optimizer || !ast_root || !symbol_table) {
        fprintf(stderr, "Hata: Optimizasyon için geçersiz giriş.\n");
        return 0;
    }

    int total_changes = 0;
    int iteration_changes;
    
    // Optimizasyonları birden fazla geçişte ve sıralı olarak uygulayabiliriz
    // Her bir optimizasyon geçişi, bir diğerinin daha fazla optimizasyon yapmasını sağlayabilir.
    // Bu nedenle bir "Fixed-Point Iteration" (Sabit Nokta İterasyonu) döngüsü kullanırız.
    // Hiçbir değişiklik yapılmayana kadar döngü devam eder.

    fprintf(stdout, "\n--- Bessambly Optimizasyon Başlatılıyor ---\n");

    // Optimizasyon öncesi sanal adresleri hesaplayın.
    // Bu kısım, semantik analizden sonra veya optimizasyonun başında yapılabilir.
    // Önemli: Bu fonksiyonun doğru çalışması için AST_Instruction yapısına 'virtual_address'
    // adında bir 'uint32_t' alanı eklemeniz gerekmektedir.
    // calculate_virtual_addresses(ast_root, symbol_table); 


    if (optimizer->optimization_level >= 1) { // Eğer optimizasyon seviyesi >= 1 ise
        do {
            iteration_changes = 0;

            // 1. Ölü Kod Eleme
            int dce_changes = optimize_dead_code_elimination(ast_root, symbol_table);
            if (dce_changes) {
                iteration_changes += dce_changes;
                total_changes += dce_changes;
                // Dead code elimination, etiket adreslerini değiştirebileceği için
                // adresleri yeniden hesaplamak gerekebilir.
                // calculate_virtual_addresses(ast_root, symbol_table);
            }

            // 2. Atlama Kısaltma (Jump Threading)
            int jt_changes = optimize_jump_threading(ast_root, symbol_table);
            if (jt_changes) {
                iteration_changes += jt_changes;
                total_changes += jt_changes;
            }

            // 3. Sabit Katlama (Şimdilik çok basit, ileri aşamalarda geliştirilmeli)
            int cf_changes = optimize_constant_folding(ast_root);
            if (cf_changes) {
                iteration_changes += cf_changes;
                total_changes += cf_changes;
            }
            
            // Diğer optimizasyon geçişleri buraya eklenebilir.

        } while (iteration_changes > 0); // Hiçbir değişiklik yapılmayana kadar tekrarla
    }

    if (total_changes > 0) {
        fprintf(stdout, "Toplam %d optimizasyon değişikliği yapıldı.\n", total_changes);
    } else {
        fprintf(stdout, "Hiçbir optimizasyon değişikliği yapılmadı.\n");
    }

    fprintf(stdout, "Optimizasyon başarıyla tamamlandı.\n");
    return 1; // Başarılı
}