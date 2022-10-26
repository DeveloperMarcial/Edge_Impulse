# ************************************************************************************
# * The MIT License (MIT)                                                            *
# *                                                                                  *
# * Copyright (c) 2022 M. Marcial                                                    *
# * If you use this code, please give me some credit.                                *
# *                                                                                  *
# * Permission is hereby granted, free of charge, to any person obtaining a copy     *
# * of this software and associated documentation files (the "Software"), to deal    *
# * in the Software without restriction, including without limitation the rights     *
# * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell        *
# * copies of the Software, and to permit persons to whom the Software is            *
# * furnished to do so, subject to the following conditions:                         *
# * The above copyright notice shall be included in                                  *    
# * all copies or substantial portions of the Software.                              *
# *                                                                                  *
# * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR       *
# * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,         *
# * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE      *
# * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER           *
# * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,    *
# * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE    *   
# * SOFTWARE.                                                                        *
# ************************************************************************************

import numpy as np
from scipy import signal
from scipy.signal import butter, lfilter, freqz
import matplotlib.pyplot as plt

from scipy.stats import skew
from scipy.stats import kurtosis as calculateKurtosis
from scipy.stats import gmean
from scipy.stats import pmean

import logging, sys
logging.basicConfig(stream=sys.stderr, level=logging.DEBUG)

def butter_bandpass(order, lowcut, highcut, fs):
	#return signal.butter(order, [lowcut, highcut], fs=fs, btype='bandpass', analog=False, output='sos')
	return signal.butter(order, [lowcut, highcut], fs=fs, btype='bandpass')

def butter_bandpass_filter(data, order, lowcut, highcut, fs):
	b, a = butter_bandpass(order, lowcut, highcut, fs)
	y = lfilter(b, a, data)
	return y

def butter_lowpass(order, cutoff, fs, normalizeHz_doIt):

	Wn = cutoff

	if (normalizeHz_doIt):
		# Normalized frequency
		Wn = cutoff * 2 / fs

	#from common.errors import ConfigurationError
	# Catch when frequency too low
	#if (Wn >= 1.0):
	#	raise ConfigurationError('Cut-off frequency is above Nyquist (1/2 sample rate (' + str(freq_hz/2)+')) Choose lower cutoff frequency.')

	# fs cannot be specified for an analog filter.
	return signal.butter(order, Wn, btype='lowpass', analog=False, fs=fs, output='sos')

#f	 butter_lowpass_filter(fx,	   orderOfFilter, criticalFrequency, sampleRateHz)
def butter_lowpass_filter(data, order,				cutoff,					fs, normalizeHz_doIt):

	butter_sos = butter_lowpass(order, cutoff, fs, normalizeHz_doIt)
	butter_sos = np.float32(butter_sos)
	y = signal.sosfilt(butter_sos, data)

	#b, a = butter_lowpass(order, cutoff, fs)
	#y = lfilter(b, a, data)
	return y

#def butterworth(sig, order, critical):
#	#b, a = signal.butter(orderOfFilter, criticalFrequency, 'bandpass', analog=True)
#	#return b,a
#	sos = signal.butter(sig, order, critical, 'bandpass', fs=1000, output='sos')
#	filtered = signal.sosfilt(sos, sig)
#	return filtered

def smoothing(y, box_pts):
	box = np.ones(box_pts) / box_pts
	y_smooth = np.convolve(y, box, mode='same')
	return y_smooth

