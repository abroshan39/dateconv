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

#include <stdlib.h>  // malloc(), NULL
#include <string.h>  // strlen(), NULL
#include <math.h>

// ****************************************************************************************** //
// /////////////////////////////////    COMMON FUNCTIONS    ///////////////////////////////// //

void str_copy_unsafe(char *dest, const char *src)
{
    while((*dest++ = *src++));
}

// /////////////////////////////////    COMMON FUNCTIONS    ///////////////////////////////// //
// ****************************************************************************************** //
// /////////////////////////////////         ASTRO          ///////////////////////////////// //

// Frequently-used constants

static const double MATH_PI = 3.141592653589793;

static const double J2000             = 2451545.0;             // Julian day of J2000 epoch
static const double JulianCentury     = 36525.0;               // Days in Julian century
static const double JulianMillennium  = 365250.0;              // Days in Julian millennium
static const double TropicalYear      = 365.24219878;          // Mean solar tropical year

// DTR: Degrees to radians

double dtr(double d)
{
    return (d * MATH_PI) / 180.0;
}

// RTD: Radians to degrees

double rtd(double r)
{
    return (r * 180.0) / MATH_PI;
}

// FIXANGLE: Range reduce angle in degrees

double fixangle(double a)
{
    return a - 360.0 * (floor(a / 360.0));
}

// FIXANGR: Range reduce angle in radians

double fixangr(double a)
{
    return a - (2 * MATH_PI) * (floor(a / (2 * MATH_PI)));
}

// DSIN: Sine of an angle in degrees

double dsin(double d)
{
    return sin(dtr(d));
}

// DCOS: Cosine of an angle in degrees

double dcos(double d)
{
    return cos(dtr(d));
}

// MOD: Modulus function which works for non-integers

double mod(double a, double b)
{
    return a - (b * floor(a / b));
}

// JWDAY: Calculate day of week from Julian day

int jwday(double jd)
{
    return (int)mod(floor((jd + 1.5)), 7);
}

/* OBLIQEQ  --  Calculate the obliquity of the ecliptic for a given
                Julian date.  This uses Laskar's tenth-degree
                polynomial fit (J. Laskar, Astronomy and
                Astrophysics, Vol. 157, page 68 [1986]) which is
                accurate to within 0.01 arc second between AD 1000
                and AD 3000, and within a few seconds of arc for
                +/-10000 years around AD 2000.  If we're outside the
                range in which this fit is valid (deep time) we
                simply return the J2000 value of the obliquity, which
                happens to be almost precisely the mean. */

double obliqeq(double jd)
{
    static const double oterms[] = {
        -4680.93,
        -1.55,
         1999.25,
        -51.38,
        -249.67,
        -39.05,
         7.12,
         27.87,
         5.79,
         2.45
    };

    double eps, u, v;
    int i;

    v = (u = (jd - J2000) / (JulianCentury * 100));

    eps = 23 + (26 / 60.0) + (21.448 / 3600.0);

    if(fabs(u) < 1.0)
    {
        for(i = 0; i < 10; i++)
        {
            eps += (oterms[i] / 3600.0) * v;
            v *= u;
        }
    }
    return eps;
}

/* NUTATION  --  Calculate the nutation in longitude, deltaPsi, and
                 obliquity, deltaEpsilon for a given Julian date
                 jd.  Results are returned as a two element Array
                 giving (deltaPsi, deltaEpsilon) in degrees. */

