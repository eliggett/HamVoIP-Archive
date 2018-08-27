/*
 *  Hamlib Kenwood backend - TM-D710 description
 *  Copyright (c) 2011 by Charles Suprin
 *
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#include "hamlib/rig.h"
#include "kenwood.h"
#include "th.h"
#include "tones.h"
#include "num_stdio.h"
#include "misc.h"

static int tmd710_get_freq(RIG *rig, vfo_t vfo, freq_t *freq);
static int tmd710_set_freq(RIG *rig, vfo_t vfo, freq_t freq);
static int tmd710_set_vfo (RIG *rig, vfo_t vfo);
static int tmd710_get_vfo(RIG *rig, vfo_t *vfo);
static int tmd710_set_ts(RIG *rig, vfo_t vfo, shortfreq_t ts);
static int tmd710_get_ts(RIG *rig, vfo_t vfo, shortfreq_t *ts);
static int tmd710_set_ctcss_tone(RIG *rig, vfo_t vfo, tone_t tone);
static int tmd710_get_ctcss_tone(RIG *rig, vfo_t vfo, tone_t *tone);
static int tmd710_set_ctcss_sql(RIG *rig, vfo_t vfo, tone_t tone);
static int tmd710_get_ctcss_sql(RIG *rig, vfo_t vfo, tone_t *tone);
static int tmd710_set_mode(RIG *rig, vfo_t vfo, rmode_t mode, pbwidth_t width);
static int tmd710_get_mode(RIG *rig, vfo_t vfo, rmode_t *mode, pbwidth_t *width);
static int tmd710_set_rptr_shift(RIG *rig, vfo_t vfo, rptr_shift_t shift);
static int tmd710_get_rptr_shift(RIG *rig, vfo_t vfo, rptr_shift_t* shift);
static int tmd710_set_rptr_offs(RIG *rig, vfo_t vfo, shortfreq_t offset);
static int tmd710_get_rptr_offs(RIG *rig, vfo_t vfo, shortfreq_t* offset);

#define TMD710_MODES	  (RIG_MODE_FM|RIG_MODE_AM)
#define TMD710_MODES_TX (RIG_MODE_FM)

/* TBC */
#define TMD710_FUNC_ALL (RIG_FUNC_TSQL|   \
                       RIG_FUNC_AIP|    \
                       RIG_FUNC_MON|    \
                       RIG_FUNC_MUTE|   \
                       RIG_FUNC_SQL|    \
                       RIG_FUNC_TONE|   \
                       RIG_FUNC_TBURST| \
                       RIG_FUNC_REV|    \
                       RIG_FUNC_LOCK|   \
                       RIG_FUNC_ARO)

#define TMD710_LEVEL_ALL (RIG_LEVEL_STRENGTH| \
                        RIG_LEVEL_SQL| \
                        RIG_LEVEL_AF| \
                        RIG_LEVEL_RF|\
                        RIG_LEVEL_MICGAIN)

#define TMD710_PARMS	(RIG_PARM_BACKLIGHT|\
                        RIG_PARM_BEEP|\
                        RIG_PARM_APO)

#define TMD710_VFO_OP (RIG_OP_UP|RIG_OP_DOWN)

#define TMD710_CHANNEL_CAPS   \
            TH_CHANNEL_CAPS,\
            .flags=1,   \
            .dcs_code=1,    \
            .dcs_sql=1,

#define TMD710_CHANNEL_CAPS_WO_LO \
            TH_CHANNEL_CAPS,\
            .dcs_code=1,    \
            .dcs_sql=1,

/*
 * TODO: Band A & B
 */
#define TMD710_VFO (RIG_VFO_A|RIG_VFO_B)

static rmode_t tmd710_mode_table[KENWOOD_MODE_TABLE_MAX] = {
    [0] = RIG_MODE_FM,
    [1] = RIG_MODE_AM,
};

static struct kenwood_priv_caps  tmd710_priv_caps  = {
    .cmdtrm =  EOM_TH,   /* Command termination character */
    .mode_table = tmd710_mode_table,
};



/*
 * TM-D710 rig capabilities.
 *
 * specs: 
 * protocol: kd7dvd.us/equipment/tm-d710a/manuals/control_commands.pdf
 */
