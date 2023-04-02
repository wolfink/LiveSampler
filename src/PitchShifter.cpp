#include "PitchShifter.h"

PitchShifter::PitchShifter() :
	_in(0),
	_out(0),
	_overlap(0),
	_sample_rate(0.0),
	_pitch_estimates{ 440.0, 440.0, 440.0 },
	_voices(*this),
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
	_voices.setSize(8);
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
	const int fft_size = _fft->getSize();
	const int hopsize = fft_size / _overlap;

	std::vector<dsp::Complex<float>>
		in_out(fft_size),
		acfs(fft_size),
		acfs2(fft_size),
		processing(fft_size);

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
	for (int i = 0; i < acfs.size(); i++) acfs[i] *= std::conj(acfs[i]);
	_fft->perform(acfs.data(), acfs2.data(), true);
	std::transform(acfs2.begin(), acfs2.end(), acfsf.begin(),
		[](dsp::Complex<float> c) -> float { return c.real(); });
	_pitch_estimates[0] = _pitch_estimates[1];
	_pitch_estimates[1] = _pitch_estimates[2];
	_pitch_estimates[2] = detectPitch(acfsf);
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

inline float parabolaMaxMin(double x1, double y1, double x2, double y2, double x3, double y3)
{
	const double denom = (x1 - x2) * (x1 - x3) * (x2 - x3);
	const double a = 2 * (x3 * (y2 - y1) + x2 * (y1 - y3) + x1 * (y3 - y2)) / denom;
	const double b = (std::pow(x3, 2) * (y1 - y2)
		+ std::pow(x2, 2) * (y3 - y1)
		+ std::pow(x1, 2) * (y2 - y3)) / denom;
	return -b / a;
}

float PitchShifter::detectPitch(std::vector<float> acfs)
{
	const int frame_size = acfs.size();
	std::vector<float> values(frame_size);

	float sum = 0.0;
	for (int L = 0; L < frame_size; L++) {
		values[L] = (2 * acfs[0] - 2 * acfs[L]) / (frame_size - L);
		sum += values[L];
		if (L == 0) values[L] = 1.0;
		else        values[L] *= L / sum;
	}

	int sample = 0;
	float sample_frac = 0.0;
	const float threshold = 0.1;
	const int values_size = values.size();
	for (int i = 2; i < values_size; i++) {
		if (i < values_size - 1 && values[i] < threshold) {
			sample = i;
			break;
		}
		if (values[i] < values[sample]) sample = i;
	}
	if (sample > 0 && sample < values_size - 1)
		sample_frac = parabolaMaxMin(
			sample, values[sample],
			sample - 1, values[sample - 1],
			sample + 1, values[sample + 1]);
	else sample_frac = sample;
	_pitch_differences[0] = _pitch_differences[1];
	_pitch_differences[1] = _pitch_differences[2];
	_pitch_differences[2] = values[sample];
	return _sample_rate / sample_frac;
}

std::vector<float> getPeaks(const std::vector<dsp::Complex<float>>& frame)
{
	std::vector<float> peaks;
	std::vector<float> mags(frame.size());
	std::transform(frame.begin(), frame.end(), mags.begin(),
		[](dsp::Complex<float> f) { return std::abs(f); });
	for (int i = 2; i < frame.size() - 2; i++) {
		if (mags[i] > mags[i - 2]
			&& mags[i] > mags[i - 1]
			&& mags[i] > mags[i + 1]
			&& mags[i] > mags[i + 2]) {

			double x1 = i, x2 = i - 1, x3 = i + 1,
				y1 = mags[i], y2 = mags[i - 1], y3 = mags[i + 1];

			float peak_est = parabolaMaxMin(i, mags[i], i - 1, mags[i - 1], i + 1, mags[i + 1]);

			peaks.push_back(peak_est);
		}
	}
	return peaks;
}

void PitchShifter::shiftPitch(std::vector<dsp::Complex<float>>& frame)
{
	const std::vector<float> peaks = getPeaks(frame);
	const auto num_bins = _fft->getSize();
	const float pi = MathConstants<float>::pi;

	std::vector<dsp::Complex<float>> processed_frame(num_bins);

	for (int voice = 0; voice < _voices.getSize(); voice++) {

		const float freq = _voices.getVoice(voice);
		const float pitch = [this]() {
			float pitch = _pitch_estimates[0];
			if (_pitch_differences[0] <= _pitch_differences[1]
				&& _pitch_differences[0] <= _pitch_differences[2]) pitch = _pitch_estimates[0];
			else if (_pitch_differences[1] < _pitch_differences[2]) pitch = _pitch_estimates[1];
			else pitch = _pitch_estimates[2];
#if(_DEBUG)
			getDebugVariableBroadcaster().setVariable(pitch);
#endif
			return pitch;
		}();
		const float shift_factor = freq / pitch;

		auto running_phase = _voices.getPhase(voice);
		std::vector<dsp::Complex<float>> shifted_frame(num_bins);

		for (int peak = 0; peak < peaks.size(); peak++) {
			const float bin_shift = peaks[peak] * shift_factor - peaks[peak];
			const int bin_shift_int = bin_shift;
			const float bin_shift_frac = std::abs(bin_shift - bin_shift_int);
			const float alpha = (1 - bin_shift_frac) / (1 + bin_shift_frac);

			int start_bin;
			int end_bin;

			if (peak == 0) start_bin = 0;
			else start_bin = (peaks[peak] + peaks[peak - 1]) / 2;

			if (peak == peaks.size() - 1) end_bin = num_bins;
			else end_bin = (peaks[peak] + peaks[peak + 1]) / 2;

			if (bin_shift < 0) for (int bin = start_bin; bin < end_bin; bin++) {
				int new_bin = bin + bin_shift_int;

				// Calculate Thiran interpolation
				dsp::Complex<float> v = 0,
					value1 = frame[bin],
					value2 = bin + 1 < num_bins ? frame[bin + 1] : 0;
				auto output = bin_shift_frac == 0 ? value1 : value2 + alpha * (value1 - v);
				v = output;

				// Calculate phase correction
				running_phase[bin] += 2 * pi * bin_shift / _overlap;
				dsp::Complex<float> Z(std::cos(running_phase[bin]), std::sin(running_phase[bin]));

				output *= Z;
				if (new_bin < 0) {
					new_bin = -new_bin;
					output = std::conj(output);
				}
				if (new_bin < num_bins) shifted_frame[new_bin] += output;
			}
			// Run backwards if shifting down
			else for (int bin = end_bin - 1; bin >= start_bin; bin--) {
				const int new_bin = bin + bin_shift_int;

				dsp::Complex<float> v = 0,
					value1 = frame[bin],
					value2 = bin - 1 >= 0 ? frame[bin - 1] : 0;
				const auto output = bin_shift_frac == 0 ? value1 : value2 + alpha * (value1 - v);
				v = output;

				running_phase[bin] += 2 * pi * bin_shift / _overlap;
				dsp::Complex<float> Z(std::cos(running_phase[bin]), std::sin(running_phase[bin]));

				if (new_bin < 0) shifted_frame[-new_bin] += std::conj(output * Z);
				else if (new_bin < num_bins) shifted_frame[new_bin] += output * Z;

			}
		}
		for (int bin = 0; bin < num_bins; bin++)
			processed_frame[bin] += shifted_frame[bin];
	}
	std::copy(processed_frame.begin(), processed_frame.end(), frame.begin());
}
