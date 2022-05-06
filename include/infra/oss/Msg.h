#ifndef HC75B50AE_F14F_4D29_AB06_78486599BBBD
#define HC75B50AE_F14F_4D29_AB06_78486599BBBD

#include <stdint.h>
const uint8_t MAX_INST_KEY_LEN = 32;

class Msg
{
public:
    Msg();
    Msg(Msg&& rhs);
    Msg(const Msg& rhs);
    Msg(const char* instKey, int eventId, const void* data, int len);
    ~Msg();
    bool empty() const;
    Msg& operator=(Msg&& rhs);
    Msg& operator=(const Msg& rhs);
public:
    uint16_t eventId_;
    void *data_;
    uint16_t len_;
    char instKey_[MAX_INST_KEY_LEN];
private:
    void assignInstKey(const char* instKey);
};

#endif /* HC75B50AE_F14F_4D29_AB06_78486599BBBD */
