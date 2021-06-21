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
impulse_response = np.fft.ifft(frames_mean, N)[:512]

# filtering
sim_ontone = (scipy.signal.lfilter(np.real(impulse_response), [1], offtone)).astype(np.int16)
sim_onsentence = (scipy.signal.lfilter(np.real(impulse_response), [1], offsentence)).astype(np.int16)


# -------------------- output ---------------------

# sound
wavfile.write('../audio/sim_maskon_{tone}.wav', 16000, sim_ontone)
wavfile.write('../audio/sim_maskon_{sentence}.wav', 16000, sim_onsentence)

# graphs
fig, axs = plt.subplots(3)

x_time_on = np.arange(0, onsentence.size/fs, 1/fs)
axs[0].set_title("Veta s rúškom")
axs[0].set_xlabel("time")
axs[0].plot(x_time_on, onsentence)

x_time_sim_on = np.arange(0, sim_onsentence.size/fs, 1/fs)
axs[1].set_title("Veta so simulovaným rúškom")
axs[1].set_xlabel("time")
axs[1].plot(x_time_sim_on, sim_onsentence)

x_time_off = np.arange(0/fs, offsentence.size/fs, 1/fs)
axs[2].set_title("Veta bez rúška")
axs[2].set_xlabel("time")
axs[2].plot(x_time_off, offsentence)

fig.tight_layout()
plt.show()
