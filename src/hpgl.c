/* Copyright (c) 1991 - 1994 Heinz W. Werntges.  All rights reserved.  
   Parts Copyright (c) 1999, 2001, 2002, 2003, 2004 
                       Martin Kroeker All rights reserved.

   Distributed by Free Software Foundation, Inc.

   This file is part of HP2xx.

   HP2xx is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
   to anyone for the consequences of using it or for whether it serves any
   particular purpose or works at all, unless he says so in writing.  Refer
   to the GNU General Public License, Version 2 or later, for full details.

   Everyone is granted permission to copy, modify and redistribute
   HP2xx, but only under the conditions described in the GNU General Public
   License.  A copy of this license is supposed to have been
   given to you along with HP2xx so you can know your rights and
   responsibilities.  It should be in a file named COPYING.  Among other
   things, the copyright notice and this notice must be preserved on all
   copies.

   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

/** HPGL.c: HPGL parser & i/o part of HP2xx (based on D. Donath's "HPtoGF.c")
 **
 ** 91/01/13  V 1.00  HWW  Originating
 ** 91/01/19  V 1.01  HWW  reorganized
 ** 91/01/24  V 1.02  HWW  ESC.-Sequences acknowledged (preliminary!!)
 ** 91/01/29  V 1.03  HWW  Incl. SUN portation
 ** 91/01/31  V 1.04  HWW  Parser: ESC sequences should be skipped now
 ** 91/02/10  V 1.05  HWW  Parser renewed
 ** 91/02/15  V 1.06  HWW  stdlib.h supported
 ** 91/02/19  V 1.07a HWW  parser refined, bugs fixed
 ** 91/06/09  V 1.08  HWW  New options added; some restructuring
 ** 91/06/16  V 1.09  HWW  VGA mode added; some renaming; silent_mode!
 ** 91/06/20  V 1.10  HWW  Rotation added
 ** 91/10/15  V 1.11  HWW  ANSI_C; header files reorganized
 ** 91/10/20  V 1.11a HWW  VAX_C support
 ** 91/10/25  V 1.11b HWW  Support of LT; and LT0; (line type, partial)
 ** 91/11/20  V 1.12  HWW  SPn; support: many changes!
 ** 91/11/21  V 1.12b HWW  First comma in "PA,xxxx,yyyy..." accepted
 ** 91/12/22  V 1.13  HWW  Multiple MOVE compression; "plot_rel", "old_pen"
 ** 92/01/13  V 1.13c HWW  VAX problem with ungetc()/fscanf() fixed; bug fixed
 ** 92/01/15  V 1.13d HWW  "vga" --> "pre"
 ** 92/01/30  V 1.14c HWW  Parser: no need of ';', better portable
 ** 92/02/06  V 1.15a HWW  Parser: AR, AA, CI, read_float() added;
 **			   toupper() removed (MACH problems)
 ** 92/02/19  V 1.16c HWW  LB etc. supported
 ** 92/02/23  V 1.17b HWW  LB etc. improved, PG supported
 ** 92/02/25  V 1.17c HWW  Parser improved: SP, LT, multi-mv suppression
 ** 92/03/01  V 1.17d HWW  Char sizes: debugged
 ** 92/03/03  V 1.17e HWW  LB_Mode introduced
 ** 92/04/15  V 1.17f HWW  Width x Height limit assumed
 ** 92/05/21  V 1.18a HWW  Multiple-file usage
 ** 92/05/28  V 1.19a HWW  XT, YT, TL, SM added
 ** 92/10/20  V 1.20c HWW  More line types added (debugged)
 ** 92/11/08  V 1.20d HWW  Interval of active pages
 ** 92/12/13  V 1.20e HWW  truesize option added
 ** 93/02/10  V 1.21a HWW  Arcs & circles now properly closed;
 **			   Bug fixed: SC does not interfere with last move
 ** 93/03/10  V 1.21b HWW  Bug fixed in LT scanner part
 ** 93/03/22, V 1.21c HWW  HYPOT() workaround for a weird BCC behavior;
 ** 93/04/02		   Line_Generator(): Case *pb==*pa caught
 ** 93/04/13  V 1.22a HWW  UC supported (code by Alois Treindl)
 ** 93/04/25  V 1.22b HWW  LB/PR bug fix by E. Norum included
 ** 93/05/20  V 1.22c HWW  LT1 pattern adjustment (report by E. Frambach)
 ** 93/09/02  V 1.22d HWW  EA (rect) added (by Th. Hiller)
 ** 94/01/01  V 1.22e HWW  Misc. additions suggested by L. Lowe:
 **			    1) PlotCmd_from_tmpfile(): int --> PlotCmd
 **                         2) ES: 2nd parameter now optional
 **                         3) evaluate_HPGL(): center_mode introduced
 ** 94/02/14  V 1.30a HWW  Re-organized; many changes; min/max bug fixed
 ** 99/02/28          MK   IW,CA,CS,SA,SS commands added
 ** 99/04/24          MK   PC,PW commands added
 ** 99/05/10          MK   RO command added (code by rs@daveg.com) 
 ** 99/05/18          MK   partial PE support (by Eugene Doudine)
 ** 99/06/05          MK   PC improvements and fixes
 ** 99/11/30          MK   support for fractional PE; PS/RO fixes
 ** 00/02/06          MK   allow commandline overrides for PC/PW
 ** 00/02/13          MK   DV support (backport from delayed 3.4.prealpha)
 ** 00/02/26          MK   ER,EP,FP,FT,PM,PT,RA,RR,WG commands added
 ** 00/02/27          MK   WU command added
 ** 00/03/02          MK   SC types 1 and 2, more robust handling of PE,
 **                        removed PE_line(), split lines() into file reader
 **                        and common linedrawing backend for PD/PA and PE
 **                        added PJL parser and RTL escape sequences 
 **                        (all these patches provided by Eugene Doudine) 
 ** 00/03/03          MK   convert IW parameters if scaling is in effect 
 ** 00/03/05          MK   AT/RT (arc through three points) added
 ** 01/01/01	      MK   UL added and PW rewritten (Andrew J.Bird)
 ** 		           empty PUPD sequence now draws a small dot
 **			   linedrawing fixed (added two moves when IW was
 **	 		   in effect, ever since IW support was added)
 ** 01/04/01	      MK   BR/BZ added
 ** 01/04/22	      MK   reset PW and RO flags/values on reinitialization
 **			   (Yuri Strelenko)  
 ** 01/12/04	      MK   moved reinitialization of n_unknown and n_unexpected
 **			   from reset_HPGL to init_HPGL so that they are not overwritten
 **			   when a single drawing contains several IN statements
 ** 02/06/02	      AJB  Moved HYPOT macro to hpgl.h - so we can use it in murphy.c
 **/

#define DEBUG_ESC 1
#define DEBUG_ESC_2 
/*
#undef DEBUG_ESC
#undef DEBUG_ESC_2
*/
#include <stdio.h>
#include <stdlib.h>

#ifndef _NO_VCL
#include <unistd.h>
#endif

#include <string.h>
#include <ctype.h>
#include <math.h>
#include "bresnham.h"
#include "hp2xx.h"
#include "chardraw.h"
#include "clip.h"
#include "pendef.h"
#include "lindef.h"
#include "hpgl.h"

#define	ETX		'\003'

/*
#define P1X_default	603.0	
#define P1Y_default	521.0
#define P2X_default	10603.0
#define P2Y_default	7721.0
*/
#define P1X_default	0.0	/* drop margins       */
#define P1Y_default	0.0
/*
#define P2X_default   11880. 
#define P2Y_default   16800.
*/
#define P2X_default   33600.	/* A0 media */
#define P2Y_default   47520.


#ifdef NORINT
#define rint(a) (long)(a+0.5)
#endif

/**
 ** Globals needed in other source files:
 **/
LineType CurrentLineType = LT_solid;
short scale_flag = FALSE;
short record_off = FALSE;
long vec_cntr_w = 0L;
short silent_mode = FALSE;
FILE *td;

HPGL_Pt HP_pos = { 0, 0 };	/* Actual plotter pen position  */
HPGL_Pt P1 = { P1X_default, P1Y_default };	/* Scaling points */
HPGL_Pt P2 = { P2X_default, P2Y_default };
int iwflag = 0;			/*MK */
int mode_vert = 0;
HPGL_Pt C1 = { P1X_default, P1Y_default };	/* Clipping points        */
HPGL_Pt C2 = { P2X_default, P2Y_default };
HPGL_Pt S1 = { P1X_default, P1Y_default };	/* Scaled       */
HPGL_Pt S2 = { P2X_default, P2Y_default };	/* points       */
HPGL_Pt Q = { 1., 1. };		/* Delta-P/Delta-S: Initialized with first SC   */
HPGL_Pt M;			/* maximum coordinates set by PS instruction */
/**
 ** Global from chardraw.c:
 **/
extern TextPar tp;

/**
 ** "Local" globals (I know this is messy...) :
 **/
static float lxmin, lxmax, lymin, lymax, neg_ticklen, pos_ticklen;
static double Diag_P1_P2, pat_pos;
static HPGL_Pt p_last = { M_PI, M_PI };	/* Init. to "impossible" values */

static HPGL_Pt polygons[MAXPOLY];
static short polygon_penstate[MAXPOLY];
static int vertices = -1;
static short polygon_mode = FALSE;
static int filltype = 1;
static float hatchspace = 0.;
static short hatchscale = FALSE;
static float hatchangle = 0.;
static float saved_hatchspace[4] = { 0., 0., 0. , 0.};
static int saved_hatchscale[4] = { FALSE , FALSE, FALSE, FALSE };
static float saved_hatchangle[4] = { 0., 0. ,0., 0.};
static float thickness = 0.;
static short polygon_penup = FALSE;
static HPGL_Pt anchor = { 100000.0, 100000.0 };
static HPGL_Pt polystart = { 0.0, 0.0 };
static float rot_cos, rot_sin;

static short rotate_flag = FALSE;	/* Flags tec external to HP-GL  */
static short ps_flag = FALSE;
static short ac_flag = FALSE;
static short pm1_flag = FALSE;
static double rot_ang = 0.;
static double rot_tmp = 0.;	/* saved RO value for resetting after drawing */
static short mv_flag = FALSE;
static short pg_flag = FALSE;
static short ct_dist = FALSE;
static int   mmsupress= TRUE;
static short fixedcolor = FALSE;
static short fixedwidth = FALSE;
static int first_page = 0;
static int last_page = 0;
static int n_unexpected = 0;
static int n_unknown = 0;
static int page_number = 1;
static long vec_cntr_r = 0L;
static short pen = -1;
static short pens_in_use[NUMPENS];
static short pen_down = FALSE;	/* Internal HP-GL book-keeping: */
static short in_place=FALSE;
static short plot_rel = FALSE;
static short saved_penstate = FALSE;	/* to track penstate over polygon mode */
static short wu_relative = FALSE;
static int again = FALSE;
static char StrTerm = ETX;	/* String terminator char       */
static short StrTermSilent = 1;	/* only terminates, or prints too */
static char *strbuf = NULL;
static unsigned int strbufsize = MAX_LB_LEN + 1;
static char symbol_char = '\0';	/* Char in Symbol Mode (0=off)  */
static unsigned char r_base = 0;
static unsigned char g_base = 0;
static unsigned char b_base = 0;
static unsigned char r_max = 255;
static unsigned char g_max = 255;
static unsigned char b_max = 255;
static int lasterror = 0;
static int initialized = 0;
static int initp1p2 = 0;
	static unsigned char **otmp;

/* Known HPGL commands, ASCII-coded as High-byte/low-byte int's */

#define AA	0x4141
#define AC	0x4143
#define AD      0x4144
#define AF	0x4146
#define AH	0x4148
#define AR	0x4152
#define AT      0x4154
#define BL	0x424C
#define BP      0x4250
#define BR      0x4252
#define BZ      0x425A
#define CA      0x4341		/*MK */
#define CI	0x4349
#define CO	0x434F /*AJB*/
#define CP	0x4350
#define CR	0x4352
#define CS      0x4353		/*MK */
#define CT	0x4354
#define DF	0x4446
#define DI	0x4449
#define DR	0x4452
#define DT	0x4454
#define DV      0x4456
#define EA	0x4541
#define EC	0x4543 /*AJB*/
#define EP      0x4550
#define ER      0x4552
#define ES	0x4553
#define EW      0x4557		/*MK */
#define FP      0x4650
#define FT      0x4654
#define IN	0x494E
#define IP	0x4950
#define IR	0x4952
#define IW      0x4957		/*MK */
#define LA	0x4C41 /*AJB*/
#define LB	0x4C42
#define LO	0x4C4F
#define LT	0x4C54
#define MG      0x4D47
#define NP      0x4E50
#define NR      0x4E52
#define OE	0x4F45
#define OI	0x4F49
#define OP	0x4F50
#define OS	0x4F53
#define OW	0x4F57
#define PA	0x5041
#define PB	0x5042
#define PC      0x5043		/*MK */
#define PD	0x5044
#define PE      0x5045
#define PG	0x5047
#define PM      0x504D
#define PR	0x5052
#define PS      0x5053
#define PT      0x5054
#define PU	0x5055
#define PW	0x5057		/*MK */
#define RA      0x5241
#define RF      0x5246
#define RO	0x524F		/*RS */
#define RR      0x5252
#define RT      0x5254
#define SA      0x5341		/*MK */
#define SC	0x5343
#define SD      0x5344
#define SI	0x5349
#define SL	0x534C
#define SM	0x534D
#define SP	0x5350
#define SR	0x5352
#define SS      0x5353		/*MK */
#define TL	0x544C
#define UC	0x5543
#define UL	0x554C /*AJB*/
#define VS	0x5653
#define WD	0x5744
#define WG      0x5747
#define WU      0x5755
#define XT	0x5854
#define YT	0x5954
static void par_err_exit(int code, int cmd, FILE * hd)
{

	const char *msg;
	char tmpstr[21];
	long position;
	int i;
	
	switch (code) {
	case 0:
		msg = "Illegal parameters";
		lasterror = 3;
		break;
	case 1:
		msg = "Error in first parameter";
		lasterror = 3;
		break;
	case 2:
		msg = "No second parameter";
		lasterror = 2;
		break;
	case 3:
		msg = "No third parameter";
		lasterror = 2;
		break;
	case 4:
		msg = "No fourth parameter";
		lasterror = 2;
		break;
	case 98:
		msg = "sscanf error: corrupted file?";
		lasterror = 7;
		break;
	case 99:
	default:
		msg = "Internal error";
		lasterror = 7;
		break;
	}
	if (!silent_mode) {
	Eprintf("\nError in command %c%c: %s\n", cmd >> 8, cmd & 0xFF,
		msg);
	Eprintf(" @ Cmd %ld\n", vec_cntr_w);
	position=ftell(hd);
	fseek(hd, -10L, SEEK_CUR);
	for (i=0;i<21;i++) tmpstr[i]=fgetc(hd);
	tmpstr[20] = '\0';
	fseek(hd, position, SEEK_SET);	
	Eprintf(" lately read: %s\n", tmpstr);
	}
	
	return;
/*	exit(ERROR);*/
}




static void reset_HPGL(void)
{
	int i;
	pat=0;
	pw[0]=pw[1]=pw[2]=ph[0]=ph[1]=ph[2]=256;
	pw[3]=pw[4]=pw[5]=ph[3]=ph[4]=ph[5]=256;
	pw[6]=pw[7]=ph[6]=ph[7]=256;
		memset(pattern[0],1,256*256);
		memset(pattern[1],1,256*256);
		memset(pattern[2],1,256*256);
		memset(pattern[3],1,256*256);
		memset(pattern[4],1,256*256);
		memset(pattern[5],1,256*256);
		memset(pattern[6],1,256*256);
		memset(pattern[7],1,256*256);
	p_last.x = p_last.y = M_PI;
	pen_down = FALSE;
	plot_rel = FALSE;
	in_place = FALSE;
	pen = -1;
/*  n_unexpected = 0;
  n_unknown = 0;*/
	mv_flag = FALSE;
	wu_relative = FALSE;
	pg_flag = FALSE;
	iwflag = FALSE;
	ps_flag = FALSE;
	ac_flag = FALSE;
	filltype = 1;
	saved_hatchangle[0] = saved_hatchangle[1] = 0.;
	saved_hatchangle[2] = saved_hatchangle[3] = 0.;
	saved_hatchspace[0] = saved_hatchspace[1] = 0.;
	saved_hatchspace[2] = saved_hatchspace[3] = 0.;
	saved_hatchscale[0] = saved_hatchscale[1] = FALSE;
	saved_hatchscale[2] = saved_hatchscale[3] = FALSE;
	ct_dist = FALSE;
	CurrentLineType = LT_solid;

	set_line_style_defaults();
/*  set_line_attr_defaults();*/
	CurrentLineAttr.Join = LAJ_plain_miter;
	CurrentLineAttr.End = LAE_butt;
	CurrentLineAttr.Limit = 5;
	tp->sstrokewidth = tp->astrokewidth = tp->strokewidth = 0.11;
	StrTerm = ETX;
	StrTermSilent = 1;
	if (strbuf == NULL) {
		strbuf = malloc(strbufsize);
		if (strbuf == NULL) {
			fprintf(stderr, "\nNo memory !\n");
			exit(ERROR);
		}
	}
	strbuf[0] = '\0';

	P1.x = P1X_default;
	P1.y = P1Y_default;

	Diag_P1_P2 = /*@-unrecog@ */ HYPOT(P2.x - P1.x, P2.y - P1.y);
	CurrentLinePatLen = 0.04 * Diag_P1_P2;
	pat_pos = 0.0;
	scale_flag = FALSE;
	S1 = P1;
	S2 = P2;
	Q.x = Q.y = 1.0;
	HP_pos.x = HP_pos.y = 0.0;
	neg_ticklen = 0.005;	/* 0.5 %        */
	pos_ticklen = 0.005;
	symbol_char = '\0';
	rot_ang -= rot_tmp;
	rot_tmp = 0.;
	if (rot_ang == 0.)
		rotate_flag = FALSE;
	if (rotate_flag) {
		rot_cos = cos(M_PI * rot_ang / 180.0);
		rot_sin = sin(M_PI * rot_ang / 180.0);
	}
	init_text_par();
	if (fixedcolor == FALSE) {
		set_color_rgb(xxBackground, 255, 255, 255);
		set_color_rgb(xxForeground, 0, 0, 0);
		set_color_rgb(xxRed, 255, 0, 0);
		set_color_rgb(xxGreen, 0, 255, 0);
		set_color_rgb(xxBlue, 0, 0, 255);
		set_color_rgb(xxCyan, 0, 255, 255);
		set_color_rgb(xxMagenta, 255, 0, 255);
		set_color_rgb(xxYellow, 255, 255, 0);
		pt.color[0] = xxBackground;
		pt.color[1] = xxForeground;
		pt.color[2] = xxRed;
		pt.color[3] = xxGreen;
		pt.color[4] = xxBlue;
		pt.color[5] = xxCyan;
		pt.color[6] = xxMagenta;
		pt.color[7] = xxYellow;
	}
	if (fixedwidth == FALSE)
		for (i = 0; i < 8; i++)
			pt.width[i] = 0.1;
	record_off = (first_page > page_number)
	    || ((last_page < page_number) && (last_page > 0));
	lasterror = 0;
	initialized = 1;
	initp1p2 = 1;
}

static void init_HPGL(GEN_PAR * pg, const IN_PAR * pi)
{
/**
 ** Re-init. global var's for multiple-file applications
 **/
	td = pg->td;
	silent_mode = (short) pg->quiet;
	lxmin = pi->x0;
	lymin = pi->y0;
	lxmax = pi->x1;
	lymax = pi->y1;
	fixedcolor = (short) pi->hwcolor;
	fixedwidth = (short) pi->hwsize;
	r_base = g_base = b_base = 0;
	r_max = g_max = b_max = 255;
	tp->truetype= pg->truetype;
/*  pens_in_use = 0; */
	pg->maxpens = 8;
	pg->maxcolor = 1;
	memset(pens_in_use, 0, NUMPENS * sizeof(short));
  /**
   ** Record ON if no page selected (pg->page == 0)!
   **/
	first_page = pi->first_page;	/* May be 0     */
	last_page = pi->last_page;	/* May be 0     */
	record_off = (first_page > page_number)
	    || ((last_page < page_number) && (last_page > 0));

	rot_ang = pi->rotation;
	rotate_flag = (rot_ang != 0.0) ? TRUE : FALSE;
	if (rotate_flag) {
		rot_cos = cos(M_PI * rot_ang / 180.0);
		rot_sin = sin(M_PI * rot_ang / 180.0);
	}
	vec_cntr_r = 0L;
	vec_cntr_w = 0L;
	n_unexpected = 0;
	pg->n_commands = 0;
	n_unknown = 0;

	if (pi->hwlimit.x > 0.)
		P2.x = S2.x = pi->hwlimit.x;
	if (pi->hwlimit.y > 0.)
		P2.y = S2.y = pi->hwlimit.y;

	if (HAS_POLY(pg->xx_mode))
		mmsupress = FALSE;
	else
		mmsupress = TRUE;
			
	reset_HPGL();
}



void User_to_Plotter_coord(const HPGL_Pt * p_usr, HPGL_Pt * p_plot)
/**
 ** 	Utility: Transformation from (scaled) user coordinates
 **	to plotter coordinates
 **/
{
	p_plot->x = P1.x + (p_usr->x - S1.x) * Q.x;
	p_plot->y = P1.y + (p_usr->y - S1.y) * Q.y;
}



static void Plotter_to_User_coord(const HPGL_Pt * p_plot, HPGL_Pt * p_usr)
/**
 ** 	Utility: Transformation from plotter coordinates
 **	to (scaled) user coordinates
 **/
{
	p_usr->x = S1.x + (p_plot->x - P1.x) / Q.x;
	p_usr->y = S1.y + (p_plot->y - P1.y) / Q.y;
}



void PlotCmd_to_tmpfile(PlotCmd cmd)
{
	if (record_off)		/* Wrong page!  */
		return;

	if (!silent_mode)
		switch (vec_cntr_w++) {
		case 0:
			Eprintf("Writing Cmd: ");
			break;
		case 1:
			Eprintf("1 ");
			break;
		case 2:
			Eprintf("2 ");
			break;
		case 5:
			Eprintf("5 ");
			break;
		case 10:
			Eprintf("10 ");
			break;
		case 20:
			Eprintf("20 ");
			break;
		case 50:
			Eprintf("50 ");
			break;
		case 100:
			Eprintf("100 ");
			break;
		case 200:
			Eprintf("200 ");
			break;
		case 500:
			Eprintf("500 ");
			break;
		case 1000:
			Eprintf("1k ");
			break;
		case 2000:
			Eprintf("2k ");
			break;
		case 5000:
			Eprintf("5k ");
			break;
		case 10000:
			Eprintf("10k ");
			break;
		case 20000:
			Eprintf("20k ");
			break;
		case 50000L:
			Eprintf("50k ");
			break;
		case 100000L:
			Eprintf("100k ");
			break;
		case 200000L:
			Eprintf("200k ");
			break;
		case 500000L:
			Eprintf("500k ");
			break;
		case 1000000L:
			Eprintf("1000k ");
			break;
		case 2000000L:
			Eprintf("2000k ");
			break;
		case 3000000L:
			Eprintf("3000k ");
			break;
		case 4000000L:
			Eprintf("4000k ");
			break;
		case 5000000L:
			Eprintf("5000k... ");
			break;
		case 10000000L:
			Eprintf("10000k ");
			break;
		}

	if (fputc((int) cmd, td) == EOF) {
		PError("PlotCmd_to_tmpfile");
		Eprintf("Error @ Cmd %ld\n", vec_cntr_w);
		exit(ERROR);
	}
}




