import numpy as np
from matplotlib import pyplot as plt
from scipy.io import wavfile


# -------------------- start ----------------------

# import
fs, offtone = wavfile.read('../audio/maskoff_tone.wav')
offtone = offtone.copy()
fs, ontone = wavfile.read('../audio/maskon_tone.wav')
ontone = ontone.copy()

# segments
odkial = 5.3
dlzka = 1
odkial_vzorky = int(odkial * fs)
pokial_vzorky = int((odkial + dlzka) * fs)
offtone_seg = offtone[odkial_vzorky:pokial_vzorky]
ontone_seg = ontone[odkial_vzorky:pokial_vzorky]

# normalization
offtone_seg = offtone_seg - np.mean(offtone_seg)
offtone_seg = offtone_seg / np.abs(offtone_seg).max()

ontone_seg = ontone_seg - np.mean(ontone_seg)
ontone_seg = ontone_seg / np.abs(ontone_seg).max()


# -------------- functions -----------------

# division to frames
def to_frames(frame_len, array):
    frame_array = []
    velkost_ramca = int(frame_len * fs)
    prekryv_ramca = int(0.010 * fs)
    posun = velkost_ramca - prekryv_ramca

    for i in range(0, len(array) - posun, posun):
        frame_array.append(array[i:i + velkost_ramca].copy())

    return frame_array


# ------------------- main code ---------------------

# use of to_frames
frame_len = 0.020
seg_frames_off = to_frames(frame_len, offtone_seg)
seg_frames_on = to_frames(frame_len, ontone_seg)

# dft use
N = 1024

# Hann window function use
hann = np.hanning(51)
hann_freq = np.fft.fft(hann, 2048) / 25.5
with np.errstate(divide='ignore', invalid='ignore'):
    hann_freq_pretty = 20 * np.log10(np.abs(np.fft.fftshift(hann_freq)))

hann_window = np.hanning(320)
example_frame = seg_frames_off[37]
example_frame_hann = np.multiply(hann_window, example_frame)

# -------------------- output ---------------------

# hann window
fig1, axs1 = plt.subplots(2)

axs1[0].set_title("Hannova funkcia")
axs1[0].plot(hann)

axs1[1].set_title("Spektrum Hannovej funkcie")
axs1[1].plot(hann_freq_pretty)
fig1.tight_layout()
plt.show()

# compare
fig2, axs2 = plt.subplots(2)

axs2[0].set_title("Spektrum rámca")
axs2[0].plot(10 * np.log10(np.abs(np.fft.fft(example_frame, N)[:512])**2))

axs2[1].set_title("Spektrum rámca s použitím Hannovej funkcie")
axs2[1].plot(10 * np.log10(np.abs(np.fft.fft(example_frame_hann, N)[:512])**2))

fig2.tight_layout()
plt.show()




