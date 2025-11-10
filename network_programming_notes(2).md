# Network Programming in C++ - Appunti Completi

## Parte 0: Fondamenti Teorici di Rete

### 0.1 Cos'è una Rete di Computer?

#### Definizione Base

Una **rete di computer** è un insieme di dispositivi interconnessi che possono comunicare e scambiare dati tra loro.

#### Componenti Fondamentali

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  Computer A │────►│   Router    │◄────│  Computer B │
└─────────────┘     └─────────────┘     └─────────────┘
     ↑                     ↑                   ↑
  Nodo/Host            Switch/Hub          Nodo/Host
```

**Elementi Chiave:**

-   **Nodi/Host**: Dispositivi connessi (computer, server, smartphone)
-   **Link**: Mezzi di comunicazione (cavi Ethernet, Wi-Fi, fibra ottica)
-   **Router**: Instrada pacchetti tra diverse reti
-   **Switch**: Connette dispositivi sulla stessa rete locale

---

### 0.2 Tipi di Rete

#### Per Dimensione

| Tipo    | Nome Completo             | Copertura         | Esempio                             |
| ------- | ------------------------- | ----------------- | ----------------------------------- |
| **PAN** | Personal Area Network     | 1-10 metri        | Bluetooth tra telefono e auricolari |
| **LAN** | Local Area Network        | Casa/Ufficio      | Rete domestica Wi-Fi                |
| **MAN** | Metropolitan Area Network | Città             | Rete universitaria multi-campus     |
| **WAN** | Wide Area Network         | Nazionale/Globale | Internet                            |

#### Per Topologia

**Topologia** = Disposizione fisica/logica dei dispositivi

```
BUS:              STAR:            RING:
────●────●────    ┌───┐            ●
    │    │        │ H │            ↓
    ●    ●        └─┬─┘          ● → ●
                  ┌─┴─┐          ↑
                  ●   ●          ●
```

---

### 0.3 Protocolli di Rete

#### Cos'è un Protocollo?

Un **protocollo** è un insieme di regole che definiscono come i dati vengono formattati, trasmessi e ricevuti.

**Analogia**: Come le regole del traffico stradale

-   Semaforo rosso = stop
-   Verde = vai
-   Corsia di sorpasso a sinistra

#### Protocolli Principali per Layer OSI

| Layer OSI       | Protocollo      | Funzione                   |
| --------------- | --------------- | -------------------------- |
| 7. Application  | HTTP, FTP, SMTP | Servizi applicativi        |
| 6. Presentation | SSL/TLS, JPEG   | Crittografia, compressione |
| 5. Session      | NetBIOS, RPC    | Gestione sessioni          |
| 4. Transport    | **TCP, UDP**    | Trasferimento dati         |
| 3. Network      | **IP, ICMP**    | Routing pacchetti          |
| 2. Data Link    | Ethernet, Wi-Fi | Comunicazione locale       |
| 1. Physical     | Ethernet fisico | Trasmissione bit           |

**In questo corso ci concentriamo su Layer 4 e 5: TCP/UDP e Socket**

---

### 0.4 Indirizzamento IP

#### Cos'è un Indirizzo IP?

Un **indirizzo IP** (Internet Protocol) è un identificatore numerico univoco assegnato a ogni dispositivo su una rete.

#### IPv4 (32 bit)

**Formato**: 4 ottetti separati da punti

```
192.168.1.100
 │   │   │  │
 │   │   │  └─ Host (0-255)
 │   │   └──── Sottorete
 │   └──────── Rete privata
 └──────────── Classe C privata
