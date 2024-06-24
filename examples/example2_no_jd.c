/*
                     Fourmilab Calendar Converter:
                  by John Walker  --  September, MMXV
              http://www.fourmilab.ch/documents/calendar/
                  (Originally written in JavaScript)

                            Converted to C:
                by Aboutaleb Roshan  --  August, MMXVII
                   22 Mordad, 1396 (13 August, 2017)
               https://www.rosybit.com/products/dateconv/
                         ab.roshan39@gmail.com

                 This library is in the public domain.
*/

#include <stdio.h>
#include <stdlib.h>
#include <date_converter.h>

int main()
{
    int year = 2024;
    int month = 5;
    int day = 2;

    int y, m, d;
    int error_code;
    char *error_str;
    const char *no_le_year;
    const char *weekday;
    const char *month_name;

    if((error_code = check_date_str(year, month, day, DC_GRE, &error_str)))
    {
        printf("%s\n", error_str);
        goto out;
    }

    printf("\n");

    weekday = weekday_ymd_str(year, month, day, DC_GRE);

    y = year; m = month; d = day;
    gregorian_to_persian(&y, &m, &d);
    no_le_year = norm_leap_str(leap_persian(y));
    month_name = persian_month_name(m);
    printf("Persian Date:\t\t\t%d/%02d/%02d\t%s\t%s\n", y, m, d, no_le_year, month_name);

    no_le_year = norm_leap_str(leap_gregorian(year));
    month_name = gregorian_month_name(month);
    printf("Gregorian Date:\t\t\t%d/%02d/%02d\t%s\t%s\n", year, month, day, no_le_year, month_name);

    y = year; m = month; d = day;
    gregorian_to_islamic(&y, &m, &d);
    no_le_year = norm_leap_str(leap_islamic(y));
    month_name = islamic_month_name(m);
    printf("Islamic Date:\t\t\t%d/%02d/%02d\t%s\t%s\n", y, m, d, no_le_year, month_name);

    y = year; m = month; d = day;
    gregorian_to_hebrew(&y, &m, &d);
    no_le_year = norm_leap_str(leap_hebrew(y));
    month_name = hebrew_month_name(y, m);
    printf("Hebrew Date:\t\t\t%d/%02d/%02d\t%s\t%s\n", y, m, d, no_le_year, month_name);

    y = year; m = month; d = day;
    gregorian_to_julian(&y, &m, &d);
    no_le_year = norm_leap_str(leap_julian(y));
    month_name = julian_month_name(m);
    printf("Julian Date:\t\t\t%d/%02d/%02d\t%s\t%s\n", y, m, d, no_le_year, month_name);

    y = year; m = month; d = day;
    gregorian_to_persianb(&y, &m, &d);
    no_le_year = norm_leap_str(leap_persianb(y));
    month_name = persianb_month_name(m);
    printf("Persian (Birashk) Date:\t\t%d/%02d/%02d\t%s\t%s\n", y, m, d, no_le_year, month_name);

    printf("Weekday:\t\t\t%s\n\n", weekday);

out:
    free(error_str);
    return error_code;
}
