#pragma once
#include "Input.h"
#include "ActionEvent.h"
#include "InputObserver.h"
#include "GameInput.h"

#include <unordered_map>

namespace CommonUtilities
{
	class InputMapper
	{
	public:
		InputMapper();
		~InputMapper();
		void Refresh();
		static InputMapper& GetInputMapper();

		void PostEvent(const ActionEvent& anEvent);
		void Subscribe(const ActionEventID& anEventType, InputObserver* anObserver);
		void UnRegister(const ActionEventID& anEventType, InputObserver* anObserver);
		void UnRegisterFromAll(InputObserver* anObserver);

		void BindEvent(const GameInput& anInput, const ActionEventID& anAction);
		void BindKey(const Keys& anInput, const ActionEventID& anAction);
	private:
		void TranslateToEvent(const GameInput& anInput, const float& aWeight);
		void TranslateToEvent(const Keys& anInput, const float& aWeight);

	private:
		std::unordered_map<GameInput, ActionEventID> myMappedInputs;
		std::unordered_map<Keys, ActionEventID> myPopperMappedInputs;
		typedef std::vector<InputObserver*> ObserversList;

		ObserversList myObservers[static_cast<int>(ActionEventID::Count)];
	};

}
namespace CU = CommonUtilities;

