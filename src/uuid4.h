#pragma once


#include <string>
#include <mutex>

namespace dwt {

enum {
  UUID4_ESUCCESS =  0,
  UUID4_EFAILURE = -1
};


class UUID4 {

public:

  static size_t generateSessionId();

private:
  const static int UUID4_LEN = 37;

  static int  uuid4_init(void);

  static void uuid4_generate(char *dst);

  static uint64_t m_seed[2];

  static std::mutex m_mutex;
};

} // end namespace dwt

