#include "utils.h"

#define LOG_BUFFER_SIZE 30

typedef struct {
  char buffer[LOG_BUFFER_SIZE][128];
  uint32_t buf_idx;
  bool over;
} RingBuf;

static RingBuf log_buffer;

void WriteToRingBuf(char* content) {
  strcpy(log_buffer.buffer[log_buffer.buf_idx], content);
  ++log_buffer.buf_idx;
  if (log_buffer.buf_idx >= LOG_BUFFER_SIZE) {
    log_buffer.buf_idx = 0;
    log_buffer.over = true;
  }
}

void FlushRingBuf() {
  if (log_buffer.over) {
    for (int i = log_buffer.buf_idx; i < LOG_BUFFER_SIZE; ++i) {
      log_write("%s\n", log_buffer.buffer[i]);
    }
  }
  for (int i = 0; i < log_buffer.buf_idx; ++i) {
    log_write("%s\n", log_buffer.buffer[i]);
  }
}
