#if defined(ENABLE_DEBUG) && !defined(ENABLE_DEBUG_MONGOOSE_MQTT_CLIENT)
#undef ENABLE_DEBUG
#endif

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <MicroDebug.h>

#include "MongooseCore.h"
#include "MongooseMqttClient.h"

MongooseMqttClient::MongooseMqttClient() :
  MongooseSocket(),
  _client_id(NULL),
  _username(NULL),
  _password(NULL),
  _cert(NULL),
  _key(NULL),
  _will_topic(NULL),
  _will_message(NULL),
  _will_retain(false),
  _connected(false),
  _reject_unauthorized(true),
  _onConnect(NULL),
  _onMessage(NULL)
{

}

MongooseMqttClient::~MongooseMqttClient()
{

}

void MongooseMqttClient::onConnect(mg_connection *nc)
{
  struct mg_send_mqtt_handshake_opts opts;
  memset(&opts, 0, sizeof(opts));
  opts.user_name = _username;
  opts.password = _password;
  opts.will_topic = _will_topic;
  opts.will_message = _will_message;
  
  if(_will_retain) {
    opts.flags |= MG_MQTT_WILL_RETAIN;
  }
  
  DBUGVAR(_client_id);
  DBUGVAR(opts.user_name);
  DBUGVAR(opts.password);
  DBUGVAR(opts.will_topic);
  DBUGVAR(opts.will_message);
  DBUGVAR(opts.flags);

  mg_set_protocol_mqtt(getConnection());
  mg_send_mqtt_handshake_opt(getConnection(), _client_id, opts);
}

void MongooseMqttClient::onClose(mg_connection *nc)
{
  _connected = false;
  MongooseSocket::onClose(nc);
}

void MongooseMqttClient::onEvent(mg_connection *nc, int ev, void *p)
{
  struct mg_mqtt_message *msg = (struct mg_mqtt_message *) p;
  switch (ev) 
  {
    case MG_EV_MQTT_CONNACK:
      if (MG_EV_MQTT_CONNACK_ACCEPTED == msg->connack_ret_code) {
        _connected = true;
        if(_onConnect) {
          _onConnect();
        }
      } else {
        DBUGF("Got mqtt connection error: %d", msg->connack_ret_code);
        onError(nc, msg->connack_ret_code);
      }
      break;

    case MG_EV_MQTT_PUBACK:
      DBUGF("Message publishing acknowledged (msg_id: %d)", msg->message_id);
      break;

    case MG_EV_MQTT_SUBACK:
      DBUGF("Subscription acknowledged");
      break;

    case MG_EV_MQTT_PUBLISH: {
      DBUGF("Got incoming message %.*s: %.*s", (int) msg->topic.len,
             msg->topic.p, (int) msg->payload.len, msg->payload.p);
      if(_onMessage) {
        _onMessage(MongooseString(msg->topic), MongooseString(msg->payload));
      }
      break;
    }
  }
}

bool MongooseMqttClient::connect(MongooseMqttProtocol protocol, const char *server, const char *client_id, MongooseMqttConnectionHandler onConnect)
{
  if(NULL == getConnection()) 
  {
    struct mg_connect_opts opts;
    bool secure = false;

#if MG_ENABLE_SSL
    if(MQTT_MQTTS == protocol || MQTT_WSS == protocol) {
      secure = true;
    }
#endif

    Mongoose.getDefaultOpts(&opts, secure);
#if MG_ENABLE_SSL
    if(secure)
    {
      if(!_reject_unauthorized) {
        opts.ssl_ca_cert = "*";
      }
      if(_cert && _key) {
        opts.ssl_cert = _cert;
        opts.ssl_key = _key;
      }
    }
#endif

    const char *err;
    opts.error_string = &err;

    DBUGF("Trying to connect to %s", server);
    _onConnect = onConnect;
    _client_id = client_id;
    if(MongooseSocket::connect(
      mg_connect_opt(Mongoose.getMgr(), server, eventHandler, this, opts)))
    {
      return true;
    }

    DBUGF("Failed to connect to %s: %s", server, err);
  }
  return false;
}

bool MongooseMqttClient::subscribe(const char *topic)
{
  if(connected())
  {
    struct mg_mqtt_topic_expression s_topic_expr = {NULL, 0};
    s_topic_expr.topic = topic;
    DBUGF("Subscribing to '%s'", topic);
    mg_mqtt_subscribe(getConnection(), &s_topic_expr, 1, 42);
    return true;
  }
  return false;
}

bool MongooseMqttClient::publish(const char *topic, mg_str payload, bool retain, int qos)
{
  int flags = qos;
  if(retain) {
    flags |= MG_MQTT_RETAIN;
  }
  
  if(connected()) {
    mg_mqtt_publish(getConnection(), topic, 65, flags, payload.p, payload.len);
    return true;
  }

  return false;
}

bool MongooseMqttClient::disconnect()
{
  if(connected()) {
    mg_mqtt_disconnect(getConnection());
    MongooseSocket::disconnect();
    return true;
  }

  return false;
}
