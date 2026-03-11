#include "h26x_reader.h"
#include <stdio.h>
#include <string.h>
//77 00 00 00 01  return 4   *p_zero_num 3
unsigned char* find_start_code(unsigned char *p_buffer, unsigned char *p_end, unsigned int *p_zero_num, int is_h265)//return 0 not find 0x1 pos,  p_zero_num 0x1 
{
    
    unsigned char *p_temp = p_buffer;
    p_end -= 2;
    if (is_h265) {
        *p_end--;
    }
    *p_zero_num = 0;
    p_temp += 2;
    while (p_temp < p_end)
    {
        if (*p_temp > 1) {
            p_temp += 3;
            continue;
        }
        if (*p_temp == 0) {
            p_temp++;
            continue;
        }
        if (p_temp[-1] || p_temp[-2]) {
            p_temp += 3;
            continue;
        }
        unsigned char *p_zero = p_temp - 1;

        while (p_zero >= p_buffer) {
            if (*p_zero) {
                break;
            }
            p_zero--;
            (*p_zero_num)++;
        }
        return p_temp - 2;
    }
    return NULL;
}

bool is_start_nal(unsigned char *p_buffer, int is_h265)
{
    if (is_h265) {
        unsigned char nal_type = (p_buffer[0] & 0x7e) >> 1;
        if ((nal_type != 1) && (nal_type != 5) && (nal_type != 9) && (nal_type != 10) && (nal_type != 32) && (nal_type != 33) && (nal_type != 34)) {
            return true;
        }
        if ((nal_type == 1) || (nal_type == 5)) {
            if (p_buffer[1] >= 0x80) {
                return true;
            }
        }
    }
    else {
        unsigned char nal_type = p_buffer[0] & 0x1f;
        if ((nal_type != 1) && (nal_type != 5) && (nal_type != 9) && (nal_type != 10)) {
            return true;
        }
        if ((nal_type == 1) || (nal_type == 5)) {
            if (p_buffer[1] >= 0x80) {
                return true;
            }
        }
    }
    return false;
}
bool is_data_nal(unsigned char *p_buffer, int is_h265)
{
    if (is_h265) {
        unsigned char nal_type = (p_buffer[0] & 0x7e) >> 1;
        if ((nal_type == 0) || (nal_type == 0) || (nal_type == 19) || (nal_type == 20) || (nal_type == 21)) {
            return true;
        }
    }
    else {
        unsigned char nal_type = p_buffer[0] & 0x1f;
        if ((nal_type == 1) || (nal_type == 5)) {
            return true;
        }
    }
    return false;
}
void read_h26x_frame(FILE *fp, unsigned char *p_buffer, unsigned int buffer_len, unsigned int *p_frame_len, unsigned int *p_read_len, int is_h265)
{
    if (*p_frame_len) {
        *p_read_len -= *p_frame_len;
        memmove(p_buffer, p_buffer + *p_frame_len, *p_read_len);
        *p_frame_len = 0;
        
    }
    unsigned char *p_start = NULL;
    unsigned char *start_code = p_buffer;
    unsigned int zero_num = 0;
    bool get_data = false;
    while (1) {
        unsigned char *p_temp = start_code;
        start_code = find_start_code(start_code, p_buffer + *p_read_len, &zero_num,is_h265);
        if (NULL == start_code) {
            if (feof(fp)) {
                return;
            }
            int ret = fread(p_buffer+ *p_read_len, 1, buffer_len- *p_read_len,fp);
            if (ret <= 0) {
                return;
            }
            *p_read_len += ret;
            start_code = p_temp;
            continue;
        }
        start_code += 3;
        if ((NULL == p_start) && is_start_nal(start_code, is_h265)) {
            p_start = start_code - zero_num -1;
            if (p_start != p_buffer) {
                start_code -= p_start - p_buffer;
                *p_read_len -= p_start - p_buffer;
                memmove(p_buffer, p_start, *p_read_len);
            }
        }
        
        if (!get_data) {
            if (is_data_nal(start_code, is_h265)) {
                get_data = true;
            }
            continue;
        }
        if (is_start_nal(start_code, is_h265)) {
            *p_frame_len = start_code - 3 - p_buffer;
            if (zero_num != 2) {
                (*p_frame_len)--;
            }
            return;
        }
    }
}

void read_h264_frame(FILE *fp, unsigned char *p_buffer, unsigned int buffer_len, unsigned int *p_frame_len, unsigned int *p_read_len)
{
    read_h26x_frame(fp, p_buffer, buffer_len, p_frame_len, p_read_len, 0);
}

void read_h265_frame(FILE *fp, unsigned char *p_buffer, unsigned int buffer_len, unsigned int *p_frame_len, unsigned int *p_read_len)
{
    read_h26x_frame(fp, p_buffer, buffer_len, p_frame_len, p_read_len, 1);
}