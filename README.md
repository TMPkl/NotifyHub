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


# PROTOCOL

## KOMUNIKACJA PRODUCENT - DYSTRYBUTOR

Całość odbywa się na kolejce o kluczu: `INITIAL_COMUNICATION_KEY`.

### Struktura `init_msg`

### Inicjalizacja
Inicjalizacja odbywa się poprzez wysłanie przez producenta zapytania o dostępność kanału, w którym dystrybutor chce nadawać, oraz proponowanego swojego `id_producenta`. W odpowiedzi producent otrzymuje informację zwrotną, w której w zależności od statusu:

- **status -1** - kanał jest już nadawany przez innego producenta.
- **status 0** - kanał i `id_producenta` są wolne, zostały pomyślnie przypisane do producenta zgodnie z zapytaniem.
- **status n** (dla `n > 0`) - proponowane przez producenta ID jest zajęte, przyznano pierwsze wolne ID → `n` jest nowym `id_producenta`.

### Wybór kanału
W strukturze `init_msg`, tablica `int info_type[5]` przechowuje informacje o kanałach, które producent chce nadawać. Wielkość ograniczona jest do 5 (zgodnie z założeniami projektu). Wartości kanałów zapisywane są zgodnie z podziałem:

```c
static char* types_of_info[] w pliku inf_16029_h.h
