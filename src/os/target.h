#ifndef TARGET_H
#define TARGET_H

#include <stdint.h> // uint32_t, uint64_t için

// --- Hedef CPU Mimarileri ---
typedef enum {
    ARCH_AMD64,    // 64-bit x86_64 (amd64)
    ARCH_AMD32,    // 32-bit IA-32 (amd64)
    ARCH_ARMV9,    // 64-bit ARM mimarisi (armv9)
    ARCH_ARMV8,    // 64-bit ARM mimarisi (armv8)
    ARCH_ARMV7,    // 32-bit ARM mimarisi (armv7)
    ARCH_RV64I,    // genel amaçlı 64 bit RISC-V mimarisi (rv64i)
    ARCH_RV64E,    // gömülü sistem 64 bit RISC-V mimarisi (rv64e)
    ARCH_RV32I,    // genel amaçlı 32 bit RISC-V mimarisi (rv32i)
    ARCH_RV32E,    // gömülü sistem 32 bit RISC-V mimarisi (rv32e)
    ARCH_POWERPC64,    // 64-bit PowerPC mimarisi (powerpc64)
    ARCH_POWERPC32,    // 32-bit PowerPC mimarisi (powerpc64)
    ARCH_MIPS64,    // 64-bit MIPS mimarisi (mips64)
    ARCH_MIPS32,    // 32-bit MIPS mimarisi (mips32)
    ARCH_MICRO_MIPS, // Micro MIPS mimarisi (micro_mips)
    ARCH_OPENRISC64,    // 64-bit OpenRISC mimarisi (openrisc64)
    ARCH_OPENRISC32,    // 32-bit OpenRISC mimarisi (openrisc32)
    ARCH_LOONGARCH64,    // 64-bit LoongArch mimarisi (loongarch64)
    ARCH_LOONGARCH32,    // 32-bit LoongArch mimarisi (loongarch32)
    ARCH_SPARCV9,    // 64-bit SPARC mimarisi (sparcv9)
    ARCH_SPARCV8,    // 32-bit SPARC mimarisi (sparcv8)
    ARCH_SPARCV7,    // 32-bit SPARC mimarisi (sparcv7)
    UNKNOWN_ARCH    // Bilinmeyen mimari
} TargetArchitecture;

// --- Hedef İşletim Sistemleri ---
typedef enum {
    OS_LINUX,       // Linux işletim sistemi
    OS_WINDOWS,     // Windows işletim sistemi
    OS_BAREMETAL,   // Çıplak Metal programlama
    OS_ANDROID,     // Android işletim sistemi
    OS_IOS,         // iOS işletim sistemi
    OS_MACOS,       // MacOS işletim sistemi
    OS_RISCOS,      // RISCOS işletim sistemi
    OS_HAIKU,       // Haiku işletim sistemi
    OS_KOLIBRIOS,   // KolibriOS işletim sistemi
    OS_REACTOS,     // ReactOS işletim sistemi
    OS_UNIX,        // Orijinal Unix işletim sistemi
    UNKNOWN_OS      // Bilinmeyen işletim sistemi
} TargetOperatingSystem;

// --- Genel Hedef Yapılandırma Yapısı ---
// Bu yapı, seçilen işletim sistemi için yüklenen spesifik yapılandırmayı işaret eder.
typedef struct {
    TargetArchitecture arch;
    TargetOperatingSystem os;

    // Hedef işletim sistemine ve mimariye özgü yapılandırma bilgileri için bir pointer.
    // Bu, seçilen OS/mimari kombinasyonuna göre farklı bir struct'a işaret edecektir.
    void* os_arch_config_data;

} Target;

// --- Fonksiyon Prototipleri ---

/**
 * @brief Yeni bir Target örneği başlatır ve belirtilen OS/mimari için yapılandırmayı yükler.
 * @param arch Hedef CPU mimarisi.
 * @param os Hedef İşletim Sistemi.
 * @return Başlatılmış Target pointer'ı veya NULL hata durumunda.
 */
Target* target_init(TargetArchitecture arch, TargetOperatingSystem os);

/**
 * @brief Target yapısının kullandığı belleği serbest bırakır.
 * Aynı zamanda OS/mimariye özgü yapılandırma verilerini de serbest bırakır.
 * @param target Serbest bırakılacak Target pointer'ı.
 */
void target_close(Target* target);

/**
 * @brief Bir TargetArchitecture enum değerini string'e dönüştürür.
 * @param arch Dönüştürülecek mimari.
 * @return Mimarinin string karşılığı.
 */
const char* target_arch_to_string(TargetArchitecture arch);

/**
 * @brief Bir TargetOperatingSystem enum değerini string'e dönüştürür.
 * @param os Dönüştürülecek işletim sistemi.
 * @return İşletim sisteminin string karşılığı.
 */
const char* target_os_to_string(TargetOperatingSystem os);

#endif // TARGET_H