```

**Classi IPv4:**

| Classe | Range                       | Uso Tipico            |
| ------ | --------------------------- | --------------------- |
| A      | 0.0.0.0 - 127.255.255.255   | Grandi organizzazioni |
| B      | 128.0.0.0 - 191.255.255.255 | Medie organizzazioni  |
| C      | 192.0.0.0 - 223.255.255.255 | Piccole reti          |
| D      | 224.0.0.0 - 239.255.255.255 | Multicast             |
| E      | 240.0.0.0 - 255.255.255.255 | Riservato             |

**IP Speciali:**

```cpp
127.0.0.1        // Loopback - "questa macchina"
0.0.0.0          // "Qualsiasi indirizzo"
255.255.255.255  // Broadcast - "tutti sulla rete"
192.168.x.x      // Reti private (non instradabili su Internet)
10.x.x.x         // Reti private grandi
```

#### IP Statico vs Dinamico

**IP Statico:**

-   Assegnato manualmente
-   Non cambia mai
-   Usato per server

```cpp
service.sin_addr.s_addr = inet_addr("192.168.1.100");
```

**IP Dinamico:**

-   Assegnato automaticamente da DHCP
-   Può cambiare ad ogni connessione
-   Usato per client (laptop, smartphone)

---

### 0.5 Porte e Multiplexing

#### Perché Servono le Porte?

Un computer può eseguire **molti programmi di rete contemporaneamente**:

```
Computer A (IP: 192.168.1.10)
├─ Browser → Porta 54321 → connesso a Server Web
├─ Email   → Porta 54322 → connesso a Server Email
├─ Gioco   → Porta 54323 → connesso a Game Server
└─ Chat    → Porta 54324 → connesso a Chat Server
```

**Senza porte**: Come distinguere quale pacchetto va a quale applicazione?

#### Range di Porte (0-65535)

| Range       | Nome                 | Uso                                   |
| ----------- | -------------------- | ------------------------------------- |
| 0-1023      | **Well-Known Ports** | Servizi standard (HTTP=80, HTTPS=443) |
| 1024-49151  | **Registered Ports** | Applicazioni specifiche               |
| 49152-65535 | **Dynamic/Private**  | Allocazione temporanea client         |

**Porte Famose:**

```
20/21   - FTP
22      - SSH
23      - Telnet
25      - SMTP (Email)
53      - DNS
80      - HTTP
443     - HTTPS
3306    - MySQL
5432    - PostgreSQL
```

**Per i nostri esercizi usiamo: 55555** (porta libera)

---

### 0.6 Byte Order (Endianness)

#### Il Problema

Computer diversi memorizzano i byte in ordine diverso!

**Esempio**: Numero 0x12345678 in memoria

**Little Endian** (Intel x86):

```
Indirizzo:  0x00  0x01  0x02  0x03
Valore:     78    56    34    12
            └─────────────────┘
            Byte meno significativo PRIMA
```

**Big Endian** (Network, ARM):

```
Indirizzo:  0x00  0x01  0x02  0x03
Valore:     12    34    56    78
            └─────────────────┘
            Byte più significativo PRIMA
```

#### Perché è Importante per le Reti?

Le **reti usano Big Endian** (network byte order).  
I PC Intel usano **Little Endian** (host byte order).

**Soluzione**: Funzioni di conversione!

```cpp
// Host TO Network Short (16 bit)
htons(55555)  // Converte porta da host a network order

// Host TO Network Long (32 bit)
htonl(ip)     // Converte IP da host a network order

// Network TO Host Short
ntohs(port)   // Converte porta da network a host order

// Network TO Host Long
ntohl(ip)     // Converte IP da network a host order
```

**Esempio Pratico:**

```cpp
// PC Intel (Little Endian)
unsigned short port = 55555;  // 0xD903 in esadecimale

// Senza conversione (SBAGLIATO per rete):
// Byte in memoria: 03 D9 (Little Endian)

// Con htons() (CORRETTO):
unsigned short networkPort = htons(port);
// Byte in memoria: D9 03 (Big Endian, pronto per rete)

service.sin_port = networkPort;  // ✅ Corretto!
```

**⚠️ REGOLA**: Usa SEMPRE `htons()` per porte e `htonl()` per IP!

---

### 0.7 Pacchetti di Rete

#### Cos'è un Pacchetto?

I dati NON vengono inviati come un blocco unico, ma divisi in **pacchetti**.

**Analogia**: Spedire un libro

-   Non mandi il libro intero in una busta
-   Lo dividi in capitoli
-   Ogni capitolo in una busta separata
-   Destinatario riassembla i capitoli

#### Struttura di un Pacchetto TCP/IP

```
┌─────────────────────────────────────────┐
│          ETHERNET HEADER                │  Layer 2
│  [MAC Src][MAC Dst][Type]              │
├─────────────────────────────────────────┤
│             IP HEADER                   │  Layer 3
│  [Src IP][Dst IP][Protocol][Checksum]  │
├─────────────────────────────────────────┤
│            TCP HEADER                   │  Layer 4
│  [Src Port][Dst Port][Seq][Ack][Flags] │
├─────────────────────────────────────────┤
│              PAYLOAD                    │  Layer 7
│        (I tuoi dati effettivi)          │
│          "Ciao server!"                 │
└─────────────────────────────────────────┘
```

#### Incapsulamento

Ogni layer aggiunge il suo header:

```
Applicazione:  "Ciao"
    ↓
Transport:     [TCP Header] + "Ciao"
    ↓