double *nutation(double jd, double result[])
{
    /* Periodic terms for nutation in longiude (delta \Psi) and
       obliquity (delta \Epsilon) as given in table 21.A of
       Meeus, "Astronomical Algorithms", first edition. */

    static const int nutArgMult[] = {
         0,  0,  0,  0,  1,
        -2,  0,  0,  2,  2,
         0,  0,  0,  2,  2,
         0,  0,  0,  0,  2,
         0,  1,  0,  0,  0,
         0,  0,  1,  0,  0,
        -2,  1,  0,  2,  2,
         0,  0,  0,  2,  1,
         0,  0,  1,  2,  2,
        -2, -1,  0,  2,  2,
        -2,  0,  1,  0,  0,
        -2,  0,  0,  2,  1,
         0,  0, -1,  2,  2,
         2,  0,  0,  0,  0,
         0,  0,  1,  0,  1,
         2,  0, -1,  2,  2,
         0,  0, -1,  0,  1,
         0,  0,  1,  2,  1,
        -2,  0,  2,  0,  0,
         0,  0, -2,  2,  1,
         2,  0,  0,  2,  2,
         0,  0,  2,  2,  2,
         0,  0,  2,  0,  0,
        -2,  0,  1,  2,  2,
         0,  0,  0,  2,  0,
        -2,  0,  0,  2,  0,
         0,  0, -1,  2,  1,
         0,  2,  0,  0,  0,
         2,  0, -1,  0,  1,
        -2,  2,  0,  2,  2,
         0,  1,  0,  0,  1,
        -2,  0,  1,  0,  1,
         0, -1,  0,  0,  1,
         0,  0,  2, -2,  0,
         2,  0, -1,  2,  1,
         2,  0,  1,  2,  2,
         0,  1,  0,  2,  2,
        -2,  1,  1,  0,  0,
         0, -1,  0,  2,  2,
         2,  0,  0,  2,  1,
         2,  0,  1,  0,  0,
        -2,  0,  2,  2,  2,
        -2,  0,  1,  2,  1,
         2,  0, -2,  0,  1,
         2,  0,  0,  0,  1,
         0, -1,  1,  0,  0,
        -2, -1,  0,  2,  1,
        -2,  0,  0,  0,  1,
         0,  0,  2,  2,  1,
        -2,  0,  2,  0,  1,
        -2,  1,  0,  2,  1,
         0,  0,  1, -2,  0,
        -1,  0,  1,  0,  0,
        -2,  1,  0,  0,  0,
         1,  0,  0,  0,  0,
         0,  0,  1,  2,  0,
        -1, -1,  1,  0,  0,
         0,  1,  1,  0,  0,
         0, -1,  1,  2,  2,
         2, -1, -1,  2,  2,
         0,  0, -2,  2,  2,
         0,  0,  3,  2,  2,
         2, -1,  0,  2,  2
    };

    static const int nutArgCoeff[] = {
        -171996,   -1742,   92095,      89,          /*  0,  0,  0,  0,  1 */
         -13187,     -16,    5736,     -31,          /* -2,  0,  0,  2,  2 */
          -2274,      -2,     977,      -5,          /*  0,  0,  0,  2,  2 */
           2062,       2,    -895,       5,          /*  0,  0,  0,  0,  2 */
           1426,     -34,      54,      -1,          /*  0,  1,  0,  0,  0 */
            712,       1,      -7,       0,          /*  0,  0,  1,  0,  0 */
           -517,      12,     224,      -6,          /* -2,  1,  0,  2,  2 */
           -386,      -4,     200,       0,          /*  0,  0,  0,  2,  1 */
           -301,       0,     129,      -1,          /*  0,  0,  1,  2,  2 */
            217,      -5,     -95,       3,          /* -2, -1,  0,  2,  2 */
           -158,       0,       0,       0,          /* -2,  0,  1,  0,  0 */
            129,       1,     -70,       0,          /* -2,  0,  0,  2,  1 */
            123,       0,     -53,       0,          /*  0,  0, -1,  2,  2 */
             63,       0,       0,       0,          /*  2,  0,  0,  0,  0 */
             63,       1,     -33,       0,          /*  0,  0,  1,  0,  1 */
            -59,       0,      26,       0,          /*  2,  0, -1,  2,  2 */
            -58,      -1,      32,       0,          /*  0,  0, -1,  0,  1 */
            -51,       0,      27,       0,          /*  0,  0,  1,  2,  1 */
             48,       0,       0,       0,          /* -2,  0,  2,  0,  0 */
             46,       0,     -24,       0,          /*  0,  0, -2,  2,  1 */
            -38,       0,      16,       0,          /*  2,  0,  0,  2,  2 */
            -31,       0,      13,       0,          /*  0,  0,  2,  2,  2 */
             29,       0,       0,       0,          /*  0,  0,  2,  0,  0 */
             29,       0,     -12,       0,          /* -2,  0,  1,  2,  2 */
             26,       0,       0,       0,          /*  0,  0,  0,  2,  0 */
            -22,       0,       0,       0,          /* -2,  0,  0,  2,  0 */
             21,       0,     -10,       0,          /*  0,  0, -1,  2,  1 */
             17,      -1,       0,       0,          /*  0,  2,  0,  0,  0 */
             16,       0,      -8,       0,          /*  2,  0, -1,  0,  1 */
            -16,       1,       7,       0,          /* -2,  2,  0,  2,  2 */
            -15,       0,       9,       0,          /*  0,  1,  0,  0,  1 */
            -13,       0,       7,       0,          /* -2,  0,  1,  0,  1 */
            -12,       0,       6,       0,          /*  0, -1,  0,  0,  1 */
             11,       0,       0,       0,          /*  0,  0,  2, -2,  0 */
            -10,       0,       5,       0,          /*  2,  0, -1,  2,  1 */
             -8,       0,       3,       0,          /*  2,  0,  1,  2,  2 */
              7,       0,      -3,       0,          /*  0,  1,  0,  2,  2 */
             -7,       0,       0,       0,          /* -2,  1,  1,  0,  0 */
             -7,       0,       3,       0,          /*  0, -1,  0,  2,  2 */
             -7,       0,       3,       0,          /*  2,  0,  0,  2,  1 */
              6,       0,       0,       0,          /*  2,  0,  1,  0,  0 */
              6,       0,      -3,       0,          /* -2,  0,  2,  2,  2 */
              6,       0,      -3,       0,          /* -2,  0,  1,  2,  1 */
             -6,       0,       3,       0,          /*  2,  0, -2,  0,  1 */
             -6,       0,       3,       0,          /*  2,  0,  0,  0,  1 */
              5,       0,       0,       0,          /*  0, -1,  1,  0,  0 */
             -5,       0,       3,       0,          /* -2, -1,  0,  2,  1 */
             -5,       0,       3,       0,          /* -2,  0,  0,  0,  1 */
             -5,       0,       3,       0,          /*  0,  0,  2,  2,  1 */
              4,       0,       0,       0,          /* -2,  0,  2,  0,  1 */
              4,       0,       0,       0,          /* -2,  1,  0,  2,  1 */
              4,       0,       0,       0,          /*  0,  0,  1, -2,  0 */
             -4,       0,       0,       0,          /* -1,  0,  1,  0,  0 */
             -4,       0,       0,       0,          /* -2,  1,  0,  0,  0 */
             -4,       0,       0,       0,          /*  1,  0,  0,  0,  0 */
              3,       0,       0,       0,          /*  0,  0,  1,  2,  0 */
             -3,       0,       0,       0,          /* -1, -1,  1,  0,  0 */
             -3,       0,       0,       0,          /*  0,  1,  1,  0,  0 */
             -3,       0,       0,       0,          /*  0, -1,  1,  2,  2 */
             -3,       0,       0,       0,          /*  2, -1, -1,  2,  2 */
             -3,       0,       0,       0,          /*  0,  0, -2,  2,  2 */
             -3,       0,       0,       0,          /*  0,  0,  3,  2,  2 */
             -3,       0,       0,       0           /*  2, -1,  0,  2,  2 */
    };

    double deltaPsi, deltaEpsilon, t = (jd - 2451545.0) / 36525.0, t2, t3, to10, dp = 0, de = 0, ang;
    double ta[5];
    int i, j;

    t3 = t * (t2 = t * t);

    /* Calculate angles.  The correspondence between the elements
       of our array and the terms cited in Meeus are:

       ta[0] = D  ta[0] = M  ta[2] = M'  ta[3] = F  ta[4] = \Omega
    */

    ta[0] = dtr(297.850363 + 445267.11148 * t - 0.0019142 * t2 + t3 / 189474.0);
    ta[1] = dtr(357.52772 + 35999.05034 * t - 0.0001603 * t2 - t3 / 300000.0);
    ta[2] = dtr(134.96298 + 477198.867398 * t + 0.0086972 * t2 + t3 / 56250.0);
    ta[3] = dtr(93.27191 + 483202.017538 * t - 0.0036825 * t2 + t3 / 327270);
    ta[4] = dtr(125.04452 - 1934.136261 * t + 0.0020708 * t2 + t3 / 450000.0);

    /* Range reduce the angles in case the sine and cosine functions
       don't do it as accurately or quickly. */

    for(i = 0; i < 5; i++)
        ta[i] = fixangr(ta[i]);

    to10 = t / 10.0;
    for(i = 0; i < 63; i++)
    {
        ang = 0;
        for(j = 0; j < 5; j++)
        {
            if(nutArgMult[(i * 5) + j] != 0)
                ang += nutArgMult[(i * 5) + j] * ta[j];
        }
        dp += (nutArgCoeff[(i * 4) + 0] + nutArgCoeff[(i * 4) + 1] * to10) * sin(ang);
        de += (nutArgCoeff[(i * 4) + 2] + nutArgCoeff[(i * 4) + 3] * to10) * cos(ang);
    }

    /* Return the result, converting from ten thousandths of arc
       seconds to radians in the process. */

    deltaPsi = dp / (3600.0 * 10000.0);
    deltaEpsilon = de / (3600.0 * 10000.0);

    result[0] = deltaPsi;
    result[1] = deltaEpsilon;

    return result;
}

/* DELTAT  --  Determine the difference, in seconds, between
               Dynamical time and Universal time. */

