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


# central clipping
def cclipping(array):
    absolute_max = np.max(np.abs(array))
    seventy_p = 0.7 * absolute_max
    for i in range(len(array)):
        if array[i] > 0 and array[i] > seventy_p:
            array[i] = 1
        elif array[i] < 0 and array[i] < -seventy_p:
            array[i] = -1
        else:
            array[i] = 0


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

unclipped_frame = seg_frames_off[37].copy()

# use of cclipping
for frame in seg_frames_off:
    cclipping(frame)

for frame in seg_frames_on:
    cclipping(frame)

clipped_frame = seg_frames_off[37]

# fundamental frequency
threshold = 32
frequency_off = []
for frame in seg_frames_off:
    frequency_off.append(lag(threshold, acorrelate(frame_len, frame)))

frequency_on = []
for frame in seg_frames_on:
    frequency_on.append(lag(threshold, acorrelate(frame_len, frame)))

autocorrelated_frame = acorrelate(frame_len, clipped_frame)
lag_xpos = lag_in_samples(threshold, autocorrelated_frame)


# -------------------- output ---------------------

fig, axs = plt.subplots(4)

x_time = np.arange(1/16000, 321/16000, 1/16000)
axs[0].set_title("Rámec")
axs[0].plot(x_time, unclipped_frame)
axs[0].set_ylabel("y")
axs[0].set_xlabel("time")

axs[1].set_title("Centrálne klipovanie s 70%")
axs[1].plot(clipped_frame)
axs[1].set_ylabel("y")
axs[1].set_xlabel("vzorky")

axs[2].set_title("Autokorelácia")
axs[2].plot(autocorrelated_frame)
axs[2].set_ylabel("y")
axs[2].set_xlabel("vzorky")
axs[2].axvline(threshold, color='k', label='plot')
axs[2].stem([int(lag_xpos)], [int(autocorrelated_frame[lag_xpos])], linefmt='C3-', label='lag')

axs[3].set_title("Základná frekvencia rámcov")
axs[3].plot(frequency_off, label='bez rúška')
axs[3].plot(frequency_on, label='s rúškom')
axs[3].set_ylabel("f0")
axs[3].set_xlabel("rámce")

fig.tight_layout()
plt.legend()
plt.show()

frequency_mean_off = np.mean(frequency_off)
frequency_mean_on = np.mean(frequency_on)
variance_off = np.var(frequency_off)
variance_on = np.var(frequency_on)

# table
print(frequency_mean_off)
print(frequency_mean_on)
print(variance_off)
print(variance_on)