Network:       [IP Header] + [TCP Header] + "Ciao"
    ↓
Data Link:     [Ethernet] + [IP] + [TCP] + "Ciao" + [CRC]
    ↓
Physical:      10110101... (bit sul cavo)
```

**Decapsulamento** (destinatario fa l'opposto):

```
Physical → Data Link → Network → Transport → Application
```

---

### 0.8 Modello Client-Server vs Peer-to-Peer

#### Modello Client-Server

```
        ┌──────────┐
        │  SERVER  │
        │ (Sempre  │
        │  attivo) │
        └────┬─────┘
             │
      ┌──────┼──────┐
      │      │      │
   ┌──▼──┐ ┌─▼───┐ ┌▼────┐
   │ C1  │ │ C2  │ │ C3  │
   │     │ │     │ │     │
   └─────┘ └─────┘ └─────┘
   Client  Client  Client
```

**Caratteristiche:**

-   Server: sempre attivo, IP fisso, fornisce servizi
-   Client: si connette quando serve, IP dinamico, richiede servizi
-   Comunicazione: sempre Client ↔ Server (mai Client ↔ Client diretto)

**Esempi**: Web, Email, Database

#### Modello Peer-to-Peer (P2P)

```
   ┌─────┐     ┌─────┐
   │ P1  │◄───►│ P2  │
   └──┬──┘     └──┬──┘
      │           │
      └─────┬─────┘
            │
         ┌──▼──┐
         │ P3  │
         └─────┘
```

**Caratteristiche:**

-   Nessun server centrale
-   Ogni nodo è sia client che server
-   Comunicazione: diretta tra peer

**Esempi**: BitTorrent, Blockchain

**Nel nostro corso usiamo Client-Server!**

---

### 0.9 Three-Way Handshake (TCP)

#### Come TCP Stabilisce una Connessione

TCP usa un processo a **3 passi** per garantire connessione affidabile:

```
CLIENT                          SERVER
  │                               │
  │  1. SYN (Seq=100)            │
  │──────────────────────────────►│
  │                               │ "Voglio connettermi"
  │                               │
  │  2. SYN-ACK (Seq=200, Ack=101)│
  │◄──────────────────────────────│
  │  "OK, sono pronto"            │
  │                               │
  │  3. ACK (Seq=101, Ack=201)   │
  │──────────────────────────────►│
  │                               │ "Confermo, connessi!"
  │                               │
  │ ═════ CONNESSIONE STABILITA ══│
```

**Spiegazione:**

1. **SYN** (Synchronize): Client chiede di connettersi
2. **SYN-ACK**: Server accetta e risponde
3. **ACK** (Acknowledge): Client conferma

**Dopo questi 3 step:**

-   `connect()` nel client ritorna con successo
-   `accept()` nel server ritorna con successo
-   Può iniziare trasmissione dati

#### Chiusura Connessione (Four-Way Handshake)

```
CLIENT                          SERVER
  │                               │
  │  1. FIN                       │
  │──────────────────────────────►│
  │                               │
  │  2. ACK                       │
  │◄──────────────────────────────│
  │                               │
  │  3. FIN                       │
  │◄──────────────────────────────│
  │                               │
  │  4. ACK                       │
  │──────────────────────────────►│
  │                               │
  │ ═════ CONNESSIONE CHIUSA ═════│
```

---

### 0.10 UDP: Senza Connessione

#### Differenza Fondamentale

**UDP non fa handshake!**

```
CLIENT                          SERVER
  │                               │
  │  Dati pacchetto 1            │
  │──────────────────────────────►│
  │                               │ Nessuna conferma!
  │  Dati pacchetto 2            │
  │──────────────────────────────►│
  │                               │
  │  Dati pacchetto 3            │
  │─────────────────────────X    │ Perso!
  │                               │ UDP: "Non è il mio problema"
```

**Vantaggi UDP:**

-   ⚡ Più veloce (no handshake)
-   📦 Meno overhead (header più piccoli)
-   🎮 Latenza minore

**Svantaggi UDP:**

-   ❌ Pacchetti possono perdersi
-   ❌ Ordine non garantito
-   ❌ Duplicati possibili

---

### 0.11 Firewall e NAT

#### Firewall

Un **firewall** blocca/permette traffico di rete basandosi su regole.

```
Internet
   │
   ▼
┌──────────┐
│ FIREWALL │ ← Regole:
│          │   - Blocca porta 23 (Telnet)
└────┬─────┘   - Permetti porta 80 (HTTP)
     │          - Permetti porta 443 (HTTPS)
     ▼
  Rete Locale