double deltat(int year)
{
    /* Table of observed Delta T values at the beginning of
       even numbered years from 1620 through 2002. */

    static const double deltaTtab[] = {
        121, 112, 103, 95, 88, 82, 77, 72, 68, 63, 60, 56, 53, 51, 48, 46,
        44, 42, 40, 38, 35, 33, 31, 29, 26, 24, 22, 20, 18, 16, 14, 12,
        11, 10, 9, 8, 7, 7, 7, 7, 7, 7, 8, 8, 9, 9, 9, 9, 9, 10, 10, 10,
        10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13,
        13, 14, 14, 14, 14, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16,
        16, 16, 15, 15, 14, 13, 13.1, 12.5, 12.2, 12, 12, 12, 12, 12, 12,
        11.9, 11.6, 11, 10.2, 9.2, 8.2, 7.1, 6.2, 5.6, 5.4, 5.3, 5.4, 5.6,
        5.9, 6.2, 6.5, 6.8, 7.1, 7.3, 7.5, 7.6, 7.7, 7.3, 6.2, 5.2, 2.7,
        1.4, -1.2, -2.8, -3.8, -4.8, -5.5, -5.3, -5.6, -5.7, -5.9, -6,
        -6.3, -6.5, -6.2, -4.7, -2.8, -0.1, 2.6, 5.3, 7.7, 10.4, 13.3, 16,
        18.2, 20.2, 21.1, 22.4, 23.5, 23.8, 24.3, 24, 23.9, 23.9, 23.7,
        24, 24.3, 25.3, 26.2, 27.3, 28.2, 29.1, 30, 30.7, 31.4, 32.2,
        33.1, 34, 35, 36.5, 38.3, 40.2, 42.2, 44.5, 46.5, 48.5, 50.5,
        52.2, 53.8, 54.9, 55.8, 56.9, 58.3, 60, 61.6, 63, 65, 66.6
    };

    double dt, f, t;
    int i;

    if((year >= 1620) && (year <= 2000))
    {
        i = (int)floor((year - 1620) / (double)2);
        f = ((year - 1620) / (double)2) - i;  // Fractional part of year
        dt = deltaTtab[i] + ((deltaTtab[i + 1] - deltaTtab[i]) * f);
    }
    else
    {
        t = (year - 2000) / (double)100;
        if(year < 948)
        {
            dt = 2177 + (497 * t) + (44.1 * t * t);
        }
        else
        {
            dt = 102 + (102 * t) + (25.3 * t * t);
            if((year > 2000) && (year < 2100))
                dt += 0.37 * (year - 2100);
        }
    }

    return dt;
}

/* EQUINOX  --  Determine the Julian Ephemeris Day of an
                equinox or solstice.  The "which" argument
                selects the item to be computed:

                    0   March equinox
                    1   June solstice
                    2   September equinox
                    3   December solstice
*/

double equinox(int year, int which)
{
    // Periodic terms to obtain true time

    static const double EquinoxpTerms[] = {
        485, 324.96,   1934.136,
        203, 337.23,  32964.467,
        199, 342.08,     20.186,
        182,  27.85, 445267.112,
        156,  73.14,  45036.886,
        136, 171.52,  22518.443,
         77, 222.54,  65928.934,
         74, 296.72,   3034.906,
         70, 243.58,   9037.513,
         58, 119.81,  33718.147,
         52, 297.17,    150.678,
         50,  21.02,   2281.226,
         45, 247.54,  29929.562,
         44, 325.15,  31555.956,
         29,  60.93,   4443.417,
         18, 155.12,  67555.328,
         17, 288.79,   4562.452,
         16, 198.04,  62894.029,
         14, 199.76,  31436.921,
         12,  95.39,  14577.848,
         12, 287.11,  31931.756,
         12, 320.81,  34777.259,
          9, 227.73,   1222.114,
          8,  15.45,  16859.074
    };

    static const double JDE0tab1000[4][5] = {
        {1721139.29189, 365242.13740,  0.06134,  0.00111, -0.00071},
        {1721233.25401, 365241.72562, -0.05323,  0.00907,  0.00025},
        {1721325.70455, 365242.49558, -0.11677, -0.00297,  0.00074},
        {1721414.39987, 365242.88257, -0.00769, -0.00933, -0.00006}
    };

    static const double JDE0tab2000[4][5] = {
        {2451623.80984, 365242.37404,  0.05169, -0.00411, -0.00057},
        {2451716.56767, 365241.62603,  0.00325,  0.00888, -0.00030},
        {2451810.21715, 365242.01767, -0.11575,  0.00337,  0.00078},
        {2451900.05952, 365242.74049, -0.06223, -0.00823,  0.00032}
    };

    double deltaL, JDE0, JDE, S, T, W, Y;
    const double (*JDE0tab)[5];
    int i, j;

    /* Initialise terms for mean equinox and solstices. We
       have two sets: one for years prior to 1000 and a second
       for subsequent years. */

    if(year < 1000)
    {
        JDE0tab = JDE0tab1000;
        Y = year / (double)1000;
    }
    else
    {
        JDE0tab = JDE0tab2000;
        Y = (year - 2000) / (double)1000;
    }

    JDE0 =  JDE0tab[which][0] +
           (JDE0tab[which][1] * Y) +
           (JDE0tab[which][2] * Y * Y) +
           (JDE0tab[which][3] * Y * Y * Y) +
           (JDE0tab[which][4] * Y * Y * Y * Y);

    T = (JDE0 - 2451545.0) / 36525;
    W = (35999.373 * T) - 2.47;
    deltaL = 1 + (0.0334 * dcos(W)) + (0.0007 * dcos(2 * W));

    // Sum the periodic terms for time T

    S = 0;
    for(i = 0, j = 0; i < 24; i++)
    {
        S += EquinoxpTerms[j] * dcos(EquinoxpTerms[j + 1] + (EquinoxpTerms[j + 2] * T));
        j += 3;
    }

    JDE = JDE0 + ((S * 0.00001) / deltaL);

    return JDE;
}

/* SUNPOS  --  Position of the Sun.  Please see the comments
               on the return statement at the end of this function
               which describe the array it returns.  We return
               intermediate values because they are useful in a
               variety of other contexts. */

double *sunpos(double jd, double result[])
{
    double T, T2, L0, M, e, C, sunLong, sunAnomaly, sunR,
           Omega, Lambda, epsilon, epsilon0, Alpha, Delta,
           AlphaApp, DeltaApp;

    T = (jd - J2000) / JulianCentury;
    T2 = T * T;
    L0 = 280.46646 + (36000.76983 * T) + (0.0003032 * T2);
    L0 = fixangle(L0);
    M = 357.52911 + (35999.05029 * T) + (-0.0001537 * T2);
    M = fixangle(M);
    e = 0.016708634 + (-0.000042037 * T) + (-0.0000001267 * T2);
    C = ((1.914602 + (-0.004817 * T) + (-0.000014 * T2)) * dsin(M)) + ((0.019993 - (0.000101 * T)) * dsin(2 * M)) + (0.000289 * dsin(3 * M));
    sunLong = L0 + C;
    sunAnomaly = M + C;
    sunR = (1.000001018 * (1 - (e * e))) / (1 + (e * dcos(sunAnomaly)));
    Omega = 125.04 - (1934.136 * T);
    Lambda = sunLong + (-0.00569) + (-0.00478 * dsin(Omega));
    epsilon0 = obliqeq(jd);
    epsilon = epsilon0 + (0.00256 * dcos(Omega));
    Alpha = rtd(atan2(dcos(epsilon0) * dsin(sunLong), dcos(sunLong)));
    Alpha = fixangle(Alpha);
    Delta = rtd(asin(dsin(epsilon0) * dsin(sunLong)));
    AlphaApp = rtd(atan2(dcos(epsilon) * dsin(Lambda), dcos(Lambda)));
    AlphaApp = fixangle(AlphaApp);
    DeltaApp = rtd(asin(dsin(epsilon) * dsin(Lambda)));

    result[0]  = L0;          //  [0] Geometric mean longitude of the Sun
    result[1]  = M;           //  [1] Mean anomaly of the Sun
    result[2]  = e;           //  [2] Eccentricity of the Earth's orbit
    result[3]  = C;           //  [3] Sun's equation of the Centre
    result[4]  = sunLong;     //  [4] Sun's true longitude
    result[5]  = sunAnomaly;  //  [5] Sun's true anomaly
    result[6]  = sunR;        //  [6] Sun's radius vector in AU
    result[7]  = Lambda;      //  [7] Sun's apparent longitude at true equinox of the date
    result[8]  = Alpha;       //  [8] Sun's true right ascension
    result[9]  = Delta;       //  [9] Sun's true declination
    result[10] = AlphaApp;    // [10] Sun's apparent right ascension
    result[11] = DeltaApp;    // [11] Sun's apparent declination

    return result;
}

