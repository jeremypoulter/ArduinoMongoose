//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

#include <MongooseCore.h>
#include <MongooseHttpClient.h>

#include "../../common/root_ca.h"

MongooseHttpClient client;

bool run = true;
bool headers_shown = false;

bool s_show_headers = false;
const char *s_url = "https://www.google.com";
//const char *s_url = "https://github-releases.githubusercontent.com/202533650/85e078fb-8a10-4dd8-a80c-40c7d86ac2e8?X-Amz-Algorithm=AWS4-HMAC-SHA256&amp;X-Amz-Credential=AKIAIWNJYAX4CSVEH53A%2F20211024%2Fus-east-1%2Fs3%2Faws4_request&amp;X-Amz-Date=20211024T222333Z&amp;X-Amz-Expires=300&amp;X-Amz-Signature=f8a0a2ae38cdc92a919b3235cb3dfa9088fe883dd4efb99e2a02fd3b50776429&amp;X-Amz-SignedHeaders=host&amp;actor_id=0&amp;key_id=0&amp;repo_id=202533650&amp;response-content-disposition=attachment%3B%20filename%3Dopenevse_wifi_v1.bin.zip&amp;response-content-type=application%2Foctet-stream";

// Root CA bundle - shared definition
const char *root_ca = ARDUINO_MONGOOSE_ROOT_CA_BUNDLE;

void printHeaders(MongooseHttpClientResponse *response)
{
  headers_shown = true;

  printf("%d %.*s\n", response->respCode(), (int)response->respStatusMsg().length(), (const char *)response->respStatusMsg());
  int headers = response->headers();
  int i;
  for(i=0; i<headers; i++) {
    printf("%.*s: %.*s\n", 
      (int)response->headerNames(i).length(), (const char *)response->headerNames(i), 
      (int)response->headerValues(i).length(), (const char *)response->headerValues(i));
  }
  printf("\n");
}

int main(int argc, char *argv[])
{
  int i;
  // Process command line arguments
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--show-headers") == 0) {
      s_show_headers = true;
    } else {
      break;
    }
  }

//  if (i + 1 != argc) {
//    fprintf(stderr, "Usage: %s [--show-headers] <URL>\n", argv[0]);
//    exit(EXIT_FAILURE);
//  }
  if(i+1 == argc) {
    s_url = argv[i];
  }

  Mongoose.begin();
  Mongoose.setRootCa(root_ca);

  // Based on https://github.com/typicode/jsonplaceholder#how-to
  client.beginRequest(s_url)->
    onBody([](MongooseHttpClientResponse *response) {
      if(s_show_headers && false == headers_shown) {
        printHeaders(response);
      }
      if (response->body().length() > 0) {
        fwrite(response->body().c_str(), response->body().length(), 1, stdout);
        fflush(stdout);
      }
    })->
    onClose([]() {
      run = false;
    })->send();

  while(run) {
    Mongoose.poll(1000);
  }

  return 0;
}