```

**Per i nostri esercizi:**

-   Windows Firewall potrebbe bloccare il programma
-   Soluzione: Autorizza applicazione nelle impostazioni firewall

#### NAT (Network Address Translation)

Permette a più dispositivi di condividere un IP pubblico.

```
RETE LOCALE               ROUTER (NAT)          INTERNET
192.168.1.10:54321   →   85.20.30.40:12345  →  Server
192.168.1.11:54322   →   85.20.30.40:12346  →  Server
192.168.1.12:54323   →   85.20.30.40:12347  →  Server
  (IP privato)           (IP pubblico)
```

**Problemi con NAT:**

-   Server dietro NAT difficili da raggiungere dall'esterno
-   Soluzione: Port forwarding sul router

---

### 0.12 DNS (Domain Name System)

#### Dal Nome all'IP

Gli umani preferiscono nomi (`www.google.com`), i computer usano IP (`142.250.180.46`).

**DNS traduce nomi in IP:**

```
1. Browser: "Voglio www.google.com"
        ↓
2. PC chiede al DNS Server: "Qual è l'IP di www.google.com?"
        ↓
3. DNS Server risponde: "142.250.180.46"
        ↓
4. PC si connette a 142.250.180.46
```

**Nel nostro codice:**

```cpp
// Usando IP direttamente
inet_addr("127.0.0.1")

// Per usare nomi di dominio (più avanzato):
// Serve funzione gethostbyname() che interroga DNS
```

---

### 0.13 Buffering e Flow Control

#### Cos'è un Buffer?

Un **buffer** è un'area di memoria temporanea per contenere dati in transito.

```
SENDER                   BUFFER                 RECEIVER
  │                    ┌──────┐                    │
  │  write() ─────────►│ XXXX │                    │
  │  write() ─────────►│ XXXX │                    │
  │                    │ XXXX │◄────── recv()      │
  │                    │      │                     │
  │                    └──────┘                     │
```

**Perché Serve?**

-   Sender più veloce del receiver → buffer accumula dati
-   Receiver più veloce del sender → buffer si svuota

#### Flow Control in TCP

TCP regola automaticamente la velocità di trasmissione:

```
SENDER                              RECEIVER
  │                                     │
  │  Invia 1000 byte                   │
  │────────────────────────────────────►│
  │                                     │ Buffer: 500 byte liberi
  │  ACK: "Posso ricevere 500 byte"   │
  │◄────────────────────────────────────│
  │                                     │
  │  Invia 500 byte                    │
  │────────────────────────────────────►│
```

**Window Size** = Quanti dati possono essere inviati prima di ACK

**Nel nostro codice:**

```cpp
char buffer[200];  // ← Questo è il nostro buffer!

// Può contenere max 200 caratteri
// Se messaggio è > 200, verrà troncato o serve logica multi-pacchetto
```

---

### 0.14 Serializzazione e Deserializzazione

#### Il Problema

Come inviare strutture dati complesse in rete?

```cpp
struct Player {
    int id;
    float x, y, z;
    char name[20];
};

Player p = {1, 10.5f, 20.3f, 5.0f, "Mario"};

// Non puoi fare:
send(socket, (char*)&p, sizeof(Player), 0);  // ⚠️ Rischioso!
```

**Problemi:**

1. Padding della struct varia per compilatore
2. Endianness diverso tra macchine
3. Puntatori non hanno senso in rete

#### Serializzazione Manuale (Approccio Sicuro)

```cpp
// SENDER: Serializza in un formato standard
char buffer[200];
sprintf(buffer, "%d|%.1f|%.1f|%.1f|%s",
        p.id, p.x, p.y, p.z, p.name);
// Risultato: "1|10.5|20.3|5.0|Mario"

send(socket, buffer, strlen(buffer), 0);
```

```cpp
// RECEIVER: Deserializza
char buffer[200];
recv(socket, buffer, 200, 0);

Player p;
sscanf(buffer, "%d|%f|%f|%f|%s",
       &p.id, &p.x, &p.y, &p.z, p.name);
// Ha ricostruito la struct!
```

**Formati Standard:**

-   JSON: `{"id":1,"x":10.5,"name":"Mario"}`
-   XML: `<player><id>1</id><x>10.5</x>...</player>`
-   Protocol Buffers (Google)
-   MessagePack

---

### 0.15 Blocking vs Non-Blocking Sockets

#### Blocking (Default)

**Funzioni bloccanti fermano l'esecuzione** fino a completamento:

```cpp
// Server
SOCKET clientSocket = accept(serverSocket, NULL, NULL);
// ↑ BLOCCA qui fino a connessione client!
// Il programma è "congelato" su questa riga

