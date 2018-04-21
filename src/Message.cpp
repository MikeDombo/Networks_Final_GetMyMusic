#include "Project4Common.h"

class MessageCastException: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Attempt to create a Message class from unsatisfactory JSON";
    }
} messageCastException;

Message::Message() {
    (*this)["version"] = (const json) json(VERSION);
}

Request::Request() {
    json emptyArr;
    emptyArr.makeArray();
    (*this)["request"] = emptyArr;
}

ListRequest::ListRequest() {
    (*this)["type"] = (const json) json("listRequest", true);
}

ListRequest::ListRequest (const JSON& j) {
    if (!verifyJSONPacket(j, "listRequest")) {
        throw messageCastException;
    }
}

PullRequest::PullRequest() {
    (*this)["type"] = (const json) json("pullRequest", true);
}

PullRequest::PullRequest (const JSON& j) {
    if (!verifyJSONPacket(j, "pullRequest")) {
        throw messageCastException;
    }
}

PushRequest::PushRequest() {
    (*this)["type"] = (const json) json("pushRequest", true);
}

PushRequest::PushRequest (const JSON& j) {
    if (!verifyJSONPacket(j, "pushRequest")) {
        throw messageCastException;
    }
}

Response::Response() {
    json emptyArr;
    emptyArr.makeArray();
    (*this)["response"] = emptyArr;
}

ListResponse::ListResponse() {
    (*this)["type"] = (const json) json("listResponse", true);
}

PullResponse::PullResponse() {
    (*this)["type"] = (const json) json("pullResponse", true);
}

PushResponse::PushResponse() {
    (*this)["type"] = (const json) json("pushResponse", true);
}