const struct rig_caps tmd710_caps = {
.rig_model =  RIG_MODEL_TMD710,
.model_name = "TM-D710",
.mfg_name =  "Kenwood",
.version =  TH_VER,
.copyright =  "LGPL",
.status =  RIG_STATUS_UNTESTED,
.rig_type =  RIG_TYPE_MOBILE|RIG_FLAG_APRS|RIG_FLAG_TNC,
.ptt_type =  RIG_PTT_RIG,
.dcd_type =  RIG_DCD_RIG,
.port_type =  RIG_PORT_SERIAL,
.serial_rate_min =  9600,
.serial_rate_max =  57600,
.serial_data_bits =  8,
.serial_stop_bits =  1,
.serial_parity =  RIG_PARITY_NONE,
.serial_handshake =  RIG_HANDSHAKE_NONE,
.write_delay =  0,
.post_write_delay =  0,
.timeout =  1000,
.retry =  3,

.has_get_func =  TMD710_FUNC_ALL,
.has_set_func =  TMD710_FUNC_ALL,
.has_get_level =  TMD710_LEVEL_ALL,
.has_set_level =  RIG_LEVEL_SET(TMD710_LEVEL_ALL),
.has_get_parm =  TMD710_PARMS,
.has_set_parm =  TMD710_PARMS,    /* FIXME: parms */
.level_gran = {
        [LVL_RAWSTR] = { .min = { .i = 0 }, .max = { .i = 7 } },
        [LVL_SQL] = { .min = { .i = 0 }, .max = { .i = 0x1f }, .step = { .f = 1./0x1f } },
        [LVL_RFPOWER] = { .min = { .i = 2 }, .max = { .i = 0 }, .step = { .f = 1./3. }  },
        [LVL_AF] = { .min = { .i = 0 }, .max = { .i = 0x3f }, .step = { .f = 1./0x3f } },
},
.parm_gran =  {},
.ctcss_list =  kenwood42_ctcss_list,
.dcs_list =  common_dcs_list,
.preamp =   { RIG_DBLST_END, },
.attenuator =   { RIG_DBLST_END, },
.max_rit =  Hz(0),
.max_xit =  Hz(0),
.max_ifshift =  Hz(0),
.vfo_ops =  TMD710_VFO_OP,
.scan_ops = RIG_SCAN_VFO,
.targetable_vfo =  RIG_TARGETABLE_NONE,
.transceive =  RIG_TRN_RIG,
.bank_qty =   0,
.chan_desc_sz =  8,

.chan_list = {
    {  1,  199, RIG_MTYPE_MEM ,  {TMD710_CHANNEL_CAPS}},  /* normal MEM */
    {  200,219, RIG_MTYPE_EDGE , {TMD710_CHANNEL_CAPS}}, /* U/L MEM */
    {  221,222, RIG_MTYPE_CALL,  {TMD710_CHANNEL_CAPS_WO_LO}},  /* Call 0/1 */
    RIG_CHAN_END,
       },
/*
 * TODO: Japan & TM-D700S, and Taiwan models
 */
.rx_range_list1 =  {
    {MHz(118),MHz(470),TMD710_MODES,-1,-1,RIG_VFO_A},
    {MHz(136),MHz(174),RIG_MODE_FM,-1,-1,RIG_VFO_B},
    {MHz(300),MHz(524),RIG_MODE_FM,-1,-1,RIG_VFO_B},
    {MHz(800),MHz(1300),RIG_MODE_FM,-1,-1,RIG_VFO_B},
	RIG_FRNG_END,
  }, /* rx range */
.tx_range_list1 =  {
    {MHz(144),MHz(146),TMD710_MODES_TX,W(5),W(50),RIG_VFO_A},
    {MHz(430),MHz(440),TMD710_MODES_TX,W(5),W(35),RIG_VFO_A},
	RIG_FRNG_END,
  }, /* tx range */

.rx_range_list2 =  {
    {MHz(118),MHz(470),TMD710_MODES,-1,-1,RIG_VFO_A},
    {MHz(136),MHz(174),RIG_MODE_FM,-1,-1,RIG_VFO_B},
    {MHz(300),MHz(524),RIG_MODE_FM,-1,-1,RIG_VFO_B},
    {MHz(800),MHz(1300),RIG_MODE_FM,-1,-1,RIG_VFO_B},	/* TODO: cellular blocked */
	RIG_FRNG_END,
  }, /* rx range */
.tx_range_list2 =  {
    {MHz(144),MHz(148),TMD710_MODES_TX,W(5),W(50),RIG_VFO_A},
    {MHz(430),MHz(450),TMD710_MODES_TX,W(5),W(35),RIG_VFO_A},
	RIG_FRNG_END,
  }, /* tx range */

.tuning_steps =  {
	 {TMD710_MODES,kHz(5)},
	 {TMD710_MODES,kHz(6.25)},
	 {TMD710_MODES,kHz(10)},
	 {TMD710_MODES,kHz(12.5)},
	 {TMD710_MODES,kHz(15)},
	 {TMD710_MODES,kHz(20)},
	 {TMD710_MODES,kHz(25)},
	 {TMD710_MODES,kHz(30)},
	 {TMD710_MODES,kHz(50)},
	 {TMD710_MODES,kHz(100)},
	 RIG_TS_END,
	},
        /* mode/filter list, remember: order matters! */
.filters =  {
		{RIG_MODE_FM, kHz(12)},
		{RIG_MODE_AM, kHz(9)},	/* TBC */
		RIG_FLT_END,
	},
.priv =  (void *)&tmd710_priv_caps,

.rig_init = kenwood_init,
.rig_cleanup = kenwood_cleanup,
.set_freq =  tmd710_set_freq,
.get_freq =  tmd710_get_freq,
.set_mode =  tmd710_set_mode,
.get_mode =  tmd710_get_mode,
.set_vfo =  tmd710_set_vfo,
.get_vfo =  tmd710_get_vfo,
.set_ts = tmd710_set_ts,
.get_ts = tmd710_get_ts,
.set_ctcss_tone =  tmd710_set_ctcss_tone,
.get_ctcss_tone =  tmd710_get_ctcss_tone,
.set_ctcss_sql =  tmd710_set_ctcss_sql,
.get_ctcss_sql =  tmd710_get_ctcss_sql,
//.set_split_vfo =  th_set_split_vfo,
//.get_split_vfo =  th_get_split_vfo,
//.set_dcs_sql =  th_set_dcs_sql,
//.get_dcs_sql =  th_get_dcs_sql,
//.set_mem =  th_set_mem,
//.get_mem =  th_get_mem,
//.set_channel =  th_set_channel,
//.get_channel =  th_get_channel,
//.set_trn =  th_set_trn,
//.get_trn =  th_get_trn,

//.set_func =  th_set_func,
//.get_func =  th_get_func,
//.set_level =  th_set_level,
//.get_level =  th_get_level,
//.set_parm =  th_set_parm,
//.get_parm =  th_get_parm,
//.get_info =  th_get_info,
//.get_dcd =  th_get_dcd,
//.set_ptt =  th_set_ptt,
//.vfo_op =  th_vfo_op,
//.scan   =  th_scan,

.set_rptr_shift = tmd710_set_rptr_shift,
.get_rptr_shift = tmd710_get_rptr_shift,
.set_rptr_offs = tmd710_set_rptr_offs,
.get_rptr_offs = tmd710_get_rptr_offs,

.decode_event =  th_decode_event,
};