void HPGL_Pt_to_tmpfile(const HPGL_Pt * pf)
{
	if (record_off)		/* Wrong page!  */
		return;

	if (fwrite((VOID *) pf, sizeof(*pf), 1, td) != 1) {
		PError("HPGL_Pt_to_tmpfile");
		Eprintf("Error @ Cmd %ld\n", vec_cntr_w);
		exit(ERROR);
	}
	lxmin = MIN(pf->x, lxmin);
	lymin = MIN(pf->y, lymin);
	lxmax = MAX(pf->x, lxmax);
	lymax = MAX(pf->y, lymax);
}

long position_from_tmpfile(void)
{
return ftell(td);
}

void reposition_tmpfile(long position)
{
fseek(td,position,SEEK_SET);
}

void HPGL_Pt_to_polygon(HPGL_Pt pf)
{
	if (record_off)		/* Wrong page!  */
		return;

	polygons[++vertices] = pf;
	polygon_penstate[vertices]=polygon_penup;

	if (scale_flag) User_to_Plotter_coord(&pf,&pf);
	if (rotate_flag) {
		double tmp = rot_cos * pf.x - rot_sin * pf.y;
		pf.y = rot_sin * pf.x + rot_cos * pf.y;
		pf.x = tmp;
	}
	lxmin = MIN(pf.x, lxmin);
	lymin = MIN(pf.y, lymin);
	lxmax = MAX(pf.x, lxmax);
	lymax = MAX(pf.y, lymax);
}




/**
 **	Low-level vector generation & file I/O
 **/

static void
LPattern_Generator(HPGL_Pt * pa,
		   double dx, double dy,
		   double start_of_pat, double end_of_pat)
/**
 **	Generator of Line type patterns:
 **
 **	pa:		Start point (ptr) of current line segment
 **	dx, dy:		Components of c * (*pb - *pa), c holding
 **				dx^2 + dy^2 = pattern_length^2
 **	start_of_pat:	Fraction of start point within pattern
 **	end_of_pat:	Fraction of end   point within pattern
 **			Valid: 0 <= start_of_pat <= end_of_pat <= 1
 **
 **	A pattern consists of alternating "line"/"point" and "gap" elements,
 **	always starting with a line/point. A point is a line of zero length.
 **	The table below contains the relative lengths of the elements
 **	of all line types except LT0; and LT; (7), which are treated separately.
 **	These lengths always add up to 1. A negative value terminates a pattern.
 **/
{
	double length_of_ele, start_of_action, end_of_action;
	static double *p_cur_pat;

	p_cur_pat = lt[(LT_MIN * -1) + (int) CurrentLinePattern];	/* was CurrentLineType */

	if (CurrentLineType == LT_adaptive)
		for (;;) {
			length_of_ele = (double) *p_cur_pat++ / 100;	/* Line or point        */
			if (length_of_ele < 0.)
				return;
			if (length_of_ele < 1.e-5)
				PlotCmd_to_tmpfile(PLOT_AT);
			else
				PlotCmd_to_tmpfile(DRAW_TO);

			pa->x += dx * length_of_ele;
			pa->y += dy * length_of_ele;
			HPGL_Pt_to_tmpfile(pa);

			length_of_ele = (double) *p_cur_pat++ / 100;	/* Gap        */
			if (length_of_ele < 0.)
				return;
			pa->x += dx * length_of_ele;
			pa->y += dy * length_of_ele;
			PlotCmd_to_tmpfile(MOVE_TO);
			HPGL_Pt_to_tmpfile(pa);
	} else			/* LT_fixed */
		for (end_of_action = 0.0;;) {
	    /**
	     ** Line or point:
	     **/
			start_of_action = end_of_action;
			length_of_ele = (double) *p_cur_pat++ / 100;
			if (length_of_ele < 0.)
				return;

			if (length_of_ele < 1.e-5) {	/* Dot Only */
				PlotCmd_to_tmpfile(PLOT_AT);
				HPGL_Pt_to_tmpfile(pa);
			} else {	/* Line Segment */
				end_of_action += length_of_ele;

				if (end_of_action > start_of_pat) {	/* If anything to do:   */
					if (start_of_pat <= start_of_action) {	/* If start is valid    */
						if (end_of_action <= end_of_pat) {	/* Draw full element    */
							pa->x +=
							    dx *
							    length_of_ele;
							pa->y +=
							    dy *
							    length_of_ele;
							PlotCmd_to_tmpfile
							    (DRAW_TO);
							HPGL_Pt_to_tmpfile
							    (pa);
						} else
							/* End_of_action beyond End_of_pattern:   */
						{	/* --> Draw only first part of element: */
							pa->x +=
							    dx *
							    (end_of_pat -
							     start_of_action);
							pa->y +=
							    dy *
							    (end_of_pat -
							     start_of_action);
							PlotCmd_to_tmpfile
							    (DRAW_TO);
							HPGL_Pt_to_tmpfile
							    (pa);
							return;
						}
					} else
						/* Start_of_action before Start_of_pattern:       */
					{
						if (end_of_action <= end_of_pat) {	/* Draw remainder of element            */
							pa->x +=
							    dx *
							    (end_of_action
							     -
							     start_of_pat);
							pa->y +=
							    dy *
							    (end_of_action
							     -
							     start_of_pat);
							PlotCmd_to_tmpfile
							    (DRAW_TO);
							HPGL_Pt_to_tmpfile
							    (pa);
						} else
							/* End_of_action beyond End_of_pattern:   */
							/* Draw central part of element & leave   */
						{
							if (end_of_pat ==
							    start_of_pat)
								PlotCmd_to_tmpfile
								    (PLOT_AT);
							else
								PlotCmd_to_tmpfile
								    (DRAW_TO);
							pa->x +=
							    dx *
							    (end_of_pat -
							     start_of_pat);
							pa->y +=
							    dy *
							    (end_of_pat -
							     start_of_pat);

							HPGL_Pt_to_tmpfile
							    (pa);
							return;
						}
					}
				}
			}
	    /**
	     ** Gap (analogous to line/point):
	     **/
			start_of_action = end_of_action;
			length_of_ele = (double) *p_cur_pat++ / 100;
			if (length_of_ele < 0)
				return;
			end_of_action += length_of_ele;
			if (end_of_action > start_of_pat) {	/* If anything to do:   */
				if (start_of_pat <= start_of_action) {	/* If start is valid    */
					if (end_of_action <= end_of_pat) {	/* Full gap             */
						pa->x +=
						    dx * length_of_ele;
						pa->y +=
						    dy * length_of_ele;
						PlotCmd_to_tmpfile
						    (MOVE_TO);
						HPGL_Pt_to_tmpfile(pa);
					} else
						/* End_of_action beyond End_of_pattern:   */
					{	/* --> Apply only first part of gap:    */
						pa->x +=
						    dx * (end_of_pat -
							  start_of_action);
						pa->y +=
						    dy * (end_of_pat -
							  start_of_action);
						PlotCmd_to_tmpfile
						    (MOVE_TO);
						HPGL_Pt_to_tmpfile(pa);
						return;
					}
				} else
					/* Start_of_action before Start_of_pattern:       */
				{
					if (end_of_action <= end_of_pat) {	/* Apply remainder of gap               */
						pa->x +=
						    dx * (end_of_action -
							  start_of_pat);
						pa->y +=
						    dy * (end_of_action -
							  start_of_pat);
						PlotCmd_to_tmpfile
						    (MOVE_TO);
						HPGL_Pt_to_tmpfile(pa);
					} else
						/* End_of_action beyond End_of_pattern:   */
						/* Apply central part of gap & leave      */
					{
						if (end_of_pat ==
						    start_of_pat)
							return;	/* A null move  */
						pa->x +=
						    dx * (end_of_pat -
							  start_of_pat);
						pa->y +=
						    dy * (end_of_pat -
							  start_of_pat);
						PlotCmd_to_tmpfile
						    (MOVE_TO);
						HPGL_Pt_to_tmpfile(pa);
						return;
					}
				}
			}
		}
}

static void fillpoly(int fillalg,int filltype,
                       float xscale,float hatchspace,float hatchscale,float hatchangle,
                       int rotate_flag,float rot_ang) 
{
       float ftmp;
       PlotCmd_to_tmpfile(fillalg);
       if ((EOF==fputc(filltype,td))) {
               PError("Writing to temporary file:");
               Eprintf ("Error @ Cmd %ld\n", vec_cntr_w);
               exit (ERROR);
       }
       if (3==filltype || 4==filltype) {
               if (hatchscale) {
                       ftmp=hatchspace*xscale;
               } else {
                       ftmp=hatchspace;
               }
               if (fwrite ((VOID *) &ftmp, sizeof(ftmp), 1, td) != 1) {
                       PError("Writing to temporary file:");
                       Eprintf ("Error @ Cmd %ld\n", vec_cntr_w);
                       exit (ERROR);
               }
               ftmp=hatchangle;
               if (rotate_flag) {
                       ftmp+=rot_ang;
               }
               if (fwrite ((VOID *) &ftmp, sizeof(ftmp), 1, td) != 1) {
                       PError("Writing to temporary file:");
                       Eprintf ("Error @ Cmd %ld\n", vec_cntr_w);
                       exit (ERROR);
               }
       }
}


/**
 ** Rectangles --  by Th. Hiller (hiller@tu-harburg.d400.de)
 **/

static void rect(const GEN_PAR * pg, int relative, int filled, float cur_pensize, HPGL_Pt p)
{
	HPGL_Pt p1;

	if (relative) {		/* Process coordinates */
		p.x += p_last.x;
		p.y += p_last.y;
	}
	if (!filled || HAS_POLY(pg->xx_mode)) {
		if (HAS_POLY(pg->xx_mode)) {
			PlotCmd_to_tmpfile(OP_PBUF);
		}
		p1.x = p_last.x;
		p1.y = p.y;
		Pen_action_to_tmpfile(DRAW_TO, &p1, scale_flag);
		p1.x = p.x;
		p1.y = p.y;
		Pen_action_to_tmpfile(DRAW_TO, &p1, scale_flag);
		p1.x = p.x;
		p1.y = p_last.y;
		Pen_action_to_tmpfile(DRAW_TO, &p1, scale_flag);
		p1.x = p_last.x;
		p1.y = p_last.y;
		Pen_action_to_tmpfile(DRAW_TO, &p1, scale_flag);
		if (HAS_POLY(pg->xx_mode)) {
			PlotCmd_to_tmpfile(CL_PBUF);			
			if (filled) {
			        fillpoly(NZFILL_POLY,filltype,Q.x,
					 hatchspace,hatchscale,hatchangle,rotate_flag,rot_ang);
			} else {
				PlotCmd_to_tmpfile(EDGE_POLY);			
			}
		}
	} else {
		vertices = -1;
		HPGL_Pt_to_polygon(p_last);
		p1.x = p_last.x;
		p1.y = p.y;
		HPGL_Pt_to_polygon(p1);
		HPGL_Pt_to_polygon(p1);
		HPGL_Pt_to_polygon(p);
		HPGL_Pt_to_polygon(p);
		p1.x = p.x;
		p1.y = p_last.y;
		HPGL_Pt_to_polygon(p1);
		HPGL_Pt_to_polygon(p1);
		HPGL_Pt_to_polygon(p_last);
		if (hatchspace == 0.)
			hatchspace = cur_pensize;
		if (filltype < 3 && thickness > 0.)
			hatchspace = thickness;
		if (ac_flag == 0) {	/* not yet initialized */
			anchor.x = P1.x;
			anchor.y = P1.y;
/*	fprintf(stderr,"anchor init to P1\n");*/

			/*      anchor.y=MIN(P1.y,ymin); */
		}
		fill(polygons, vertices, anchor, P2, scale_flag,
		     filltype, hatchspace, hatchangle,cur_pensize,pg->dpi,0);
	}
	Pen_action_to_tmpfile(MOVE_TO, &p_last, scale_flag);
}

static void rects(const GEN_PAR * pg, int relative, int filled, float cur_pensize, FILE * hd)
{
	HPGL_Pt p;
	for (;;) {
		if (read_float(&p.x, hd))	/* No number found */
			return;

		if (read_float(&p.y, hd)) {	/* x without y invalid! */
			par_err_exit(2, EA, hd);
			return;
			}
		rect(pg, relative, filled, cur_pensize, p);
	}
}


/* 
   conditional dot
   
   
   
   
*/
static int cdot(int relative,HPGL_Pt *p,int pd) {
  int outside;
  HPGL_Pt dp,p1;
  float tmp;
  double p1x,p1y;

  if (NULL==p) {
    dp=p_last;
  } else {
    dp=*p;
    if (relative) {
      dp.x += p_last.x;
      dp.y += p_last.y;
    }
  }

if (pt.width[pen] >0.9) return(0);

  /*  fprintf(stderr,"pd=%d mv_flag=%d in_place=%d p_last=(%f,%f) dp=(%f,%f): ",
      pd,mv_flag,in_place,p_last.x,p_last.y,dp.x,dp.y);*/
  if (in_place && !pd) {
    outside = 0;

	p1 = p_last;

    if (scale_flag) 
      User_to_Plotter_coord(&p1, &p1);

    if (rotate_flag) {	
      tmp = rot_cos * p1.x - rot_sin * p1.y;
      p1.y = rot_sin *p1.x + rot_cos * p1.y;
      p1.x = tmp;
    }

p1x=(P1.x + p_last.x -S1.x)*Q.x;
p1y=(P1.y + p_last.y -S1.y)*Q.y;

    if (iwflag) {
      outside =(DtClipLine(C1.x, C1.y, C2.x, C2.y,&p1x,&p1y,&p1x,&p1y) == CLIP_NODRAW);
    }
    if (!outside && !record_off) {
      PlotCmd_to_tmpfile(PLOT_AT);
      HPGL_Pt_to_tmpfile(&p1);
    }
    /*    fprintf(stderr,"DOT (%f,%f) ",p1.x,p1.y);*/
    in_place=FALSE;
  } else {
    if (mv_flag && pd && dp.x==p_last.x && dp.y==p_last.y) {
      in_place=TRUE;
    } else if (dp.x!=p_last.x || dp.y!=p_last.y || !pd) {
      in_place=FALSE;
    }
  }
  /*fprintf(stderr," in_place<-%d,rc<-%d\n",in_place,(dp.x==p_last.x && dp.y==p_last.y));*/
  return (dp.x==p_last.x && dp.y==p_last.y)?1:0;
}


int read_PE_flags(GEN_PAR * pg, int c, FILE * hd, PE_flags * fl)
{
	short old_pen;
	float ftmp;
	int ctmp;
	switch (c) {
	case 183:
	case '7':
		/* seven bit mode */
		fl->sbmode = 1;
		break;

	case 185:
	case '9':
		/* rectangle mode */
		fl->rect = 1;
		fl->up = 1;
		break;

	case 186:
	case ':':
		/* select pen */
		if (EOF == (fl->pen = getc(hd))) {
			par_err_exit(98, PE, hd);
			return(0);
		}
		if (c==10) c=getc(hd);
		old_pen = pen;
		read_PE_coord(fl->pen, hd, fl, &ftmp);
		pen = (short) ftmp;
		if (pen < 0 || (int) pen > pg->maxpens) {
			Eprintf
			    ("\nIllegal pen number %d: replaced by %d\n",
			     pen, pen % pg->maxpens);
			n_unexpected++;
			lasterror=3;
			pen = pen % pg->maxpens;
		}
		if (pen == 0 && pg->mapzero > -1)
			pen = pg->mapzero;
		if (old_pen != pen) {
			if ((fputc(SET_PEN, td) == EOF)
			    || (fputc(pen, td) == EOF)) {
				PError("Writing to temporary file:");
				Eprintf("Error @ Cmd %ld\n", vec_cntr_w);
				exit(ERROR);
			}
		}
		if (pen)
			pens_in_use[pen] = 1;
		pg->maxcolor = MAX(pg->maxcolor, (int) pen);
/*MK */
/*	fprintf(stderr,"PE: pen %d\n",pen);*/
		break;

	case 190:
	case '>':
		/* fractional data */

		if (EOF == (ctmp = getc(hd))) {
			par_err_exit(98, PE, hd);
			return(0);
		}
		if (ctmp==10) ctmp=getc(hd);
		fl->fract = decode_PE_char(ctmp, fl);
		fl->fract =
		    ((fl->fract >> 1) * ((fl->fract & 0x01) ? -1 : 1));
/*      fprintf(stderr,"PE > flag, fract =%d (%d decimals) ",fl->fract, fl->fract/3); */
		break;

	case 188:
	case '<':
		/* pen up */
		fl->up = 1;
		fl->rect = 0;
		break;

	case 189:
	case '=':
		/* abs plotting */

		fl->abs = 1;
		break;
		
	default:
		return (0);
	}
	return (1);
}

int isPEterm(int c, PE_flags * fl)
{
	if ((fl->sbmode) && ((c > 94) || (c < 63)))
		return 1;
	if ((!fl->sbmode) && ((c > 190) || (c < 63)))
		return 1;
	return (0);
}


int decode_PE_char(int c, PE_flags * fl)
{
	if (fl->sbmode) {
		c &= 0x7f;
		return ((c > 94) ? (c - 95) : (c - 63));
	} else {
		return ((c > 190) ? (c - 191) : (c - 63));
	}
}

int read_PE_coord(int c, FILE * hd, PE_flags * fl, float *fv)
{
	long lv = 0;
	int i = 0;
	int shft = (fl->sbmode) ? 5 : 6;

	for (;;) {
	        if (c==10) c=getc(hd); /* skip linebreaks */
		if (c==59) {
		        ungetc(c,hd);
		        return 0; /* semicolon */
		}	
		if (c < 63) {
			if (!i) {	/* avoid endless getc/ungetc loop with broken files */
				Eprintf("error in PE data (code=%d) !\n",c);
				ungetc(c,hd);
				return 0;
			}
			ungetc(c, hd);
			break;
		}
		lv |= ((long) decode_PE_char(c, fl)) << (i * shft);
		i++;
		if (isPEterm(c, fl)) {
			break;
		}
		if (EOF == (c = getc(hd))) {
			par_err_exit(98, PE, hd);
			return(0);
		}
	}
	*fv = (float) (((lv >> 1) * ((lv & 0x01) ? -1 : 1)) << fl->fract);
	return (1);
}


int read_PE_pair(int c, FILE * hd, PE_flags * fl, HPGL_Pt * p)
{
	if (!read_PE_coord(c, hd, fl, &(p->x)))
		return 0;
	if (EOF == (c = getc(hd))) {
		par_err_exit(98, PE, hd);
		return(0);
	}
        if (c==10) c=getc(hd); /* skip linebreaks */
	if (!read_PE_coord(c, hd, fl, &(p->y)))
		return 0;
	return (1);
}




void read_PE(GEN_PAR * pg, FILE * hd)
{
	int c;

	HPGL_Pt p;
	PE_flags fl;

	fl.fract = 0;
	fl.sbmode = 0;
	fl.abs = 0;
	fl.up = 0;
	fl.pen = 0;
	fl.rect = 0;

	for (c = getc(hd); (c != EOF) && (c != ';'); c = getc(hd)) {
	        if (c=='\n') c=getc(hd);
		if (!read_PE_flags(pg, c, hd, &fl)) {
			if (!read_PE_pair(c, hd, &fl, &p))
				continue;
			switch (fl.rect) {
			case 1:
				pen_down = 0;
				line(!fl.abs, p);
				fl.rect = 2;
				break;
			case 2:
				pen_down = 1;
				rect(pg, 1, pg->nofill ? 0 : 1, pt.width[pen],
				     p);
				fl.rect = 1;
				/* should be up when PE ends while */
				/* in PE mode */
				pen_down = 0;
				break;
			default:
				pen_down = (fl.up) ? FALSE : TRUE;
				if (!cdot(!fl.abs,&p,pen_down)) line(!fl.abs, p);
				fl.up = 0;
				break;
			}
			fl.abs = 0;
			tp->CR_point = HP_pos;
		}
	}
}


double ceil_with_tolerance(double x, double tol)
{
	double rounded;

/*    rounded=rint(x);*/
	rounded = (double) (x + 0.5);

	if (fabs(rounded - x) <= tol)
		return (rounded);
	else
		return (ceil(x));
}

static void Line_Generator(HPGL_Pt * pa, const HPGL_Pt * pb, int mv_flag)
{
	double seg_len, dx, dy, quot;
	int n_pat, i;

	dx = pb->x - pa->x;
	dy = pb->y - pa->y;
	seg_len = HYPOT(dx, dy);

	switch (CurrentLineType) {

	case LT_solid:
		if (seg_len < 1.e-8) {
			return;	/* No line to draw ??           */
		}
		PlotCmd_to_tmpfile(DRAW_TO);
		HPGL_Pt_to_tmpfile(pb);
		return;

	case LT_adaptive:
		if (seg_len < 1.e-8) {
			return;	/* No line to draw ??           */
		}
		pat_pos = 0.0;	/* Reset to start-of-pattern    */
		n_pat =
		    (int) ceil_with_tolerance(seg_len / CurrentLinePatLen,
					      CurrentLinePatLen *
					      LT_PATTERN_TOL);
		if (n_pat == 0) {	/* sanity check for segment << pattern length */
			n_pat = 1;
			if (!silent_mode)
				fprintf(stderr,
					"very short pattern run encountered\n");
		}
		dx /= n_pat;
		dy /= n_pat;
		/* Now draw n_pat complete line patterns */
		for (i = 0; i < n_pat; i++)
			LPattern_Generator(pa, dx, dy, 0.0, 1.0);
		return;

	case LT_plot_at:
		PlotCmd_to_tmpfile(PLOT_AT);
		HPGL_Pt_to_tmpfile(pb);
		return;

	case LT_fixed:
		if (seg_len < 1.e-8) {
			return;	/* No line to draw ??           */
		}
		if (mv_flag)	/* Last move ends old line pattern      */
			pat_pos = 0.0;
		quot = seg_len / CurrentLinePatLen;
		dx /= quot;
		dy /= quot;
		while (quot >= 1.0) {
			LPattern_Generator(pa, dx, dy, pat_pos, 1.0);
			quot -= (1.0 - pat_pos);
			pat_pos = 0.0;
		}
		quot += pat_pos;
		if (quot >= 1.0) {
			LPattern_Generator(pa, dx, dy, pat_pos, 1.0);
			quot -= 1.0;
			pat_pos = 0.0;
		}
		if (quot > LT_PATTERN_TOL) {
			LPattern_Generator(pa, dx, dy, pat_pos, quot);
			pat_pos = quot;
		} else {
			PlotCmd_to_tmpfile(MOVE_TO);
			HPGL_Pt_to_tmpfile(pb);
		}
		return;

	default:
		break;
	}

}




void Pen_action_to_tmpfile(PlotCmd cmd, const HPGL_Pt * p, int scaled)
{
	static HPGL_Pt P_last;
	HPGL_Pt P;
	double tmp;

	if (record_off)		/* Wrong page!  */
		return;

	if (scaled)		/* Rescaling    */
		User_to_Plotter_coord(p, &P);
	else
		P = *p;		/* Local copy   */


	HP_pos = P;		/* Actual plotter pos. in plotter coord */
	if (rotate_flag) {	/* hp2xx-specific global rotation       */
		tmp = rot_cos * P.x - rot_sin * P.y;
		P.y = rot_sin * P.x + rot_cos * P.y;
		P.x = tmp;
	}

	/* Extreme values needed for later scaling:    */

	switch (cmd) {
	case MOVE_TO:
		if (!mmsupress) {
			PlotCmd_to_tmpfile(MOVE_TO);
			HPGL_Pt_to_tmpfile(&P);
		}
		mv_flag = TRUE;
		break;

  /**
   ** Multiple-move suppression. In addition,
   ** a move only precedes a draw -- nothing else!
   **/

	case DRAW_TO:
		if (mv_flag && mmsupress) {
			PlotCmd_to_tmpfile(MOVE_TO);
			HPGL_Pt_to_tmpfile(&P_last);
		}
		/* drop through */
	case PLOT_AT:
		Line_Generator(&P_last, &P, mv_flag);
		mv_flag = FALSE;
		break;

	default:
		Eprintf("Illegal Pen Action: %d\n", cmd);
		Eprintf("Error @ Cmd %ld\n", vec_cntr_w);
		exit(ERROR);
	}
	P_last = P;
}