// Dopo accettazione continua...
recv(clientSocket, buffer, 200, 0);
// ↑ BLOCCA qui fino a ricezione dati!
```

**Problemi:**

-   Non puoi fare altro mentre aspetti
-   Server single-threaded può gestire solo 1 client alla volta

#### Non-Blocking (Avanzato)

```cpp
// Imposta socket non-bloccante
u_long mode = 1;
ioctlsocket(serverSocket, FIONBIO, &mode);

// Ora accept() ritorna IMMEDIATAMENTE
SOCKET clientSocket = accept(serverSocket, NULL, NULL);

if (clientSocket == INVALID_SOCKET) {
    // Nessuna connessione disponibile, continua con altro lavoro
    doOtherStuff();
} else {
    // C'è una connessione!
    handleClient(clientSocket);
}
```

**Soluzione Migliore: Threading**

-   Thread separato per `accept()`
-   Thread separato per ogni client
-   Main thread gestisce logica del gioco/app

---

## Parte 1: Configurazione Socket e Connessione Client-Server

### 1A. Socket Programming: Unix/Linux/macOS vs Windows

#### Panoramica delle Differenze

**Sistema Unix/Linux/macOS (POSIX):**

-   ✅ API Berkeley Sockets (standard originale anni '80)
-   ✅ Librerie integrate nel kernel
-   ✅ Socket = file descriptor (come file normali)
-   ✅ Nessuna inizializzazione speciale richiesta

**Sistema Windows (WinSock):**

-   ⚠️ Basato su Berkeley Sockets ma con modifiche
-   ⚠️ Richiede DLL separata (ws2_32.dll)
-   ⚠️ Socket = SOCKET type (non int)
-   ⚠️ Richiede WSAStartup() e WSACleanup()

---

### 1B. Guida Completa Unix/Linux/macOS

#### Header Files Necessari

```cpp
// === UNIX/LINUX/MACOS ===
#include <iostream>       // cout, cin
#include <cstring>        // memset, strlen
#include <sys/socket.h>   // socket, bind, listen, accept, send, recv
#include <netinet/in.h>   // sockaddr_in, INADDR_ANY
#include <arpa/inet.h>    // inet_addr, htons, htonl
#include <unistd.h>       // close, read, write
#include <errno.h>        // errno per gestione errori
#include <cstdio>         // std::perror

using namespace std;
```

**⚠️ macOS Note**: Su macOS potresti anche necessitare:

```cpp
#include <netdb.h>        // gethostbyname, getaddrinfo
```

#### Tabella Comparativa Dettagliata

| Funzionalità          | Unix/Linux/macOS                     | Windows (WinSock)           |
| --------------------- | ------------------------------------ | --------------------------- |
| **Header principale** | `<sys/socket.h>`                     | `<winsock2.h>`              |
| **Inizializzazione**  | ❌ Non serve                         | `WSAStartup()`              |
| **Tipo socket**       | `int`                                | `SOCKET` (unsigned int)     |
| **Socket invalido**   | `-1`                                 | `INVALID_SOCKET`            |
| **Errore operazioni** | `-1`                                 | `SOCKET_ERROR`              |
| **Chiusura socket**   | `close(sockfd)`                      | `closesocket(socket)`       |
| **Cleanup**           | ❌ Non serve                         | `WSACleanup()`              |
| **Gestione errori**   | `errno` + `std::perror()`            | `WSAGetLastError()`         |
| **Compilazione**      | `g++ file.cpp -o exe`                | `g++ file.cpp -lws2_32`     |
| **Threading**         | `#include <pthread.h>` + `-lpthread` | `<windows.h>` + API Windows |

#### Compilazione su Unix/Linux/macOS

```bash
# === COMPILAZIONE BASE ===
g++ server.cpp -o server
g++ client.cpp -o client

# === CON STANDARD C++98 ===
g++ -std=c++98 server.cpp -o server

# === CON WARNING ===
g++ -Wall -Wextra -pedantic server.cpp -o server

# === CON THREADING (pthread) ===
g++ server.cpp -o server -lpthread

# === CON DEBUG SYMBOLS ===
g++ -g server.cpp -o server

# === OTTIMIZZAZIONE ===
g++ -O2 server.cpp -o server

# === TUTTO INSIEME ===
g++ -std=c++98 -Wall -Wextra -O2 server.cpp -o server -lpthread
```