/* structure for handling fo radio command */
typedef struct {
  int vfo;
  freq_t freq;
  int step;
  int shift;
  int reverse;
  int tone;
  int ct;
  int dsc;
  int tone_freq;
  int ct_freq;
  int dsc_val;
  int offset;
  int mode;
} tmd710_fo;


/* the d710 has a single command FO that queries and sets many values */
/* this pulls that string from the radio given a vfo */
/* push/pull language is uses for stuff inside 710 driver rather than get/set */
int 
tmd710_pull_fo(RIG * rig,vfo_t vfo, tmd710_fo *fo_struct) {
  char cmdbuf[50];
  char buf[50];
  int vfonum = 0;
  int retval;

  rig_debug(RIG_DEBUG_TRACE, "%s: called\n", __func__);

  switch (vfo) {
  case (RIG_VFO_CURR):
    vfonum = rig->state.current_vfo==RIG_VFO_B;
    break;
  case (RIG_VFO_A):
    vfonum = 0;
    break;
  case (RIG_VFO_B):
    vfonum = 1;
    break;

  }
  
  
  // if (vfo != RIG_VFO_CURR && vfo != rig->state.current_vfo)
  //return kenwood_wrong_vfo(__func__, vfo);
  snprintf(cmdbuf,49,"FO %d",vfonum);
  
  retval = kenwood_safe_transaction(rig, cmdbuf, buf, sizeof(buf), 49);
  if (retval != RIG_OK)
    return retval;
  
  retval = num_sscanf(buf, "FO %x,%"SCNfreq",%x,%x,%x,%x,%x,%x,%d,%d,%d,%d,%d", 
		      &fo_struct->vfo, &fo_struct->freq, 
		      &fo_struct->step, &fo_struct->shift, 
		      &fo_struct->reverse, &fo_struct->tone, 
		      &fo_struct->ct, &fo_struct->dsc, 
		      &fo_struct->tone_freq, &fo_struct->ct_freq,
		      &fo_struct->dsc_val, &fo_struct->offset,
		      &fo_struct->mode);
  if (retval != 13) {
    rig_debug(RIG_DEBUG_ERR, "%s: Unexpected reply '%s'\n", __func__, buf);
    return -RIG_ERJCTED;
  }
  return RIG_OK;
  
}

