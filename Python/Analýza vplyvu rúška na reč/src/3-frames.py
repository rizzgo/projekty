import numpy as np
from matplotlib import pyplot as plt
from scipy.io import wavfile

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

# division to frames
offtone_seg_frames = []
ontone_seg_frames = []
velkost_ramca = int(0.020 * fs)
prekryv_ramca = int(0.010 * fs)
posun = velkost_ramca - prekryv_ramca

for i in range(0, len(offtone_seg) - posun, posun):
    offtone_seg_frames.append(offtone_seg[i:i + velkost_ramca])

for i in range(0, len(ontone_seg) - posun, posun):
    ontone_seg_frames.append(ontone_seg[i:i + velkost_ramca])

# output
plt.plot(offtone_seg_frames[37], label='bez rúška')
plt.plot(ontone_seg_frames[41], label='s rúškom')
plt.legend()
plt.show()
