#pragma once

#include <fstream>


#define PST_PROFILE 0


#if PST_PROFILE
	#define PST_PROFILE_BEGIN_SESSION(name, filepath)  ::Pistachio::Instrumentor::Instance().BeginSession(name, filepath)
	#define PST_PROFILE_END_SESSION()                  ::Pistachio::Instrumentor::Instance().EndSession()
	#define PST_PROFILE_SCOPE(name)                    ::Pistachio::InstrumentationTimer timer##__LINE__(name)
	#define PST_PROFILE_FUNCTION()                     PST_PROFILE_SCOPE(__FUNCSIG__)
#else
	#define PST_PROFILE_BEGIN_SESSION(name, filepath)
	#define PST_PROFILE_END_SESSION()
	#define PST_PROFILE_SCOPE(name)
	#define PST_PROFILE_FUNCTION()
#endif


namespace Pistachio {

	struct ProfileResult {
		const char* Name;
		long long int Start, End;
	};

	struct InstrumentationSession {
		std::string Name;
	};


	class Instrumentor {
	public:
		Instrumentor()
			: m_CurrentSession(nullptr), m_ProfileCount(0) {
		}

		void BeginSession(const std::string& name, const std::string& filepath = "results.json") {
			m_OutputStream.open(filepath);
			WriteHeader();
			m_CurrentSession = new InstrumentationSession{ name };
		}

		void EndSession() {
			WriteFooter();
			m_OutputStream.close();

			delete m_CurrentSession;
			m_CurrentSession = nullptr;

			m_ProfileCount = 0;
		}

		void WriteProfile(const ProfileResult& result) {
			if (m_ProfileCount++ > 0) {
				m_OutputStream << ",";
			}

			std::string name = result.Name;
			std::replace(name.begin(), name.end(), '"', '\'');

			m_OutputStream << "{";
			m_OutputStream <<     "\"cat\":\"function\",";
			m_OutputStream <<     "\"dur\":" << (result.End - result.Start) << ",";
			m_OutputStream <<     "\"name\":\"" << name << "\",";
			m_OutputStream <<     "\"ph\":\"X\",";
			m_OutputStream <<     "\"pid\":0,";
			m_OutputStream <<     "\"tid\":0,";
			m_OutputStream <<     "\"ts\":" << result.Start;
			m_OutputStream << "}";

			m_OutputStream.flush();
		}

		void WriteHeader() {
			m_OutputStream << "{\"otherData\": {}, \"traceEvents\": [";
			m_OutputStream.flush();
		}

		void WriteFooter() {
			m_OutputStream << "]}";
			m_OutputStream.flush();
		}

		bool IsProfiling() const { return m_CurrentSession; }

		static Instrumentor& Instance() {
			static Instrumentor* instance = new Instrumentor();
			return *instance;
		}

	private:
		InstrumentationSession* m_CurrentSession;
		std::ofstream m_OutputStream;
		unsigned int m_ProfileCount;
	};


	class InstrumentationTimer {
	public:
		InstrumentationTimer(const char* name)
			: m_Name(name), m_Stopped(false) {
			if (!Instrumentor::Instance().IsProfiling()) return;

			m_StartTimepoint = std::chrono::high_resolution_clock::now();
		}

		~InstrumentationTimer() {
			if (!Instrumentor::Instance().IsProfiling()) return;

			if (!m_Stopped) {
				Stop();
			}
		}

		void Stop() {
			auto endTimepoint = std::chrono::high_resolution_clock::now();

			long long int start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
			long long int end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

			m_Stopped = true;

			Instrumentor::Instance().WriteProfile({ m_Name, start, end });
		}

	private:
		const char* m_Name;
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
		bool m_Stopped;
	};

}
