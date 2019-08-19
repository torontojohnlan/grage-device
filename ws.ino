const char *wsHost = "grage.herokuapp.com", *wsPath = "/ws";
const long refreshRate = 60 * 1000;

WebSocketsClient ws;

bool setupWS()
{
  // server address, port and URL
  //ws.begin(wsHost, 80, wsPath); //TODO get ssl working
    ws.beginSSL(wsHost, 443, wsPath);

  // event handler
  ws.onEvent(handleWsEvent);

  // try ever 5000 again if connection has failed
  ws.setReconnectInterval(10000);

  return 0;
}

bool wsConnected = false;

void connectChannel() {
  StaticJsonDocument<128> doc;
  doc["type"] = "connect";
  doc["id"] = deviceID;
  doc["fromDevice"]=true;
  serializeJson(doc, jsonBuf, sizeof(jsonBuf));
  sendBuf();
}

void initWSConnection() {
  connectChannel();
}

void sendBuf() {
  ws.sendTXT(jsonBuf);
}

void handleWsEvent(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
    case WStype_DISCONNECTED:
      wsConnected = false;
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED:
      wsConnected = true;
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
  Serial.println(payload);
  DeserializationError err = deserializeJson(doc, payload, length);
  if (err) {
    Serial.println(err.c_str());
    return true;
  }
  JsonObject root = doc.as<JsonObject>();
  const char* command = root["type"];
  if (strcmp(command, "data") == 0) {
    handleCommand(root["data"]);
  } else if (strcmp(command, "rping") == 0) {
    return readAndSend();
  } else if (strcmp(command, "error") == 0) {
    Serial.println("Server sent error:");
    Serial.println(root["error"].as<const char*>());
    return true;
  }
  return false;
}

void handleWS() {
  ws.loop();
}
