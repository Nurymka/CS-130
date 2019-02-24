#include <iostream>
#include <fstream>

#include "reverse_proxy_handler.h"
#include "location.h"

#include <iostream>
#include <ctype.h>
#include <cstring>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <string>

Handler* ReverseProxyHandler::create(const NginxConfig& config,
                               const string& root_path) {
  return new ReverseProxyHandler(config, root_path);
}

ReverseProxyHandler::ReverseProxyHandler(const NginxConfig& config, const string& root_path) {
  // TODO(nurymka): force root & location statements inside the block config
  serverRootPath_ = root_path;
  if (config.getTopLevelStatement("root", destinationPath_))
    destinationPath_ = LocationUtils::extractPathOnly(destinationPath_);
  if (config.getTopLevelStatement("location", location_))
    location_ = LocationUtils::extractPathOnly(location_);
}

unique_ptr<HttpResponse> ReverseProxyHandler::handle_request(const HttpRequest& req) {
 unique_ptr<HttpResponse> res = make_unique<HttpResponse>();
  res->version = req.version;
  res->headers.push_back("Content-Type: text/plain");

  //TODO: fix hardcdded destinationPath with correct destination path 
  // struct hostent * host = gethostbyname(destinationPath_.c_str());
  struct hostent * host = gethostbyname("www.ucla.edu");

  int sock;
	struct sockaddr_in client;

  //TODO: configure port to be passed in as parameter into reverse proxy handler, may need to modify session.cc to do so
	int PORT = 80;

  //TODO: fix cout to utilize logger instead, also log proper diagnostic info

  if ( (host == NULL) || (host->h_addr == NULL) ) {
    cout << "Error retrieving DNS information." << endl;
    res->body = "Fail";
    return res;
  }

  bzero(&client, sizeof(client));
  client.sin_family = AF_INET;
  client.sin_port = htons( PORT );
  memcpy(&client.sin_addr, host->h_addr, host->h_length);

  sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock < 0) {
    cout << "Error creating socket." << endl;
    res->body = "Fail";
    res->status_code = 400;
    return res;
  }

  if ( connect(sock, (struct sockaddr *)&client, sizeof(client)) < 0 ) {
    close(sock);
    cout << "Could not connect" << endl;
    res->body = "Fail";
    res->status_code = 400;
    return res;
  }

  //TODO: set host to be correct destination path
  string request = "GET / HTTP/1.1\r\nHost: www.ucla.edu\r\n\r\n";

  if (send(sock, request.c_str(), request.length(), 0) != (int)request.length()) {
    cout << "Error sending request." << endl;
    res->body = "Fail";
    res->status_code = 400;
    return res;
  }

  char recvbuf[512];

  string response;
  int result;

  do {
	  result = recv(sock, recvbuf, 512, 0);
	  if(result > 0){
	    response.append(recvbuf);
	  } 
  } while(result > 0);


	res->body = response;
  res->status_code = 200;

  //TODO: configure 302 redirects, properly display redirected page instead of just setting the entire http proxied response in body of redirected response


  return res;
}