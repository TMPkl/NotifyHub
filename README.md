# NotifyHub

## Opis projektu
NotifyHub to system przekazywania powiadomień, który umożliwia komunikację między producentami powiadomień, centralnym dyspozytorem oraz klientami subskrybującymi określone typy powiadomień. Głównym celem systemu jest efektywne rozgłaszanie wiadomości w oparciu o mechanizm kolejek komunikatów.

## Zawartość projektu
Projekt składa się z trzech głównych komponentów:
1. **Producent (Producer inf52024_160269_p.c)**  
   - Generuje powiadomienia, takie jak alerty lub aktualizacje.  
   - Wysyła powiadomienia do dyspozytora.

2. **Dyspozytor (Dispatcher inf52024_160269_d.c)**  
   - Zarządza subskrypcjami klientów.  
   - Przekazuje powiadomienia od producentów do odpowiednich klientów.  

3. **Klient (Client: inf52024_160269_k.c)**  
   - Subskrybuje określone typy powiadomień.  
   - Otrzymuje i wyświetla powiadomienia przesyłane przez dyspozytora.  

## Kluczowe funkcjonalności
- Mechanizm kolejek komunikatów do przesyłania wiadomości.  
- Protokół komunikacyjny zapewniający wydajność i niezawodność.  
- Możliwość dynamicznego zarządzania subskrypcjami klientów.  


![Application Architecture](./pa.svg)
# KOMUNIKACJA PRODUCENT - DYSTRYBUTOR

Całość odbywa się na kolejce o kluczu: `INITIAL_COMUNICATION_KEY`.

## Struktura `init_msg`

### Inicjalizacja

Inicjalizacja odbywa się poprzez wysłanie przez producenta zapytania o dostępność kanału, w którym dystrybutor chce nadawać, oraz proponowanego swojego `id_producenta`. W odpowiedzi producent otrzymuje informację zwrotną, w której w zależności od statusu:

- **status -1** - kanał jest już nadawany przez innego producenta.
- **status 0** - kanał i `id_producenta` są wolne, zostały pomyślnie przypisane do producenta zgodnie z zapytaniem.
- **status n** (dla `n > 0`) - proponowane przez producenta ID jest zajęte, przyznano pierwsze wolne ID → `n` jest nowym `id_producenta`.

### Wybór kanału

W strukturze `init_msg`, tablica `int info_type[5]` przechowuje informacje o kanałach, które producent chce nadawać. Wielkość ograniczona jest do 5 (zgodnie z założeniami projektu). Wartości kanałów zapisywane są zgodnie z podziałem:

```c
static char* types_of_info[] w pliku inf_16029_h.h
```

Kanały są zapisywane kolejno w tablicy od indeksu `0`.

---

# DODAWANIE NOWEGO KANAŁU DO NADAWANIA PRODUCENT - KLIENT

Całość odbywa się na kolejce o kluczu: `INITIAL_COMUNICATION_KEY`.

Rozpoczęcie dodawania kanału odbywa się poprzez wysłanie zapytania `producent → dystrybutor` na kanale `UPDATING_CHANEL`.\
Jeżeli kanał jest wolny, dystrybutor zapisuje skojarzenie do tablicy powiązań **kanał → id\_nadawcy**.

Dystrybutor zwraca informację zwrotną o statusie wykonania operacji kanałem `UPDATKING_CHANEL_FEEDBACK`.

---

# NADAWANIE WIADOMOŚCI PRODUCENT - KLIENT

Całość odbywa się na kolejce o kluczu: `INITIAL_COMUNICATION_KEY`.

Wiadomość stworzona przez producenta jest przekazywana za pomocą struktury `news`:

```c
struct news {
    long type;   
    int id_poroducer;
    char news_content[100];
};
```

### Wyjaśnienie pól struktury

- `long type`\
  Określa kanał wiadomości i jest dobierana zgodnie z tablicą powiązań **kanał → treść**:

  ```c
  static char* types_of_info[]
  ```

  Trzeba uwzględnić offset `+1` (np. `types_of_info[0]` nadawane jest na kanale `1`, `types_of_info[1]` na kanale `2`, itd.).\
  Zdefiniowane jest 10 kanałów zgodnie z założeniami projektu.

- `int id_poroducer`\
  Zmienna zawierająca ID producenta. Obecnie niewykorzystywana przy przesyłaniu, ale może służyć do dalszej rozbudowy, np. obliczania, który producent najwięcej nadaje.

- `char news_content[100]`\
  Domyślny komunikat.

### Przesyłanie struktury

Tak zdefiniowana struktura jest przesyłana dystrybutorowi.

---

# KOMUNIKACJA KLIENT - DYSTRYBUTOR

Całość odbywa się na kolejce o kluczu: `INITIAL_COMUNICATION_KEY`.

## Inicjalizacja klienta

### Proces inicjalizacji

1. Klient wysyła zapytanie do dystrybutora, aby uzyskać swoje `id_client`.  
   Zapytanie jest wysyłane w postaci struktury `init_client` na kanał `INITIAL_CLIENT_DISTRIBUTOR_CHANEL`:
   ```c
   struct init_client {
       long type;
       int id_client;
   };
   ```
2. Dystrybutor odpowiada za pomocą struktury `producent_distributor_feedback`:
   ```c
   struct producent_distributor_feedback {
       long type;
       int status;
   };
   ```
   - **status -1**: Podane ID jest nieprawidłowe – inicjalizacja nie powiodła się.
   - **status 0**: Klient został pomyślnie połączony z wybranym ID.

3. W przypadku udanej inicjalizacji tworzona jest kolejka wiadomości dla klienta na kluczu wyliczanym jako `id_client * 1000`.

---

## Subskrypcja kanałów informacyjnych

### Lista dostępnych kanałów

Po nawiązaniu połączenia klient otrzymuje od dystrybutora listę dostępnych kanałów w strukturze `news_request`:
```c
struct news_request {
    long type;
    int id_client;
    int chanel[10];
};
```
- Tablica `chanel[10]` przechowuje informacje o dostępnych kanałach, gdzie `0` oznacza brak informacji, a `1` oznacza, że kanał jest aktywny.

### Wybór kanału

1. Klient wyświetla listę dostępnych kanałów, korzystając z `types_of_info[]` do przetłumaczenia numerów kanałów na ich opisy.
2. Po wybraniu kanału klient aktualizuje swoją subskrypcję, wysyłając strukturę `news_request` do dystrybutora.\
   Wybrany kanał jest ustawiony na `1` w odpowiednim indeksie tablicy `chanel[]`.

---

## Odbieranie wiadomości

### Struktura wiadomości

Dystrybutor przesyła wiadomości do klienta w strukturze `news`:
```c
struct news {
    long type;
    char news_content[100];
};
```
- `long type`: Numer kanału (offsetowany o `+1` zgodnie z tablicą `types_of_info[]`).
- `char news_content[100]`: Treść wiadomości.

### Obsługa wiadomości

1. Klient odbiera wiadomości z kolejki za pomocą `msgrcv()`.
2. Wiadomości są filtrowane na podstawie subskrybowanych kanałów.
3. Każda wiadomość jest wyświetlana z informacją o jej kanale i treści.

---

## Dodanie nowej subskrypcji

1. Klient wysyła zapytanie o aktualną listę kanałów, wysyłając pustą strukturę `ping`:
   ```c
   struct ping {
       long type;
       int id_client;
   };
   ```
2. Dystrybutor odpowiada strukturą `news_request` z listą dostępnych kanałów.
3. Klient wybiera kanał i aktualizuje swoją subskrypcję, wysyłając zaktualizowaną strukturę `news_request`.

---
