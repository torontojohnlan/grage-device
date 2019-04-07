WiFiManager wifiManager;

bool setupWifi()
{
  wifiManager.autoConnect(devName);
  return 0;
}
