#include "LiveSampler.h"

class Queue
{
	std::vector<float> _arr;
	size_t _start, _end, _size;
public:
	Queue(int array_size, int end = 0) : _arr(array_size), _start(0), _end(0), _size(0)
	{
		if (!array_size) return;
		if (end >= array_size) _end = array_size - 1;
		_end %= array_size;
		_size = _end;
	}
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
	float readAtIndex(int index)
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
	void resize(int array_size, int end = 0)
	{
		if (array_size <= _arr.size()) return;
		std::vector<float> temp;
		for (int i = 0; i < _size; i++) temp.push_back(readAtIndex(i));
		_end = 0;
		_start = 0;
		for (int i = 0; i < temp.size(); i++) push(temp[i]);
		_arr.resize(array_size);

		if (_end >= array_size) _end = array_size - 1;
		_end %= array_size;
		_size = _end;
	}
private:
	JUCE_LEAK_DETECTOR(Queue)
};

class PitchShifter
{
	std::shared_ptr<dsp::FFT> _fft;
	uptr<dsp::WindowingFunction<float>> _window;
	int _overlap;
	float _pitch;
	float _sample_rate;
	float _mix;

	class Voices
	{
		std::vector<float> _freqs;
		std::vector<std::vector<float>> _phases;
		PitchShifter& _parent;
		int _pos;
	public:
		Voices(PitchShifter& parent) : _parent(parent), _pos(0) {}

		int getSize() { return _freqs.size(); }

		void setSize(int new_size)
		{
			int old_size = _freqs.size();
			int fft_size = _parent._fft->getSize();

			_freqs.resize(new_size);
			_phases.resize(new_size);
			if (new_size > old_size)
				for (int i = old_size; i < new_size; i++)
					_phases[i].resize(fft_size);
		}

		void createVoice(int midi_note)
		{
			float frequency = MidiMessage::getMidiNoteInHertz(midi_note);
			_freqs[_pos++] = frequency;
			_pos %= _freqs.size();
		}

		void deleteVoice(int midi_note)
		{
			float frequency = MidiMessage::getMidiNoteInHertz(midi_note);
			int move_back = 0;
			for (int i = 0; i < _freqs.size(); i++)
			{
				if (_freqs[i] == frequency) {
					move_back++;
					_freqs[i] = 0;
				}
				else if (move_back) {
					_freqs[i - move_back] = _freqs[i];
					_freqs[i] = 0;
				}
			}
		}
		
		const float& getVoice(int index)
		{
			return _freqs[index];
		}

		std::vector<float>& getPhase(int index)
		{
			return _phases[index];
		}
	} _voices;

	Queue _in, _out;


public:
	PitchShifter();
	void prepare(float sample_rate, std::shared_ptr<dsp::FFT> fft, int overlap);
	void process(float*, int num_samples);
	//void setShift(float shift) { _shift_factor = shift; }
	void createVoice(int midi_note) { _voices.createVoice(midi_note); }
	void deleteVoice(int midi_note) { _voices.deleteVoice(midi_note); }
	void setNumShifters(int new_num) { _voices.setSize(new_num); }
	void setMix(float mix) { _mix = mix; }

private:
	float detectPitch(std::vector<float> acfs);
	void processFrame();
	void shiftPitch(std::vector<dsp::Complex<float>>& frame);

private:
	JUCE_LEAK_DETECTOR(PitchShifter)
};
