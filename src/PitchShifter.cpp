#include "PitchShifter.h"

PitchShifter::PitchShifter() :
	_in(0),
	_out(0),
	_overlap(0),
	_sample_rate(0.0),
	_shift_factor(0.0),
	_shift_frequency(-1.0),
	_mix(0.5)
{
}

void PitchShifter::prepare(float sample_rate, std::shared_ptr<dsp::FFT> fft, int overlap)
{
	auto new_size = fft->getSize();
	auto hopsize = new_size / overlap;

	_fft = fft;
	_overlap = overlap;
	_sample_rate = sample_rate;
	_in.resize(new_size, -hopsize);
	_out.resize(new_size, -hopsize);
	_window = std::make_unique<dsp::WindowingFunction<float>>
		(new_size, dsp::WindowingFunction<float>::WindowingMethod::hann);
	_last_phase.resize(new_size);
	_running_phase.resize(new_size);
}

void PitchShifter::process(float* samples, int num_samples)
{
	for (int i = 0; i < num_samples; i++) {
		if (!_in.push(samples[i])) {
			processFrame();
			_in.push(samples[i]);
		}
		samples[i] = _mix * _out.pop() + (1 - _mix) * samples[i];
	}
}

void PitchShifter::processFrame()
{
	//_shift_factor = detectPitch(2, 110) / _shift_frequency;
	//if (_shift_factor < 0.0) _shift_factor = 1.0;
	//_shift_factor = 1.0;
	int fft_size = _fft->getSize();
	std::vector<dsp::Complex<float>>
		in_out(fft_size),
		acfs(fft_size),
		acfs2(fft_size),
		processing(fft_size);
	int hopsize = fft_size / _overlap;

	// Multiply frame by window function
	{
		std::vector<float> input(fft_size);
		for (int i = 0; i < fft_size; i++) input[i] = _in.readAtIndex(i);
		_in.remove(hopsize);
		_window->multiplyWithWindowingTable(input.data(), fft_size);
		for (int i = 0; i < fft_size; i++) in_out[i] = input[i];
	}

	_fft->perform(in_out.data(), processing.data(), false);
	std::copy(processing.begin(), processing.end(), acfs.begin());

	// Detect pitch and get shift factor by dividing detected pitch by desired pitch
	{
		std::vector<float> acfsf(fft_size);
		std::transform(acfs.begin(), acfs.end(), acfs.begin(),
			[](dsp::Complex<float> c) { return std::norm(c); });
		for (int i = 0; i < acfs.size(); i++)
		{
			acfs[i] *= std::conj(acfs[i]);
		}
		_fft->perform(acfs.data(), acfs2.data(), true);
		std::transform(acfs2.begin(), acfs2.end(), acfsf.begin(),
			[](dsp::Complex<float> c) -> float { return c.real(); });
		float pitch = detectPitch(acfsf);
		_shift_factor = _shift_frequency / pitch;
	}

	shiftPitch(processing);

	_fft->perform(processing.data(), in_out.data(), true);

	// Sum fft results into outgoing queue
	std::vector<float> output(fft_size);
	for (int i = 0; i < fft_size; i++) output[i] = in_out[i].real();
	_window->multiplyWithWindowingTable(output.data(), fft_size);
	for (int i = 0; i < fft_size; i++) {
			_out.push(0.0);
			_out.writeAtIndex(i, _out.readAtIndex(i) + output[i]);
	}
}

float PitchShifter::detectPitch(std::vector<float> acfs)
{
	int frame_size = acfs.size();
	std::vector<float> values(frame_size);

	float sum = 0.0;
	for (int L = 0; L < frame_size; L++) {
		values[L] = (2 * acfs[0] - 2 * acfs[L]) / (frame_size - L);
		sum += values[L];
		if (L == 0) values[L] = 1.0;
		else        values[L] *= L / sum;
	}

	int argMin = 0;
	int argMaxACF = 2;
	for (int i = 0; i < values.size(); i++) {
		if (values[i] < values[argMin]) argMin = i;
		if (i > 1 && acfs[i] > acfs[argMaxACF]) argMaxACF = i;
	}
	float sample = argMin;
#if(_DEBUG)
		getDebugVariableBroadcaster().setVariable(_sample_rate / sample);
#endif
	return _sample_rate / sample;
}

std::vector<int> getPeaks(const std::vector<dsp::Complex<float>>& frame)
{
	std::vector<int> peaks;
	float prev_2 = std::abs(frame[0]);
	float prev_1 = std::abs(frame[1]);
	float curr   = std::abs(frame[2]);
	float next_1 = std::abs(frame[3]);
	float next_2 = std::abs(frame[4]);
	for (int i = 2; i < frame.size() - 2; i++) {
		if (curr > prev_2
			&& curr > prev_1
			&& curr > next_1
			&& curr > next_2) peaks.push_back(i);
		prev_2 = prev_1;
		prev_1 = curr;
		curr = next_1;
		next_1 = next_2;
		next_2 = std::abs(frame[i + 2]);
	}
	return peaks;
}

void PitchShifter::shiftPitch(std::vector<dsp::Complex<float>>& frame)
{
	std::vector<int> peaks = getPeaks(frame);
	auto num_bins = _fft->getSize();
	float pi = MathConstants<float>::pi;
	float analysis_hop = num_bins / _overlap;
	float synthesis_hop = analysis_hop / _shift_factor;
	std::vector<dsp::Complex<float>> shifted_frame(num_bins);
	for (int peak = 0; peak < peaks.size(); peak++) {
		int start_bin;
		int end_bin;
		int bin_shift = peaks[peak] * _shift_factor - peaks[peak];

		if (peak == 0) start_bin = 0;
		else start_bin = (peaks[peak] + peaks[peak - 1]) / 2;

		if (peak == peaks.size() - 1) end_bin = num_bins;
		else end_bin = (peaks[peak] + peaks[peak + 1]) / 2;

		for (int bin = start_bin; bin < end_bin; bin++) {
			int new_bin = bin + bin_shift;
			_running_phase[bin] += 2 * pi * bin_shift / _overlap;
			dsp::Complex<float> Z(std::cos(_running_phase[bin]), std::sin(_running_phase[bin]));
			if (new_bin < 0) shifted_frame[-new_bin] += std::conj(frame[bin] * Z);
			else if (new_bin < num_bins) shifted_frame[new_bin] += frame[bin] * Z;
		}
	}
	for (int bin = 0; bin < num_bins; bin++)
		frame[bin] = shifted_frame[bin];
	/*
	for (int bin = 0; bin < num_bins; bin++) {
		float bin_phase = atan2(shifted_frame[bin].imag(), shifted_frame[bin].real());
		float bin_magnitude = abs(shifted_frame[bin]);
		float bin_frequency = bin / num_bins * _sample_rate / 2;
		float heterodyned_phase_increment = bin_phase - _last_phase[bin] + analysis_hop * bin_frequency;
		float true_frequency_estimate = bin_frequency + heterodyned_phase_increment / analysis_hop;
		_running_phase[bin] += synthesis_hop * true_frequency_estimate;
		_last_phase[bin] = bin_phase;

		frame[bin] = std::polar<float>(bin_magnitude, _running_phase[bin]);
	}
	*/
}
