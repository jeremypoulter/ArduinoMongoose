#ifndef MongooseMqttClient_h
#define MongooseMqttClient_h

#ifdef ARDUINO
#include "Arduino.h"
#endif

#include <mongoose.h>

#include <functional>

#include "MongooseString.h"
#include "MongooseSocket.h"

class MongooseMqttClient;

typedef std::function<void()> MongooseMqttConnectionHandler;
typedef std::function<void(const MongooseString topic, const MongooseString payload)> MongooseMqttMessageHandler;
typedef enum {
  MQTT_MQTT = 0,
  MQTT_MQTTS,
  MQTT_WS,  // TODO
  MQTT_WSS  // TODO
} MongooseMqttProtocol;

class MongooseMqttClient : public MongooseSocket
{
  private:
    const char *_client_id;
    const char *_username;
    const char *_password;
    const char *_cert;
    const char *_key;
    const char *_will_topic;
    const char *_will_message;
    bool _will_retain;
    bool _connected;
    bool _reject_unauthorized; 

    MongooseMqttConnectionHandler _onConnect;
    MongooseMqttMessageHandler _onMessage;

  protected:
    void onConnect();
    void onClose();
    void onEvent(int ev, void *p);

  public:
    MongooseMqttClient();
    ~MongooseMqttClient();

    bool connect(const char *server, const char *client_id, MongooseMqttConnectionHandler onConnect) {
      return connect(MQTT_MQTT, server, client_id, onConnect);
    }
    bool connect(MongooseMqttProtocol protocol, const char *server, const char *client_id, MongooseMqttConnectionHandler onConnect);

    void setCredentials(const char *username, const char *password) {
      _username = username;
      _password = password;
    }

    void setCertificate(const char *cert, const char *key) {
      _cert = cert;
      _key = key;
    }

    void setLastWillAndTestimment(const char *topic, const char *message, bool retain = false) {
      _will_topic = topic;
      _will_message = message;
      _will_retain = retain;
    }

    void setRejectUnauthorized(bool reject) {
      _reject_unauthorized = reject;
    }

#ifdef ARDUINO
    bool connect(String &server, String &client_id, MongooseMqttConnectionHandler onConnect) {
      return connect(MQTT_MQTT, server.c_str(), client_id.c_str(), onConnect);
    }
    bool connect(MongooseMqttProtocol protocol, String &server, String &client_id, MongooseMqttConnectionHandler onConnect) {
      return connect(protocol, server.c_str(), client_id.c_str(), onConnect);
    }
    void setCredentials(String &username, String &password) {
      setCredentials(username.c_str(), password.c_str());
    }
    void setLastWillAndTestimment(String &topic, String &message, bool retain = false) {
      setLastWillAndTestimment(topic.c_str(), message.c_str(), retain);
    }
#endif

    bool disconnect();

    bool connected() {
      return MongooseSocket::connected() &&  _connected;
    }

    void onMessage(MongooseMqttMessageHandler fnHandler) {
      _onMessage = fnHandler;
    }

    bool subscribe(const char *topic);
#ifdef ARDUINO
    bool subscribe(String &topic) {
      return subscribe(topic.c_str());
    }
#endif

    bool publish(const char *topic, const char *payload, bool retain = false, int qos=MG_MQTT_QOS(0)) {
      return publish(topic, mg_mk_str(payload), retain, qos);
    }
    bool publish(const char *topic, MongooseString payload, bool retain = false, int qos=MG_MQTT_QOS(0)) {
      return publish(topic, payload.toMgStr(), retain, qos);
    }
    bool publish(const char *topic, mg_str payload, bool retain = false, int qos=MG_MQTT_QOS(0));
#ifdef ARDUINO
    bool publish(String &topic, const char *payload, bool retain = false, int qos=MG_MQTT_QOS(0)) {
      return publish(topic.c_str(), mg_mk_str(payload), retain, qos);
    }
    bool publish(String &topic, String &payload, bool retain = false, int qos=MG_MQTT_QOS(0)) {
      return publish(topic.c_str(), mg_mk_str(payload.c_str()), retain, qos);
    }
    bool publish(const char *topic, String &payload, bool retain = false, int qos=MG_MQTT_QOS(0)) {
      return publish(topic, mg_mk_str(payload.c_str()), retain, qos);
    }
#endif
};

#endif /* MongooseMqttClient_h */
