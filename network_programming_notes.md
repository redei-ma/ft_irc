# Network Programming in C++ - Appunti Completi

## Parte 1: Configurazione Socket e Connessione Client-Server

### 1. Introduzione a WinSock

#### Concetti Base

-   **WinSock**: Windows Socket API (versione Windows dei socket Unix)
-   **WSA**: Windows Sockets API
-   **DLL Core**: `ws2_32.dll` (situata in `system32`)
-   **Scopo**: Gestisce connessione tra hardware e software a basso livello

#### Perché DLL?

-   **DLL** = Dynamic Link Library
-   Codice riutilizzabile da più applicazioni
-   Evita duplicazione del codice socket in ogni programma

---

### 2. Modello OSI (Open Systems Interconnection)

#### I 7 Livelli

```
7. APPLICATION    ← Codice C++
6. PRESENTATION   ← Serializzazione, buffering
5. SESSION        ← Socket
4. TRANSPORT      ← TCP/UDP
3. NETWORK        ← IP Address
2. DATA LINK      ← MAC Address
1. PHYSICAL       ← Hardware/Ethernet
```

#### Flusso di Dati

**SENDER (Discesa)**

```
Application (7)  →  Crea dati/eventi di gioco
     ↓
Presentation (6) →  Serializza e prepara pacchetti
     ↓
Session (5)      →  Gestisce connessione via socket
     ↓
Transport (4)    →  Sceglie TCP o UDP
     ↓
Network (3)      →  Aggiunge indirizzo IP
     ↓
Data Link (2)    →  Aggiunge MAC address
     ↓
Physical (1)     →  Trasmette segnale elettrico/ottico
```

**ROUTER (Bridge - Solo 3 livelli)**

```
Physical (1) → Riceve segnale
     ↓
Data Link (2) → Legge MAC address
     ↓
Network (3) → Instrada basandosi su IP
     ↓
Data Link (2) → Prepara per nuova trasmissione
     ↓
Physical (1) → Invia al destinatario
```

**RECEIVER (Salita)**

```
Physical (1)     →  Riceve segnale
     ↓
Data Link (2)    →  Verifica MAC address
     ↓
Network (3)      →  Verifica IP destinazione
     ↓
Transport (4)    →  Riassembla pacchetti TCP/UDP
     ↓
Session (5)      →  Mantiene connessione socket
     ↓
Presentation (6) →  Deserializza dati
     ↓
Application (7)  →  Aggiorna stato/input del gioco
```

**⚠️ IMPORTANTE**: I dati devono attraversare tutti i livelli in entrambe le direzioni

---

### 3. Architettura Client-Server

#### Ruoli Distinti

| **SERVER**               | **CLIENT**                     |
| ------------------------ | ------------------------------ |
| Ascolta su IP:Porta      | Invia richiesta di connessione |
| Accetta connessioni      | Specifica IP:Porta del server  |
| Può gestire più client   | Può connettersi a più server   |
| IP statico (tipicamente) | IP dinamico (possibile)        |

#### Indirizzi IP

**IP Loopback per Testing**

-   Indirizzo: `127.0.0.1`
-   Riferisce alla macchina locale
-   Utile per test senza configurare rete

**⚠️ NOTA IMPORTANTE**:

-   Porta SERVER: specificata nel codice
-   Porta CLIENT: allocata automaticamente dall'OS

---

### 4. Porte di Rete

#### Concetti

-   Ogni macchina ha fino a **65,536 porte** (indirizzo 16-bit)
-   Prime **1024 porte riservate** per applicazioni note
    -   Esempio: Porta 21 = FTP

#### Porta Consigliata per Esercizi

```
Porta: 55555
```

**Analogia**: Come i porti di una nazione

-   UK (IP) → Porto Dover (Porta)
-   Francia (IP) → Porto Calais (Porta)

---

### 5. Socket: Il Concetto Centrale

#### Definizione

**Socket** = "Tubo" che connette due computer per trasferire dati

#### Processo di Binding

```
Socket + IP Address + Port Number = Socket Vincolato
```