**⚠️ IMPORTANTE**: Su Linux/macOS **NON serve** linkare librerie socket (già incluse nel sistema)

#### Esecuzione

```bash
# Avvia server
./server

# In un altro terminale, avvia client
./client

# Con valgrind (check memory leaks)
valgrind --leak-check=full ./server

# Background execution
./server &

# Con sudo (per porte < 1024)
sudo ./server
```

---

### 1C. Template Codice Unix/Linux/macOS

#### Server TCP Completo (Unix)

```cpp
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

using namespace std;

int main() {
    // ====================================
    // STEP 1: NON SERVE INIZIALIZZAZIONE
    // (diverso da Windows WSAStartup)
    // ====================================

    // ====================================
    // STEP 2: CREA SOCKET`
    // ====================================
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    //  ↑ int (non SOCKET come Windows)

    if (serverSocket < 0) {  // < 0 invece di == INVALID_SOCKET
        std::perror("Errore socket");  // std::perror invece di WSAGetLastError
        return 1;
    }

    cout << "Socket creato: " << serverSocket << endl;

    // ====================================
    // STEP 3: BIND
    // ====================================
    struct sockaddr_in serverAddr;  // struct esplicito su alcuni sistemi
    memset(&serverAddr, 0, sizeof(serverAddr));  // Inizializza a zero

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // Alternativa: INADDR_ANY per tutte le interfacce
    serverAddr.sin_port = htons(55555);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::perror("Errore bind");
        close(serverSocket);  // close invece di closesocket
        return 1;
    }

    cout << "Socket vincolato a 127.0.0.1:55555" << endl;

    // ====================================
    // STEP 4: LISTEN
    // ====================================
    if (listen(serverSocket, 5) < 0) {  // Backlog = 5 client
        std::perror("Errore listen");
        close(serverSocket);
        return 1;
    }

    cout << "Server in ascolto..." << endl;

    // ====================================
    // STEP 5: ACCEPT
    // ====================================
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    //        ↑ socklen_t (non int su tutti i sistemi)

    int clientSocket = accept(serverSocket,
                              (struct sockaddr*)&clientAddr,
                              &clientAddrLen);

    if (clientSocket < 0) {
        std::perror("Errore accept");
        close(serverSocket);
        return 1;
    }

    cout << "Client connesso!" << endl;

    // ====================================
    // STEP 6: RECV/SEND
    // ====================================
    char buffer[200];
    memset(buffer, 0, sizeof(buffer));

    ssize_t bytesRecv = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    //      ↑ ssize_t (signed size_t)

    if (bytesRecv < 0) {
        std::perror("Errore recv");
    } else if (bytesRecv == 0) {
        cout << "Client ha chiuso la connessione" << endl;
    } else {
        buffer[bytesRecv] = '\0';  // Null terminator
        cout << "Ricevuto: " << buffer << endl;

        // Risposta
        const char* risposta = "Messaggio ricevuto!";
        send(clientSocket, risposta, strlen(risposta), 0);
    }

    // ====================================
    // STEP 7: CLEANUP
    // ====================================
    close(clientSocket);  // close invece di closesocket
    close(serverSocket);
    // NO WSACleanup() su Unix!

    cout << "Server terminato" << endl;
    return 0;
}
```

#### Client TCP Completo (Unix)

```cpp
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main() {
    // STEP 1: (nessuna inizializzazione)

    // STEP 2: CREA SOCKET
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket < 0) {
        std::perror("Errore socket");
        return 1;
    }

    // STEP 3: CONNECT
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(55555);

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::perror("Errore connect");
        close(clientSocket);
        return 1;
    }

    cout << "Connesso al server!" << endl;

    // STEP 4: SEND/RECV
    char buffer[200];
    cout << "Inserisci messaggio: ";
    cin.getline(buffer, 200);

    send(clientSocket, buffer, strlen(buffer), 0);

    // Ricevi risposta
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytesRecv = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesRecv > 0) {
        buffer[bytesRecv] = '\0';
        cout << "Server: " << buffer << endl;
    }

    // STEP 5: CLEANUP
    close(clientSocket);

    return 0;
}
```

---

### 1D. Gestione Errori Unix/Linux/macOS

#### errno e std::perror()

```cpp
#include <errno.h>
#include <cstring>

// Metodo 1: std::perror (stampa a stderr)
if (bind(sockfd, ...) < 0) {
    std::perror("Errore bind");  // Output: "Errore bind: Address already in use"
}

