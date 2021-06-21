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


# idft
def manualidft(array, N):
    result = []
    padded_array = np.pad(array, N - array.size, 'constant', constant_values=0)
    for k in range(N):
        sum_n = 0.0
        for n in range(N):
            sum_n += padded_array[n] * cmath.exp(complex(0, n*k*2*math.pi/N))
        result.append(sum_n / N)
    return result


# ------------------- main code ---------------------

# use of to_frames
frame_len = 0.020
seg_frames_off = to_frames(frame_len, offtone_seg)
seg_frames_on = to_frames(frame_len, ontone_seg)

# dft use
N = 1024
frame_len_in_samples = int(frame_len * fs)  # fs = sampling frequency

dft_array_off = []
for frame in seg_frames_off:
    dft_array_off.append(np.abs(np.fft.fft(frame, N)[:512]))

dft_array_on = []
for frame in seg_frames_on:
    dft_array_on.append(np.abs(np.fft.fft(frame, N)[:512]))

# frequency characteristic
seg_frames_char = []
frames_sum = np.zeros(512)
seg_frames_len = len(seg_frames_off)

for i in range(seg_frames_len):
    seg_frames_char.append(dft_array_on[i] / dft_array_off[i])
    frames_sum = np.add(frames_sum, seg_frames_char[i])

frames_mean = np.divide(frames_sum, seg_frames_len)

# impulse response
impulse_response_ifft = np.fft.ifft(frames_mean, N)[:512]
impulse_response_manual = manualidft(frames_mean, N)[:512]  # manual idft use

# -------------------- output ---------------------

plt.title("Impulzná odozva rúška")
plt.plot(np.real(impulse_response_ifft))
plt.xlabel("vzorky")
plt.show()

# idft comparison
fig, axs = plt.subplots(2)

axs[0].set_title("Impulzná odozva počítaná np.fft.ifft")
axs[0].plot(np.real(impulse_response_ifft))

axs[1].set_title("Impulzná odozva počítaná vlastnou implemetáciou IDFT")
axs[1].plot(np.real(impulse_response_manual))

fig.tight_layout()
plt.show()