int 
tmd710_push_fo(RIG * rig,vfo_t vfo, tmd710_fo *fo_struct) {
  char cmdbuf[50];
  char buf[50];
  int  retval;
  
  rig_debug(RIG_DEBUG_TRACE, "%s: called\n", __func__);
  
  //if (vfo != RIG_VFO_CURR && vfo != rig->state.current_vfo)
  //  return kenwood_wrong_vfo(__func__, vfo);

  snprintf(cmdbuf,49,"FO %1d,%010.0f,%1d,%1d,%1d,%1d,%1d,%1d,%02d,%02d,%03d,%08d,%1d", 
	   fo_struct->vfo, fo_struct->freq, 
		      fo_struct->step, fo_struct->shift, 
		      fo_struct->reverse, fo_struct->tone, 
		      fo_struct->ct, fo_struct->dsc, 
		      fo_struct->tone_freq, fo_struct->ct_freq,
		      fo_struct->dsc_val, fo_struct->offset,
		      fo_struct->mode);
  
  retval = kenwood_safe_transaction(rig, cmdbuf, buf, sizeof(buf), 49);
  if (retval != RIG_OK)
    return retval;
  
  retval = num_sscanf(buf, "FO %x,%"SCNfreq",%x,%x,%x,%x,%x,%x,%d,%d,%d,%d,%d", 
		      &fo_struct->vfo, &fo_struct->freq, 
		      &fo_struct->step, &fo_struct->shift, 
		      &fo_struct->reverse, &fo_struct->tone, 
		      &fo_struct->ct, &fo_struct->dsc, 
		      &fo_struct->tone_freq, &fo_struct->ct_freq,
		      &fo_struct->dsc_val, &fo_struct->offset,
		      &fo_struct->mode);
  if (retval != 13) {
    rig_debug(RIG_DEBUG_ERR, "%s: Unexpected reply '%s'\n", __func__, buf);
    return -RIG_ERJCTED;
  }
  return RIG_OK;
  
}

/*
 * th_set_freq
 * Assumes rig!=NULL
 */
int
tmd710_set_freq(RIG *rig, vfo_t vfo, freq_t freq)
{
  int retval;
  tmd710_fo fo_struct;
  long freq5,freq625,freq_sent;
  int step;
  
  rig_debug(RIG_DEBUG_TRACE, "%s: called\n", __func__);
  
  // if (vfo != RIG_VFO_CURR && vfo != rig->state.current_vfo)
  //   return kenwood_wrong_vfo(__func__, vfo);
  
  retval = tmd710_pull_fo (rig,vfo,&fo_struct);
  if (retval != RIG_OK){
     return retval;
  }

  freq5=round(freq/5000)*5000;
  freq625=round(freq/6250)*6250;
  if (abs(freq5-freq)<abs(freq625-freq)) {
    step=0;
    freq_sent=freq5;
  }
  else {
    step=1;
    freq_sent=freq625;
  }
  /* Step needs to be at least 10kHz on higher band, otherwise 5 kHz */
  fo_struct.step = freq_sent >= MHz(470) ? 4 : step;
  fo_struct.freq = freq_sent >= MHz(470) ? (round(freq_sent/10000)*10000) : freq_sent;
  
  retval=tmd710_push_fo (rig, vfo, &fo_struct);
  
  return retval;
}

