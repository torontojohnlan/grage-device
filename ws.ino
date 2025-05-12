// main script where it maintains websocket session
// A controller acts as passive client (mostly), it is never the initiating party to send type "data" to server except when there is a pin status change
// (it does, though, initiating  socket housekeeping control types)
// This controller as a client of server side ws.ts only process 
//            1. type="data" messages. 
//                    There is sub-command in the "data" structure, including "digitalWrite", "pinMode", "attachInterrupt" etc.
//                    sub-command is handled in io.ino
//            2. type="rping" messages. 
//                    "rping" stands for request ping. It's basically another client on same channel asking current reading
//                    of this device. We simply call readAndSend() here, which reads pin statuss and send back to channel
//            3. Although this script also include "error" type, but ws.ts and app.ts from server side never sends this type
const char *wsPath = "/ws";

WebSocketsClient ws;

bool setupWS()
{
  // server address, port and URL
  //ws.begin(config.wsHost, 80, wsPath); //TODO get ssl working
  ws.beginSSL(config.wsHost, 443, wsPath);

  // event handler
  ws.onEvent(handleWsEvent);

  // try ever 5000 again if connection has failed
  ws.setReconnectInterval(10000);

  return 0;
}

const long refreshRate = 60 * 1000;
long lastConnection = 0;
int reconnectTries = 0, maxRetries = 3;
bool wsConnected = false;

void refreshConnection() {
  if (millis() - lastConnection > refreshRate) {
    connectChannel();
  }
}

void connectChannel() {
  StaticJsonDocument<128> doc;
  doc["type"] = "connect";
  doc["id"] = config.deviceID;
  doc["fromDevice"]=true;
  serializeJson(doc, jsonBuf, sizeof(jsonBuf));
  sendBuf();
  lastConnection = millis();
}

void initWSConnection() { // This should really be called "initChannelConnection" as this is not to initialize WS connection. Instead, it finds correct channel on server to connect to with device ID
  connectChannel();
}

void sendBuf() {
  ws.sendTXT(jsonBuf);
}

void handleWsEvent(WStype_t type, uint8_t *payload, size_t length)
{ // handles different message types that are received from channel (from server)
  switch (type)
  {
    case WStype_DISCONNECTED:  //comment out whole "case disconnected" section so instead of calling wifi setup page, it will just try to reconnect to server
//      wsConnected = false;
//      Serial.printf("[WSc] Disconnected! Retry: %d\n", reconnectTries);
//      reconnectTries++;
//      if(reconnectTries > maxRetries)
//      {
//        Serial.printf("[WSc] Max Retries reached, open config panel!\n"); 
//        if(setupWifi(false)){
//          Serial.printf("Wifi config failed, restart\n"); 
//          ESP.restart();
//        }
//        else
//        {
//          Serial.printf("Wifi config succedded\n"); 
//          reconnectTries = 0;
//        }
//      }
      break;
    case WStype_CONNECTED:
      wsConnected = true;
      Serial.printf("[WSc] Connected to url: %s\n", payload);
      reconnectTries = 0;
      initWSConnection(); // pick a correct server channel to connect to. 
      break;
    case WStype_TEXT:
      reconnectTries = 0;
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
  StaticJsonDocument<256> doc;
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
  if (wsConnected)
    refreshConnection();
  ws.loop();
}
