#include <JuceHeader.h>
#include "LiveSampler.h"

class PitchShifter
{
	std::shared_ptr<dsp::FFT> _fft;
	uptr<dsp::WindowingFunction<float>> _window;
	int _overlap;
	float _shift;
	float _sample_rate;
	float _mix;


public:
	PitchShifter();
	void prepare(float sample_rate, std::shared_ptr<dsp::FFT> fft, int overlap);
	void process(float*, int num_samples);
	void setShift(float shift) { _shift = shift; }
	void setMix(float mix) { _mix = mix; }

private:
	void processFrame();
	void shiftPitch(std::vector<dsp::Complex<float>>& frame);

	class Queue
	{
		std::vector<float> _arr;
		size_t _start, _end, _size;
	public:
		Queue(int size) : _arr(size), _start(0), _end(0), _size(0) {}
		bool push(float f) 
		{ 
			if (_size >= _arr.size()) return false;

			_end %= _arr.size();
			_arr[_end++] = f;
			_size++;
			return true;
		}
		float pop()
		{
			if (_size < 1) return 0.0;
			_start %= _arr.size();
			_size--;
			return _arr[_start++];
		}
		float readAtIndex(size_t index)
		{
			if (index < _size) return _arr[(_start + index) % _arr.size()];
			return 0.0;
		}
		void writeAtIndex(size_t index, float value)
		{
			if (index >= _size) return;
			_arr[(_start + index) % _arr.size()] = value;
		}
		void remove(size_t number_to_remove)
		{
			if (number_to_remove < _size)
			{
				_size -= number_to_remove;
				_start = (_start + number_to_remove) % _arr.size();
			}
		}
		void resize(int size)
		{ 
			if (size <= _arr.size()) return;
			std::vector<float> temp;
			for (int i = 0; i < _size; i++) temp.push_back(readAtIndex(i));
			_end = 0;
			_start = 0;
			for (int i = 0; i < temp.size(); i++) push(temp[i]);
			_arr.resize(size);
		}
	private:
		JUCE_LEAK_DETECTOR(PitchShifter::Queue)
	} _in, _out;
private:
	JUCE_LEAK_DETECTOR(PitchShifter)
};