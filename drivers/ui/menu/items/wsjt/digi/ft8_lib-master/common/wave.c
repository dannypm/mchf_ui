// ******************************************
//
// ft8_lib by Karlis Goba, YL3JG
// https://github.com/kgoba/ft8_lib
//
// ******************************************

#include "mchf_pro_board.h"

#include "wave.h"
//#include "ff.h"
#include "C:\Projects\mcHFx\firmware\mchf_ui\middleware\FatFs\src\ff.h"

//#include <cstdlib>
//#include <cstring>
//#include <cstdio>
//#include <stdint.h>

// Save signal in floating point format (-1 .. +1) as a WAVE file using 16-bit signed integers.
void save_wav(const float *signal, int num_samples, int sample_rate, const char *path)
{
    char subChunk1ID[4] = {'f', 'm', 't', ' '};
    uint32_t subChunk1Size = 16;    // 16 for PCM
    uint16_t audioFormat = 1;       // PCM = 1
    uint16_t numChannels = 1;
    uint16_t bitsPerSample = 16;
    uint32_t sampleRate = sample_rate;
    uint16_t blockAlign = numChannels * bitsPerSample / 8;
    uint32_t byteRate = sampleRate * blockAlign;

    FIL f;
    uint written;

    char subChunk2ID[4] = {'d', 'a', 't', 'a'};
    uint32_t subChunk2Size = num_samples * blockAlign;

    char chunkID[4] = {'R', 'I', 'F', 'F'};
    uint32_t chunkSize = 4 + (8 + subChunk1Size) + (8 + subChunk2Size);
    char format[4] = {'W', 'A', 'V', 'E'};

    int16_t *raw_data = (int16_t *)pvPortMalloc(num_samples * blockAlign);

    for (int i = 0; i < num_samples; i++)
    {
        float x = signal[i];
        if (x > 1.0) x = 1.0;
        else if (x < -1.0) x = -1.0;
        raw_data[i] = (int)(0.5 + (x * 32767.0));
    }

    //FILE *f = fopen(path, "wb");
    if(f_open(&f,path,FA_WRITE|FA_CREATE_NEW))
    {
    	vPortFree(raw_data);
    	return;
    }

    // ToDo: test file creation and enable write...
    //		 also crashes the OS


    // NOTE: works only on little-endian architecture
    //fwrite(chunkID, sizeof(chunkID), 1, f);
    f_write(&f,chunkID, sizeof(chunkID), &written);

    //fwrite(&chunkSize, sizeof(chunkSize), 1, f);
    f_write(&f,&chunkSize, sizeof(chunkSize), &written);

    //fwrite(format, sizeof(format), 1, f);
    f_write(&f,format, sizeof(format), &written);

    //fwrite(subChunk1ID, sizeof(subChunk1ID), 1, f);
    f_write(&f,subChunk1ID, sizeof(subChunk1ID), &written);

    //fwrite(&subChunk1Size, sizeof(subChunk1Size), 1, f);
    f_write(&f,&subChunk1Size, sizeof(subChunk1Size), &written);

    //fwrite(&audioFormat, sizeof(audioFormat), 1, f);
    f_write(&f,&audioFormat, sizeof(audioFormat), &written);

    //fwrite(&numChannels, sizeof(numChannels), 1, f);
    f_write(&f,&numChannels, sizeof(numChannels), &written);

    //fwrite(&sampleRate, sizeof(sampleRate), 1, f);
    f_write(&f,&sampleRate, sizeof(sampleRate), &written);

    //fwrite(&byteRate, sizeof(byteRate), 1, f);
    f_write(&f,&byteRate, sizeof(byteRate), &written);

    //fwrite(&blockAlign, sizeof(blockAlign), 1, f);
    f_write(&f,&blockAlign, sizeof(blockAlign), &written);

    //fwrite(&bitsPerSample, sizeof(bitsPerSample), 1, f);
    f_write(&f,&bitsPerSample, sizeof(bitsPerSample), &written);

    //fwrite(subChunk2ID, sizeof(subChunk2ID), 1, f);
    f_write(&f,subChunk2ID, sizeof(subChunk2ID), &written);

    //fwrite(&subChunk2Size, sizeof(subChunk2Size), 1, f);
    f_write(&f,&subChunk2Size, sizeof(subChunk2Size), &written);

    //fwrite(raw_data, blockAlign, num_samples, f);
    f_write(&f,raw_data, blockAlign, &written);		// (blockAlign * num_samples) ??

    f_close(&f);

    vPortFree(raw_data);
}

// Load signal in floating point format (-1 .. +1) as a WAVE file using 16-bit signed integers.
int load_wav(float *signal, int *num_samples, int *sample_rate, const char *path)
{
#if 0
    char subChunk1ID[4];    // = {'f', 'm', 't', ' '};
    uint32_t subChunk1Size; // = 16;    // 16 for PCM
    uint16_t audioFormat;   // = 1;     // PCM = 1
    uint16_t numChannels;   // = 1;
    uint16_t bitsPerSample; // = 16;
    uint32_t sampleRate;
    uint16_t blockAlign;    // = numChannels * bitsPerSample / 8;
    uint32_t byteRate;      // = sampleRate * blockAlign;

    char subChunk2ID[4];    // = {'d', 'a', 't', 'a'};
    uint32_t subChunk2Size; // = num_samples * blockAlign;

    char chunkID[4];        // = {'R', 'I', 'F', 'F'};
    uint32_t chunkSize;     // = 4 + (8 + subChunk1Size) + (8 + subChunk2Size);
    char format[4];         // = {'W', 'A', 'V', 'E'};

    FILE *f = fopen(path, "rb");

    // NOTE: works only on little-endian architecture
    fread((void *)chunkID, sizeof(chunkID), 1, f);
    fread((void *)&chunkSize, sizeof(chunkSize), 1, f);
    fread((void *)format, sizeof(format), 1, f);

    fread((void *)subChunk1ID, sizeof(subChunk1ID), 1, f);
    fread((void *)&subChunk1Size, sizeof(subChunk1Size), 1, f);
    if (subChunk1Size != 16) return -1;

    fread((void *)&audioFormat, sizeof(audioFormat), 1, f);
    fread((void *)&numChannels, sizeof(numChannels), 1, f);
    fread((void *)&sampleRate, sizeof(sampleRate), 1, f);
    fread((void *)&byteRate, sizeof(byteRate), 1, f);
    fread((void *)&blockAlign, sizeof(blockAlign), 1, f);
    fread((void *)&bitsPerSample, sizeof(bitsPerSample), 1, f);

    if (audioFormat != 1 || numChannels != 1 || bitsPerSample != 16) return -1;

    fread((void *)subChunk2ID, sizeof(subChunk2ID), 1, f);
    fread((void *)&subChunk2Size, sizeof(subChunk2Size), 1, f);

    if (subChunk2Size / blockAlign > num_samples) return -2;
    
    num_samples = subChunk2Size / blockAlign;
    sample_rate = sampleRate;

    int16_t *raw_data = (int16_t *)malloc(num_samples * blockAlign);

    fread((void *)raw_data, blockAlign, num_samples, f);
    for (int i = 0; i < num_samples; i++) {
        signal[i] = raw_data[i] / 32768.0f;
    }
    
    free(raw_data);

    fclose(f);
#endif
    return 0;
}
