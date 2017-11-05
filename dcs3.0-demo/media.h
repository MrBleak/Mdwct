#ifndef _MEDIA_H_
#define _MEDIA_H_

#define URL_LEN (2048)

void media_init();
void media_uninit();
void media_Play_url(char* m_url);
void media_Volume_Change(double m_volume);
void media_Set_Volume(double m_volume);
double media_Get_Volume();
void media_Mute(bool mute);
void media_Play_Pause();
void media_Play_Seek();
void media_Stop();

#endif // _MEDIA_H_
