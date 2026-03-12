#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LOG_LENGTH 512

/* * Çift Yönlü Bağlı Liste Düğümü (Doubly Linked List Node)
 * Her bir düğüm, tek bir syslog satırını temsil eder.
 */
typedef struct SyslogEntry {
    char message[MAX_LOG_LENGTH]; // Log mesajını tutan dizi
    struct SyslogEntry* prev;     // Önceki log kaydını gösteren işaretçi (Sondan başa okuma için)
    struct SyslogEntry* next;     // Sonraki log kaydını gösteren işaretçi
} SyslogEntry;

/*
 * Log Listesi Kontrol Yapısı
 * Hem başı (head) hem de sonu (tail) tutarak, listeye yeni eleman ekleme
 * işlemini O(1) karmaşıklığına indirgiyoruz. Syslog sürekli veri ürettiği için bu çok önemlidir.
 */
typedef struct {
    SyslogEntry* head;
    SyslogEntry* tail;
} SyslogList;

// Listeyi ilk kullanıma hazırlayan fonksiyon
void initializeList(SyslogList* list) {
    list->head = NULL;
    list->tail = NULL;
}

// Yeni gelen bir log satırını listenin SONUNA ekleyen fonksiyon (Append işlemi)
void appendLog(SyslogList* list, const char* logMessage) {
    // Yeni log kaydı için bellekten yer tahsis et (Dynamic memory allocation)
    SyslogEntry* newEntry = (SyslogEntry*)malloc(sizeof(SyslogEntry));
    if (newEntry == NULL) {
        printf("Hata: Bellek tahsisi basarisiz oldu!\n");
        return;
    }

    // Log mesajini yeni dugume kopyala ve isaretcileri ayarla
    strncpy(newEntry->message, logMessage, MAX_LOG_LENGTH - 1);
    newEntry->message[MAX_LOG_LENGTH - 1] = '\0'; // Guvenlik icin null-terminator
    newEntry->next = NULL;

    // Eger liste bossa, yeni eleman hem head hem tail olur
    if (list->head == NULL) {
        newEntry->prev = NULL;
        list->head = newEntry;
        list->tail = newEntry;
    } else {
        // Liste bos degilse, mevcut tail'in sonuna ekle ve tail'i guncelle
        newEntry->prev = list->tail;
        list->tail->next = newEntry;
        list->tail = newEntry; // Yeni tail artik son eklenen dugum
    }
}

// Log dosyasini okuyup bagli listeye aktaran fonksiyon
void loadLogsFromFile(SyslogList* list, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Hata: '%s' dosyasi acilamadi. Lutfen dosyanin var oldugundan emin olun.\n", filename);
        return;
    }

    char buffer[MAX_LOG_LENGTH];
    // Dosyadan satir satir oku ve listeye ekle
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        // Satir sonundaki \n karakterini temizle
        buffer[strcspn(buffer, "\n")] = 0; 
        if(strlen(buffer) > 0) {
            appendLog(list, buffer);
        }
    }
    fclose(file);
    printf("Loglar basariyla bellege (bagli listeye) yuklendi.\n");
}

// Loglari kronolojik sirayla (bastan sona) yazdiran fonksiyon
void printLogsForward(SyslogList* list) {
    SyslogEntry* current = list->head;
    printf("\n--- Tum Loglar (Eskiden Yeniye) ---\n");
    if (current == NULL) {
        printf("Liste su an bos.\n");
        return;
    }
    while (current != NULL) {
        printf("%s\n", current->message);
        current = current->next;
    }
    printf("-----------------------------------\n");
}

// Loglari ters sirayla (sondan basa) yazdiran fonksiyon (Linux 'tail' komutu mantigi)
void printLogsBackward(SyslogList* list) {
    SyslogEntry* current = list->tail;
    printf("\n--- Son Loglar (Yeniden Eskiye - Tail Mantigi) ---\n");
    if (current == NULL) {
        printf("Liste su an bos.\n");
        return;
    }
    // prev isaretcisini kullanarak geriye dogru git
    while (current != NULL) {
        printf("%s\n", current->message);
        current = current->prev;
    }
    printf("--------------------------------------------------\n");
}

// Program kapanirken bellekte (Heap) ayrilan alanlari temizleyen fonksiyon
void freeListMemory(SyslogList* list) {
    SyslogEntry* current = list->head;
    SyslogEntry* nextEntry;
    while (current != NULL) {
        nextEntry = current->next;
        free(current);
        current = nextEntry;
    }
    list->head = NULL;
    list->tail = NULL;
    printf("Bellek temizlendi.\n");
}

int main() {
    SyslogList mySyslogList;
    initializeList(&mySyslogList);

    // Sistemin gercek syslog dosyasi yerine, platform bagimsiz calismasi icin ornek dosya
    const char* logFile = "sample_syslog.txt"; 
    
    int choice;
    do {
        printf("\n=== LINUX SYSLOG YONETICISI ===\n");
        printf("1. Log Dosyasini Oku ve Listeye Yukle\n");
        printf("2. Loglari Bastan Sona Listele (Kronolojik)\n");
        printf("3. Loglari Sondan Basa Listele (Tail - Guncel Durum)\n");
        printf("4. Cikis\n");
        printf("Seciminiz: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                loadLogsFromFile(&mySyslogList, logFile);
                break;
            case 2:
                printLogsForward(&mySyslogList);
                break;
            case 3:
                printLogsBackward(&mySyslogList);
                break;
            case 4:
                freeListMemory(&mySyslogList);
                printf("Programdan cikiliyor...\n");
                break;
            default:
                printf("Gecersiz secim. Lutfen tekrar deneyin.\n");
        }
    } while (choice != 4);

    return 0;
}
