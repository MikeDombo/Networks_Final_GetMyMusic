#ifndef MESSAGE_H
#define MESSAGE_H

#include "Project4Common.h"

using json = JSON;

class Message: public json {
public:
    Message();  // Sets the "version" field
    //explicit Message(const json &j);  // I have no idea how to make this work
};

class Request: public Message {
public:
    Request();  // Sets the "request" field to an empty JSON array
};

// Request classes: appropriate "type" strings and getter/setter methods
class ListRequest: public Request {
public:
    ListRequest();
    explicit ListRequest (const JSON& j);
};

class PullRequest: public Request {
public:
    PullRequest();
    explicit PullRequest (const JSON& j);
};

class PushRequest: public Request {
public:
    PushRequest();
    explicit PushRequest (const JSON& j);
};

class Response: public Message {
public:
    Response();  // Sets the "response" field to an empty JSON array
};

// Response classes: appropriate "type" strings and getter/setter methods
class ListResponse: public Response {
public:
    ListResponse();
};

class PullResponse: public Response {
public:
    PullResponse();
};

class PushResponse: public Response {
public:
    PushResponse();
};

#endif
