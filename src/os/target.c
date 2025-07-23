#include "target.h"
#include <stdlib.h> // malloc, free
#include <stdio.h>  // fprintf
#include <string.h> // strcmp

// Her bir işletim sistemi ve desteklediği mimariler için özel başlık dosyalarını dahil ediyoruz.
// Bu kısım, her yeni OS/Mimari kombinasyonu için güncellenmelidir.

// LINUX
#include "os/linux/target_config_amd64.h"
#include "os/linux/target_config_amd32.h"
#include "os/linux/target_config_armv9.h"
#include "os/linux/target_config_armv8.h"
#include "os/linux/target_config_armv7.h"
#include "os/linux/target_config_rv64i.h"
#include "os/linux/target_config_rv64e.h"
#include "os/linux/target_config_rv32i.h"
#include "os/linux/target_config_rv32e.h"
#include "os/linux/target_config_powerpc64.h"
#include "os/linux/target_config_powerpc32.h"
#include "os/linux/target_config_mips64.h"
#include "os/linux/target_config_mips32.h"
#include "os/linux/target_config_micro_mips.h"
#include "os/linux/target_config_openrisc64.h"
#include "os/linux/target_config_openrisc32.h"
#include "os/linux/target_config_loongarch64.h"
#include "os/linux/target_config_loongarch32.h"
#include "os/linux/target_config_sparcv9.h"
#include "os/linux/target_config_sparcv8.h"
#include "os/linux/target_config_sparcv7.h"

// WINDOWS
#include "os/windows/target_config_amd64.h"
#include "os/windows/target_config_amd32.h"
#include "os/windows/target_config_armv9.h"
#include "os/windows/target_config_armv8.h"

// BAREMETAL
#include "os/baremetal/target_config_amd64.h"
#include "os/baremetal/target_config_amd32.h"
#include "os/baremetal/target_config_armv9.h"
#include "os/baremetal/target_config_armv8.h"
#include "os/baremetal/target_config_armv7.h"
#include "os/baremetal/target_config_rv64i.h"
#include "os/baremetal/target_config_rv64e.h"
#include "os/baremetal/target_config_rv32i.h"
#include "os/baremetal/target_config_rv32e.h"
#include "os/baremetal/target_config_powerpc64.h"
#include "os/baremetal/target_config_powerpc32.h"
#include "os/baremetal/target_config_mips64.h"
#include "os/baremetal/target_config_mips32.h"
#include "os/baremetal/target_config_micro_mips.h"
#include "os/baremetal/target_config_openrisc64.h"
#include "os/baremetal/target_config_openrisc32.h"
#include "os/baremetal/target_config_loongarch64.h"
#include "os/baremetal/target_config_loongarch32.h"
#include "os/baremetal/target_config_sparcv9.h"
#include "os/baremetal/target_config_sparcv8.h"
#include "os/baremetal/target_config_sparcv7.h"

// ANDROID
#include "os/android/target_config_amd64.h"
#include "os/android/target_config_amd32.h"
#include "os/android/target_config_armv9.h"
#include "os/android/target_config_armv8.h"
#include "os/android/target_config_armv7.h"

// IOS
#include "os/ios/target_config_armv9.h"
#include "os/ios/target_config_armv8.h"
#include "os/ios/target_config_armv7.h"

// MACOS
#include "os/macos/target_config_amd64.h"
#include "os/macos/target_config_amd32.h"
#include "os/macos/target_config_armv9.h"

// RISCOS
#include "os/riscos/target_config_armv7.h"

// HAIKU
#include "os/haiku/target_config_amd64.h" // Varsayılan olarak amd64 destekliyor kabul edelim
#include "os/haiku/target_config_amd32.h" // Veya diğer mimariler için de ekleyin

// KOLIBRIOS
#include "os/kolibrios/target_config_amd64.h"
#include "os/kolibrios/target_config_amd32.h"

// REACTOS
#include "os/reactos/target_config_amd64.h"
#include "os/reactos/target_config_amd32.h"
#include "os/reactos/target_config_armv7.h"

