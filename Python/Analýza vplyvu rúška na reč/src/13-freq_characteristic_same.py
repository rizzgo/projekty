import numpy as np
from matplotlib import pyplot as plt
from scipy.io import wavfile
import scipy.signal


# -------------------- start ----------------------

# import
fs, offtone = wavfile.read('../audio/maskoff_tone.wav')
offtone = offtone.copy()
fs, ontone = wavfile.read('../audio/maskon_tone.wav')
ontone = ontone.copy()

fs, offsentence = wavfile.read('../audio/maskoff_sentence.wav')
offsentence = offsentence.copy()
fs, onsentence = wavfile.read('../audio/maskon_sentence.wav')
onsentence = onsentence.copy()

# segments
odkial = 5.3
dlzka = 1
odkial_vzorky = int(odkial * fs)
pokial_vzorky = int((odkial + dlzka) * fs)
offtone_seg = offtone[odkial_vzorky:pokial_vzorky]
ontone_seg = ontone[odkial_vzorky:pokial_vzorky]

# normalization

# tone segments
offtone_seg = offtone_seg - np.mean(offtone_seg)
offtone_seg = offtone_seg / np.abs(offtone_seg).max()

ontone_seg = ontone_seg - np.mean(ontone_seg)
ontone_seg = ontone_seg / np.abs(ontone_seg).max()

# sentence
onsentence = onsentence[23000:63000]

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


# central clipping
def cclipping(array):
    clipped_array = np.copy(array)
    absolute_max = np.max(np.abs(clipped_array))
    seventy_p = 0.7 * absolute_max
    for i in range(len(clipped_array)):
        if clipped_array[i] > 0 and clipped_array[i] > seventy_p:
            clipped_array[i] = 1
        elif clipped_array[i] < 0 and clipped_array[i] < -seventy_p:
            clipped_array[i] = -1
        else:
            clipped_array[i] = 0
    return clipped_array


# autocorrelation
def acorrelate(frame_len, array):
    result = []
    frame_length = int(frame_len * 16000)

    for k in range(0, frame_length):
        sum_n = 0.0
        for n in range(len(array) - k):
            sum_n += array[n] * array[n + k]
        result.append(sum_n)
    return result


def lag(threshold, autocorrelated):
    lag_in_samples = np.argmax(autocorrelated[threshold:]) + threshold
    lag_in_seconds = lag_in_samples/16000
    return 1 / lag_in_seconds


def lag_in_samples(threshold, autocorrelated):
    return np.argmax(autocorrelated[threshold:]) + threshold


# ------------------- main code ---------------------

# use of to_frames
frame_len = 0.020
seg_frames_off = to_frames(frame_len, offtone_seg)
seg_frames_on = to_frames(frame_len, ontone_seg)

# use of cclipping
clipped_frames_off = []
for frame in seg_frames_off:
    clipped_frames_off.append(cclipping(frame))

clipped_frames_on = []
for frame in seg_frames_on:
    clipped_frames_on.append(cclipping(frame))

# fundamental frequency
threshold = 32
frequency_off = []
for frame in clipped_frames_off:
    frequency_off.append(lag(threshold, acorrelate(frame_len, frame)))

frequency_on = []
for frame in clipped_frames_on:
    frequency_on.append(lag(threshold, acorrelate(frame_len, frame)))

# dft use
N = 1024

# for same frames
freq_array_len = len(frequency_off)
same_array_off = []
same_array_on = []
for i in range(freq_array_len):
    if frequency_off[i] == frequency_on[i]:
        same_array_off.append(np.abs(np.fft.fft(seg_frames_off[i], N)[:512]))
        same_array_on.append(np.abs(np.fft.fft(seg_frames_on[i], N)[:512]))

# for all frames
dft_array_off = []
for frame in seg_frames_off:
    dft_array_off.append(np.abs(np.fft.fft(frame, N)[:512]))

dft_array_on = []
for frame in seg_frames_on:
    dft_array_on.append(np.abs(np.fft.fft(frame, N)[:512]))

# frequency characteristic

# from same frames
seg_frames_char_bonus = []
frames_sum_bonus = np.zeros(512)
same_array_len = len(same_array_off)

for i in range(same_array_len):
    seg_frames_char_bonus.append(same_array_on[i] / same_array_off[i])
    frames_sum_bonus = np.add(frames_sum_bonus, seg_frames_char_bonus[i])

frames_mean_bonus = np.divide(frames_sum_bonus, same_array_len)
frames_mean_bonus_pretty = 10 * np.log10(frames_mean_bonus ** 2)

# from all frames
seg_frames_char = []
frames_sum = np.zeros(512)
seg_frames_len = len(seg_frames_off)
for i in range(seg_frames_len):
    seg_frames_char.append(dft_array_on[i] / dft_array_off[i])
    frames_sum = np.add(frames_sum, seg_frames_char[i])

frames_mean = np.divide(frames_sum, seg_frames_len)
frames_mean_pretty = 10 * np.log10(frames_mean**2)

# impulse response
impulse_response = np.fft.ifft(frames_mean, N)[:512]

# filtering
sim_ontone = (scipy.signal.lfilter(np.real(impulse_response), [1], offtone)).astype(np.int16)
sim_onsentence = (scipy.signal.lfilter(np.real(impulse_response), [1], offsentence)).astype(np.int16)

# -------------------- output ---------------------

# sound
wavfile.write('../audio/sim_maskon_{tone}_only_match.wav', 16000, sim_ontone)
wavfile.write('../audio/sim_maskon_{sentence}_only_match.wav', 16000, sim_onsentence)

# graphs
fig, axs = plt.subplots(2)

x_time = np.arange(1/16000, 321/16000, 1/16000)

axs[0].set_title("Frekvenčná charakterisitka rúška")
x_freq = np.arange(0, 8000, 8000/512)
axs[0].plot(x_freq, frames_mean_pretty)
axs[0].set_xlabel("Frekvencia")
axs[0].set_ylabel("Spektrálna hustota výkonu")

axs[1].set_title("Frekvenčná charakterisitka rúška z rámcov s rovnakým základným tónom")
x_freq = np.arange(0, 8000, 8000/512)
axs[1].plot(x_freq, frames_mean_bonus_pretty)
axs[1].set_xlabel("Frekvencia")
axs[1].set_ylabel("Spektrálna hustota výkonu")

fig.tight_layout()
plt.show()

# sentence comparison
fig, axs = plt.subplots(3)

x_time_on = np.arange(0, onsentence.size/fs, 1/fs)
axs[0].set_title("Veta s rúškom")
axs[0].set_xlabel("time")
axs[0].plot(x_time_on, onsentence)

x_time_sim_on = np.arange(0, sim_onsentence.size/fs, 1/fs)
axs[1].set_title("Veta so simulovaným rúškom (použitá Hannova funkcia)")
axs[1].set_xlabel("time")
axs[1].plot(x_time_sim_on, sim_onsentence)

x_time_off = np.arange(0/fs, offsentence.size/fs, 1/fs)
axs[2].set_title("Veta bez rúška")
axs[2].set_xlabel("time")
axs[2].plot(x_time_off, offsentence)

fig.tight_layout()
plt.show()
