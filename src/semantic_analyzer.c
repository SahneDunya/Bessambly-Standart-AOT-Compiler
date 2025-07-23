#include "semantic_analyzer.h"
#include <stdlib.h> // malloc, free, realloc
#include <string.h> // strdup, strcmp
#include <stdio.h>  // fprintf

// --- Sembol Tablosu Gerçeklemeleri ---

SymbolTable* symbol_table_init() {
    SymbolTable* table = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (!table) {
        fprintf(stderr, "Hata: Sembol tablosu için bellek tahsis edilemedi.\n");
        return NULL;
    }
    table->count = 0;
    table->capacity = 16; // Başlangıç kapasitesi
    table->entries = (SymbolEntry*)malloc(sizeof(SymbolEntry) * table->capacity);
    if (!table->entries) {
        fprintf(stderr, "Hata: Sembol tablosu girdileri için bellek tahsis edilemedi.\n");
        free(table);
        return NULL;
    }
    return table;
}

void symbol_entry_free(SymbolEntry* entry) {
    if (entry && entry->name) {
        free(entry->name);
    }
}

void symbol_table_free(SymbolTable* table) {
    if (table) {
        if (table->entries) {
            for (size_t i = 0; i < table->count; i++) {
                symbol_entry_free(&table->entries[i]);
            }
            free(table->entries);
        }
        free(table);
    }
}

int symbol_table_add_symbol(SymbolTable* table, const char* name, uint32_t address, int line, int column) {
    // Sembolün zaten tanımlı olup olmadığını kontrol et (tekrar tanım hatası)
    if (symbol_table_lookup_symbol(table, name) != NULL) {
        fprintf(stderr, "Hata (%d:%d): '%s' etiketi zaten tanımlı.\n", line, column, name);
        return 0; // Hata: Sembol zaten var
    }

    // Kapasite kontrolü ve genişletme
    if (table->count >= table->capacity) {
        table->capacity *= 2;
        SymbolEntry* new_entries = (SymbolEntry*)realloc(table->entries, sizeof(SymbolEntry) * table->capacity);
        if (!new_entries) {
            fprintf(stderr, "Hata: Sembol tablosu kapasitesi genişletilemedi.\n");
            return 0; // Bellek hatası
        }
        table->entries = new_entries;
    }

    SymbolEntry* new_entry = &table->entries[table->count];
    new_entry->name = strdup(name); // Adı kopyala
    if (!new_entry->name) {
        fprintf(stderr, "Hata: Sembol adı için bellek tahsis edilemedi.\n");
        return 0; // Bellek hatası
    }
    new_entry->address = address;
    new_entry->line = line;
    new_entry->column = column;
    table->count++;
    return 1; // Başarılı
}

SymbolEntry* symbol_table_lookup_symbol(SymbolTable* table, const char* name) {
    for (size_t i = 0; i < table->count; i++) {
        if (strcmp(table->entries[i].name, name) == 0) {
            return &table->entries[i];
        }
    }
    return NULL; // Bulunamadı
}

// --- Semantik Analizci Gerçeklemeleri ---

SemanticAnalyzer* semantic_analyzer_init() {
    SemanticAnalyzer* analyzer = (SemanticAnalyzer*)malloc(sizeof(SemanticAnalyzer));
    if (!analyzer) {
        fprintf(stderr, "Hata: SemanticAnalyzer için bellek tahsis edilemedi.\n");
        return NULL;
    }
    analyzer->symbol_table = symbol_table_init();
    if (!analyzer->symbol_table) {
        free(analyzer);
        return NULL;
    }
    analyzer->has_error = 0;
    return analyzer;
}

void semantic_analyzer_close(SemanticAnalyzer* analyzer) {
    if (analyzer) {
        symbol_table_free(analyzer->symbol_table);
        free(analyzer);
    }
}

/**
 * @brief AST üzerinde etiket tanımlamalarını toplar ve sembol tablosuna ekler.
 * Bu birinci geçiştir (first pass).
 * @param analyzer SemanticAnalyzer pointer'ı.
 * @param node Şu anki işlenen AST düğümü.
 */
