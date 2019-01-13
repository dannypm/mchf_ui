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

#include "mchf_pro_board.h"

#include "GUI.h"

//extern GUI_CONST_STORAGE GUI_BITMAP bmicon_compref;

static GUI_CONST_STORAGE unsigned short _acicon_compref[] = {
  0x0EFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0EFF, 0x0BED, 0x0BED, 0x0CDD, 0x0BBC, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0EFF, 0x0BED, 0x0BED, 0x0CEE,
        0x0BCC, 0x0EFE, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0DFE, 0x0CED, 0x0AED, 0x0CDD, 0x0BCC, 0x0EFF, 0x0EFF, 0x0FFF, 0x0FFE, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF,
  0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0DEE, 0x0BDD, 0x0BDD, 0x0BCC, 0x0889, 0x0EEE, 0x0EFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0DEE, 0x0BDD, 0x0BDD, 0x0ACC,
        0x0888, 0x0EEE, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0EFF, 0x0DEE, 0x0BED, 0x0ADD, 0x0BCC, 0x0888, 0x0EEE, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF,
  0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FEE, 0x0DDD, 0x0CDD, 0x0CDD, 0x0AAA, 0x0655, 0x0EDD, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FEF, 0x0EDE, 0x0CDD, 0x0CDD, 0x0AAA,
        0x0655, 0x0DDD, 0x0EEE, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FEF, 0x0EDE, 0x0CDD, 0x0CCC, 0x0AAA, 0x0556, 0x0DDE, 0x0FEF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF,
  0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0EEE, 0x0CDD, 0x0BDD, 0x0BCC, 0x0AAA, 0x0666, 0x0CCD, 0x0DEE, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0EFF, 0x0EFF, 0x0EEF, 0x0DDD, 0x0BCC, 0x0BCC, 0x09AA,
        0x0666, 0x0DDE, 0x0EEF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0EFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0EEE, 0x0DDD, 0x0CCD, 0x0BCD, 0x0A9A, 0x0667, 0x0DCD, 0x0EEE, 0x0FEF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF,
  0x0599, 0x0599, 0x05AA, 0x05A9, 0x05A9, 0x0599, 0x0599, 0x0599, 0x06A9, 0x06A9, 0x0599, 0x06A9, 0x07A9, 0x0698, 0x06A9, 0x05A9, 0x0599, 0x0599, 0x0599, 0x0599, 0x0599, 0x0599, 0x0599, 0x0599, 0x06A9, 0x0699, 0x0699, 0x0699, 0x0599, 0x06A9, 0x06A9,
        0x0688, 0x0599, 0x0599, 0x05A9, 0x05A9, 0x0599, 0x0599, 0x0599, 0x0599, 0x0599, 0x0699, 0x0699, 0x06AA, 0x0699, 0x0699, 0x0699, 0x0699, 0x06AA, 0x0498, 0x06AA, 0x06AA, 0x06A9, 0x06A9, 0x0699, 0x0699, 0x05A9, 0x0599, 0x0599, 0x0599,
  0x0477, 0x0588, 0x0588, 0x0588, 0x0588, 0x0477, 0x0588, 0x0588, 0x0487, 0x0488, 0x0487, 0x0488, 0x0488, 0x0598, 0x0487, 0x0488, 0x0588, 0x0588, 0x0588, 0x0588, 0x0588, 0x0588, 0x0588, 0x0588, 0x0488, 0x0488, 0x0488, 0x0588, 0x0488, 0x0487, 0x0588,
        0x0588, 0x0588, 0x0588, 0x0588, 0x0588, 0x0588, 0x0588, 0x0588, 0x0588, 0x0488, 0x0488, 0x0488, 0x0488, 0x0488, 0x0488, 0x0488, 0x0588, 0x0588, 0x0688, 0x0477, 0x0588, 0x0577, 0x0588, 0x0588, 0x0588, 0x0588, 0x0588, 0x0587, 0x0477,
  0x0133, 0x0233, 0x0233, 0x0244, 0x0344, 0x0133, 0x0244, 0x0233, 0x0344, 0x0233, 0x0344, 0x0234, 0x0233, 0x0233, 0x0233, 0x0344, 0x0233, 0x0233, 0x0233, 0x0233, 0x0233, 0x0233, 0x0233, 0x0233, 0x0233, 0x0233, 0x0334, 0x0344, 0x0344, 0x0233, 0x0233,
        0x0233, 0x0233, 0x0233, 0x0233, 0x0233, 0x0233, 0x0233, 0x0244, 0x0244, 0x0344, 0x0334, 0x0234, 0x0234, 0x0233, 0x0233, 0x0233, 0x0334, 0x0233, 0x0234, 0x0233, 0x0233, 0x0244, 0x0233, 0x0233, 0x0233, 0x0233, 0x0233, 0x0233, 0x0233,
  0x0000, 0x0111, 0x0000, 0x0000, 0x0000, 0x0111, 0x0878, 0x0CCC, 0x0444, 0x0001, 0x0000, 0x0000, 0x0001, 0x0000, 0x0001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0101, 0x0000, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0888, 0x0EEE, 0x0EEE, 0x0999, 0x0222, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0888, 0x0AAA, 0x0333, 0x0BBB, 0x0EEE, 0x0888, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0111, 0x0000, 0x0555, 0x0666, 0x0666, 0x0777, 0x0BBB, 0x0AAA, 0x0000, 0x0000, 0x0666, 0x0DDD, 0x0DDD, 0x0555, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0999, 0x0CCC, 0x0CCC, 0x0CCC, 0x0DDD, 0x0AAA, 0x0000, 0x0000, 0x0000, 0x0111, 0x0888, 0x0EEE, 0x0BBB, 0x0333, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0999, 0x0999, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0333, 0x0999, 0x0EEE, 0x0999, 0x0222, 0x0000, 0x0000, 0x0000, 0x0000, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0888, 0x0AAA, 0x0000, 0x0000, 0x0000, 0x0111, 0x0000, 0x0000, 0x0111, 0x0444, 0x0CCC, 0x0EEE, 0x0777, 0x0000, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0888, 0x0AAA, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0555, 0x0DDD, 0x0CCC, 0x0444, 0x0000, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0999, 0x0AAA, 0x0000, 0x0000, 0x0000, 0x0000, 0x0666, 0x0BBB, 0x0CCC, 0x0BBB, 0x0CCC, 0x0BBB, 0x0BBB, 0x0AAA, 0x0EEE, 0x0BBB, 0x0222, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0888, 0x0AAA, 0x0000, 0x0000, 0x0000, 0x0111, 0x0EEE, 0x0888, 0x0777, 0x0FFF, 0x0999, 0x0666, 0x0555, 0x0000, 0x0222, 0x0BBB, 0x0EEE, 0x0999, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0888, 0x0AAA, 0x0000, 0x0000, 0x0000, 0x0444, 0x0DDD, 0x0111, 0x0222, 0x0DDD, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0444, 0x0CCC, 0x0DDD, 0x0777, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0888, 0x0BBB, 0x0666, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0888, 0x0AAA, 0x0000, 0x0000, 0x0000, 0x0888, 0x0CCC, 0x0000, 0x0777, 0x0CCC, 0x0000, 0x0000, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0111, 0x0EEE, 0x0EEE, 0x0666, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0BBB, 0x0FFF, 0x0777, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0888, 0x0AAA, 0x0000, 0x0000, 0x0000, 0x0CCC, 0x0666, 0x0000, 0x0BBB, 0x0888, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0555, 0x0DDD, 0x0CCC, 0x0666, 0x0000, 0x0000, 0x0000, 0x0000, 0x0111,
        0x0BBB, 0x0FFF, 0x0777, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0888, 0x0AAA, 0x0333, 0x0444, 0x0666, 0x0FFF, 0x0666, 0x0555, 0x0EEE, 0x0333, 0x0000, 0x0000, 0x0111, 0x0000, 0x0333, 0x0BBB, 0x0EEE, 0x0888, 0x0111, 0x0000, 0x0000, 0x0000, 0x0666, 0x0BBB, 0x0BBB,
        0x0EEE, 0x0FFF, 0x0DDD, 0x0BBB, 0x0BBB, 0x0666, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0888, 0x0AAA, 0x0AAA, 0x0DDD, 0x0DDD, 0x0DDD, 0x0DDD, 0x0CCC, 0x0999, 0x0111, 0x0000, 0x0000, 0x0111, 0x0999, 0x0FFF, 0x0999, 0x0222, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0777, 0x0FFF, 0x0FFF,
        0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0EEE, 0x0777, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0888, 0x0AAA, 0x0111, 0x0000, 0x0111, 0x0000, 0x0111, 0x0111, 0x0000, 0x0000, 0x0000, 0x0777, 0x0EEE, 0x0CCC, 0x0444, 0x0000, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0BBB, 0x0FFF, 0x0777, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0888, 0x0AAA, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0444, 0x0DDD, 0x0DDD, 0x0666, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0BBB, 0x0FFF, 0x0777, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0888, 0x0AAA, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0333, 0x0AAA, 0x0EEE, 0x0888, 0x0222, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0111,
        0x0AAA, 0x0FFF, 0x0777, 0x0000, 0x0000, 0x0111, 0x0000, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0000, 0x0000,
  0x0000, 0x0111, 0x0888, 0x0CCC, 0x0DDD, 0x0BBB, 0x0EEE, 0x0999, 0x0000, 0x0000, 0x0000, 0x0111, 0x0888, 0x0EEE, 0x0BBB, 0x0333, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0EEE, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0FFF, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0555, 0x0777, 0x0666, 0x0666, 0x0BBB, 0x0999, 0x0000, 0x0000, 0x0555, 0x0DDD, 0x0DDD, 0x0555, 0x0000, 0x0000, 0x0000, 0x0000, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0FFF, 0x0000, 0x0666, 0x0BBB, 0x0BBB, 0x0BBB, 0x0333, 0x0000, 0x0BBB, 0x0BBB, 0x0AAA, 0x0CCC, 0x0666, 0x0888, 0x0BBB, 0x0BBB, 0x0BBB, 0x0888, 0x0000, 0x0FFF, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0111, 0x0000, 0x0000, 0x0000, 0x0888, 0x0AAA, 0x0333, 0x0BBB, 0x0EEE, 0x0777, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0FFF, 0x0000, 0x0777, 0x0FFF, 0x0000, 0x0888, 0x0EEE, 0x0000, 0x0FFF, 0x0777, 0x0000, 0x0000, 0x0000, 0x0BBB, 0x0AAA, 0x0000, 0x0000, 0x0000, 0x0000, 0x0EEE, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0888, 0x0EEE, 0x0EEE, 0x0888, 0x0222, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0FFF, 0x0000, 0x0777, 0x0EEE, 0x0000, 0x0777, 0x0FFF, 0x0111, 0x0EEE, 0x0777, 0x0000, 0x0111, 0x0000, 0x0CCC, 0x0BBB, 0x0000, 0x0000, 0x0000, 0x0000, 0x0FFF, 0x0000, 0x0000,
  0x0000, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0777, 0x0CCC, 0x0333, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0FFF, 0x0000, 0x0777, 0x0FFF, 0x0BBB, 0x0CCC, 0x0888, 0x0000, 0x0FFF, 0x0DDD, 0x0BBB, 0x0888, 0x0000, 0x0AAA, 0x0EEE, 0x0BBB, 0x0BBB, 0x0000, 0x0000, 0x0FFF, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0FFF, 0x0000, 0x0777, 0x0FFF, 0x0000, 0x0777, 0x0EEE, 0x0000, 0x0FFF, 0x0777, 0x0000, 0x0000, 0x0000, 0x0BBB, 0x0BBB, 0x0000, 0x0000, 0x0000, 0x0000, 0x0FFF, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0FFF, 0x0000, 0x0777, 0x0FFF, 0x0000, 0x0777, 0x0FFF, 0x0000, 0x0FFF, 0x0777, 0x0000, 0x0000, 0x0000, 0x0BBB, 0x0BBB, 0x0000, 0x0000, 0x0000, 0x0000, 0x0FFF, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0FFF, 0x0000, 0x0777, 0x0FFF, 0x0000, 0x0888, 0x0EEE, 0x0111, 0x0FFF, 0x0DDD, 0x0BBB, 0x0BBB, 0x0777, 0x0BBB, 0x0AAA, 0x0000, 0x0000, 0x0000, 0x0000, 0x0FFF, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0111, 0x0000, 0x0000, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0FFF, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0111, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0EEE, 0x0EEE, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0111, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0AAA, 0x0999, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0777, 0x0777, 0x0777, 0x0777, 0x0555, 0x0344, 0x0777, 0x0777, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0777, 0x0777, 0x0677, 0x0555,
        0x0344, 0x0777, 0x0888, 0x0888, 0x0888, 0x0999, 0x0888, 0x0999, 0x0888, 0x0888, 0x0999, 0x0888, 0x0888, 0x0888, 0x0677, 0x0676, 0x0777, 0x0666, 0x0333, 0x0777, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0888, 0x0999, 0x0AAA,
  0x0CCC, 0x0BBB, 0x0AAA, 0x0AAA, 0x0AAA, 0x0AAA, 0x0AAA, 0x0AAA, 0x09AA, 0x0899, 0x0888, 0x0888, 0x0666, 0x0444, 0x0888, 0x0AAA, 0x0AAA, 0x0AAA, 0x0AAA, 0x0AAA, 0x0AAA, 0x0AAA, 0x0AAA, 0x0AAA, 0x0AAA, 0x0AAA, 0x0999, 0x0888, 0x0888, 0x0888, 0x0666,
        0x0444, 0x0888, 0x0999, 0x0AAA, 0x0AAA, 0x0AAA, 0x0AAA, 0x0AAA, 0x0AAA, 0x0AAA, 0x0AAA, 0x0AAA, 0x0AAA, 0x0999, 0x0999, 0x0888, 0x0898, 0x0777, 0x0444, 0x0888, 0x0999, 0x0AAA, 0x0AAA, 0x0AAA, 0x0AAA, 0x0AAA, 0x0AAA, 0x0BBB, 0x0CCC,
  0x0DDD, 0x0DDD, 0x0DDD, 0x0CCC, 0x0CCC, 0x0DDD, 0x0CCC, 0x0CCC, 0x0CCC, 0x0AAA, 0x0AAA, 0x0888, 0x0666, 0x0333, 0x0BBB, 0x0CCC, 0x0CCC, 0x0CCC, 0x0CCC, 0x0CCC, 0x0CCC, 0x0CCC, 0x0CCC, 0x0CCC, 0x0DDD, 0x0CCC, 0x0BBB, 0x0BBB, 0x0AAA, 0x0999, 0x0666,
        0x0333, 0x0BBB, 0x0CCC, 0x0DDD, 0x0CCC, 0x0DDD, 0x0DDD, 0x0DDD, 0x0DDD, 0x0CCC, 0x0CCC, 0x0DDD, 0x0CCC, 0x0BBB, 0x0BBB, 0x0AAA, 0x0999, 0x0666, 0x0333, 0x0AAA, 0x0CCC, 0x0CCC, 0x0DDD, 0x0CCC, 0x0CCC, 0x0CCC, 0x0CCC, 0x0DDD, 0x0DDD,
  0x0FFF, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0CCC, 0x0777, 0x0444, 0x0333, 0x0333, 0x0CCC, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0BBB, 0x0777, 0x0444, 0x0333,
        0x0333, 0x0CCC, 0x0DDD, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0FFF, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0CCC, 0x0777, 0x0333, 0x0222, 0x0333, 0x0BBB, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0FFF,
  0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0EEE, 0x0CCC, 0x0BBB, 0x0999, 0x0999, 0x0BBB, 0x0DDD, 0x0EEE, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0EEE, 0x0DDD, 0x0BBB, 0x0999, 0x0999,
        0x0AAA, 0x0DDD, 0x0EEE, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0EEE, 0x0DDD, 0x0BBB, 0x0999, 0x0999, 0x0AAA, 0x0DDD, 0x0EEE, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF,
  0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0EEE, 0x0EEE, 0x0BBB, 0x0BBB, 0x0BBB, 0x0CCC, 0x0DDD, 0x0EEE, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0DDD, 0x0CCC, 0x0BBB, 0x0BBB,
        0x0CCC, 0x0DDD, 0x0EEE, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0DDD, 0x0CCC, 0x0AAA, 0x0AAA, 0x0CCC, 0x0DDD, 0x0EEE, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF,
  0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0DDD, 0x0DDD, 0x0DDD, 0x0DDD, 0x0EEE, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0EEE, 0x0DDD, 0x0DDD, 0x0DDD,
        0x0DDD, 0x0EEE, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0EEE, 0x0DDD, 0x0DDD, 0x0DDD, 0x0EEE, 0x0EEE, 0x0EEE, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF,
  0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0EEE, 0x0EEE, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0EEE, 0x0EEE,
        0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0EEE, 0x0EEE, 0x0EEE, 0x0EEE, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF
};

GUI_CONST_STORAGE GUI_BITMAP bmicon_compref = {
  60, // xSize
  46, // ySize
  120, // BytesPerLine
  16, // BitsPerPixel
  (unsigned char *)_acicon_compref,  // Pointer to picture data
  NULL,  // Pointer to palette
  GUI_DRAW_BMP444_12
};