#### Tipi di Socket

| **TCP (Streaming Socket)** | **UDP (Datagram Socket)** |
| -------------------------- | ------------------------- |
| Connessione richiesta      | Nessuna connessione       |
| Affidabile e sicuro        | Meno affidabile           |
| Controlla pacchetti        | Non controlla ricezione   |
| Più lento                  | Più veloce                |
| Usa: file transfer, chat   | Usa: gaming, streaming    |

---

## 6. I 7 Step del Server

### Schema Completo

```
STEP 1: Carica DLL          → WSAStartup()
STEP 2: Crea Socket         → socket()
STEP 3: Bind IP:Porta       → bind()
STEP 4: Ascolta             → listen()
STEP 5: Accetta Client      → accept()
STEP 6: Invia/Ricevi Dati   → send()/recv()
STEP 7: Chiudi Socket       → closesocket() + WSACleanup()
```

---

### STEP 1: Carica DLL (WSAStartup)

#### Funzione

```cpp
int WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData);
```

#### Parametri

1. **wVersionRequested**: Versione WinSock (2.2)
2. **lpWSAData**: Puntatore a struttura WSADATA

#### Codice Esempio

```cpp
WSADATA wsaData;
WORD wVersionRequested = MAKEWORD(2, 2);
int wsaerr = WSAStartup(wVersionRequested, &wsaData);

if (wsaerr != 0) {
    cout << "WSAStartup fallito: " << wsaerr << endl;
    WSACleanup();
} else {
    cout << "WinSock DLL trovato: " << wsaData.szSystemStatus << endl;
}
```

**⚠️ NOTA**: Ritorna 0 se successo, valore errore altrimenti

#### Struttura WSADATA

```cpp
struct WSADATA {
    WORD wVersion;
    WORD wHighVersion;
    char szDescription[...];
    char szSystemStatus[...];
    // ... altri attributi
};
```

---

### STEP 2: Crea Socket

#### Funzione

```cpp
SOCKET socket(int af, int type, int protocol);
```

#### Parametri per TCP

```cpp
SOCKET serverSocket = socket(
    AF_INET,        // 1. Address Family: Internet
    SOCK_STREAM,    // 2. Type: TCP
    IPPROTO_TCP     // 3. Protocol: TCP
);
```

#### Validazione

```cpp
if (serverSocket == INVALID_SOCKET) {
    cout << "Errore: " << WSAGetLastError() << endl;
    WSACleanup();
    return 0;
}
```

**⚠️ RICORDA**: Chiamare `closesocket(serverSocket)` alla fine

---

### STEP 3: Bind Socket a IP:Porta

#### Funzione

```cpp
int bind(SOCKET s, const sockaddr* addr, int namelen);
```

#### Struttura sockaddr_in

```cpp
struct sockaddr_in {
    short sin_family;        // AF_INET
    unsigned short sin_port; // Porta (con htons)
    struct in_addr sin_addr; // IP Address
    char sin_zero[8];        // Padding
};
```

#### Codice Completo

```cpp
int port = 55555;
sockaddr_in service;

service.sin_family = AF_INET;
service.sin_addr.s_addr = inet_addr("127.0.0.1");
service.sin_port = htons(port);

if (bind(serverSocket, (sockaddr*)&service, sizeof(service)) == SOCKET_ERROR) {
    cout << "Bind fallito: " << WSAGetLastError() << endl;
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
```

**🔑 FUNZIONE CHIAVE**: `htons()` = Host TO Network Short

-   Converte da Little Endian a Big Endian (network byte order)

---

### STEP 4: Ascolta Connessioni

#### Funzione

```cpp
int listen(SOCKET s, int backlog);
```

#### Parametri

-   **s**: Socket da usare per ascoltare
-   **backlog**: Numero massimo di connessioni in coda

#### Codice

```cpp
if (listen(serverSocket, 1) == SOCKET_ERROR) {
    cout << "Errore listen: " << WSAGetLastError() << endl;
}
```

**💡 NOTA**: Nel nostro esempio `backlog = 1` (un solo client)

