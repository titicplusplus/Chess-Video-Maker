import os

for video_file in os.listdir():
    audio_file = "AudioFiles/" + video_file[0:video_file.find(".")] + ".wav"

    os.system(f"ffmpeg -i {video_file} -acodec pcm_s16le -ac 1 -ar 44100 {audio_file}")