int read_float(float *pnum, FILE * hd)
/**
 ** Main work-horse for parameter input:
 **
 ** Search for next number, skipping white space but return if mnemonic met.
 ** If found, read in number
 **	returns	0 if valid number
 **		1 if command ended
 **		2 if scanf failed (possibly corrupted file)
 **	      EOF if EOF met
 **/
{
	int c;
	char *ptr, numbuf[80];

	for (c = getc(hd);
	     (c != '.') && (c != '+') && (c != '-') && ((c < '0')
							|| (c > '9'));
	     c = getc(hd)) {
		if (c == EOF)	/* Wait for number      */
			return EOF;	/* Should not happen    */
		if (c == ';')
			return 1;	/* Terminator reached   */
		if (((c >= 'A') && (c <= 'Z')) ||
		    ((c >= 'a') && (c <= 'z')) || (c == ESC)) {
			ungetc(c, hd);
			return 1;	/* Next Mnemonic reached */
		}
	}
	/* Number found: Get it */
	ptr = numbuf;
	for (*ptr++ = c, c = getc(hd);
	     ((c >= '0') && (c <= '9')) || (c == '.')|| (c=='\n') ; c = getc(hd))
		if (c!='\n') *ptr++ = c;	/* Read number          */
	*ptr = '\0';
	if (c != EOF)
		ungetc(c, hd);

	if (sscanf(numbuf, "%f", pnum) != 1)
		return 11;	/* Should never happen  */
	return 0;
}




void read_string(char *buf, FILE * hd)
{
	int c;
	unsigned int n;

	for (n = 0, c = getc(hd); (c != EOF) && (c != StrTerm);
	     c = getc(hd)) {
		if (n > strbufsize / 2) {
			strbufsize *= 2;
			strbuf = realloc(strbuf, strbufsize);
			if (strbuf == NULL) {
				fprintf(stderr, "\nNo memory !\n");
				exit(ERROR);
			}
			buf = strbuf + n;
		}
		if (c == '\0')
			continue;	/* ignore \0 */
		if (n++ < strbufsize)
			*buf++ = c;
	}
	if (c != StrTerm || StrTermSilent == 0)
		*buf++ = c;
	*buf = '\0';
}




static void read_symbol_char(FILE * hd)
{
	int c;

	for (c = getc(hd); /* ended by switch{} */ ; c = getc(hd))
		switch (c) {
		case ' ':
		case _HT:
		case _LF:
			break;	/* Skip white space             */
		case _CR:
		case EOF:
		case ';':	/* CR or "term" end symbol mode */
			symbol_char = '\0';
			return;
		default:
			if (c < ' ' || c > '~')
				break;	/* Ignore unprintable chars     */
			else {
				symbol_char = c;
				return;
			}
		}
}




static void read_ESC_HP7550A(FILE * hd)
/*
 * Read & skip HP 7550A control commands (ESC.-Commands)
 */
{
	int c;

	switch (getc(hd)) {
	case EOF:
		n_unexpected++;
		Eprintf("\nUnexpected EOF!\n");
		return;
		break;
	case 'A':
	case 'B':
	case 'E':
	case 'J':
	case 'K':
	case 'L':
	case 'O':
	case 'U':
	case 'Y':
	case '(':
	case ')':
		return;		/* Commands without parameters  */
	case '@':
	case 'H':
	case 'I':
	case 'M':
	case 'N':
	case 'P':
	case 'Q':
	case 'S':
	case 'T':
		do {		/* Search for terminator ':'    */
			c = getc(hd);
		}
		while ((c != ':') && (c != EOF));
		if (c == EOF) {
			n_unexpected++;
			Eprintf("\nUnexpected EOF!\n");
		}
		return;
	default:
		n_unknown++;
		return;
	}
}


static int read_PJL(FILE * hd)
/*
 * a simple PJL parser
 * just reads PJL header and
 * return
 *   TRUE if PJL enters HPGL context,
 *   FALSE  if not
 *  *
 * PJL lines are like this:
 * @PJL[ command][ args][\r]\n
 * (however I've seen some wrong PJL files with \n\r )
 * @PJL must be uppercase, ther rest of string is not
 * case sensitive
 * The last line of a PJL header is like these:
 * @PJL ENTER LANGUAGE = HPGL2
 * @PJL EOJ [NAME = "something"]
 */
{
#define PJLBS 257

	char strbuf[PJLBS];
	int i, j, ov, ctmp, qt, el = 0, nw = 0, rc = -2, nl = 0;
	for (;;) {
		/* read word */
		for (i = ov = qt = 0;; i++) {
			ctmp = getc(hd);
			if (ctmp == ESC) {
				while (ctmp != 'X')
					ctmp = getc(hd);
				ctmp = getc(hd);
			}
			if (PJLBS - 1 == i) {
				if (!silent_mode)
					Eprintf
					    ("PJL buffer overflow, rest of token dropped\n");
				ov = 1;
				strbuf[i] = '\0';
			}
			if (!ov)
				strbuf[i] = (0 == nw
					     || qt) ? ctmp : toupper(ctmp);
			if (EOF == ctmp) {
				if (!ov)
					strbuf[i] = 0;
				break;
			} else if ('=' == ctmp && 0 == i) {
				strbuf[i] = '=';
				strbuf[++i] = '\0';
				ctmp = ' ';
				break;
			} else if (strchr(" \t=", ctmp)) {
				if (!qt) {
					if (!ov)
						strbuf[i] = 0;
					break;
				}
			} else if ('\n' == ctmp || '\r' == ctmp) {
				if (!ov)
					strbuf[i] = 0;
				nl = 1;
				break;
			} else if ('"' == ctmp) {
				qt = !qt;
			}
		}
		/* handle word */
		if (i) {
#ifdef DEBUG_ESC
			Eprintf("word %d: read %d bytes: '%s'\n", nw, i,
				strbuf);
#endif
			if (0 == nw && strcmp(strbuf, "@PJL")) {
				Eprintf
				    ("unexpected end of a PJL header!\n");
				return (TRUE);
			} else if (1 == nw && !strcmp(strbuf, "EOJ")) {
				if (!silent_mode)
					Eprintf("end of a PJL job\n");
				rc = TRUE;
			} else if (1 == nw && !strcmp(strbuf, "ENTER")) {
				el++;
			} else if (2 == nw && 1 == el
				   && !strcmp(strbuf, "LANGUAGE")) {
				el++;
			} else if (3 == nw && 2 == el
				   && !strcmp(strbuf, "=")) {
				el++;
			} else if (4 == nw && 3 == el) {
				if (!silent_mode)
					Eprintf("Entering %s context\n",
						strbuf);
				rc = strncmp(strbuf, "HPGL",
					     4) ? FALSE : TRUE;
			}
			nw++;
		}
		/* read separator */
		for (j = 0; EOF != ctmp; j++) {
			if (!strchr(" \t\n\r", ctmp)) {
				ungetc(ctmp, hd);
				break;
			}
			ctmp = getc(hd);
			if ('\n' == ctmp) {
				nl = 1;
			}
		}
#ifdef DEBUG_ESC
		if (j)
			Eprintf("separator: read %d bytes\n", j);
#endif

		if (nl) {
			nw = el = nl = 0;
			if (-2 != rc)
				return rc;
		}
		if (EOF == ctmp) {
			if (!silent_mode)
				Eprintf("EOF in PJL context\n");
			return (FALSE);
		}
	}
}

