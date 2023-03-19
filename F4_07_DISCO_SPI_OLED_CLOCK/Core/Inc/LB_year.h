//  LB_year.h
//  v1.0

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _LB_YEAR_H_
#define _LB_YEAR_H_

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

#define MONTHS 12
#define MAX_YEAR 9999
#define LAST_DAY (whole_year[pday->month_number].days == pday->day)
#define LAST_DAY_FEB_NOT_LEAP ((whole_year[1].days - 1) <= pday->day)
#define LAST_MONTH ( (MONTHS - 1) == pday->month_number )

typedef struct month {
	const char * short_form;
	uint8_t days;
} month_t;

typedef struct day_m_y {
	uint8_t day;
	uint8_t month_number;
	uint16_t year;
} day_t;

bool leap_year(uint16_t year);
void LB_next_day(day_t * pday);

extern const month_t whole_year[MONTHS];

#endif
