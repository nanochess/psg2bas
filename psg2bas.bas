        '
        ' Play sounds converted with psg2bas
        ' by Oscar Toledo G.
        ' http://nanochess.org/
        '
        ' Creation date: Feb/24/2018.
        '

        '
        ' So your game doesn't need to know anything about the sound player
        '
        ON FRAME GOSUB play_sound

        '
        ' Just to allow to TV synchro start successfully
        '
	FOR c = 0 TO 60: WAIT: NEXT c

        '
        ' This is the way to invoke a sound sample (2 variables init)
        '
	sound_counter = 0
        #pointer = VARPTR sound_sample1(0) - VARPTR sound_data(0)

'       sound_counter = 0
'       #pointer = VARPTR sound_sample2(0) - VARPTR sound_data(0)

        WHILE 1
		WAIT
	WEND

        '
        ' The main sound playing routine
        '
play_sound:	PROCEDURE

	IF sound_counter = 0 THEN
		#flags = sound_data(#pointer)
		IF #flags = 0 THEN SOUND 0,,0: SOUND 1,,0: SOUND 2,,0: RETURN

		#pointer = #pointer + 1
		IF #flags AND 1 THEN
			SOUND 0,sound_data(#pointer) / 16, sound_data(#pointer) AND $0F
			#pointer = #pointer + 1
		END IF
		IF #flags AND 2 THEN
			SOUND 1,sound_data(#pointer) / 16, sound_data(#pointer) AND $0F
			#pointer = #pointer + 1
		END IF
		IF #flags AND 4 THEN
			SOUND 2,sound_data(#pointer) / 16, sound_data(#pointer) AND $0F
			#pointer = #pointer + 1
		END IF
		SOUND 4,,$38
		sound_counter = (#flags / 16) - 1
	ELSE
		sound_counter = sound_counter - 1
	END IF

	END

        '
        ' The chunks of sound data
        '
sound_data:
	DATA 0

sound_sample1:
        INCLUDE "music_sample1.bas"

'sound_sample2:
'       INCLUDE "music_sample2.bas"


