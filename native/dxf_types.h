#pragma once
#include <stdint.h>

typedef enum dxf_event_types {
    dxf_event_type_quote,
    dxf_event_type_time_and_sale,
} dxf_event_types;

typedef struct dxf_event {
    int32_t event_type;
    char *symbol_name;
} dxf_event;

typedef struct dxfevent_quote {
    dxf_event event;
    double bid_price;
    double ask_price;
} dxf_event_quote;

typedef struct dxf_event_time_and_sale {
    dxf_event event;
    int32_t event_flag;
    int64_t index;
} dxf_event_time_and_sale;