---

### STEP 5: Accetta Connessioni

#### Funzione

```cpp
SOCKET accept(SOCKET s, sockaddr* addr, int* addrlen);
```

#### Caratteristica Importante

**⚠️ FUNZIONE BLOCCANTE**: Pausa l'esecuzione fino a connessione client

#### Codice

```cpp
SOCKET acceptSocket = accept(serverSocket, NULL, NULL);

if (acceptSocket == INVALID_SOCKET) {
    cout << "Errore accept: " << WSAGetLastError() << endl;
    WSACleanup();
    return 0;
}
```

#### Risultato

-   **serverSocket**: Continua ad ascoltare
-   **acceptSocket**: Socket dedicato per comunicare con il client

```
┌──────────────┐
│ serverSocket │ → Rimane in ascolto
└──────────────┘
       ↓ crea
┌──────────────┐
│ acceptSocket │ → Connesso al client
└──────────────┘
```

---

## 7. I 5 Step del Client

### Schema Completo

```
STEP 1: Carica DLL          → WSAStartup()
STEP 2: Crea Socket         → socket()
STEP 3: Connetti al Server  → connect()
STEP 4: Invia/Ricevi Dati   → send()/recv()
STEP 5: Chiudi Socket       → closesocket() + WSACleanup()
```

---

### STEP 3 CLIENT: Connetti al Server

#### Funzione

```cpp
int connect(SOCKET s, const sockaddr* name, int namelen);
```

#### Caratteristiche

-   **Bind Automatico**: IP e porta client allocati automaticamente
-   **Specifica Destinazione**: IP:Porta del server
-   **Timeout**: 75 secondi di default

#### Codice Completo

```cpp
int port = 55555;
sockaddr_in clientService;

clientService.sin_family = AF_INET;
clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
clientService.sin_port = htons(port);

if (connect(clientSocket, (sockaddr*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
    cout << "Connessione fallita: " << WSAGetLastError() << endl;
    closesocket(clientSocket);
    WSACleanup();
    return 0;
} else {
    cout << "Connesso al server!" << endl;
}
```

---

## Parte 2: Trasmissione Dati TCP e UDP

### 8. Protocollo TCP (Transmission Control Protocol)

#### Caratteristiche

✅ Richiede connessione stabilita  
✅ Affidabile - controlla tutti i pacchetti  
✅ Ordine garantito  
✅ Ideale per: chat, file transfer, email

#### Step già configurati per TCP

-   Socket creato con `SOCK_STREAM`
-   Binding IP:Porta completato
-   Connessione stabilita con `accept()`/`connect()`

---

### STEP 6 SERVER / STEP 4 CLIENT: Invia Dati (TCP)

#### Funzione send()

```cpp
int send(SOCKET s, const char* buf, int len, int flags);
```

#### Parametri

1. **s**: Socket connesso (acceptSocket o clientSocket)
2. **buf**: Puntatore a char array con messaggio
3. **len**: Lunghezza del messaggio
4. **flags**: 0 per default

#### Ritorno

-   **> 0**: Numero di byte inviati (SUCCESSO)
-   **0 o SOCKET_ERROR**: Errore

#### Codice Completo - Client

```cpp
char buffer[200];

printf("Inserisci messaggio da inviare: ");
cin.getline(buffer, 200);

int byteCount = send(clientSocket, buffer, 200, 0);

if (byteCount == SOCKET_ERROR) {
    printf("Errore invio: %ld\n", WSAGetLastError());
} else {
    printf("Messaggio inviato: %s\n", buffer);
}
```

**🔑 FUNZIONI UTILI**:

-   `cin.getline()`: Legge intera riga (inclusi spazi)
-   `printf()`: Alternativa a cout con placeholder `%ld`, `%s`

---

### STEP 6 SERVER / STEP 4 CLIENT: Ricevi Dati (TCP)

#### Funzione recv()

```cpp
int recv(SOCKET s, char* buf, int len, int flags);
```

#### Parametri (identici a send)