static void read_ESC_RTL(FILE * hd, int c1, int hp, GEN_PAR * pg)
/*
 *read and skip ESC% control commands
 */
{
	/*
	 * known escapes:
	 * ESC%-12345X    UEL (Universal Escape Language)
	 *                followed by @PJL..
	 *
	 * ESC%-1B        Enter HPGL/2 context
	 * ESC%0B         -
	 * ESC%1B         -
	 * ESC%2B         -
	 * ESC%3B         -
	 *
	 * ESC%1A         Exit HPGL/2 context
	 * ESC%0A         -
	 * ESC%-1A        -

	 * how a PCL escape looks like:
	 * ESC, lowercase letters and digits, an Upper case letter

	 */
	int c0, c2, ctmp = 0, nf;
	unsigned char ptmp=0;
	static int numplanes=1;
	static int bitsperpixel=8;
	static int planeno=0;
	static int haverow=0,compression=0,rtlgraphics=0;
	static int colormode=0;
        static int sw=1,sh=1,dw=1,dh=1,swb=1;
        static int wr=2,hr=10;
        static double sdw,sdh;
	int position,offset,numbytes,sum;
	static int penr=0,peng=0,penb=0;
int kmax,idx;
static int oidx=0;
unsigned char bit;	
 int k,kk,ival,gray;
float val;
	for (c0 = ESC, c2 = getc(hd), nf = 0;
	     EOF != c2; c0 = c1, c1 = c2, c2 = getc(hd)) {

		if ((ESC == c0) && (c1 == '%')) {
			if ('-' == c2) {
				c2 = getc(hd);
				nf = 1;
			}
			switch (c2) {
			case EOF:
				n_unexpected++;
				Eprintf("\nUnexpected EOF!\n");
				return;
				break;
			case '3':
			case '2':	
			case '1':
			case '0':
				switch (ctmp = getc(hd)) {
				case 'A':

					if (hp && !silent_mode) {
#ifdef DEBUG_ESC
						Eprintf
						    ("leaving HPGL context,");
						Eprintf
						    ("entering PCL context with CAP set to %s\n",(c2=='1')?"current HPGL position":"previous RTL CAP");    
#endif
				        if (!record_off) cdot(0,NULL,0);
#if 0
	for (ctmp = getc(hd); (ctmp == EOF) || (ctmp == ';') || (ctmp ==13) || (ctmp==10); ctmp = getc(hd));
					if (ctmp != ESC) {
					fprintf(stderr,"pagebreak on %d\n",ctmp);
					ungetc(ctmp,hd);
					page_number++;
					pg_flag=TRUE;
					record_off =
					    (first_page > page_number)
					    || ((last_page < page_number)
						&& (last_page > 0));
						hp = FALSE;
						return;
					}else ungetc(ctmp,hd);
#endif
					}
					return;
				case 'B':
#ifdef DEBUG_ESC
					if (!silent_mode && !hp)
						Eprintf
						    ("entering HPGL context\n");
#endif
					return;
				case '2':
					/* check for UEL */
					if (nf && '1' == c2 &&
					    '3' == (c2 = getc(hd)) &&
					    '4' == (c2 = getc(hd)) &&
					    '5' == (c2 = getc(hd))
					    && 'X' == (c2 = getc(hd))) {
#ifdef DEBUG_ESC
						if (!silent_mode)
							Eprintf
							    ("UEL found\n");
#endif
						if (read_PJL(hd)) {
							return;
						} else {
							hp = 0;
							continue;
						}
					} else {
						ungetc(ctmp, hd);
						if (hp)
							return;
					}
					break;
				default:
					if (!silent_mode)
						Eprintf
						    ("unknown escape: ESC%%%s%c%c\n",
						     nf ? "-" : "", c2,
						     ctmp);
				if (rtlgraphics==1) { /* if we are reading RTL data, something went wrong */
				                   /* try to find the start of the next data block */
				                   if (!silent_mode) Eprintf("error in RTL data, skipping to next esc\n");
				while ( (ctmp=fgetc(hd)) != 27){};
				} 
					ungetc(ctmp, hd);
					if (hp)
						return;
				}
				break;
			default:
				if (!silent_mode)
					Eprintf
					    ("unknown escape: ESC%%%s%c",
					     nf ? "-" : "", c2);
				if (rtlgraphics==1){ /* if we are reading RTL data, something went wrong */
				                   /* try to find the start of the next data block */
				if (!silent_mode) Eprintf("error in RTL data, skipping to next esc\n");	
				while ( (c2=fgetc(hd)) != 27) {};
				}
				ungetc(c2, hd);
				if (hp)
					return;
				break;
			}
		}
		if ((ESC == c0) && (c1 == 'E')) {
/*		fprintf(stderr,"Esc+E\n");*/
		ungetc(c2,hd);
		return;
		}
		
		if ( ESC== c0 && c1 == '*' ) {
		        if (c2 == 'l') {
				ctmp=getc(hd);
				fprintf(stderr,"Logical operation mode %d (ignored)\n",ctmp);
				ctmp=getc(hd);
				return;
			}
			if (c2 == 'p') {
				ctmp=getc(hd);
#ifdef DEBUG_ESC
				fprintf(stderr,"%s palette\n",(ctmp==49) ? "Pop" : "Push");
#endif
				ctmp=getc(hd); /* fetch terminator*/
			if (ctmp!=49){ /* load grayscale palette */
#if 1
fprintf(stderr,"generating grayscale palette\n"); 
				for (k=0;k<255;k++){
				PlotCmd_to_tmpfile(DEF_PC);
				gray=rint(0.3*(double)k+0.59*(double)k+0.11*(double)k);
				Pen_Color_to_tmpfile(gray+1, gray, gray, gray);
				}
#endif
			}
				return;
				}
			if (c2 == 'r') {
				ctmp=getc(hd);
				if (ctmp=='C'|| ctmp=='c'){
				if (!silent_mode) Eprintf("end raster graphics, CAP at %f,%f\n",p_last.x,p_last.y);
				rtlgraphics=0;
				return;
				}
				ungetc(ctmp,hd);
				read_float(&val,hd);
				ival=val;
				ctmp=getc(hd);
				if (ctmp=='a' || ctmp=='A'){
					if (!silent_mode) Eprintf("start raster graphics");
					switch(ival) {
						case 0:
						default:
						if (!silent_mode) Eprintf(" at left edge\n");
						wr=lxmax/sw;
						if (sh<=1) sh=sw;
						hr=lymax/sh;
						break;
						case 1:
						if (!silent_mode) Eprintf(" at current position (%f,%f)\n",p_last.x,p_last.y);
						hr=wr=7;
						fprintf(stderr,"lxmax=%f, sw=%d, swb=%d, sdw=%f\n",lxmax,sw,swb,sdw);
						break;
						case 2:
						if (!silent_mode) Eprintf(" scaled, at left edge\n");
						wr=(int)(sdw*40)/sw;
						hr=(int)(sdh*40)/sh;
						break;
						case 3:
						wr=(int)((float)sdw*40.)/(float)sw;
						hr=(int)((float)sdh*40.)/(float)sh;
						if (!silent_mode) Eprintf(" scaled by (%d,%d), at current position (%f,%f)\n",wr,hr,p_last.x,p_last.y);
			PlotCmd_to_tmpfile(DEF_PW);
			Pen_Width_to_tmpfile(pen, 0.025);
						break;
						}
					rtlgraphics=1;	
#if 1
fprintf(stderr,"generating grayscale palette\n"); 
				for (k=0;k<255;k++){
				PlotCmd_to_tmpfile(DEF_PC);
				gray=rint(0.3*(double)k+0.59*(double)k+0.11*(double)k);
				Pen_Color_to_tmpfile(gray+1, 0, 0, 0);
				}
#endif
					return;
				}	
				if (ctmp=='u' || ctmp=='U'){
					fprintf(stderr,"simple color mode, palette %d\n",(int)val);
					bitsperpixel=1;
					if (val==-4) { 
						int i;
						numplanes=4;
						pg->is_color = TRUE;
						pg->maxpens=pg->maxcolor=15;
						for (i = 0; i < 16; i++){
						pens_in_use[i] = 1;
			PlotCmd_to_tmpfile(DEF_PW);
			Pen_Width_to_tmpfile(i, 0.025);
				}
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(0, 255, 255, 255);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(1, 0, 0, 0);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(2, 0, 255, 255);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(3, 0, 0, 0);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(4, 255, 0, 255);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(5, 0, 0, 0);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(6, 0, 0, 255);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(7, 0, 0, 0);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(8, 255, 255, 0);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(9, 0, 0, 0);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(10, 0, 255, 0);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(11, 0, 0, 0);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(12, 255, 0, 0);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(13, 0, 0, 0);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(14, 0, 0, 0);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(15, 0, 0, 0);
						return;
					}
					if (fabs(val)==3.)numplanes=3;
					if (val==-3) { 
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(0, 255, 255, 255);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(1, 0, 255, 255);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(2, 255, 0, 255);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(3, 0, 0, 255);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(4, 255, 255, 0);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(5, 0, 255, 0);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(6, 255, 0, 0);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(7, 0, 0, 0);
						return;
					}
					if (val== 3) { 
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(0, 0, 0, 0);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(1, 255, 0, 0);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(2, 0, 255, 0);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(3, 255, 255, 0);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(4, 0, 0, 255);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(5, 255, 0, 255);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(6, 0, 255, 255);
						PlotCmd_to_tmpfile(DEF_PC);
						Pen_Color_to_tmpfile(7, 255, 255, 255);
						return;
					}
				        numplanes = 1;	
					PlotCmd_to_tmpfile(DEF_PC);
					Pen_Color_to_tmpfile(0, 255, 255, 255);
					PlotCmd_to_tmpfile(DEF_PC);
					Pen_Color_to_tmpfile(1, 0, 0, 0);
					return;
				}
#ifdef DEBUG_ESC				
				fprintf(stderr,"source raster dimensions (pixel):\n");
#endif
				if (ctmp=='s' || ctmp=='S'){
					sw=(int)val;
					swb=ceil(sw*bitsperpixel/8.);
					otmp=(unsigned char**)malloc(4*sizeof(unsigned char*));
					otmp[0]=otmp[1]=otmp[2]=otmp[3]=NULL;
					otmp[0]=realloc(otmp[0],swb*sizeof(unsigned char));
					otmp[1]=realloc(otmp[1],swb*sizeof(unsigned char));
					otmp[2]=realloc(otmp[2],swb*sizeof(unsigned char));
					otmp[3]=realloc(otmp[3],swb*sizeof(unsigned char));
					memset(otmp[0],0,swb);
					memset(otmp[1],0,swb);
					memset(otmp[2],0,swb);
					memset(otmp[3],0,swb);
#ifdef DEBUG_ESC
					fprintf(stderr,"width1: %d\n",(int)val);	
#endif
				}
				else if (ctmp=='t'|| ctmp=='T'){
					sh=(int)val;
#ifdef DEBUG_ESC					
					fprintf(stderr,"height1: %d\n",(int)val);	
#endif
				}
				
				if (ctmp=='S' || ctmp=='T') {
				return;
				}
				read_float(&val,hd);
				ctmp=getc(hd);
fprintf(stderr,"??? read %f #%c#\n",val,ctmp);
				if (ctmp=='S'){
					sw=(int)val;
					swb=ceil(sw*bitsperpixel/8.);
					fprintf(stderr,"allocating %d bytes for seed row\n",swb);
					otmp=(unsigned char**)malloc(4*sizeof(unsigned char*));
					otmp[0]=otmp[1]=otmp[2]=otmp[3]=NULL;
					otmp[0]=realloc(otmp[0],swb*sizeof(unsigned char));
					otmp[1]=realloc(otmp[1],swb*sizeof(unsigned char));
					otmp[2]=realloc(otmp[2],swb*sizeof(unsigned char));
					otmp[3]=realloc(otmp[3],swb*sizeof(unsigned char));
					memset(otmp[0],0,swb);
					memset(otmp[1],0,swb);
					memset(otmp[2],0,swb);
					memset(otmp[3],0,swb);
#ifdef DEBUG_ESC
					fprintf(stderr,"width2: %d\n",(int)val);	
#endif
				return;
				}
				else if (ctmp=='T'){
					sh=(int)val;
#ifdef DEBUG_ESC					
					fprintf(stderr,"height2: %d\n",(int)val);	
#endif
				return;
				}
				else if (ctmp=='A') {
					fprintf(stderr,"start raster graphics in mode %d\n",(int)val);
				rtlgraphics=1;
				}
				return;				 
			}
			if (c2 == 't') {
				ungetc(ctmp,hd);
				for (;;) {
				read_float(&val,hd);
				ival=val;
				ctmp=getc(hd);
				if (ctmp=='h' || ctmp=='H'){ 
#ifdef DEBUG_ESC
				fprintf(stderr,"destination raster dimensions (1/720 in):\n");
#endif
					dw=ival;
					sdw=(double)dw/720.*25.4; 
#ifdef DEBUG_ESC					
					fprintf(stderr,"width: %d dpt (%f mm, %d pixel)\n",dw,sdw,(int)(sdw*40));	
#endif
				}
				else if (ctmp=='v'|| ctmp=='V'){
#ifdef DEBUG_ESC
				fprintf(stderr,"destination raster dimensions (1/720 in):\n");
#endif
					dh=ival;
					sdh=(double)dh/720.*25.4;
#ifdef DEBUG_ESC					
					fprintf(stderr,"height: %d dpt (%f mm, %d pixel)\n",dh,sdh,(int)(sdh*40));	
#endif
				} 
				else if (ctmp=='r'|| ctmp=='R') {
#ifdef DEBUG_ESC
				fprintf(stderr,"set graphics resolution :%4.0f dpi\n",val);
#endif
				}				
				else if (ctmp=='j'|| ctmp=='J') {
#ifdef DEBUG_ESC
				fprintf(stderr,"dither mode %d (ignored)\n",(int)val);
#endif
				}				
				if (ctmp=='H' || ctmp=='V'|| ctmp == 'R' || ctmp == 'J') return;
				}
			}
			if (c2 == 'v') {
				ungetc(ctmp,hd);
				for (;;) {
				read_float(&val,hd);
				ival=val;
				ctmp=getc(hd); 
				if (ctmp=='N'||ctmp=='n') {
					if (!silent_mode) Eprintf("ignoring source transparency mode %d\n",ival);
					return;
				}
				if (ctmp=='O'||ctmp=='o') {
					if (!silent_mode) Eprintf("ignoring pattern transparency mode %d\n",ival);
					return;
				}
				if (ctmp=='A'||ctmp=='a') {
					penr=ival;
					if (ctmp=='A') return;
				}
				if (ctmp=='b'||ctmp=='B') {
					peng=ival;
					if (ctmp=='B') return;
				}
				if (ctmp=='c'||ctmp=='C') {
					penb=ival;
					if (ctmp=='C') return;
				}
				if (ctmp=='i'||ctmp=='I') {
#if DEBUG_ESC
					if (!silent_mode) Eprintf("set rgb palette entry %d (%d,%d,%d)\n",ival,penr,peng,penb);
#endif
						pg->is_color = TRUE;
					if (pg->maxpens<(int)val)  pg->maxpens=pg->maxcolor=ival;
						pens_in_use[ival] = 1;
					PlotCmd_to_tmpfile(DEF_PC);
					Pen_Color_to_tmpfile(ival, penr,peng,penb);
					penr=peng=penb=0;
					if (ctmp=='I') return;
				}
				if (ctmp=='W') {
				if (val==6) {
					if (!silent_mode) Eprintf("using default 8-bit color ranges\n");
					ctmp=getc(hd);
					if (!silent_mode) Eprintf("color space: %d\n",(int)ctmp);
					ctmp=getc(hd);
					if (!silent_mode) Eprintf("pixel encoding: %d",(int)ctmp);
					colormode=(int)ctmp;
					switch(colormode){
					case 0:
					if (!silent_mode) Eprintf(" (indexed by plane)\n");
					break;
					case 1:
					if (!silent_mode) Eprintf(" (indexed by pixel)\n");
					break;
					case 2:
					if (!silent_mode) Eprintf(" (direct by plane)\n");
					break;
					case 3:
					if (!silent_mode) Eprintf(" (direct by pixel)\n");
					break;
					case 4:
					if (!silent_mode) Eprintf(" (indexed plane-by-plane)\n");
					}
					ctmp=getc(hd);
					if (!silent_mode) Eprintf("bits per index: %d\n",(int)ctmp);
					bitsperpixel=(int)ctmp;
					if (colormode<2) bitsperpixel = 1;
					numplanes=(int)ctmp;
					ctmp=getc(hd);
					if (!silent_mode) Eprintf("red bits: %d\n",(int)ctmp);
					ctmp=getc(hd);
					if (!silent_mode) Eprintf("green bits: %d\n",(int)ctmp);
					ctmp=getc(hd);
					if (!silent_mode) Eprintf("blue bits: %d\n",(int)ctmp);
				} else {
					if (!silent_mode) Eprintf("using explicit color ranges\n");
					ctmp=getc(hd);
					if (!silent_mode) Eprintf("color space: %c\n",ctmp);
					ctmp=getc(hd);
					if (!silent_mode) Eprintf("color space: %c\n",ctmp);
					ctmp=getc(hd);
					if (!silent_mode) Eprintf("pixel encoding: %c(%d)\n",ctmp,(int)ctmp);
					ctmp=getc(hd);
					if (!silent_mode) Eprintf("bits per index: %c\n",ctmp);
					ctmp=getc(hd);
					if (!silent_mode) Eprintf("red bits: %c\n",ctmp);
					ctmp=getc(hd);
					if (!silent_mode) Eprintf("green bits: %c\n",ctmp);
					ctmp=getc(hd);
					if (!silent_mode) Eprintf("blue bits: %c\n",ctmp);
				}
				return;
				}
			     } /*while...*/				 
			}
			if (c2 == 'b') {
			double px;
				ungetc(ctmp,hd);
				read_float(&val,hd);
				ival=val;
				ctmp=getc(hd); 
				if (ctmp=='m' || ctmp=='M') {
				if (!silent_mode) Eprintf("compression: ");
				compression=ival;
				switch(compression){
					case 0:
					default:
					if (!silent_mode) Eprintf(" unencoded by row\n");
					break;
					case 1:
					if (!silent_mode) Eprintf(" RLE by row\n");
					break;
					case 2:
					if (!silent_mode) Eprintf(" TIFF packbits by row\n");
					break;
					case 3:
					if (!silent_mode) Eprintf(" delta by row\n");
					break;
					case 4:
					if (!silent_mode) Eprintf(" unencoded by block\n");
					break;
					case 5:
					if (!silent_mode) Eprintf(" adaptive by block\n");
					break;
					case 6:
					if (!silent_mode) Eprintf(" CCITT group 3 one-dim by block\n");
					break;
					case 7:
					if (!silent_mode) Eprintf(" CCITT group 3 two-dim by block\n");
					break;
					case 8:
					if (!silent_mode) Eprintf(" CCITT group 4 by block\n");
					break;
					case 9:
					if (!silent_mode) Eprintf(" replacement delta row\n");
					break;
					case 10:
					if (!silent_mode) Eprintf(" lossless RGB/KCMY replacement delta\n");
					break;
					}
				if (ctmp=='M') return;

				read_float(&val,hd);
				ival=val;
				ctmp=getc(hd);
				}
				if (ctmp=='Y') {
					if (rot_tmp==90.||rot_tmp==270.) {
						p_last.x+=val; 
					} else	{			 
						p_last.y+=val;
					}
#ifdef DEBUG_ESC
				fprintf(stderr,"CAP moved to (%f,%f)\n",p_last.x,p_last.y);
#endif	
					return;
				}	
				if (ctmp=='V') {
#ifdef DEBUG_ESC_2				
				fprintf(stderr,"data by plane: %d bytes in mode %d\n",(int)val,compression);
#endif
                                position=0;
                                switch (compression) {
				   char count,value;
				   char ctrl;
                                  case 0:
   				      for (k=0;k<ival;k++) otmp[planeno][position++]=getc(hd);
                                    break;
                                 case 1: 
				   kk=0;
				   while (kk<ival) {
				     count=getc(hd);
			             count++;
			             value=getc(hd);
				     kk+=2;
				       for (k=0;k<count;k++) 
					  	otmp[planeno][position++]=value;
				   }
                                   break;
                                 case 2:
				   kk=0;
				   while (kk<ival-1) {
				     ctrl=getc(hd);
				     kk++;
				     if ( (int)ctrl >= 0) {
#ifdef DEBUG_ESC_2
fprintf(stderr,"%d literal bytes (%d of %d control bytes)\n",ctrl+1,kk,(int)val);
#endif
if (kk+ctrl+1>val) { 
ctrl=val-kk-1;
#ifdef DEBUG_ESC_2 
fprintf(stderr,"truncated to %d literal bytes (%d of %d control bytes)\n",ctrl+1,kk+ctrl+1,(int)val);
#endif
}
				       kk+=ctrl+1;
				 	 for (k=0;k<=ctrl;k++) 
						otmp[planeno][position++]=getc(hd);
				     } else {
				       if ((int)ctrl==-128) {
#ifdef DEBUG_ESC_2
fprintf(stderr,"skipped control byte\n");				       
#endif   
				         continue;
				       }
				       ptmp=getc(hd);
				       kk++;
				       ctrl*=-1;
#ifdef DEBUG_ESC_2
fprintf(stderr,"%d copies of %x (%d of %d control bytes)\n",ctrl+1,ptmp,kk,(int)val);
#endif
					 for (k=0;k<=ctrl;k++) 
						otmp[planeno][position++]=ptmp;
				     }
				   }
			           break;
                                    case 3:
				      kk=0;
				      while (kk<ival) { 
					ptmp=getc(hd);
					kk++;
				  	offset = ptmp & 31;
				  	numbytes=(ptmp >> 5)+1;
				  	if (offset == 31) {
				  		ptmp = getc(hd);
				  		kk++;
				  		offset += (int)ptmp;
				  		while (ptmp == 255) {
				  			ptmp = getc(hd);
				  			offset += (int)ptmp;
				  			kk++;
						}
					}	
					position += offset;
					for (k=0;k<numbytes;k++) 
						otmp[planeno][position++]=getc(hd);
					kk+=numbytes;
					}
                                        break;
				     default:
				       Eprintf("Unsupported compression mode %d\n",compression);
				       break;
                		}
				      for (k=position;k<swb;k++)
						otmp[planeno][k]=0;
                                planeno++;
#if 1
				ctmp=fgetc(hd);
				if ((int)ctmp!=27) {
				int nskip=0;
				if (!silent_mode) Eprintf("error in RTL raster data, skipping to next ESC\n");
				while ( (ctmp=fgetc(hd)) != 27) {nskip++;};
				if (!silent_mode) Eprintf("      skipped %d bytes\n",nskip);
				}
				ungetc(ctmp,hd);
#endif
				return;
				}
				if (ctmp=='W') {
#ifdef DEBUG_ESC_2
				  fprintf(stderr,"data by row or block: %d bytes\n",(int)val);
#endif
                                  position=0;
					otmp[0]=realloc(otmp[0],ival*sizeof(unsigned char));
					otmp[1]=realloc(otmp[1],ival*sizeof(unsigned char));
					otmp[2]=realloc(otmp[2],ival*sizeof(unsigned char));
					otmp[3]=realloc(otmp[3],ival*sizeof(unsigned char));
                                  switch(compression) {
			              char count,value;
				      char ctrl;
				      int num;
                                    case 0:
   				      haverow = ival;
				      if ((int)fread(otmp[planeno],1,haverow,hd)!= haverow) fprintf(stderr,"short read!\n");
				      for (k=haverow;k<swb;k++)
					otmp[planeno][k]=0;
				      if (haverow<swb)haverow=swb;
		                      break;
		                   case 1:
				      kk=0;
			              while (kk<ival) {
		                        count=getc(hd);
		                        count++;
					value=getc(hd);
					kk+=2;
					for (k=0;k<count;k++) 
					   otmp[planeno][position++]=value;
				      }	
				      for (k=position;k<swb;k++)
					   otmp[planeno][k]=0;
				      haverow=swb;
                                      break;
                                    case 2:
				      kk=0;
				      while (kk<ival-1) {
		                        ctrl=getc(hd);
		                        kk++;
					if ( (int)ctrl >= 0) {
#ifdef DEBUG_ESC_2
fprintf(stderr,"%d literal bytes (%d of %d bytes)\n",ctrl+1,kk,(int)val);
#endif
if (kk+ctrl+1>val) { 
ctrl=val-kk-1; 
#ifdef DEBUG_ESC_2 
fprintf(stderr,"truncated to %d literal bytes (%d of %d control bytes)\n",ctrl+1,kk+ctrl+1,(int)val);
#endif
}
				  	  kk+=ctrl+1;
					  for (k=0;k<=ctrl;k++) 
						otmp[planeno][position++]=getc(hd);
				        } else {
				          if ((int)ctrl==-128) {
#ifdef DEBUG_ESC_2
fprintf(stderr,"skipped control byte\n");				          
#endif
				          continue;
				          }
				          ptmp=getc(hd);
					  kk++;
				          ctrl*=-1;
#ifdef DEBUG_ESC_2
fprintf(stderr,"%d copies of %x (%d of %d bytes)\n",ctrl+1,ptmp,kk,(int)val);
#endif
					  for (k=0;k<=ctrl;k++) 
						otmp[planeno][position++]=ptmp;
					}
				      }
#if 1
					for (k=position;k<swb;k++)
						otmp[planeno][k]=0;
					haverow=swb;
#else
haverow=position;
#endif
                                      break;
                                    case 3:
				      kk=0;
				      while (kk<ival) { 
					ptmp=getc(hd);
					kk++;
				  	offset = ptmp & 31;
				  	numbytes=(ptmp >> 5)+1;
				  	if (offset == 31) {
				  		ptmp = getc(hd);
				  		kk++;
				  		offset += (int)ptmp;
				  		while (ptmp == 255) {
				  			ptmp = getc(hd);
				  			offset += (int)ptmp;
				  			kk++;
						}
					}	
					position += offset;
					for (k=0;k<numbytes;k++) 
						otmp[planeno][position++]=getc(hd);
					kk+=numbytes;
					}
#if 1
					for (k=position;k<swb;k++)
						otmp[planeno][k]=0;
					haverow=swb;
#else
haverow=position;	
#endif
                                        break;


				     case 4:
				       sum = 0;
				       kk=16777216*getc(hd)+65536*getc(hd)+256*getc(hd)+getc(hd);
				       fprintf(stderr,"unencoded block, %d pixels per row\n",kk);
				       num=(int)ceil((float)kk/(8./bitsperpixel));
				       	numplanes=1;
#if 0
					otmp[0]=realloc(otmp[0],sw*sizeof(unsigned char));
					otmp[1]=realloc(otmp[1],sw*sizeof(unsigned char));
					otmp[2]=realloc(otmp[2],sw*sizeof(unsigned char));
					otmp[3]=realloc(otmp[3],sw*sizeof(unsigned char));
#endif
				       while (sum<(int)val)  {
#ifdef DEBUG_ESC_2
				       fprintf(stderr,"this row: %d bytes in mode %d\n",num,ptmp);
#endif    
				       position=0;
				       planeno=0;
				        	for (k=0;k<(int)num;k++) otmp[planeno][position++]=getc(hd);
						sum+=num;
						haverow=swb;

			if (rot_tmp==90.||rot_tmp==270.) {
				px=p_last.x; /* save start of line */
			} else				 
				px=p_last.y;
kmax=haverow;
{
int l,ll;			
			for (l=0;l<kmax;l++){ /* for each input pixel */  /*sw statt haverow ??*/
				ptmp=otmp[0][l]; /* indexed by pixel */
				
				if (ptmp!=oidx && ptmp!=0) {
#ifdef DEBUG_ESC_2
fprintf(stderr,"SetPen %d\n",ptmp);
#endif
			fputc(SET_PEN,td);
			fputc(ptmp,td);
			oidx=ptmp;
			}
			if (rot_tmp == 90. || rot_tmp==270.)
				p_last.x++;
			else				
				p_last.y--;

			for (ll=0;ll<=wr;ll++) { /*for all output pixels resulting from scaling */

				if (rot_tmp == 90. || rot_tmp==270.)
					p_last.x--;
				else				
					p_last.y++;
					if (ptmp>0) {
			PlotCmd_to_tmpfile(PLOT_AT);
			HPGL_Pt_to_tmpfile(&p_last);
				}
			}
			
			}
		if (rot_tmp==90.||rot_tmp==270.) {
			p_last.x=px; 
			p_last.y+=hr;
		} else	{			 
			p_last.y=px; /* first column of next row */
			p_last.x+=hr; /* one row below previous one */
		}
}		
					}
					haverow=0;

					break;


				     case 5:
				       sum = 0;
				       	numplanes=1;
#if 0
					otmp[0]=realloc(otmp[0],sw*sizeof(unsigned char));
					otmp[1]=realloc(otmp[1],sw*sizeof(unsigned char));
					otmp[2]=realloc(otmp[2],sw*sizeof(unsigned char));
					otmp[3]=realloc(otmp[3],sw*sizeof(unsigned char));
#endif
				       while (sum<(int)val)  {
				       ptmp=getc(hd);
				       num=256*getc(hd);
				       num+=getc(hd);
				       sum+=3;
#ifdef DEBUG_ESC_2
				       fprintf(stderr,"this row: %d bytes in mode %d\n",num,ptmp);
#endif    
				       position=0;
				       planeno=0;
				       switch (ptmp) {
				        case 0:
				        	for (k=0;k<(int)num;k++) otmp[planeno][position++]=getc(hd);
						sum+=num;
						haverow=swb;
					break;
		                   case 1:
                                      kk=0;
                                     while (kk<num) {
#ifdef DEBUG_ESC
					fprintf(stderr,"rle %d bytes per row (%d of %d compressed bytes)\n",num,kk,(int)val);
#endif
		                        count=getc(hd);
		                        count++;
					value=getc(hd);
					kk+=2;
					for (k=0;k<count;k++) 
					   otmp[planeno][position++]=value;
				      }
				      sum+=num;	
				      for (k=position;k<swb;k++)
					   otmp[planeno][k]=0;
				 haverow=swb;
                                      break;

                                    case 2:
                                    planeno=0;
				      kk=0;
				      while (kk<num) {
		                        ctrl=getc(hd);
		                        kk++;
					if ( (int)ctrl >= 0) {
#ifdef DEBUG_ESC_2
fprintf(stderr,"%d literal bytes (%d of %d bytes)\n",ctrl+1,kk,(int)val);
#endif
if (kk+ctrl+1>val) { 
ctrl=val-kk-1; 
#ifdef DEBUG_ESC_2 
fprintf(stderr,"truncated to %d literal bytes (%d of %d control bytes)\n",ctrl+1,kk+ctrl+1,(int)val);
#endif
}
				  	  kk+=ctrl+1;
					  for (k=0;k<=ctrl;k++) 
						otmp[planeno][position++]=getc(hd);
				        } else {
				          if ((int)ctrl==-128) {
#ifdef DEBUG_ESC_2
fprintf(stderr,"skipped control byte\n");				          
#endif
				          continue;
				          }
				          ptmp=getc(hd);
					  kk++;
				          ctrl*=-1;
#ifdef DEBUG_ESC_2
fprintf(stderr,"%d copies of %x (%d of %d bytes)\n",ctrl+1,ptmp,kk,(int)val);
#endif
					  for (k=0;k<=ctrl;k++) 
						otmp[planeno][position++]=ptmp;
					}
				      }
				      sum+=num;
#if 1
					for (k=position;k<swb;k++)
						otmp[planeno][k]=0;
					haverow=swb;
#else
haverow=position;
#endif
                                      break;

                                    case 3:
				      kk=0;
				      while (kk<num) { 
					ptmp=getc(hd);
					kk++;
				  	offset = ptmp & 31;
				  	numbytes=(ptmp >> 5)+1;
				  	if (offset == 31) {
				  		ptmp = getc(hd);
				  		kk++;
				  		offset += (int)ptmp;
				  		while (ptmp == 255) {
				  			ptmp = getc(hd);
				  			offset += (int)ptmp;
				  			kk++;
						}
					}	
					position += offset;
					for (k=0;k<numbytes;k++) 
						otmp[planeno][position++]=getc(hd);
					kk+=numbytes;
					}
					haverow=swb;	
					sum+=num;
                                        break;

					case 4:
					for (k=0;k<swb;k++)
						otmp[planeno][position++]=0;
						haverow=swb;
						sum+=num;
					break;
					case 5:
						haverow=swb;
						sum+=num;
					break;	
					default:
					fprintf(stderr,"skipping %d bytes in unknown/undefined mode %d\n",num,ptmp);
					for (k=0;k<num;k++) ptmp=getc(hd);
					sum+=num;
					break;
					}
			if (rot_tmp==90.||rot_tmp==270.) {
				px=p_last.x; /* save start of line */
			} else				 
				px=p_last.y;
kmax=haverow;
{
int l,ll;			
			for (l=0;l<kmax;l++){ /* for each input pixel */  /*sw statt haverow ??*/
				ptmp=otmp[0][l]; /* indexed by pixel */
				
				if (ptmp!=oidx && ptmp!=0) {
#ifdef DEBUG_ESC_2
fprintf(stderr,"SetPen %d\n",ptmp);
#endif
			fputc(SET_PEN,td);
			fputc(ptmp,td);
			oidx=ptmp;
			}
			if (rot_tmp == 90. || rot_tmp==270.)
				p_last.x++;
			else				
				p_last.y--;

			for (ll=0;ll<=wr;ll++) { /*for all output pixels resulting from scaling */

				if (rot_tmp == 90. || rot_tmp==270.)
					p_last.x--;
				else				
					p_last.y++;
					if (ptmp>0) {
			PlotCmd_to_tmpfile(PLOT_AT);
			HPGL_Pt_to_tmpfile(&p_last);
				}
			}
			
			}
		if (rot_tmp==90.||rot_tmp==270.) {
			p_last.x=px; 
			p_last.y+=hr;
		} else	{			 
			p_last.y=px; /* first column of next row */
			p_last.x+=hr; /* one row below previous one */
		}
}		
					}
					haverow=0;
				       break;				
				       
				     default:
				       Eprintf("Unsupported compression mode %d\n",compression);
				       break;
                		}
                		
                		planeno=0;
				  
			if (rot_tmp==90.||rot_tmp==270.) {
				px=p_last.x; /* save start of line */
			} else				 
				px=p_last.y;
kmax=haverow;
			oidx=0;
			for (k=0;k<kmax;k++){ /* for each input pixel */  /*sw statt haverow ??*/
				int ir,ig,ib, gray;
			if (bitsperpixel==1) {
			   bit=128;
                           for (ib=0;ib<8;ib++) {
                                 idx=0;
                                 if ( (otmp[0][k]&bit) >0) idx+=1;
                                 if (numplanes>1)
                                 if ( (otmp[1][k]&bit) >0) idx+=2;
				 if (numplanes>2)
                                 if ( (otmp[2][k]&bit) >0) idx+=4;
                                 if (numplanes>3)
                                 if ( (otmp[3][k]&bit) >0) idx+=8;

				 if (idx!=oidx) {
				 fputc(SET_PEN,td);
				 fputc(idx,td);
				 oidx=idx;
				 }
                           bit/=2;
			if (rot_tmp == 90. || rot_tmp==270.)
				p_last.x++;
			else				
				p_last.y--;

			for (kk=0;kk<=wr;kk++) { /*for all output pixels resulting from scaling */

				if (rot_tmp == 90. || rot_tmp==270.)
					p_last.x--;
				else				
					p_last.y++;
#if 0
			Pen_action_to_tmpfile(MOVE_TO, &p_last, scale_flag);
			
				if (rot_tmp == 90. || rot_tmp==270.)
					p_last.y+=hr;
				else				
					p_last.x+=hr; /* draw line accoring to vertical scale factor */
	
				Pen_action_to_tmpfile(DRAW_TO, &p_last, scale_flag);

				if (rot_tmp == 90. || rot_tmp==270.)
					p_last.y-=hr;
				else				
					p_last.x-=hr; /* return to baseline of row */
#else
			PlotCmd_to_tmpfile(PLOT_AT);
			HPGL_Pt_to_tmpfile(&p_last);
#endif			
			} /* for all output pixels */
		   }	
              } else {		
			if (colormode==1) {
				gray=otmp[0][k]; /* indexed by pixel */
					 
			} else if (colormode==3) { /* direct by-pixel - all bits in one row */
				ir=otmp[0][k];
				ig=otmp[0][k+1];
				ib=otmp[0][k+2];
			gray=rint(0.3*ir+0.59*ig+0.11*ib);
			}else { /* indexed or direct, by-plane - values split across color planes */
				ir=otmp[0][k];
				ig=otmp[1][k];
				ib=otmp[2][k];
			gray=rint(0.3*ir+0.59*ig+0.11*ib);
			}
/*			if (gray==0) gray=254;*/
if (gray==0) continue;
			fputc(SET_PEN,td);
			fputc(gray+1,td);

			if (rot_tmp == 90. || rot_tmp==270.)
				p_last.x++;
			else				
				p_last.y--;

			for (kk=0;kk<=wr;kk++) { /*for all output pixels resulting from scaling */

				if (rot_tmp == 90. || rot_tmp==270.)
					p_last.x--;
				else				
					p_last.y++;
					
			Pen_action_to_tmpfile(MOVE_TO, &p_last, scale_flag);
			
				if (rot_tmp == 90. || rot_tmp==270.)
					p_last.y+=hr;
				else				
					p_last.x+=hr; /* draw line accoring to vertical scale factor */
	
				Pen_action_to_tmpfile(DRAW_TO, &p_last, scale_flag);

				if (rot_tmp == 90. || rot_tmp==270.)
					p_last.y-=hr;
				else				
					p_last.x-=hr; /* return to baseline of row */
			} /* for all output pixels */
			if (colormode==3) 
			  k+=2; /* advance to next input pixel */
		    }
		}

		if (rot_tmp==90.||rot_tmp==270.) {
			p_last.x=px; 
			p_last.y+=hr;
		} else	{			 
			p_last.y=px; /* first column of next row */
			p_last.x+=hr; /* one row below previous one */
		}
#if 1
				ctmp=fgetc(hd);
				if ((int)ctmp!=27) {
				int nskip=0;
				if (!silent_mode) Eprintf("error in RTL raster data, skipping to next ESC\n");
				while ( (ctmp=fgetc(hd)) != 27) {nskip++;};
				if (!silent_mode) Eprintf("     skipped %d bytes\n",nskip);
				}
				ungetc(ctmp,hd);
#endif
#if 0
free(otmp[0]);
free(otmp[1]);
free(otmp[2]);
free(otmp[3]);
otmp[0]=otmp[1]=otmp[2]=otmp[3]=NULL;
#endif	
			fputc(SET_PEN,td);
			fputc(1,td);
	
		return; /* completed this command */
		}
		
		fprintf(stderr,"unknown subtype %c\n",ctmp);
		}
	fprintf(stderr,"unknown graphics command ESC*%c\n",c2);
	}
		if (hp == TRUE && !nf && c1 != '%' && c1 != 'E') {
			ungetc(ctmp, hd);
			if (!silent_mode) {
				if ( c1 == '&' && c2 == 'l' )
				Eprintf("ignoring escape ESC&l... (paper size/orientation/...)\n");
				else 
				if ( c1 == '&' && c2 == 'a' )
				Eprintf("ignoring escape ESC&a... (transparency mode)\n");
				else {
				Eprintf("invalid escape ESC%c%c (#%d#%d#%d#)\n", c1,
					c2,c0,c1,c2);
				if (rtlgraphics==1) {
				if (!silent_mode) Eprintf("error in RTL data, skipping to next ESC\n");
				 ctmp=0;
				 while ( (ctmp=fgetc(hd)) != 27){};
				 ungetc(ctmp,hd);
				 }
				}	
			}
			return;
		}
	}
}

