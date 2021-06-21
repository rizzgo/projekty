import cmath
import math
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


# dft
def manualdft(array, N):
    result = []
    padded_array = np.pad(array, N - array.size, 'constant', constant_values=0)
    for n in range(N):
        sum_k = 0.0
        for k in range(N):
            sum_k += padded_array[k] * cmath.exp(complex(0, -n*k*2*math.pi/N))
        result.append(sum_k)
    return result


# ------------------- main code ---------------------

# use of to_frames
frame_len = 0.020
seg_frames_off = to_frames(frame_len, offtone_seg)
seg_frames_on = to_frames(frame_len, ontone_seg)

# dft use
N = 1024

dft_array_off = []
for frame in seg_frames_off:
    dft_array_off.append(10 * np.log10(np.abs(np.fft.fft(frame, N)[:512])**2))

dft_array_on = []
for frame in seg_frames_on:
    dft_array_on.append(10 * np.log10(np.abs(np.fft.fft(frame, N)[:512])**2))


# -------------------- output ---------------------

# spectrogram
fig, axs = plt.subplots(2)

axs[0].set_title("Spektrogram bez rúška")
graph_off = axs[0].imshow(np.array(dft_array_off).T, origin='lower', aspect='auto', extent=[0, 1, 0, 8000])
fig.colorbar(graph_off, ax=axs[0])
axs[0].set_ylabel("frequency")
axs[0].set_xlabel("time")

axs[1].set_title("Spektrogram s rúškom")
graph_on = axs[1].imshow(np.array(dft_array_on).T, origin='lower', aspect='auto', extent=[0, 1, 0, 8000])
fig.colorbar(graph_on, ax=axs[1])
axs[1].set_ylabel("frequency")
axs[1].set_xlabel("time")

fig.tight_layout()
plt.show()

# dft comparison
fig, axs = plt.subplots(2)

axs[0].set_title("Spektrum rámca počítane np.fft.fft")
axs[0].plot(10 * np.log10(np.abs(np.fft.fft(seg_frames_off[37], N)[:512])**2))

axs[1].set_title("Spektrum rámca počítané vlastnou implemetáciou DFT")
axs[1].plot(10 * np.log10(np.abs(manualdft(seg_frames_off[37], N)[:512])**2))

fig.tight_layout()
plt.show()




