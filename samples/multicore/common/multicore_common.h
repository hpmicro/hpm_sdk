#ifndef MULTICORE_COMMON_H
#define MULTICORE_COMMON_H


extern const uint32_t sec_core_img_size;
extern const uint8_t sec_core_img[];

void multicore_release_cpu(uint8_t cpu_id, uint32_t start_addr);

#endif /* MULTICORE_COMMON_H */