1. **s**: Socket connesso
2. **buf**: Buffer per ricevere dati
3. **len**: Dimensione buffer
4. **flags**: 0 per default

#### Codice Completo - Server

```cpp
char receiveBuffer[200];

int byteCount = recv(acceptSocket, receiveBuffer, 200, 0);

if (byteCount < 0) {
    printf("Errore ricezione: %ld\n", WSAGetLastError());
} else {
    printf("Messaggio ricevuto: %s\n", receiveBuffer);
}
```

**⚠️ FUNZIONE BLOCCANTE**: Pausa fino a ricezione dati

---

### Sequenza Completa Client-Server (TCP)

```
CLIENT                          SERVER
  |                               |
  | 1. send("Ciao")              |
  |----------------------------->|
  |                              | 2. recv() riceve "Ciao"
  |                              |
  |                              | 3. send("Ricevuto")
  |<-----------------------------|
  | 4. recv() riceve "Ricevuto"  |
  |                               |
```

**📌 IMPORTANTE**:

-   Client inizia con `send()` poi `recv()`
-   Server inizia con `recv()` poi `send()`

---

### 9. Invio di Oggetti (TCP)

#### Scenario

Inviare oggetto di classe invece di semplice stringa

#### Esempio Classe

```cpp
class Data {
public:
    int value;
    Data(int v) : value(v) {}
};
```

#### Invio Oggetto - Client

```cpp
Data data(42);

int byteCount = send(
    clientSocket,
    (char*)&data,           // Cast oggetto a char*
    sizeof(Data),           // Dimensione classe
    0
);
```

#### Ricezione Oggetto - Server

```cpp
Data receivedData;

int byteCount = recv(
    acceptSocket,
    (char*)&receivedData,   // Cast a char*
    sizeof(Data),
    0
);

cout << "Valore ricevuto: " << receivedData.value << endl;
```

**🔑 PUNTI CHIAVE**:

-   Cast esplicito: `(char*)&oggetto`
-   Usa `sizeof(NomeClasse)` non `sizeof(oggetto)`
-   Classe deve essere definita in entrambi client e server

---

### 10. Funzioni String Utili

#### strcpy_s() - Copia Stringa

```cpp
char buffer[200];
strcpy_s(buffer, "Messaggio iniziale");
// Permette sovrascrittura successiva
```

#### strlen() - Lunghezza Stringa

```cpp
int lunghezza = strlen(buffer);
```

#### strcmp() - Confronto Stringhe

```cpp
char password[200];
cin.getline(password, 200);

if (strcmp(password, "PASSWORD") == 0) {
    cout << "Password corretta!" << endl;
}
```

**Ritorna**:

-   `0`: Stringhe uguali
-   `< 0`: Prima stringa minore
-   `> 0`: Prima stringa maggiore

**💡 NOTA**: Confronto basato su valori ASCII

---

## 11. Protocollo UDP (User Datagram Protocol)

### Caratteristiche UDP

| Caratteristica   | TCP          | UDP              |
| ---------------- | ------------ | ---------------- |
| Connessione      | ✅ Richiesta | ❌ Non richiesta |
| Affidabilità     | ✅ Alta      | ⚠️ Bassa         |
| Ordine pacchetti | ✅ Garantito | ❌ Non garantito |
| Velocità         | 🐢 Più lento | 🚀 Più veloce    |
| Overhead         | Alto         | Basso            |

### Quando Usare UDP?

✅ Gaming multiplayer  
✅ Streaming video/audio  
✅ Broadcasting dati in tempo reale  
✅ Situazioni dove perdita occasionale è accettabile

---

### Configurazione Socket UDP

#### Differenze negli Step

| Step                  | TCP           | UDP                    |
| --------------------- | ------------- | ---------------------- |
| 1. WSAStartup         | ✅ Uguale     | ✅ Uguale              |
| 2. socket()           | `SOCK_STREAM` | `SOCK_DGRAM`           |
| 3. bind()             | ✅ Server     | ✅ Server              |
| 4. listen()           | ✅ Server     | ❌ Non usato           |
| 5. accept()/connect() | ✅ Entrambi   | ❌ Non usato           |
| 6. send/recv          | ✅ Usa        | ❌ Usa sendto/recvfrom |

