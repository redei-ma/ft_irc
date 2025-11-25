#!/bin/bash

# Configurazione
HOST="127.0.0.1"
PORT="6666"
DELAY="0.05" # Ritardo tra una connessione e l'altra (secondi)

echo "--- Inizio Stress Test su $HOST:$PORT ---"
echo "Premi CTRL+C per fermare."

count=0

while true; do
    # Genera un numero casuale per il nickname per evitare conflitti
    NICK="StressBot$RANDOM"

    # 1. echo invia i comandi IRC (NICK e USER).
    # 2. il pipe (|) li passa a netcat (nc).
    # 3. netcat si connette, spedisce i dati e SI CHIUDE subito dopo (perché l'echo finisce).
    # 4. la & commerciale alla fine manda il processo in background (non blocca il loop).
    
    echo -e "CAP LS\r\nNICK $NICK\r\nUSER $NICK 0 * :Stress Test\r\n" | nc $HOST $PORT > /dev/null 2>&1 &

    ((count++))

    # Ogni 100 connessioni stampa un avviso
    if (( count % 100 == 0 )); then
        echo "Lanciate $count connessioni..."
    fi

    # Un piccolo ritardo per non far esplodere IL TUO computer (il server deve soffrire, non il client)
    sleep $DELAY
done