#ifndef STATUSCODE_H
#define STATUSCODE_H

#include <string>

class http {
public:
  enum statusCode {
    CONTINUE = 100,
    SWITCHING_PROTOCOLS = 101,
    PROCESSING = 102,
    EARLY_HINTS = 103,
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    // Non auth 203
    NO_CONTENT = 204,
    RESET_CONTENT = 205,
    PARTIAL_CONTENT = 206,
    MULTIPLE_CHOICES = 300,
    MOVED_PERMANENTLY = 301,
    FOUND = 302,
    SEE_OTHER = 303,
    NOT_MODIFIED = 304,
    USE_PROXY = 305,
    SWITCH_PROXY = 306,
    TEMPORARY_REDIRECT = 307,
    PERMANENT_REDIRECT = 308,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    PAYMENT_REQUIRED = 402,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    NOT_ACCEPTABLE = 406,
    PROXY_AUTHENTICATION_REQUIRED = 407,
    REQUEST_TIMEOUT = 408,
    CONFLICT = 409,
    GONE = 410,
    LENGTH_REQUIRED = 411,
    PRECONDITION_FAILED = 412,
    PAYLOAD_TOO_LARGE = 413,
    URI_TOO_LONG = 414,
    UNSUPPORTED_MEDIA_TYPE = 415,
    RANGE_NOT_SATISFIABLE = 416,
    EXPECTATION_FAILED = 417,
    IM_A_TEAPOT = 418,
    MISDIRECTION_REQUEST = 421,
    UNPROCESSABLE_CONTENT = 422,
    UPGRADE_REQUIRED = 426,
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAILABLE = 503,
    GATEWAY_TIMEOUT = 504,
    HTTP_VERSION_NOT_SUPPORTED = 505
  };

  constexpr static std::string StatusCodeString(const statusCode code) {
    switch (code) {
    case CONTINUE:
      return "Continue";
    case SWITCHING_PROTOCOLS:
      return "Switching Protocols";
    case PROCESSING:
      return "Processing";
    case EARLY_HINTS:
      return "Early Hints";
    case OK:
      return "OK";
    case CREATED:
      return "Created";
    case ACCEPTED:
      return "Accepted";
    case NO_CONTENT:
      return "No Content";
    case RESET_CONTENT:
      return "Reset Content";
    case PARTIAL_CONTENT:
      return "Partial Content";
    case MULTIPLE_CHOICES:
      return "Multiple Choices";
    case MOVED_PERMANENTLY:
      return "Moved Permanently";
    case FOUND:
      return "Found";
    case SEE_OTHER:
      return "See Other";
    case NOT_MODIFIED:
      return "Not Modified";
    case USE_PROXY:
      return "Use Proxy";
    case SWITCH_PROXY:
      return "Switch Proxy";
    case TEMPORARY_REDIRECT:
      return "Temporary Redirect";
    case PERMANENT_REDIRECT:
      return "Permanent Redirect";
    case BAD_REQUEST:
      return "Bad Request";
    case UNAUTHORIZED:
      return "Unauthorized";
    case PAYMENT_REQUIRED:
      return "Payment Required";
    case FORBIDDEN:
      return "Forbidden";
    case NOT_FOUND:
      return "Not Found";
    case METHOD_NOT_ALLOWED:
      return "Method Not Allowed";
    case NOT_ACCEPTABLE:
      return "Not Acceptable";
    case PROXY_AUTHENTICATION_REQUIRED:
      return "Proxy Authentication Required";
    case REQUEST_TIMEOUT:
      return "Request Timeout";
    case CONFLICT:
      return "Conflict";
    case GONE:
      return "Gone";
    case LENGTH_REQUIRED:
      return "Length Required";
    case PRECONDITION_FAILED:
      return "Precondition Failed";
    case PAYLOAD_TOO_LARGE:
      return "Payload Too Large";
    case URI_TOO_LONG:
      return "URI Too Long";
    case UNSUPPORTED_MEDIA_TYPE:
      return "Unsupported Media Type";
    case RANGE_NOT_SATISFIABLE:
      return "Range Not Satisfiable";
    case EXPECTATION_FAILED:
      return "Expectation Failed";
    case IM_A_TEAPOT:
      return "I'm a teapot";
    case MISDIRECTION_REQUEST:
      return "Misdirection Request";
    case UNPROCESSABLE_CONTENT:
      return "Unprocessable Content";
    case UPGRADE_REQUIRED:
      return "Upgrade Required";
    case INTERNAL_SERVER_ERROR:
      return "Internal Server Error";
    case NOT_IMPLEMENTED:
      return "Not Implemented";
    case BAD_GATEWAY:
      return "Bad Gateway";
    case SERVICE_UNAVAILABLE:
      return "Service Unavailable";
    case GATEWAY_TIMEOUT:
      return "Gateway Timeout";
    case HTTP_VERSION_NOT_SUPPORTED:
      return "HTTP Version Not Supported";
    default: // This is undefined Behaviour
      return "";
    }
  }
};

#endif // !STATUSCODE_H
