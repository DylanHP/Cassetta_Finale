/*
 *  Foglio per gli script della pagina web 
 */

// Definizione delle variabili
var distanza = { slider: document.getElementById("mySliderDistanza"), output: document.getElementById("sliderValueDistanza") };
var secondi = { slider: document.getElementById("mySliderSecondi"), output: document.getElementById("sliderValueSecondi") };

// Funzione di aggiornamento dello slider
function updateSliderValue(item) { item.output.value = item.slider.value; }

// Aggiornamento dei valori contenuti negli slider
updateSliderValue(distanza);
updateSliderValue(secondi);

// Aggiunta dei due eventi listener sul movimento dello slider
distanza.slider.addEventListener("input", () => { updateSliderValue(distanza); });
secondi.slider.addEventListener("input", () => { updateSliderValue(secondi); });

var inputTimeoutDistanza;
var inputTimeoutSecondi;

// Funzione di controllo della correttezza del valore inserito nel campo di testo
function isValidInput(value, min, max) {
    var numValue = parseInt(value);
    return !isNaN(numValue) && numValue >= min && numValue <= max;
}

// Valore di distanza di misurazione che invio al micro
distanza.output.addEventListener("input", function () {
    clearTimeout(inputTimeoutDistanza);
    inputTimeoutDistanza = setTimeout(() => {
        var inputValue = this.value;
        var min = parseInt(this.min);
        var max = parseInt(this.max);
        // controllo la validità del dato inserito se corretto allora lo invio al micro
        if (isValidInput(inputValue, min, max)) {
            distanza.slider.value = inputValue;
        }
        // se è sbagliato invece me lo metto al massimo o al minimo aseconda di chi è più vicino 
        else {
            this.value = Math.max(min, Math.min(parseInt(inputValue), max));
            distanza.slider.value = this.value;
        }
    }, 1000);
});

// Valore di tempo di azionamento che invio al micro
secondi.output.addEventListener("input", function () {
    clearTimeout(inputTimeoutSecondi);
    inputTimeoutSecondi = setTimeout(() => {
        var inputValue = this.value;
        var min = parseInt(this.min);
        var max = parseInt(this.max);
        if (isValidInput(inputValue, min, max)) {
            secondi.slider.value = inputValue;
        } else {
            this.value = Math.max(min, Math.min(parseInt(inputValue), max));
            secondi.slider.value = this.value;
        }
    }, 1000);
});

// variabili per il websocket
var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

// Aggiunta dell'event listener sul caricamento della pagina
window.addEventListener('load', onLoad);

// Funzione di init del websocket
function initWebSocket() {
    console.log('Sto provando ad aprire una connessione Websocket...');
    websocket = new WebSocket(gateway);
    //definisco cosa deve fare all'apertura chisura e arrivo del messaggio
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

// Funzione eseguita sull'apertura 
function onOpen(event) {
    console.log('Connessione aperta');
}

// Funzione eseguita sulla chiusura  
function onClose(event) {
    console.log('Connessione chiusa');
    setTimeout(initWebSocket, 2000);
}

// Funzione eseguita sull'apertura 
function onLoad(event) {
    initWebSocket();
    initButton();
}

// Funzione per l'inizializzazione dei bottoni
function initButton() {
    // bottone di invio
    document.getElementById('submitButton').addEventListener('click', () => {
        var json = JSON.stringify({ distanza: distanza.slider.value, secondi: secondi.slider.value });
        var messageContainer = document.getElementById('messageContainer');
        var resetContainer = document.getElementById('resetContainer');
        clearTimeout(messageContainer.timeout); // Pulisco il timer
        resetContainer.style.display = 'none';
        messageContainer.style.display = 'block';
        console.log(json);
        websocket.send(json);
        messageContainer.timeout = setTimeout(() => { messageContainer.style.display = 'none'; }, 5000);
    });

    // bottone di reset
    document.getElementById('resetButton').addEventListener('click', () => {
        var json = JSON.stringify({ distanza: 75, secondi: 5 });
        var messageContainer = document.getElementById('messageContainer');
        var resetContainer = document.getElementById('resetContainer');
        clearTimeout(resetContainer.timeout); // Pulisco il timer
        messageContainer.style.display = 'none';
        resetContainer.style.display = 'block';
        distanza.slider.value = 75; // Aggiorno la distanza sullo slider
        distanza.output.value = 75; // Aggiorno la distanza sull'output
        secondi.slider.value = 5;   // Aggiorno i secondi di attivazione sullo slider
        secondi.output.value = 5;   // Aggiorno i secondi di attivazione sull'output
        console.log(json);
        websocket.send(json);
        resetContainer.timeout = setTimeout(() => { resetContainer.style.display = 'none'; }, 5000);
    });
}

// Variabile booleana per sapere se il messaggio è arrivato
var arrivato = false;

// Funzione eseguita all'arrivo di un messaggio
function onMessage(event) {
    console.log('Message received:', event.data);
    var data = JSON.parse(event.data);
    // Quando il messaggio arriva aggiorno i valori
    if (!arrivato) {
        arrivato = true;
        distanza.slider.value = data.distanza;
        distanza.output.value = data.distanza;
        secondi.slider.value = data.secondi;
        secondi.output.value = data.secondi;
    }

    // I dati inviati dall'esp su Websocket sono convertiti in testo e viene aggiunta anche l'unità di misura
    if (data.liveMeasurement !== undefined) {
        document.getElementById('liveMeasurement').innerText = data.liveMeasurement + " cm";
    }
}