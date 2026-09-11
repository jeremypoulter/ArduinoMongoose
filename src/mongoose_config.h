#if defined(ESP32)
#define MG_ARCH MG_ARCH_ESP32
#elif defined(ESP8266)
#define MG_ARCH MG_ARCH_ESP8266
#endif

// Fallback definitions for SSL option constants that may be absent from the
// WolfSSL OpenSSL compatibility layer on ESP32/ESP8266.  These must be placed
// before the SSL headers are pulled in (which happens later inside mongoose.h),
// so we guard them with MG_ARCH to avoid pre-defining them on desktop where
// real OpenSSL already provides the correct values.
#if defined(MG_ARCH)
#ifndef SSL_OP_NO_SSLv2
#define SSL_OP_NO_SSLv2 0
#endif
#ifndef SSL_OP_NO_SSLv3
#define SSL_OP_NO_SSLv3 0
#endif
#ifndef SSL_OP_NO_TLSv1
#define SSL_OP_NO_TLSv1 0
#endif
#ifndef SSL_OP_NO_TLSv1_1
#define SSL_OP_NO_TLSv1_1 0
#endif
#ifndef SSL_FILETYPE_PEM
#define SSL_FILETYPE_PEM 1
#endif
#endif
