#ifndef ARDUINO_MONGOOSE_TEST_TLS_CERTS_H
#define ARDUINO_MONGOOSE_TEST_TLS_CERTS_H

// Self-signed certificate and private key for native unit tests only.
// Generated with:
//   openssl req -x509 -newkey rsa:2048 -keyout key.pem -out cert.pem \
//     -days 3650 -nodes -subj "/CN=localhost" \
//     -addext "subjectAltName=IP:127.0.0.1,DNS:localhost"

static const char TEST_TLS_CERT[] =
  "-----BEGIN CERTIFICATE-----\r\n"
  "MIIDJTCCAg2gAwIBAgIULtzK4M9iD68ZjYNXDyntQCmEN8UwDQYJKoZIhvcNAQEL\r\n"
  "BQAwFDESMBAGA1UEAwwJbG9jYWxob3N0MB4XDTI2MDcwOTA5MDIyNFoXDTM2MDcw\r\n"
  "NjA5MDIyNFowFDESMBAGA1UEAwwJbG9jYWxob3N0MIIBIjANBgkqhkiG9w0BAQEF\r\n"
  "AAOCAQ8AMIIBCgKCAQEAuEjPxXcrQjioJSVg+BwtunBsNG/hCx18PTQoVVS/Oxh7\r\n"
  "pnuBjiU317iP7R/a3M6wentl5AcuQcY2Q4bqNSKpB5d5A+rZtbd3LEktl06DDCle\r\n"
  "O+Tql9pJcxxVF9dTuIUocE+DJdWeyY3D0B5Ev5+DJEYdGCCQeUsNYwgKMKOcQ5ol\r\n"
  "G/ECCbHBzoHK4Kys5kqVZHNpPb+o4GDfv89s/mYMLEz5p++pqZFducEVP7XtOAMI\r\n"
  "Gm+SsgkTiJeaIDEF6E3B79RRMuF4Qx9Hv6e8Se3cLmjET84Vyc5J9uYNuJ8P6h1r\r\n"
  "1H/5r9sZuV8Rc0N8YKgV4HNwoyqZf9uwROWc49rx2wIDAQABo28wbTAdBgNVHQ4E\r\n"
  "FgQUmWs9yYsksbsa6OSVLAx1fyVEqa8wHwYDVR0jBBgwFoAUmWs9yYsksbsa6OSV\r\n"
  "LAx1fyVEqa8wDwYDVR0TAQH/BAUwAwEB/zAaBgNVHREEEzARhwR/AAABgglsb2Nh\r\n"
  "bGhvc3QwDQYJKoZIhvcNAQELBQADggEBAIxvZmBsiBtru2MVD8vksqQ95fB21MoA\r\n"
  "UwtZxw4a+LhffAalZpsSiPyRLsmL5iPYG/ZZAPT9WCEzSIcKxx3rAHim8m2wdwxm\r\n"
  "UORb4MPN+MSW3sTjbRE23bfi2QXCKOsC54MMV+sIYX0wRpQLh9i+tsL8mnfPJKXb\r\n"
  "jwO6FRSfYYPeeItJw6CB1svl5H+jEOPQR3XPygWHVgUFAWfko4sq7+sPrFmq2MLR\r\n"
  "gMwmLA5MgQmiiIXHTcqIHGHUdo0OWPlXN+krWHaKuBI7UljwLXu2tO0YdC69pezZ\r\n"
  "NX9VH13WyDYJOitvWP4JMjm2BGPwev81RSNcri+jigwv9G6TZ9GDuPQ=\r\n"
  "-----END CERTIFICATE-----\r\n";

