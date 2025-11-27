#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<string.h>
#include<stdint.h>

short distance_between_two(short *read_data, uint16_t first_point, uint16_t second_point);
void iteration_find_neighbor(short *read_data, uint16_t core_id, uint8_t neighbor_value, uint8_t *noise_flag, uint8_t cluster_id, uint16_t data_len);
void dbscan_less_space(short *read_data, uint16_t data_len, uint8_t neighbor_value, uint8_t neighbor_min, uint8_t *noise_flag);
void adjust_cluster_order(uint16_t data_len,uint8_t *noise_flag);