/* EQUATIONOFTIME  --  Compute equation of time for a given moment.
                       Returns the equation of time as a fraction of
                       a day. */

double equationOfTime(double jd)
{
    double alpha, deltaPsi, E, epsilon, L0, tau;
    double spos[12];
    double nut[2];

    tau = (jd - J2000) / JulianMillennium;
    L0 = 280.4664567 + (360007.6982779 * tau) +
         (0.03032028 * tau * tau) +
         ((tau * tau * tau) / 49931) +
         (-((tau * tau * tau * tau) / 15300)) +
         (-((tau * tau * tau * tau * tau) / 2000000));

    L0 = fixangle(L0);
    alpha = sunpos(jd, spos)[10];
    deltaPsi = nutation(jd, nut)[0];
    epsilon = obliqeq(jd) + nutation(jd, nut)[1];
    E = L0 + (-0.0057183) + (-alpha) + (deltaPsi * dcos(epsilon));
    E = E - 20.0 * (floor(E / 20.0));
    E = E / (24 * 60);

    return E;
}

// /////////////////////////////////         ASTRO          ///////////////////////////////// //
// ****************************************************************************************** //
// /////////////////////////////////   GREGORIAN CALENDAR   ///////////////////////////////// //

static const double GREGORIAN_EPOCH = 1721425.5;

// LEAP_GREGORIAN: Is a given year in the Gregorian calendar a leap year?

int leap_gregorian(int year)
{
    // return ((year % 4) == 0) && (!(((year % 100) == 0) && ((year % 400) != 0)));
    return (year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0));
}

// GREGORIAN_TO_JD:‌ Determine Julian day number from Gregorian calendar date

double gregorian_to_jd(int year, int month, int day)
{
    return (GREGORIAN_EPOCH - 1) + (365 * (year - 1)) + floor((year - 1) / (double)4) +
           (-floor((year - 1) / (double)100)) + floor((year - 1) / (double)400) +
           floor((((367 * month) - 362) / (double)12) +
           ((month <= 2) ? 0 : (leap_gregorian(year) ? -1 : -2)) + day);
}

// JD_TO_GREGORIAN: Calculate Gregorian calendar date from Julian day

void jd_to_gregorian(double jd, int *year, int *month, int *day)
{
    double wjd, depoch, quadricent, dqc, cent, dcent, quad, dquad, yindex, yearday, leapadj;

    wjd = floor(jd - 0.5) + 0.5;
    depoch = wjd - GREGORIAN_EPOCH;
    quadricent = floor(depoch / 146097);
    dqc = mod(depoch, 146097);
    cent = floor(dqc / 36524);
    dcent = mod(dqc, 36524);
    quad = floor(dcent / 1461);
    dquad = mod(dcent, 1461);
    yindex = floor(dquad / 365);

    *year = (int)((quadricent * 400) + (cent * 100) + (quad * 4) + yindex);
    if(!((cent == 4) || (yindex == 4)))
        (*year)++;

    yearday = wjd - gregorian_to_jd(*year, 1, 1);
    leapadj = ((wjd < gregorian_to_jd(*year, 3, 1)) ? 0 : (leap_gregorian(*year) ? 1 : 2));
    *month = (int)floor((((yearday + leapadj) * 12) + 373) / 367);
    *day = (int)(wjd - gregorian_to_jd(*year, *month, 1)) + 1;
}

