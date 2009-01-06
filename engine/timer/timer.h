#pragma once

namespace timer
{
	void init(void);
	void release(void);
	void doTick();

	extern float time_ms;
	extern float game_ms;
	extern float delta_ms;
	extern double delta_s;

	extern float time_scale;

	inline unsigned __int64 RDTSC(void)
	{
		_asm _emit 0x0F
		_asm _emit 0x31
	}

	class Stopwatch
	{
	protected:
		unsigned __int64 start_count_;
		unsigned __int64 elapsed_count_;
		bool running_;

		static double ticks_per_ms_;
		static __int64 timer_frequency_;

	public:
		Stopwatch()
			: start_count_(0), elapsed_count_(0), running_(false)
		{
			if (!timer_frequency_)
			{
				QueryPerformanceFrequency((LARGE_INTEGER*)&timer_frequency_);
				assert(timer_frequency_);
				ticks_per_ms_ = (double)timer_frequency_ / 1000.0;
			}
		}

		~Stopwatch()
		{
		}

		inline void Start()
		{
			if (!running_)
			{
				QueryPerformanceCounter((LARGE_INTEGER*)&start_count_);
				running_ = true;
			}
		}

		inline void Stop()
		{
			if (running_)
			{
				unsigned __int64 current_count;
				QueryPerformanceCounter((LARGE_INTEGER*)&current_count);
				elapsed_count_ += current_count - start_count_;
				running_ = false;
			}
		}

		inline void Reset(double elapsed = 0.0)
		{
			ResetCount((unsigned __int64)(elapsed * ticks_per_ms_));
		}

		inline void ResetCount(unsigned __int64 elapsed_count = 0)
		{
			if (running_)
				QueryPerformanceCounter((LARGE_INTEGER*)&start_count_);

			elapsed_count_ = elapsed_count;
		}

		inline double Elapsed() const
		{
			unsigned __int64 elapsed_count = ElapsedCount();
			if (elapsed_count)
				return (double)elapsed_count / ticks_per_ms_;
			else
				return 0.0;
		}

		inline unsigned __int64 ElapsedCount() const
		{
			unsigned __int64 current_count = 0;

			if (running_)
				QueryPerformanceCounter((LARGE_INTEGER*)&current_count);

			return elapsed_count_ + (current_count - start_count_);
		}

		inline double TicksPerMs() const
		{
			return ticks_per_ms_;
		}
	};

	class DeltaTimer
	{
	protected:
		unsigned __int64 last_elapsed_count_;
		double current_tick_time_ms_;
		double tick_delta_ms_;
		Stopwatch timer_;

	public:
		DeltaTimer()
		{
			timer_.Start();
			last_elapsed_count_ = timer_.ElapsedCount();
		}

		~DeltaTimer()
		{
		}

		void Reset()
		{
			timer_.Reset();
			last_elapsed_count_ = timer_.ElapsedCount();
		}

		void Tick()
		{
			unsigned __int64 current_count = timer_.ElapsedCount();
			current_tick_time_ms_ = (double)current_count / timer_.TicksPerMs();
			tick_delta_ms_ = (double)(current_count - last_elapsed_count_) / timer_.TicksPerMs();
			last_elapsed_count_ = current_count;
		}

		inline double CurrentTime() const
		{
			return current_tick_time_ms_;
		}

		inline double DeltaTime() const
		{
			return tick_delta_ms_;
		}

		inline double Elapsed() const
		{
			return timer_.Elapsed();
		}
	};

	class AutoTimer
	{
	protected:
		char* message_;
		Stopwatch timer_;

	public:
		AutoTimer()
			: message_(NULL), timer_()
		{
			timer_.Start();
		}

		explicit AutoTimer(char* message)
			: message_(message), timer_()
		{
			timer_.Start();
		}

		~AutoTimer()
		{
			if (message_)
				LogInterval(message_);
		}

		inline double Elapsed()
		{
			return timer_.Elapsed();
		}

		inline void LogInterval(const char* message) const
		{
			INFO((format("%1% : %2% ms") % message % timer_.Elapsed()).str().c_str());
		}

		inline void Pause()
		{
			timer_.Stop();
		}

		inline void Resume()
		{
			timer_.Start();
		}

	};

	class AutoRDTSCTimer
	{
	protected:
		const char* message_;
		unsigned __int64 start_cycle_;
		unsigned __int64 pause_start_;
		unsigned __int64 pause_time_;

	public:
		AutoRDTSCTimer()
			: message_(NULL), start_cycle_(RDTSC()), pause_time_(0)
		{
		}

		~AutoRDTSCTimer()
		{
			if (message_)
				LogInterval(message_);
		}

		explicit AutoRDTSCTimer(const char* message) :
		message_(message), start_cycle_(RDTSC()), pause_time_(0)
		{
		}

		inline void LogInterval(const char* message) const
		{
			INFO((format("%1% : %2% ticks") % message % (unsigned __int64)((RDTSC() - start_cycle_) - pause_time_)).str().c_str());
		}

		inline void Pause()
		{
			pause_start_ = RDTSC();
		}

		inline void Resume()
		{
			pause_time_ += RDTSC() - pause_start_;
		}
	};
}