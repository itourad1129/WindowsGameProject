//
// DirectAudio ŠÖŒW
//

#ifndef	__SND_
#define	__SND_


#define	SND_MAX				256				// ƒTƒEƒ“ƒh”
#define	SND_NONE_			-1				// –³‰¹‚Ì ID
#define	SND_DISABLE_TIME	10				// “¯”Ô†‚ÌÄ¶‚ğó‚¯•t‚¯‚È‚¢ŠÔ


extern	bool	snd_init(void);
extern	bool	snd_load(int iNum, char *strOriFile, bool bLoop);
extern	bool	snd_load(int iNum, char *strOriFile, bool bLoop, bool bLoadAtOnce);
extern	void	snd_free(int iNum);
extern	void	snd_stop(int iNum);
extern	void	snd_main(void);

extern	void	snd_end(void);
extern	void	snd_play(int iNum);
extern	void	snd_play_delay(int iNum, int iDelay);
extern	void	snd_pause(int iNum, bool bSw);
extern	void	snd_pause_all(bool bSw);
extern	void	snd_system_pause_all(bool bSw);
extern	void	snd_volume(int iNum, float fVolume, int iTime);
extern	void	snd_pitch(int iNum, float fVol, int iTime);
extern	void	snd_free(int iNum);


//--- ‚l‚o‚R—p ------------------------------------------------------------------------------------
enum
{
	MP3_MESSAGE0 = (WM_USER	+ 290),
};
extern	void	snd_mp3_proc(int a);

#endif	//	__SND_