/*
 * th_get_freq
 * Assumes rig!=NULL, freq!=NULL
 */
int
tmd710_get_freq(RIG *rig, vfo_t vfo, freq_t *freq)
{
  tmd710_fo fo_struct;
  int retval;

  rig_debug(RIG_DEBUG_TRACE, "%s: called\n", __func__);

	//if (vfo != RIG_VFO_CURR && vfo != rig->state.current_vfo)
	//	return kenwood_wrong_vfo(__func__, vfo);

  retval = tmd710_pull_fo(rig,vfo,&fo_struct);
  
  *freq = fo_struct.freq;;


  return retval;
}

/*
 * tmd710_set_ctcss_tone
 * Assumes rig!=NULL, freq!=NULL
 */
static int
tmd710_set_ctcss_tone(RIG *rig, vfo_t vfo, tone_t tone)
{
  int retval,k,stepind = -1;
  tmd710_fo fo_struct;
  rig_debug(RIG_DEBUG_TRACE, "%s: called\n", __func__);
  for (k=0;k<42;k++) {
    if (rig->caps->ctcss_list[k]==tone) {
      stepind = k;
      break;
    }
  }
  if (stepind == -1) {
          rig_debug(RIG_DEBUG_ERR, "%s: Unsupported tone value '%d'\n", __func__, tone);
      return -RIG_EINVAL;
  }
  
  retval = tmd710_pull_fo(rig,vfo,&fo_struct);
  if (retval == RIG_OK) {
    fo_struct.tone_freq = stepind;
    
    retval = tmd710_push_fo(rig,vfo,&fo_struct);
  }
  return retval;
}



/*
 * tmd710_get_ctcss_tone
 * Assumes rig!=NULL, freq!=NULL
 */
int
tmd710_get_ctcss_tone(RIG *rig, vfo_t vfo, tone_t *tone)
{
  tmd710_fo fo_struct;
  int retval;
  
  const struct rig_caps *caps;
  caps = rig->caps;
  
  
  rig_debug(RIG_DEBUG_TRACE, "%s: called\n", __func__);

	//if (vfo != RIG_VFO_CURR && vfo != rig->state.current_vfo)
	//	return kenwood_wrong_vfo(__func__, vfo);

  retval = tmd710_pull_fo(rig,vfo,&fo_struct);
  
  if (retval == RIG_OK) {
    
    *tone = caps->ctcss_list[fo_struct.tone_freq];
  }
  
  return retval;
}

/*
 * tmd710_set_ts
 * Assumes rig!=NULL, freq!=NULL
 */
static int
tmd710_set_ctcss_sql(RIG *rig, vfo_t vfo, tone_t tone)
{
  int retval,k,stepind = -1;
  tmd710_fo fo_struct;
  rig_debug(RIG_DEBUG_TRACE, "%s: called\n", __func__);
  for (k=0;k<42;k++) {
    if (rig->caps->ctcss_list[k]==tone) {
      stepind = k;
      break;
    }
  }
  if (stepind == -1) {
          rig_debug(RIG_DEBUG_ERR, "%s: Unsupported tone value '%d'\n", __func__, tone);
      return -RIG_EINVAL;
  }
  
  retval = tmd710_pull_fo(rig,vfo,&fo_struct);
  if (retval == RIG_OK) {
    fo_struct.ct_freq = stepind;
    
    retval = tmd710_push_fo(rig,vfo,&fo_struct);
  }
  return retval;
}



/*
 * tmd710_get_ctcss_sql
 * Assumes rig!=NULL, freq!=NULL
 */
int
tmd710_get_ctcss_sql(RIG *rig, vfo_t vfo, tone_t *tone)
{
  tmd710_fo fo_struct;
  int retval;
  
  const struct rig_caps *caps;
  caps = rig->caps;
  
  
  rig_debug(RIG_DEBUG_TRACE, "%s: called\n", __func__);

	//if (vfo != RIG_VFO_CURR && vfo != rig->state.current_vfo)
	//	return kenwood_wrong_vfo(__func__, vfo);

  retval = tmd710_pull_fo(rig,vfo,&fo_struct);
  
  if (retval == RIG_OK) {
    
    *tone = caps->ctcss_list[fo_struct.ct_freq];
  }
  
  return retval;
}

