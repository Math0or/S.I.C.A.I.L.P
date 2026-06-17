#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid     = "NOME_DA_REDE";
const char* password = "SENHA_DA_REDE";

const char* serverUrl = "http://SEU_SERVIDOR:3000/api/chamada";

const int botaoPin = 2;   
const int ledPin   = 4;   


const unsigned long tempoLongo        = 1500; 
const unsigned long intervaloMultiplo = 600;  
const int cliquesParaUrgente          = 3;   

unsigned long tempoPressao = 0;
unsigned long ultimoClique = 0;
int contadorCliques = 0;
bool botaoEstavaPressionado = false;

void setup() {
  Serial.begin(115200);
  pinMode(botaoPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  conectarWiFi();
}

void loop() {
  bool botaoPressionado = (digitalRead(botaoPin) == LOW);

  
  if (botaoPressionado && !botaoEstavaPressionado) {
    tempoPressao = millis();
  }

 
  if (!botaoPressionado && botaoEstavaPressionado) {
    unsigned long duracao = millis() - tempoPressao;

    if (duracao >= tempoLongo) {
      enviarAlerta("URGENTE");
      contadorCliques = 0;
    } else {
      unsigned long agora = millis();
      if (agora - ultimoClique <= intervaloMultiplo) {
        contadorCliques++;
      } else {
        contadorCliques = 1;
      }
      ultimoClique = agora;

      if (contadorCliques >= cliquesParaUrgente) {
        enviarAlerta("URGENTE");
        contadorCliques = 0;
      } else {
        enviarAlerta("SUPORTE");
      }
    }
  }

  botaoEstavaPressionado = botaoPressionado;
  delay(20);

void conectarWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void enviarAlerta(String tipo) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"tipo\":\"" + tipo + "\",\"dispositivo\":\"quarto01\"}";
    int httpCode = http.POST(payload);

    Serial.print("Alerta enviado [");
    Serial.print(tipo);
    Serial.print("] - Resposta HTTP: ");
    Serial.println(httpCode);

    if (tipo == "URGENTE") {
      for (int i = 0; i < 6; i++) {
        digitalWrite(ledPin, HIGH); delay(100);
        digitalWrite(ledPin, LOW);  delay(100);
      }
    } else {
      digitalWrite(ledPin, HIGH); delay(500);
      digitalWrite(ledPin, LOW);
    }

    http.end();
  } else {
    Serial.println("Erro: sem conexao Wi-Fi.");
  }
}