static const char TEST_TLS_KEY[] =
  "-----BEGIN PRIVATE KEY-----\r\n"
  "MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQC4SM/FdytCOKgl\r\n"
  "JWD4HC26cGw0b+ELHXw9NChVVL87GHume4GOJTfXuI/tH9rczrB6e2XkBy5BxjZD\r\n"
  "huo1IqkHl3kD6tm1t3csSS2XToMMKV475OqX2klzHFUX11O4hShwT4Ml1Z7JjcPQ\r\n"
  "HkS/n4MkRh0YIJB5Sw1jCAowo5xDmiUb8QIJscHOgcrgrKzmSpVkc2k9v6jgYN+/\r\n"
  "z2z+ZgwsTPmn76mpkV25wRU/te04Awgab5KyCROIl5ogMQXoTcHv1FEy4XhDH0e/\r\n"
  "p7xJ7dwuaMRPzhXJzkn25g24nw/qHWvUf/mv2xm5XxFzQ3xgqBXgc3CjKpl/27BE\r\n"
  "5Zzj2vHbAgMBAAECggEAC0B62yozDHiMH7SADrYdvqf1m2AHZBfZSZ1hJxClGnUc\r\n"
  "u6YH24d/ON8ufoJ26Qt/sALPzlP36ZEyJ/5ZV4Q1wN5pR6lzwIoiryQKshXbn4b5\r\n"
  "F3wbLMlpuasNsQZ3OKl8thrMvack6laLEa5/cRnKtpDHXv1CBP3lo9nOUXjz5hat\r\n"
  "TfsrA/Gm5nIuM7mGP6LcpXXGhk6VE+HYuF1XhkUCCmG3Z9tDe1bhMMkUctuHT7Sz\r\n"
  "Dq7rbY9c54Q+VBpZb9tPEn96k9ZFqH2cKK2kO49hHrkpzX6VBiQNVi62Bsz6yikS\r\n"
  "oaBYW5+GrDFNWRn6YvpfH4FWS0fwm8ZnHsLfyfiSnQKBgQDbKgTzps7G7jtBrdGW\r\n"
  "ooF9NbajLoj/2i53sNwdIA48pJgrarKgoidRwuTO2lQZEAgom7s+XGCxfBQBZXbm\r\n"
  "klimYErqlgMX/MyWQLUeZ1Rj6cXxwENbTK0mlItI5mAHYzTg+HU5ZgVUXzq2AwOZ\r\n"
  "31qMva0hiH2XMLK/xc9KwgkM1wKBgQDXQgWVNWOU7I3DhNOafjBFCogvteVrCKIB\r\n"
  "jimqrOMxzedL2fgD9+B/FBy3QPMC8ItIMv2CyFDuGZ6i6VJ6jWivU10wRuT/dlHi\r\n"
  "UriYoFAse2GhGN+v+4JZ6KMcwWuOCTRNp1v4PMqWkli4DMcprmOyaILTwDsTJzTI\r\n"
  "HfjTHh4+nQKBgGuzdId/+7GKtRgLoSmTZmrvkHcUkUKaJ6euBB1T+WsXv6fmYS3L\r\n"
  "QeNOET8Nz7IZTMkX7qYwo1vVC8e74vhgBqyMOYSaf560Hp3f4qG0SaZn5VxuDSu6\r\n"
  "vIRvDxueCqbWu8+qiYDRf+D8g0+an5RoTIlJVOFbv8wipFTVko68V9I1AoGBALbx\r\n"
  "2rLUPD0uytFYC0trYfTbnaEG8+OC48V0oH75DuKeP6zcpIgi3H7Ehh9NKPvfmlCT\r\n"
  "XZgQTY/+4C9PI5Uap9USgcntBuUDF1bFvTOiVGFpvzDeZgY4OLHXIQHodVOzCOvS\r\n"
  "iDyy1e8glBxgZ8hJ+VuArWfwIYUGtjo6Fua+2y+VAoGBAJLinvFIA95hr2gMAKr4\r\n"
  "r7QbBdUThxth6Kv/hn9bYiYWvEOW8qEFt9kWDpPQrQ3Uzg2d/uXpkz4tiLCO/zf5\r\n"
  "LYAoQGpWFYZJE1Vmj+3+PssHCDvaqWoxMq5rnI7uo+E6FHOXzzF8Diw7bohv8qrC\r\n"
  "FFfNlRmU3Gah+3ri+fsGkLIT\r\n"
  "-----END PRIVATE KEY-----\r\n";

#endif // ARDUINO_MONGOOSE_TEST_TLS_CERTS_H
