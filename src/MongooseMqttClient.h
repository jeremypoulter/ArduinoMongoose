#ifndef MongooseMqttClient_h
#define MongooseMqttClient_h

#include "MongoosePlatform.h"

#include <mongoose.h>

#include <functional>

#include "MongooseString.h"
#include "MongooseSocket.h"

 /**
  * @brief MQTT Client wrapper
  */ 
class MongooseMqttClient;

typedef std::function<void()> MongooseMqttConnectionHandler;
typedef std::function<void(const MongooseString topic, const MongooseString payload)> MongooseMqttMessageHandler;
typedef std::function<void()> MongooseMqttDisconnectHandler;
typedef enum {
  MQTT_MQTT = 0,
  MQTT_MQTTS,
  MQTT_WS,  // TODO
  MQTT_WSS  // TODO
} MongooseMqttProtocol;

 /**
  * @brief MQTT Client wrapper
  */ 
class MongooseMqttClient : public MongooseSocket
{
  private:
    MongooseString _client_id;
    MongooseString _username;
    MongooseString _password;
    MongooseString _will_topic;
    MongooseString _will_message;
    bool _will_retain;
    bool _connected;

    MongooseMqttConnectionHandler _onConnect;
    MongooseMqttMessageHandler _onMessage;
    MongooseMqttDisconnectHandler _onDisconnect;

  protected:
    void onClose(mg_connection *nc);
    void handleEvent(mg_connection *nc, int ev, void *p);

  public:
    MongooseMqttClient();
    ~MongooseMqttClient();

    bool connect(const char *server, const char *client_id) {
      return connect(MQTT_MQTT, server, client_id, _onConnect);
    }
    bool connect(const char *server, const char *client_id, MongooseMqttConnectionHandler onConnect) {
      return connect(MQTT_MQTT, server, client_id, onConnect);
    }
    bool connect(MongooseMqttProtocol protocol, const char *server, const char *client_id) {
      return connect(protocol, server, client_id, _onConnect);
    }
    bool connect(MongooseMqttProtocol protocol, const char *server, const char *client_id, MongooseMqttConnectionHandler onConnect);

    void setCredentials(const char *username, const char *password) {
      _username = username;
      _password = password;
    }

    void setCertificate(const char *cert, const char *key) {
      MongooseSocket::setCertificate(cert, key);
    }

    void setRejectUnauthorized(bool reject) {
      MongooseSocket::setRejectUnauthorized(reject);
    }

    void setLastWillAndTestimment(const char *topic, const char *message, bool retain = false) {
      _will_topic = topic;
      _will_message = message;
      _will_retain = retain;
    }

    // Correct-spelling alias; setLastWillAndTestimment kept for backward compatibility
    void setLastWillAndTestament(const char *topic, const char *message, bool retain = false) {
      setLastWillAndTestimment(topic, message, retain);
    }

#ifdef ARDUINO
    bool connect(String &server, String &client_id) {
      return connect(MQTT_MQTT, server.c_str(), client_id.c_str(), _onConnect);
    }
    bool connect(String &server, String &client_id, MongooseMqttConnectionHandler onConnect) {
      return connect(MQTT_MQTT, server.c_str(), client_id.c_str(), onConnect);
    }
    bool connect(MongooseMqttProtocol protocol, String &server, String &client_id) {
      return connect(protocol, server.c_str(), client_id.c_str(), _onConnect);
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
    void setLastWillAndTestament(String &topic, String &message, bool retain = false) {
      setLastWillAndTestimment(topic.c_str(), message.c_str(), retain);
    }
#endif

    bool disconnect();

    bool connected() {
      return MongooseSocket::connected() &&  _connected;
    }

    MongooseMqttClient *onConnect(MongooseMqttConnectionHandler fnHandler) {
      _onConnect = fnHandler;
      return this;
    }

    MongooseMqttClient *onMessage(MongooseMqttMessageHandler fnHandler) {
      _onMessage = fnHandler;
      return this;
    }

    MongooseMqttClient *onError(MongooseSocketErrorHandler fnHandler) {
      MongooseSocket::onError(fnHandler);
      return this;
    }

    MongooseMqttClient *onClose(MongooseSocketCloseHandler fnHandler) {
      MongooseSocket::onClose(fnHandler);
      return this;
    }

    void onDisconnect(MongooseMqttDisconnectHandler fnHandler) {
      _onDisconnect = fnHandler;
    }

    bool subscribe(const char *topic, int qos = 0);
#ifdef ARDUINO
    bool subscribe(String &topic, int qos = 0) {
      return subscribe(topic.c_str(), qos);
    }
#endif

    bool publish(const char *topic, const char *payload, bool retain = false, int qos=0) {
      return publish(topic, mg_str_s(payload), retain, qos);
    }
    bool publish(const char *topic, MongooseString payload, bool retain = false, int qos=0) {
      return publish(topic, payload.toMgStr(), retain, qos);
    }
    bool publish(const char *topic, mg_str payload, bool retain = false, int qos=0);
#ifdef ARDUINO
    bool publish(String &topic, const char *payload, bool retain = false, int qos=0) {
      return publish(topic.c_str(), mg_str_s(payload), retain, qos);
    }
    bool publish(String &topic, String &payload, bool retain = false, int qos=0) {
      return publish(topic.c_str(), mg_str_s(payload.c_str()), retain, qos);
    }
    bool publish(const char *topic, String &payload, bool retain = false, int qos=0) {
      return publish(topic, mg_str_s(payload.c_str()), retain, qos);
    }
#endif
};

#endif /* MongooseMqttClient_h */