// UNIX (Tüm mimarileri destekler)
#include "os/unix/target_config_amd64.h"
#include "os/unix/target_config_amd32.h"
#include "os/unix/target_config_armv9.h"
#include "os/unix/target_config_armv8.h"
#include "os/unix/target_config_armv7.h"
#include "os/unix/target_config_rv64i.h"
#include "os/unix/target_config_rv64e.h"
#include "os/unix/target_config_rv32i.h"
#include "os/unix/target_config_rv32e.h"
#include "os/unix/target_config_powerpc64.h"
#include "os/unix/target_config_powerpc32.h"
#include "os/unix/target_config_mips64.h"
#include "os/unix/target_config_mips32.h"
#include "os/unix/target_config_micro_mips.h"
#include "os/unix/target_config_openrisc64.h"
#include "os/unix/target_config_openrisc32.h"
#include "os/unix/target_config_loongarch64.h"
#include "os/unix/target_config_loongarch32.h"
#include "os/unix/target_config_sparcv9.h"
#include "os/unix/target_config_sparcv8.h"
#include "os/unix/target_config_sparcv7.h"


// Yardımcı fonksiyon: Belirli bir OS ve mimari için yapılandırma yükle
static void* load_os_arch_config(TargetOperatingSystem os, TargetArchitecture arch) {
    void* config_data = NULL;
    switch (os) {
        case OS_LINUX:
            switch (arch) {
                case ARCH_AMD64: config_data = linux_target_config_amd64_init(); break;
                case ARCH_AMD32: config_data = linux_target_config_amd32_init(); break;
                case ARCH_ARMV9: config_data = linux_target_config_armv9_init(); break;
                case ARCH_ARMV8: config_data = linux_target_config_armv8_init(); break;
                case ARCH_ARMV7: config_data = linux_target_config_armv7_init(); break;
                case ARCH_RV64I: config_data = linux_target_config_rv64i_init(); break;
                case ARCH_RV64E: config_data = linux_target_config_rv64e_init(); break;
                case ARCH_RV32I: config_data = linux_target_config_rv32i_init(); break;
                case ARCH_RV32E: config_data = linux_target_config_rv32e_init(); break;
                case ARCH_POWERPC64: config_data = linux_target_config_powerpc64_init(); break;
                case ARCH_POWERPC32: config_data = linux_target_config_powerpc32_init(); break;
                case ARCH_MIPS64: config_data = linux_target_config_mips64_init(); break;
                case ARCH_MIPS32: config_data = linux_target_config_mips32_init(); break;
                case ARCH_MICRO_MIPS: config_data = linux_target_config_micro_mips_init(); break;
                case ARCH_OPENRISC64: config_data = linux_target_config_openrisc64_init(); break;
                case ARCH_OPENRISC32: config_data = linux_target_config_openrisc32_init(); break;
                case ARCH_LOONGARCH64: config_data = linux_target_config_loongarch64_init(); break;
                case ARCH_LOONGARCH32: config_data = linux_target_config_loongarch32_init(); break;
                case ARCH_SPARCV9: config_data = linux_target_config_sparcv9_init(); break;
                case ARCH_SPARCV8: config_data = linux_target_config_sparcv8_init(); break;
                case ARCH_SPARCV7: config_data = linux_target_config_sparcv7_init(); break;
                default: fprintf(stderr, "Hata: Linux için desteklenmeyen mimari: %s\n", target_arch_to_string(arch)); break;
            }
            break;
        case OS_WINDOWS:
            switch (arch) {
                case ARCH_AMD64: config_data = windows_target_config_amd64_init(); break;
                case ARCH_AMD32: config_data = windows_target_config_amd32_init(); break;
                case ARCH_ARMV9: config_data = windows_target_config_armv9_init(); break;
                case ARCH_ARMV8: config_data = windows_target_config_armv8_init(); break;
                default: fprintf(stderr, "Hata: Windows için desteklenmeyen mimari: %s\n", target_arch_to_string(arch)); break;
            }
            break;
        case OS_BAREMETAL:
            switch (arch) { // Baremetal tüm mimarileri destekler olarak kabul edildi
                case ARCH_AMD64: config_data = baremetal_target_config_amd64_init(); break;
                case ARCH_AMD32: config_data = baremetal_target_config_amd32_init(); break;
                case ARCH_ARMV9: config_data = baremetal_target_config_armv9_init(); break;
                case ARCH_ARMV8: config_data = baremetal_target_config_armv8_init(); break;
                case ARCH_ARMV7: config_data = baremetal_target_config_armv7_init(); break;
                case ARCH_RV64I: config_data = baremetal_target_config_rv64i_init(); break;
                case ARCH_RV64E: config_data = baremetal_target_config_rv64e_init(); break;
                case ARCH_RV32I: config_data = baremetal_target_config_rv32i_init(); break;
                case ARCH_RV32E: config_data = baremetal_target_config_rv32e_init(); break;
                case ARCH_POWERPC64: config_data = baremetal_target_config_powerpc64_init(); break;
                case ARCH_POWERPC32: config_data = baremetal_target_config_powerpc32_init(); break;
                case ARCH_MIPS64: config_data = baremetal_target_config_mips64_init(); break;
                case ARCH_MIPS32: config_data = baremetal_target_config_mips32_init(); break;
                case ARCH_MICRO_MIPS: config_data = baremetal_target_config_micro_mips_init(); break;
                case ARCH_OPENRISC64: config_data = baremetal_target_config_openrisc64_init(); break;
                case ARCH_OPENRISC32: config_data = baremetal_target_config_openrisc32_init(); break;
                case ARCH_LOONGARCH64: config_data = baremetal_target_config_loongarch64_init(); break;
                case ARCH_LOONGARCH32: config_data = baremetal_target_config_loongarch32_init(); break;
                case ARCH_SPARCV9: config_data = baremetal_target_config_sparcv9_init(); break;
                case ARCH_SPARCV8: config_data = baremetal_target_config_sparcv8_init(); break;
                case ARCH_SPARCV7: config_data = baremetal_target_config_sparcv7_init(); break;
                default: fprintf(stderr, "Hata: Baremetal için desteklenmeyen mimari: %s\n", target_arch_to_string(arch)); break;
            }
            break;
        case OS_ANDROID:
            switch (arch) {
                case ARCH_AMD64: config_data = android_target_config_amd64_init(); break;
                case ARCH_AMD32: config_data = android_target_config_amd32_init(); break;
                case ARCH_ARMV9: config_data = android_target_config_armv9_init(); break;
                case ARCH_ARMV8: config_data = android_target_config_armv8_init(); break;
                case ARCH_ARMV7: config_data = android_target_config_armv7_init(); break;
                default: fprintf(stderr, "Hata: Android için desteklenmeyen mimari: %s\n", target_arch_to_string(arch)); break;
            }
            break;
        case OS_IOS:
            switch (arch) {
                case ARCH_ARMV9: config_data = ios_target_config_armv9_init(); break;
                case ARCH_ARMV8: config_data = ios_target_config_armv8_init(); break;
                case ARCH_ARMV7: config_data = ios_target_config_armv7_init(); break;
                default: fprintf(stderr, "Hata: iOS için desteklenmeyen mimari: %s\n", target_arch_to_string(arch)); break;
            }
            break;
        case OS_MACOS:
            switch (arch) {
                case ARCH_AMD64: config_data = macos_target_config_amd64_init(); break;
                case ARCH_AMD32: config_data = macos_target_config_amd32_init(); break;
                case ARCH_ARMV9: config_data = macos_target_config_armv9_init(); break;
                default: fprintf(stderr, "Hata: MacOS için desteklenmeyen mimari: %s\n", target_arch_to_string(arch)); break;
            }
            break;
        case OS_RISCOS:
            switch (arch) {
                case ARCH_ARMV7: config_data = riscos_target_config_armv7_init(); break;
                default: fprintf(stderr, "Hata: RISCOS için desteklenmeyen mimari: %s\n", target_arch_to_string(arch)); break;
            }
            break;
        case OS_HAIKU:
            switch (arch) {
                case ARCH_AMD64: config_data = haiku_target_config_amd64_init(); break;
                case ARCH_AMD32: config_data = haiku_target_config_amd32_init(); break;
                // Diğer mimariler için eklemeyi unutmayın
                default: fprintf(stderr, "Hata: Haiku için desteklenmeyen mimari: %s\n", target_arch_to_string(arch)); break;
            }
            break;
        case OS_KOLIBRIOS:
            switch (arch) {
                case ARCH_AMD64: config_data = kolibrios_target_config_amd64_init(); break;
                case ARCH_AMD32: config_data = kolibrios_target_config_amd32_init(); break;
                default: fprintf(stderr, "Hata: KolibriOS için desteklenmeyen mimari: %s\n", target_arch_to_string(arch)); break;
            }
            break;
        case OS_REACTOS:
            switch (arch) {
                case ARCH_AMD64: config_data = reactos_target_config_amd64_init(); break;
                case ARCH_AMD32: config_data = reactos_target_config_amd32_init(); break;
                case ARCH_ARMV7: config_data = reactos_target_config_armv7_init(); break;
                default: fprintf(stderr, "Hata: ReactOS için desteklenmeyen mimari: %s\n", target_arch_to_string(arch)); break;
            }
            break;
        case OS_UNIX: // UNIX tüm mimarileri destekler
            switch (arch) {
                case ARCH_AMD64: config_data = unix_target_config_amd64_init(); break;
                case ARCH_AMD32: config_data = unix_target_config_amd32_init(); break;
                case ARCH_ARMV9: config_data = unix_target_config_armv9_init(); break;
                case ARCH_ARMV8: config_data = unix_target_config_armv8_init(); break;
                case ARCH_ARMV7: config_data = unix_target_config_armv7_init(); break;
                case ARCH_RV64I: config_data = unix_target_config_rv64i_init(); break;
                case ARCH_RV64E: config_data = unix_target_config_rv64e_init(); break;
                case ARCH_RV32I: config_data = unix_target_config_rv32i_init(); break;
                case ARCH_RV32E: config_data = unix_target_config_rv32e_init(); break;
                case ARCH_POWERPC64: config_data = unix_target_config_powerpc64_init(); break;
                case ARCH_POWERPC32: config_data = unix_target_config_powerpc32_init(); break;
                case ARCH_MIPS64: config_data = unix_target_config_mips64_init(); break;
                case ARCH_MIPS32: config_data = unix_target_config_mips32_init(); break;
                case ARCH_MICRO_MIPS: config_data = unix_target_config_micro_mips_init(); break;
                case ARCH_OPENRISC64: config_data = unix_target_config_openrisc64_init(); break;
                case ARCH_OPENRISC32: config_data = unix_target_config_openrisc32_init(); break;
                case ARCH_LOONGARCH64: config_data = unix_target_config_loongarch64_init(); break;
                case ARCH_LOONGARCH32: config_data = unix_target_config_loongarch32_init(); break;
                case ARCH_SPARCV9: config_data = unix_target_config_sparcv9_init(); break;
                case ARCH_SPARCV8: config_data = unix_target_config_sparcv8_init(); break;
                case ARCH_SPARCV7: config_data = unix_target_config_sparcv7_init(); break;
                default: fprintf(stderr, "Hata: UNIX için desteklenmeyen mimari: %s\n", target_arch_to_string(arch)); break;
            }
            break;
        default:
            fprintf(stderr, "Hata: Desteklenmeyen işletim sistemi hedefi: %s\n", target_os_to_string(os));
            break;
    }

    if (!config_data) {
        fprintf(stderr, "Hata: Seçilen OS (%s) ve mimari (%s) kombinasyonu için yapılandırma yüklenemedi veya desteklenmiyor.\n",
                target_os_to_string(os), target_arch_to_string(arch));
        free(target);
        return NULL;
    }

    target->os_arch_config_data = config_data;
    fprintf(stdout, "Target: Hedef yapılandırma başarıyla yüklendi.\n");
    return target;
}

