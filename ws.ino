const char *wsHost = "myanee.herokuapp.com", *wsPath = "/ws";

WebSocketsClient ws;

bool setupWS()
{
  // server address, port and URL
//  ws.begin(wsHost, 80, wsPath); //TODO get ssl working
  ws.beginSSL(wsHost, 443, wsPath);

  // event handler
  ws.onEvent(handleWsEvent);

  // try ever 5000 again if connection has failed
  ws.setReconnectInterval(10000);

  return 0;
}

void initWSConnection() {
  StaticJsonDocument<64> doc;
  doc["command"] = "SUBSCRIBE_SEND";
  doc["device"] = "test_device";
  serializeJson(doc, jsonBuf, sizeof(jsonBuf));
  sendBuf();
}

void sendBuf(){
  ws.sendTXT(jsonBuf);
}

void handleWsEvent(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED:
      Serial.printf("[WSc] Connected to url: %s\n", payload);
      initWSConnection();
      break;
    case WStype_TEXT:
      if (handleMessage((char*)payload, length)) {
        //TODO handle error
      }
      break;
    case WStype_BIN:
      //ignore
      break;
  }
}

bool handleMessage(char *payload, size_t length) {
  StaticJsonDocument<128> doc;
  DeserializationError err = deserializeJson(doc, payload, length);
  if (err) {
    Serial.println(err.c_str());
    return true;
  }
  JsonObject root = doc.as<JsonObject>();
  const char* command = root["command"];
  if (strcmp(command, "SENT") == 0) {
    handleCommand(root["data"]);
  } else if (strcmp(command, "ERROR") == 0) {
    Serial.println("Server sent error:");
    Serial.println(root["error"].as<const char*>());
    return true;
  }
  return false;
}

void handleWS() {
  ws.loop();
}
