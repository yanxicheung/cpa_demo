#include "Msg.h"
#include <assert.h>
#include <cstring>
#include <string>
#include <assert.h>

Msg::Msg():eventId_(0), data_(nullptr), len_(0)
{
    memset(instKey_, 0, sizeof(instKey_));
}

bool Msg::empty() const
{
    return eventId_ == 0 && len_ == 0;
}

Msg::Msg(const char* instKey, int eventId, const void* data, int len)
     :eventId_(eventId), data_(nullptr), len_(0)
{
    assignInstKey(instKey);
    if (data && len != 0)
    {
        data_ = new char[len];
        assert(data_ != nullptr);
        memcpy(data_, data, len);
        len_ = len;
    }
}

Msg::Msg(const Msg& rhs)
{
    data_ = new char[rhs.len_];
    assert(data_ != nullptr);
    eventId_ = rhs.eventId_;
    len_ = rhs.len_;
    memcpy(data_, rhs.data_, rhs.len_);
    assignInstKey(rhs.instKey_);
}

Msg::Msg(Msg&& rhs)
: eventId_(rhs.eventId_), data_(rhs.data_), len_(rhs.len_)
{
    rhs.data_ = nullptr;
    rhs.len_ = 0;
    assignInstKey(rhs.instKey_);
}

Msg& Msg::operator=(Msg&& rhs)
{
    if (this != &rhs)
    {
        if (data_)
        {
            char *p = (char*) data_;
            delete[] p;
        }
        assignInstKey(rhs.instKey_);
        len_ = rhs.len_;
        data_ = rhs.data_;
        rhs.len_ = 0;
        rhs.data_ = nullptr;
    }
    return *this;
}

Msg& Msg::operator=(const Msg& rhs)
{
    if (this != &rhs)
    {
        Msg tmp(rhs);
        // swap pointer
        void *p = data_;
        data_ = tmp.data_;
        tmp.data_ = p;

        eventId_ = rhs.eventId_;
        len_ = rhs.len_;
        assignInstKey(tmp.instKey_);
    }
    return *this;
}

Msg::~Msg()
{
    char *p = (char*) data_;
    if (p)
    {
        delete[] p;
    }
}

void Msg::assignInstKey(const char* instKey)
{
    assert(sizeof(instKey_) >= strlen(instKey) + 1);
    memset(instKey_, 0, sizeof(instKey_));
    strcpy(instKey_, instKey);
}

