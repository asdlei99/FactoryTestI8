ffmpeg ת�� mp3 Ϊ pcm �����

ffmpeg -i music.mp3 -f s16le -ar 16000 -ac 1 -acodec pcm_s16le startrecord.pcm

