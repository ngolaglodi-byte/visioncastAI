#pragma once
#include <stdio.h>
void read_h264_frame(FILE *fp, unsigned char *p_buffer, unsigned int buffer_len, unsigned int *p_frame_len, unsigned int *p_read_len);
void read_h265_frame(FILE *fp, unsigned char *p_buffer, unsigned int buffer_len, unsigned int *p_frame_len, unsigned int *p_read_len);