static void read_ESC_cmd(FILE * hd, int hp, GEN_PAR * pg)
/*
 * Read & skip device control commands (ESC.-Commands)

 */
{
	int ctmp;
	switch (ctmp = getc(hd)) {
	case '.':
		read_ESC_HP7550A(hd);
		break;
	case EOF:
		n_unexpected++;
		Eprintf("\nUnexpected EOF!\n");
		return;
	default:
		read_ESC_RTL(hd, ctmp, hp, pg);
		break;
	}
}



/****************************************************************************/



/**
 **	lines:	Process PA-, PR-, PU-, and  PD- commands
 **/
static void lines(int relative, FILE * hd)
/**
 ** Examples of anticipated commands:
 **
 **	PA PD0,0,80,50,90,20PU140,30PD150,80;
 **	PU0,0;PD20.53,40.32,30.08,60.2,40,90,;PU100,300;PD120,340...
 **/
{
	HPGL_Pt p;
	int numcmds = 0;

	for (;;) {
		if (read_float(&p.x, hd)) {	/* No number found      */
			if (numcmds > 0)
				return;
			cdot(0,NULL,pen_down);
			return;
		}

		if (read_float(&p.y, hd)) {	/* x without y invalid! */
			par_err_exit(2, PA, hd);
		break; /* ignore this move and continue */
		}
		if (!cdot(relative,&p,pen_down)) line(relative, p);
		numcmds++;
	}
}


/*
 * line : process a pair of coordinates
 */
void line(int relative, HPGL_Pt p)
{
	HPGL_Pt pl, porig;
	int outside = 0;
	double x1, y1, x2, y2;

	if (relative) {
		p.x += p_last.x;
		p.y += p_last.y;
	}

	porig.x = p.x;
	porig.y = p.y;

	if (iwflag) {
		x1 = P1.x + (p_last.x - S1.x) * Q.x;
		y1 = P1.y + (p_last.y - S1.y) * Q.y;
		x2 = P1.x + (p.x - S1.x) * Q.x;
		y2 = P1.y + (p.y - S1.y) * Q.y;

		outside =
		    (DtClipLine(C1.x, C1.y, C2.x, C2.y, &x1, &y1, &x2, &y2)
		     == CLIP_NODRAW);

		if (!outside) {
			p.x = (x2 - P1.x) / Q.x + S1.x;
			p.y = (y2 - P1.y) / Q.y + S1.y;
			pl.x = (x1 - P1.x) / Q.x + S1.x;
			pl.y = (y1 - P1.y) / Q.y + S1.y;
			if (pl.x != p_last.x || pl.y != p_last.y)
				Pen_action_to_tmpfile(MOVE_TO, &pl,
						      scale_flag);

		}

	} else
		pl = p_last;

	if (polygon_mode) {
		if (!outside && !pm1_flag) {
			HPGL_Pt_to_polygon(pl);
			HPGL_Pt_to_polygon(p);
		}
	}	else {
	if (pen_down && !outside) {
			Pen_action_to_tmpfile(DRAW_TO, &p, scale_flag);
	} else {
		if (iwflag) {
			Pen_action_to_tmpfile(MOVE_TO, &porig, scale_flag);
		} else {
			Pen_action_to_tmpfile(MOVE_TO, &p, scale_flag);
		}
	}
       }
	if (polygon_mode && pm1_flag) {
	pm1_flag=FALSE;
	polystart=p;
	}
	
	if (symbol_char) {
		plot_symbol_char(symbol_char,pen);
		Pen_action_to_tmpfile(MOVE_TO, &p, scale_flag);
	}
	outside = 0;
	p_last = porig;

}


/**
 **	Arcs, circles and alike
 **/


static void arc_increment(HPGL_Pt * pcenter, double r, double phi)
{
	HPGL_Pt p;
	int outside = 0;
	p.x = pcenter->x + r * cos(phi);
	p.y = pcenter->y + r * sin(phi);

	if (iwflag) {
		if (P1.x + (p.x - S1.x) * Q.x > C2.x
		    || P1.y + (p.y - S1.y) * Q.y > C2.y) {
/*fprintf(stderr,"IW set:point %f %f >P2\n",p.x,p.y); */
			outside = 1;
		}
		if (P1.x + (p.x - S1.x) * Q.x < C1.x
		    || P1.y + (p.y - S1.y) * Q.y < C1.y) {
/*fprintf(stderr,"IW set:point  %f %f <P1\n",p.x,p.y); */
			outside = 1;
		}
	}

	if (polygon_mode) {
	        if (pen_down && !outside) {
			HPGL_Pt_to_polygon(p_last);
			HPGL_Pt_to_polygon(p);

		} else if ((p.x != p_last.x) || (p.y != p_last.y)) {
			HPGL_Pt_to_polygon(p_last);
			HPGL_Pt_to_polygon(p);
		}
	} else {
		if (pen_down && !outside)
			Pen_action_to_tmpfile(DRAW_TO, &p, scale_flag);
		else if (/*!outside
			 &&*/ ((p.x != p_last.x) || (p.y != p_last.y)))
			Pen_action_to_tmpfile(MOVE_TO, &p, scale_flag);
	}
	p_last = p;
}

static void bezier(int relative, FILE * hd)
{
	HPGL_Pt p, p1, p2, p3, polyp;
	int i, outside;
	float t;
/*  double SafeLinePatLen = CurrentLinePatLen;*/

	for (;;) {		/* parameter set may contain several bezier curves */
		if (read_float(&p1.x, hd))	/* No number found      */
			return;

		if (read_float(&p1.y, hd)){	/* x without y invalid! */
			par_err_exit(2, BZ, hd);
			return;
			}
		if (read_float(&p2.x, hd)){	/* No number found      */
			par_err_exit(3, BZ, hd);
			return;
			}
		if (read_float(&p2.y, hd)){	/* x without y invalid! */
			par_err_exit(3, BZ, hd);
			return;
			}
		if (read_float(&p3.x, hd)){	/* No endpoint */
			par_err_exit(4, BZ, hd);
			return;
			}
		if (read_float(&p3.y, hd)){	/* No endpoint */
			par_err_exit(4, BZ, hd);
			return;
			}
		if (relative) {	/* Transform coordinates  */
			p1.x = p1.x + p_last.x;
			p1.y = p1.y + p_last.y;
			p2.x = p2.x + p_last.x;
			p2.y = p2.y + p_last.y;
			p3.x = p3.x + p_last.x;
			p3.y = p3.y + p_last.y;
		}

/*    
p(t) = t^3*P3 + 3*t^2*(1-t)*P2 + 3*t*(1-t)^2* P1 + (1-t)^3 * P0
*/

		polyp = p_last;
		outside = 0;

		for (i = 0; i < 51; i++) {
			t = (float) i / 50.;
			p.x =
			    t * t * t * p3.x + 3 * t * t * (1. -
							    t) * p2.x +
			    3 * t * (1. - t) * (1. - t) * p1.x + (1. -
								  t) *
			    (1. - t) * (1. - t) * p_last.x;
			p.y =
			    t * t * t * p3.y + 3 * t * t * (1. -
							    t) * p2.y +
			    3 * t * (1. - t) * (1. - t) * p1.y + (1. -
								  t) *
			    (1. - t) * (1. - t) * p_last.y;

/*fprintf(stderr,"bezier point %f %f\n",p.x,p.y);*/
			if (iwflag) {
				if (P1.x + (p.x - S1.x) * Q.x > C2.x
				    || P1.y + (p.y - S1.y) * Q.y > C2.y) {
/*fprintf(stderr,"IW set:point %f %f >P2\n",p.x,p.y); */
					outside = 1;
				}
				if (P1.x + (p.x - S1.x) * Q.x < C1.x
				    || P1.y + (p.y - S1.y) * Q.y < C1.y) {
/*fprintf(stderr,"IW set:point  %f %f <P1\n",p.x,p.y); */
					outside = 1;
				}
			}

			if (!outside) {
				if (polygon_mode) {
					HPGL_Pt_to_polygon(polyp);
					HPGL_Pt_to_polygon(p);
					polyp.x = p.x;
					polyp.y = p.y;
				} else {
					Pen_action_to_tmpfile(DRAW_TO, &p,
							      scale_flag);
				}
			} else
				Pen_action_to_tmpfile(MOVE_TO, &p,
						      scale_flag);
			outside = 0;
		}

		p_last.x = p3.x;
		p_last.y = p3.y;

	}
}

static void tarcs(int relative, FILE * hd)
{
	HPGL_Pt p, p2, p3, center, d;
	float eps;
	double phi, phi1, phi2, phi3,r;
	double SafeLinePatLen = CurrentLinePatLen;

	if (read_float(&p2.x, hd)){	/* No number found      */
		return;
		}
	if (read_float(&p2.y, hd)){	/* x without y invalid! */
		par_err_exit(2, AT, hd);
		return;
		}
	if (read_float(&p3.x, hd)){	/* No endpoint */
		par_err_exit(3, AT, hd);
		return;
		}
	if (read_float(&p3.y, hd)){	/* No endpoint */
		par_err_exit(3, AT, hd);
		return;
		}
	switch (read_float(&eps, hd)) {	/* chord angle is optional */
	case 0:
		if (eps < 0.5)
			eps = 0.5;
		break;
	case 1:		/* No resolution option */
		eps = 5.0;	/*    so use default!   */
		break;
	case 2:		/* Illegal state        */
		par_err_exit(98, AT, hd);
	case EOF:
		return;
	default:		/* Illegal state        */
		par_err_exit(99, AT, hd);
		return;
	}
	if (ct_dist == FALSE)
		eps *= M_PI / 180.0;	/* Deg-to-Rad           */

	d = p_last;

	if (!relative) {	/* Transform coordinates  */
		p2.x = p2.x - p_last.x;
		p2.y = p2.y - p_last.y;
		p3.x = p3.x - p_last.x;
		p3.y = p3.y - p_last.y;
	}
	/* 
    2*p2.x*h+2*p2.y*k = p2.x^2 + p2.y^2
    
    k= (p2.x^2 + p2.y^2 - 2*p2.x*h) /2*p2.y
     
    2*p3.x*h+2*p3.y*k = p3.x^2 + p3.y^2 
   

	       2* p3.x*h +2*p3.y * (p2.x^2 + p2.y^2 -2*p2.x*h) / 2*p2.y = (p3.x^2 + p3.y^2)   
	       2*p2.y*2*p3.x*h +2*p3.y * (p2.x^2 + p2.y^2 -2*p2.x*h)  = (p3.x^2 + p3.y^2) *2*p2.y  

	       4*p2.y*p3.x*h + 2*p3.y*p2.x^2 + 2*p3.y*p2.y^2 - 4*p2.x*p3.y*h =...
	       (4*p2.y*p3.x-4*p2.x*p3.y)*h  + 2*p3.y*p2.x^2 + 2*p3.y*p2.y^2 = ...
	       (2*p2.y*p3.x-2*p2.x*p3.y)*h  + p3.y*p2.x^2 + p3.y*p2.y^2 = (p3.x^2 + p3.y^2)*p2.y  
	       
	       h = ( p2.y*(p3.x^2 + p3.y^2) - p3.y*p2.x^2 - p3.y*p2.y^2 )  / (2*p2.y*p3.x-2*p2.x*p3.y)
	*/
	if (p3.x*p2.y==p2.x*p3.y) {
	  /* special cases */
	  if (p3.x == 0 && p2.x==0 && p3.y==0 && p2.y==0) {
	    cdot(0,&p_last,pen_down);
	    return;
	  } else if ((p2.x==0 && p2.y==0) || (p2.x==p3.x && p2.y==p3.y)) {
	    line(1,p3);
	    return;
	  } else if (p3.x==0 && p3.y==0) {
	    center.x=p2.x/2.0;
	    center.y=p2.y/2.0;
	  } else {
	    phi2=p2.x*p2.x+p2.y*p2.y;
	    phi3=p3.x*p3.x+p3.y*p3.y;
	    phi1=(p2.x-p3.x)*(p2.x-p3.x)+(p2.y-p3.y)*(p2.y-p3.y);

	    if (phi1>phi2 && phi1>phi2) {
	      /* FIXME: p_last,p2,inf,p3    */
	      fprintf(stderr,"tarcs: not implemented special case: (p_last,p2,inf,p3) p2=(%f,%f) p3=(%f,%f)\n",p2.x,p2.y,p3.x,p3.y); 
	      line(1,p3);
	    } else if (phi2 > phi3 && phi2 > phi1) {
	      /* FIXME: p_last,inf,p2,p3    */
	      fprintf(stderr,"tarcs: not implemented special case: (p_last,inf,p2,p3) p2=(%f,%f) p3=(%f,%f)\n",p2.x,p2.y,p3.x,p3.y); 
	      line(1,p3);
	    } else {
	      /* p_last,p2,p3 */
	      line(1,p3);
	    }
	    return;
	  }
	} else if (p2.y==0) {
	  center.x=p2.x/2.0;
	  center.y=(p3.x*p3.x+p3.y*p3.y-p3.x*p2.x)/(2.0*p3.y);
	} else {
	  center.x=(p2.y*(p3.x*p3.x+p3.y*p3.y)-p3.y*p2.x*p2.x-p3.y*p2.y*p2.y)/(2.*p3.x*p2.y-2.*p2.x*p3.y);
	  center.y =(p2.x * p2.x + p2.y * p2.y -  2. * p2.x * center.x) / (2. * p2.y);
	}

	r = sqrt(center.x * center.x + center.y * center.y);

	if (ct_dist == TRUE)
		eps = 2. * acos((r - eps) / r);
	
	center.x = center.x + p_last.x;
	center.y = center.y + p_last.y;


	d.x = p_last.x - center.x;
	d.y = p_last.y - center.y;

	phi1 = atan2(d.y, d.x);

	d.x = p2.x + p_last.x - center.x;
	d.y = p2.y + p_last.y - center.y;

	phi2 = atan2(d.y, d.x);

	d.x = p3.x + p_last.x - center.x;
	d.y = p3.y + p_last.y - center.y;

	phi3 = atan2(d.y, d.x);

	if (phi3==phi1) {
	  phi3=M_PI*2+phi3;
	}
/*
fprintf(stderr,"AT: P1 at %f %f , P2 %f %f, P3 %f %f, center %f %f radius %f\n",
p_last.x,p_last.y,p2.x+p_last.x,p2.y+p_last.y,p3.x+p_last.x,p3.y+p_last.y,
center.x,center.y,r);
*/
	if (CurrentLineType == LT_adaptive) {	/* Adaptive patterns:   */
		p.x = r * cos(eps);	/* A chord segment      */
		p.y = r * sin(eps);
		if (scale_flag)
			User_to_Plotter_coord(&p, &p);

		/*      Pattern length = chord length           */
		CurrentLinePatLen = HYPOT(p.x, p.y);
	}

	if (phi3 < phi2 && phi2 < phi1) {
	  eps=-eps;
	} else if (phi3 < phi1 && phi1 < phi2) {
	  phi3=2.0*M_PI+phi3;
	} else if (phi2 < phi3 && phi3 < phi1) {
	  phi3=2.0*M_PI+phi3;
	  phi2=2.0*M_PI+phi2;
	} else if (phi2 < phi1 && phi1 < phi3) {
	  eps=-eps;
	  phi3=phi3-2.0*M_PI;
	} else if (phi1 < phi3 && phi3 < phi2) {
	  eps=-eps;
	  phi2=phi2-2.0*M_PI;
	  phi3=phi3-2.0*M_PI;
	}
	
	if (eps > 0) {
	  for (phi = phi1 + eps; phi < phi3;phi += eps)
			arc_increment(&center, r, phi);
	  arc_increment(&center, r, phi3);	/* to endpoint */
	} else {
	  for (phi = phi1 + eps; phi > phi3;phi += eps)
			arc_increment(&center, r, phi);
	  arc_increment(&center, r, phi3);	/* to endpoint */
	}
	CurrentLinePatLen = SafeLinePatLen;	/* Restore */

}

static void arcs(int relative, FILE * hd)
{
	HPGL_Pt p, d, center;
	float alpha, eps;
	double phi, phi0, r;
	double SafeLinePatLen = CurrentLinePatLen;

	if (read_float(&p.x, hd))	/* No number found      */
		return;

	if (read_float(&p.y, hd)){	/* x without y invalid! */
		par_err_exit(2, AA, hd);
		return;
		}
	if (read_float(&alpha, hd)){	/* Invalid without angle */
		par_err_exit(3, AA, hd);
		return;
		}
	else
		alpha *= M_PI / 180.0;	/* Deg-to-Rad           */

	switch (read_float(&eps, hd)) {
	case 0:
		if (eps < 0.5)
			eps = 0.5;
		break;
	case 1:		/* No resolution option */
		eps = 5.0;	/*    so use default!   */
		break;
	case 2:		/* Illegal state        */
		par_err_exit(98, AA, hd);
	case EOF:
		return;
	default:		/* Illegal state        */
		par_err_exit(99, AA, hd);
		return;
	}

	if (ct_dist == FALSE)
		eps *= M_PI / 180.0;	/* Deg-to-Rad           */

	if (relative) {		/* Process coordinates  */
		d = p;		/* Difference vector    */
		center.x = d.x + p_last.x;
		center.y = d.y + p_last.y;
	} else {
		d.x = p.x - p_last.x;
		d.y = p.y - p_last.y;
		center.x = p.x;
		center.y = p.y;
	}

	if (((r = sqrt(d.x * d.x + d.y * d.y)) == 0.0) || (alpha == 0.0)) {
	        cdot(0,&p_last,pen_down);		/* Zero radius or zero arc angle given  */
		return;
        }

	if (ct_dist == TRUE)
		eps = 2. * acos((r - eps) / r);

	phi0 = atan2(-d.y, -d.x);

	if (CurrentLineType == LT_adaptive) {	/* Adaptive patterns:   */
		p.x = r * cos(eps);	/* A chord segment      */
		p.y = r * sin(eps);
		if (scale_flag)
			User_to_Plotter_coord(&p, &p);

		/*      Pattern length = chord length           */
		CurrentLinePatLen = HYPOT(p.x, p.y);
	}

	if (alpha > 0.0) {
		for (phi = phi0 + MIN(eps, alpha); phi < phi0 + alpha;
		     phi += eps)
			arc_increment(&center, r, phi);
		arc_increment(&center, r, phi0 + alpha);	/* to endpoint */
	} else {
		for (phi = phi0 - MIN(eps, -alpha); phi > phi0 + alpha;
		     phi -= eps)
			arc_increment(&center, r, phi);
		arc_increment(&center, r, phi0 + alpha);	/* to endpoint */
	}
	CurrentLinePatLen = SafeLinePatLen;	/* Restore */
}

static void fwedges(FILE * hd, float cur_pensize)
{				/*derived from circles */
	HPGL_Pt p, oldp, center;
	float eps, r, start, sweep;
	double phi;
	double SafeLinePatLen = CurrentLinePatLen;
	int outside = 0;

	if (read_float(&r, hd))	/* No radius found      */
		return;
	if (read_float(&start, hd))	/* No start angle found */
		return;

	if (read_float(&sweep, hd))	/* No sweep angle found */
		return;

	switch (read_float(&eps, hd)) {	/* chord angle */
	case 0:
		if (eps < 0.5)
			eps = 0.5;
		break;
	case 1:		/* No resolution option */
		eps = 5.0;	/*    so use default!   */
		break;
	case 2:		/* Illegal state        */
		par_err_exit(98, EW, hd);
	case EOF:
		return;
	default:		/* Illegal state        */
		par_err_exit(99, EW, hd);
		return;
	}

	if (ct_dist == TRUE)
		eps = 2. * acos((r - eps) / r);
	else
		eps *= M_PI / 180.0;	/* Deg-to-Rad           */
	start *= M_PI / 180.0;	/* Deg-to-Rad           */
	sweep *= M_PI / 180.0;	/* Deg-to-Rad           */


	center = p_last;	/* reference point is last position */
	vertices = -1;		/* clear the polygon buffer */
	if (r == 0.0)		/* Zero radius given    */
		return;

	HPGL_Pt_to_polygon(p_last);
	p.x = center.x + r * cos(start);
	p.y = center.y + r * sin(start);
	HPGL_Pt_to_polygon(p);


	if (CurrentLineType == LT_adaptive) {	/* Adaptive patterns    */
		p.x = r * cos(eps);	/* A chord segment      */
		p.y = r * sin(eps);
		if (scale_flag)
			User_to_Plotter_coord(&p, &p);

		/*      Pattern length = chord length           */
		CurrentLinePatLen = HYPOT(p.x, p.y);
	}
	for (phi = eps; phi <= sweep; phi += eps) {
		oldp = p;
		p.x = center.x + r * cos(start + phi);
		p.y = center.y + r * sin(start + phi);
		if (iwflag) {
			if (P1.x + (p.x - S1.x) * Q.x > C2.x
			    || P1.y + (p.y - S1.y) * Q.y > C2.y) {
/*fprintf(stderr,"IW set:point %f %f >P2\n",p.x,p.y); */
				outside = 1;
			}
			if (P1.x + (p.x - S1.x) * Q.x < C1.x
			    || P1.y + (p.y - S1.y) * Q.y < C1.y) {
/*fprintf(stderr,"IW set:point  %f %f <P1\n",p.x,p.y); */
				outside = 1;
			}
		}
		if (!outside) {
			HPGL_Pt_to_polygon(oldp);
			HPGL_Pt_to_polygon(p);
		}
		outside = 0;
	}
	HPGL_Pt_to_polygon(p);
	HPGL_Pt_to_polygon(center);
	if (hatchspace == 0.)
		hatchspace = cur_pensize;
	if (filltype < 3 && thickness > 0.)
		hatchspace = thickness;
	if (!ac_flag) {		/* not yet initialized */
		anchor.x = lxmin;
		anchor.y = lymin;
		if (scale_flag) Plotter_to_User_coord(&anchor,&anchor);	
	}
/*	
FIXME: add calls for the "haspoly" case
*/
	fill(polygons, vertices, anchor, P2, scale_flag, filltype,
	     hatchspace, hatchangle,cur_pensize,0,0);

	CurrentLinePatLen = SafeLinePatLen;	/* Restore */

}



