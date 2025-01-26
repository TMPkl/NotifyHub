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

