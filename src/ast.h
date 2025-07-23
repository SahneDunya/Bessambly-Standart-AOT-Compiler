#ifndef AST_H
#define AST_H

#include <stdint.h> // int64_t için
#include <stddef.h> // size_t için
#include "lexer.h" // Token türlerine erişim için

// --- AST Düğüm Türleri (AstNodeType) ---
// Bessambly'deki her farklı yapısal öğeyi temsil eder.
typedef enum {
    AST_PROGRAM,            // Bessambly programının kök düğümü
    AST_LABEL_DECLARATION,  // Etiket tanımı (örn: "MY_LABEL:")
    AST_INSTRUCTION,        // Bir komut (örn: MOV, ADD, JMP)
    AST_REGISTER_OPERAND,   // Kaydedici operandı (örn: R0, R15)
    AST_INTEGER_OPERAND,    // Tamsayı değişmez operandı (örn: 123, 0xABC)
    AST_IDENTIFIER_OPERAND, // Tanımlayıcı operandı (örn: etiket adı, değişken adı)
    AST_SYSCALL_ARGUMENT,   // Sistem çağrısı argümanı (bu da bir operand türüdür)
    // ... (gelecekte eklenebilecek diğer AST düğüm türleri)

} AstNodeType;

// --- Operand Türleri (OperandType) ---
// Bir komutun operandının ne tür bir değer taşıdığını belirtir.
typedef enum {
    OP_REGISTER,        // Kaydedici (örn: R0)
    OP_INTEGER,         // Tamsayı değişmezi (örn: 123)
    OP_HEX_INTEGER,     // Onaltılık tamsayı değişmezi (örn: 0xABC)
    OP_LABEL_REF,       // Etiket referansı (örn: JMP HedefEtiket)
} OperandType;

// --- Operand Yapısı ---
// Bir komutun aldığı parametreleri temsil eder.
typedef struct {
    OperandType type;       // Operandın türü
    union {                 // Operandın türüne göre depolanan değer
        int reg_index;      // Eğer OP_REGISTER ise kaydedici indeksi
        int64_t int_value;  // Eğer OP_INTEGER veya OP_HEX_INTEGER ise tamsayı değeri
        char* label_name;   // Eğer OP_LABEL_REF ise etiket adı
    } value;
} AstOperand;

// --- Komut Yapısı (InstructionNode) ---
// Bessambly'deki tek bir komutu (opcode ve operandları) temsil eder.
typedef struct {
    TokenType opcode;       // Komutun türü (MOV, ADD, JMP vb. lexer'daki TokenType'dan alınır)
    size_t num_operands;    // Komutun aldığı operand sayısı
    AstOperand* operands;   // Operandların dinamik dizisi
} AstInstruction;

// --- Etiket Bildirimi Yapısı (LabelDeclarationNode) ---
// Bir etiket tanımını temsil eder (örn: 'MY_LABEL:').
typedef struct {
    char* name;             // Etiketin adı
} AstLabelDeclaration;

// --- Temel AST Düğümü ---
// Tüm AST düğümlerinin temelini oluşturur. Polymorphic bir yapı sağlar.
typedef struct AstNode {
    AstNodeType type;       // Düğümün türü
    int line;               // Kaynak koddaki satır numarası
    int column;             // Kaynak koddaki sütun numarası
    
    // Düğümün türüne göre farklı veri yapılarına işaret eden union
    union {
        // AST_PROGRAM için:
        struct {
            size_t num_statements;     // Programdaki ifade (statement) sayısı
            struct AstNode** statements; // İfade düğümlerinin dizisi (Instruction veya LabelDeclaration olabilir)
        } program;
        
        // AST_LABEL_DECLARATION için:
        AstLabelDeclaration label_decl;

        // AST_INSTRUCTION için:
        AstInstruction instruction;

        // AST_REGISTER_OPERAND için:
        // AstOperand struct'ı doğrudan kullanılabilir veya buraya uygun bir alan eklenebilir.
        // Şimdilik AstOperand'ı ayrı bir struct olarak tanımladık ve instruction içinde kullanacağız.
        // Ancak operandlar da kendi başlarına birer AST düğümü olarak tanımlanabilir.
        // Basitlik adına, operandları şimdilik doğrudan InstructionNode'un parçası yapalım.
        // Eğer operandları ayrı AST düğümleri olarak modellemek isterseniz, bu union'a eklersiniz.
        // Örnek: AstOperand operand_data;
    } data;

} AstNode;

// --- Fonksiyon Prototipleri ---

/**
 * @brief Yeni bir AST düğümü oluşturur ve türünü ayarlar.
 * @param type Düğümün türü.
 * @param line Kaynak koddaki satır numarası.
 * @param column Kaynak koddaki sütun numarası.
 * @return Yeni AstNode pointer'ı veya NULL bellek hatası durumunda.
 */
AstNode* ast_node_create(AstNodeType type, int line, int column);

/**
 * @brief Yeni bir AstOperand yapısı oluşturur.
 * @param type Operandın türü.
 * @return Yeni AstOperand pointer'ı veya NULL bellek hatası durumunda.
 */
AstOperand* ast_operand_create(OperandType type);

/**
 * @brief Bir AST düğümünü ve alt düğümlerini serbest bırakır.
 * @param node Serbest bırakılacak AST düğümü.
 */
void ast_node_free(AstNode* node);

/**
 * @brief Bir AstOperand'ı ve içerdiği belleği serbest bırakır.
 * @param operand Serbest bırakılacak operand.
 */
void ast_operand_free(AstOperand* operand);

#endif // AST_H