static void circles(FILE * hd)
{
	HPGL_Pt p, center, polyp;
	float eps, r;
	double phi;
	double SafeLinePatLen = CurrentLinePatLen;
	int outside = 0;

	if (read_float(&r, hd))	/* No radius found      */
		return;

	switch (read_float(&eps, hd)) {
	case 0:
		if (eps < 0.5)
			eps = 0.5;
		break;
	case 1:		/* No resolution option */
		eps = 5.0;	/*    so use default!   */
		break;
	case 2:		/* Illegal state        */
		par_err_exit(98, CI, hd);
	case EOF:
		return;
	default:		/* Illegal state        */
		par_err_exit(99, CI, hd);
		return;
	}

	if (ct_dist == TRUE)
		eps = 2. * acos((r - eps) / r);
	else
		eps *= M_PI / 180.0;	/* Deg-to-Rad           */


	center = p_last;

	if (r == 0.0)		/* Zero radius given    */
		return;

	p.x = center.x + r;
	p.y = center.y;
	Pen_action_to_tmpfile(MOVE_TO, &p, scale_flag);
	if (polygon_mode) {
		polyp.x = p.x;
		polyp.y = p.y;
	}
	if (CurrentLineType == LT_adaptive) {	/* Adaptive patterns    */
		p.x = r * cos(eps);	/* A chord segment      */
		p.y = r * sin(eps);
		if (scale_flag)
			User_to_Plotter_coord(&p, &p);

		/*      Pattern length = chord length           */
		CurrentLinePatLen = HYPOT(p.x, p.y);
	}

	for (phi = eps; phi < 2.0 * M_PI; phi += eps) {
		p.x = center.x + r * cos(phi);
		p.y = center.y + r * sin(phi);
		if (iwflag) {
			if (P1.x + (p.x - S1.x) * Q.x > C2.x
			    || P1.y + (p.y - S1.y) * Q.y > C2.y) {
/*fprintf(stderr,"IW set:point %f %f >P2\n",p.x,p.y); */
				outside = 1;
			}
			if (P1.x + (p.x - S1.x) * Q.x < C1.x
			    || P1.y + (p.y - S1.y) * Q.y < C1.y) {
/*fprintf(stderr,"IW set:point  %f %f <P1\n",p.x,p.y); */
				outside = 1;
			}
		}

		if (!outside) {
			if (polygon_mode) {
				HPGL_Pt_to_polygon(polyp);
				HPGL_Pt_to_polygon(p);
				polyp.x = p.x;
				polyp.y = p.y;
			} else {
				Pen_action_to_tmpfile(DRAW_TO, &p,
						      scale_flag);
			}
		} else
			Pen_action_to_tmpfile(MOVE_TO, &p, scale_flag);
		outside = 0;
	}

	p.x = center.x + r;	/* Close circle at r * (1, 0)   */
	p.y = center.y;
		if (iwflag) {
			if (P1.x + (p.x - S1.x) * Q.x > C2.x
			    || P1.y + (p.y - S1.y) * Q.y > C2.y) {
				outside = 1;
			}
			if (P1.x + (p.x - S1.x) * Q.x < C1.x
			    || P1.y + (p.y - S1.y) * Q.y < C1.y) {
				outside = 1;
			}
		}

		if (!outside) {
	if (polygon_mode) {
		HPGL_Pt_to_polygon(polyp);
		HPGL_Pt_to_polygon(p);
	} else
		Pen_action_to_tmpfile(DRAW_TO, &p, scale_flag);
	} else
		Pen_action_to_tmpfile(MOVE_TO, &p, scale_flag);
	
	if (!polygon_mode) {
		/* draw one overlapping segment to avoid leaving gap with wide pens */
		p.x = center.x + r * cos(eps);
		p.y = center.y + r * sin(eps);
		if (iwflag) {
			if (P1.x + (p.x - S1.x) * Q.x > C2.x
			    || P1.y + (p.y - S1.y) * Q.y > C2.y) {
/*fprintf(stderr,"IW set:point %f %f >P2\n",p.x,p.y); */
				outside = 1;
			}
			if (P1.x + (p.x - S1.x) * Q.x < C1.x
			    || P1.y + (p.y - S1.y) * Q.y < C1.y) {
/*fprintf(stderr,"IW set:point  %f %f <P1\n",p.x,p.y); */
				outside = 1;
			}
		}

		if (!outside) {
			if (polygon_mode) {
				HPGL_Pt_to_polygon(polyp);
				HPGL_Pt_to_polygon(p);
				polyp.x = p.x;
				polyp.y = p.y;
			} else {
				Pen_action_to_tmpfile(DRAW_TO, &p,
						      scale_flag);
			}
		}
	}
	Pen_action_to_tmpfile(MOVE_TO, &center, scale_flag);

	CurrentLinePatLen = SafeLinePatLen;	/* Restore */
}

static void wedges(FILE * hd)
{				/*derived from circles */
	HPGL_Pt p, center;
	float eps, r, start, sweep;
	double phi;
	double SafeLinePatLen = CurrentLinePatLen;
	int outside = 0;

	if (read_float(&r, hd))	/* No radius found      */
		return;

	if (read_float(&start, hd))	/* No start angle found */
		return;

	if (read_float(&sweep, hd))	/* No sweep angle found */
		return;

	switch (read_float(&eps, hd)) {	/* chord angle */
	case 0:
		if (eps < 0.5)
			eps = 0.5;
		break;
	case 1:		/* No resolution option */
		eps = 5.0;	/*    so use default!   */
		break;
	case 2:		/* Illegal state        */
		par_err_exit(98, EW, hd);
	case EOF:
		return;
	default:		/* Illegal state        */
		par_err_exit(99, EW, hd);
		return;
	}

	if (ct_dist == TRUE)
		eps = 2. * acos((r - eps) / r);
	else
		eps *= M_PI / 180.0;	/* Deg-to-Rad           */
	start *= M_PI / 180.0;	/* Deg-to-Rad           */
	sweep *= M_PI / 180.0;	/* Deg-to-Rad           */


	center = p_last;	/* reference point is last position */

	if (r == 0.0)		/* Zero radius given    */
		return;

	p.x = center.x + r * cos(start);
	p.y = center.y + r * sin(start);
	Pen_action_to_tmpfile(DRAW_TO, &p, scale_flag);

	if (CurrentLineType == LT_adaptive) {	/* Adaptive patterns    */
		p.x = r * cos(eps);	/* A chord segment      */
		p.y = r * sin(eps);
		if (scale_flag)
			User_to_Plotter_coord(&p, &p);

		/*      Pattern length = chord length           */
		CurrentLinePatLen = HYPOT(p.x, p.y);
	}

	for (phi = eps; phi <= sweep; phi += eps) {
		p.x = center.x + r * cos(start + phi);
		p.y = center.y + r * sin(start + phi);
		if (iwflag) {
			if (P1.x + (p.x - S1.x) * Q.x > C2.x
			    || P1.y + (p.y - S1.y) * Q.y > C2.y) {
/*fprintf(stderr,"IW set:point %f %f >P2\n",p.x,p.y); */
				outside = 1;
			}
			if (P1.x + (p.x - S1.x) * Q.x < C1.x
			    || P1.y + (p.y - S1.y) * Q.y < C1.y) {
/*fprintf(stderr,"IW set:point  %f %f <P1\n",p.x,p.y); */
				outside = 1;
			}
		}
		if (!outside)
			Pen_action_to_tmpfile(DRAW_TO, &p, scale_flag);
		else
			Pen_action_to_tmpfile(MOVE_TO, &p, scale_flag);
		outside = 0;
	}

	Pen_action_to_tmpfile(DRAW_TO, &center, scale_flag);

	CurrentLinePatLen = SafeLinePatLen;	/* Restore */
}



static void ax_ticks(int mode)
{
	HPGL_Pt p0, p1, p2;
	LineType SafeLineType = CurrentLineType;

	p0 = p1 = p2 = p_last;
/**
 ** According to the HP-GL manual,
 ** XT & YT are not affected by LT
 **/
	CurrentLineType = LT_solid;

	if (mode == 0) {	/* X tick       */
		if (scale_flag) {
			p1.y -= neg_ticklen * (P2.y - P1.y) / Q.y;
			p2.y += pos_ticklen * (P2.y - P1.y) / Q.y;
		} else {
			p1.y -= neg_ticklen * (P2.y - P1.y);
			p2.y += pos_ticklen * (P2.y - P1.y);
		}
	} else
		/* Y tick */
	{
		if (scale_flag) {
			p1.x -= neg_ticklen * (P2.x - P1.x) / Q.x;
			p2.x += pos_ticklen * (P2.x - P1.x) / Q.x;
		} else {
			p1.x -= neg_ticklen * (P2.x - P1.x);
			p2.x += pos_ticklen * (P2.x - P1.x);
		}
	}

	Pen_action_to_tmpfile(MOVE_TO, &p1, scale_flag);
	Pen_action_to_tmpfile(DRAW_TO, &p2, scale_flag);
	Pen_action_to_tmpfile(MOVE_TO, &p0, scale_flag);

	CurrentLineType = SafeLineType;
}



/**
 **	Process a single HPGL command
 **/

