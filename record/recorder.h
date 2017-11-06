#ifndef _RECORDER_H_
#define _RECORDER_H_

typedef void (*f_recordListener)(char* data, int size);

void recorder_Start();
void recorder_Stop();
void set_Recorder_Listener(f_recordListener func);

#endif // _RECORDER_H_
