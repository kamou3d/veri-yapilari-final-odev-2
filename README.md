# Linux Syslog Yönetimi ve Çift Yönlü Bağlı Liste İmplementasyonu

Bu proje, Linux işletim sistemindeki sistem günlüklerinin (syslog) nasıl çalıştığını anlamak ve bu günlükleri bellek üzerinde verimli bir şekilde yönetebilmek amacıyla **C dili** kullanılarak geliştirilmiştir. Projede veri yapısı olarak **Çift Yönlü Bağlı Liste (Doubly Linked List)** tercih edilmiştir.

## 📝 Syslog Mekanizması Nedir ve Ne İçin Kullanılır?

Linux sistemlerinde çekirdek (kernel), servisler ve uygulamalar sürekli olarak çalışma durumları, hata mesajları ve uyarılar üretir. **Syslog**, tüm bu dağınık ve farklı kaynaklardan gelen olay kayıtlarının (logların) standart bir formatta, merkezi bir konumda (genellikle `/var/log/` dizini altında) toplanmasını sağlayan bir sistem ve protokoldür. 
Sistem yöneticileri bu logları;
* Sistemdeki anormallikleri tespit etmek,
* Güvenlik ihlallerini izlemek,
* Çöken uygulamaların (crash) nedenlerini geriye dönük (debug) analiz etmek için kullanır.

## ⚙️ Neden Çift Yönlü Bağlı Liste (Doubly Linked List) Kullanıldı?

Syslog verilerini bellek (RAM) üzerinde tutmak için Çift Yönlü Bağlı Liste tercih edilmiştir. Bu tercihin Syslog mekanizması ile doğrudan teknik bir ilişkisi bulunmaktadır:

1. **O(1) Karmaşıklığında Veri Ekleme:** Log mekanizmaları sürekli olarak yeni veri üretir. Yeni gelen her bir log satırının dosyanın/listenin sonuna eklenmesi (append) gerekir. Çift yönlü bağlı listemizde tuttuğumuz `tail` (kuyruk) işaretçisi sayesinde, listeyi baştan sona taramaya gerek kalmadan en son log satırını $O(1)$ zaman karmaşıklığı ile listeye ekleyebiliyoruz.
2. **Sondan Başa Doğru Hızlı Okuma (Tail Simülasyonu):** Bir sistemde sorun çıktığında, genellikle en eski loglara değil, **en son üretilen (en güncel) loglara** bakılır. Linux terminalindeki `tail` komutunun yaptığı bu işlemi simüle edebilmek için listeyi sondan başa doğru okumak gerekir. Çift yönlü bağlı listelerdeki `prev` (önceki) işaretçileri sayesinde `tail` düğümünden başlayarak geriye doğru çok hızlı bir şekilde gezinmek ve güncel logları ekrana basmak mümkündür. Tek yönlü bir liste (Singly Linked List) kullanılsaydı bu işlem çok daha maliyetli olacaktı.

## 🚀 Kurulum ve Çalıştırma Demosu

Projenin tüm ortamlarda (farklı Linux dağıtımları veya Windows/macOS) sorunsuz test edilebilmesi için sistemin gerçek syslog dizini yerine, örnek bir `sample_syslog.txt` dosyası üzerinden okuma yapılmaktadır.

**Derleme İşlemi:**
Terminali açın ve proje dizininde aşağıdaki komutu çalıştırın:
```bash
gcc main.c -o syslog_reader
