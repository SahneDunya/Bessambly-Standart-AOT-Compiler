#include "ast.h"
#include <stdlib.h> // malloc, free
#include <string.h> // strdup (isteğe bağlı, daha güvenli kopyalama yapılmalı)
#include <stdio.h>  // fprintf for error messages

// --- Fonksiyon Gerçeklemeleri ---

AstNode* ast_node_create(AstNodeType type, int line, int column) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!node) {
        fprintf(stderr, "Hata: AST düğümü için bellek tahsis edilemedi (tip: %d).\n", type);
        return NULL;
    }
    node->type = type;
    node->line = line;
    node->column = column;

    // Union alanlarını başlat
    switch (type) {
        case AST_PROGRAM:
            node->data.program.num_statements = 0;
            node->data.program.statements = NULL;
            break;
        case AST_LABEL_DECLARATION:
            node->data.label_decl.name = NULL;
            break;
        case AST_INSTRUCTION:
            node->data.instruction.opcode = TOKEN_UNKNOWN; // Başlangıç değeri
            node->data.instruction.num_operands = 0;
            node->data.instruction.operands = NULL;
            break;
        case AST_REGISTER_OPERAND:
        case AST_INTEGER_OPERAND:
        case AST_IDENTIFIER_OPERAND:
        case AST_SYSCALL_ARGUMENT:
            // Bu tür düğümlerin verileri genellikle 'data' union'ında değil,
            // AstOperand struct'ında depolanır ve InstructionNode içinde kullanılır.
            // Eğer bunları ayrı AST düğümleri olarak modellediyseniz buraya ekleme yapın.
            break;
    }
    return node;
}

AstOperand* ast_operand_create(OperandType type) {
    AstOperand* operand = (AstOperand*)malloc(sizeof(AstOperand));
    if (!operand) {
        fprintf(stderr, "Hata: AST operand için bellek tahsis edilemedi (tip: %d).\n", type);
        return NULL;
    }
    operand->type = type;
    // Union alanlarını başlat
    switch (type) {
        case OP_REGISTER:
            operand->value.reg_index = -1;
            break;
        case OP_INTEGER:
        case OP_HEX_INTEGER:
            operand->value.int_value = 0;
            break;
        case OP_LABEL_REF:
            operand->value.label_name = NULL;
            break;
    }
    return operand;
}


void ast_operand_free(AstOperand* operand) {
    if (operand) {
        if (operand->type == OP_LABEL_REF && operand->value.label_name) {
            free(operand->value.label_name);
        }
        free(operand);
    }
}

void ast_node_free(AstNode* node) {
    if (!node) {
        return;
    }

    switch (node->type) {
        case AST_PROGRAM:
            if (node->data.program.statements) {
                for (size_t i = 0; i < node->data.program.num_statements; i++) {
                    ast_node_free(node->data.program.statements[i]); // Alt düğümleri rekürsif olarak serbest bırak
                }
                free(node->data.program.statements);
            }
            break;
        case AST_LABEL_DECLARATION:
            if (node->data.label_decl.name) {
                free(node->data.label_decl.name);
            }
            break;
        case AST_INSTRUCTION:
            if (node->data.instruction.operands) {
                for (size_t i = 0; i < node->data.instruction.num_operands; i++) {
                    ast_operand_free(&node->data.instruction.operands[i]); // Operandları serbest bırak
                }
                free(node->data.instruction.operands);
            }
            break;
        // Operand düğümleri (eğer ayrı düğümler olarak tanımlandıysa) burada serbest bırakılmalı
        // Şu anki modelde operandlar InstructionNode'un bir parçası olduğu için bu kısım gerekli değil.
        case AST_REGISTER_OPERAND:
        case AST_INTEGER_OPERAND:
        case AST_IDENTIFIER_OPERAND:
        case AST_SYSCALL_ARGUMENT:
             // Bu tür düğümlerin özel serbest bırakma mantığı yoksa burası boş kalabilir
             // veya AstOperand serbest bırakma mantığı buraya entegre edilebilir.
             // Ancak mevcut modelde bu düğüm tipleri doğrudan AstOperand'a karşılık gelmiyor
             // ve Instruction içinde AstOperand dizisi olarak tutuluyor.
            break;
    }

    free(node); // Düğümün kendisini serbest bırak
}