#### Creazione Socket UDP

```cpp
SOCKET udpSocket = socket(
    AF_INET,        // Address Family
    SOCK_DGRAM,     // UDP Datagram
    IPPROTO_UDP     // Protocollo UDP
);
```

**⚠️ CAMBIA**: Solo parametri 2 e 3 rispetto a TCP

---

### Invio Dati UDP - sendto()

#### Funzione

```cpp
int sendto(
    SOCKET s,
    const char* buf,
    int len,
    int flags,
    const sockaddr* to,
    int tolen
);
```

#### Parametri Aggiuntivi vs send()

5. **to**: Struttura sockaddr con IP:Porta destinazione
6. **tolen**: Dimensione struttura

#### Codice Completo

```cpp
// Definisci destinazione
sockaddr_in address;
address.sin_family = AF_INET;
address.sin_addr.s_addr = inet_addr("127.0.0.1");
address.sin_port = htons(55555);

// Prepara messaggio
char buffer[200] = "Messaggio UDP al server";

// Invia
int byteCount = sendto(
    clientSocket,
    buffer,
    strlen(buffer),
    0,
    (sockaddr*)&address,
    sizeof(address)
);

if (byteCount == -1) {
    cout << "Errore invio UDP" << endl;
} else {
    cout << "Inviati " << byteCount << " byte" << endl;
}
```

**🔑 NOTA**: IP:Porta specificati nella funzione, non in connect()

---

### Ricezione Dati UDP - recvfrom()

#### Funzione

```cpp
int recvfrom(
    SOCKET s,
    char* buf,
    int len,
    int flags,
    sockaddr* from,
    int* fromlen
);
```

#### Parametri

5. **from**: Riceve info su mittente
6. **fromlen**: Dimensione struttura from

#### Codice Completo

```cpp
sockaddr_in clientAddress;
int clientAddressSize = sizeof(clientAddress);
char receiveBuffer[200];

int byteCount = recvfrom(
    serverSocket,
    receiveBuffer,
    200,
    0,
    (sockaddr*)&clientAddress,
    &clientAddressSize
);

if (byteCount < 0) {
    cout << "Errore ricezione" << endl;
} else {
    cout << "Ricevuto: " << receiveBuffer << endl;
}
```

**⚠️ FUNZIONE BLOCCANTE**: Aspetta fino a ricezione dati

---

### 12. Invio Oggetti UDP

#### Classe Esempio: Vector3D

```cpp
class Vector3D {
public:
    float x, y, z;

    Vector3D(float x, float y, float z)
        : x(x), y(y), z(z) {}
};
```

#### Preparazione Dati con sprintf()

```cpp
Vector3D position(1.5f, 2.3f, 4.7f);
char positionBuffer[200];

sprintf(positionBuffer, "%6.1f %6.1f %6.1f",
        position.x,
        position.y,
        position.z);

// Invia
sendto(socket, positionBuffer, 200, 0, ...);
```

#### Formato Placeholder

-   `%6.1f`:
    -   `6`: Larghezza totale
    -   `.1`: 1 decimale
    -   `f`: Tipo float

**vs TCP**: UDP richiede conversione manuale a char array

---

## 13. Setup Progetto Visual Studio

### File Necessari

1. **stdafx.h** (Header precompilato)
2. **targetver.h** (Versione Windows target)

### Configurazione Dipendenze

#### Passi

1. **Project** → **Properties**
2. **Linker** → **Input**
3. **Additional Dependencies** → Edit
4. Aggiungi: `ws2_32.lib`
5. **Apply** → **OK**

### Struttura Progetti

```
📁 NetworkProgramming/
├── 📁 ServerProject/
│   ├── stdafx.h
│   ├── targetver.h
│   └── Server.cpp
└── 📁 ClientProject/
    ├── stdafx.h
    ├── targetver.h
    └── Client.cpp
```

