#pragma once

#include "../auxiliary/traits/subject/subject.h"

namespace manager
{
	class EventManager : public trait::Observer
	{
	public:
		class DebugFlagSubject : public trait::Subject
		{
		public:
			static DebugFlagSubject* GetInstance();
			static void DeconsInstance();

			void SetDebugFlag(const bool flagState);
			const bool GetDebugFlag();

		private:
			DebugFlagSubject();
			~DebugFlagSubject();

		private:
			bool debugFlag;

			static DebugFlagSubject* instance;
		};
		class InputSubject : public trait::Subject
		{
		public:
			static InputSubject* GetInstance();
			static void DeconsInstance();

			void SetEvent(const sf::Event& _newEvent);
			const sf::Event& GetEvent();

		private:
			InputSubject();
			~InputSubject();

		private:
			sf::Event _event;

			static InputSubject* instance;
		};

	public:
		static EventManager* GetInstance();
		static void DeconsInstance();

		void UpdateObs(const trait::Subject* alteredSub);

		void Update();

	private:
		EventManager();
		~EventManager();

	private:
		static EventManager* instance;

		DebugFlagSubject* pDebugFlagSub;
		InputSubject* pInputSubject;
	};
}