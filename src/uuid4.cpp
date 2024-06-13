/**
 * Copyright (c) 2018 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include <stdio.h>
#include <stdint.h>

#include "uuid4.h"

namespace dwt {


// 静态变量类外初始化
uint64_t UUID4::m_seed[2];
std::mutex UUID4::m_mutex;

static uint64_t xorshift128plus(uint64_t *s) {
  /* http://xorshift.di.unimi.it/xorshift128plus.c */
  uint64_t s1 = s[0];
  const uint64_t s0 = s[1];
  s[0] = s0;
  s1 ^= s1 << 23;
  s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);
  return s[1] + s0;
}


int UUID4::uuid4_init(void) {
    int res;
    FILE *fp = fopen("/dev/urandom", "rb");
    if (!fp) {
      return UUID4_EFAILURE;
    }
    res = fread(m_seed, 1, sizeof(m_seed), fp);
    fclose(fp);
    if ( res != sizeof(m_seed) ) {
      return UUID4_EFAILURE;
    }
    return UUID4_ESUCCESS;
}


size_t UUID4::generateSessionId() {
    char buf[UUID4_LEN] = {0};
    // TODO: performance?
    std::lock_guard<std::mutex> lock(m_mutex);
    uuid4_init();
    uuid4_generate(buf);
    
    return std::hash<std::string>()(std::string(buf));
}


void UUID4::uuid4_generate(char *dst) {
  static const char *template_str = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
  static const char *chars = "0123456789abcdef";
  union { unsigned char b[16]; uint64_t word[2]; } s;
  const char *p;
  int i, n;
  /* get random */
  s.word[0] = xorshift128plus(m_seed);
  s.word[1] = xorshift128plus(m_seed);
  /* build string */
  p = template_str;
  i = 0;
  while (*p) {
    n = s.b[i >> 1];
    n = (i & 1) ? (n >> 4) : (n & 0xf);
    switch (*p) {
      case 'x'  : *dst = chars[n];              i++;  break;
      case 'y'  : *dst = chars[(n & 0x3) + 8];  i++;  break;
      default   : *dst = *p;
    }
    dst++, p++;
  }
  *dst = '\0';
}




} // end namespace dwt