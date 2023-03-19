#include "LB_year.h"

const month_t whole_year[MONTHS] = {
	{"jan", 31},
	{"feb", 29},
	{"mar", 31},
	{"apr", 30},
	{"may", 31},
	{"jun", 30},
	{"jul", 31},
	{"aug", 31},
	{"sep", 30},
	{"oct", 31},
	{"nov", 30},
	{"dec", 31},
};

void LB_next_day(day_t * pday)
{
	if ( LAST_MONTH && LAST_DAY)
	{
		if (MAX_YEAR < ++(pday->year))		// increment year
		{
			pday->year = 0;
		}
		pday->month_number = 0;				// set jan
		pday->day = 1;						// set the first day of the month
	}
	else if (!leap_year(pday->year) && (1 == pday->month_number) && LAST_DAY_FEB_NOT_LEAP)
	{
		pday->month_number++;				// increment month
		pday->day = 1;						// set the first day of the month
	}
	else if (LAST_DAY)
	{
		pday->month_number++;				// increment month
		pday->day = 1;						// set the first day of the month
	}
	else
	{
		pday->day++;						// increment day
	}
}

bool leap_year(uint16_t year)
{
	if (0 == year % 4)
	{
		if (0 == year % 100)
		{
			if (0 == year % 400)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}