static void read_HPGL_cmd(GEN_PAR * pg, int cmd, FILE * hd)
{
	short old_pen;
	HPGL_Pt p1 = { 0., 0. }, p2 = {
	0., 0.};
	float ftmp;
	float csfont;
	int mypen, myred, mygreen, myblue, i,j;
	float mywidth, myheight;
	char tmpstr[1024];
	char ctmp;
	char SafeTerm;
	int statusbyte;
#if 0
	static int FoundUserFill = 0;
#endif
/**
 ** Each command consists of 2 characters. We unite them here to a single int
 ** to allow for easy processing within a big switch statement:
 **/

	switch (cmd & 0xDFDF) {	/* & forces to upper case       */
  /**
   ** Commands appear in alphabetical order within each topic group
   ** except for command synonyms.
   **/
	case AA:		/* Arc Absolute                 */
		arcs(FALSE, hd);
		tp->CR_point = HP_pos;
		break;
	case AR:		/* Arc Relative                 */
		arcs(TRUE, hd);
		tp->CR_point = HP_pos;
		break;
	case AT:		/* Arc Absolute, through Three points */
		tarcs(FALSE, hd);
		break;
	case BR:		/* cubic bezier curve, relative control points */
		bezier(TRUE, hd);
		break;
	case BZ:		/* cubic bezier curve, absolute control points */
		bezier(FALSE, hd);
		break;
	case AC:		/* anchor corner of fill patterns */
		if (read_float(&ftmp, hd)) {	/* just AC - default hard-clip limit */
			anchor.x = P1.x;
			anchor.y = P1.y;
			if (scale_flag)
				User_to_Plotter_coord(&anchor, &anchor);
			break;
		} else {
			ac_flag = 1;
			anchor.x = ftmp;
		}
		if (read_float(&ftmp, hd))
			anchor.y = 0.;
		else
			anchor.y = ftmp;
		if (scale_flag)
			User_to_Plotter_coord(&anchor, &anchor);
		break;
	case AD:
		if (read_float(&ftmp, hd))	/* just AD - defaults */
			tp->altfont = 0;
		else {
			do {
			switch ((int) ftmp) {
			case 1:	/* charset */
				if (read_float(&csfont, hd)){
					par_err_exit(2, cmd, hd);
					return;
					}
				else
					tp->altfont = (int) csfont;
				break;
			case 2:	/* fixed or variable spacing */
				if (read_float(&csfont, hd)){
					par_err_exit(2, cmd, hd);
					return;
					}
#ifdef STROKED_VARFONTS
				else if ((int) csfont == 1) tp->avariable=csfont;
#else									
				else if ((int) csfont == 1 && !silent_mode)
					fprintf(stderr,
						"only fixed fonts available\n");
#endif
				break;
			case 6:	/* stroke weight */
				if (read_float(&csfont, hd)){
					par_err_exit(2, cmd, hd);
					return;
					}
				if (csfont == 9999.)
					tp->astrokewidth = csfont;
				else {
					if (csfont < -7. || csfont > 7.)
						csfont = 0.;
					tp->astrokewidth = 0.11 + csfont / 70.;	/* 0.01 ... 0.21 mm */
				}
				break;
			case 3:	/* font pitch */
			case 4:	/* font height */
			case 5:	/* posture */
			case 7:	/* typeface */
				if (read_float(&csfont, hd)){
					par_err_exit(2, cmd, hd);
					return;
					}
				else if (!silent_mode)
					fprintf(stderr,
						"pitch/height/posture/typeface unsupported\n");
				break;
			default:
				par_err_exit(1, cmd, hd);
				return;
			}
		} while (!read_float(&ftmp,hd));	
		}
		break;
	case CA:		/* Alternate character set      */
		if (read_float(&csfont, hd))	/* just CA;    */
			tp->altfont = 0;
		else
			tp->altfont = (int) csfont;
		break;
	case CI:		/* Circle                       */
		circles(hd);
		break;
	case CO:		/* Comment                      */
		SafeTerm = StrTerm;
		StrTerm = ';';
		read_string(tmpstr, hd);
		StrTerm = SafeTerm;
		if (strlen(tmpstr) > 0)
			tmpstr[strlen(tmpstr)] = '\0';
		if (!silent_mode)
			printf("\n%s\n", tmpstr);
		break;
	case CR:		/* color range */
		if (read_float(&ftmp, hd)) {
			r_base = g_base = b_base = 0;
			r_max = g_max = b_max = 255;
			break;
		} else {
			r_base = (unsigned char) ftmp;
		}
		if (read_float(&ftmp, hd)) {
			r_max = 255;
			break;

		} else {
			r_max = (unsigned char) ftmp;
		}
		if (read_float(&ftmp, hd)) {
			break;

		} else {
			g_base = (unsigned char) ftmp;
		}
		if (read_float(&ftmp, hd)) {
			g_max = 255;
			break;
		} else {
			g_max = (unsigned char) ftmp;
		}
		if (read_float(&ftmp, hd)) {
			break;
		} else {
			b_base = (unsigned char) ftmp;
		}
		if (read_float(&ftmp, hd)) {
			b_max = 255;
			break;
		} else {
			b_max = (unsigned char) ftmp;
			break;
		}
	case CS:		/*character set selection       */
		if (read_float(&csfont, hd))	/* just CS;     */
			tp->font = 0;
		else
			tp->font = (int) csfont;
		tp->stdfont = tp->font;
		break;
	case CT:		/* chord tolerance */
		if (read_float(&ftmp, hd) || ftmp != 1.)
			ct_dist = FALSE;
		else
			ct_dist = TRUE;
		break;
	case EP:		/* edge polygon */
		/* EA, ER, EW, RA, RR, and WG */
		if (HAS_POLY(pg->xx_mode)) {
			 PlotCmd_to_tmpfile(EDGE_POLY);
			 break;
		}
		if (polygon_penup == TRUE)
			if (p_last.x != polystart.x
			    || p_last.y != polystart.y)
				vertices -= 2;
		for (i = 0; i < vertices; i = i + 2) {	/*for all polygon edges */
		if (polygon_penstate[i]!=FALSE){
			p1.x = polygons[i].x;
			p1.y = polygons[i].y;
			Pen_action_to_tmpfile(MOVE_TO, &p1, scale_flag);
			p1.x = polygons[i + 1].x;
			p1.y = polygons[i + 1].y;
			Pen_action_to_tmpfile(DRAW_TO, &p1, scale_flag);
			}
		}
		Pen_action_to_tmpfile(MOVE_TO, &p_last, scale_flag);
		break;

	case EW:		/* Edge Wedge                   */
		wedges(hd);
		break;

	case EC:
		/*  printf("cut paper\n"); */
		break;

	case FP:		/* fill polygon */
			if (read_float (&ftmp, hd)) ftmp=0; /* No number found  */
		if (HAS_POLY(pg->xx_mode)) {
			if (pg->nofill) {
				PlotCmd_to_tmpfile(EDGE_POLY);
				break;
			}
			fillpoly(((ftmp)?NZFILL_POLY:EOFILL_POLY),filltype,Q.x,
				 hatchspace,hatchscale,hatchangle,rotate_flag,rot_ang);
			break;
		}
		if (pg->nofill) {	/* treat like EP */
			if (!silent_mode)
				fprintf(stderr, "FP : suppressed\n");
			for (i = 0; i < vertices; i = i + 2) {	/*for all polygon edges */
				p1.x = polygons[i].x;
				p1.y = polygons[i].y;
				Pen_action_to_tmpfile(MOVE_TO, &p1,
						      scale_flag);
				p1.x = polygons[i + 1].x;
				p1.y = polygons[i + 1].y;
				Pen_action_to_tmpfile(DRAW_TO, &p1,
						      scale_flag);
			}
			Pen_action_to_tmpfile(MOVE_TO, &p_last,
					      scale_flag);
			break;
		}
		if (pen == -1) pen=1; /* no preceding SP command */
		if (hatchspace == 0.)
			hatchspace = pt.width[pen];
		if (filltype < 3 && thickness > 0.)
			hatchspace = thickness;
		if (!ac_flag) {	/* not yet initialized */
		anchor.x = lxmin;
		anchor.y = lymin;
		if (scale_flag) Plotter_to_User_coord(&anchor,&anchor);	
		}
		fill(polygons, vertices, anchor, P2, scale_flag, filltype,
		     hatchspace, hatchangle,pt.width[pen],pg->dpi,((ftmp)?1:0) );
		Pen_action_to_tmpfile(MOVE_TO, &p_last, scale_flag);
		break;
	case FT:		/* Fill Type */
		if (read_float(&ftmp, hd)) {	/* just FT -> FT=1 */
			filltype = 1;
			break;
		} else {
			filltype = (int) ftmp;
		}
		if (filltype < 3)
			break;
#if 0
		if (filltype > 4) {
			if (FoundUserFill == 0) {
				FoundUserFill = 1;
				if (!silent_mode)
					fprintf(stderr,
						"\nNo support for user-defined fill types, using type 1 instead\n");
			}
			filltype = 1;
		}
#endif
		if (filltype < 3)
			break;

		if (read_float(&ftmp, hd)) {
			hatchspace = saved_hatchspace[filltype - 3 - (filltype> 9 ? 5:0)];
			if (hatchspace == 0.)
				hatchspace = 0.01 * Diag_P1_P2;
			hatchangle = saved_hatchangle[filltype - 3- (filltype> 9 ? 5:0)];
		} else {
			if (ftmp <= 0.)
				ftmp = 0.01 * Diag_P1_P2;
			hatchspace = ftmp;
			hatchscale=scale_flag;

			saved_hatchspace[filltype - 3- (filltype>9 ? 5:0)] = hatchspace;
			saved_hatchscale[filltype - 3- (filltype>9 ? 5:0)] = hatchscale;
		
		if (read_float(&ftmp, hd)) {
			hatchangle = saved_hatchangle[filltype - 3- (filltype>9 ? 5:0)];
		} else {
			hatchangle = ftmp;
			saved_hatchangle[filltype - 3- (filltype>9 ? 5:0)] = hatchangle;
		}
		}
		break;
	case NP:		/* Number of Pens                    */
		if (read_float(&ftmp, hd) || ftmp > NUMPENS)	/* invalid or missing */
			break;
		else {
			pg->maxpens = (int) ftmp;
			if (!silent_mode)
				fprintf(stderr, "NP: %d pens requested\n",
					pg->maxpens);
		}
		break;
	case NR:		/*Not ready - pause plotter (noop) */
		if (read_float(&ftmp, hd))
			break;
		break;
	case PA:		/* Plot Absolute                */
		lines(plot_rel = FALSE, hd);
		tp->CR_point = HP_pos;
		break;
	case PC:		/* Pen Color                    */
		if (read_float(&ftmp, hd) || fixedcolor || (int) ftmp > pg->maxpens) {	/* invalid or missing */
			break;
		} else {
			mypen = (int) ftmp;
			if (pg->mapzero == mypen)
				break;
			/* this color is remapped for pen 0, ignore original definition */
			if (read_float(&ftmp, hd))	/* no red component  */
				myred = 0;
			else
				myred = 255 * (ftmp - r_base) / r_max;
			if (read_float(&ftmp, hd))	/* no green component  */
				mygreen = 0;
			else
				mygreen = 255 * (ftmp - g_base) / g_max;
			if (read_float(&ftmp, hd))	/* no blue component  */
				myblue = 0;
			else
				myblue = 255 * (ftmp - b_base) / b_max;
			pg->is_color = TRUE;

			if (mypen == 0 && pg->mapzero > -1)
				mypen = pg->mapzero;
			if (pen==mypen) 
			  cdot(0,NULL,0);
			PlotCmd_to_tmpfile(DEF_PC);
			Pen_Color_to_tmpfile(mypen, myred, mygreen,
					     myblue);
			break;
		}
	case PD:		/* Pen  Down                    */
		pen_down = TRUE;
		lines(plot_rel, hd);
		tp->CR_point = HP_pos;
		break;
	case PE:
		read_PE(pg, hd);
		tp->CR_point = HP_pos;
		break;
	case PM:
	        cdot(0,NULL,0);
		if (read_float(&ftmp, hd) || ftmp == 0) {	/* no parameters or PM0 */
			polystart = p_last;
			if (HAS_POLY(pg->xx_mode)) {
				PlotCmd_to_tmpfile(OP_PBUF);
				break;
			}
			polygon_mode = TRUE;
			polygon_penup = TRUE;
			saved_penstate = pen_down;
			pen_down = TRUE;
			vertices = -1;
			pm1_flag = FALSE;
			break;
		}
		if (ftmp == 1) {
			if (HAS_POLY(pg->xx_mode)) {
				PlotCmd_to_tmpfile(SUBPOLY);
				break;
			}
			if (p_last.x != polystart.x
			    || p_last.y != polystart.y) {
				int outside = 0;
				double x1, y1, x2, y2;
				if (iwflag) {
					x1 = P1.x + (p_last.x -
						     S1.x) * Q.x;
					y1 = P1.y + (p_last.y -
						     S1.y) * Q.y;
					x2 = P1.x + (polystart.x -
						     S1.x) * Q.x;
					y2 = P1.y + (polystart.y -
						     S1.y) * Q.y;

					outside =
					    (DtClipLine
					     (C1.x, C1.y, C2.x, C2.y, &x1,
					      &y1, &x2,
					      &y2) == CLIP_NODRAW);

					if (!outside) {
						p_last.x =
						    (x2 - P1.x) / Q.x +
						    S1.x;
						p_last.y =
						    (y2 - P1.y) / Q.y +
						    S1.y;
						polystart.x =
						    (x1 - P1.x) / Q.x +
						    S1.x;
						polystart.y =
						    (y1 - P1.y) / Q.y +
						    S1.y;
					}
				}

				if (!outside) {
					polygon_penup=TRUE;
					HPGL_Pt_to_polygon(p_last);
					HPGL_Pt_to_polygon(polystart);	/* force closing of open polygon */
				}
			}
			if (vertices > 0)
				pm1_flag = TRUE;
/*			pen_down = FALSE;*/
			break;
		}
		if (ftmp == 2) {
			
			if (HAS_POLY(pg->xx_mode)) {
				PlotCmd_to_tmpfile(CL_PBUF);
			p_last=polystart;
				break;
			}
			polygon_mode = FALSE;
			pen_down = saved_penstate;
			if (p_last.x != polystart.x
			    || p_last.y != polystart.y) {
				int outside = 0;
				double x1, y1, x2, y2;
				if (iwflag) {
					x1 = P1.x + (p_last.x -
						     S1.x) * Q.x;
					y1 = P1.y + (p_last.y -
						     S1.y) * Q.y;
					x2 = P1.x + (polystart.x -
						     S1.x) * Q.x;
					y2 = P1.y + (polystart.y -
						     S1.y) * Q.y;

					outside =
					    (DtClipLine
					     (C1.x, C1.y, C2.x, C2.y, &x1,
					      &y1, &x2,
					      &y2) == CLIP_NODRAW);

					if (!outside) {
						p_last.x =
						    (x2 - P1.x) / Q.x +
						    S1.x;
						p_last.y =
						    (y2 - P1.y) / Q.y +
						    S1.y;
						polystart.x =
						    (x1 - P1.x) / Q.x +
						    S1.x;
						polystart.y =
						    (y1 - P1.y) / Q.y +
						    S1.y;
					}
				}
				if (!outside) {
					HPGL_Pt_to_polygon(p_last);
					HPGL_Pt_to_polygon(polystart);	/* force closing of open polygon */
				}
			}
			p_last=polystart;
		}
		break;
	case PR:		/* Plot Relative                */
		lines(plot_rel = TRUE, hd);
		tp->CR_point = HP_pos;
		break;
	case PS:
		if (read_float(&ftmp, hd) || ftmp == 0.) {	/* no parameters or PS0; */
			break;
		} else {
			myheight = ftmp;
		}
		if (read_float(&ftmp, hd)) {	/* no parameters */
			mywidth = P2.y;
		} else {
			mywidth = ftmp;
			if (mywidth > myheight) {
				mywidth = myheight;
				myheight = ftmp;
			}
		}
		if (pg->no_ps == TRUE) {
			if (!silent_mode)
				Eprintf("PS: suppressed\n");
			break;
		}
		ps_flag = 1;
		M.x = myheight;
		M.y = mywidth;
		p1.x = 0;
		p1.y = 0;

		if (scale_flag)	/* Rescaling    */
			User_to_Plotter_coord(&p1, &p2);
		else
			p2 = p1;	/* Local copy   */
		if (rotate_flag) {	/* hp2xx-specific global rotation       */
			ftmp = rot_cos * p2.x - rot_sin * p2.y;
			p2.y = rot_sin * p2.x + rot_cos * p2.y;
			p2.x = ftmp;
		}
		lxmin = MIN(p2.x, lxmin);
		lymin = MIN(p2.y, lymin);
		lxmax = MAX(p2.x, lxmax);
		lymax = MAX(p2.y, lymax);

		p1.x = myheight;
		p1.y = mywidth;
		if (scale_flag)	/* Rescaling    */
			User_to_Plotter_coord(&p1, &p2);
		else
			p2 = p1;	/* Local copy   */
		if (rotate_flag) {	/* hp2xx-specific global rotation       */
			ftmp = rot_cos * p2.x - rot_sin * p2.y;
			p2.y = rot_sin * p2.x + rot_cos * p2.y;
			p2.x = ftmp;
		}
		lxmin = MIN(p2.x, lxmin);
		lymin = MIN(p2.y, lymin);
		lxmax = MAX(p2.x, lxmax);
		lymax = MAX(p2.y, lymax);

/* add the following - to get the correct linetype scale etc */
		P1.x = 0.;
		P1.y = 0.;
		P2.x = myheight;
		P2.y = mywidth;
		if (rotate_flag) {
			P2.x = mywidth;
			P2.y = myheight;
		}
		Diag_P1_P2 = HYPOT(P2.x - P1.x, P2.y - P1.y);
		CurrentLinePatLen = 0.04 * Diag_P1_P2;
		S1 = P1;
		S2 = P2;
/* ajb */
		break;
	case PT:		/* Pen thickness (for solid fills - current pen only */
		if (read_float(&ftmp, hd)) {	/* no parameters */
			thickness = 0.3;
			break;
		} else {
			if (ftmp >= 0.1 && ftmp <= 5.)
				thickness = ftmp;
		}
	case PU:		/* Pen  Up                      */
		if (!polygon_mode)
			pen_down = FALSE;
		else
			polygon_penup = TRUE;

		lines(plot_rel, hd);
		tp->CR_point = HP_pos;
		break;
	case PW:		/* Pen Width                    */
		if (fixedwidth) {
			if (!silent_mode)
				fprintf(stderr,
					"PW: ignored (hardware mode)\n");
			break;
		}
		if (read_float(&ftmp, hd)) {	/* no parameters -> set defaults */
  		        cdot(0,NULL,0);
			mywidth = 0.35;
/* FIXME - scaling here ! */
			if (wu_relative)
				mywidth = Diag_P1_P2 / 1000.;
			if (mywidth < 0.1)
				mywidth = 0.1;
			PlotCmd_to_tmpfile(DEF_PW);
			Pen_Width_to_tmpfile(0, mywidth);
/*	 
          fprintf(stderr,"PW: defaulting to 0.35 for all pens\n");
*/
			break;
		} else {
			mywidth = ftmp;	/* first or only parameter is width */
			if (wu_relative)
				mywidth = Diag_P1_P2 * ftmp / 1000.;
			if (mywidth < 0.1)
				mywidth = 0.1;
		}

		if (read_float(&ftmp, hd)) {	/* width only, applies to all pens */
  		        cdot(0,NULL,0);
			PlotCmd_to_tmpfile(DEF_PW);
			Pen_Width_to_tmpfile(0, mywidth);
			if (pg->maxpensize < mywidth)
				pg->maxpensize = mywidth;
/*	 
          fprintf(stderr,"PW: defaulting to %f for all pens\n",mywidth);
*/
		} else {	/* second parameter is pen */
		        cdot(0,NULL,0);
			PlotCmd_to_tmpfile(DEF_PW);
			Pen_Width_to_tmpfile((int) ftmp, mywidth);
			if ((int) ftmp <= pg->maxpens) {
				if (pg->maxpensize < mywidth)
					pg->maxpensize = mywidth;
			}

         fprintf(stderr,"pen%d, size now %f\n",(int) ftmp,mywidth);
		}
		break;
	case TL:		/* Tick Length                  */
		if (read_float(&ftmp, hd)) {	/* No number found  */
			neg_ticklen = pos_ticklen = 0.005;
			return;
		} else
			pos_ticklen = ftmp / 100.0;

		if (read_float(&ftmp, hd)) {	/* pos, but not neg */
			neg_ticklen = 0.0;
			return;
		} else
			neg_ticklen = ftmp / 100.0;
		break;
	case WG:		/* Filled Wedge                 */
	        if (HAS_POLY(pg->xx_mode) && !pg->nofill) {
		  	PlotCmd_to_tmpfile(OP_PBUF);
		    	wedges(hd);
	    		PlotCmd_to_tmpfile(CL_PBUF);
			fillpoly(NZFILL_POLY,filltype,Q.x,
				 hatchspace,hatchscale,hatchangle,rotate_flag,rot_ang);
	  	} else {
		if (pg->nofill)
		wedges(hd);
		else
		fwedges(hd, pt.width[pen]);

	  	}
		break;
	case WU:		/* pen Width Unit is relative  */
		if (read_float(&ftmp, hd) || ftmp == 0.)	/* Zero or no number  */
			wu_relative = FALSE;
		else
			wu_relative = TRUE;
		break;
	case XT:		/* X Tick                       */
		ax_ticks(0);
		break;
	case YT:		/* Y Tick                       */
		ax_ticks(1);
		break;


	case IP:		/* Input reference Points P1,P2 */
		tp->width /= (P2.x - P1.x);
		tp->height /= (P2.y - P1.y);
		if (read_float(&p1.x, hd)) {	/* No number found  */
			P1.x = P1X_default;
			P1.y = P1Y_default;
			P2.x = P2X_default;
			P2.y = P2Y_default;
			goto IP_Exit;
		}
		if (read_float(&p1.y, hd)){	/* x without y! */
			par_err_exit(2, cmd, hd);
			break;
			}
		if (read_float(&p2.x, hd)) {	/* No number found  */
			P2.x += p1.x - P1.x;
			P2.y += p1.y - P1.y;
			P1 = p1;
			goto IP_Exit_1;
		}
		if (read_float(&p2.y, hd)){	/* x without y! */
			par_err_exit(4, cmd, hd);
			break;
			}
		P1 = p1;
		P2 = p2;

	      IP_Exit:
		S1 = P1;
		S2 = P2;
	      IP_Exit_1:
		Q.x = (P2.x - P1.x) / (S2.x - S1.x);
		Q.y = (P2.y - P1.y) / (S2.y - S1.y);
		Diag_P1_P2 = HYPOT(P2.x - P1.x, P2.y - P1.y);
		CurrentLinePatLen = 0.04 * Diag_P1_P2;
		tp->width *= (P2.x - P1.x);
		tp->height *= (P2.y - P1.y);
		adjust_text_par();
		initp1p2 = 1;
		return;

	case IR:		/* input reference points P1,P2 as percentages of defaults */
		if (read_float(&p1.x, hd))	/* No number found  */
			return;	/* keep defaults */
		if (read_float(&p1.y, hd)){	/* x without y! */
			par_err_exit(2, cmd, hd);
			break;
			}
/*fprintf(stderr,"P1,P2 vor IR: %f %f, %f %f\n",P1.x,P1.y,P2.x,P2.y);*/

		mywidth = P2.x - P1.x;
		myheight = P2.y - P1.y;
		ftmp = p1.x;
		p1.x = P1.x;	/* need old value for computation of new P2 */
		P1.x = p1.x + ftmp / 100. * mywidth;
		ftmp = p1.y;
		p1.y = P1.y;
		P1.y = p1.y + ftmp / 100. * myheight;

		if (read_float(&p2.x, hd)) {	/* No number found  */
			P2.x = P1.x + mywidth;	/* P2 tracks new P1 to keep constant size */
			P2.y = P1.y + myheight;
/*fprintf(stderr,"P1,P2 nach IR: %f %f, %f %f\n",P1.x,P1.y,P2.x,P2.y);*/
			initp1p2 = 1;
			return;
		}
		if (read_float(&p2.y, hd)){	/* x without y! */
			par_err_exit(4, cmd, hd);
			break;
			}
		P2.x = p1.x + p2.x / 100. * mywidth;
		P2.y = p1.y + p2.y / 100. * myheight;
		if (P1.x == P2.x)
			P2.x = P2.x + 1.;
		if (P1.y == P2.y)
			P2.y = P2.y + 1.;
		fprintf(stderr, "P1,P2 nach IR: %f %f, %f %f\n", P1.x,
			P1.y, P2.x, P2.y);
		Q.x = (P2.x - P1.x) / (S2.x - S1.x);
		Q.y = (P2.y - P1.y) / (S2.y - S1.y);
		Diag_P1_P2 = HYPOT(P2.x - P1.x, P2.y - P1.y);
		CurrentLinePatLen = 0.04 * Diag_P1_P2;
		tp->width *= (P2.x - P1.x);
		tp->height *= (P2.y - P1.y);
		adjust_text_par();
		initp1p2 = 1;
		return;

	case IW:
		iwflag = 1;
		if (read_float(&C1.x, hd)) {	/* No number found  */
			if (P1.x == P1X_default && P1.y == P1Y_default
			    && P2.x == P2X_default
			    && P2.y == P2Y_default) {
				iwflag = 0;
				if (HAS_CLIP(pg->xx_mode)) {
				  PlotCmd_to_tmpfile(NOCLIP);
				}
				break;
			}
			C1 = P1;
			C2 = P2;
if (scale_flag){
C1=S1;
C2=S2;
}
			if (rotate_flag && !ps_flag) {
				switch ((int) fabs(rot_tmp)) {
				case 90:
				case 270:
					ftmp = C1.x;
					C1.x = C1.y;
					C1.y = ftmp;
					ftmp = C2.x;
					C2.x = C2.y;
					C2.y = ftmp;
					break;
				default:
					break;
				}
			}
		} else {
			if (read_float(&C1.y, hd)){	/* x without y! */
				par_err_exit(2, cmd, hd);
				break;
				}
			if (read_float(&C2.x, hd)){	/* No number found  */
				par_err_exit(3, cmd, hd);
				break;
				}
			if (read_float(&C2.y, hd)){	/* x without y! */
				par_err_exit(4, cmd, hd);
				break;
				}
		}
/*
  fprintf (stderr," clip limits1 (%f,%f)(%f,%f)\n",C1.x,C1.y,C2.x,C2.y);
  fprintf (stderr," sc limits1 (%f,%f)(%f,%f)\n",S1.x,S1.y,S2.x,S2.y);
*/  
		if (scale_flag ) {
			User_to_Plotter_coord(&C1, &C1);
			User_to_Plotter_coord(&C2, &C2);
		}
/*
  fprintf (stderr," clip limits2 (%f,%f)(%f,%f)\n",C1.x,C1.y,C2.x,C2.y);
*/

		if (C2.x < C1.x) {
			ftmp = C2.x;
			C2.x = C1.x;
			C1.x = ftmp;
		}
		if (C2.y < C1.y) {
			ftmp = C2.y;
			C2.y = C1.y;
			C1.y = ftmp;
		}

		C1.x -= pg->extraclip;
		C1.y -= pg->extraclip;
		C2.x += pg->extraclip;
		C2.y += pg->extraclip;

		if (HAS_CLIP(pg->xx_mode)) {
		  PlotCmd_to_tmpfile(CLIP);
		  if (rotate_flag) {	/* hp2xx-specific global rotation       */
		    p1.x = rot_cos * C1.x - rot_sin * C1.y;
		    p1.y = rot_sin * C1.x + rot_cos * C1.y;
    		    HPGL_Pt_to_tmpfile(&p1);

		    p1.x = rot_cos * C2.x - rot_sin * C1.y;
		    p1.y = rot_sin * C2.x + rot_cos * C1.y;
    		    HPGL_Pt_to_tmpfile(&p1);

		    p1.x = rot_cos * C2.x - rot_sin * C2.y;
		    p1.y = rot_sin * C2.x + rot_cos * C2.y;

		    HPGL_Pt_to_tmpfile(&p1);
		    
		    p1.x = rot_cos * C1.x - rot_sin * C2.y;
		    p1.y = rot_sin * C1.x + rot_cos * C2.y;
    		    HPGL_Pt_to_tmpfile(&p1);
		  } else {
		    HPGL_Pt_to_tmpfile(&C1);
		    p1.x=C2.x;
 		    p1.y=C1.y;
		    HPGL_Pt_to_tmpfile(&p1);
		    HPGL_Pt_to_tmpfile(&C2);
		    p1.x=C1.x;
 		    p1.y=C2.y;
		    HPGL_Pt_to_tmpfile(&p1);
		  }
		  iwflag=0;
		}
		break;
	case OE:
		Eprintf("%d\n",lasterror);
		lasterror=0;
		break;	
	case OI:
		Eprintf("HP2XX-%s\r",VERS_NO);
		break;
	case OP:		/* Output reference Points P1,P2 */
		if (!silent_mode) {
			Eprintf("\nP1 = (%g, %g)\n", P1.x, P1.y);
			Eprintf("P2 = (%g, %g)\n", P2.x, P2.y);
		}
		initp1p2=0;
		break;
	case OS:
		statusbyte = pen_down + 2*initp1p2 + 8*initialized + 16;
		if (lasterror >0) statusbyte += 32;
		Eprintf("%c\n",statusbyte);
		initialized=0;
		break;
	case OW:		/* Output clip box  */
		if (!silent_mode) {
			Eprintf("\nC1 = (%g, %g)\n", C1.x, C1.y);
			Eprintf("C2 = (%g, %g)\n", C2.x, C2.y);
		}
		break;

	case AF:
	case AH:
	case PG:		/* new PaGe                     */
		/* record ON happens only once! */
	  fprintf(stderr, "\n\nPG CALLED\n");
	        if (!record_off) {

		  cdot(0,NULL,0);
		}
		page_number++;
/*		fprintf(stderr, "PG: page_number now %d\n", page_number);*/
		record_off = (first_page > page_number)
		    || ((last_page < page_number) && (last_page > 0));
		pg_flag = TRUE;
		return;
		break;

	case EA:		/* Edge Rectangle absolute */
		rects(pg,plot_rel = FALSE, 0, pt.width[pen], hd);
		tp->CR_point = HP_pos;
		break;

	case ER:		/* Edge Rectangle relative */
		rects(pg,TRUE, 0, 0., hd);
		tp->CR_point = HP_pos;
		break;

	case RA:		/* Fill Rectangle absolute */
		rects(pg,plot_rel = FALSE, pg->nofill ? 0 : 1, pt.width[pen], hd);
		tp->CR_point = HP_pos;
		break;
	case RF:
		if (read_float(&ftmp,hd)) {	/* just RF */
		memset(pattern[0],1,256*256);
		memset(pattern[1],1,256*256);
		memset(pattern[2],1,256*256);
		memset(pattern[3],1,256*256);
		memset(pattern[4],1,256*256);
		memset(pattern[5],1,256*256);
		memset(pattern[6],1,256*256);
		memset(pattern[7],1,256*256);
		break;		/* default all patterns to solid*/
		}
		if (ftmp <0 || ftmp>7) break; /* we only have 8 patterns - ignore */
		pat=(int)ftmp;
		if (read_float(&ftmp,hd)){	/* no width */
		memset(pattern[pat],1,256*256);
		break; 		/* default this pattern to solid*/
		}
		pw[pat]=(int)ftmp;
		if (read_float(&ftmp,hd)){	/* width but no height -invalid */
			par_err_exit(3, cmd, hd);
			break;
			}
		ph[pat]=(int)ftmp;	
		for (i=0;i<ph[pat];i++)
			for(j=0;j<pw[pat];j++){ read_float(&ftmp,hd);pattern[pat][i][j]=(unsigned char)ftmp;}
#if 0	
			for (i=0;i<ph[pat];i++) {
				for(j=0;j<pw[pat];j++) {
				if (pattern[pat][i][j] == 0) printf(" ");
				else 
				printf("#");
				}
				printf("\n");
			}
#endif			
		break;
	case RR:		/* Fill Rectangle relative */
		rects(pg,plot_rel = TRUE, pg->nofill ? 0 : 1, pt.width[pen], hd);
		tp->CR_point = HP_pos;
		break;

	case RT:		/* Relative arc, through Three points */
		tarcs(TRUE, hd);
		break;

	case LT:		/* Line Type:                   */
		if (read_float(&p1.x, hd))	/* just LT;     */
			CurrentLineType = LT_solid;
		else {
			if ((((int) p1.x) >= LT_MIN)
			    && (((int) p1.x) < LT_ZERO))
				CurrentLineType = LT_adaptive;
			else if (((int) p1.x) == LT_ZERO)
				CurrentLineType = LT_plot_at;
			else if ((((int) p1.x) > LT_ZERO)
				 && (((int) p1.x) <= LT_MAX))
				CurrentLineType = LT_fixed;
			else {
				Eprintf("Illegal line type:\t%d\n",
					(int) p1.x);
				CurrentLineType = LT_solid;	/* set to something sane */
			}
			CurrentLinePattern = (int) p1.x;

			if (!read_float(&p1.y, hd)) {	/* optional pattern length?     */
				if (p1.y <= 0.0)
					Eprintf
					    ("Illegal pattern length:\t%g\n",
					     p1.y);
				else {
					Diag_P1_P2 =
					    HYPOT(P2.x - P1.x,
						  P2.y - P1.y);

					if (!read_float(&ftmp, hd)) {
						if (ftmp == 1.0) {
							CurrentLinePatLen = p1.y * 40;	/* absolute */
						} else {
							CurrentLinePatLen = Diag_P1_P2 * p1.y / 100.0;	/* relative */
						}
					} else {
						CurrentLinePatLen = Diag_P1_P2 * p1.y / 100.0;	/* relative */
					}
				}
			}
		}

		break;

	case SC:		/* Input Scale Points S1,S2     */
		User_to_Plotter_coord(&p_last, &p_last);
		if (read_float(&p1.x, hd)) {	/* No number found  */
			S1.x = P1X_default;
			S1.y = P1Y_default;
			S2.x = P2X_default;
			S2.y = P2Y_default;
			scale_flag = FALSE;
			Q.x = Q.y = 1.0;
			break;
		}
		if (read_float(&p2.x, hd)){	/* x without y! */
			par_err_exit(2, cmd, hd);
			break;
			}
		if (read_float(&p1.y, hd)){	/* No number found  */
			par_err_exit(3, cmd, hd);
			break;
			}
		if (read_float(&p2.y, hd)){	/* x without y! */
			par_err_exit(4, cmd, hd);
			break;
			}
		if (p1.x == p2.x || p1.y == p2.y) {	/* min must differ from max */
			if (!silent_mode)
				Eprintf
				    ("Warning: Invalid SC command parameters -- ignored\n");
			lasterror = 3;
/*			Q.x = Q.y = 1.0;*/
			break;
		}
		S1.x = p1.x;
		S1.y = p1.y;
		S2.x = p2.x;
		S2.y = p2.y;
		if (read_float(&ftmp, hd))
			ftmp = 0;	/*scaling defaults to type 0 */
		switch ((int) ftmp) {
		case 0:	/* anisotropic scaling */
			Q.x = (P2.x - P1.x) / (S2.x - S1.x);
			Q.y = (P2.y - P1.y) / (S2.y - S1.y);
			break;

		case 1:	/* isotropic scaling */
			if (read_float(&ftmp, hd))	/* percentage of unused space on the left */
				ftmp = 50.0;	/* of the isotropic area defaults to 50%  */
			Q.x = (P2.x - P1.x) / (S2.x - S1.x);
			Q.y = (P2.y - P1.y) / (S2.y - S1.y);
			if (Q.x < Q.y) {
				if (read_float(&ftmp, hd))
					ftmp = 50.0;	/* percentage of unused space below the plot */
				S1.y +=
				    ftmp * ((P2.y - P1.y) / Q.y -
					    (P2.y - P1.y) / Q.x) / 100.0;
				Q.y = Q.x;
				S2.y = S1.y + (P2.y - P1.y) / Q.y;
			} else {
				S1.x +=
				    ftmp * ((P2.x - P1.x) / Q.x -
					    (P2.x - P1.x) / Q.y) / 100.0;
				read_float(&ftmp, hd);	/* mandatory 'bottom' value is unused */
				Q.x = Q.y;
				S2.x = S1.x + (P2.x - P1.x) / Q.x;
			}
			break;
		case 2:	/* point factor scaling */
			Q.x = S2.x;
			Q.y = S2.y;
			S2.x = S1.x + (P2.x - P1.x) / Q.x;
			S2.y = S1.y + (P2.y - P1.y) / Q.y;
			break;
		default:
			par_err_exit(0, cmd, hd);
			return;
		}
		scale_flag = TRUE;
		Plotter_to_User_coord(&p_last, &p_last);
		break;

	case SP:		/* Select pen: none/0, or number */
		old_pen = pen;
		thickness = 0.;	/* clear any PT setting (should we default to 0.3 here ??) */
		if (read_float(&p1.x, hd))	/* just SP;     */
			pen = 0;
		else {
			pen = (short) p1.x;
			if (pen == 0 && pg->mapzero > -1)
				pen = pg->mapzero;
		}
		if (pen < 0 || (int) pen > pg->maxpens) {
			Eprintf
			    ("\nIllegal pen number %d: replaced by %d\n",
			     pen, pen % pg->maxpens);
			lasterror=3;
			n_unexpected++;
			pen = pen % pg->maxpens;
		}
		if (old_pen != pen) {
		        cdot(0,NULL,0);
			if ((fputc(SET_PEN, td) == EOF)
			    || (fputc(pen, td) == EOF)) {
				PError("Writing to temporary file:");
				Eprintf("Error @ Cmd %ld\n", vec_cntr_w);
				exit(ERROR);
			}
		}
		if (pen)
			pens_in_use[pen] = 1;
		pg->maxcolor = MAX(pg->maxcolor, (int) pen);
/*              pens_in_use |= (1 << (pen-1)); */
		break;

	case BP:		/* Begin Plot */
		for (;;) {
			if (read_float(&ftmp, hd)) {	/* No number found */
				break;
			} else {
				switch ((int) ftmp) {
				case 1:	/* picture name follows */
					tmpstr[0] = fgetc(hd);	/* skip comma */
					tmpstr[0] = fgetc(hd);
					if (!silent_mode)
						fprintf(stderr,
							"HPGL picture name: %c",
							tmpstr[0]);
					if (tmpstr[0] == '"') {
						tmpstr[0] = ' ';
						do {
							tmpstr[0] =
							    fgetc(hd);
							if (!silent_mode)
								fputc
								    (tmpstr
								     [0],
								     stderr);
						}
						while (tmpstr[0] != '"');
					}
					if (!silent_mode)
						fprintf(stderr, "\n");
					break;
				case 2:	/* number of copies */
				case 3:	/* disposition code */
				case 4:	/* render unfinished */
				case 5:	/* autorotation */
					if (read_float(&ftmp, hd))
						break;
					break;
				default:
					break;
				}
			}
		}
		/* fall through to initialization code now */
	case DF:		/* Set to default               */
	case IN:		/* Initialize */
		reset_HPGL();
		if (HAS_POLY(pg->xx_mode)) {
		  PlotCmd_to_tmpfile(OP_PBUF);
		  PlotCmd_to_tmpfile(CL_PBUF);
		}
		if (HAS_CLIP(pg->xx_mode)) {
		  PlotCmd_to_tmpfile(NOCLIP);
		}
		tp->CR_point = HP_pos;
		break;
	case RO:
		if (read_float(&ftmp, hd)) {	/* No number found  */
			break;
		} else {
			/*if (!silent_mode)
			   fprintf (stderr, "RO encountered, rotating P1,P2 by %f\n", ftmp);
			 */
			rotate_flag = 1;
			rot_ang += ftmp;
			rot_tmp = ftmp;
			switch ((int) ftmp) {
			case 90:
			case 270:
				ftmp = M.x;
				M.x = M.y;
				M.y = ftmp;
				break;
			case 0:
			case 180:
				break;
			default:
				rotate_flag = 0;
				break;
			}

			/* if (!silent_mode)
			   fprintf (stderr, "cumulative rot_ang now %f\n", rot_ang); */
			rot_cos = cos(M_PI * rot_ang / 180.0);
			rot_sin = sin(M_PI * rot_ang / 180.0);

			if (ps_flag) {	/* transform extents from previous PS statement */
fprintf(stderr,"PS/RO\n");
				lxmin = 1e10;
				lymin = 1e10;
				lxmax = 1e-10;
				lymax = 1e-10;

				p1.x = 0;
				p1.y = 0;
				if (scale_flag)	/* Rescaling    */
					User_to_Plotter_coord(&p1, &p2);
				else
					p2 = p1;	/* Local copy   */
				HP_pos = p2;	/* Actual plotter pos. in plotter coord */
				ftmp = rot_cos * p2.x - rot_sin * p2.y;
				p2.y = rot_sin * p2.x + rot_cos * p2.y;
				p2.x = ftmp;
				lxmin = MIN(p2.x, lxmin);
				lymin = MIN(p2.y, lymin);
				lxmax = MAX(p2.x, lxmax);
				lymax = MAX(p2.y, lymax);
				p1.x = M.x;
				p1.y = M.y;
				if (scale_flag)	/* Rescaling    */
					User_to_Plotter_coord(&p1, &p2);
				else
					p2 = p1;	/* Local copy   */
				HP_pos = p2;	/* Actual plotter pos. in plotter coord */
				ftmp = rot_cos * p2.x - rot_sin * p2.y;
				p2.y = rot_sin * p2.x + rot_cos * p2.y;
				p2.x = ftmp;
				lxmin = MIN(p2.x, lxmin);
				lymin = MIN(p2.y, lymin);
				lxmax = MAX(p2.x, lxmax);
				lymax = MAX(p2.y, lymax);
			}
		}
		break;
	case BL:		/* Buffer label string          */
		read_string(strbuf, hd);
		break;
	case CP:		/* Char Plot (rather: move)     */
		if (read_float(&p1.x, hd)) {	/* No number found  */
			plot_string("\n\r", LB_direct, pen);
			return;
		} else if (read_float(&p1.y, hd)){
			par_err_exit(2, cmd, hd);
			break;
			}
		p2.x =
		    p1.x * tp->chardiff.x - p1.y * tp->linediff.x +
		    HP_pos.x;
		p2.y =
		    p1.x * tp->chardiff.y - p1.y * tp->linediff.y +
		    HP_pos.y;
		Pen_action_to_tmpfile(MOVE_TO, &p2, FALSE);
		break;
	case DI:		/* Char plot Dir (absolute)     */
		if (read_float(&p1.x, hd)) {	/* No number found  */
			tp->dir = 0.0;
			tp->CR_point = HP_pos;
			adjust_text_par();
			break;
		}
		if (read_float(&p1.y, hd)){	/* x, but not y */
			par_err_exit(2, cmd, hd);
			break;
			}
		if ((p1.x == 0.0) && (p1.y == 0.0)){
			par_err_exit(0, cmd, hd);
			break;
			}
		tp->dir = atan2(p1.y, p1.x);
		tp->CR_point = HP_pos;
		adjust_text_par();
		break;
	case DR:		/* Char plot Dir (rel P1,P2)    */
		if (read_float(&p1.x, hd)) {	/* No number found  */
			tp->dir = 0.0;
			tp->CR_point = HP_pos;
			adjust_text_par();
			break;
		}
		if (read_float(&p1.y, hd)){
			par_err_exit(2, cmd, hd);	/* x, but not y */
			break;
			}
		if ((p1.x == 0.0) && (p1.y == 0.0)){
			par_err_exit(0, cmd, hd);
			break;
			}
		tp->dir =
		    atan2(p1.y * (P2.y - P1.y), p1.x * (P2.x - P1.x));
		tp->CR_point = HP_pos;
		adjust_text_par();
		break;
	case DT:		/* Define string terminator     */
		StrTerm = getc(hd);
		if (StrTerm == ';') {	/*just DT */
			StrTerm = ETX;
			StrTermSilent = 1;
			break;
		}
		if (read_float(&ftmp, hd)) {
			StrTermSilent = 1;
		} else
			StrTermSilent = (short) ftmp;
		break;
	case DV:		/* Text direction vertical      */
		if (read_float(&ftmp, hd) || ftmp == 0)
			mode_vert = 0;
		else
			mode_vert = 1;
		break;
	case ES:		/* Extra Space                  */
		if (read_float(&tp->espace, hd)) {	/* No number found */
			tp->espace = 0.0;
			tp->eline = 0.0;
		} else if (read_float(&tp->eline, hd))
			tp->eline = 0.0;	/* Supply default       */
		adjust_text_par();
		break;
	case LA:		/* Line Attributes */
		set_line_attr(hd);
		break;
	case LB:		/* Label string                 */
		read_string(strbuf, hd);
		plot_string(strbuf, LB_direct, pen);
		/*
		 * Bug fix by W. Eric Norum:
		 * Update the position so that subsequent `PR's will work.
		 */
		if (scale_flag)
			Plotter_to_User_coord(&HP_pos, &p_last);
		else
			p_last = HP_pos;
		break;
	case LO:		/* Label Origin                 */
		if (read_float(&p1.x, hd))	/* No number found */
			tp->orig = 1;
		else {
			tp->orig = (int) p1.x;
			if (tp->orig < 1 || tp->orig == 10
			    || tp->orig > 19)
				tp->orig = 1;	/* Error        */
		}
		adjust_text_par();
		break;
	case PB:		/* Plot Buffered label string   */
		plot_string(strbuf, LB_buffered, pen);
		break;
	case SI:		/* Char cell Sizes (absolute)   */
		if (read_float(&tp->width, hd)) {	/* No number found */
			tp->width = 0.187;	/* [cm], A4     */
			tp->height = 0.269;	/* [cm], A4     */
		} else {
			if (read_float(&tp->height, hd)){
				par_err_exit(2, cmd, hd);
				break;
				}
			if ((tp->width == 0.0) || (tp->height == 0.0)){
				par_err_exit(0, cmd, hd);
				break;
				}
		}
		tp->width *= 400.0;	/* [cm] --> [plotter units]        */
		tp->height *= 400.0;	/* [cm] --> [plotter units]        */
		adjust_text_par();
		break;
	case SL:		/* Char Slant                   */
		if (read_float(&tp->slant, hd))	/* No number found     */
			tp->slant = 0.0;
		adjust_text_par();
		break;
	case SM:		/* Symbol Mode                  */
		read_symbol_char(hd);
		break;
	case SR:		/* Character  sizes (Rel P1,P2) */
		if (read_float(&tp->width, hd)) {	/* No number found */
			tp->width = 0.75;	/* % of (P2-P1)_x    */
			tp->height = 1.5;	/* % of (P2-P1)_y    */
		} else {
			if (read_float(&tp->height, hd)){
				par_err_exit(2, (short) cmd, hd);
				break;
				}
			if ((tp->width == 0.0) || (tp->height == 0.0)){
				par_err_exit(0, (short) cmd, hd);
				break;
				}
		}
		tp->width *= (P2.x - P1.x) / 100.0;	/* --> [pl. units]     */
		tp->height *= (P2.y - P1.y) / 100.0;
		adjust_text_par();
		break;
	case SA:		/* Select designated alternate charset */
		if (tp->altfont) 
			tp->font = tp->altfont;
		else		/* Was never designated, default to 0 */
			tp->font = 0;
		tp->strokewidth = tp->astrokewidth;
		tp->variable = tp->avariable;
		break;
	case SD:
		if (read_float(&ftmp, hd))	/* just SD - defaults */
			tp->stdfont = 0;
		else {
			do {
			switch ((int) ftmp) {
			case 1:	/* charset */
				if (read_float(&csfont, hd)){
					par_err_exit(2, cmd, hd);
					return;
					}
				else
					tp->stdfont = (int) csfont;
				break;
			case 2:	/* fixed or variable spacing */
				if (read_float(&csfont, hd)){
					par_err_exit(2, cmd, hd);
					return;
					}
#ifdef STROKED_VARFONTS
				else if ((int) csfont == 1) tp->svariable=csfont;
#else									
				else if ((int) csfont == 1 && !silent_mode)
					fprintf(stderr,
						"only fixed fonts available\n");
#endif
				break;
			case 6:	/* stroke weight */
				if (read_float(&csfont, hd)){
					par_err_exit(2, cmd, hd);
					return;
					}
				if (csfont == 9999.) {
					tp->sstrokewidth = csfont;
					tp->strokewidth = csfont; /* update current setting in case we never see SS */
				} else {
					if (csfont < -7. || csfont > 7.)
						csfont = 0.;
					tp->sstrokewidth = 0.11 + csfont / 70.;	/* 0.01 ... 0.21 mm */
				}
				break;
			case 3:	/* font pitch */
			case 4:	/* font height */
			case 5:	/* posture */
			case 7:	/* typeface */
				if (read_float(&csfont, hd)){
					par_err_exit(2, cmd, hd);
					return;
					}
				else if (!silent_mode)
					fprintf(stderr,
						"pitch/height/posture/typeface unsupported\n");
				break;
			default:
				par_err_exit(1, cmd, hd);
				return;
			}
		} while (!read_float(&ftmp,hd));	
		}
		break;
	case SS:		/* Select designated standard character set */
		if (tp->stdfont)
			tp->font = tp->stdfont;
		else		/* Was never designated, default to 0 */
			tp->font = 0;
		tp->strokewidth = tp->sstrokewidth;
		tp->variable = tp->svariable;
		break;
	case UC:		/* User defined character       */
		plot_user_char(hd, pen);
		break;
	case UL:		/* User defined line style      */
		set_line_style_by_UL(hd);
		break;
	case MG:
	case WD:		/* Write string to display      */
		ctmp = fgetc(hd);
		if (ctmp != '"') {
			par_err_exit(1, cmd, hd);
			return;
		}
		do {
			ctmp=fgetc(hd);
			if (!silent_mode)
				fputc(ctmp,stderr);
		}
		while (ctmp != '"');
		break;
	case VS:
		if (read_float(&ftmp, hd))	/* Just VS */
			break;
		if (read_float(&ftmp, hd))	/* uniform speed */
			break;
		if (read_float(&ftmp, hd))	/* speed for given pen */
			break;
	default:		/* Skip unknown HPGL command: */
		n_unknown++;
		lasterror = 1;
		if (!silent_mode)
			Eprintf("  %c%c: ignored  ", cmd >> 8, cmd & 0xFF);
		if (cmd == EOF) {
			n_unexpected++;
			if (!silent_mode)
				Eprintf("\nUnexpected EOF!\t");
		}
		break;
	}
}


