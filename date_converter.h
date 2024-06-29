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

#ifndef DATE_CONVERTER_H
#define DATE_CONVERTER_H

#ifdef __cplusplus
extern "C"
{
#endif  // __cplusplus

#define DATE_CONVERTER_VERSION "1.1.2"
#define DATE_CONVERTER_VER_MAJOR 1
#define DATE_CONVERTER_VER_MINOR 1
#define DATE_CONVERTER_VER_PATCH 2

typedef enum DC_CalendarType {DC_PER, DC_GRE, DC_ISM, DC_HEB, DC_JUL, DC_PER_B} dc_calendar_t;

void persian_to_gregorian(int *year, int *month, int *day);
void persian_to_islamic(int *year, int *month, int *day);
void persian_to_hebrew(int *year, int *month, int *day);
void persian_to_julian(int *year, int *month, int *day);
void persian_to_persianb(int *year, int *month, int *day);
void gregorian_to_persian(int *year, int *month, int *day);
void gregorian_to_islamic(int *year, int *month, int *day);
void gregorian_to_hebrew(int *year, int *month, int *day);
void gregorian_to_julian(int *year, int *month, int *day);
void gregorian_to_persianb(int *year, int *month, int *day);
void islamic_to_persian(int *year, int *month, int *day);
void islamic_to_gregorian(int *year, int *month, int *day);
void islamic_to_hebrew(int *year, int *month, int *day);
void islamic_to_julian(int *year, int *month, int *day);
void islamic_to_persianb(int *year, int *month, int *day);
void hebrew_to_persian(int *year, int *month, int *day);
void hebrew_to_gregorian(int *year, int *month, int *day);
void hebrew_to_islamic(int *year, int *month, int *day);
void hebrew_to_julian(int *year, int *month, int *day);
void hebrew_to_persianb(int *year, int *month, int *day);
void julian_to_persian(int *year, int *month, int *day);
void julian_to_gregorian(int *year, int *month, int *day);
void julian_to_islamic(int *year, int *month, int *day);
void julian_to_hebrew(int *year, int *month, int *day);
void julian_to_persianb(int *year, int *month, int *day);
void persianb_to_persian(int *year, int *month, int *day);
void persianb_to_gregorian(int *year, int *month, int *day);
void persianb_to_islamic(int *year, int *month, int *day);
void persianb_to_hebrew(int *year, int *month, int *day);
void persianb_to_julian(int *year, int *month, int *day);

int *persian_to_gregorian_arr(int year, int month, int day, int result_ymd[]);
int *persian_to_islamic_arr(int year, int month, int day, int result_ymd[]);
int *persian_to_hebrew_arr(int year, int month, int day, int result_ymd[]);
int *persian_to_julian_arr(int year, int month, int day, int result_ymd[]);
int *persian_to_persianb_arr(int year, int month, int day, int result_ymd[]);
int *gregorian_to_persian_arr(int year, int month, int day, int result_ymd[]);
int *gregorian_to_islamic_arr(int year, int month, int day, int result_ymd[]);
int *gregorian_to_hebrew_arr(int year, int month, int day, int result_ymd[]);
int *gregorian_to_julian_arr(int year, int month, int day, int result_ymd[]);
int *gregorian_to_persianb_arr(int year, int month, int day, int result_ymd[]);
int *islamic_to_persian_arr(int year, int month, int day, int result_ymd[]);
int *islamic_to_gregorian_arr(int year, int month, int day, int result_ymd[]);
int *islamic_to_hebrew_arr(int year, int month, int day, int result_ymd[]);
int *islamic_to_julian_arr(int year, int month, int day, int result_ymd[]);
int *islamic_to_persianb_arr(int year, int month, int day, int result_ymd[]);
int *hebrew_to_persian_arr(int year, int month, int day, int result_ymd[]);
int *hebrew_to_gregorian_arr(int year, int month, int day, int result_ymd[]);
int *hebrew_to_islamic_arr(int year, int month, int day, int result_ymd[]);
int *hebrew_to_julian_arr(int year, int month, int day, int result_ymd[]);
int *hebrew_to_persianb_arr(int year, int month, int day, int result_ymd[]);
int *julian_to_persian_arr(int year, int month, int day, int result_ymd[]);
int *julian_to_gregorian_arr(int year, int month, int day, int result_ymd[]);
int *julian_to_islamic_arr(int year, int month, int day, int result_ymd[]);
int *julian_to_hebrew_arr(int year, int month, int day, int result_ymd[]);
int *julian_to_persianb_arr(int year, int month, int day, int result_ymd[]);
int *persianb_to_persian_arr(int year, int month, int day, int result_ymd[]);
int *persianb_to_gregorian_arr(int year, int month, int day, int result_ymd[]);
int *persianb_to_islamic_arr(int year, int month, int day, int result_ymd[]);
int *persianb_to_hebrew_arr(int year, int month, int day, int result_ymd[]);
int *persianb_to_julian_arr(int year, int month, int day, int result_ymd[]);

int leap_persian(int year);
int leap_gregorian(int year);
int leap_islamic(int year);
int leap_hebrew(int year);
int leap_julian(int year);
int leap_persianb(int year);
int leap_persianb2(int year);

int hebrew_year_months(int year);
int hebrew_year_days(int year);

int persian_month_days(int year, int month);
int gregorian_month_days(int year, int month);
int islamic_month_days(int year, int month);
int hebrew_month_days(int year, int month);
int julian_month_days(int year, int month);
int persianb_month_days(int year, int month);

const char *persian_month_name(int month);
const char *gregorian_month_name(int month);
const char *islamic_month_name(int month);
const char *hebrew_month_name(int year, int month);
const char *julian_month_name(int month);
const char *persianb_month_name(int month);

const char *weekday_str(int i);
int weekday_jd(double jd);
const char *weekday_jd_str(double jd);
int weekday_ymd(int year, int month, int day, dc_calendar_t calendar_type);
const char *weekday_ymd_str(int year, int month, int day, dc_calendar_t calendar_type);

int check_date_ldom(int year, int month, int day, dc_calendar_t calendar_type, int *last_day_of_month);
int check_date_ldom_str(int year, int month, int day, dc_calendar_t calendar_type, int *last_day_of_month, char **error_str);
int check_date_str(int year, int month, int day, dc_calendar_t calendar_type, char **error_str);
int check_date(int year, int month, int day, dc_calendar_t calendar_type);

const char *norm_leap_str(int is_leap_year);

double persian_to_jd(int year, int month, int day);
double gregorian_to_jd(int year, int month, int day);
double islamic_to_jd(int year, int month, int day);
double hebrew_to_jd(int year, int month, int day);
double julian_to_jd(int year, int month, int day);
double persianb_to_jd(int year, int month, int day);

void jd_to_persian(double jd, int *year, int *month, int *day);
void jd_to_gregorian(double jd, int *year, int *month, int *day);
void jd_to_islamic(double jd, int *year, int *month, int *day);
void jd_to_hebrew(double jd, int *year, int *month, int *day);
void jd_to_julian(double jd, int *year, int *month, int *day);
void jd_to_persianb(double jd, int *year, int *month, int *day);

int *jd_to_persian_arr(double jd, int result_ymd[]);
int *jd_to_gregorian_arr(double jd, int result_ymd[]);
int *jd_to_islamic_arr(double jd, int result_ymd[]);
int *jd_to_hebrew_arr(double jd, int result_ymd[]);
int *jd_to_julian_arr(double jd, int result_ymd[]);
int *jd_to_persianb_arr(double jd, int result_ymd[]);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // DATE_CONVERTER_H