static void collect_label_declarations(SemanticAnalyzer* analyzer, AstNode* node) {
    if (!node || analyzer->has_error) return;

    if (node->type == AST_PROGRAM) {
        for (size_t i = 0; i < node->data.program.num_statements; i++) {
            collect_label_declarations(analyzer, node->data.program.statements[i]);
            if (analyzer->has_error) return;
        }
    } else if (node->type == AST_LABEL_DECLARATION) {
        if (!symbol_table_add_symbol(analyzer->symbol_table,
                                     node->data.label_decl.name,
                                     0, // Adres bilgisi daha sonra hesaplanacak
                                     node->line,
                                     node->column)) {
            analyzer->has_error = 1; // Hata durumunda bayrağı ayarla
        }
    }
    // Komutlar bu aşamada işlenmez.
}

/**
 * @brief AST üzerinde etiket referanslarını ve operandları doğrular.
 * Bu ikinci geçiştir (second pass).
 * @param analyzer SemanticAnalyzer pointer'ı.
 * @param node Şu anki işlenen AST düğümü.
 */
static void validate_references_and_operands(SemanticAnalyzer* analyzer, AstNode* node) {
    if (!node || analyzer->has_error) return;

    if (node->type == AST_PROGRAM) {
        for (size_t i = 0; i < node->data.program.num_statements; i++) {
            validate_references_and_operands(analyzer, node->data.program.statements[i]);
            if (analyzer->has_error) return;
        }
    } else if (node->type == AST_INSTRUCTION) {
        // Her komut için operandları kontrol et
        AstInstruction* instr = &node->data.instruction;

        // Örnek: JMP komutu sadece bir etiket referansı almalı
        // Bessambly'nin tam talimat setini ve operand kısıtlamalarını burada tanımlamalısınız.
        // Bu örnekler, temel mantığı gösterir.

        if (instr->opcode == TOKEN_JMP || instr->opcode == TOKEN_JEQ ||
            instr->opcode == TOKEN_JNE || instr->opcode == TOKEN_JLT ||
            instr->opcode == TOKEN_JGT) {
            if (instr->num_operands != 1 || instr->operands[0].type != OP_LABEL_REF) {
                fprintf(stderr, "Hata (%d:%d): '%s' komutu bir etiket referansı operandı bekliyor.\n",
                        node->line, node->column, token_type_to_string(instr->opcode));
                analyzer->has_error = 1;
            } else {
                // Etiket referansının sembol tablosunda tanımlı olup olmadığını kontrol et
                if (symbol_table_lookup_symbol(analyzer->symbol_table, instr->operands[0].value.label_name) == NULL) {
                    fprintf(stderr, "Hata (%d:%d): Tanımlanmamış etiket referansı '%s'.\n",
                            node->line, node->column, instr->operands[0].value.label_name);
                    analyzer->has_error = 1;
                }
            }
        }
        // Örnek: MOV R0, 10 veya MOV R0, R1
        else if (instr->opcode == TOKEN_MOV || instr->opcode == TOKEN_ADD ||
                 instr->opcode == TOKEN_SUB || instr->opcode == TOKEN_MUL ||
                 instr->opcode == TOKEN_DIV) {
            if (instr->num_operands != 2) {
                fprintf(stderr, "Hata (%d:%d): '%s' komutu iki operand bekliyor.\n",
                        node->line, node->column, token_type_to_string(instr->opcode));
                analyzer->has_error = 1;
            } else {
                // İlk operandın kaydedici olması beklenir
                if (instr->operands[0].type != OP_REGISTER) {
                    fprintf(stderr, "Hata (%d:%d): '%s' komutunun ilk operandı kaydedici olmalı.\n",
                            node->line, node->column, token_type_to_string(instr->opcode));
                    analyzer->has_error = 1;
                }
                // İkinci operand kaydedici veya sabit olabilir
                if (instr->operands[1].type != OP_REGISTER &&
                    instr->operands[1].type != OP_INTEGER &&
                    instr->operands[1].type != OP_HEX_INTEGER) {
                    fprintf(stderr, "Hata (%d:%d): '%s' komutunun ikinci operandı kaydedici veya sabit olmalı.\n",
                            node->line, node->column, token_type_to_string(instr->opcode));
                    analyzer->has_error = 1;
                }
                // Register index kontrolü (örneğin R0-R15 arası)
                if (instr->operands[0].type == OP_REGISTER &&
                    (instr->operands[0].value.reg_index < 0 || instr->operands[0].value.reg_index > 15)) {
                    fprintf(stderr, "Hata (%d:%d): Geçersiz kaydedici R%d. (0-15 arası bekleniyor)\n",
                            node->line, node->column, instr->operands[0].value.reg_index);
                    analyzer->has_error = 1;
                }
                if (instr->operands[1].type == OP_REGISTER &&
                    (instr->operands[1].value.reg_index < 0 || instr->operands[1].value.reg_index > 15)) {
                    fprintf(stderr, "Hata (%d:%d): Geçersiz kaydedici R%d. (0-15 arası bekleniyor)\n",
                            node->line, node->column, instr->operands[1].value.reg_index);
                    analyzer->has_error = 1;
                }
            }
        }
        // Örnek: SYSCALL
        else if (instr->opcode == TOKEN_SYSCALL) {
            // SYSCALL'ın ilk operandı bir tamsayı (sistem çağrı numarası) olmalı
            if (instr->num_operands < 1 ||
                (instr->operands[0].type != OP_INTEGER && instr->operands[0].type != OP_HEX_INTEGER)) {
                fprintf(stderr, "Hata (%d:%d): SYSCALL komutu geçerli bir sistem çağrı numarası bekliyor.\n",
                        node->line, node->column);
                analyzer->has_error = 1;
            }
            // Diğer operandlar (eğer varsa) kaydedici olmalı
            for (size_t i = 1; i < instr->num_operands; i++) {
                if (instr->operands[i].type != OP_REGISTER) {
                    fprintf(stderr, "Hata (%d:%d): SYSCALL komutunun argümanları (ilk hariç) kaydedici olmalı.\n",
                            node->line, node->column);
                    analyzer->has_error = 1;
                    break;
                }
            }
        }
        // Örnek: RET
        else if (instr->opcode == TOKEN_RET) {
            if (instr->num_operands != 0) {
                fprintf(stderr, "Hata (%d:%d): RET komutu hiçbir operand beklememektedir.\n",
                        node->line, node->column);
                analyzer->has_error = 1;
            }
        }
        // Diğer komutlar için de kısıtlamalar eklenebilir.
    }
}