**⚠️ IMPORTANTE**: Due progetti separati (due .sln)

---

## 14. Sequenze di Esecuzione

### TCP - Chat Completa

#### Server

```cpp
// Step 1-5: Setup e accept()
SOCKET acceptSocket = accept(serverSocket, NULL, NULL);

// Step 6: Loop chat
while (true) {
    // Ricevi
    recv(acceptSocket, buffer, 200, 0);
    cout << "Client: " << buffer << endl;

    // Controlla uscita
    if (strcmp(buffer, "shutdown") == 0) break;

    // Invia
    cin.getline(reply, 200);
    send(acceptSocket, reply, 200, 0);
}

// Step 7: Cleanup
closesocket(acceptSocket);
closesocket(serverSocket);
WSACleanup();
```

#### Client

```cpp
// Step 1-3: Setup e connect()

// Step 4: Loop chat
while (true) {
    // Invia
    cin.getline(buffer, 200);
    send(clientSocket, buffer, 200, 0);

    if (strcmp(buffer, "shutdown") == 0) break;

    // Ricevi
    recv(clientSocket, buffer, 200, 0);
    cout << "Server: " << buffer << endl;
}

// Step 5: Cleanup
closesocket(clientSocket);
WSACleanup();
```

---

## 15. Esercizi Proposti

### Esercizio 1: Basic TCP

✅ Implementa Step 1-5 Server  
✅ Implementa Step 1-3 Client  
✅ Client invia messaggio  
✅ Server riceve e visualizza

### Esercizio 2: Receipt Automatico

✅ Server risponde con "Messaggio ricevuto"  
✅ Client visualizza conferma

### Esercizio 3: Chat Continua

✅ Loop infinito send/recv  
✅ Terminazione con keyword "shutdown"

### Esercizio 4: Threading (Avanzato)

✅ Thread dedicato per listening  
✅ Thread separato per sending  
✅ Comunicazione bidirezionale simultanea

### Esercizio 5: UDP Broadcast

✅ Converti chat in UDP  
✅ Test perdita pacchetti  
✅ Confronta performance TCP vs UDP

---

## 16. Troubleshooting Comune

### Errori Frequenti

#### "Cannot find ws2_32.lib"

**Soluzione**: Verifica dipendenze linker

#### "INVALID_SOCKET"

**Causa**: Parametri errati in socket()  
**Soluzione**: Controlla AF_INET, SOCK_STREAM, IPPROTO_TCP

#### "Bind failed"

**Causa 1**: Porta già in uso  
**Causa 2**: IP non valido  
**Soluzione**: Cambia porta o verifica IP

#### Server non riceve dati

**Causa**: Ordine send/recv invertito  
**Soluzione**:

-   Server: recv() → send()
-   Client: send() → recv()

#### "Connection refused"

**Causa**: Server non in ascolto  
**Soluzione**: Avvia server PRIMA del client

---

## 17. Codice Template Completo

### Server TCP Minimo

```cpp
#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

int main() {
    // 1. WSAStartup
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    // 2. Socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // 3. Bind
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
    service.sin_port = htons(55555);
    bind(serverSocket, (sockaddr*)&service, sizeof(service));

    // 4. Listen
    listen(serverSocket, 1);
    cout << "Server in ascolto..." << endl;

    // 5. Accept
    SOCKET acceptSocket = accept(serverSocket, NULL, NULL);
    cout << "Client connesso!" << endl;

    // 6. Ricevi
    char buffer[200];
    recv(acceptSocket, buffer, 200, 0);
    cout << "Ricevuto: " << buffer << endl;

    // Invia risposta
    char reply[200] = "Messaggio ricevuto";
    send(acceptSocket, reply, 200, 0);

    // 7. Cleanup
    closesocket(acceptSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
```

### Client TCP Minimo