// Metodo 2: strerror (restituisce stringa)
if (connect(sockfd, ...) < 0) {
    cout << "Errore: " << strerror(errno) << endl;
}

// Metodo 3: Controllo errno specifico
if (accept(sockfd, ...) < 0) {
    if (errno == EINTR) {
        cout << "Interrotto da segnale" << endl;
    } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
        cout << "Nessuna connessione disponibile (non-blocking)" << endl;
    } else {
        std::perror("Errore accept");
    }
}
```

#### Codici Errore Comuni

| errno | Costante       | Significato            | Causa Tipica                   |
| ----- | -------------- | ---------------------- | ------------------------------ |
| 98    | `EADDRINUSE`   | Address already in use | Porta già in uso               |
| 111   | `ECONNREFUSED` | Connection refused     | Server non in ascolto          |
| 104   | `ECONNRESET`   | Connection reset       | Connessione chiusa bruscamente |
| 32    | `EPIPE`        | Broken pipe            | Scrivi su socket chiuso        |
| 110   | `ETIMEDOUT`    | Connection timed out   | Timeout connessione            |
| 9     | `EBADF`        | Bad file descriptor    | Socket non valido              |

---

### 1E. Opzioni Socket Unix-Specific

#### SO_REUSEADDR (Importante!)

**Problema**: Dopo aver chiuso un server, la porta rimane "occupata" per ~60s (stato TIME_WAIT)

**Soluzione**:

```cpp
int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

// Permetti riuso immediato della porta
int opt = 1;
if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    std::perror("setsockopt SO_REUSEADDR");
}

// Ora bind() funzionerà anche se porta usata recentemente
bind(serverSocket, ...);
```

#### SO_REUSEPORT (Linux/macOS)

Permetti a più processi di ascoltare sulla stessa porta:

```cpp
int opt = 1;
setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
```

#### TCP_NODELAY (Disabilita Nagle Algorithm)

Utile per gaming (latenza bassa):

```cpp
#include <netinet/tcp.h>

int flag = 1;
setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
// Invia pacchetti immediatamente senza accumularli
```

---

### 1F. Differenze macOS-Specifiche

#### 1. MSG_NOSIGNAL Non Disponibile

**Linux**:

```cpp
send(socket, buffer, len, MSG_NOSIGNAL);  // ✅ Funziona
```

**macOS** (genera errore compilazione):

```cpp
// Soluzione: Disabilita SIGPIPE globalmente
#include <signal.h>

signal(SIGPIPE, SIG_IGN);  // Ignora SIGPIPE
// Ora send() su socket chiuso ritorna -1 invece di crashare
```

#### 2. socklen_t

**Sempre usa** `socklen_t` invece di `int` per dimensioni addr:

```cpp
socklen_t addrLen = sizeof(clientAddr);  // ✅ Portable
accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
```

---

### 1G. Conversione Codice Windows → Unix

#### Tabella di Conversione Rapida

```cpp
// ===== WINDOWS =====              // ===== UNIX =====

#include <winsock2.h>               #include <sys/socket.h>
                                    #include <netinet/in.h>
                                    #include <arpa/inet.h>
                                    #include <unistd.h>

WSADATA wsaData;                    // (rimuovi)
WSAStartup(MAKEWORD(2,2),&wsaData); // (rimuovi)

SOCKET serverSocket;                int serverSocket;

if (serverSocket == INVALID_SOCKET) if (serverSocket < 0)

if (result == SOCKET_ERROR)         if (result < 0)

WSAGetLastError()                   errno + std::perror() o strerror()

closesocket(serverSocket);          close(serverSocket);

WSACleanup();                       // (rimuovi)

#pragma comment(lib, "ws2_32.lib")  // (rimuovi, non serve)
```

#### Script di Conversione Automatica

```bash
#!/bin/bash
# convert_win_to_unix.sh