def generate_features(implementation_version,
								 draw_graphs,
								 raw_data, axes,
								 sampling_freq,
								 scale_axes,
								 smooth,
								 butterworth_doIt,
								 normalizeHz_doIt,
								 orderOfFilter,
								 criticalFrequency,
								 sampleRateHz,
								 notchFilter_doIt,
								 orderOfNotchFilter,
								 lowerFrequency,
								 upperFrequency
								 ):

	#  For ‘bandpass’ and ‘bandstop’ filters, the resulting order of the final second-order sections (‘sos’) matrix
	# is 2*N, with N the number of biquad sections of the desired system.
	
	# features is a 1D array, reshape so we have a matrix with one raw per axis
	raw_data = raw_data.reshape(int(len(raw_data) / len(axes)), len(axes))
	logging.info('raw_data length = %d', len(raw_data))
	# The number of raw features per channel is:
	#	Window Size / 16.

	features = []
	labels = []
	smoothed_graph = {}

	# split out the data from all axes
	for ax in range(0, len(axes)):
		X = []
		for ix in range(0, raw_data.shape[0]):
			X.append(raw_data[ix][ax])

		# X now contains only the current axis
		fx = np.array(X)
		logging.info('fx length = %d', len(fx))

		# first scale the values
		fx = fx * scale_axes

		# if smoothing is enabled, do that
		if (smooth):
			fx = smoothing(fx, 5)

		if (butterworth_doIt):
			#fx = butterworth(fx, orderOfFilter, criticalFrequency)
			# Filter requirements.
			#order = 6
			#fs = 30.0		 # sample rate, Hz
			#cutoff = 3.667	 # desired cutoff frequency of the filter, Hz

			# Get the filter coefficients so we can check its frequency response.
			fx = butter_lowpass_filter(fx, orderOfFilter, criticalFrequency, sampleRateHz, normalizeHz_doIt)
			logging.info("Executed butter_lowpass_filter")

		if (notchFilter_doIt):
			 #	  butter_bandpass_filter(data,	order,						 lowcut,			   highcut,				 fs,				   ):
			fx = butter_bandpass_filter(fx,	  orderOfNotchFilter,  lowerFrequency, upperFrequency, sampleRateHz)
		
		# we save bandwidth by only drawing graphs when needed
		if (draw_graphs):
			smoothed_graph[axes[ax]] = list(fx)

		# we need to return a 1D array again, so flatten here again
		for f in fx:
			features.append(f)
			labels.append("PD")

		# draw the graph with time in the window on the Y axis, and the values on the X axes
		# note that the 'suggestedYMin/suggestedYMax' names are incorrect, they describe
		# the min/max of the X axis
		graphs = []
		if (draw_graphs):
			graphs.append({
			'name': 'Smoothed',
			'X': smoothed_graph,
			'y': np.linspace(0.0, raw_data.shape[0] * (1 / sampling_freq) * 1000, raw_data.shape[0] + 1).tolist(),
			'suggestedYMin': -20,
			'suggestedYMax': 20
		}) # End 'graphs.append()'

		features.append(float(skew(fx)))
		labels.append('Skewness')
		# kurtosis(a[, axis, fisher, bias, ...]), Compute the kurtosis (Fisher or Pearson) of a dataset.
		features.append(float(calculateKurtosis(fx)))
		labels.append('Kurtosis')
		#features.append(float(gmean(fx)))
		#labels.append('GMean')
		#features.append(float(pmean(fx,2)))
		#labels.append('PMean')
		
		#https://numpy.org/doc/stable/reference/routines.statistics.html
		features.append(float(np.mean(fx)))
		labels.append('Mean')
		features.append(float(np.median(fx)))
		labels.append('Median')	 
		features.append(float(np.std(fx)))
		labels.append('StDev')		  
		features.append(float(np.std(fx)))
		labels.append('StDev')		  
		
		#logging.debug('A debug message!')
		logging.info('features length = %d', len(features))
		logging.info('labels length = %d', len(labels))
		#raise ConfigurationError('...Example Config Error...')
		
#	 return { 'features': features, 'graphs': graphs }
	return {
		'features': features,
		'graphs': graphs,
		'labels': labels,
		# if you use FFTs then set the used FFTs here (this helps with memory optimization on MCUs)
		'fft_used': [],
		'output_config': {
		# type can be 'flat', 'image' or 'spectrogram'
		'type': 'flat',
		'shape': {
		# shape should be { width, height, channels } for image, { width, height } for spectrogram
		'width': len(features)
		}
		}
	}
