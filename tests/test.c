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
    dc_calendar_t calendar_type = DC_GRE;
    int year = 2024;
    int month = 5;
    int day = 2;

    double jd;
    int y, m, d;
    int error_code;
    char *error_str;
    const char *no_le_year;
    const char *weekday;
    const char *month_name;

    if((error_code = check_date_str(year, month, day, calendar_type, &error_str)))
    {
        printf("%s\n", error_str);
        goto out;
    }

    printf("\n");

    switch(calendar_type)
    {
        case DC_PER: jd = persian_to_jd(year, month, day); break;
        case DC_ISM: jd = islamic_to_jd(year, month, day); break;
        case DC_HEB: jd = hebrew_to_jd(year, month, day); break;
        case DC_JUL: jd = julian_to_jd(year, month, day); break;
        case DC_PER_B: jd = persianb_to_jd(year, month, day); break;
        default: jd = gregorian_to_jd(year, month, day); break;
    }

    weekday = weekday_jd_str(jd);

    jd_to_persian(jd, &y, &m, &d);
    no_le_year = norm_leap_str(leap_persian(y));
    month_name = persian_month_name(m);
    printf("Persian Date:\t\t\t%d/%02d/%02d\t%s\t%s\n", y, m, d, no_le_year, month_name);

    jd_to_gregorian(jd, &y, &m, &d);
    no_le_year = norm_leap_str(leap_gregorian(y));
    month_name = gregorian_month_name(m);
    printf("Gregorian Date:\t\t\t%d/%02d/%02d\t%s\t%s\n", y, m, d, no_le_year, month_name);

    jd_to_islamic(jd, &y, &m, &d);
    no_le_year = norm_leap_str(leap_islamic(y));
    month_name = islamic_month_name(m);
    printf("Islamic Date:\t\t\t%d/%02d/%02d\t%s\t%s\n", y, m, d, no_le_year, month_name);

    jd_to_hebrew(jd, &y, &m, &d);
    no_le_year = norm_leap_str(leap_hebrew(y));
    month_name = hebrew_month_name(y, m);
    printf("Hebrew Date:\t\t\t%d/%02d/%02d\t%s\t%s\n", y, m, d, no_le_year, month_name);

    jd_to_julian(jd, &y, &m, &d);
    no_le_year = norm_leap_str(leap_julian(y));
    month_name = julian_month_name(m);
    printf("Julian Date:\t\t\t%d/%02d/%02d\t%s\t%s\n", y, m, d, no_le_year, month_name);

    jd_to_persianb(jd, &y, &m, &d);
    no_le_year = norm_leap_str(leap_persianb(y));
    month_name = persianb_month_name(m);
    printf("Persian (Birashk) Date:\t\t%d/%02d/%02d\t%s\t%s\n", y, m, d, no_le_year, month_name);

    printf("Weekday:\t\t\t%s\n\n", weekday);

out:
    free(error_str);
    return error_code;
}
