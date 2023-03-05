#include "PitchShifter.h"

PitchShifter::PitchShifter() :
	_in(0),
	_out(0),
	_overlap(0),
	_sample_rate(0.0),
	_shift(0.0),
	_mix(0.5)
{
}


void PitchShifter::prepare(float sample_rate, std::shared_ptr<dsp::FFT> fft, int overlap)
{
	_fft = fft;
	_overlap = overlap;
	_sample_rate = sample_rate;
	_in.resize(fft->getSize());
	_out.resize(fft->getSize());
	_window = std::make_unique<dsp::WindowingFunction<float>>
		(fft->getSize(), dsp::WindowingFunction<float>::WindowingMethod::blackman);
}

void PitchShifter::process(float* samples, int num_samples)
{
	for (int i = 0; i < num_samples; i++) {
		if (!_in.push(samples[i])) processFrame();
		samples[i] = _mix * _out.pop() + (1 - _mix) * samples[i];
	}
}

void PitchShifter::processFrame()
{
	int fft_size = _fft->getSize();
	std::vector<dsp::Complex<float>>
		in_out(fft_size),
		processing(fft_size);
	int hopsize = fft_size / _overlap;

	// Multiply frame by window function
	{
		std::vector<float> temp(fft_size);
		for (int i = 0; i < fft_size; i++) temp[i] = _in.readAtIndex(i);
		_in.remove(hopsize);
		_window->multiplyWithWindowingTable(temp.data(), fft_size);
		for (int i = 0; i < fft_size; i++) in_out[i] = temp[i];
	}
	_fft->perform(in_out.data(), processing.data(), false);
	shiftPitch(processing);
	_fft->perform(processing.data(), in_out.data(), true);
	// Sum fft results into outgoing queue
	for (int i = 0; i < fft_size; i++) {
		auto val = in_out[i].real() / _overlap;
		if (!_out.push(val)) // Push into queue until its full
			_out.writeAtIndex(i, _out.readAtIndex(i) + val);
	}
}

void PitchShifter::shiftPitch(std::vector<dsp::Complex<float>>& frame)
{
	auto num_bins = _fft->getSize();
	int bin_shift = _shift / _sample_rate * num_bins;
	int bin; for (bin = num_bins - 1; bin >= bin_shift; bin--)
	{
		frame[bin] = frame[bin - bin_shift];
	}
	while (bin >= 0) frame[bin--] = 0.0;
}