void target_close(Target* target) {
    if (target) {
        // OS/mimariye özgü yapılandırma verilerini serbest bırak
        switch (target->os) {
            case OS_LINUX:
                switch (target->arch) {
                    case ARCH_AMD64: linux_target_config_amd64_close((LinuxTargetConfigAmd64*)target->os_arch_config_data); break;
                    case ARCH_AMD32: linux_target_config_amd32_close((LinuxTargetConfigAmd32*)target->os_arch_config_data); break;
                    case ARCH_ARMV9: linux_target_config_armv9_close((LinuxTargetConfigArmv9*)target->os_arch_config_data); break;
                    case ARCH_ARMV8: linux_target_config_armv8_close((LinuxTargetConfigArmv8*)target->os_arch_config_data); break;
                    case ARCH_ARMV7: linux_target_config_armv7_close((LinuxTargetConfigArmv7*)target->os_arch_config_data); break;
                    case ARCH_RV64I: linux_target_config_rv64i_close((LinuxTargetConfigRv64I*)target->os_arch_config_data); break;
                    case ARCH_RV64E: linux_target_config_rv64e_close((LinuxTargetConfigRv64E*)target->os_arch_config_data); break;
                    case ARCH_RV32I: linux_target_config_rv32i_close((LinuxTargetConfigRv32I*)target->os_arch_config_data); break;
                    case ARCH_RV32E: linux_target_config_rv32e_close((LinuxTargetConfigRv32E*)target->os_arch_config_data); break;
                    case ARCH_POWERPC64: linux_target_config_powerpc64_close((LinuxTargetConfigPowerPC64*)target->os_arch_config_data); break;
                    case ARCH_POWERPC32: linux_target_config_powerpc32_close((LinuxTargetConfigPowerPC32*)target->os_arch_config_data); break;
                    case ARCH_MIPS64: linux_target_config_mips64_close((LinuxTargetConfigMips64*)target->os_arch_config_data); break;
                    case ARCH_MIPS32: linux_target_config_mips32_close((LinuxTargetConfigMips32*)target->os_arch_config_data); break;
                    case ARCH_MICRO_MIPS: linux_target_config_micro_mips_close((LinuxTargetConfigMicroMips*)target->os_arch_config_data); break;
                    case ARCH_OPENRISC64: linux_target_config_openrisc64_close((LinuxTargetConfigOpenRISC64*)target->os_arch_config_data); break;
                    case ARCH_OPENRISC32: linux_target_config_openrisc32_close((LinuxTargetConfigOpenRISC32*)target->os_arch_config_data); break;
                    case ARCH_LOONGARCH64: linux_target_config_loongarch64_close((LinuxTargetConfigLoongArch64*)target->os_arch_config_data); break;
                    case ARCH_LOONGARCH32: linux_target_config_loongarch32_close((LinuxTargetConfigLoongArch32*)target->os_arch_config_data); break;
                    case ARCH_SPARCV9: linux_target_config_sparcv9_close((LinuxTargetConfigSparcV9*)target->os_arch_config_data); break;
                    case ARCH_SPARCV8: linux_target_config_sparcv8_close((LinuxTargetConfigSparcV8*)target->os_arch_config_data); break;
                    case ARCH_SPARCV7: linux_target_config_sparcv7_close((LinuxTargetConfigSparcV7*)target->os_arch_config_data); break;
                    default: break; // Desteklenmeyen mimari
                }
                break;
            case OS_WINDOWS:
                switch (target->arch) {
                    case ARCH_AMD64: windows_target_config_amd64_close((WindowsTargetConfigAmd64*)target->os_arch_config_data); break;
                    case ARCH_AMD32: windows_target_config_amd32_close((WindowsTargetConfigAmd32*)target->os_arch_config_data); break;
                    case ARCH_ARMV9: windows_target_config_armv9_close((WindowsTargetConfigArmv9*)target->os_arch_config_data); break;
                    case ARCH_ARMV8: windows_target_config_armv8_close((WindowsTargetConfigArmv8*)target->os_arch_config_data); break;
                    default: break;
                }
                break;
            case OS_BAREMETAL:
                switch (target->arch) {
                    case ARCH_AMD64: baremetal_target_config_amd64_close((BaremetalTargetConfigAmd64*)target->os_arch_config_data); break;
                    case ARCH_AMD32: baremetal_target_config_amd32_close((BaremetalTargetConfigAmd32*)target->os_arch_config_data); break;
                    case ARCH_ARMV9: baremetal_target_config_armv9_close((BaremetalTargetConfigArmv9*)target->os_arch_config_data); break;
                    case ARCH_ARMV8: baremetal_target_config_armv8_close((BaremetalTargetConfigArmv8*)target->os_arch_config_data); break;
                    case ARCH_ARMV7: baremetal_target_config_armv7_close((BaremetalTargetConfigArmv7*)target->os_arch_config_data); break;
                    case ARCH_RV64I: baremetal_target_config_rv64i_close((BaremetalTargetConfigRv64I*)target->os_arch_config_data); break;
                    case ARCH_RV64E: baremetal_target_config_rv64e_close((BaremetalTargetConfigRv64E*)target->os_arch_config_data); break;
                    case ARCH_RV32I: baremetal_target_config_rv32i_close((BaremetalTargetConfigRv32I*)target->os_arch_config_data); break;
                    case ARCH_RV32E: baremetal_target_config_rv32e_close((BaremetalTargetConfigRv32E*)target->os_arch_config_data); break;
                    case ARCH_POWERPC64: baremetal_target_config_powerpc64_close((BaremetalTargetConfigPowerPC64*)target->os_arch_config_data); break;
                    case ARCH_POWERPC32: baremetal_target_config_powerpc32_close((BaremetalTargetConfigPowerPC32*)target->os_arch_config_data); break;
                    case ARCH_MIPS64: baremetal_target_config_mips64_close((BaremetalTargetConfigMips64*)target->os_arch_config_data); break;
                    case ARCH_MIPS32: baremetal_target_config_mips32_close((BaremetalTargetConfigMips32*)target->os_arch_config_data); break;
                    case ARCH_MICRO_MIPS: baremetal_target_config_micro_mips_close((BaremetalTargetConfigMicroMips*)target->os_arch_config_data); break;
                    case ARCH_OPENRISC64: baremetal_target_config_openrisc64_close((BaremetalTargetConfigOpenRISC64*)target->os_arch_config_data); break;
                    case ARCH_OPENRISC32: baremetal_target_config_openrisc32_close((BaremetalTargetConfigOpenRISC32*)target->os_arch_config_data); break;
                    case ARCH_LOONGARCH64: baremetal_target_config_loongarch64_close((BaremetalTargetConfigLoongArch64*)target->os_arch_config_data); break;
                    case ARCH_LOONGARCH32: baremetal_target_config_loongarch32_close((BaremetalTargetConfigLoongArch32*)target->os_arch_config_data); break;
                    case ARCH_SPARCV9: baremetal_target_config_sparcv9_close((BaremetalTargetConfigSparcV9*)target->os_arch_config_data); break;
                    case ARCH_SPARCV8: baremetal_target_config_sparcv8_close((BaremetalTargetConfigSparcV8*)target->os_arch_config_data); break;
                    case ARCH_SPARCV7: baremetal_target_config_sparcv7_close((BaremetalTargetConfigSparcV7*)target->os_arch_config_data); break;
                    default: break;
                }
                break;
            case OS_ANDROID:
                switch (target->arch) {
                    case ARCH_AMD64: android_target_config_amd64_close((AndroidTargetConfigAmd64*)target->os_arch_config_data); break;
                    case ARCH_AMD32: android_target_config_amd32_close((AndroidTargetConfigAmd32*)target->os_arch_config_data); break;
                    case ARCH_ARMV9: android_target_config_armv9_close((AndroidTargetConfigArmv9*)target->os_arch_config_data); break;
                    case ARCH_ARMV8: android_target_config_armv8_close((AndroidTargetConfigArmv8*)target->os_arch_config_data); break;
                    case ARCH_ARMV7: android_target_config_armv7_close((AndroidTargetConfigArmv7*)target->os_arch_config_data); break;
                    default: break;
                }
                break;
            case OS_IOS:
                switch (target->arch) {
                    case ARCH_ARMV9: ios_target_config_armv9_close((IOSTargetConfigArmv9*)target->os_arch_config_data); break;
                    case ARCH_ARMV8: ios_target_config_armv8_close((IOSTargetConfigArmv8*)target->os_arch_config_data); break;
                    case ARCH_ARMV7: ios_target_config_armv7_close((IOSTargetConfigArmv7*)target->os_arch_config_data); break;
                    default: break;
                }
                break;
            case OS_MACOS:
                switch (target->arch) {
                    case ARCH_AMD64: macos_target_config_amd64_close((MacOSTargetConfigAmd64*)target->os_arch_config_data); break;
                    case ARCH_AMD32: macos_target_config_amd32_close((MacOSTargetConfigAmd32*)target->os_arch_config_data); break;
                    case ARCH_ARMV9: macos_target_config_armv9_close((MacOSTargetConfigArmv9*)target->os_arch_config_data); break;
                    default: break;
                }
                break;
            case OS_RISCOS:
                switch (target->arch) {
                    case ARCH_ARMV7: riscos_target_config_armv7_close((RISCOSTargetConfigArmv7*)target->os_arch_config_data); break;
                    default: break;
                }
                break;
            case OS_HAIKU:
                switch (target->arch) {
                    case ARCH_AMD64: haiku_target_config_amd64_close((HaikuTargetConfigAmd64*)target->os_arch_config_data); break;
                    case ARCH_AMD32: haiku_target_config_amd32_close((HaikuTargetConfigAmd32*)target->os_arch_config_data); break;
                    default: break;
                }
                break;
            case OS_KOLIBRIOS:
                switch (target->arch) {
                    case ARCH_AMD64: kolibrios_target_config_amd64_close((KolibriOSTargetConfigAmd64*)target->os_arch_config_data); break;
                    case ARCH_AMD32: kolibrios_target_config_amd32_close((KolibriOSTargetConfigAmd32*)target->os_arch_config_data); break;
                    default: break;
                }
                break;
            case OS_REACTOS:
                switch (target->arch) {
                    case ARCH_AMD64: reactos_target_config_amd64_close((ReactOSTargetConfigAmd64*)target->os_arch_config_data); break;
                    case ARCH_AMD32: reactos_target_config_amd32_close((ReactOSTargetConfigAmd32*)target->os_arch_config_data); break;
                    case ARCH_ARMV7: reactos_target_config_armv7_close((ReactOSTargetConfigArmv7*)target->os_arch_config_data); break;
                    default: break;
                }
                break;
            case OS_UNIX:
                switch (target->arch) {
                    case ARCH_AMD64: unix_target_config_amd64_close((UnixTargetConfigAmd64*)target->os_arch_config_data); break;
                    case ARCH_AMD32: unix_target_config_amd32_close((UnixTargetConfigAmd32*)target->os_arch_config_data); break;
                    case ARCH_ARMV9: unix_target_config_armv9_close((UnixTargetConfigArmv9*)target->os_arch_config_data); break;
                    case ARCH_ARMV8: unix_target_config_armv8_close((UnixTargetConfigArmv8*)target->os_arch_config_data); break;
                    case ARCH_ARMV7: unix_target_config_armv7_close((UnixTargetConfigArmv7*)target->os_arch_config_data); break;
                    case ARCH_RV64I: unix_target_config_rv64i_close((UnixTargetConfigRv64I*)target->os_arch_config_data); break;
                    case ARCH_RV64E: unix_target_config_rv64e_close((UnixTargetConfigRv64E*)target->os_arch_config_data); break;
                    case ARCH_RV32I: unix_target_config_rv32i_close((UnixTargetConfigRv32I*)target->os_arch_config_data); break;
                    case ARCH_RV32E: unix_target_config_rv32e_close((UnixTargetConfigRv32E*)target->os_arch_config_data); break;
                    case ARCH_POWERPC64: unix_target_config_powerpc64_close((UnixTargetConfigPowerPC64*)target->os_arch_config_data); break;
                    case ARCH_POWERPC32: unix_target_config_powerpc32_close((UnixTargetConfigPowerPC32*)target->os_arch_config_data); break;
                    case ARCH_MIPS64: unix_target_config_mips64_close((UnixTargetConfigMips64*)target->os_arch_config_data); break;
                    case ARCH_MIPS32: unix_target_config_mips32_close((UnixTargetConfigMips32*)target->os_arch_config_data); break;
                    case ARCH_MICRO_MIPS: unix_target_config_micro_mips_close((UnixTargetConfigMicroMips*)target->os_arch_config_data); break;
                    case ARCH_OPENRISC64: unix_target_config_openrisc64_close((UnixTargetConfigOpenRISC64*)target->os_arch_config_data); break;
                    case ARCH_OPENRISC32: unix_target_config_openrisc32_close((UnixTargetConfigOpenRISC32*)target->os_arch_config_data); break;
                    case ARCH_LOONGARCH64: unix_target_config_loongarch64_close((UnixTargetConfigLoongArch64*)target->os_arch_config_data); break;
                    case ARCH_LOONGARCH32: unix_target_config_loongarch32_close((UnixTargetConfigLoongArch32*)target->os_arch_config_data); break;
                    case ARCH_SPARCV9: unix_target_config_sparcv9_close((UnixTargetConfigSparcV9*)target->os_arch_config_data); break;
                    case ARCH_SPARCV8: unix_target_config_sparcv8_close((UnixTargetConfigSparcV8*)target->os_arch_config_data); break;
                    case ARCH_SPARCV7: unix_target_config_sparcv7_close((UnixTargetConfigSparcV7*)target->os_arch_config_data); break;
                    default: break;
                }
                break;
            default:
                break; // Bilinmeyen OS durumu
        }
        free(target);
    }
}