/*
 * tmd710_set_mode
 * Assumes rig!=NULL, freq!=NULL
 */
static int
tmd710_set_mode(RIG *rig, vfo_t vfo, rmode_t mode, pbwidth_t width)
{
  int retval;
  tmd710_fo fo_struct;
  rig_debug(RIG_DEBUG_TRACE, "%s: called\n", __func__);
  
  retval = tmd710_pull_fo(rig,vfo,&fo_struct);
  if (retval == RIG_OK) {
    if ((mode == RIG_MODE_FM) && (width ==15000)) {
      fo_struct.mode = 0;
    } else if ((mode == RIG_MODE_FM) && (width == 6250)) {
      fo_struct.mode = 1;
    }else if (mode == RIG_MODE_AM) {
       fo_struct.mode = 1;
    } else {
      rig_debug(RIG_DEBUG_ERR, "%s: Illegal value from radio '%ld'\n", __func__, mode);
      return -RIG_EINVAL;
    }
    retval = tmd710_push_fo(rig,vfo,&fo_struct);
  }
  return retval;
}


/*
 * tmd710_get_mode
 * Assumes rig!=NULL, freq!=NULL
 */
int
tmd710_get_mode(RIG *rig, vfo_t vfo, rmode_t *mode, pbwidth_t *width)
{
  tmd710_fo fo_struct;
  int retval;
  
  rig_debug(RIG_DEBUG_TRACE, "%s: called\n", __func__);

  
  retval = tmd710_pull_fo(rig,vfo,&fo_struct);
  
  if (retval == RIG_OK) {
    switch (fo_struct.mode) {
    case 0: 
      *mode = RIG_MODE_FM;
      *width = 15000;
      break;
    case 1:
      *mode = RIG_MODE_FM;
      *width = 6250;
      break;
    case 2:
      *mode = RIG_MODE_AM;
      *width = 4000;
      break;
    default:
          rig_debug(RIG_DEBUG_ERR, "%s: Illegal value from radio '%ld'\n", __func__, mode);
      return -RIG_EINVAL;
      
    }
  }
  
  return retval;
}

/*
 * tmd710_set_ts
 * Assumes rig!=NULL, freq!=NULL
 */
static int
tmd710_set_ts(RIG *rig, vfo_t vfo, shortfreq_t ts)
{
  int retval,k,stepind = -1;
  tmd710_fo fo_struct;
  rig_debug(RIG_DEBUG_TRACE, "%s: called\n", __func__);
  for (k=0;k<TSLSTSIZ;k++) {
    if ((rig->caps->tuning_steps[k].modes==RIG_MODE_NONE) 
	&& (rig->caps->tuning_steps[k].ts==0)) 
      break;
    else if (rig->caps->tuning_steps[k].ts==ts) {
      stepind = k;
      break;
    }
  }
  if (stepind == -1) {
          rig_debug(RIG_DEBUG_ERR, "%s: Unsupported step value '%ld'\n", __func__, ts);
      return -RIG_EINVAL;
  }
  
  retval = tmd710_pull_fo(rig,vfo,&fo_struct);
  if (retval == RIG_OK) {
    fo_struct.step = stepind;
    
    retval = tmd710_push_fo(rig,vfo,&fo_struct);
  }
  return retval;
}

/*
 * tmd710_get_ts
 * Assumes rig!=NULL, freq!=NULL
 */
static int
tmd710_get_ts(RIG *rig, vfo_t vfo, shortfreq_t* ts)
{
  int retval;
  tmd710_fo fo_struct;
  rig_debug(RIG_DEBUG_TRACE, "%s: called\n", __func__);
  
  retval = tmd710_pull_fo(rig,vfo,&fo_struct);
  
  if (retval == RIG_OK) {
    *ts = rig->caps->tuning_steps[fo_struct.step].ts;
  }
  return retval;
}

/*
 * tmd710_get_rptr_shft
 * Assumes rig!=NULL, freq!=NULL
 */
