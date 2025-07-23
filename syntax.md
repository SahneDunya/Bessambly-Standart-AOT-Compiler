# Bessambly Sözdizimi
Bessambly, donanımdan bağımsız, en düşük seviyeli bir programlama dilidir. Hiçbir programlama dilini temel almazken, düşük seviyeli yapısı sayesinde sözdizimi Assembly'ye benzer bir his verir. Amacımız, makine kodunun soyutlanmış bir temsilini sunarak geliştiricilere donanım üzerinde benzer bir kontrol sağlamaktır, ancak farklı mimarilerde taşınabilirlik sunar.

Bessambly kaynak kod dosyaları .bsm uzantısına sahiptir.

## Temel Felsefe ve Yapı
Bessambly, imperatif (zorunlu) programlamayı destekler. Bu, programın durumunu doğrudan değiştiren komutlar dizisiyle çalışacağınız anlamına gelir. Dil, modül sisteminden ve standart bir kütüphaneden yoksundur, bu da onun son derece minimalist ve hafif olmasını sağlar.

## Kontrol Akışı
Bessambly'de kontrol akışı, geleneksel yüksek seviyeli dillerdeki if, for, while gibi yapıların aksine, doğrudan jump (atlama) komutları ve flag (bayrak) kullanımıyla sağlanır. Bu yaklaşım, Assembly'ye daha yakın bir deneyim sunar ve programın yürütülmesi üzerinde mutlak kontrol sağlar.

### jump (Atlama): Programın belirli bir etiketlenmiş noktasına koşulsuz veya koşullu olarak atlamayı sağlar.
### flag (Bayrak): İşlemlerin sonuçlarını (örneğin, karşılaştırma sonuçları, aritmetik taşmalar) belirtmek için kullanılan ikili durum değişkenleridir. jump komutları genellikle bu bayrakların durumuna göre karar verir.
Örnek (Kavramsal):
```
    CMP R1, R2       ; R1 ile R2'yi karşılaştır, bayrakları ayarla
    JEQ  EQUAL_LABEL ; Eğer eşitse EQUAL_LABEL'a atla

    ; Eşit değilse buradaki kod çalışır

EQUAL_LABEL:
    ; Eşitse buradaki kod çalışır
``` 
## Sistem Çağrıları ve Etkileşim
Bessambly, I/O işlemleri, aygıt erişimi, ağ iletişimi, IPC (İşlemler Arası İletişim), dinamik bellek yönetimi ve sistem hizmetlerine erişim gibi dış dünya ile etkileşim gerektiren durumlar için sistem çağrılarını kullanır.

Bu sistem çağrıları, bazı durumlarda altında yatan donanıma veya işletim sistemine bağımlı kod gerektirebilir. Ancak Bessambly'nin donanım bağımsızlığı hedefini korumak için alternatif yaklaşımlar sunulmuştur:

### Boot Çağrıları: Doğrudan sistem başlangıcı seviyesinde belirli işlevlere erişim sağlar.
### Sanal Makine Çağrıları (Hypercalls, VM Exits vb.): Bessambly, kendi Sanal Makinesi (BVM) çağrılarını kullanabildiği gibi, WebAssembly (WASM), JVM, .NET gibi diğer popüler sanal makinelerin çağrılarını da kullanabilir. Bu, Bessambly kodunun donanımdan ziyade belirli bir sanal makineye bağımlı olmasını sağlar. Sanal makine çağrıları, Bessambly'nin donanımdan bağımsızlığını sağlamanın tercih edilen yoludur, ancak bu durumda kod çalıştığı sanal makineye bağımlı hale gelir.
Sözdizimsel olarak, sistem çağrıları genellikle özel bir talimat veya çağrı mekanizması ile tetiklenir ve gerekli parametreleri belirtilen "kaydedicilere" (veya benzeri geçici depolama alanlarına) yükleyerek çalışır.

Örnek (Kavramsal Sistem Çağrısı):
```
    MOV R0, FILE_HANDLE_ID ; Dosya tanıtıcısını R0'a yükle
    MOV R1, BUFFER_ADDR    ; Veri tamponu adresini R1'e yükle
    MOV R2, LENGTH         ; Okunacak uzunluğu R2'ye yükle
    SYSCALL READ_FILE      ; Dosya okuma sistem çağrısını yap
```
## Bessambly ve Assembly Arasındaki İlişki
### Assembly, makine kodunun doğrudan bir temsilidir ve belirli bir CPU mimarisine sıkıca bağlıdır. Her CPU mimarisi için farklı bir Assembly dili vardır.

### Bessambly ise, Assembly dilinin donanımdan soyutlanmış bir hali olarak tasarlanmıştır. Bessambly, donanım spesifik kaydediciler veya adresleme modları gibi kavramları, genel bir sanal mimari üzerinden soyutlayarak programcılara sunar. Bu sayede yazılan Bessambly kodu, farklı donanım platformlarında Bessambly Sanal Makinesi (BVM) aracılığıyla veya doğrudan derlenerek çalışabilir.

## Geliştirme Araçları
Bessambly, geliştirme sürecini desteklemek için şu araçlarla birlikte gelir:

### Bessambly AOT (Ahead-of-Time) Compiler: Bessambly kodunu, belirli bir hedef platform için doğrudan makine koduna çeviren bir derleyicidir. Bu derlenen kodlar, Bessambly Sanal Makinesi (BVM) gerekmeksizin doğrudan gerçek donanım üzerinde çalışır. Performans kritik uygulamalar ve sistem seviyesi programlama için idealdir.
### Bessambly JIT (Just-In-Time) Compiler: Bessambly kodlarını doğrudan makine koduna çevirmek yerine ara koda derler. JIT derleyici tarafından desteklenen iki ana ara kod formatı vardır:
#### WebAssembly (.wasm): Web platformu ve diğer sanal makinelerde taşınabilirlik için endüstri standardı bir ara formattır.
#### VirtualBessambly (.vbsm): Bessambly'nin kendi sanal makinesi olan BVM için özel olarak tasarlanmış optimize edilmiş bir ara formattır. JIT derleyici, dinamik olarak uyarlanabilirlik ve hızlı geliştirme döngüleri için faydalıdır.
#### Bessambly Virtual Machine (BVM): Bessambly kodunu (özellikle .vbsm ara kodunu) donanımdan bağımsız bir şekilde çalıştıran sanal yürütme ortamıdır. Taşınabilirlik, güvenlik katmanı ve dinamik optimizasyon yetenekleri sağlar.

Bessambly'nin sözdizimi, düşük seviyeli programlamanın gücünü ve kontrolünü sunarken, modern yazılımın temel gereksinimlerinden biri olan donanım bağımsızlığını sağlamayı hedefler.