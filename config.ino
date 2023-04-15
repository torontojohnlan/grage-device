#include <FS.h>

struct Config{
  char wsHost[256];
  char deviceName[32];
  char deviceID[64];
} config;

bool setupConfg()
{
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) 
  {
    Serial.println("mounted file system");
  }
  else
  {
    Serial.println("failed to mount file system");
    return 1;
  }

  if(loadConfig() || strlen(config.deviceID)==0)
  {
    Serial.println("failed to load config file, load defaults");
    setConfigDefaults();
  }

  Serial.print("before config wsHost = "); Serial.println(config.wsHost);
  Serial.print("before config deviceName = "); Serial.println(config.deviceName);
  Serial.print("before config deviceID = "); Serial.println(config.deviceID);
  
  return 0;
}

const char *configFilePath = "/config.bin";

bool loadConfig()
{
  if (SPIFFS.exists(configFilePath)) 
  {
    //file exists, reading and loading
    Serial.println("reading config file");
    File configFile = SPIFFS.open(configFilePath, "r");
    
    if (configFile) 
    {
      Serial.println("opened config file");
      configFile.readBytes((char*)&config, sizeof(config));
      
      Serial.println("read config file");
      configFile.close();
      return 0;
    }
  }

  return 1;
}

char randomChar(char st, char ed)
{
  byte rng = random(0, ed-st);
  return rng + st;
}

void genRandomID(char st, char ed, int len)
{
  for(int i=0;i<len;i++)
  {
    config.deviceID[i] = randomChar(st, ed);
  }
  config.deviceID[len]='\0';
}

void setConfigDefaults()
{
  strcpy(config.wsHost, "server domain here");
  strcpy(config.deviceName, "grage");
  genRandomID('a','z', 32);
}

bool saveConfig()
{
    File configFile = SPIFFS.open(configFilePath, "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
      return 1;
    }

    Serial.println("wroting to config file");
    configFile.write((char*)&config, sizeof(config));
    
    Serial.println("wrote config file");
    configFile.close();
    return 0;
}