int *jd_to_gregorian_arr(double jd, int result_ymd[])
{
    jd_to_gregorian(jd, &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

const char *gregorian_month_name(int month)
{
    if(month < 1 || month > 12)
        return NULL;
    static const char *gm_name[] = {"January", "February", "March", "April", "May", "June",
                                    "July", "August", "September", "October", "November", "December"};
    return gm_name[month - 1];
}

int gregorian_month_days(int year, int month)
{
    switch(month)
    {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            return 31;
        case 4:
        case 6:
        case 9:
        case 11:
            return 30;
        case 2:
            break;
        default:
            return -1;  // ERROR: The month must be between 1-12
    }

    return leap_gregorian(year) ? 29 : 28;
}

// /////////////////////////////////   GREGORIAN CALENDAR   ///////////////////////////////// //
// ****************************************************************************************** //
// /////////////////////////////////    PERSIAN CALENDAR    ///////////////////////////////// //

static const double PERSIAN_EPOCH = 1948320.5;

/* TEHRAN_EQUINOX  --  Determine Julian day and fraction of the
                       March equinox at the Tehran meridian in
                       a given Gregorian year. */

double tehran_equinox(int year)
{
    double equJED, equJD, equAPP, equTehran, dtTehran;

    // March equinox in dynamical time
    equJED = equinox(year, 0);

    // Correct for delta T to obtain Universal time
    equJD = equJED - (deltat(year) / (24 * 60 * 60));

    // Apply the equation of time to yield the apparent time at Greenwich
    equAPP = equJD + equationOfTime(equJED);

    /* Finally, we must correct for the constant difference between
       the Greenwich meridian andthe time zone standard for
       Iran Standard time, 52°30' to the East. */

    dtTehran = (52 + (30 / 60.0) + (0 / (60.0 * 60.0))) / 360;
    equTehran = equAPP + dtTehran;

    return equTehran;
}

/* TEHRAN_EQUINOX_JD  --  Calculate Julian day during which the
                          March equinox, reckoned from the Tehran
                          meridian, occurred for a given Gregorian
                          year. */

double tehran_equinox_jd(int year)
{
    return floor(tehran_equinox(year));
}

/* PERSIAN_YEAR  --  Determine the year in the Persian
                     astronomical calendar in which a
                     given Julian day falls.  Returns an
                     array of two elements:

                           [0]  Persian year
                           [1]  Julian day number containing
                                equinox for this year.
*/

double *persian_year(double jd, double result[])
{
    double lasteq, nexteq, adr;
    int guess;
    int y, m, d;

    jd_to_gregorian(jd, &y, &m, &d);
    guess = y - 2;

    lasteq = tehran_equinox_jd(guess);
    while(lasteq > jd)
    {
        guess--;
        lasteq = tehran_equinox_jd(guess);
    }

    nexteq = lasteq - 1;
    while(!((lasteq <= jd) && (jd < nexteq)))
    {
        lasteq = nexteq;
        guess++;
        nexteq = tehran_equinox_jd(guess);
    }
    adr = round((lasteq - PERSIAN_EPOCH) / TropicalYear) + 1;

    result[0] = adr;
    result[1] = lasteq;

    return result;
}

/* PERSIAN_TO_JD  --  Obtain Julian day from a given Persian
                      astronomical calendar date. */

double persian_to_jd(int year, int month, int day)
{
    double equinox, guess, jd;
    double adr[2];

    guess = (PERSIAN_EPOCH - 1) + (TropicalYear * ((year - 1) - 1));
    adr[0] = year - 1;
    adr[1] = 0;

    while(adr[0] < year)
        guess = persian_year(guess, adr)[1] + (TropicalYear + 2);
    equinox = adr[1];

    jd = equinox + ((month <= 7) ? ((month - 1) * 31) : (((month - 1) * 30) + 6)) + (day - 1) + 0.5;
    return jd;
}

/* JD_TO_PERSIAN  --  Calculate date in the Persian astronomical
                      calendar from Julian day. */

void jd_to_persian(double jd, int *year, int *month, int *day)
{
    double equinox, yday;
    double adr[2];

    jd = floor(jd) + 0.5;
    *year = (int)persian_year(jd, adr)[0];
    equinox = adr[1];
    *day = (int)floor((jd - equinox) / 30) + 1;

    yday = (floor(jd) - (persian_to_jd(*year, 1, 1) - 0.5)) + 1;
    *month = (int)((yday <= 186) ? ceil(yday / 31) : ceil((yday - 6) / 30));
    *day = (int)(floor(jd) - (persian_to_jd(*year, *month, 1) - 0.5)) + 1;
}

int *jd_to_persian_arr(double jd, int result_ymd[])
{
    jd_to_persian(jd, &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

/* LEAP_PERSIAN  --  Is a given year a leap year in the Persian
                     astronomical calendar? */

int leap_persian(int year)
{
    return (persian_to_jd(year + 1, 1, 1) - persian_to_jd(year, 1, 1)) > 365;
}

const char *persian_month_name(int month)
{
    if(month < 1 || month > 12)
        return NULL;
    static const char *pm_name[] = {"Farvardin", "Ordibehesht", "Khordad", "Tir", "Mordad", "Shahrivar",
                                    "Mehr", "Aban", "Azar", "Dey", "Bahman", "Esfand"};
    return pm_name[month - 1];
}

int persian_month_days(int year, int month)
{
    switch(month)
    {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            return 31;
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            return 30;
        case 12:
            break;
        default:
            return -1;  // ERROR: The month must be between 1-12
    }

    return leap_persian(year) ? 30 : 29;
}

// PERSIANB_TO_JD: Determine Julian day from Birashk's Persian date

double persianb_to_jd(int year, int month, int day)
{
    double epbase, epyear;

    epbase = year - ((year >= 0) ? 474 : 473);
    epyear = 474 + mod(epbase, 2820);

    return day + ((month <= 7) ? ((month - 1) * 31) : (((month - 1) * 30) + 6)) +
           floor(((epyear * 682) - 110) / 2816) + (epyear - 1) * 365 + floor(epbase / 2820) * 1029983 +
           (PERSIAN_EPOCH - 1);
}

// JD_TO_PERSIANB: Calculate Birashk's Persian date from Julian day

void jd_to_persianb(double jd, int *year, int *month, int *day)
{
    double depoch, cycle, cyear, ycycle, aux1, aux2, yday;

    jd = floor(jd) + 0.5;

    depoch = jd - persianb_to_jd(475, 1, 1);
    cycle = floor(depoch / 1029983);
    cyear = mod(depoch, 1029983);
    if(cyear == 1029982)
    {
        ycycle = 2820;
    }
    else
    {
        aux1 = floor(cyear / 366);
        aux2 = mod(cyear, 366);
        ycycle = floor(((2134 * aux1) + (2816 * aux2) + 2815) / 1028522) + aux1 + 1;
    }

    *year = (int)(ycycle + (2820 * cycle) + 474);
    if(*year <= 0)
        (*year)--;

    yday = (jd - persianb_to_jd(*year, 1, 1)) + 1;
    *month = (int)((yday <= 186) ? ceil(yday / 31) : ceil((yday - 6) / 30));
    *day = (int)(jd - persianb_to_jd(*year, *month, 1)) + 1;
}

int *jd_to_persianb_arr(double jd, int result_ymd[])
{
    jd_to_persianb(jd, &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

// LEAP_PERSIANB: Is a given year a leap year in the Birashk's Persian calendar?

int leap_persianb(int year)
{
    int y1, y2, m1, m2, d1, d2;

    jd_to_gregorian(persianb_to_jd(year, 12, 30), &y1, &m1, &d1);
    jd_to_gregorian(persianb_to_jd((year + 1), 1, 1), &y2, &m2, &d2);

    // return !((d1 == d2) && (m1 == m2) && (y1 == y2));
    return (d1 != d2) || (m1 != m2) || (y1 != y2);
}

int leap_persianb2(int year)
{
    return ((((((year - ((year > 0) ? 474 : 473)) % 2820) + 474) + 38) * 682) % 2816) < 682;
}

const char *persianb_month_name(int month)
{
    return persian_month_name(month);
}

int persianb_month_days(int year, int month)
{
    switch(month)
    {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            return 31;
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            return 30;
        case 12:
            break;
        default:
            return -1;  // ERROR: The month must be between 1-12
    }

    return leap_persianb(year) ? 30 : 29;
}

// /////////////////////////////////    PERSIAN CALENDAR    ///////////////////////////////// //
// ****************************************************************************************** //
// /////////////////////////////////    ISLAMIC CALENDAR    ///////////////////////////////// //

static const double ISLAMIC_EPOCH = 1948439.5;

// LEAP_ISLAMIC: Is a given year a leap year in the Islamic calendar?

int leap_islamic(int year)
{
    // return (((year * 11) + 14) % 30) < 11;  // Negative years -> ERROR
    return mod(((year * 11) + 14), 30) < 11;
}

// ISLAMIC_TO_JD: Determine Julian day from Islamic date

double islamic_to_jd(int year, int month, int day)
{
    return (day + ceil(29.5 * (month - 1)) + (year - 1) * 354 + floor((3 + (11 * year)) / (double)30) + ISLAMIC_EPOCH) - 1;
}

// JD_TO_ISLAMIC: Calculate Islamic date from Julian day

void jd_to_islamic(double jd, int *year, int *month, int *day)
{
    int tm;
    jd = floor(jd) + 0.5;
    *year = (int)floor(((30 * (jd - ISLAMIC_EPOCH)) + 10646) / 10631);
    tm = (int)ceil((jd - (29 + islamic_to_jd(*year, 1, 1))) / 29.5) + 1;
    *month = (tm < 12) ? tm : 12;
    *day = (int)(jd - islamic_to_jd(*year, *month, 1)) + 1;
}

int *jd_to_islamic_arr(double jd, int result_ymd[])
{
    jd_to_islamic(jd, &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

const char *islamic_month_name(int month)
{
    if(month < 1 || month > 12)
        return NULL;
    static const char *im_name[] = {"Muharram", "Safar", "Rabi' al-Awwal", "Rabi' al-Thani", "Jumada al-Awwal", "Jumada al-Thani",
                                    "Rajab", "Sha'ban", "Ramadan", "Shawwal", "Dhu al-Qadah", "Dhu al-Hijjah"};
    return im_name[month - 1];
}

int islamic_month_days(int year, int month)
{
    int y1, y2, m1, m2, d1, d2;

    switch(month)
    {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            jd_to_gregorian(islamic_to_jd(year, month, 30), &y1, &m1, &d1);
            jd_to_gregorian(islamic_to_jd(year, (month + 1), 1), &y2, &m2, &d2);
            break;
        case 12:
            jd_to_gregorian(islamic_to_jd(year, month, 30), &y1, &m1, &d1);
            jd_to_gregorian(islamic_to_jd((year + 1), 1, 1), &y2, &m2, &d2);
            break;
        default:
            return -1;  // ERROR: The month must be between 1-12
    }

    return ((y1 == y2) && (m1 == m2) && (d1 == d2)) ? 29 : 30;
}

// /////////////////////////////////    ISLAMIC CALENDAR    ///////////////////////////////// //
// ****************************************************************************************** //
// /////////////////////////////////    HEBREW CALENDAR     ///////////////////////////////// //

static const double HEBREW_EPOCH = 347995.5;

int hebrew_year_days(int year);

// Is a given Hebrew year a leap year?

int leap_hebrew(int year)
{
    return mod(((year * 7) + 1), 19) < 7;
}

// Test for delay of start of new year and to avoid
// Sunday, Wednesday, and Friday as start of the new year.

int hebrew_delay_1(int year)
{
    double months, days, parts;

    months = floor(((235 * year) - 234) / (double)19);
    parts = 12084 + (13753 * months);
    days = (months * 29) + floor(parts / 25920);

    if(mod((3 * (days + 1)), 7) < 3)
        days++;

    return (int)days;
}

// Check for delay in start of new year due to length of adjacent years

int hebrew_delay_2(int year)
{
    int last, present, next;

    last = hebrew_delay_1(year - 1);
    present = hebrew_delay_1(year);
    next = hebrew_delay_1(year + 1);

    return ((next - present) == 356) ? 2 : (((present - last) == 382) ? 1 : 0);
}

// How many months are there in a Hebrew year (12 = normal, 13 = leap)

int hebrew_year_months(int year)
{
    return leap_hebrew(year) ? 13 : 12;
}

const char *hebrew_month_name(int year, int month)
{
    if(month < 1 || month > 13)
        return NULL;
    static const char *adar_i = "Adar I";
    static const char *hm_name[] = {"Nisan", "Iyar", "Sivan", "Tammuz", "Av", "Elul",
                                    "Tishrei", "Cheshvan", "Kislev", "Tevet", "Shevat", "Adar", "Adar II"};
    if(month == 12 && leap_hebrew(year))
        return adar_i;
    return hm_name[month - 1];
}

// How many days are in a given month of a given year

int hebrew_month_days(int year, int month)
{
    if(month < 1 || month > 13)
        return -1;  // ERROR: The month must be between 1-13

    switch(month)
    {
        case 2:
        case 4:
        case 6:
        case 10:
        case 13:
            return 29;  // First of all, dispose of fixed-length 29 day months
        case 8:
            if(!(mod(hebrew_year_days(year), 10) == 5))
                return 29;  // If it's Cheshvan, days depend on length of year
            break;
        case 9:
            if(mod(hebrew_year_days(year), 10) == 3)
                return 29;  // Similarly, Kislev varies with the length of year
            break;
        case 12:
            if(!leap_hebrew(year))
                return 29;  // If it's not a leap year, Adar has 29 days
            break;
    }

    return 30;  // Nope, it's a 30 day month
}

// HEBREW_TO_JD: Determine Julian day from Hebrew date

double hebrew_to_jd(int year, int month, int day)
{
    double jd;
    int mon, months;

    months = hebrew_year_months(year);
    jd = HEBREW_EPOCH + hebrew_delay_1(year) + hebrew_delay_2(year) + day + 1;

    if(month < 7)
    {
        for(mon = 7; mon <= months; mon++)
            jd += hebrew_month_days(year, mon);
        for(mon = 1; mon < month; mon++)
            jd += hebrew_month_days(year, mon);
    }
    else
    {
        for(mon = 7; mon < month; mon++)
            jd += hebrew_month_days(year, mon);
    }

    return jd;
}

/* JD_TO_HEBREW  --  Convert Julian date to Hebrew date
                     This works by making multiple calls to
                     the inverse function, and is this very
                     slow. */

void jd_to_hebrew(double jd, int *year, int *month, int *day)
{
    int i, count, first;

    jd = floor(jd) + 0.5;
    count = (int)floor(((jd - HEBREW_EPOCH) * 98496.0) / 35975351.0);

    *year = count - 1;
    for(i = count; jd >= hebrew_to_jd(i, 7, 1); i++)
        (*year)++;

    first = (jd < hebrew_to_jd(*year, 1, 1)) ? 7 : 1;
    *month = first;
    for(i = first; jd > hebrew_to_jd(*year, i, hebrew_month_days(*year, i)); i++)
        (*month)++;

    *day = (int)(jd - hebrew_to_jd(*year, *month, 1)) + 1;
}

int *jd_to_hebrew_arr(double jd, int result_ymd[])
{
    jd_to_hebrew(jd, &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

// How many days are in a Hebrew year?

int hebrew_year_days(int year)
{
    return (int)(hebrew_to_jd(year + 1, 7, 1) - hebrew_to_jd(year, 7, 1));
}

// /////////////////////////////////    HEBREW CALENDAR     ///////////////////////////////// //
// ****************************************************************************************** //
// /////////////////////////////////    JULIAN CALENDAR     ///////////////////////////////// //

// static const double JULIAN_EPOCH = 1721423.5;

int leap_julian(int year)
{
    return mod(year, 4) == ((year > 0) ? 0 : 3);
}

// JULIAN_TO_JD: Determine Julian day number from Julian calendar date

double julian_to_jd(int year, int month, int day)
{
    // Adjust negative common era years to the zero-based notation we use.

    if(year < 1)
        year++;

    // Algorithm as given in Meeus, Astronomical Algorithms, Chapter 7, page 61

    if(month <= 2)
    {
        year--;
        month += 12;
    }

    return ((floor((365.25 * (year + 4716))) + floor((30.6001 * (month + 1))) + day) - 1524.5);
}

// JD_TO_JULIAN: Calculate Julian calendar date from Julian day

void jd_to_julian(double jd, int *year, int *month, int *day)
{
    double z, a, b, c, d, e;

    jd += 0.5;
    z = floor(jd);
    a = z;
    b = a + 1524;
    c = floor((b - 122.1) / 365.25);
    d = floor(365.25 * c);
    e = floor((b - d) / 30.6001);

    *month = (int)floor((e < 14) ? (e - 1) : (e - 13));
    *year = (int)floor((*month > 2) ? (c - 4716) : (c - 4715));
    *day = (int)(b - d - floor(30.6001 * e));

    /* If year is less than 1, subtract one to convert from
       a zero based date system to the common era system in
       which the year -1 (1 B.C.E) is followed by year 1 (1 C.E.). */

    if(*year < 1)
        (*year)--;
}

int *jd_to_julian_arr(double jd, int result_ymd[])
{
    jd_to_julian(jd, &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

const char *julian_month_name(int month)
{
    return gregorian_month_name(month);
}

int julian_month_days(int year, int month)
{
    switch(month)
    {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            return 31;
        case 4:
        case 6:
        case 9:
        case 11:
            return 30;
        case 2:
            break;
        default:
            return -1;  // ERROR: The month must be between 1-12
    }

    return leap_julian(year) ? 29 : 28;
}

// /////////////////////////////////    JULIAN CALENDAR     ///////////////////////////////// //
// ****************************************************************************************** //

typedef enum DC_CalendarType {DC_PER, DC_GRE, DC_ISM, DC_HEB, DC_JUL, DC_PER_B} dc_calendar_t;

const char *weekday_str(int i)
{
    static const char *weekdays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    return weekdays[abs(i) % 7];
}

int weekday_jd(double jd)
{
    return jwday(jd);
}

const char *weekday_jd_str(double jd)
{
    return weekday_str(weekday_jd(jd));
}

int weekday_ymd(int year, int month, int day, dc_calendar_t calendar_type)
{
    double jd;
    switch(calendar_type)
    {
        case DC_PER: jd = persian_to_jd(year, month, day); break;
        case DC_ISM: jd = islamic_to_jd(year, month, day); break;
        case DC_HEB: jd = hebrew_to_jd(year, month, day); break;
        case DC_JUL: jd = julian_to_jd(year, month, day); break;
        case DC_PER_B: jd = persianb_to_jd(year, month, day); break;
        default: jd = gregorian_to_jd(year, month, day); break;
    }
    return weekday_jd(jd);
}

const char *weekday_ymd_str(int year, int month, int day, dc_calendar_t calendar_type)
{
    return weekday_str(weekday_ymd(year, month, day, calendar_type));
}

// check_date_ldom() returns:
// 0: Successful
// 1: Error: Select the type of the calendar correctly.
// 2: Error: Enter the year correctly (between -81739 and 213719).
// 3: Error: This calendar has no year zero (0).
// 4: Error: Enter the month correctly (between 1 and 12).
// 5: Error: Enter the month correctly (between 1 and 13).
// 6: Error: This year doesn't have 13 months (in Hebrew calendar).
// 7: Error: Enter the day correctly (greater than 0).
// 8: Error: This month of this year has *last_day_of_month days.
// 9: Error: This month has *last_day_of_month days.

int check_date_ldom(int year, int month, int day, dc_calendar_t calendar_type, int *last_day_of_month)
{
    int hym = 0;

    *last_day_of_month = 0;

    switch(calendar_type)
    {
        case DC_PER:
        case DC_GRE:
        case DC_ISM:
        case DC_HEB:
        case DC_JUL:
        case DC_PER_B:
            break;
        default:
            return 1;  // 1: Error: Select the type of the calendar correctly.
    }

    if(year < -81739 || year > 213719)
        return 2;  // 2: Error: Enter the year correctly (between -81739 and 213719).

    if(year == 0 && (calendar_type == DC_PER_B || calendar_type == DC_JUL))
        return 3;  // 3: Error: This calendar has no year zero (0).

    switch(calendar_type)
    {
        case DC_PER:
        case DC_GRE:
        case DC_ISM:
        case DC_JUL:
        case DC_PER_B:
            if(month < 1 || month > 12)
                return 4;  // 4: Error: Enter the month correctly (between 1 and 12).
            break;
        case DC_HEB:
            hym = hebrew_year_months(year);
            if(month == 13)
            {
                if(month != hym)
                    return 6;  // 6: Error: This year doesn't have 13 months (in Hebrew calendar).
            }
            else if(month < 1 || month > 13)
            {
                if(hym == 13)
                    return 5;  // 5: Error: Enter the month correctly (between 1 and 13).
                else
                    return 4;  // 4: Error: Enter the month correctly (between 1 and 12).
            }
            break;
    }

    if(day < 1)
        return 7;  // 7: Error: Enter the day correctly (greater than 0).

    switch(calendar_type)
    {
        case DC_PER:
            *last_day_of_month = persian_month_days(year, month);
            if(month == 12 && day > *last_day_of_month)
                return 8;  // 8: Error: This month of this year has *last_day_of_month days.
            break;
        case DC_GRE:
            *last_day_of_month = gregorian_month_days(year, month);
            if(month == 2 && day > *last_day_of_month)
                return 8;  // 8: Error: This month of this year has *last_day_of_month days.
            break;
        case DC_ISM:
            *last_day_of_month = islamic_month_days(year, month);
            if(day > *last_day_of_month)
                return 8;  // 8: Error: This month of this year has *last_day_of_month days.
            break;
        case DC_HEB:
            *last_day_of_month = hebrew_month_days(year, month);
            if((month == 8 || month == 9 || month == 12 ) && day > *last_day_of_month)
                return 8;  // 8: Error: This month of this year has *last_day_of_month days.
            break;
        case DC_JUL:
            *last_day_of_month = julian_month_days(year, month);
            if(month == 2 && day > *last_day_of_month)
                return 8;  // 8: Error: This month of this year has *last_day_of_month days.
            break;
        case DC_PER_B:
            *last_day_of_month = persianb_month_days(year, month);
            if(month == 12 && day > *last_day_of_month)
                return 8;  // 8: Error: This month of this year has *last_day_of_month days.
            break;
    }

    if(day > *last_day_of_month)
        return 9;  // 9: Error: This month has *last_day_of_month days.

    return 0;  // 0: Successful
}

int check_date_ldom_str(int year, int month, int day, dc_calendar_t calendar_type, int *last_day_of_month, char **error_str)
{
    int error_code;

    if(!(error_code = check_date_ldom(year, month, day, calendar_type, last_day_of_month)))
    {
        *error_str = NULL;
        return error_code;  // 0: Successful
    }

    const char *error_msg = NULL;
    static const char *e1 = "Error: Select the type of the calendar correctly.";
    static const char *e2 = "Error: Enter the year correctly (between -81739 and 213719).";
    static const char *e3 = "Error: This calendar has no year zero (0).";
    static const char *e4 = "Error: Enter the month correctly (between 1 and 12).";
    static const char *e5 = "Error: Enter the month correctly (between 1 and 13).";
    static const char *e6 = "Error: This year doesn't have 13 months (in Hebrew calendar).";
    static const char *e7 = "Error: Enter the day correctly (greater than 0).";
    static const char *e8 = "Error: This month of this year has dd days.";
    static const char *e9 = "Error: This month has dd days.";

    switch(error_code)
    {
        case 1: error_msg = e1; break;
        case 2: error_msg = e2; break;
        case 3: error_msg = e3; break;
        case 4: error_msg = e4; break;
        case 5: error_msg = e5; break;
        case 6: error_msg = e6; break;
        case 7: error_msg = e7; break;
        case 8: error_msg = e8; break;
        case 9: error_msg = e9; break;
    }

    if(!(*error_str = (char *)malloc((strlen(error_msg) + 1) * sizeof(char))))
        return error_code;

    // strcpy(*error_str, error_msg);
    str_copy_unsafe(*error_str, error_msg);

    if(error_code == 8)
    {
        (*error_str)[35] = '0' + *last_day_of_month / 10;
        (*error_str)[36] = '0' + *last_day_of_month % 10;
    }
    else if(error_code == 9)
    {
        (*error_str)[22] = '0' + *last_day_of_month / 10;
        (*error_str)[23] = '0' + *last_day_of_month % 10;
    }

    return error_code;
}

int check_date_str(int year, int month, int day, dc_calendar_t calendar_type, char **error_str)
{
    int last_day_of_month;
    return check_date_ldom_str(year, month, day, calendar_type, &last_day_of_month, error_str);
}

int check_date(int year, int month, int day, dc_calendar_t calendar_type)
{
    int last_day_of_month;
    return check_date_ldom(year, month, day, calendar_type, &last_day_of_month);
}

const char *norm_leap_str(int is_leap_year)
{
    static const char *nl[] = {"Normal Year", "Leap Year"};
    return nl[!!is_leap_year];
}

// ****************************************************************************************** //

void persian_to_gregorian(int *year, int *month, int *day)
{
    jd_to_gregorian(persian_to_jd(*year, *month, *day), year, month, day);
}

void persian_to_islamic(int *year, int *month, int *day)
{
    jd_to_islamic(persian_to_jd(*year, *month, *day), year, month, day);
}

void persian_to_hebrew(int *year, int *month, int *day)
{
    jd_to_hebrew(persian_to_jd(*year, *month, *day), year, month, day);
}

void persian_to_julian(int *year, int *month, int *day)
{
    jd_to_julian(persian_to_jd(*year, *month, *day), year, month, day);
}

void persian_to_persianb(int *year, int *month, int *day)
{
    jd_to_persianb(persian_to_jd(*year, *month, *day), year, month, day);
}

void gregorian_to_persian(int *year, int *month, int *day)
{
    jd_to_persian(gregorian_to_jd(*year, *month, *day), year, month, day);
}

void gregorian_to_islamic(int *year, int *month, int *day)
{
    jd_to_islamic(gregorian_to_jd(*year, *month, *day), year, month, day);
}

void gregorian_to_hebrew(int *year, int *month, int *day)
{
    jd_to_hebrew(gregorian_to_jd(*year, *month, *day), year, month, day);
}

void gregorian_to_julian(int *year, int *month, int *day)
{
    jd_to_julian(gregorian_to_jd(*year, *month, *day), year, month, day);
}

void gregorian_to_persianb(int *year, int *month, int *day)
{
    jd_to_persianb(gregorian_to_jd(*year, *month, *day), year, month, day);
}

void islamic_to_persian(int *year, int *month, int *day)
{
    jd_to_persian(islamic_to_jd(*year, *month, *day), year, month, day);
}

void islamic_to_gregorian(int *year, int *month, int *day)
{
    jd_to_gregorian(islamic_to_jd(*year, *month, *day), year, month, day);
}

void islamic_to_hebrew(int *year, int *month, int *day)
{
    jd_to_hebrew(islamic_to_jd(*year, *month, *day), year, month, day);
}

void islamic_to_julian(int *year, int *month, int *day)
{
    jd_to_julian(islamic_to_jd(*year, *month, *day), year, month, day);
}

void islamic_to_persianb(int *year, int *month, int *day)
{
    jd_to_persianb(islamic_to_jd(*year, *month, *day), year, month, day);
}

void hebrew_to_persian(int *year, int *month, int *day)
{
    jd_to_persian(hebrew_to_jd(*year, *month, *day), year, month, day);
}

void hebrew_to_gregorian(int *year, int *month, int *day)
{
    jd_to_gregorian(hebrew_to_jd(*year, *month, *day), year, month, day);
}

void hebrew_to_islamic(int *year, int *month, int *day)
{
    jd_to_islamic(hebrew_to_jd(*year, *month, *day), year, month, day);
}

void hebrew_to_julian(int *year, int *month, int *day)
{
    jd_to_julian(hebrew_to_jd(*year, *month, *day), year, month, day);
}

void hebrew_to_persianb(int *year, int *month, int *day)
{
    jd_to_persianb(hebrew_to_jd(*year, *month, *day), year, month, day);
}

void julian_to_persian(int *year, int *month, int *day)
{
    jd_to_persian(julian_to_jd(*year, *month, *day), year, month, day);
}

void julian_to_gregorian(int *year, int *month, int *day)
{
    jd_to_gregorian(julian_to_jd(*year, *month, *day), year, month, day);
}

void julian_to_islamic(int *year, int *month, int *day)
{
    jd_to_islamic(julian_to_jd(*year, *month, *day), year, month, day);
}

void julian_to_hebrew(int *year, int *month, int *day)
{
    jd_to_hebrew(julian_to_jd(*year, *month, *day), year, month, day);
}

void julian_to_persianb(int *year, int *month, int *day)
{
    jd_to_persianb(julian_to_jd(*year, *month, *day), year, month, day);
}

void persianb_to_persian(int *year, int *month, int *day)
{
    jd_to_persian(persianb_to_jd(*year, *month, *day), year, month, day);
}

void persianb_to_gregorian(int *year, int *month, int *day)
{
    jd_to_gregorian(persianb_to_jd(*year, *month, *day), year, month, day);
}

void persianb_to_islamic(int *year, int *month, int *day)
{
    jd_to_islamic(persianb_to_jd(*year, *month, *day), year, month, day);
}

void persianb_to_hebrew(int *year, int *month, int *day)
{
    jd_to_hebrew(persianb_to_jd(*year, *month, *day), year, month, day);
}

void persianb_to_julian(int *year, int *month, int *day)
{
    jd_to_julian(persianb_to_jd(*year, *month, *day), year, month, day);
}

// ****************************************************************************************** //

int *persian_to_gregorian_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_gregorian(persian_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *persian_to_islamic_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_islamic(persian_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *persian_to_hebrew_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_hebrew(persian_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *persian_to_julian_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_julian(persian_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *persian_to_persianb_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_persianb(persian_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *gregorian_to_persian_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_persian(gregorian_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *gregorian_to_islamic_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_islamic(gregorian_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *gregorian_to_hebrew_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_hebrew(gregorian_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *gregorian_to_julian_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_julian(gregorian_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *gregorian_to_persianb_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_persianb(gregorian_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *islamic_to_persian_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_persian(islamic_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *islamic_to_gregorian_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_gregorian(islamic_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *islamic_to_hebrew_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_hebrew(islamic_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *islamic_to_julian_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_julian(islamic_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *islamic_to_persianb_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_persianb(islamic_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *hebrew_to_persian_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_persian(hebrew_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *hebrew_to_gregorian_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_gregorian(hebrew_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *hebrew_to_islamic_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_islamic(hebrew_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *hebrew_to_julian_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_julian(hebrew_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *hebrew_to_persianb_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_persianb(hebrew_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *julian_to_persian_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_persian(julian_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *julian_to_gregorian_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_gregorian(julian_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *julian_to_islamic_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_islamic(julian_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *julian_to_hebrew_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_hebrew(julian_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *julian_to_persianb_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_persianb(julian_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *persianb_to_persian_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_persian(persianb_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *persianb_to_gregorian_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_gregorian(persianb_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *persianb_to_islamic_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_islamic(persianb_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *persianb_to_hebrew_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_hebrew(persianb_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}

int *persianb_to_julian_arr(int year, int month, int day, int result_ymd[])
{
    jd_to_julian(persianb_to_jd(year, month, day), &result_ymd[0], &result_ymd[1], &result_ymd[2]);
    return result_ymd;
}