int perform_semantic_analysis(SemanticAnalyzer* analyzer, AstNode* ast_root) {
    if (!analyzer || !ast_root) {
        fprintf(stderr, "Hata: Semantik analiz için geçersiz giriş.\n");
        return 0;
    }

    // Birinci Geçiş: Tüm etiket tanımlamalarını topla ve sembol tablosuna ekle
    fprintf(stdout, "Semantik Analiz: Birinci geçiş (Etiket tanımlarını toplama)...\n");
    collect_label_declarations(analyzer, ast_root);

    if (analyzer->has_error) {
        fprintf(stderr, "Semantik analizde birinci geçişte hatalar bulundu.\n");
        return 0;
    }
    fprintf(stdout, "Semantik Analiz: Birinci geçiş tamamlandı. Toplanan etiketler:\n");
    for (size_t i = 0; i < analyzer->symbol_table->count; i++) {
        fprintf(stdout, "  - '%s' (Tanım: %d:%d)\n",
                analyzer->symbol_table->entries[i].name,
                analyzer->symbol_table->entries[i].line,
                analyzer->symbol_table->entries[i].column);
    }


    // İkinci Geçiş: Etiket referanslarını ve komut operandlarını doğrula
    fprintf(stdout, "Semantik Analiz: İkinci geçiş (Referansları ve operandları doğrulama)...\n");
    validate_references_and_operands(analyzer, ast_root);

    if (analyzer->has_error) {
        fprintf(stderr, "Semantik analizde ikinci geçişte hatalar bulundu.\n");
        return 0;
    }

    fprintf(stdout, "Semantik analiz başarıyla tamamlandı. Hata yok.\n");
    return 1; // Başarılı
}
