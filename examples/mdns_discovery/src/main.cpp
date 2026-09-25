#include <MongooseCore.h>
#include <MongooseMdns.h>
#include <cstdio>

// Run on a host with an active network interface. Pass a unique hostname when
// launching several copies. No Avahi/Bonjour daemon is needed by this process.
int main(int argc, char **argv) {
  Mongoose.begin();
  if (!Mdns.begin(argc > 1 ? argv[1] : "mongoose-example")) return 1;
  Mdns.addService("_http._tcp", 8000);
  Mdns.addServiceTxt("_http._tcp", "version", "1");
  Mdns.browse("_http._tcp");
  uint64_t deadline = mg_millis() + 5000;
  while (mg_millis() < deadline) Mongoose.poll(10);
  for (const auto &peer : Mdns.services()) {
    std::printf("%s -> %s:%u (%zu addresses)\n", peer.instance.c_str(),
                peer.hostname.c_str(), peer.port, peer.addresses.size());
  }
  Mdns.cancelBrowse();
  Mdns.end();
  Mongoose.end();
  return 0;
}
