#ifndef __GEN_FT8_H
#define __GEN_FT8_H

#define sample_rate  		12000
//#define symbol_rate 		6.25
#define num_samples  		(0.5 + FT8_NN / 6.25 * 12000)
#define num_silence 		((15 * 12000 - num_samples) / 2)
#define elements_c			(num_silence + num_samples + num_silence)

void encode_ft8_message(char *msg);

#endif
