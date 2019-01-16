/************************************************************************************
**                                                                                 **
**                             mcHF Pro QRP Transceiver                            **
**                         Krassi Atanassov - M0NKA 2012-2019                      **
**                            mail: djchrismarc@gmail.com                          **
**                                 twitter: @M0NKA_                                **
**---------------------------------------------------------------------------------**
**                                                                                 **
**  File name:                                                                     **
**  Description:                                                                   **
**  Last Modified:                                                                 **
**  Licence:                                                                       **
**          The mcHF project is released for radio amateurs experimentation,       **
**          non-commercial use only. All source files under GPL-3.0, unless        **
**          third party drivers specifies otherwise. Thank you!                    **
************************************************************************************/

// Who can call here ? Use critical section ?
//

#include "mchf_pro_board.h"

#include "hw_dsp_eep.h"

// Public radio state
extern struct	TRANSCEIVER_STATE_UI	tsu;

void hw_dsp_eep_update_audio_gain(int value)
{
	struct DspTransceiverState 	temp_ts;	// just temp copy in stack
	ulong 						abs_addr;

	// There is old request waiting processing, do not overwrite!
	if(tsu.update_dsp_eep_req)
		return;

	// Set value locally
	temp_ts.audio_gain = value;

	// Get absolute offset in structure
	abs_addr = (ulong)&(temp_ts.audio_gain) - (ulong)&(temp_ts.samp_rate);

	// Fill data in public request
	tsu.update_dsp_eep_offset 	= (ushort)abs_addr;
	tsu.update_dsp_eep_size 	= sizeof(temp_ts.audio_gain);
	tsu.update_dsp_eep_value 	= temp_ts.audio_gain;

	//printf("Update Audio Gain in DSP:\r\n");
	//printf("addr in struct:%d\r\n",	tsu.update_dsp_eep_offset );
	//printf("var size:%d\r\n",		tsu.update_dsp_eep_size);
	//printf("value:%d\r\n",			tsu.update_dsp_eep_value);

	// Post request
	tsu.update_dsp_eep_req = 1;
}

// Needs AGC off + call of re-calc routine in ui driver(DSP code)
void hw_dsp_eep_update_rf_gain(int value)
{
	struct DspTransceiverState 	temp_ts;	// just temp copy in stack
	ulong 						abs_addr;

	// There is old request waiting processing, do not overwrite!
	if(tsu.update_dsp_eep_req)
		return;

	// Set value locally
	temp_ts.rf_gain = value;

	// Get absolute offset in structure
	abs_addr = (ulong)&(temp_ts.rf_gain) - (ulong)&(temp_ts.samp_rate);

	// Fill data in public request
	tsu.update_dsp_eep_offset 	= (ushort)abs_addr;
	tsu.update_dsp_eep_size 	= sizeof(temp_ts.rf_gain);
	tsu.update_dsp_eep_value 	= temp_ts.rf_gain;

	//printf("Update RF Gain in DSP:\r\n");
	//printf("addr in struct:%d\r\n",	tsu.update_dsp_eep_offset );
	//printf("var size:%d\r\n",		tsu.update_dsp_eep_size);
	//printf("value:%d\r\n",			tsu.update_dsp_eep_value);

	// Post request
	tsu.update_dsp_eep_req = 1;
}

// not tested!
void hw_dsp_eep_update_rit(short value)
{
	struct DspTransceiverState 	temp_ts;	// just temp copy in stack
	ulong 						abs_addr;

	// There is old request waiting processing, do not overwrite!
	if(tsu.update_dsp_eep_req)
		return;

	// Set value locally
	temp_ts.rit_value = value;

	// Get absolute offset in structure
	abs_addr = (ulong)&(temp_ts.rit_value) - (ulong)&(temp_ts.samp_rate);

	// Fill data in public request
	tsu.update_dsp_eep_offset 	= (ushort)abs_addr;
	tsu.update_dsp_eep_size 	= sizeof(temp_ts.rit_value);
	tsu.update_dsp_eep_value 	= temp_ts.rit_value;

	// Post request
	tsu.update_dsp_eep_req = 1;
}

