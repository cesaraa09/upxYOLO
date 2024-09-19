#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* serverAddress = "https://testesemaforo.onrender.com/obter_rlista/";

void setup() {
  pinMode(32, OUTPUT);
  digitalWrite(32, LOW);
  pinMode(33, OUTPUT);
  digitalWrite(33, LOW);
  Serial.begin(115200);

  WiFi.begin("NOME_DA_REDE", "_SENHA_");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao Wi-Fi...");
  }
  Serial.println("Conectado ao Wi-Fi");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    Serial.println("Esperando GET request...");
    if (http.begin(serverAddress)) {
      int httpResponseCode = http.GET();  
      Serial.print("HTTP Response Code: ");
      Serial.println(httpResponseCode);
      
      if (httpResponseCode == 200) {
        String listaRecebida = http.getString();
        Serial.println(listaRecebida);
        
        
        const size_t tam = JSON_OBJECT_SIZE(1) + 2 * JSON_OBJECT_SIZE(2) + 100;
        StaticJsonDocument<tam> listaFinal;
        DeserializationError error = deserializeJson(listaFinal, listaRecebida);

        if (error) {
          Serial.print("Falha ao analisar JSON: ");
          Serial.println(error.c_str());
          return;
        }

        if (listaFinal.containsKey("rlista")) {
        JsonObject rlista = listaFinal["rlista"].as<JsonObject>();
        
        JsonArray lista1 = rlista["lista1"].as<JsonArray>();
        JsonArray lista2 = rlista["lista2"].as<JsonArray>();

        if (lista1.size() > 0 && lista2.size() > 0) {
          int numElementosLista1 = lista1[0].as<int>();
          int numElementosLista2 = lista2[0].as<int>();
          
          Serial.print("Número de veículos na Rua 1: ");
          Serial.println(numElementosLista1);
          Serial.print("Número de veículos na Rua 2: ");
          Serial.println(numElementosLista2);

          if (numElementosLista1 > numElementosLista2) {
            digitalWrite(32, HIGH);
            digitalWrite(33, LOW);
          } else {
            digitalWrite(33, HIGH);
            digitalWrite(32, LOW);
          }
        } else {
          Serial.println("Listas vazias ou sem elementos.");
        }
      } else {
        Serial.println("Campo 'rlista' não encontrado no JSON.");
      }
      } else {
        Serial.print("Falha na solicitação HTTP, código de resposta: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    } else {
      Serial.println("Falha na conexão ao servidor");
    }
  } else {
    Serial.println("Desconectado do Wi-Fi. Reconectando...");
    WiFi.reconnect();
  }
}