```cpp
#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

int main() {
    // 1. WSAStartup
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    // 2. Socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // 3. Connect
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
    clientService.sin_port = htons(55555);

    if (connect(clientSocket, (sockaddr*)&clientService, sizeof(clientService)) == 0) {
        cout << "Connesso al server!" << endl;

        // 4. Invia
        char buffer[200];
        cout << "Messaggio: ";
        cin.getline(buffer, 200);
        send(clientSocket, buffer, 200, 0);

        // Ricevi risposta
        recv(clientSocket, buffer, 200, 0);
        cout << "Server: " << buffer << endl;
    }

    // 5. Cleanup
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
```

---

## 18. Checklist Pre-Esecuzione

### Prima di Compilare

-   [ ] File stdafx.h e targetver.h presenti
-   [ ] Dipendenza ws2_32.lib aggiunta
-   [ ] Porta non in conflitto (55555)
-   [ ] Header `#include <winsock2.h>` presente

### Prima di Eseguire

-   [ ] Server compilato senza errori
-   [ ] Client compilato senza errori
-   [ ] Avvia SERVER per primo
-   [ ] Poi avvia CLIENT
-   [ ] Entrambe le console visibili

### Debug

-   [ ] Usa `WSAGetLastError()` per errori specifici
-   [ ] Verifica valori ritorno (0 = successo per setup, >0 per send/recv)
-   [ ] Aggiungi `system("pause")` prima di chiusura

---

## 19. Concetti Avanzati (Per Approfondimento)

### Threading con Socket

-   Thread per listening continuo
-   Thread per input utente
-   Sincronizzazione con mutex

### Gestione Multipli Client

-   Array di socket accettati
-   Loop accept() in thread separato
-   Broadcast a tutti i client

### Sicurezza

-   SSL/TLS per crittografia
-   Autenticazione client
-   Validazione input

### Performance

-   Buffer circolari
-   Non-blocking sockets
-   Select() per multiplexing

---

## 20. Glossario Tecnico

| Termine        | Significato                         |
| -------------- | ----------------------------------- |
| **Socket**     | Endpoint per comunicazione di rete  |
| **Binding**    | Associare socket a IP:Porta         |
| **Listening**  | Attesa connessioni in ingresso      |
| **Blocking**   | Funzione che pausa esecuzione       |
| **Byte Order** | Ordine byte in memoria (Endianness) |
| **Loopback**   | Indirizzo locale (127.0.0.1)        |
| **Port**       | Numero identificativo per servizio  |
| **Protocol**   | Regole per comunicazione (TCP/UDP)  |
| **Handshake**  | Processo stabilire connessione      |
| **Packet**     | Unità dati trasmessa                |

---

## 📚 Risorse per Studio

### Priorità Alta

1. ⭐ **Modello OSI** - Comprendere i 7 livelli
2. ⭐ **Differenze TCP/UDP** - Quando usare quale
3. ⭐ **7 Step Server + 5 Step Client** - Memorizzare sequenza

### Priorità Media

4. Funzioni WinSock API (send, recv, bind, etc.)
5. Strutture dati (sockaddr_in, WSADATA)
6. Gestione errori con WSAGetLastError()

### Approfondimenti

7. Threading per socket asincroni
8. Protocolli applicativi (HTTP, FTP su TCP)
9. Network byte order (Big Endian vs Little Endian)

---

## ✅ Quick Reference

### Ordine Funzioni Server

```
WSAStartup → socket → bind → listen → accept → recv/send → closesocket → WSACleanup
```

### Ordine Funzioni Client

```
WSAStartup → socket → connect → send/recv → closesocket → WSACleanup
```

### Valori Ritorno Importanti

-   `WSAStartup()`: 0 = successo
-   `socket()`: INVALID_SOCKET = errore
-   `bind/listen/connect()`: SOCKET_ERROR = errore
-   `send/recv()`: >0 = byte trasferiti, ≤0 = errore

### Costanti Essenziali

```cpp
AF_INET          // IPv4
SOCK_STREAM      // TCP
SOCK_DGRAM       // UDP
IPPROTO_TCP      // Protocollo TCP
IPPROTO_UDP      // Protocollo UDP
INVALID_SOCKET   // Socket non valido
SOCKET_ERROR     // Operazione fallita
```

---

**Fine Appunti Network Programming C++**
