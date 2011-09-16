/*** date-core.h -- our universe of dates
 *
 * Copyright (C) 2011 Sebastian Freundt
 *
 * Author:  Sebastian Freundt <freundt@ga-group.nl>
 *
 * This file is part of datetools.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the author nor the names of any contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **/

#if !defined INCLUDED_date_core_h_
#define INCLUDED_date_core_h_

#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#if defined __cplusplus
extern "C" {
#endif	/* __cplusplus */

#if !defined DECLF
# define DECLF	static __attribute__((unused))
# define DEFUN	static
# define INCLUDE_DATE_CORE_IMPL
#elif !defined DEFUN
# define DEFUN
#endif	/* !DECLF */

typedef enum {
	DT_UNK,
	DT_YMD,
	DT_YMCW,
	DT_DAISY,
	DT_BIZDA,
} dt_dtyp_t;

typedef enum {
	DT_DUR_UNK,
	DT_DUR_MD,
	DT_DUR_WD,
	DT_DUR_YM,
} dt_durtyp_t;

#define DT_MIN_YEAR	(0)
#define DT_MAX_YEAR	(4095)

/** ymds
 * ymds are just bcd coded concatenations of 8601 dates */
typedef union {
	uint32_t u;
	struct {
		unsigned int d:5;
		unsigned int m:4;
		unsigned int y:12;
		/* 11 bits left */
	};
} dt_ymd_t;

/** ymcws
 * ymcws are year-month-count-weekday bcd coded. */
typedef union {
	uint32_t u;
	struct {
		unsigned int w:3;
		unsigned int c:3;
		unsigned int m:4;
		unsigned int y:12;
		/* 10 bits left */
	};
} dt_ymcw_t;

/** daysi
 * daisys are days since X, 1917-01-01 here */
typedef uint32_t dt_daisy_t;
#define DT_DAISY_BASE_YEAR	(1917)

/** bizda
 * bizdas is a calendar that counts business days before or after a
 * certain day in the month, mostly ultimo. */
typedef union {
	uint32_t u;
	struct {
		/* key day, use 00 for ultimo */
		unsigned int x:5;
		/* before or after */
		unsigned int ba:1;
		unsigned int bd:5;
		unsigned int m:4;
		unsigned int y:12;
		/* 5 bits left */
	};
} dt_bizda_t;

/**
 * Collection of all date types. */
struct dt_d_s {
	dt_dtyp_t typ;
	union {
		uint32_t u;
		dt_ymd_t ymd;
		dt_ymcw_t ymcw;
		dt_daisy_t daisy;
		dt_bizda_t bizda;
	};
};

/* widely understood notion of weekdays */
typedef enum {
	DT_SUNDAY,
	DT_MONDAY,
	DT_TUESDAY,
	DT_WEDNESDAY,
	DT_THURSDAY,
	DT_FRIDAY,
	DT_SATURDAY,
	DT_MIRACLEDAY
} dt_dow_t;

/** mddur
 * duration type for calendars where 1 year = 12 months and 1 week = 7 days */
typedef union {
	uint32_t u;
	struct {
		int d:16;
		int m:16;
	};
} dt_mddur_t;

/** wddur
 * duration type in weeks and days. */
typedef union {
	uint32_t u;
	struct {
		int d:16;
		int w:16;
	};
} dt_wddur_t;

/** ymdur
 * duration type in years and months. */
typedef union {
	uint32_t u;
	struct {
		int m:16;
		int y:16;
	};
} dt_ymdur_t;

/**
 * Collection of all duration types. */
struct dt_dur_s {
	dt_durtyp_t typ;
	union {
		uint32_t u;
		dt_mddur_t md;
		dt_wddur_t wd;
		dt_ymdur_t ym;
	};
};


/* decls */
/**
 * Like strptime() for our dates.
 * The format characters are _NOT_ compatible with strptime().
 * This is what we support:
 * %F - alias for %Y-%m-%d
 * %Y - year in ymd and ymcw mode
 * %m - month in ymd and ymcw mode
 * %d - day in ymd mode
 * %c - week of the month in ymcw mode
 * %w - numeric weekday in ymcw mode
 *
 * If FMT is NULL the standard format for each calendric system is used,
 * that is:
 * - %Y-%m-%d for YMD dates
 * - %Y-%m-%c-%w for YMCW dates
 * - %Y-%m-%d%u for YMDU dates */
DECLF struct dt_d_s dt_strpd(const char *str, const char *fmt);
/**
 * Like strftime() for our dates */
DECLF size_t
dt_strfd(char *restrict buf, size_t bsz, const char *fmt, struct dt_d_s d);

/**
 * Parse durations as in 1w5d, etc. */
DECLF struct dt_dur_s dt_strpdur(const char *str);

/**
 * Print a duration. */
DECLF size_t
dt_strfdur(char *restrict buf, size_t bsz, struct dt_dur_s this);

/**
 * Like time() but return the current date in the desired format. */
DECLF struct dt_d_s dt_date(dt_dtyp_t outtyp);

/**
 * Convert D to another calendric system, specified by TGTTYP. */
DECLF struct dt_d_s dt_conv(dt_dtyp_t tgttyp, struct dt_d_s d);

/**
 * Get the weekday of a date. */
DECLF dt_dow_t dt_get_wday(struct dt_d_s d);

/**
 * Get the day of the month of a date. */
DECLF int dt_get_mday(struct dt_d_s d);

/**
 * Get the day of the year of a date.
 * This might only be intuitive for YMD dates.  The formal definition
 * is to find a representation of D that lacks the notion of a month,
 * so for YMD dates this would be the sum of the days in the months
 * preceding M and the current day of the month in M.
 * For YMCW dates this will yield the n-th W-day in Y.
 * For calendars without the notion of a year this will return 0. */
DECLF unsigned int dt_get_yday(struct dt_d_s d);

/**
 * Add duration DUR to date D.
 * The result will be in the calendar as specified by TGTTYP, or if
 * DT_UNK is given, the calendar of D will be used. */
DECLF struct dt_d_s
dt_add(struct dt_d_s d, struct dt_dur_s dur);

/**
 * Get duration between D1 and D2.
 * The result will be in the duration type as specified by TGTTYP,
 * the calendar of D1 will be used, e.g. its month-per-year, days-per-week,
 * etc. conventions count.
 * If instead D2 should count, swap D1 and D2 and negate the duration
 * using `dt_neg_dur()'. */
DECLF struct dt_dur_s
dt_diff(struct dt_d_s d1, struct dt_d_s d2);

/**
 * Negate the duration. */
DECLF struct dt_dur_s dt_neg_dur(struct dt_dur_s);


#if defined INCLUDE_DATE_CORE_IMPL
# include "date-core.c"
#endif	/* INCLUDE_DATE_CORE_IMPL */

#if defined __cplusplus
}
#endif	/* __cplusplus */

#endif	/* INCLUDED_date_core_h_ */