// target_arch_to_string ve target_os_to_string fonksiyonları
// (Bunlar daha önceki versiyonlardan aynı kalabilir, sadece UNKNOWN_ARCH yerine
// switch'e yeni eklenen mimariler ve OS'ler dahil edilmeliydi.
// Sizin güncellediğiniz target.h'den kopyaladığım için onlar da doğru.)

const char* target_arch_to_string(TargetArchitecture arch) {
    switch (arch) {
        case ARCH_AMD64: return "AMD64";
        case ARCH_AMD32: return "AMD32";
        case ARCH_ARMV9: return "ARMV9";
        case ARCH_ARMV8: return "ARMV8";
        case ARCH_ARMV7: return "ARMV7";
        case ARCH_RV64I: return "RV64I";
        case ARCH_RV64E: return "RV64E";
        case ARCH_RV32I: return "RV32I";
        case ARCH_RV32E: return "RV32E";
        case ARCH_POWERPC64: return "POWERPC64";
        case ARCH_POWERPC32: return "POWERPC32";
        case ARCH_MIPS64: return "MIPS64";
        case ARCH_MIPS32: return "MIPS32";
        case ARCH_MICRO_MIPS: return "MICRO_MIPS";
        case ARCH_OPENRISC64: return "OPENRISC64";
        case ARCH_OPENRISC32: return "OPENRISC32";
        case ARCH_LOONGARCH64: return "LOONGARCH64";
        case ARCH_LOONGARCH32: return "LOONGARCH32";
        case ARCH_SPARCV9: return "SPARCV9";
        case ARCH_SPARCV8: return "SPARCV8";
        case ARCH_SPARCV7: return "SPARCV7";
        case UNKNOWN_ARCH: return "UNKNOWN_ARCH";
        default: return "UNKNOWN_ARCH_ERROR"; // Hata durumu
    }
}

const char* target_os_to_string(TargetOperatingSystem os) {
    switch (os) {
        case OS_LINUX: return "LINUX";
        case OS_WINDOWS: return "WINDOWS";
        case OS_BAREMETAL: return "BAREMETAL";
        case OS_ANDROID: return "ANDROID";
        case OS_IOS: return "IOS";
        case OS_MACOS: return "MACOS";
        case OS_RISCOS: return "RISCOS";
        case OS_HAIKU: return "HAIKU";
        case OS_KOLIBRIOS: return "KOLIBRIOS";
        case OS_REACTOS: return "REACTOS";
        case OS_UNIX: return "UNIX";
        case UNKNOWN_OS: return "UNKNOWN_OS";
        default: return "UNKNOWN_OS_ERROR"; // Hata durumu
    }
}
