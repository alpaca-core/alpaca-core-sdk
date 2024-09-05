# File Download Library

Library to facilitate file downloads through HTTP GET requests.

## Note

The CMake configuration of the library will check for OpenSSL and will use it if found. If not found, the library will be built without SSL support and not GETs through HTTPS will be available.
