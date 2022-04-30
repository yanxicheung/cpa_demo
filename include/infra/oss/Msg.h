#ifndef HC75B50AE_F14F_4D29_AB06_78486599BBBD
#define HC75B50AE_F14F_4D29_AB06_78486599BBBD

#include <stdint.h>
const uint8_t MAX_INST_KEY_LEN = 32;

class Msg
{
public:
    Msg(Msg&& rhs);
    Msg(const Msg& rhs);
    Msg(const char* instKey, int eventId, const void* data, int len);
    ~Msg();

    Msg& operator=(Msg&& rhs);
    Msg& operator=(const Msg& rhs);
public:
    int eventId_;
    void *data_;
    int len_;
    char instKey_[MAX_INST_KEY_LEN];
private:
    void assignInstKey(const char* instKey);
};

#endif /* HC75B50AE_F14F_4D29_AB06_78486599BBBD */
