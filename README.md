# ArduinoMongoose

[![Build Status](https://travis-ci.org/jeremypoulter/ArduinoMongoose.svg?branch=master)](https://travis-ci.org/jeremypoulter/ArduinoMongoose)

A wrapper for Mongoose to help build into Arduino framework.

## Upgrade Notes

### File Upload Behaviour (Mongoose 7)

Mongoose 7 changed how HTTP uploads are handled. If you are upgrading from an earlier version or porting existing upload code, note the following:

**Large file / OTA uploads (recommended: `application/octet-stream`)**

For large payloads such as OTA firmware images, POST the file as a raw binary body with `Content-Type: application/octet-stream` (or any type without a `boundary` parameter). The library will stream the body incrementally to your `onUpload` handler via `MG_EV_HTTP_PART_BEGIN` / `MG_EV_HTTP_PART_DATA` / `MG_EV_HTTP_PART_END` events, keeping peak heap usage proportional to the chunk size rather than the total file size.

**`multipart/form-data` uploads**

`multipart/form-data` requests are still supported and the `onUpload` callback API is unchanged. However, Mongoose 7 buffers the entire request body before firing the message event, so `multipart/form-data` is **not suitable for large files** on memory-constrained devices (e.g. ESP32 with ~300 KB free heap). Use `application/octet-stream` for firmware-sized payloads.

**Client-side change**

If your upload page previously used an HTML `<form enctype="multipart/form-data">` or set `contentType: false` in an Ajax call, switch to posting the raw `File` object with an explicit content type:

```js
$.ajax({
  url: '/update',
  type: 'POST',
  data: file,
  contentType: file.type || 'application/octet-stream',
  processData: false
});
```

**Server-side API is unchanged** — the `onRequest` / `onUpload` / `onClose` handler registration and all `MG_EV_HTTP_PART_*` event constants remain the same.
