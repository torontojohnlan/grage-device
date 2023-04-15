bool setupWifi(bool autoConnect)
{
  WiFiManager wifiManager;

  WiFiManagerParameter wsHost("wsHost", "Server Domain", config.wsHost, sizeof(config.wsHost));
  WiFiManagerParameter deviceName("deviceName", "Device Name", config.deviceName, sizeof(config.deviceName));
  WiFiManagerParameter deviceID("deviceID", "Device ID", config.deviceID, sizeof(config.deviceID));

  wifiManager.addParameter(&wsHost);
  wifiManager.addParameter(&deviceName);
  wifiManager.addParameter(&deviceID);

  Serial.print("trigger wifimanager autoConnect = "); Serial.println(autoConnect);
  if(autoConnect)
    wifiManager.autoConnect(config.deviceName);
  else
    wifiManager.startConfigPortal(config.deviceName);

  strcpy(config.wsHost, wsHost.getValue());
  strcpy(config.deviceName, deviceName.getValue());
  strcpy(config.deviceID, deviceID.getValue());

  Serial.print("after config wsHost = "); Serial.println(config.wsHost);
  Serial.print("after config deviceName = "); Serial.println(config.deviceName);
  Serial.print("after config deviceID = "); Serial.println(config.deviceID);
  
  if(saveConfig())
  {
    Serial.println("failed to save config file");
    return 1;
  }
  
  return 0;
}