sed -i 's/#include <winsock2.h>/#include <sys\/socket.h>\n#include <netinet\/in.h>\n#include <arpa\/inet.h>\n#include <unistd.h>/g' $1
sed -i 's/SOCKET /int /g' $1
sed -i 's/INVALID_SOCKET/-1/g' $1
sed -i 's/SOCKET_ERROR/-1/g' $1
sed -i 's/closesocket/close/g' $1
sed -i '/WSAStartup/d' $1
sed -i '/WSACleanup/d' $1
sed -i '/WSADATA/d' $1
```

**Uso**: `./convert_win_to_unix.sh server.cpp`

---

### 1H. Introduzione a WinSock (Solo per Windows)

#### Concetti Base

-   **WinSock**: Windows Socket API (versione Windows dei socket Unix)
-   **WSA**: Windows Sockets API
-   **DLL Core**: `ws2_32.dll` (situata in `system32`)
-   **Scopo**: Gestisce connessione tra hardware e software a basso livello

#### Perché DLL?

-   **DLL** = Dynamic Link Library
-   Codice riutilizzabile da più applicazioni
-   Evita duplicazione del codice socket in ogni programma

**⚠️ NOTA PER UTENTI UNIX**: Le sezioni seguenti descrivono l'API Windows. Se programmi principalmente su Linux/macOS, concentrati sulla sezione 1B-1G.

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

#### Schema Completo OSI Model con Router

```
┌────────────────────────┐                                    ┌────────────────────────┐
│        SENDER          │                                    │       RECEIVER         │
├────────────────────────┤                                    ├────────────────────────┤
│                        │                                    │                        │
│  ┌─────────────────┐   │ ─ ─ ─ ─ Game Events─ ─ ─ ─ ─ ─ ─ ─ │  ┌─────────────────┐   │  Input Updates              |   |                                    |  |                 |   |
│  │  Application    │●─ ┼────────────────────────────────────┼─●│  Application    │   │  State Updates              |   |                                    |  |                 |   |
│  └────────┬────────┘   │                                    │  └────────┬────────┘   │
│           ↓            │                                    │           ↑            │
│  ┌─────────────────┐   │ ─ ─ ─ Game Packetization─ ─ ─ ─ ─ ─│  ┌─────────────────┐   │  Serialization        |   |                                    |  |                 |   |
│  │  Presentation   │●─ ┼────────────────────────────────────┼─●│  Presentation   │   │  Buffering
│  └────────┬────────┘   │                                    │  └────────┬────────┘   │
│           ↓            │                                    │           ↑            │
│  ┌─────────────────┐   │ ─ ─ Connection & Data Exchange ─ ─ │  ┌─────────────────┐   │  Sockets
│  │    Session      │●─ ┼────────────────────────────────────┼─●│    Session      │   │
│  └────────┬────────┘   │                                    │  └────────┬────────┘   │
│           ↓            │                                    │           ↑            │
│  ┌─────────────────┐   │                                    │  ┌─────────────────┐   │  TCP
│  │   Transport     │   │                                    │  │   Transport     │   │  UDP
│  └────────┬────────┘   │                                    │  └────────┬────────┘   │
│           ↓            │          ┌──────────┐              │           ↑            │
│  ┌─────────────────┐   │          │  ROUTER  │              │  ┌─────────────────┐   │
│  │    Network      │   │   ┌──────┤          ├──────┐       │  │    Network      │   │  IP
│  └────────┬────────┘   │   │      │ Network  │      │       │  └────────┬────────┘   │
│           ↓            │   │      └────┬─────┘      │       │           ↑            │
│  ┌─────────────────┐   │   │           ↓            │       │  ┌─────────────────┐   │
│  │   Data Link     │───┼───┤    ┌──────────────┐    ├───────┼──│   Data Link     │   │  Ethernet (MAC)                │   │   │    │              │    │       │  │                 │   │
│  └────────┬────────┘   │   │    │   Data Link  │    │       │  └────────┬────────┘   │
│           ↓            │   │    └──────┬───────┘    │       │           ↑            │
│  ┌─────────────────┐   │   │           ↓            │       │  ┌─────────────────┐   │  Wired (C5, Cable)          │   │   │    ┌──────────────┐    │       │  |                 |   |
│  │    Physical     │───┼───┘    │              │    └───────┼──│    Physical     │   │  Fiber Optics               │   │        │   Physical   │            │  |                 |   |
│  └─────────────────┘   │        └──────────────┘            │  └─────────────────┘   │  Wireless
│                        │                                    │                        │
└────────────────────────┘                                    └────────────────────────┘
```

#### Flusso di Dati (Descrizione Testuale)

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

### ⚠️ NOTA IMPORTANTE: Unix vs Windows

**Gli esempi seguenti usano sintassi Windows (WinSock).**

**Se programmi su Unix/Linux/macOS**, fai riferimento alla **Sezione 1B-1G** per:

-   ✅ Template completi Unix
-   ✅ Header corretti (`<sys/socket.h>`, `<unistd.h>`)
-   ✅ Gestione errori con `errno` e `std::perror()`
-   ✅ Uso di `int` invece di `SOCKET`
-   ✅ `close()` invece di `closesocket()`

---

### Server TCP Minimo (Windows/WinSock)

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

_Buono studio! 🚀_
