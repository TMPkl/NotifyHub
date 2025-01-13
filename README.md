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