int
tmd710_set_rptr_shift(RIG *rig, vfo_t vfo, rptr_shift_t shift)
{
  int retval;
  tmd710_fo fo_struct;
  rig_debug(RIG_DEBUG_TRACE, "%s: called\n", __func__);
  
  retval = tmd710_pull_fo(rig,vfo,&fo_struct);
  
  if (retval == RIG_OK) {
    switch (shift) {
    case RIG_RPT_SHIFT_NONE:
      fo_struct.shift = 0;
      break;
    case RIG_RPT_SHIFT_PLUS:
      fo_struct.shift = 1;
      break;
    case RIG_RPT_SHIFT_MINUS:
      fo_struct.shift = 2;
      break;
    default:
      rig_debug(RIG_DEBUG_ERR, "%s: Unexpected shift value '%d'\n", __func__, fo_struct.shift);
      return -RIG_EPROTO;
      break;
    }
  }
  return retval;
}


/*
 * tmd710_get_rptr_shft
 * Assumes rig!=NULL, freq!=NULL
 */
int
tmd710_get_rptr_shift(RIG *rig, vfo_t vfo, rptr_shift_t* shift)
{
  int retval;
  tmd710_fo fo_struct;
  rig_debug(RIG_DEBUG_TRACE, "%s: called\n", __func__);
  
  retval = tmd710_pull_fo(rig,vfo,&fo_struct);
  
  if (retval == RIG_OK) {
    switch (fo_struct.shift) {
    case 0:
      *shift = RIG_RPT_SHIFT_NONE;
      break;
    case 1:
      *shift = RIG_RPT_SHIFT_PLUS;
      break;
    case 2:
      *shift = RIG_RPT_SHIFT_MINUS;
      break;
    default:
      rig_debug(RIG_DEBUG_ERR, "%s: Unexpected shift value '%d'\n", __func__, fo_struct.shift);
      return -RIG_EPROTO;
      break;
    }
  }
  return retval;
}

/*
 * th_set_rptr_offs
 * Assumes rig!=NULL
 */
int
tmd710_set_rptr_offs(RIG *rig, vfo_t vfo, shortfreq_t freq)
{
  int retval;
  tmd710_fo fo_struct;
  long freq5,freq625,freq_sent;
  
  rig_debug(RIG_DEBUG_TRACE, "%s: called\n", __func__);
  
  // if (vfo != RIG_VFO_CURR && vfo != rig->state.current_vfo)
  //   return kenwood_wrong_vfo(__func__, vfo);
  
  retval = tmd710_pull_fo (rig,vfo,&fo_struct);
  if (retval != RIG_OK){
     return retval;
  }

  freq5=round(freq/5000)*5000;
  freq625=round(freq/6250)*6250;
  if (abs(freq5-freq)<abs(freq625-freq)) {
    freq_sent=freq5;
  }
  else {
    freq_sent=freq625;
  }
  /* Step needs to be at least 10kHz on higher band, otherwise 5 kHz */
  fo_struct.offset = freq_sent >= MHz(470) ? (round(freq_sent/10000)*10000) : freq_sent;
  
  retval=tmd710_push_fo (rig, vfo, &fo_struct);
  
  return retval;
}



/*
 * tmd710_get_rptr_offs
 * Assumes rig!=NULL, freq!=NULL
 */
int
tmd710_get_rptr_offs(RIG *rig, vfo_t vfo, shortfreq_t *rptr_offs)
{
  tmd710_fo fo_struct;
  int retval;
  
  rig_debug(RIG_DEBUG_TRACE, "%s: called\n", __func__);
  
  //if (vfo != RIG_VFO_CURR && vfo != rig->state.current_vfo)
	//	return kenwood_wrong_vfo(__func__, vfo);

  retval = tmd710_pull_fo(rig,vfo,&fo_struct);
  
  if (retval == RIG_OK)
    *rptr_offs = fo_struct.offset;


  return retval;
}



