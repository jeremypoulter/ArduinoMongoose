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
  _client_id(),
  _username(),
  _password(),
  _will_topic(),
  _will_message(),
  _will_retain(false),
  _connected(false),
  _onConnect(nullptr),
  _onMessage(nullptr)
{

}

MongooseMqttClient::~MongooseMqttClient()
{

}

void MongooseMqttClient::onClose(mg_connection *nc)
{
  _connected = false;
  MongooseSocket::onClose(nc);
}

void MongooseMqttClient::onEvent(mg_connection *nc, int ev, void *p)
{
  switch (ev) 
  {
    case MG_EV_MQTT_OPEN:
    {
      int connack_status_code = *(int *) p;
      if (0 == connack_status_code) {
        _connected = true;
        if(_onConnect) {
          _onConnect();
        }
      } else {
        DBUGF("Got mqtt connection error: %d", connack_status_code);
        char buf[100];
        snprintf(buf, sizeof(buf), "MQTT connection error: %d", connack_status_code);
        onError(nc, buf);
      }
      break;
    }

    case MG_EV_MQTT_CMD:
    {
      struct mg_mqtt_message *msg = (struct mg_mqtt_message *) p;
      DBUGF("MQTT command received: %d %d", nc->id, msg->cmd);
      break;
    }

    case MG_EV_MQTT_MSG:
    {
      struct mg_mqtt_message *msg = (struct mg_mqtt_message *) p;
      DBUGF("%lu RECEIVED %.*s <- %.*s", nc->id, (int) msg->data.len,
             msg->data.buf, (int) msg->topic.len, msg->topic.buf);
      if(_onMessage) {
        _onMessage(MongooseString(msg->topic), MongooseString(msg->data));
      }
      break;
    }
  }
}

bool MongooseMqttClient::connect(MongooseMqttProtocol protocol, const char *server, const char *client_id, MongooseMqttConnectionHandler onConnect)
{
  if(nullptr == getConnection()) 
  {
    mg_mqtt_opts opts {
      .user = _username,
      .pass = _password,
      .client_id = mg_str_s(client_id),
      .topic = _will_topic,
      .message = _will_message,
      .retain = _will_retain,
      .clean = true
    };

    char url[128];
    snprintf(url, sizeof(url), "%s://%s", (MQTT_MQTTS == protocol) ? "mqtts" : "mqtt", server);

    DBUGF("Trying to connect to %s", server);
    _onConnect = onConnect;
    _client_id = client_id;

    if(MQTT_MQTTS == protocol) {
      setSecure(server);
    }

    if(MongooseSocket::connect(
      mg_mqtt_connect(Mongoose.getMgr(), url, &opts, eventHandler, this)))
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
    DBUGF("Subscribing to '%s'", topic);

    struct mg_mqtt_opts sub_opts;
    memset(&sub_opts, 0, sizeof(sub_opts));
    sub_opts.topic = mg_str_s(topic);

    mg_mqtt_sub(getConnection(), &sub_opts);
    return true;
  }
  return false;
}

bool MongooseMqttClient::publish(const char *topic, mg_str payload, bool retain, int qos)
{
  if(connected())
  {
    struct mg_mqtt_opts opts;
    memset(&opts, 0, sizeof(opts));
    opts.qos = qos;
    opts.retain = retain;
    opts.topic = mg_str_s(topic);
    opts.message = payload;

    mg_mqtt_pub(getConnection(), &opts);
    return true;
  }

  return false;
}

bool MongooseMqttClient::disconnect()
{
  if(connected())
  {
    struct mg_mqtt_opts opts;
    memset(&opts, 0, sizeof(opts));

    mg_mqtt_disconnect(getConnection(), &opts);
    MongooseSocket::disconnect();
    return true;
  }

  return false;
}