void read_HPGL(GEN_PAR * pg, const IN_PAR * pi)
/**
 ** This routine is the high-level entry for HP-GL processing.
 ** It reads the input stream character-by-character, identifies
 ** ESC. commands (device controls) and HP-GL mnemonics, reads
 ** parameters (if expected), and initiates processing of these
 ** commands. It finally reports on this parsing process.
 **/
{
	int c;
	int cmd;

	vec_cntr_r = 0L;
	vec_cntr_w = 0L;
	n_unexpected = 0;
	pg->n_commands = 0;
	n_unknown = 0;

	if ((c = getc(pi->hd)) == EOF)
		return;
	else
		ungetc(c, pi->hd);

	if (!pg_flag)
		init_HPGL(pg, pi);
/*
        else
		init_text_par();
*/	
	if (!pg->quiet)
		Eprintf("\nReading HPGL file\n");

  /**
   ** MAIN parser LOOP!!
   **/
	while ((c = getc(pi->hd)) != EOF) {
		switch (c) {
#ifdef MUTOH_KLUGE
		case '\a':
			Eprintf("Mutoh header found\n");
			read_ESC_cmd(pi->hd, FALSE, pg);	/* ESC sequence */
			break;
#endif
		case ESC:
			read_ESC_cmd(pi->hd, TRUE, pg);	/* ESC sequence */
			if (pg_flag==TRUE) {	/*kludge for Esc%0A as pagebreak */
			pg_flag=FALSE;
			goto END;
			}
			break;
		default:
			if ((c < 'A') || (c > 'z')
			    || ((c > 'Z') && (c < 'a')))
				break;
			if (c == 'P') {
				if ((cmd = getc(pi->hd)) == 'G') {
				        if (!record_off) cdot(0,NULL,0);
					page_number++;
		fprintf(stderr, "stream-reading PG: page_number now %d\n", page_number);
					record_off =
					    (first_page > page_number)
					    || ((last_page < page_number)
						&& (last_page > 0));
				pg_flag = TRUE;
				goto END;
				} else {
					if (cmd == EOF)
						return;
					ungetc(cmd, pi->hd);
				}
			}
			if (c == 'N') {
				if ((cmd = getc(pi->hd)) == 'R') {
/*	  fprintf(stderr,"***NR***\n");'*/
				        if (!record_off) cdot(0,NULL,0);
					page_number++;
/*		fprintf(stderr, "stream-reading NR: page_number now %d\n", page_number);*/
					record_off =
					    (first_page > page_number)
					    || ((last_page < page_number)
						&& (last_page > 0));
				} else {
					if (cmd == EOF)
						return;
					ungetc(cmd, pi->hd);
				}
			}
			if (c == 'A') {
				cmd = getc(pi->hd);
				if (cmd == 'F' || cmd == 'H') {
/*	  fprintf(stderr,"***AF/AH***\n");*/
				        if (!record_off) cdot(0,NULL,0);
					page_number++;
/*		fprintf(stderr, "stream-reading AF/AH: page_number now %d\n", page_number);*/
					record_off =
					    (first_page > page_number)
					    || ((last_page < page_number)
						&& (last_page > 0));
				} else {
					if (cmd == EOF)
						return;
					ungetc(cmd, pi->hd);
				}
			}
			cmd = c << 8;
			if ((c = getc(pi->hd)) == EOF)
				return;
			if (c=='\n') c=getc(pi->hd);	
			if ((c < 'A') || (c > 'z')
			    || ((c > 'Z') && (c < 'a'))) {
				ungetc(c, pi->hd);
				break;
			}
			cmd |= (c & 0xFF);
			pg->n_commands++;
			read_HPGL_cmd(pg, cmd, pi->hd);
		}
	}
	if (c == EOF) {
	        if (!record_off) cdot(0,NULL,0);
		page_number++;
/*			fprintf(stderr, "EOF : page_number now %d\n", page_number);*/
	}
      END:
	if (!pg->quiet  && pg->n_commands > 1 ) {
		Eprintf("Page number %d of range %d - %d\n",
			page_number - 1, pi->first_page, pi->last_page);
		Eprintf("\nHPGL commands read: %d\n", pg->n_commands);
		Eprintf("HPGL command(s) ignored: %d\n", n_unknown);
		Eprintf("Unexpected event(s):  %d\n", n_unexpected);
		Eprintf("Internal command(s):  %ld\n", vec_cntr_w);
		if ((pi->first_page > page_number - 1)
		    || ((pi->last_page < page_number - 1)
			&& (pi->last_page > 0))) {
			pg->n_commands = -1;
			Eprintf
			    ("Page %d not drawn (outside selected range %d-%d)\n",
			     page_number - 1, pi->first_page,
			     pi->last_page);
		}
		Eprintf("Pens used: ");
/*      for (c=0; c < NUMPENS; c++, pens_in_use >>= 1)
   if (pens_in_use & 1)
 */
		for (c = 0; c < NUMPENS; c++)
			if (pens_in_use[c] == 1)
				Eprintf("%d ", c);
/*                      Eprintf ("%d ", c+1); */
		Eprintf("\nMax. number of pages: %d\n", page_number - 1);
	}
}




void
adjust_input_transform(const GEN_PAR * pg, const IN_PAR * pi, OUT_PAR * po)
{
/**
 ** The temporary input data of the temp. file may be re-used multiple
 ** times by calling this function with varying parameters,
 ** mainly in pi.
 **
 ** Some conversion factors for transformation from HP-GL coordinates
 ** (as given in the temp. file) into mm or pel numbers are set here.
 ** There are both global parameters and elemts of po set here.
 ** DPI-related factors only apply if the current mode is a raster mode.
 **
 ** # points (dots) in any direction = range [mm] * 1in/25.4mm * #dots/in
 **/

	double dot_ratio, Dx, Dy, tmp_w, tmp_h;
	char *dir_str;
	Dx = lxmax - lxmin;
	Dy = lymax - lymin;
	dot_ratio = (double) po->dpi_y / (double) po->dpi_x;
	po->width = pi->width;
	po->height = pi->height;
	po->xoff = pi->xoff;
	po->yoff = pi->yoff;

	/* Width  assuming given height:      */
	tmp_w = pi->height * Dx / Dy * pi->aspectfactor;
/*  tmp_w     = pi->height * Dx / Dx / pi->aspectfactor; */
	/* Height assuming given width:       */
	tmp_h = pi->width * Dy / Dx / pi->aspectfactor;

  /**
   ** EITHER width OR height MUST be the correct limit. The other will
   ** be adapted. Adaptation of both is inconsistent, except in truesize mode.
   **/

	if (pi->truesize) {
		po->width = Dx / 40.0;	/* Ignore -w, take natural HP-GL range  */
		po->height = Dy / 40.0;	/* Ignore -h, take natural HP-GL range  */
		po->HP_to_xdots = (float) (po->dpi_x / 1016.0);	/* dots per HP unit */
		po->HP_to_ydots = (float) (po->dpi_y / 1016.0);	/*  (1/40 mm)       */
		dir_str = "true sizes";
		if (pi->center_mode) {
			if (!pg->quiet) {
				fprintf(stderr,
					"trying to center image\n");
				fprintf(stderr,
					"po->width ?<? tmp_w: %f %f\n",
					po->width, tmp_w);
				fprintf(stderr,
					"po->height ?<? tmp_h: %f %f\n",
					po->height, tmp_h);
			}
			if (po->width < tmp_w)
				po->xoff += (tmp_w - po->width) / 2.0;
			if (po->height < tmp_h)
				po->yoff += (tmp_h - po->height) / 2.0;
		}

	} else {
/*    if (po->width > tmp_w) */
		if (Dy > Dx) {
			po->HP_to_ydots =
			    (float) (po->dpi_y * po->height) / Dy / 25.4;
			po->HP_to_xdots =
			    po->HP_to_ydots * pi->aspectfactor / dot_ratio;
			if (pi->center_mode)
				po->xoff += (po->width - tmp_w) / 2.0;	/* by L. Lowe   */
			po->width = tmp_w;
			dir_str = "width adapted";	/* Height fits, adjust width    */
		} else {
			po->HP_to_xdots =
			    (float) (po->dpi_x * po->width) / Dx / 25.4;
			po->HP_to_ydots =
			    po->HP_to_xdots * dot_ratio / pi->aspectfactor;
			if (pi->center_mode)
				po->yoff += (po->height - tmp_h) / 2.0;	/* by L. Lowe   */
			po->height = tmp_h;
			dir_str = "height adapted";	/* Width  fits, adjust height   */
		}
	}

	if (!pg->quiet) {
		Eprintf("\nWidth  x  height: %5.2f x %5.2f mm, %s\n",
			po->width, po->height, dir_str);
		Eprintf("Coordinate range: (%g, %g) ... (%g, %g)\n",
			lxmin, lymin, lxmax, lymax);
		Eprintf("Offsets: (%g, %g)\n",
			po->xoff, po->yoff);
	}

	po->xmin = lxmin;
	po->xmax = lxmax;
	po->ymin = lymin;
	po->ymax = lymax;
}

#ifdef EMF
void reset_tmpfile(void)
{
	(void) lseek(fileno(td), 0L, SEEK_SET);
	if (vec_cntr_r)
		again = TRUE;
	vec_cntr_r = 0;
}
#endif

PlotCmd PlotCmd_from_tmpfile(void)
{
	PlotCmd cmd;

	if (!silent_mode && !again)
		switch (vec_cntr_r++) {
		case 0:
			Eprintf("\nProcessing Cmd: ");
			break;
		case 1:
			Eprintf("1 ");
			break;
		case 2:
			Eprintf("2 ");
			break;
		case 5:
			Eprintf("5 ");
			break;
		case 10:
			Eprintf("10 ");
			break;
		case 20:
			Eprintf("20 ");
			break;
		case 50:
			Eprintf("50 ");
			break;
		case 100:
			Eprintf("100 ");
			break;
		case 200:
			Eprintf("200 ");
			break;
		case 500:
			Eprintf("500 ");
			break;
		case 1000:
			Eprintf("1k ");
			break;
		case 2000:
			Eprintf("2k ");
			break;
		case 5000:
			Eprintf("5k ");
			break;
		case 10000:
			Eprintf("10k ");
			break;
		case 20000:
			Eprintf("20k ");
			break;
		case 50000L:
			Eprintf("50k ");
			break;
		case 100000L:
			Eprintf("100k ");
			break;
		case 200000L:
			Eprintf("200k ");
			break;
		case 500000L:
			Eprintf("500k ");
			break;
		}

	switch (cmd = fgetc(td)) {
	case NOP:
	case MOVE_TO:
	case DRAW_TO:
	case PLOT_AT:
	case SET_PEN:
	case DEF_PW:
	case DEF_PC:
	case DEF_LA:
	case OP_PBUF:
	case CL_PBUF:
	case SUBPOLY:
	case EDGE_POLY:
	case NZFILL_POLY:
	case EOFILL_POLY:
	case CLIP:
	case NOCLIP:
	case FILL_TYPE:
		return cmd;
/*	case (unsigned int) EOF:*/
	default:
		return CMD_EOF;
	}
}



void HPGL_Pt_from_tmpfile(HPGL_Pt * pf)
{
	if (fread((VOID *) pf, sizeof(*pf), 1, td) != 1) {
		PError("HPGL_Pt_from_tmpfile");
		Eprintf("Error @ Cmd %ld\n", vec_cntr_r);
		exit(ERROR);
	}
	if (pf->x < lxmin || pf->x > lxmax) {
		Eprintf
		    ("HPGL_Pt_from_tmpfile: x out of range (%g not in [%g, %g])\n",
		     pf->x, lxmin, lxmax);
	if (pf->x <lxmin)  
			pf->x=lxmin; 
		else	
			pf->x=lxmax;
	}	     
	if (pf->y < lymin || pf->y > lymax) {
		Eprintf
		    ("HPGL_Pt_from_tmpfile: y out of range (%g not in [%g, %g])\n",
		     pf->y, lymin, lymax);
	if (pf->y <lymin)  
			pf->y=lymin; 
		else	
			pf->y=lymax; 
	}    
}