int
tmd710_get_vfo_char(RIG *rig, vfo_t *vfo, char *vfoch)
{
	char cmdbuf[10], buf[10], vfoc;
	size_t buf_size=10;
	int retval;

	rig_debug(RIG_DEBUG_TRACE, "%s: called\n", __func__);

	/* Get VFO band */
	
	retval = kenwood_transaction(rig, "BC", 2, buf, &buf_size);
	if (retval != RIG_OK)
		return retval;
	switch (buf_size) {
	case 7: /*intended for D700 BC 0,0*/
	  if ((buf[0]=='B') &&(buf[1]=='C') && (buf[2]==' ') && (buf[4]=',')){
	    vfoc = buf[3];
	  } else {
	    rig_debug(RIG_DEBUG_ERR, "%s: Unexpected answer format '%s'\n", __func__, buf);
	    return -RIG_EPROTO;
	  }
	  break;
	default:
	  rig_debug(RIG_DEBUG_ERR, "%s: Unexpected answer length '%c'\n", __func__, buf_size);
	  return -RIG_EPROTO;
	  break;
	}


	switch (vfoc) {

	   	case '0': *vfo = RIG_VFO_A; break;
		case '1': *vfo = RIG_VFO_B; break;
		default:
		rig_debug(RIG_DEBUG_ERR, "%s: Unexpected VFO value '%c'\n", __func__, buf[3]);
		return -RIG_EVFO;

	}
	rig->state.current_vfo = *vfo;

	/* Get mode of the VFO band */
	
	snprintf(cmdbuf, 9, "VM %c", vfoc);

	retval = kenwood_safe_transaction(rig, cmdbuf, buf, 10, 7);
	if (retval != RIG_OK)
		return retval;

	*vfoch = buf[5];

    return RIG_OK;
}



/*
 * tmd710_get_vfo
 * Assumes rig!=NULL
 */
int
tmd710_get_vfo(RIG *rig, vfo_t *vfo)
{
	char vfoch;
	int retval;

	rig_debug(RIG_DEBUG_TRACE, "%s: called\n", __func__);

	retval = tmd710_get_vfo_char(rig, vfo, &vfoch);
	if (retval != RIG_OK)
		return retval;

	switch (vfoch) {
	case '0' :
	case '1' :
		break;
	case '2' :
		*vfo = RIG_VFO_MEM;
		break;
		default:
		rig_debug(RIG_DEBUG_ERR, "%s: Unexpected VFO value '%c'\n", __func__, vfoch);
		return -RIG_EVFO;
	}

	return RIG_OK;
}

/*
 * tm_set_vfo_bc2
 * Apply to split-capable models (with BC command taking 2 args): TM-V7, TM-D700
 *
 * Assumes rig!=NULL
 */
int tmd710_set_vfo (RIG *rig, vfo_t vfo)
{
    struct kenwood_priv_data *priv = rig->state.priv;
    char vfobuf[16], ackbuf[16];
    int vfonum, txvfonum, vfomode=0;
    int retval;
    size_t ack_len;


    rig_debug(RIG_DEBUG_TRACE, "%s: called %s\n", __func__, rig_strvfo(vfo));

	switch (vfo) {
        case RIG_VFO_A:
        case RIG_VFO_VFO:
            vfonum = 0;
            /* put back split mode when toggling */
            txvfonum = (priv->split == RIG_SPLIT_ON &&
                rig->state.tx_vfo == RIG_VFO_B) ? 1 : vfonum;
            break;
        case RIG_VFO_B:
            vfonum = 1;
            /* put back split mode when toggling */
            txvfonum = (priv->split == RIG_SPLIT_ON &&
                rig->state.tx_vfo == RIG_VFO_A) ? 0 : vfonum;
            break;
        case RIG_VFO_MEM:
            /* get current band */
	  snprintf(vfobuf, 10, "BC");
            ack_len=16;
            retval = kenwood_transaction(rig, vfobuf, strlen(vfobuf), ackbuf, &ack_len);
            if (retval != RIG_OK)
                return retval;
            txvfonum = vfonum = ackbuf[3]-'0';
            vfomode = 2;
            break;

        default:
            rig_debug(RIG_DEBUG_ERR, "%s: Unsupported VFO %d\n", __func__, vfo);
            return -RIG_EVFO;
	}

	snprintf(vfobuf,9, "VM %d,%d", vfonum, vfomode);
	retval = kenwood_cmd(rig, vfobuf);
	if (retval != RIG_OK)
        return retval;

    if (vfo == RIG_VFO_MEM)
        return RIG_OK;

    snprintf(vfobuf, 15, "BC %d,%d", vfonum, txvfonum);
    retval = kenwood_cmd(rig, vfobuf);
	if (retval != RIG_OK)
        return retval;

    return RIG_OK;
}

/* end of file */
